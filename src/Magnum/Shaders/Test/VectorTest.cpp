/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021 Vladimír Vondruš <mosra@centrum.cz>

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/

#include <new>
#include <Corrade/TestSuite/Tester.h>

#include "Magnum/Shaders/Vector.h"

namespace Magnum { namespace Shaders { namespace Test { namespace {

struct VectorTest: TestSuite::Tester {
    explicit VectorTest();

    template<class T> void uniformSize();

    void drawUniformConstructDefault();
    void drawUniformConstructNoInit();
    void drawUniformSetters();
};

VectorTest::VectorTest() {
    addTests({&VectorTest::uniformSize<VectorDrawUniform>,

              &VectorTest::drawUniformConstructDefault,
              &VectorTest::drawUniformConstructNoInit,
              &VectorTest::drawUniformSetters});
}

using namespace Math::Literals;

template<class> struct UniformTraits;
template<> struct UniformTraits<VectorDrawUniform> {
    static const char* name() { return "VectorDrawUniform"; }
};

template<class T> void VectorTest::uniformSize() {
    setTestCaseTemplateName(UniformTraits<T>::name());

    CORRADE_FAIL_IF(sizeof(T) % sizeof(Vector4) != 0, sizeof(T) << "is not a multiple of vec4 for UBO alignment.");

    /* 48-byte structures are fine, we'll align them to 768 bytes and not
       256, but warn about that */
    CORRADE_FAIL_IF(768 % sizeof(T) != 0, sizeof(T) << "can't fit exactly into 768-byte UBO alignment.");
    if(256 % sizeof(T) != 0)
        CORRADE_WARN(sizeof(T) << "can't fit exactly into 256-byte UBO alignment, only 768.");
}

void VectorTest::drawUniformConstructDefault() {
    VectorDrawUniform a;
    VectorDrawUniform b{DefaultInit};
    CORRADE_COMPARE(a.color, 0xffffffff_rgbaf);
    CORRADE_COMPARE(b.color, 0xffffffff_rgbaf);
    CORRADE_COMPARE(a.backgroundColor, 0x00000000_rgbaf);
    CORRADE_COMPARE(b.backgroundColor, 0x00000000_rgbaf);

    constexpr VectorDrawUniform ca;
    constexpr VectorDrawUniform cb{DefaultInit};
    CORRADE_COMPARE(ca.color, 0xffffffff_rgbaf);
    CORRADE_COMPARE(cb.color, 0xffffffff_rgbaf);
    CORRADE_COMPARE(ca.backgroundColor, 0x00000000_rgbaf);
    CORRADE_COMPARE(cb.backgroundColor, 0x00000000_rgbaf);

    CORRADE_VERIFY(std::is_nothrow_default_constructible<VectorDrawUniform>::value);
    CORRADE_VERIFY(std::is_nothrow_constructible<VectorDrawUniform, DefaultInitT>::value);

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!std::is_convertible<DefaultInitT, VectorDrawUniform>::value);
}

void VectorTest::drawUniformConstructNoInit() {
    /* Testing only some fields, should be enough */
    VectorDrawUniform a;
    a.color = 0x354565fc_rgbaf;
    a.backgroundColor = 0x98769facb_rgbaf;

    new(&a) VectorDrawUniform{NoInit};
    CORRADE_COMPARE(a.color, 0x354565fc_rgbaf);
    CORRADE_COMPARE(a.backgroundColor, 0x98769facb_rgbaf);

    CORRADE_VERIFY(std::is_nothrow_constructible<VectorDrawUniform, NoInitT>::value);

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!std::is_convertible<NoInitT, VectorDrawUniform>::value);
}

void VectorTest::drawUniformSetters() {
    VectorDrawUniform a;
    a.setColor(0x354565fc_rgbaf)
     .setBackgroundColor(0x98769facb_rgbaf);
    CORRADE_COMPARE(a.color, 0x354565fc_rgbaf);
    CORRADE_COMPARE(a.backgroundColor, 0x98769facb_rgbaf);
}

}}}}

CORRADE_TEST_MAIN(Magnum::Shaders::Test::VectorTest)
