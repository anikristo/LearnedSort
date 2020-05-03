#ifndef LEARNED_SORT_H
#define LEARNED_SORT_H

/**
 * @file learned_sort.h
 * @author Ani Kristo (anikristo@gmail.com)
 * @brief The purpose of this file is to provide an implementation of Learned Sort, a model-enhanced sorting algorithm. 
 * @version 0.1
 * @date 2020-05-02
 * 
 * @copyright Copyright (c) 2020 Ani Kristo <anikristo@gmail.com>
 * @copyright Copyright (C) 2020 Kapil Vaidya <kapilv@mit.edu>
 * 
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

#define LEARNED_SORT_VERSION_MAJOR 0
#define LEARNED_SORT_VERSION_MINOR 1

#include <algorithm>
#include <cmath>
#include <iterator>
#include <iostream>
#include <vector>

using std::iterator_traits;
using std::vector;

using std::cerr;
using std::endl;

namespace learned_sort
{

// A training point struct, which packs the key and its respective scaled CDF
// value
template <typename T>
struct training_point
{

  // Key value
  T x;

  // The scaled CDF (a.k.a the position in the sorted sample, divided by the
  // sample size)
  double y;
};

class RMI
{
public:
  // Individual linear models
  struct linear_model
  {
    double slope = 0;
    double intercept = 0;
  };

  // CDF model hyperparameters
  struct Params
  {

    // Member fields
    unsigned int batch_sz;
    unsigned int fanout;
    float overallocation_ratio;
    float sampling_rate;
    unsigned int threshold;
    vector<unsigned int> arch;

    // Default hyperparameters
    static constexpr unsigned int DEFAULT_BATCH_SZ = 10;
    static constexpr unsigned int DEFAULT_FANOUT = 1e3;
    static constexpr float DEFAULT_OVERALLOCATION_RATIO = 1.1;
    static constexpr float DEFAULT_SAMPLING_RATE = .01;
    static constexpr unsigned int DEFAULT_THRESHOLD = 100;
    vector<unsigned int> DEFAULT_ARCH = {1, 1000};
    static constexpr unsigned int MIN_SORTING_SIZE = 1e4;

    // Default constructor
    Params();

    // Constructor with custom hyperparameter values
    Params(float sampling_rate, float overallocation, unsigned int fanout,
           unsigned int batch_size, unsigned int threshold, vector<unsigned int> model_arch);
  };

  // Member variables of the CDF model
  bool trained;
  vector<vector<linear_model>> models;
  double *training_sample = nullptr;
  unsigned int training_sample_sz = 0;
  Params hp;

  // CDF model constructor
  explicit RMI(Params p);
};

// Training function
template <class RandomIt>
RMI train(RandomIt, RandomIt, RMI::Params &);

// Default comparison function [std::less()] and default hyperparameters
// Drop-in replacement for std::sort()
template <class RandomIt>
void sort(RandomIt, RandomIt);

// Default comparison function [std::less()] and custom hyperparameters
template <class RandomIt>
void sort(RandomIt, RandomIt, RMI::Params &);
} // namespace learned_sort

using namespace learned_sort;

learned_sort::RMI::Params::Params()
{
  this->batch_sz = DEFAULT_BATCH_SZ;
  this->fanout = DEFAULT_FANOUT;
  this->overallocation_ratio = DEFAULT_OVERALLOCATION_RATIO;
  this->sampling_rate = DEFAULT_SAMPLING_RATE;
  this->threshold = DEFAULT_THRESHOLD;
  this->arch = DEFAULT_ARCH;
}

learned_sort::RMI::Params::Params(float sampling_rate, float overallocation,
                                  unsigned int fanout, unsigned int batch_sz,
                                  unsigned int threshold, vector<unsigned int> arch)
{
  this->batch_sz = batch_sz;
  this->fanout = fanout;
  this->overallocation_ratio = overallocation;
  this->sampling_rate = sampling_rate;
  this->threshold = threshold;
  this->arch = std::move(arch);
}

learned_sort::RMI::RMI(Params p)
{
  this->trained = false;
  this->hp = p;
  this->models.resize(p.arch.size());

  for (size_t layer_idx = 0; layer_idx < p.arch.size(); ++layer_idx)
  {
    this->models[layer_idx].resize(p.arch[layer_idx]);
  }
}

using namespace learned_sort;

/**
 * @brief Train a CDF function with an RMI architecture, using linear spline
 * interpolation.
 *
 * @tparam RandomIt A bi-directional random iterator over the dataset
 * @tparam Compare  A comparison function to learn. If std::less, the predicted
 * order of keys will be the normal sorted order. Otherwise, if std::greater is
 * passed, the keys will be predicted in reverse sorting order.
 * @param begin Random-access iterators to the initial position of the sequence
 * to be used for sorting. The range used is [begin,end), which contains all the
 * elements between first and last, including the element pointed by first but
 * not the element pointed by last.
 * @param end Random-access iterators to the last position of the sequence to be
 * used for sorting. The range used is [begin,end), which contains all the
 * elements between first and last, including the element pointed by first but
 * not the element pointed by last.
 * @param comp Binary function that accepts two elements in the range as
 * arguments, and returns a value convertible to bool. The value returned
 * indicates whether the element passed as first argument is considered to go
 * before the second in the specific strict weak ordering it defines. The
 * function shall not modify any of its arguments. This can either be a function
 * pointer or a function object.
 * @param p The hyperparameters for the CDF model, which describe the
 * architecture and sampling ratio.
 * @return learned_sort::RMI The trained model which, given a key, will output a
 * value between [0,1] as a predicted CDF value.
 */
