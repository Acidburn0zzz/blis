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

#include "blis.h"
#include <dlfcn.h>
#include "level1/ref_axpbyv.h"

namespace testinghelpers {

#if !defined(REF_IS_OPENBLAS) || !defined(REF_IS_MKL)
template<typename T>
void ref_axpbyv( char conj_x, gtint_t n, T alpha, const T* x,
                    gtint_t incx, T beta, T* y, gtint_t incy )
{
    using scalar_t = std::conditional_t<testinghelpers::type_info<T>::is_complex, T&, T>;
    typedef void (*Fptr_ref_cblas_scal)( f77_int, scalar_t , const T *, f77_int);
    Fptr_ref_cblas_scal ref_cblas_scal;

    // Call C function
    /* Check the typename T passed to this function template and call respective function.*/
    if (typeid(T) == typeid(float))
    {
        ref_cblas_scal = (Fptr_ref_cblas_scal)dlsym(refCBLASModule.get(), "cblas_sscal");
    }
    else if (typeid(T) == typeid(double))
    {
        ref_cblas_scal = (Fptr_ref_cblas_scal)dlsym(refCBLASModule.get(), "cblas_dscal");
    }
    else if (typeid(T) == typeid(scomplex))
    {
        ref_cblas_scal = (Fptr_ref_cblas_scal)dlsym(refCBLASModule.get(), "cblas_cscal");
    }
    else if (typeid(T) == typeid(dcomplex))
    {
        ref_cblas_scal = (Fptr_ref_cblas_scal)dlsym(refCBLASModule.get(), "cblas_zscal");
    }
    else
    {
        throw std::runtime_error("Error in ref_axpby.cpp: Invalid typename is passed function template.");
    }
    if (!ref_cblas_scal) {
        throw std::runtime_error("Error in ref_axpby.cpp: Function pointer == 0 -- symbol not found.");
    }

    ref_cblas_scal( n, beta, y, incy );
    typedef void (*Fptr_ref_cblas_axpby)( f77_int, scalar_t , const T *, f77_int , T *, f77_int );
    Fptr_ref_cblas_axpby ref_cblas_axpby;

    // Call C function
    /* Check the typename T passed to this function template and call respective function.*/
    if (typeid(T) == typeid(float))
    {
        ref_cblas_axpby = (Fptr_ref_cblas_axpby)dlsym(refCBLASModule.get(), "cblas_saxpy");
    }
    else if (typeid(T) == typeid(double))
    {
        ref_cblas_axpby = (Fptr_ref_cblas_axpby)dlsym(refCBLASModule.get(), "cblas_daxpy");
    }
    else if (typeid(T) == typeid(scomplex))
    {
        ref_cblas_axpby = (Fptr_ref_cblas_axpby)dlsym(refCBLASModule.get(), "cblas_caxpy");
    }
    else if (typeid(T) == typeid(dcomplex))
    {
        ref_cblas_axpby = (Fptr_ref_cblas_axpby)dlsym(refCBLASModule.get(), "cblas_zaxpy");
    }
    else
    {
        throw std::runtime_error("Error in ref_axpby.cpp: Invalid typename is passed function template.");
    }
    if (!ref_cblas_axpby) {
        throw std::runtime_error("Error in ref_axpby.cpp: Function pointer == 0 -- symbol not found.");
    }
#ifdef TEST_BLIS_TYPED
    if( chkconj( conj_x ) )
    {
        std::vector<T> X( testinghelpers::buff_dim(n, incx) );
        memcpy( X.data(), x, testinghelpers::buff_dim(n, incx)*sizeof(T) );
        testinghelpers::conj<T>( X.data(), n, incx );
        ref_cblas_axpby( n, alpha, X.data(), incx, y, incy );
    }
    else
#endif
    {
        ref_cblas_axpby( n, alpha, x, incx, y, incy );
    }

}
#else
template<typename T>
void ref_axpbyv( char conj_x, gtint_t n, T alpha, const T* x,
                    gtint_t incx, T beta, T* y, gtint_t incy ) {

    using scalar_t = std::conditional_t<testinghelpers::type_info<T>::is_complex, T&, T>;
    typedef void (*Fptr_ref_cblas_axpby)( f77_int, scalar_t , const T *, f77_int , scalar_t, T *, f77_int );
    Fptr_ref_cblas_axpby ref_cblas_axpby;

    // Call C function
    /* Check the typename T passed to this function template and call respective function.*/
    if (typeid(T) == typeid(float))
    {
        ref_cblas_axpby = (Fptr_ref_cblas_axpby)dlsym(refCBLASModule.get(), "cblas_saxpby");
    }
    else if (typeid(T) == typeid(double))
    {
        ref_cblas_axpby = (Fptr_ref_cblas_axpby)dlsym(refCBLASModule.get(), "cblas_daxpby");
    }
    else if (typeid(T) == typeid(scomplex))
    {
        ref_cblas_axpby = (Fptr_ref_cblas_axpby)dlsym(refCBLASModule.get(), "cblas_caxpby");
    }
    else if (typeid(T) == typeid(dcomplex))
    {
        ref_cblas_axpby = (Fptr_ref_cblas_axpby)dlsym(refCBLASModule.get(), "cblas_zaxpby");
    }
    else
    {
        throw std::runtime_error("Error in ref_axpby.cpp: Invalid typename is passed function template.");
    }
    if (!ref_cblas_axpby) {
        throw std::runtime_error("Error in ref_axpby.cpp: Function pointer == 0 -- symbol not found.");
    }
#ifdef TEST_BLIS_TYPED
    if( chkconj( conj_x ) )
    {
        std::vector<T> X( testinghelpers::buff_dim(n, incx) );
        memcpy( X.data(), x, testinghelpers::buff_dim(n, incx)*sizeof(T) );
        testinghelpers::conj<T>( X.data(), n, incx );
        ref_cblas_axpby( n, alpha, X.data(), incx, beta, y, incy );
    }
    else
#endif
    {
        ref_cblas_axpby( n, alpha, x, incx, beta, y, incy );
    }
}
#endif

// Explicit template instantiations
template void ref_axpbyv<float>(char, gtint_t, float, const float*, gtint_t, float, float*, gtint_t);
template void ref_axpbyv<double>(char, gtint_t, double, const double*, gtint_t, double, double*, gtint_t);
template void ref_axpbyv<scomplex>(char, gtint_t, scomplex, const scomplex*, gtint_t, scomplex, scomplex*, gtint_t);
template void ref_axpbyv<dcomplex>(char, gtint_t, dcomplex, const dcomplex*, gtint_t, dcomplex, dcomplex*, gtint_t);

} //end of namespace testinghelpers
