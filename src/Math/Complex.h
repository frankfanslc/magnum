#ifndef Magnum_Math_Complex_h
#define Magnum_Math_Complex_h
/*
    Copyright © 2010, 2011, 2012 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Magnum.

    Magnum is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Magnum is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
*/

/** @file
 * @brief Class Magnum::Math::Complex
 */

#include <Utility/Debug.h>

#include "Math/MathTypeTraits.h"

#include "magnumVisibility.h"

namespace Magnum { namespace Math {

/**
@brief %Complex number
@tparam T   Data type

Represents 2D rotation.
*/
template<class T> class Complex {
    public:
        typedef T Type;                         /**< @brief Underlying data type */

        /**
         * @brief Default constructor
         *
         * @f[
         *      c = 0 + i0
         * @f]
         */
        inline constexpr /*implicit*/ Complex(): _real(T(0)), _imaginary(T(0)) {}

        /**
         * @brief Construct complex from real and imaginary part
         *
         * @f[
         *      c = a + ib
         * @f]
         */
        inline constexpr /*implicit*/ Complex(T real, T imaginary = T(0)): _real(real), _imaginary(imaginary) {}

        /** @brief Equality comparison */
        inline bool operator==(const Complex<T>& other) const {
            return MathTypeTraits<T>::equals(_real, other._real) &&
                   MathTypeTraits<T>::equals(_imaginary, other._imaginary);
        }

        /** @brief Non-equality comparison */
        inline bool operator!=(const Complex<T>& other) const {
            return !operator==(other);
        }

        /** @brief Real part */
        inline constexpr T real() const { return _real; }

        /** @brief Imaginary part */
        inline constexpr T imaginary() const { return _imaginary; }

        /**
         * @brief Add and assign complex
         *
         * The computation is done in-place. @f[
         *      c_0 + c_1 = a_0 + a_1 + i(b_0 + b_1)
         * @f]
         */
        inline Complex<T>& operator+=(const Complex<T>& other) {
            _real += other._real;
            _imaginary += other._imaginary;
            return *this;
        }

        /**
         * @brief Add complex
         *
         * @see operator+=()
         */
        inline Complex<T> operator+(const Complex<T>& other) const {
            return Complex<T>(*this) += other;
        }

        /**
         * @brief Negated complex
         *
         * @f[
         *      -c = -a -ib
         * @f]
         */
        inline Complex<T> operator-() const {
            return {-_real, -_imaginary};
        }

        /**
         * @brief Subtract and assign complex
         *
         * The computation is done in-place. @f[
         *      c_0 - c_1 = a_0 - a_1 + i(b_0 - b_1)
         * @f]
         */
        inline Complex<T>& operator-=(const Complex<T>& other) {
            _real -= other._real;
            _imaginary -= other._imaginary;
            return *this;
        }

        /**
         * @brief Subtract complex
         *
         * @see operator-=()
         */
        inline Complex<T> operator-(const Complex<T>& other) const {
            return Complex<T>(*this) -= other;
        }

        /**
         * @brief Multiply with scalar and assign
         *
         * The computation is done in-place. @f[
         *      c \cdot t = ta + itb
         * @f]
         */
        inline Complex<T>& operator*=(T scalar) {
            _real *= scalar;
            _imaginary *= scalar;
            return *this;
        }

        /**
         * @brief Multiply with scalar
         *
         * @see operator*=(T)
         */
        inline Complex<T> operator*(T scalar) const {
            return Complex<T>(*this) *= scalar;
        }

        /**
         * @brief Divide with scalar and assign
         *
         * The computation is done in-place. @f[
         *      \frac c t = \frac a t + i \frac b t
         * @f]
         */
        inline Complex<T>& operator/=(T scalar) {
            _real /= scalar;
            _imaginary /= scalar;
            return *this;
        }

        /**
         * @brief Divide with scalar
         *
         * @see operator/=(T)
         */
        inline Complex<T> operator/(T scalar) const {
            return Complex<T>(*this) /= scalar;
        }

    private:
        T _real, _imaginary;
};

/** @relates Complex
@brief Multiply scalar with complex

Same as Complex::operator*(T) const.
*/
template<class T> inline Complex<T> operator*(T scalar, const Complex<T>& complex) {
    return complex*scalar;
}

/** @relates Complex
@brief Divide complex with number and invert

@f[
    \frac t c = \frac t a + i \frac t b
@f]
@see Complex::operator/()
*/
template<class T> inline Complex<T> operator/(T scalar, const Complex<T>& complex) {
    return {scalar/complex.real(), scalar/complex.imaginary()};
}

/** @debugoperator{Magnum::Math::Complex} */
template<class T> Corrade::Utility::Debug operator<<(Corrade::Utility::Debug debug, const Complex<T>& value) {
    debug << "Complex(";
    debug.setFlag(Corrade::Utility::Debug::SpaceAfterEachValue, false);
    debug << value.real() << ", " << value.imaginary() << ")";
    debug.setFlag(Corrade::Utility::Debug::SpaceAfterEachValue, true);
    return debug;
}

/* Explicit instantiation for commonly used types */
#ifndef DOXYGEN_GENERATING_OUTPUT
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const Complex<float>&);
#ifndef MAGNUM_TARGET_GLES
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const Complex<double>&);
#endif
#endif

}}

#endif
