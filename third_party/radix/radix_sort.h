/*
 * @author: Andrew I. Schein, PhD
 *
 * Adapted from:
 * https://bitbucket.org/ais/usort/src/474cc2a19224/usort/f8_sort.c
 *
 *
 * IEEE format (float_32): [1 bit: Sign][ 8 bits: exponent][23 bits: fraction]
 * IEEE format (float_64): [1 bit: Sign][11 bits: exponent][52 bits: fraction]
 *
 * NOTE: Only works for little endian systems.
 */

#ifndef RADIXSORT_H
#define RADIXSORT_H

#include <string.h>

#include <algorithm>
#include <vector>

using namespace std;

constexpr size_t HIST_SIZE = 2048;

#define get_byte_0(v) ((v)&0x7FF)
#define get_byte_1(v) (((v) >> 11) & 0x7FF)
#define get_byte_2(v) (((v) >> 22) & 0x7FF)
#define get_byte_3(v) (((v) >> 33) & 0x7FF)
#define get_byte_4(v) (((v) >> 44) & 0x7FF)
#define get_byte_5(v) (((v) >> 55) & 0x7FF)
#define get_byte_2_flip_sign(v) (((unsigned)(v) >> 22) ^ 0x200)
#define get_byte_5_flip_sign(v) ((((v) >> 55) & 0x7FF) ^ 0x400)

static inline uint32_t f4_sort_FloatFlip(uint32_t f) {
  uint32_t mask = -(f >> 31) | 0x80000000u;
  return (f ^ mask);
}

static inline uint32_t f4_sort_IFloatFlip(uint32_t f) {
  uint32_t mask = ((f >> 31) - 1) | 0x80000000u;
  return (f ^ mask);
}

static inline uint64_t f8_sort_FloatFlip(uint64_t u) {
  uint64_t mask = -(u >> 63) | 0x8000000000000000ull;
  return (u ^ mask);
}

static inline uint64_t f8_sort_IFloatFlip(uint64_t u) {
  uint64_t mask = ((u >> 63) - 1) | 0x8000000000000000ull;
  return (u ^ mask);
}

void radix_sort(vector<float>::iterator begin, vector<float>::iterator end) {
  // Tiny test to see if the system is little endian.
  // If not, use std::sort as begin fallback.
  unsigned int i = 1;
  char *is_little_endian = (char *)&i;
  if (*is_little_endian) {
    // Start Radix Sort
    const size_t sz = std::distance(begin, end);
    size_t j;
    size_t pos;
    size_t n, sum0 = 0, sum1 = 0, sum2 = 0, tsum = 0;
    uint32_t *reader, *writer, *buf1 = (uint32_t *)&begin[0], *buf2;
    size_t b0[HIST_SIZE * 3], *b1, *b2;

    if (sz < HIST_SIZE) return std::sort(begin, end);

    buf2 = (uint32_t *)malloc(sz * sizeof(uint32_t));

    b1 = b0 + HIST_SIZE;
    b2 = b1 + HIST_SIZE;
    memset(b0, 0, HIST_SIZE * 3 * sizeof(size_t));

    for (n = 0; n < sz; n++) {
      buf1[n] = f4_sort_FloatFlip(buf1[n]);
      b0[get_byte_0(buf1[n])]++;
      b1[get_byte_1(buf1[n])]++;
      b2[get_byte_2(buf1[n])]++;
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
    }

    writer = buf2; /* byte 0 */
    reader = buf1;
    for (n = 0; n < sz; n++) {
      pos = get_byte_0(reader[n]);
      writer[++b0[pos]] = reader[n];
    }

    writer = buf1; /* byte 1 */
    reader = buf2;
    for (n = 0; n < sz; n++) {
      pos = get_byte_1(reader[n]);
      writer[++b1[pos]] = reader[n];
    }

    writer = buf2; /* byte 2 */
    reader = buf1;
    for (n = 0; n < sz; n++) {
      pos = get_byte_2(reader[n]);
      writer[++b2[pos]] = f4_sort_IFloatFlip(reader[n]);
    }
    memcpy(&begin[0], writer, sz * sizeof(float));
    free(buf2);

  } else {  // The system is big endian, so fall back to std::sort
    std::sort(begin, end);
  }
}

