/*
 * NOTES:
 * Works only with double-precision IEEE 754 floating points and _little endian_
 * systems.
 *
 * Taken and adapted from:
 * https://bitbucket.org/ais/usort/src/474cc2a19224/usort/f8_sort.c
 *
 *
 * IEEE format: [1 bit: Sign bit][11 bits: biased exponent][52 bits: mantissa /
 * significand]
 */

#ifndef RADIXSORT_HH
#define RADIXSORT_HH

#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>

using namespace std;

#define HIST_SIZE 2048
#define _0(v) ((v)&0x7FF)
#define _1(v) (((v) >> 11) & 0x7FF)
#define _2(v) (((v) >> 22) & 0x7FF)
#define _3(v) (((v) >> 33) & 0x7FF)
#define _4(v) (((v) >> 44) & 0x7FF)
#define _5(v) (((v) >> 55) & 0x7FF)

static inline unsigned long long f8_sort_FloatFlip(unsigned long long u) {
  unsigned long long mask = -(u >> 63) | 0x8000000000000000ull;
  return (u ^ mask);
}

static inline unsigned long long f8_sort_IFloatFlip(unsigned long long u) {
  unsigned long long mask = ((u >> 63) - 1) | 0x8000000000000000ull;
  return (u ^ mask);
}

void radix_sort(vector<double>::iterator a, vector<double>::iterator b) {
  // Tiny test to see if the system is little endian.
  // If not, use std::sort as a fallback.
  unsigned int i = 1;
  char *is_little_endian = (char *)&i;
  if (*is_little_endian) {
    // Start Radix Sort
    const long sz = std::distance(a, b);
    long j;
    unsigned long pos;
    long n, sum0 = 0, sum1 = 0, sum2 = 0, sum3 = 0, sum4 = 0, sum5 = 0,
            tsum = 0;
    unsigned long long *reader, *writer, *buf1 = (unsigned long long *)&a[0],
                                         *buf2;
    unsigned long *b0, *b1, *b2, *b3, *b4, *b5;

    if (sz < HIST_SIZE) return std::sort(a, b);

    buf2 = (unsigned long long *)malloc(sz * sizeof(double));
    b0 = (unsigned long *)calloc(HIST_SIZE * 6, sizeof(unsigned long));
    b1 = b0 + HIST_SIZE;
    b2 = b1 + HIST_SIZE;
    b3 = b2 + HIST_SIZE;
    b4 = b3 + HIST_SIZE;
    b5 = b4 + HIST_SIZE;

    for (n = 0; n < sz; n++) {
      buf1[n] = f8_sort_FloatFlip(buf1[n]);
      b0[_0(buf1[n])]++;
      b1[_1(buf1[n])]++;
      b2[_2(buf1[n])]++;
      b3[_3(buf1[n])]++;
      b4[_4(buf1[n])]++;
      b5[_5(buf1[n])]++;
    }

    for (j = 0; j < HIST_SIZE; j++) {
      tsum = b0[j] + sum0;
      b0[j] = sum0 - 1;
      sum0 = tsum;

      tsum = b1[j] + sum1;
      b1[j] = sum1 - 1;
      sum1 = tsum;

      tsum = b2[j] + sum2;
      b2[j] = sum2 - 1;
      sum2 = tsum;

      tsum = b3[j] + sum3;
      b3[j] = sum3 - 1;
      sum3 = tsum;

      tsum = b4[j] + sum4;
      b4[j] = sum4 - 1;
      sum4 = tsum;

      tsum = b5[j] + sum5;
      b5[j] = sum5 - 1;
      sum5 = tsum;
    }

    writer = buf2; /* byte 0 */
    reader = buf1;
    for (n = 0; n < sz; n++) {
      pos = _0(reader[n]);
      writer[++b0[pos]] = reader[n];
    }

    writer = buf1; /* byte 1 */
    reader = buf2;
    for (n = 0; n < sz; n++) {
      pos = _1(reader[n]);
      writer[++b1[pos]] = reader[n];
    }

    writer = buf2; /* byte 2 */
    reader = buf1;
    for (n = 0; n < sz; n++) {
      pos = _2(reader[n]);
      writer[++b2[pos]] = reader[n];
    }

    writer = buf1; /* byte 3 */
    reader = buf2;
    for (n = 0; n < sz; n++) {
      pos = _3(reader[n]);
      writer[++b3[pos]] = reader[n];
    }

    writer = buf2; /* byte 4 */
    reader = buf1;

    for (n = 0; n < sz; n++) {
      pos = _4(reader[n]);
      writer[++b4[pos]] = reader[n];
    }

    writer = buf1; /* byte 5 */
    reader = buf2;
    for (n = 0; n < sz; n++) {
      pos = _5(reader[n]);
      writer[++b5[pos]] = f8_sort_IFloatFlip(reader[n]);
    }

    free(buf2);
    free(b0);

  } else {  // The system is big endian, so fall back to std::sort
    std::sort(a, b);
  }
}

#undef HIST_SIZE
#undef _0
#undef _1
#undef _2
#undef _3
#undef _4
#undef _5

template <class RandomIt>
void radix_sort(RandomIt a, RandomIt b) {
  cout << "Radix sort is not implemented for non-double types. Using std::sort." << endl;
  std::sort(a, b);
}

#endif  // RADIX_SORT_HH