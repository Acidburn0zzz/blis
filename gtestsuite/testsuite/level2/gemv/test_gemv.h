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

#pragma once

#include "gemv.h"
#include "level2/ref_gemv.h"
#include "inc/check_error.h"
#include <stdexcept>
#include <algorithm>

template<typename T>

void test_gemv( char storage, char trnsa, char conjx, gtint_t m, gtint_t n,
    T alpha, gtint_t lda_inc, gtint_t incx, T beta, gtint_t incy,
    double thresh, char datatype ) {

    // Compute the leading dimensions for matrix size calculation.
    gtint_t lda = testinghelpers::get_leading_dimension(storage, 'n', m, n, lda_inc);

    // Get correct vector lengths.
    gtint_t lenx = ( testinghelpers::chknotrans( trnsa ) ) ? n : m ;
    gtint_t leny = ( testinghelpers::chknotrans( trnsa ) ) ? m : n ;

    //----------------------------------------------------------
    //        Initialize matrics with random integer numbers.
    //----------------------------------------------------------
    std::vector<T> a = testinghelpers::get_random_matrix<T>(1, 5, storage, 'n', m, n, lda, datatype);
    std::vector<T> x = testinghelpers::get_random_vector<T>(1, 3, lenx, incx, datatype);
    std::vector<T> y = testinghelpers::get_random_vector<T>(1, 3, leny, incy, datatype);

    // Create a copy of c so that we can check reference results.
    std::vector<T> y_ref(y);
    //----------------------------------------------------------
    //                  Call BLIS function
    //----------------------------------------------------------
    gemv( storage, trnsa, conjx, m, n, &alpha, a.data(), lda,
                         x.data(), incx, &beta, y.data(), incy );

    //----------------------------------------------------------
    //                  Call reference implementation.
    //----------------------------------------------------------
    testinghelpers::ref_gemv( storage, trnsa, conjx, m, n, alpha, a.data(),
                         lda, x.data(), incx, beta, y_ref.data(), incy );

    //----------------------------------------------------------
    //              check component-wise error.
    //----------------------------------------------------------
    computediff<T>( leny, y.data(), y_ref.data(), incy, thresh );
}
