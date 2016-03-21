/**
 * Copyright (c) 2016 Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of 
 * this software and associated documentation files (the "Software"), to deal in 
 * the Software without restriction, including without limitation the rights to 
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of 
 * the Software, and to permit persons to whom the Software is furnished to do so, 
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all 
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS 
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR 
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER 
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN 
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

/*
 * Stripped down version of vcf.h to re-use some values, expressions
 * if the full htslib library is not available
 */

#ifndef HTSLIB_VCF_LITE_H
#define HTSLIB_VCF_LITE_H

#include "c_api.h"

#ifdef HTSDIR   //Use htslib's headers

#include "htslib/vcf.h"

#else

#define BCF_HL_FLT  0 // header line : FLT = filter
#define BCF_HL_INFO 1
#define BCF_HL_FMT  2

#define BCF_VL_FIXED 0 // variable length
#define BCF_VL_VAR   1
#define BCF_VL_A     2
#define BCF_VL_G     3
#define BCF_VL_R     4
#define BCF_VL_P     5          //ploidy


#define bcf_int32_missing    INT32_MIN
#define bcf_int32_vector_end (INT32_MIN+1)
#define bcf_str_missing      0x07
#define bcf_str_vector_end   0
extern uint32_t bcf_float_vector_end;
extern uint32_t bcf_float_missing;

static inline void bcf_float_set(float *ptr, uint32_t value)
{
    union { uint32_t i; float f; } u;
    u.i = value;
    *ptr = u.f;
}
#define bcf_float_set_vector_end(x) bcf_float_set(&(x),bcf_float_vector_end)
#define bcf_float_set_missing(x)    bcf_float_set(&(x),bcf_float_missing)
static inline int bcf_float_is_missing(float f)
{
    union { uint32_t i; float f; } u;
    u.f = f;
    return u.i==bcf_float_missing ? 1 : 0;
}
static inline int bcf_float_is_vector_end(float f)
{
    union { uint32_t i; float f; } u;
    u.f = f;
    return u.i==bcf_float_vector_end ? 1 : 0;
}

#define bcf_alleles2gt(a,b) ((a)>(b)?((a)*((a)+1)/2+(b)):((b)*((b)+1)/2+(a)))

#endif //ifdef HTSDIR

//For bitwise comparison of float values
typedef union
{
  unsigned i;
  float f;
}fi_union;

typedef union
{
  uint64_t i;
  double d;
}di_union;

extern fi_union bcf_float_missing_union;
extern fi_union bcf_float_vector_end_union;

//Template function that return bcf missing value
template<class T>
inline T get_bcf_missing_value();

template<>
inline int8_t get_bcf_missing_value<int8_t>() { return bcf_int8_missing; }

template<>
inline int16_t get_bcf_missing_value<int16_t>() { return bcf_int16_missing; }

template<>
inline int get_bcf_missing_value<int>() { return bcf_int32_missing; }

template<>
inline unsigned get_bcf_missing_value<unsigned>() { return bcf_int32_missing; }

template<>
inline int64_t get_bcf_missing_value<int64_t>() { return bcf_int32_missing; }

template<>
inline uint64_t get_bcf_missing_value<uint64_t>() { return bcf_int32_missing; }

template<>
inline float get_bcf_missing_value<float>() { return bcf_float_missing_union.f; }

template<>
inline double get_bcf_missing_value<double>() { return bcf_float_missing_union.f; }

template<>
inline std::string get_bcf_missing_value<std::string>() { return ""; }

template<>
inline char get_bcf_missing_value<char>() { return bcf_str_missing; }

//Template function that return bcf vector_end value
template<class T>
inline T get_bcf_vector_end_value();

template<>
inline int8_t get_bcf_vector_end_value<int8_t>() { return bcf_int8_vector_end; }

template<>
inline int16_t get_bcf_vector_end_value<int16_t>() { return bcf_int16_vector_end; }

template<>
inline int get_bcf_vector_end_value<int>() { return bcf_int32_vector_end; }

template<>
inline unsigned get_bcf_vector_end_value<unsigned>() { return bcf_int32_vector_end; }

template<>
inline int64_t get_bcf_vector_end_value<int64_t>() { return bcf_int32_vector_end; }

template<>
inline uint64_t get_bcf_vector_end_value<uint64_t>() { return bcf_int32_vector_end; }

template<>
inline float get_bcf_vector_end_value<float>() { return bcf_float_vector_end_union.f; }

template<>
inline double get_bcf_vector_end_value<double>() { return bcf_float_vector_end_union.f; }

template<>
inline std::string get_bcf_vector_end_value<std::string>() { return ""; }

template<>
inline char get_bcf_vector_end_value<char>() { return bcf_str_vector_end; }

template<class T>
inline bool is_bcf_missing_value(const T val) { return val == get_bcf_missing_value<T>(); }

template<class T>
inline bool is_bcf_vector_end_value(const T val) { return val == get_bcf_vector_end_value<T>(); }

//for float, equality is not an adequate check
template<>
inline bool is_bcf_missing_value(const float val) { return bcf_float_is_missing(val); }

template<>
inline bool is_bcf_vector_end_value(const float val) { return bcf_float_is_vector_end(val); }

template<class T>
inline bool is_bcf_valid_value(const T val) { return !is_bcf_missing_value(val) && !is_bcf_vector_end_value(val); }

//template function for obtaining TileDB null value
template<class T>
inline T get_tiledb_null_value();

template<>
inline char get_tiledb_null_value<char>() { return TILEDB_EMPTY_CHAR; }

template<>
inline int get_tiledb_null_value<int>() { return TILEDB_EMPTY_INT32; }

template<>
inline unsigned get_tiledb_null_value<unsigned>() { return TILEDB_EMPTY_INT32; }

template<>
inline int64_t get_tiledb_null_value<int64_t>() { return TILEDB_EMPTY_INT64; }

template<>
inline size_t get_tiledb_null_value<size_t>() { return TILEDB_EMPTY_INT64; }

template<>
inline float get_tiledb_null_value<float>() { return TILEDB_EMPTY_FLOAT32; }

template<>
inline double get_tiledb_null_value<double>() { return TILEDB_EMPTY_FLOAT64; }

//Template function for checking TileDB null value
template<class T>
inline bool is_tiledb_missing_value(const T val) { return (val == get_tiledb_null_value<T>()); }

//For FP values, bitwise exact null comparison needed
extern fi_union g_tiledb_null_float;
extern di_union g_tiledb_null_double;

template<>
inline bool is_tiledb_missing_value(const float val)
{
  fi_union x = { .f = val };
  return (x.i == g_tiledb_null_float.i);
}

template<>
inline bool is_tiledb_missing_value(const double val)
{
  di_union x = { .d = val };
  return (x.i == g_tiledb_null_double.i);
}

#endif