template <class RandomIt>
learned_sort::RMI learned_sort::train(RandomIt begin, RandomIt end, RMI::Params &p)
{

  // Determine the data type
  typedef typename iterator_traits<RandomIt>::value_type T;

  // Determine input size
  const unsigned int INPUT_SZ = std::distance(begin, end);

  // Validate parameters
  if (p.batch_sz >= INPUT_SZ)
  {
    p.batch_sz = RMI::Params::DEFAULT_BATCH_SZ;
    cerr << "\33[93;1mWARNING\33[0m: Invalid batch size. Using default (" << RMI::Params::DEFAULT_BATCH_SZ << ")." << endl;
  }

  if (p.fanout >= INPUT_SZ)
  {
    p.fanout = RMI::Params::DEFAULT_FANOUT;
    cerr << "\33[93;1mWARNING\33[0m: Invalid fanout. Using default (" << RMI::Params::DEFAULT_FANOUT << ")." << endl;
  }

  if (p.overallocation_ratio <= 0)
  {
    p.overallocation_ratio = 1;
    cerr << "\33[93;1mWARNING\33[0m: Invalid overallocation ratio. Using default (" << RMI::Params::DEFAULT_OVERALLOCATION_RATIO << ")." << endl;
  }

  if (p.sampling_rate <= 0 or p.sampling_rate > 1)
  {
    p.sampling_rate = RMI::Params::DEFAULT_SAMPLING_RATE;
    cerr << "\33[93;1mWARNING\33[0m: Invalid sampling rate. Using default (" << RMI::Params::DEFAULT_SAMPLING_RATE << ")." << endl;
  }

  if (p.threshold <= 0 or p.threshold >= INPUT_SZ or p.threshold >= INPUT_SZ / p.fanout)
  {
    p.threshold = RMI::Params::DEFAULT_THRESHOLD;
    cerr << "\33[93;1mWARNING\33[0m: Invalid threshold. Using default (" << RMI::Params::DEFAULT_THRESHOLD << ")." << endl;
  }

  if (p.arch.size() > 2 or p.arch[0] != 1 or p.arch[1] <= 0)
  {
    p.arch = p.DEFAULT_ARCH;
    cerr << "\33[93;1mWARNING\33[0m: Invalid architecture. Using default {" << p.DEFAULT_ARCH[0] << ", " << p.DEFAULT_ARCH[1] << "}." << endl;
  }

  // Initialize the CDF model
  RMI rmi(p);
  static const unsigned int NUM_LAYERS = p.arch.size();
  vector<vector<vector<training_point<T>>>> training_data(NUM_LAYERS);
  for (unsigned int layer_idx = 0; layer_idx < NUM_LAYERS; ++layer_idx)
  {
    training_data[layer_idx].resize(p.arch[layer_idx]);
  }

  //----------------------------------------------------------//
  //                           SAMPLE                         //
  //----------------------------------------------------------//

  // Determine sample size
  const unsigned int SAMPLE_SZ = std::min<unsigned int>(
      INPUT_SZ, std::max<unsigned int>(p.sampling_rate * INPUT_SZ,
                                       RMI::Params::MIN_SORTING_SIZE));

  // Create a sample array
  rmi.training_sample = new double[SAMPLE_SZ];

  // Start sampling
  unsigned int offset = static_cast<unsigned int>(1. * INPUT_SZ / SAMPLE_SZ);
  for (auto i = begin; i < end; i += offset)
  {
    // NOTE:  We don't directly assign SAMPLE_SZ to rmi.training_sample_sz
    //        to avoid issues with divisibility
    rmi.training_sample[rmi.training_sample_sz++] = static_cast<double>(*i);
  }

  // Sort the sample using the provided comparison function
  std::sort(rmi.training_sample, rmi.training_sample + rmi.training_sample_sz);

  // Stop early if the array is identical
  if (rmi.training_sample[0] ==
      rmi.training_sample[rmi.training_sample_sz - 1])
  {
    return rmi;
  }

  //----------------------------------------------------------//
  //                     TRAIN THE MODELS                     //
  //----------------------------------------------------------//

  // Populate the training data for the root model
  for (unsigned int i = 0; i < SAMPLE_SZ; ++i)
  {
    training_point<T> tp;
    tp.x = rmi.training_sample[i];
    tp.y = static_cast<float>(i) / SAMPLE_SZ;

    training_data[0][0].push_back(tp);
  }

  // Train the root model using linear interpolation
  auto *current_training_data = &training_data[0][0];
  RMI::linear_model *current_model = &rmi.models[0][0];

  // Find the min and max values in the training set
  training_point<T> min = current_training_data->front();
  training_point<T> max = current_training_data->back();

  // Calculate the slope and intercept terms
  current_model->slope =
      1 / (max.x - min.x); // Assuming min.y = 0 and max.y = 1
  current_model->intercept = -current_model->slope * min.x;

  // Extrapolate for the number of models in the next layer
  current_model->slope *= p.arch[1] - 1;
  current_model->intercept *= p.arch[1] - 1;

  // Populate the training data for the next layer
  for (const auto &d : *current_training_data)
  {

    // Predict the model index in next layer
    unsigned int rank = current_model->slope * d.x + current_model->intercept;

    // Normalize the rank between 0 and the number of models in the next layer
    rank =
        std::max(static_cast<unsigned int>(0), std::min(p.arch[1] - 1, rank));

    // Place the data in the predicted training bucket
    training_data[1][rank].push_back(d);
  }

  // Train the leaf models
  for (unsigned int model_idx = 0; model_idx < p.arch[1]; ++model_idx)
  {

    // Update iterator variables
    current_training_data = &training_data[1][model_idx];
    current_model = &rmi.models[1][model_idx];

    // Interpolate the min points in the training buckets
    if (model_idx ==
        0)
    { // The current model is the first model in the current layer

      if (current_training_data->size() <
          2)
      { // Case 1: The first model in this layer is empty
        current_model->slope = 0;
        current_model->intercept = 0;

        // Insert a fictive training point to avoid propagating more than one
        // empty initial models.
        training_point<T> tp;
        tp.x = 0;
        tp.y = 0;
        current_training_data->push_back(tp);
      }
      else
      { // Case 2: The first model in this layer is not empty

        min = current_training_data->front();
        max = current_training_data->back();

        current_model->slope =
            (max.y) / (max.x - min.x); // Hallucinating as if min.y = 0
        current_model->intercept = min.y - current_model->slope * min.x;
      }
    }
    else if (model_idx == p.arch[1] - 1)
    {

      if (current_training_data
              ->empty())
      { // Case 3: The final model in this layer is empty

        current_model->slope = 0;
        current_model->intercept = 1;
      }
      else
      { // Case 4: The last model in this layer is not empty

        min = training_data[1][model_idx - 1].back();
        max = current_training_data->back();

        current_model->slope =
            (min.y - 1) / (min.x - max.x); // Hallucinating as if max.y = 1
        current_model->intercept = min.y - current_model->slope * min.x;
      }
    }
    else
    { // The current model is not the first model in the current layer

      if (current_training_data->empty())
      { // Case 5: The intermediate model in
        // this layer is empty
        current_model->slope = 0;
        current_model->intercept =
            training_data[1][model_idx - 1].back().y; // If the previous model
                                                      // was empty too, it will
                                                      // use the fictive
                                                      // training points

        // Insert a fictive training point to avoid propagating more than one
        // empty initial models.
        // NOTE: This will _NOT_ throw to DIV/0 due to identical x's and y's
        // because it is working backwards.
        training_point<T> tp;
        tp.x = training_data[1][model_idx - 1].back().x;
        tp.y = training_data[1][model_idx - 1].back().y;
        current_training_data->push_back(tp);
      }
      else
      { // Case 6: The intermediate leaf model is not empty

        min = training_data[1][model_idx - 1].back();
        max = current_training_data->back();

        current_model->slope = (min.y - max.y) / (min.x - max.x);
        current_model->intercept = min.y - current_model->slope * min.x;
      }
    }
  }

  // NOTE:
  // The last stage (layer) of this model contains weights that predict the CDF
  // of the keys (i.e. Range is [0-1])
  // When using this model to predict the position of the keys in the sorted
  // order, you MUST scale the
  // weights of the last layer to whatever range you are predicting for. The
  // inner layers of the model have
  // already been extrapolated to the length of the stage.git
  //
  // This is a design choice to help with the portability of the model.
  //
  rmi.trained = true;

  return rmi;
} // end of training function

