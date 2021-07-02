/*
 * @author: Andrew I. Schein, PhD
 * @co-author: Ani Kristo
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
#include <iterator>
#include <vector>

using namespace std;

// RADIX SORT FLAVORS
void radix_sort(vector<float>::iterator begin, vector<float>::iterator end);

void radix_sort(vector<double>::iterator begin, vector<double>::iterator end);

void radix_sort(vector<int32_t>::iterator begin, vector<int32_t>::iterator end);

void radix_sort(vector<int64_t>::iterator begin, vector<int64_t>::iterator end);

void radix_sort(vector<uint32_t>::iterator begin,
                vector<uint32_t>::iterator end);

void radix_sort(vector<uint64_t>::iterator begin,
                vector<uint64_t>::iterator end);

#endif  // RADIX_SORT_H