void radix_sort(vector<double>::iterator begin, vector<double>::iterator end) {
  // Tiny test to see if the system is little endian.
  // If not, use std::sort as a fallback.
  unsigned int i = 1;
  char *is_little_endian = (char *)&i;
  if (*is_little_endian) {
    // Start Radix Sort
    const size_t sz = std::distance(begin, end);
    size_t j;
    size_t pos;
    size_t n, sum0 = 0, sum1 = 0, sum2 = 0, sum3 = 0, sum4 = 0, sum5 = 0,
              tsum = 0;
    uint64_t *reader, *writer, *buf1 = (uint64_t *)&begin[0], *buf2;
    size_t *b0, *b1, *b2, *b3, *b4, *b5;

    if (sz < HIST_SIZE) return std::sort(begin, end);

    buf2 = (uint64_t *)malloc(sz * sizeof(uint64_t));
    b0 = (uint64_t *)calloc(HIST_SIZE * 6, sizeof(uint64_t));
    b1 = b0 + HIST_SIZE;
    b2 = b1 + HIST_SIZE;
    b3 = b2 + HIST_SIZE;
    b4 = b3 + HIST_SIZE;
    b5 = b4 + HIST_SIZE;

    for (n = 0; n < sz; n++) {
      buf1[n] = f8_sort_FloatFlip(buf1[n]);
      b0[get_byte_0(buf1[n])]++;
      b1[get_byte_1(buf1[n])]++;
      b2[get_byte_2(buf1[n])]++;
      b3[get_byte_3(buf1[n])]++;
      b4[get_byte_4(buf1[n])]++;
      b5[get_byte_5(buf1[n])]++;
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
      pos = get_byte_0(reader[n]);
      writer[++b0[pos]] = reader[n];
    }

    writer = buf1; /* byte 1 */
    reader = buf2;
    for (n = 0; n < sz; n++) {
      pos = get_byte_1(reader[n]);
      writer[++b1[pos]] = reader[n];
    }

    writer = buf2; /* byte 2 */
    reader = buf1;
    for (n = 0; n < sz; n++) {
      pos = get_byte_2(reader[n]);
      writer[++b2[pos]] = reader[n];
    }

    writer = buf1; /* byte 3 */
    reader = buf2;
    for (n = 0; n < sz; n++) {
      pos = get_byte_3(reader[n]);
      writer[++b3[pos]] = reader[n];
    }

    writer = buf2; /* byte 4 */
    reader = buf1;

    for (n = 0; n < sz; n++) {
      pos = get_byte_4(reader[n]);
      writer[++b4[pos]] = reader[n];
    }

    writer = buf1; /* byte 5 */
    reader = buf2;
    for (n = 0; n < sz; n++) {
      pos = get_byte_5(reader[n]);
      writer[++b5[pos]] = f8_sort_IFloatFlip(reader[n]);
    }

    free(buf2);
    free(b0);

  } else {  // The system is big endian, so fall back to std::sort
    std::sort(begin, end);
  }
}

void radix_sort(vector<int32_t>::iterator begin,
                vector<int32_t>::iterator end) {
  // Tiny test to see if the system is little endian.
  // If not, use std::sort as a fallback.
  unsigned int i = 1;
  char *is_little_endian = (char *)&i;

  if (*is_little_endian) {
    // Start Radix Sort
    int32_t *const begin_ptr = &*begin;
    const size_t sz = std::distance(begin, end);
    size_t j;
    int32_t pos;
    size_t n, sum0 = 0, sum1 = 0, sum2 = 0, tsum = 0;
    int32_t *reader, *writer, *buf = (int32_t *)malloc(sz * sizeof(int32_t));
    size_t *b0, *b1, *b2;

    if (sz < HIST_SIZE) {
      free(buf);
      return std::sort(begin, end);
    }

    b0 = (size_t *)malloc(HIST_SIZE * 3 * sizeof(size_t));
    b1 = b0 + HIST_SIZE;
    b2 = b1 + HIST_SIZE;

    memset(b0, 0, 3 * HIST_SIZE * sizeof(size_t));
    for (n = 0; n < sz; n++) {
      b0[get_byte_0(begin[n])]++;
      b1[get_byte_1(begin[n])]++;
      b2[get_byte_2_flip_sign(begin[n])]++;
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
    }

    writer = buf; /* byte 0 */
    reader = begin_ptr;
    for (n = 0; n < sz; n++) {
      pos = get_byte_0(reader[n]);
      writer[++b0[pos]] = reader[n];
    }

    writer = begin_ptr; /* byte 1 */
    reader = buf;
    for (n = 0; n < sz; n++) {
      pos = get_byte_1(reader[n]);
      writer[++b1[pos]] = reader[n];
    }

    writer = buf; /* byte 2 */
    reader = begin_ptr;
    for (n = 0; n < sz; n++) {
      pos = get_byte_2_flip_sign(reader[n]);
      writer[++b2[pos]] = reader[n];
    }

    memcpy(begin_ptr, buf, sz * sizeof(int));

    free(buf);
    free(b0);

  } else {  // The system is big endian, so fall back to std::sort
    std::sort(begin, end);
  }
}

