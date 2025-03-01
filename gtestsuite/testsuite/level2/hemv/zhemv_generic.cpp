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

#include <gtest/gtest.h>
#include "test_hemv.h"

class zhemvTest :
        public ::testing::TestWithParam<std::tuple<char,
                                                   char,
                                                   char,
                                                   char,
                                                   gtint_t,
                                                   dcomplex,
                                                   dcomplex,
                                                   gtint_t,
                                                   gtint_t,
                                                   gtint_t,
                                                   char>> {};

TEST_P(zhemvTest, RandomData) {
    using T = dcomplex;

    //----------------------------------------------------------
    // Initialize values from the parameters passed through
    // test suite instantiation (INSTANTIATE_TEST_SUITE_P).
    //----------------------------------------------------------
    // matrix storage format(row major, column major)
    char storage = std::get<0>(GetParam());
    // denotes whether matrix a is u,l
    char uploa = std::get<1>(GetParam());
    // denotes whether matrix a is n,c
    char conja = std::get<2>(GetParam());
    // denotes whether vector x is n,c
    char conjx = std::get<3>(GetParam());
    // matrix size n
    gtint_t n  = std::get<4>(GetParam());
    // specifies alpha value
    T alpha = std::get<5>(GetParam());
    // specifies beta value
    T beta = std::get<6>(GetParam());
    // stride size for x:
    gtint_t incx = std::get<7>(GetParam());
    // stride size for y
    gtint_t incy = std::get<8>(GetParam());
    // lda increment.
    // If increment is zero, then the array size matches the matrix size.
    // If increment are nonnegative, the array size is bigger than the matrix size.
    gtint_t lda_inc = std::get<9>(GetParam());
    // specifies the datatype for randomgenerators
    char datatype   = std::get<10>(GetParam());

    // Set the threshold for the errors:
    double thresh = 8*std::sqrt(n)*testinghelpers::getEpsilon<T>();

    //----------------------------------------------------------
    //     Call test body using these parameters
    //----------------------------------------------------------
    test_hemv<T>(storage, uploa, conja, conjx, n, alpha, lda_inc, incx, beta, incy, thresh, datatype);
}

class zhemvTestPrint {
public:
    std::string operator()(
        testing::TestParamInfo<std::tuple<char,char,char,char,gtint_t,dcomplex,dcomplex,gtint_t,gtint_t,gtint_t,char>> str) const {
        char sfm       = std::get<0>(str.param);
        char uploa     = std::get<1>(str.param);
        char conja     = std::get<2>(str.param);
        char conjx     = std::get<3>(str.param);
        gtint_t n      = std::get<4>(str.param);
        dcomplex alpha = std::get<5>(str.param);
        dcomplex beta  = std::get<6>(str.param);
        gtint_t incx   = std::get<7>(str.param);
        gtint_t incy   = std::get<8>(str.param);
        gtint_t ld_inc = std::get<9>(str.param);
        char datatype  = std::get<10>(str.param);
#ifdef TEST_BLAS
        std::string str_name = "zhemv_";
#elif TEST_CBLAS
        std::string str_name = "cblas_zhemv";
#else  //#elif TEST_BLIS_TYPED
        std::string str_name = "blis_zhemv";
#endif
        str_name    = str_name + "_" + sfm;
        str_name    = str_name + "_" + uploa+conja+conjx;
        str_name    = str_name + "_" + std::to_string(n);
        std::string alpha_str = ( alpha.real > 0) ? std::to_string(int(alpha.real)) : ("m" + std::to_string(int(std::abs(alpha.real))));
                    alpha_str = alpha_str + "pi" + (( alpha.imag > 0) ? std::to_string(int(alpha.imag)) : ("m" + std::to_string(int(std::abs(alpha.imag)))));
        std::string beta_str = ( beta.real > 0) ? std::to_string(int(beta.real)) : ("m" + std::to_string(int(std::abs(beta.real))));
                    beta_str = beta_str + "pi" + (( beta.imag > 0) ? std::to_string(int(beta.imag)) : ("m" + std::to_string(int(std::abs(beta.imag)))));
        str_name    = str_name + "_a" + alpha_str;
        str_name    = str_name + "_a" + beta_str;
        std::string incx_str = ( incx > 0) ? std::to_string(incx) : "m" + std::to_string(std::abs(incx));
        std::string incy_str = ( incy > 0) ? std::to_string(incy) : "m" + std::to_string(std::abs(incy));
        str_name    = str_name + "_" + incx_str;
        str_name    = str_name + "_" + incy_str;
        str_name    = str_name + "_" + std::to_string(ld_inc);
        str_name    = str_name + "_" + datatype;
        return str_name;
    }
};

// Black box testing.
INSTANTIATE_TEST_SUITE_P(
        Blackbox,
        zhemvTest,
        ::testing::Combine(
            ::testing::Values('c'
#ifndef TEST_BLAS
            ,'r'
#endif
            ),                                                               // storage format
            ::testing::Values('u','l'),                                      // uploa
            ::testing::Values('n'),                                          // conja
            ::testing::Values('n'),                                          // conjx
            ::testing::Range(gtint_t(10), gtint_t(31), 10),                  // n
            ::testing::Values(dcomplex{1.0, -2.0}),                          // alpha
            ::testing::Values(dcomplex{2.0, -1.0}),                          // beta
            ::testing::Values(gtint_t(1)),                                   // stride size for x
            ::testing::Values(gtint_t(1)),                                   // stride size for y
            ::testing::Values(gtint_t(0), gtint_t(5)),                       // increment to the leading dim of a
            ::testing::Values(ELEMENT_TYPE)                                  // i : integer, f : float  datatype type tested
        ),
        ::zhemvTestPrint()
    );