template <class RandomIt>
void _sort_trained(RandomIt begin, RandomIt end, learned_sort::RMI &rmi)
{
  // Determine the data type
  typedef typename iterator_traits<RandomIt>::value_type T;

  // Cache runtime parameters
  static const unsigned int BATCH_SZ = rmi.hp.batch_sz;
  static const double OA_RATIO = rmi.hp.overallocation_ratio;
  static const unsigned int FANOUT = rmi.hp.fanout;
  static const unsigned int THRESHOLD = rmi.hp.threshold;

  // Determine the input size
  const unsigned int INPUT_SZ = std::distance(begin, end);

  //----------------------------------------------------------//
  //                          INIT                            //
  //----------------------------------------------------------//

  // Constants for buckets
  const unsigned int MAJOR_BCKT_CAPACITY = INPUT_SZ / FANOUT;

  // Constants for repeated keys
  const unsigned int EXCEPTION_VEC_INIT_CAPACITY = FANOUT;
  constexpr unsigned int EXC_CNT_THRESHOLD = 60;
  const size_t TRAINING_SAMPLE_SZ = rmi.training_sample_sz;

  // Initialize the spill bucket
  vector<T> spill_bucket;

  // Initialize the buckets
  vector<T> major_bckts(INPUT_SZ + 1);

  // Array to keep track of the major bucket sizes
  vector<unsigned int> major_bckt_sizes(FANOUT, 0);

  // Initialize the exception lists for handling repeated keys
  vector<T> repeated_keys; // Stores the heavily repeated key values
  vector<vector<T>> repeated_keys_predicted_ranks(
      EXCEPTION_VEC_INIT_CAPACITY); // Stores the predicted ranks of each
                                    // repeated
                                    // key
  vector<vector<T>> repeated_key_counts(
      EXCEPTION_VEC_INIT_CAPACITY); // Stores the count of repeated keys
  unsigned int total_repeated_keys = 0;

  // Counts the nubmer of total elements that are in the buckets, hence
  // INPUT_SZ - spill_bucket.size() at the end of the recursive bucketization
  unsigned int num_tot_elms_in_bckts = 0;

  // Cache the model parameters
  auto root_slope = rmi.models[0][0].slope;
  auto root_intrcpt = rmi.models[0][0].intercept;
  unsigned int num_models = rmi.hp.arch[1];
  vector<double> slopes, intercpts;
  for (unsigned int i = 0; i < num_models; ++i)
  {
    slopes.push_back(rmi.models[1][i].slope);
    intercpts.push_back(rmi.models[1][i].intercept);
  }

  //----------------------------------------------------------//
  //       DETECT REPEATED KEYS IN THE TRAINING SAMPLE        //
  //----------------------------------------------------------//

  // Count the occurrences of equal keys
  unsigned int cnt_rep_keys = 1;
  for (size_t i = 1; i < TRAINING_SAMPLE_SZ; i++)
  {
    if (rmi.training_sample[i] == rmi.training_sample[i - 1])
    {
      ++cnt_rep_keys;
    }
    else
    { // New values start here. Reset counter. Add value in the
      // exeception_vals if above threshold
      if (cnt_rep_keys > EXC_CNT_THRESHOLD)
      {
        repeated_keys.push_back(rmi.training_sample[i - 1]);
      }
      cnt_rep_keys = 1;
    }
  }

  if (cnt_rep_keys > EXC_CNT_THRESHOLD)
  { // Last batch of repeated keys
    repeated_keys.push_back(rmi.training_sample[TRAINING_SAMPLE_SZ - 1]);
  }

  delete[] rmi.training_sample;

  //----------------------------------------------------------//
  //             SHUFFLE THE KEYS INTO BUCKETS                //
  //----------------------------------------------------------//

  // For each spike value, predict the bucket.
  // In repeated_keys_predicted_ranks[predicted_bucket_idx] save the value, in
  // repeated_keys_predicted_ranks[predicted_bucket_idx] save the counts
  int pred_model_idx = 0;
  double pred_cdf = 0.;
  for (size_t i = 0; i < repeated_keys.size(); ++i)
  {
    pred_model_idx = static_cast<int>(
        std::max(0., std::min(num_models - 1.,
                              root_slope * repeated_keys[i] + root_intrcpt)));
    pred_cdf =
        slopes[pred_model_idx] * repeated_keys[i] + intercpts[pred_model_idx];
    pred_model_idx = static_cast<int>(
        std::max(0., std::min(FANOUT - 1., pred_cdf * FANOUT)));

    repeated_keys_predicted_ranks[pred_model_idx].push_back(repeated_keys[i]);
    repeated_key_counts[pred_model_idx].push_back(0);
  }

  if (repeated_keys.size() ==
      0)
  { // No significantly repeated keys in the sample
    pred_model_idx = 0;

    // Process each key in order
    for (auto cur_key = begin; cur_key < end; ++cur_key)
    {

      // Predict the model idx in the leaf layer
      pred_model_idx = static_cast<int>(
          std::max(0., std::min(num_models - 1.,
                                root_slope * cur_key[0] + root_intrcpt)));

      // Predict the CDF
      pred_cdf = slopes[pred_model_idx] * cur_key[0] + intercpts[pred_model_idx];

      // Scale the CDF to the number of buckets
      pred_model_idx = static_cast<int>(
          std::max(0., std::min(FANOUT - 1., pred_cdf * FANOUT)));

      if (major_bckt_sizes[pred_model_idx] <
          MAJOR_BCKT_CAPACITY)
      { // The predicted bucket is not full
        major_bckts[MAJOR_BCKT_CAPACITY * pred_model_idx +
                    major_bckt_sizes[pred_model_idx]] = cur_key[0];

        // Update the bucket size
        ++major_bckt_sizes[pred_model_idx];
      }
      else
      { // The predicted bucket is full, place in the spill bucket
        spill_bucket.push_back(cur_key[0]);
      }
    }
  }
  else
  { // There are many repeated keys in the sample

    // Batch size for exceptions
    static constexpr unsigned int BATCH_SZ_EXP = 100;

    // Stores the predicted bucket for each input key in the current batch
    unsigned int pred_idx_in_batch_exc[BATCH_SZ_EXP] = {0};

    // Process elements in batches of size BATCH_SZ_EXP
    for (auto cur_key = begin; cur_key < end; cur_key += BATCH_SZ_EXP)
    {

      // Process each element in the batch and save their predicted indices
      for (unsigned int elm_idx = 0; elm_idx < BATCH_SZ_EXP; ++elm_idx)
      {

        // Predict the leaf model idx
        pred_idx_in_batch_exc[elm_idx] = static_cast<int>(std::max(
            0., std::min(num_models - 1.,
                         root_slope * cur_key[elm_idx] + root_intrcpt)));

        // Predict the CDF
        pred_cdf = slopes[pred_idx_in_batch_exc[elm_idx]] * cur_key[elm_idx] +
                   intercpts[pred_idx_in_batch_exc[elm_idx]];

        // Extrapolate the CDF to the number of buckets
        pred_idx_in_batch_exc[elm_idx] = static_cast<unsigned int>(
            std::max(0., std::min(FANOUT - 1., pred_cdf * FANOUT)));
      }

      // Go over the batch again and place the flagged keys in an exception
      // list
      bool exc_found = false; // If exceptions in the batch, don't insert into
                              // buckets, but save in an exception list

      for (unsigned int elm_idx = 0; elm_idx < BATCH_SZ_EXP; ++elm_idx)
      {
        exc_found = false;

        // Iterate over the keys in the exception list corresponding to the
        // predicted rank for the current key in the batch and the rank of the
        // exception
        for (unsigned int j = 0;
             j < repeated_keys_predicted_ranks[pred_idx_in_batch_exc[elm_idx]]
                     .size();
             ++j)
        {

          // If key in exception list, then flag it and update the counts that
          // will be used later
          if (repeated_keys_predicted_ranks[pred_idx_in_batch_exc[elm_idx]]
                                           [j] == cur_key[elm_idx])
          {
            ++repeated_key_counts[pred_idx_in_batch_exc[elm_idx]]
                                 [j]; // Increment count of exception value
            exc_found = true;
            ++total_repeated_keys;
            break;
          }
        }

        if (!exc_found) // If no exception value was found in the batch,
                        // then proceed to putting them in the predicted
                        // buckets
        {

          // Check if the element will cause a bucket overflow
          if (major_bckt_sizes[pred_idx_in_batch_exc[elm_idx]] <
              MAJOR_BCKT_CAPACITY)
          { // The predicted bucket has not reached
            // full capacity, so place the element
            // in the bucket
            major_bckts[MAJOR_BCKT_CAPACITY *
                            pred_idx_in_batch_exc[elm_idx] +
                        major_bckt_sizes[pred_idx_in_batch_exc[elm_idx]]] =
                cur_key[elm_idx];
            ++major_bckt_sizes[pred_idx_in_batch_exc[elm_idx]];
          }
          else
          { // Place the item in the spill bucket
            spill_bucket.push_back(cur_key[elm_idx]);
          }
        }
      }
    }
  }

  //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - //
  //               Second round of shuffling                  //
  //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

  unsigned int NUM_MINOR_BCKT_PER_MAJOR_BCKT =
      MAJOR_BCKT_CAPACITY * OA_RATIO /
      THRESHOLD;

  const unsigned int ARR_SZ_MINOR_BCKTS =
      static_cast<int>(MAJOR_BCKT_CAPACITY * OA_RATIO) + 1;
  const unsigned int TOT_NUM_MINOR_BCKTS =
      std::ceil(static_cast<int>(OA_RATIO * INPUT_SZ) / THRESHOLD);

  vector<T> minor_bckts(ARR_SZ_MINOR_BCKTS);

  // Stores the index where the current bucket will start
  int bckt_start_offset = 0;

  // Stores the predicted CDF values for the elements in the current bucket
  unsigned int pred_idx_cache[THRESHOLD];

  // Caches the predicted bucket indices for each element in the batch
  vector<unsigned int> batch_cache(BATCH_SZ, 0);

  // Iterate over each major bucket
  for (unsigned int major_bckt_idx = 0; major_bckt_idx < FANOUT; ++major_bckt_idx)
  {

    // Update the bucket start offset
    bckt_start_offset = major_bckt_idx * MAJOR_BCKT_CAPACITY;

    // Array to keep track of sizes for the minor buckets in the current
    // bucket
    vector<unsigned int> minor_bckt_sizes(NUM_MINOR_BCKT_PER_MAJOR_BCKT, 0);

    // Find out the number of batches for this bucket
    unsigned int num_batches = major_bckt_sizes[major_bckt_idx] / BATCH_SZ;

    // Iterate over the elements in the current bucket in batch-mode
    for (unsigned int batch_idx = 0; batch_idx < num_batches; ++batch_idx)
    {

      // Iterate over the elements in the batch and store their predicted
      // ranks
      for (unsigned int elm_idx = 0; elm_idx < BATCH_SZ; ++elm_idx)
      {

        // Find the current element
        auto cur_elm = major_bckts[bckt_start_offset + elm_idx];

        // Predict the leaf-layer model
        batch_cache[elm_idx] = static_cast<int>(std::max(
            0., std::min(num_models - 1., root_slope * cur_elm + root_intrcpt)));

        // Predict the CDF
        pred_cdf = slopes[batch_cache[elm_idx]] * cur_elm +
                   intercpts[batch_cache[elm_idx]];

        // Scale the predicted CDF to the number of minor buckets and cache it
        batch_cache[elm_idx] = static_cast<int>(
            std::max(0., std::min(NUM_MINOR_BCKT_PER_MAJOR_BCKT - 1.,
                                  pred_cdf * TOT_NUM_MINOR_BCKTS -
                                      major_bckt_idx * NUM_MINOR_BCKT_PER_MAJOR_BCKT)));
      }

      // Iterate over the elements in the batch again, and place them in the
      // sub-buckets, or spill bucket
      for (unsigned int elm_idx = 0; elm_idx < BATCH_SZ; ++elm_idx)
      {

        // Find the current element
        auto cur_elm = major_bckts[bckt_start_offset + elm_idx];

        // Check if the element will cause a bucket overflow
        if (minor_bckt_sizes[batch_cache[elm_idx]] <
            THRESHOLD)
        { // The predicted bucket has not reached
          // full capacity, so place the element in
          // the bucket
          minor_bckts[THRESHOLD * batch_cache[elm_idx] +
                      minor_bckt_sizes[batch_cache[elm_idx]]] = cur_elm;
          ++minor_bckt_sizes[batch_cache[elm_idx]];
        }
        else
        { // Place the item in the spill bucket
          spill_bucket.push_back(cur_elm);
        }
      }

      // Update the start offset
      bckt_start_offset += BATCH_SZ;
    }

    //-  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -//
    // Repeat the above for the rest of the elements in the     //
    // current bucket in case its size wasn't divisible by      //
    // the BATCH_SZ                                             //
    //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - //
    unsigned int num_remaining_elm =
        major_bckt_sizes[major_bckt_idx] - num_batches * BATCH_SZ;

    for (unsigned int elm_idx = 0; elm_idx < num_remaining_elm; ++elm_idx)
    {
      auto cur_elm = major_bckts[bckt_start_offset + elm_idx];
      batch_cache[elm_idx] = static_cast<int>(std::max(
          0., std::min(num_models - 1., root_slope * cur_elm + root_intrcpt)));
      pred_cdf =
          slopes[batch_cache[elm_idx]] * cur_elm + intercpts[batch_cache[elm_idx]];
      batch_cache[elm_idx] = static_cast<int>(
          std::max(0., std::min(NUM_MINOR_BCKT_PER_MAJOR_BCKT - 1.,
                                pred_cdf * TOT_NUM_MINOR_BCKTS -
                                    major_bckt_idx * NUM_MINOR_BCKT_PER_MAJOR_BCKT)));
    }

    for (unsigned elm_idx = 0;
         elm_idx < major_bckt_sizes[major_bckt_idx] - num_batches * BATCH_SZ;
         ++elm_idx)
    {
      auto cur_elm = major_bckts[bckt_start_offset + elm_idx];
      if (minor_bckt_sizes[batch_cache[elm_idx]] < THRESHOLD)
      {
        minor_bckts[THRESHOLD * batch_cache[elm_idx] +
                    minor_bckt_sizes[batch_cache[elm_idx]]] = cur_elm;
        ++minor_bckt_sizes[batch_cache[elm_idx]];
      }
      else
      {
        spill_bucket.push_back(cur_elm);
      }
    }

    //----------------------------------------------------------//
    //                MODEL-BASED COUNTING SORT                 //
    //----------------------------------------------------------//

    // Iterate over the minor buckets of the current bucket
    for (unsigned int bckt_idx = 0;
         bckt_idx < NUM_MINOR_BCKT_PER_MAJOR_BCKT;
         ++bckt_idx)
    {
      if (minor_bckt_sizes[bckt_idx] > 0)
      {

        // Update the bucket start offset
        bckt_start_offset = 1. * (major_bckt_idx * NUM_MINOR_BCKT_PER_MAJOR_BCKT + bckt_idx) * INPUT_SZ / TOT_NUM_MINOR_BCKTS;

        // Count array for the model-enhanced counting sort subroutine
        vector<unsigned int> cnt_hist(THRESHOLD, 0);

        /*
        * OPTIMIZATION
        * We check to see if the first and last element in the current bucket 
        * used the same leaf model to obtain their CDF. If that is the case, 
        * then we don't need to traverse the CDF model for every element in 
        * this bucket, hence decreasing the inference complexity from O(num_layer) 
        * to O(1).
        */

        int pred_model_first_elm = static_cast<int>(std::max(
            0.,
            std::min(
                num_models - 1.,
                root_slope *
                        minor_bckts[bckt_idx * THRESHOLD] +
                    root_intrcpt)));

        int pred_model_last_elm = static_cast<int>(std::max(
            0.,
            std::min(
                num_models - 1.,
                root_slope *
                        minor_bckts[bckt_idx * THRESHOLD +
                                    minor_bckt_sizes[bckt_idx] - 1] +
                    root_intrcpt)));

        if (pred_model_first_elm ==
            pred_model_last_elm)
        { // Avoid CDF model traversal and predict the
          // CDF only using the leaf model

          // Iterate over the elements and place them into the minor buckets
          for (unsigned int elm_idx = 0; elm_idx < minor_bckt_sizes[bckt_idx];
               ++elm_idx)
          {

            // Find the current element
            auto cur_elm =
                minor_bckts[bckt_idx * THRESHOLD + elm_idx];

            // Predict the CDF
            pred_cdf = slopes[pred_model_first_elm] * cur_elm +
                       intercpts[pred_model_first_elm];

            // Scale the predicted CDF to the input size and cache it
            pred_idx_cache[elm_idx] = static_cast<int>(std::max(
                0., std::min(THRESHOLD - 1.,
                             (pred_cdf * INPUT_SZ) - bckt_start_offset)));

            // Update the counts
            ++cnt_hist[pred_idx_cache[elm_idx]];
          }
        }
        else
        { // Fully traverse the CDF model again to predict the CDF of the
          // current element

          // Iterate over the elements and place them into the minor buckets
          for (unsigned int elm_idx = 0; elm_idx < minor_bckt_sizes[bckt_idx];
               ++elm_idx)
          {
            // Find the current element
            auto cur_elm =
                minor_bckts[bckt_idx * THRESHOLD + elm_idx];

            // Predict the model idx in the leaf layer
            auto model_idx_next_layer = static_cast<int>(
                std::max(0., std::min(num_models - 1.,
                                      root_slope * cur_elm + root_intrcpt)));
            // Predict the CDF
            pred_cdf = slopes[model_idx_next_layer] * cur_elm +
                       intercpts[model_idx_next_layer];

            // Scale the predicted CDF to the input size and cache it
            pred_idx_cache[elm_idx] = static_cast<unsigned int>(std::max(
                0., std::min(THRESHOLD - 1.,
                             (pred_cdf * INPUT_SZ) - bckt_start_offset)));

            // Update the counts
            ++cnt_hist[pred_idx_cache[elm_idx]];
          }
        }

        --cnt_hist[0];

        // Calculate the running totals
        for (unsigned int cnt_idx = 1; cnt_idx < THRESHOLD; ++cnt_idx)
        {
          cnt_hist[cnt_idx] += cnt_hist[cnt_idx - 1];
        }

        // Re-shuffle the elms based on the calculated cumulative counts
        for (unsigned int elm_idx = 0; elm_idx < minor_bckt_sizes[bckt_idx];
             ++elm_idx)
        {
          // Place the element in the predicted position in the array
          major_bckts[num_tot_elms_in_bckts +
                      cnt_hist[pred_idx_cache[elm_idx]]] =
              minor_bckts[bckt_idx * THRESHOLD + elm_idx];
          // Update counts
          --cnt_hist[pred_idx_cache[elm_idx]];
        }

        //----------------------------------------------------------//
        //                  TOUCH-UP & COMPACTION                   //
        //----------------------------------------------------------//

        // After the model-based bucketization process is done, switch to a
        // deterministic sort
        T elm;
        int cmp_idx;

        // Perform Insertion Sort
        for (unsigned int elm_idx = 0; elm_idx < minor_bckt_sizes[bckt_idx];
             ++elm_idx)
        {
          cmp_idx = num_tot_elms_in_bckts + elm_idx - 1;
          elm = major_bckts[num_tot_elms_in_bckts + elm_idx];
          while (cmp_idx >= 0 && elm < major_bckts[cmp_idx])
          {
            major_bckts[cmp_idx + 1] = major_bckts[cmp_idx];
            --cmp_idx;
          }

          major_bckts[cmp_idx + 1] = elm;
        }

        num_tot_elms_in_bckts += minor_bckt_sizes[bckt_idx];
      } // end of iteration of each minor bucket
    }
  } // end of iteration over each major bucket

  //----------------------------------------------------------//
  //                 SORT THE SPILL BUCKET                    //
  //----------------------------------------------------------//

  std::sort(spill_bucket.begin(), spill_bucket.end());

  //----------------------------------------------------------//
  //               PLACE BACK THE EXCEPTION VALUES            //
  //----------------------------------------------------------//

  vector<T> linear_vals, linear_count;

  for (auto val_idx = 0; val_idx < 1000; ++val_idx)
  {
    for (size_t exc_elm_idx = 0;
         exc_elm_idx < repeated_keys_predicted_ranks[val_idx].size();
         ++exc_elm_idx)
    {
      linear_vals.push_back(
          repeated_keys_predicted_ranks[val_idx][exc_elm_idx]);
      linear_count.push_back(repeated_key_counts[val_idx][exc_elm_idx]);
    }
  }

  //----------------------------------------------------------//
  //               MERGE BACK INTO ORIGINAL ARRAY             //
  //----------------------------------------------------------//

  // Merge the spill bucket with the elements in the buckets
  std::merge(
      major_bckts.begin(), major_bckts.begin() + num_tot_elms_in_bckts,
      spill_bucket.begin(), spill_bucket.end(), begin + total_repeated_keys);

  //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - //
  //            Start merging the exception values            //
  //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

  // The read index for the exceptions
  unsigned int exc_idx = 0;

  // The read index for the already-merged elements from the buckets and the
  // spill bucket
  unsigned int input_idx = total_repeated_keys;

  // The write index for the final merging of everything
  int ptr = 0;

  while (input_idx < INPUT_SZ && exc_idx < linear_vals.size())
  {
    if (begin[input_idx] < linear_vals[exc_idx])
    {
      begin[ptr] = begin[input_idx];
      ptr++;
      input_idx++;
    }
    else
    {
      for (int i = 0; i < linear_count[exc_idx]; i++)
      {
        begin[ptr + i] = linear_vals[exc_idx];
      }
      ptr += linear_count[exc_idx];
      exc_idx++;
    }
  }

  while (exc_idx < linear_vals.size())
  {

    for (int i = 0; i < linear_count[exc_idx]; i++)
    {
      begin[ptr + i] = linear_vals[exc_idx];
    }
    ptr += linear_count[exc_idx];
    exc_idx++;
  }

  while (input_idx < INPUT_SZ)
  {
    begin[ptr] = begin[input_idx];
    ptr++;
    input_idx++;
  }

  // The input array is now sorted
}