void radix_sort(vector<int64_t>::iterator begin,
                vector<int64_t>::iterator end) {
  // Tiny test to see if the system is little endian.
  // If not, use std::sort as a fallback.
  unsigned int i = 1;
  char *is_little_endian = (char *)&i;

  if (*is_little_endian) {
    // Start Radix Sort
    int64_t *const begin_ptr = &*begin;
    const size_t sz = std::distance(begin, end);
    int64_t j;
    size_t pos;
    size_t n, sum0 = 0, sum1 = 0, sum2 = 0, sum3 = 0, sum4 = 0, sum5 = 0,
              tsum = 0;
    int64_t *reader, *writer, *buf = (int64_t *)malloc(sz * sizeof(int64_t));
    size_t *b0, *b1, *b2, *b3, *b4, *b5;

    if (sz < HIST_SIZE) {
      free(buf);
      return std::sort(begin, end);
    }

    b0 = (size_t *)malloc(HIST_SIZE * 6 * sizeof(size_t));
    b1 = b0 + HIST_SIZE;
    b2 = b1 + HIST_SIZE;
    b3 = b2 + HIST_SIZE;
    b4 = b3 + HIST_SIZE;
    b5 = b4 + HIST_SIZE;

    memset(b0, 0, 6 * HIST_SIZE * sizeof(size_t));
    for (n = 0; n < sz; n++) {
      b0[get_byte_0(begin[n])]++;
      b1[get_byte_1(begin[n])]++;
      b2[get_byte_2(begin[n])]++;
      b3[get_byte_3(begin[n])]++;
      b4[get_byte_4(begin[n])]++;
      b5[get_byte_5_flip_sign(begin[n])]++;
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

    writer = buf; /* byte 0 */
    reader = begin_ptr;
    for (n = 0; n < sz; n++) {
      pos = get_byte_0(reader[n]);
      writer[++b0[pos]] = reader[n];
    }

    writer = begin_ptr; /* byte 1 */
    reader = buf;
    for (n = 0; n < sz; n++) {
      pos = get_byte_1(reader[n]);
      writer[++b1[pos]] = reader[n];
    }

    writer = buf; /* byte 2 */
    reader = begin_ptr;
    for (n = 0; n < sz; n++) {
      pos = get_byte_2(reader[n]);
      writer[++b2[pos]] = reader[n];
    }

    writer = begin_ptr; /* byte 3 */
    reader = buf;
    for (n = 0; n < sz; n++) {
      pos = get_byte_3(reader[n]);
      writer[++b3[pos]] = reader[n];
    }

    writer = buf; /* byte 4 */
    reader = begin_ptr;
    for (n = 0; n < sz; n++) {
      pos = get_byte_4(reader[n]);
      writer[++b4[pos]] = reader[n];
    }

    writer = begin_ptr; /* byte 5 */
    reader = buf;
    for (n = 0; n < sz; n++) {
      pos = get_byte_5_flip_sign(reader[n]);
      writer[++b5[pos]] = reader[n];
    }

    free(buf);
    free(b0);

  } else {  // The system is big endian, so fall back to std::sort
    std::sort(begin, end);
  }
}

void radix_sort(vector<uint32_t>::iterator begin,
                vector<uint32_t>::iterator end) {
  // Tiny test to see if the system is little endian.
  // If not, use std::sort as a fallback.
  unsigned int i = 1;
  char *is_little_endian = (char *)&i;

  if (*is_little_endian) {
    // Start Radix Sort
    uint32_t *const begin_ptr = &*begin;
    const size_t sz = std::distance(begin, end);
    size_t pos;
    size_t n, sum0 = 0, sum1 = 0, sum2 = 0, tsum = 0;
    uint32_t *reader, *writer, *buf = (uint32_t *)malloc(sz * sizeof(uint32_t));
    size_t *b0, *b1, *b2;

    if (sz < HIST_SIZE) {
      free(buf);
      return std::sort(begin, end);
    }

    b0 = (size_t *)malloc(HIST_SIZE * 3 * sizeof(size_t));
    b1 = b0 + HIST_SIZE;
    b2 = b1 + HIST_SIZE;

    memset(b0, 0, 3 * HIST_SIZE * sizeof(size_t));
    for (n = 0; n < sz; n++) {
      b0[get_byte_0(begin[n])]++;
      b1[get_byte_1(begin[n])]++;
      b2[get_byte_2(begin[n])]++;
    }

    for (size_t j = 0; j < HIST_SIZE; j++) {
      tsum = b0[j] + sum0;
      b0[j] = sum0 - 1;
      sum0 = tsum;

      tsum = b1[j] + sum1;
      b1[j] = sum1 - 1;
      sum1 = tsum;

      tsum = b2[j] + sum2;
      b2[j] = sum2 - 1;
      sum2 = tsum;
    }

    writer = buf; /* byte 0 */
    reader = begin_ptr;
    for (n = 0; n < sz; n++) {
      pos = get_byte_0(reader[n]);
      writer[++b0[pos]] = reader[n];
    }

    writer = begin_ptr; /* byte 1 */
    reader = buf;
    for (n = 0; n < sz; n++) {
      pos = get_byte_1(reader[n]);
      writer[++b1[pos]] = reader[n];
    }

    writer = buf; /* byte 2 */
    reader = begin_ptr;
    for (n = 0; n < sz; n++) {
      pos = get_byte_2(reader[n]);
      writer[++b2[pos]] = reader[n];
    }

    memcpy(begin_ptr, buf, sz * sizeof(uint32_t));

    free(buf);
    free(b0);

  } else {  // The system is big endian, so fall back to std::sort
    std::sort(begin, end);
  }
}

void radix_sort(vector<uint64_t>::iterator begin,
                vector<uint64_t>::iterator end) {
  // Tiny test to see if the system is little endian.
  // If not, use std::sort as a fallback.
  unsigned int i = 1;
  char *is_little_endian = (char *)&i;

  if (*is_little_endian) {
    // Start Radix Sort
    uint64_t *const begin_ptr = &*begin;
    const size_t sz = std::distance(begin, end);
    size_t j;
    uint64_t pos;
    size_t n, sum0 = 0, sum1 = 0, sum2 = 0, sum3 = 0, sum4 = 0, sum5 = 0,
              tsum = 0;
    uint64_t *reader, *writer, *buf = (uint64_t *)malloc(sz * sizeof(uint64_t));
    size_t *b0, *b1, *b2, *b3, *b4, *b5;

    if (sz < HIST_SIZE) {
      free(buf);
      return std::sort(begin, end);
    }

    b0 = (size_t *)malloc(HIST_SIZE * 6 * sizeof(size_t));
    b1 = b0 + HIST_SIZE;
    b2 = b1 + HIST_SIZE;
    b3 = b2 + HIST_SIZE;
    b4 = b3 + HIST_SIZE;
    b5 = b4 + HIST_SIZE;

    memset(b0, 0, 6 * HIST_SIZE * sizeof(size_t));
    for (n = 0; n < sz; n++) {
      b0[get_byte_0(begin[n])]++;
      b1[get_byte_1(begin[n])]++;
      b2[get_byte_2(begin[n])]++;
      b3[get_byte_3(begin[n])]++;
      b4[get_byte_4(begin[n])]++;
      b5[get_byte_5(begin[n])]++;
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

    writer = buf; /* byte 0 */
    reader = begin_ptr;
    for (n = 0; n < sz; n++) {
      pos = get_byte_0(reader[n]);
      writer[++b0[pos]] = reader[n];
    }

    writer = begin_ptr; /* byte 1 */
    reader = buf;
    for (n = 0; n < sz; n++) {
      pos = get_byte_1(reader[n]);
      writer[++b1[pos]] = reader[n];
    }

    writer = buf; /* byte 2 */
    reader = begin_ptr;
    for (n = 0; n < sz; n++) {
      pos = get_byte_2(reader[n]);
      writer[++b2[pos]] = reader[n];
    }

    writer = begin_ptr; /* byte 3 */
    reader = buf;
    for (n = 0; n < sz; n++) {
      pos = get_byte_3(reader[n]);
      writer[++b3[pos]] = reader[n];
    }

    writer = buf; /* byte 4 */
    reader = begin_ptr;
    for (n = 0; n < sz; n++) {
      pos = get_byte_4(reader[n]);
      writer[++b4[pos]] = reader[n];
    }

    writer = begin_ptr; /* byte 5 */
    reader = buf;
    for (n = 0; n < sz; n++) {
      pos = get_byte_5(reader[n]);
      writer[++b5[pos]] = reader[n];
    }

    free(buf);
    free(b0);

  } else {  // The system is big endian, so fall back to std::sort
    std::sort(begin, end);
  }
}

#undef get_byte_0
#undef get_byte_1
#undef get_byte_2
#undef get_byte_3
#undef get_byte_4
#undef get_byte_5
#undef get_byte_2_flip_sign
#undef get_byte_2_flip_sign

template <class RandomIt>
void radix_sort(RandomIt begin, RandomIt end) {
  cout << "Radix sort on this data type has not been implemented yet!" << endl;
}

#endif  // RADIX_SORT_HH