#ifndef GENERATORS_H
#define GENERATORS_H

/**
 * @file generators.h
 * @author Ani Kristo (anikristo@gmail.com)
 * @brief Synthetic data generator functions
 *
 * @copyright Copyright (c) 2020 Ani Kristo (anikristo@gmail.com)
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3 of the License.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <random>
#include <vector>

using namespace std;

template <class T>
vector<T> exponential_distr(size_t size, double lambda = 2, double scale = 0) {
  // Adjust the default scale parameter w.r.t. the numerical type
  if (!(is_same<float, T>() || is_same<double, T>()) && scale <= 0)
    scale = size;
  else if (scale <= 0)
    scale = 1;

  // Initialize random engine with normal distribution
  random_device rd;
  mt19937 generator(rd());
  exponential_distribution<> distribution(lambda);

  // Populate the input
  vector<T> arr;
  for (size_t i = 0; i < size; i++) {
    arr.push_back(distribution(generator) * scale);
  }

  return arr;
}

template <class T>
vector<T> lognormal_distr(size_t size, double mean = 0, double stddev = 0.5,
                          double scale = 0) {
  // Adjust the default scale parameter w.r.t. the numerical type
  if (!(is_same<float, T>() || is_same<double, T>()) && scale <= 0)
    scale = size;
  else if (scale <= 0)
    scale = 1;

  // Initialize random engine with normal distribution
  random_device rd;
  mt19937 generator(rd());
  lognormal_distribution<> distribution(mean, stddev);

  // Populate the input
  vector<T> arr;
  for (size_t i = 0; i < size; i++) {
    arr.push_back(distribution(generator) * scale);
  }

  return arr;
}

template <class T>
vector<T> normal_distr(size_t size, double mean = 1 << 12,
                       double stddev = 1 << 10) {
  // Initialize random engine with normal distribution
  random_device rd;
  mt19937 generator(rd());
  normal_distribution<> distribution(mean, stddev);

  // Populate the input
  vector<T> arr;
  for (size_t i = 0; i < size; i++) {
    arr.push_back(distribution(generator));
  }

  return arr;
}

template <class T>
vector<T> uniform_distr(size_t size, double a = 0, double b = -1) {
  // Adjust the default parameters
  if (a == 0 && b == -1) b = size;

  // Initialize random engine with normal distribution
  random_device rd;
  mt19937 generator(rd());
  uniform_real_distribution<> distribution(a, b);

  // Populate the input
  vector<T> arr;
  for (size_t i = 0; i < size; i++) {
    arr.push_back(distribution(generator));
  }

  return arr;
}

template <class T>
vector<T> mix_of_gauss_distr(size_t size, size_t num_gauss = 5) {
  // Generate the means
  vector means = uniform_distr<double>(num_gauss, -500, 500);

  // Generate the stdevs
  vector stdevs = uniform_distr<double>(num_gauss, 0, 100);

  // Generate the weights
  vector weights = uniform_distr<double>(num_gauss, 0, 1);

  // Normalize the weights
  double sum_of_weights = std::accumulate(weights.begin(), weights.end(), 0.);
  std::for_each(weights.begin(), weights.end(),
                [&](auto x) { x /= sum_of_weights; });

  // Initialize random number generator
  random_device rd;
  mt19937 generator(rd());

  // Initialize random distribution selector
  discrete_distribution<int> index_selector(weights.begin(), weights.end());

  // Start generating random numbers from normal distributions
  vector<T> arr;
  for (size_t i = 0; i < size; ++i) {
    auto random_idx = index_selector(generator);
    normal_distribution<> distribution(means[random_idx], stdevs[random_idx]);

    auto val = distribution(generator);
    arr.push_back(val);
  }

  return arr;
}

template <class T>
vector<T> chi_squared_distr(size_t size, double k = 4, double scale = 0) {
  // Adjust the default scale parameter w.r.t. the numerical type
  if (!(is_same<float, T>() || is_same<double, T>()) && scale <= 0)
    scale = size;
  else if (scale <= 0)
    scale = 1;

  // Initialize random number generator
  random_device rd;
  mt19937 generator(rd());
  chi_squared_distribution<> distribution(k);

  // Populate the input
  vector<T> arr;
  for (size_t i = 0; i < size; ++i) {
    arr.push_back(distribution(generator) * scale);
  }

  return arr;
}

// Adapted from
// https://stackoverflow.com/questions/9983239/how-to-generate-zipf-distributed-numbers-efficiently
template <class T>
vector<T> zipf_distr(size_t size, double skew = 0.5, size_t cardinality = 1e8) {
  // Allocate space
  vector<T> arr;

  // Start generating numbers
  for (size_t elm_idx = 0; elm_idx < size; ++elm_idx) {
    static bool first = true;  // Static first time flag
    static double c = 0;       // Normalization constant
    static double *sum_probs;  // Pre-calculated sum of probabilities

    // Compute normalization constant on first call only
    if (first) {
      for (size_t i = 1; i <= cardinality; ++i)
        c = c + (1.0 / pow((double)i, skew));
      c = 1.0 / c;

      sum_probs = (double *)malloc((cardinality + 1) * sizeof(*sum_probs));
      sum_probs[0] = 0;
      for (size_t i = 1; i <= cardinality; ++i) {
        sum_probs[i] = sum_probs[i - 1] + c / pow((double)i, skew);
      }
      first = false;
    }

    // Pull a uniform random number (0 < z < 1)
    double z;
    do {
      z = 1. * rand() / RAND_MAX;
    } while ((z == 0) || (z == 1));

    // Map z to the value
    size_t low = 1, high = cardinality, mid;
    T zipf_value;  // Computed exponential value to be returned
    do {
      mid = floor((low + high) / 2);

      if (sum_probs[mid] >= z && sum_probs[mid - 1] < z) {
        zipf_value = mid;
        break;
      } else if (sum_probs[mid] >= z) {
        high = mid - 1;
      } else {
        low = mid + 1;
      }
    } while (low <= high);

    arr.push_back(zipf_value);
  }
  return arr;
}

template <class T>
vector<T> root_dups_distr(size_t size) {
  // Populate the input
  vector<T> arr;
  const size_t root = std::sqrt(size);
  for (size_t i = 0; i < size; i++) {
    arr.push_back(i % root);
  }

  return arr;
}

template <class T>
vector<T> sorted_uniform_distr(size_t size) {
  // Populate the input
  vector<T> arr = uniform_distr<T>(size);

  // Sort the input
  std::sort(arr.begin(), arr.end());

  return arr;
}

template <class T>
vector<T> reverse_sorted_uniform_distr(size_t size) {
  // Populate the input
  vector<T> arr = sorted_uniform_distr<T>(size);

  // Reverse the input
  std::reverse(arr.begin(), arr.end());
  return arr;
}

template <class T>
vector<T> identical_distr(size_t size, T value = 0) {
  return vector<T>(size, value);
}

#endif  // GENERATORS_H