/*

   BLIS
   An object-based framework for developing high-performance BLAS-like
   libraries.

   Copyright (C) 2023, Advanced Micro Devices, Inc. All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are
   met:
	- Redistributions of source code must retain the above copyright
	  notice, this list of conditions and the following disclaimer.
	- Redistributions in binary form must reproduce the above copyright
	  notice, this list of conditions and the following disclaimer in the
	  documentation and/or other materials provided with the distribution.
	- Neither the name(s) of the copyright holder(s) nor the names of its
	  contributors may be used to endorse or promote products derived
	  from this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
   HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/
#ifndef AOCL_LPGEMM_MATH_UTILS_AVX2_H
#define AOCL_LPGEMM_MATH_UTILS_AVX2_H

//constants for exp function
#define lpgemm_exp_c0 0x1.0000014439a91p0
#define lpgemm_exp_c1 0x1.62e43170e3344p-1
#define lpgemm_exp_c2 0x1.ebf906bc4c115p-3
#define lpgemm_exp_c3 0x1.c6ae2bb88c0c8p-5
#define lpgemm_exp_c4 0x1.3d1079db4ef69p-7
#define lpgemm_exp_c5 0x1.5f8905cb0cc4ep-10

#define TBL_LN2 0x1.71547652b82fep+0
#define EXPF_HUGE 0x1.8p+23
#define EXPF_MIN -88.7228393f
#define EXPF_MAX 88.7228393f
#define inf 1.0/0.0
#define sign -2147483648

//constants for erf function
#define lpgemm_erf_c0 0x1.20dd7890d27e1cec99fce48c29cp0
#define lpgemm_erf_c1 -0x1.ab4bed70f238422edeeba9c558p-16
#define lpgemm_erf_c2 -0x1.80a1bd5878e0b0689c5ff4fcdd4p-2
#define lpgemm_erf_c3 -0x1.07cb4cde6a7d9528c8a732990e4p-8
#define lpgemm_erf_c4 0x1.092cba598f96f00ddc5854cf7cp-3
#define lpgemm_erf_c5 -0x1.51f0ce4ac87c55f11f685864714p-5
#define lpgemm_erf_c6 0x1.4101f320bf8bc4d41c228faaa6cp-5
#define lpgemm_erf_c7 -0x1.2300882a7d1b712726997de80ep-4
#define lpgemm_erf_c8 0x1.d45745fff0e4b6d0604a9ab6284p-5
#define lpgemm_erf_c9 -0x1.9eb1491956e31ded96176d7c8acp-6
#define lpgemm_erf_c10 0x1.b9183fc75d326b9044bc63c9694p-8
#define lpgemm_erf_c11 -0x1.10e8f8c89ad8645e7d769cd596cp-10
#define lpgemm_erf_c12 0x1.224ffc80cc19957a48ecedad6c8p-14
#define lpgemm_erf_c13 0x1.12a30f42c71308321e7e7cb0174p-18
#define lpgemm_erf_c14 -0x1.155445e2e006723066d72d22ddcp-20
#define lpgemm_erf_c15 0x1.c6a4181da4ef76f22bd39bb5dcp-25

//Trignometric EXP, TANH and ERF functions for AVX2

#define POLY_EVAL_6_AVX2(r, r2, z) \
    r2 = _mm256_mul_ps (r, r); \
    z = _mm256_fmadd_ps (r2, _mm256_fmadd_ps (r, _mm256_set1_ps(lpgemm_exp_c3), _mm256_set1_ps(lpgemm_exp_c2)), \
        _mm256_fmadd_ps (r, _mm256_set1_ps(lpgemm_exp_c1), _mm256_set1_ps(lpgemm_exp_c0))); \
    r2 = _mm256_mul_ps (r2, r2); \
    r = _mm256_fmadd_ps (r2, _mm256_fmadd_ps (r, _mm256_set1_ps(lpgemm_exp_c5), _mm256_set1_ps(lpgemm_exp_c4)), z); \

#define EXPF_AVX2(x, r, r2, z, dn, q) \
    z = _mm256_mul_ps (x, _mm256_set1_ps(TBL_LN2));	\
	  dn = _mm256_add_ps (z , _mm256_set1_ps(EXPF_HUGE));  \
    r = _mm256_sub_ps (z , _mm256_sub_ps (dn , _mm256_set1_ps(EXPF_HUGE)));  \
\
    POLY_EVAL_6_AVX2 (r, r2, z); \
\
    q = _mm256_add_epi32((__m256i) (r), _mm256_sllv_epi32 ((__m256i)dn, _mm256_set1_epi32 (23)) ); \
    q =  (__m256i)_mm256_blendv_ps ((__m256)q, _mm256_set1_ps(inf), _mm256_cmp_ps (_mm256_set1_ps(88.0), x, 1)); \
    q =  (__m256i)_mm256_blendv_ps ((__m256)q, _mm256_set1_ps(0.0), _mm256_cmp_ps (x, _mm256_set1_ps(-88.0), 1));

#define TANHF_AVX2(x_tanh, r, r2, x, z, dn, q) \
    x = _mm256_mul_ps (_mm256_andnot_ps(_mm256_set1_ps(-0.0f), x_tanh), _mm256_set1_ps(-2) ); \
\
    EXPF_AVX2(x, r, r2, z, dn, q); \
\
    z =  _mm256_add_ps ((__m256)q, _mm256_set1_ps(-1)); \
    z = _mm256_div_ps (z, _mm256_add_ps (z, _mm256_set1_ps(2))); \
    z = _mm256_mul_ps (z, _mm256_set1_ps(-1)); \
    x_tanh = (_mm256_xor_ps (_mm256_and_ps (x_tanh, (__m256)(_mm256_set1_epi32(sign))), z)) ;

#define POLY_EVAL_HORNER_16_0_AVX2(r,x) \
    x = _mm256_mul_ps (_mm256_fmadd_ps ( \
    _mm256_fmadd_ps(_mm256_fmadd_ps (_mm256_fmadd_ps (_mm256_fmadd_ps (_mm256_fmadd_ps ( _mm256_fmadd_ps ( \
    _mm256_fmadd_ps (_mm256_fmadd_ps (_mm256_fmadd_ps (_mm256_fmadd_ps (_mm256_fmadd_ps (_mm256_fmadd_ps ( \
    _mm256_fmadd_ps ( _mm256_fmadd_ps (r, _mm256_set1_ps(lpgemm_erf_c15), _mm256_set1_ps(lpgemm_erf_c14)), r, _mm256_set1_ps(lpgemm_erf_c13)), \
    r, _mm256_set1_ps(lpgemm_erf_c12)), r,  _mm256_set1_ps(lpgemm_erf_c11)), r, _mm256_set1_ps(lpgemm_erf_c10)), r, _mm256_set1_ps(lpgemm_erf_c9)), \
    r, _mm256_set1_ps(lpgemm_erf_c8)), r, _mm256_set1_ps(lpgemm_erf_c7)), r, _mm256_set1_ps(lpgemm_erf_c6)), r, _mm256_set1_ps(lpgemm_erf_c5)), r, \
    _mm256_set1_ps(lpgemm_erf_c4)), r, _mm256_set1_ps(lpgemm_erf_c3)), r, _mm256_set1_ps(lpgemm_erf_c2)), r, _mm256_set1_ps(lpgemm_erf_c1)), r, \
    _mm256_set1_ps(lpgemm_erf_c0)), r); \

#define ERF_AVX2(x_erf, r, x) \
    r = _mm256_and_ps (x_erf, (__m256)_mm256_set1_epi32(0x7FFFFFFF)); \
\
    POLY_EVAL_HORNER_16_0_AVX2(r,x); \
\
    x = _mm256_blendv_ps (x, _mm256_set1_ps(1), _mm256_cmp_ps (_mm256_set1_ps(3.9192059040069580078125f), r, 1)); \
    x_erf = _mm256_or_ps(_mm256_and_ps (x_erf, (__m256)_mm256_set1_epi32(~(0x7FFFFFFF))), x);

//Trignometric EXP, TANH and ERF functions for SSE

#define POLY_EVAL_6_SSE(r, r2, z) \
    r2 = _mm_mul_ps (r, r); \
    z = _mm_fmadd_ps (r2, _mm_fmadd_ps (r, _mm_set1_ps(lpgemm_exp_c3), _mm_set1_ps(lpgemm_exp_c2)), \
        _mm_fmadd_ps (r, _mm_set1_ps(lpgemm_exp_c1), _mm_set1_ps(lpgemm_exp_c0))); \
    r2 = _mm_mul_ps (r2, r2); \
    r = _mm_fmadd_ps (r2, _mm_fmadd_ps (r, _mm_set1_ps(lpgemm_exp_c5), _mm_set1_ps(lpgemm_exp_c4)), z); \

#define EXPF_SSE(x, r, r2, z, dn, q) \
    z = _mm_mul_ps (x, _mm_set1_ps(TBL_LN2));	\
	  dn = _mm_add_ps (z , _mm_set1_ps(EXPF_HUGE));  \
    r = _mm_sub_ps (z , _mm_sub_ps (dn , _mm_set1_ps(EXPF_HUGE)));  \
\
    POLY_EVAL_6_SSE (r, r2, z); \
\
    q = _mm_add_epi32((__m128i) (r), _mm_sllv_epi32 ((__m128i)dn, _mm_set1_epi32 (23)) ); \
    q =  (__m128i)_mm_blendv_ps ((__m128)q, _mm_set1_ps(inf), _mm_cmp_ps (_mm_set1_ps(88.0), x, 1)); \
    q =  (__m128i)_mm_blendv_ps ((__m128)q, _mm_set1_ps(0.0), _mm_cmp_ps (x, _mm_set1_ps(-88.0), 1));

#define TANHF_SSE(x_tanh, r, r2, x, z, dn, q) \
    x = _mm_mul_ps (_mm_andnot_ps(_mm_set1_ps(-0.0f), x_tanh), _mm_set1_ps(-2) ); \
\
    EXPF_SSE(x, r, r2, z, dn, q); \
\
    z =  _mm_add_ps ((__m128)q, _mm_set1_ps(-1)); \
    z = _mm_div_ps (z, _mm_add_ps (z, _mm_set1_ps(2))); \
    z = _mm_mul_ps (z, _mm_set1_ps(-1)); \
    x_tanh = (_mm_xor_ps (_mm_and_ps (x_tanh, (__m128)(_mm_set1_epi32(sign))), z)) ;

#define POLY_EVAL_HORNER_16_0_SSE(r,x) \
    x = _mm_mul_ps (_mm_fmadd_ps ( \
    _mm_fmadd_ps(_mm_fmadd_ps (_mm_fmadd_ps (_mm_fmadd_ps (_mm_fmadd_ps ( _mm_fmadd_ps ( \
    _mm_fmadd_ps (_mm_fmadd_ps (_mm_fmadd_ps (_mm_fmadd_ps (_mm_fmadd_ps (_mm_fmadd_ps ( \
    _mm_fmadd_ps ( _mm_fmadd_ps (r, _mm_set1_ps(lpgemm_erf_c15), _mm_set1_ps(lpgemm_erf_c14)), r, _mm_set1_ps(lpgemm_erf_c13)), \
    r, _mm_set1_ps(lpgemm_erf_c12)), r,  _mm_set1_ps(lpgemm_erf_c11)), r, _mm_set1_ps(lpgemm_erf_c10)), r, _mm_set1_ps(lpgemm_erf_c9)), \
    r, _mm_set1_ps(lpgemm_erf_c8)), r, _mm_set1_ps(lpgemm_erf_c7)), r, _mm_set1_ps(lpgemm_erf_c6)), r, _mm_set1_ps(lpgemm_erf_c5)), r, \
    _mm_set1_ps(lpgemm_erf_c4)), r, _mm_set1_ps(lpgemm_erf_c3)), r, _mm_set1_ps(lpgemm_erf_c2)), r, _mm_set1_ps(lpgemm_erf_c1)), r, \
    _mm_set1_ps(lpgemm_erf_c0)), r); \

#define ERF_SSE(x_erf, r, x) \
    r = _mm_and_ps (x_erf, (__m128)_mm_set1_epi32(0x7FFFFFFF)); \
\
    POLY_EVAL_HORNER_16_0_SSE(r,x); \
\
    x = _mm_blendv_ps (x, _mm_set1_ps(1), _mm_cmp_ps (_mm_set1_ps(3.9192059040069580078125f), r, 1)); \
    x_erf = _mm_or_ps(_mm_and_ps (x_erf, (__m128)_mm_set1_epi32(~(0x7FFFFFFF))), x);

#endif // AOCL_LPGEMM_MATH_UTILS_AVX2_H