/**
 * @brief Sorts a sequence of numerical keys from [begin, end) using Learned
 * Sort, in ascending order.
 *
 * @tparam RandomIt A bi-directional random iterator over the sequence of keys
 * @param begin Random-access iterators to the initial position of the sequence
 * to be used for sorting. The range used is [begin,end), which contains all the
 * elements between first and last, including the element pointed by first but
 * not the element pointed by last.
 * @param end Random-access iterators to the last position of the sequence to be
 * used for sorting. The range used is [begin,end), which contains all the
 * elements between first and last, including the element pointed by first but
 * not the element pointed by last.
 * @param params The hyperparameters for the CDF model, which describe the
 * architecture and sampling ratio.
 */
template <class RandomIt>
void learned_sort::sort(RandomIt begin, RandomIt end, RMI::Params &params)
{

  // Use std::sort for very small arrays
  if (std::distance(begin, end) <= std::max(params.fanout * params.threshold, 5 * params.arch[1]))
  {
    std::sort(begin, end);
  }
  else
  {

    // Determine the data type
    typedef typename iterator_traits<RandomIt>::value_type T;

    // Train
    RMI rmi = train(begin, end, params);

    // Sort
    if (rmi.trained)
      _sort_trained(begin, end, rmi);

    else // Fall back in case the model could not be trained
      std::sort(begin, end);
  }
}

/**
 * @brief Sorts a sequence of numerical keys from [begin, end) using Learned
 * Sort, in ascending order.
 *
 * @tparam RandomIt A bi-directional random iterator over the sequence of keys
 * @param begin Random-access iterators to the initial position of the sequence
 * to be used for sorting. The range used is [begin,end), which contains all the
 * elements between first and last, including the element pointed by first but
 * not the element pointed by last.
 * @param end Random-access iterators to the last position of the sequence to be
 * used for sorting. The range used is [begin,end), which contains all the
 * elements between first and last, including the element pointed by first but
 * not the element pointed by last.
 */
template <class RandomIt>
void learned_sort::sort(RandomIt begin, RandomIt end)
{
  RMI::Params p;
  learned_sort::sort(begin, end, p);
}

#endif // LEARNED_SORT_H
