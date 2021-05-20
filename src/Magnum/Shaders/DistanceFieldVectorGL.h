#ifndef Magnum_Shaders_DistanceFieldVectorGL_h
#define Magnum_Shaders_DistanceFieldVectorGL_h
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

/** @file
 * @brief Class @ref Magnum::Shaders::DistanceFieldVectorGL, typedef @ref Magnum::Shaders::DistanceFieldVectorGL2D, @ref Magnum::Shaders::DistanceFieldVectorGL3D
 * @m_since_latest
 */

#include "Magnum/DimensionTraits.h"
#include "Magnum/GL/AbstractShaderProgram.h"
#include "Magnum/Shaders/GenericGL.h"
#include "Magnum/Shaders/visibility.h"

namespace Magnum { namespace Shaders {

namespace Implementation {
    enum class DistanceFieldVectorGLFlag: UnsignedByte {
        TextureTransformation = 1 << 0,
        #ifndef MAGNUM_TARGET_GLES2
        UniformBuffers = 1 << 1
        #endif
    };
    typedef Containers::EnumSet<DistanceFieldVectorGLFlag> DistanceFieldVectorGLFlags;
}

/**
@brief Distance field vector OpenGL shader
@m_since_latest

Renders vector graphics in a form of signed distance field. See
@ref TextureTools::DistanceField for more information and @ref VectorGL for a
simpler variant of this shader. Note that the final rendered outlook will
greatly depend on radius of input distance field and value passed to
@ref setSmoothness(). You need to provide @ref Position and
@ref TextureCoordinates attributes in your triangle mesh and call at least
@ref bindVectorTexture(). By default, the shader renders the distance field
texture with a white color in an identity transformation, use
@ref setTransformationProjectionMatrix(), @ref setColor() and others to
configure the shader.

Alpha / transparency is supported by the shader implicitly, but to have it
working on the framebuffer, you need to enable
@ref GL::Renderer::Feature::Blending and set up the blending function. See
@ref GL::Renderer::setBlendFunction() for details.

@image html shaders-distancefieldvector.png width=256px

@section Shaders-DistanceFieldVectorGL-usage Example usage

Common mesh setup:

@snippet MagnumShaders-gl.cpp DistanceFieldVectorGL-usage1

Common rendering setup:

@snippet MagnumShaders-gl.cpp DistanceFieldVectorGL-usage2

@see @ref shaders, @ref DistanceFieldVectorGL2D, @ref DistanceFieldVectorGL3D
@todo Use fragment shader derivations to have proper smoothness in perspective/
    large zoom levels, make it optional as it might have negative performance
    impact
*/
template<UnsignedInt dimensions> class MAGNUM_SHADERS_EXPORT DistanceFieldVectorGL: public GL::AbstractShaderProgram {
    public:
        /**
         * @brief Vertex position
         *
         * @ref shaders-generic "Generic attribute",
         * @ref Magnum::Vector2 "Vector2" in 2D, @ref Magnum::Vector3 "Vector3"
         * in 3D.
         */
        typedef typename GenericGL<dimensions>::Position Position;

        /**
         * @brief 2D texture coordinates
         *
         * @ref shaders-generic "Generic attribute",
         * @ref Magnum::Vector2 "Vector2".
         */
        typedef typename GenericGL<dimensions>::TextureCoordinates TextureCoordinates;

        enum: UnsignedInt {
            /**
             * Color shader output. @ref shaders-generic "Generic output",
             * present always. Expects three- or four-component floating-point
             * or normalized buffer attachment.
             */
            ColorOutput = GenericGL<dimensions>::ColorOutput
        };

        #ifdef DOXYGEN_GENERATING_OUTPUT
        /**
         * @brief Flag
         * @m_since{2020,06}
         *
         * @see @ref Flags, @ref flags()
         */
        enum class Flag: UnsignedByte {
            /**
             * Enable texture coordinate transformation.
             * @see @ref setTextureMatrix()
             * @m_since{2020,06}
             */
            TextureTransformation = 1 << 0,

            #ifndef MAGNUM_TARGET_GLES2
            /**
             * Use uniform buffers. Expects that uniform data are supplied via
             * @ref bindTransformationProjectionBuffer(),
             * @ref bindDrawBuffer(), @ref bindTextureTransformationBuffer(),
             * and @ref bindMaterialBuffer() instead of direct uniform setters.
             * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
             * @requires_gles30 Uniform buffers are not available in OpenGL ES
             *      2.0.
             * @requires_webgl20 Uniform buffers are not available in WebGL
             *      1.0.
             * @m_since_latest
             */
            UniformBuffers = 1 << 1
            #endif
        };

        /**
         * @brief Flags
         * @m_since{2020,06}
         *
         * @see @ref flags()
         */
        typedef Containers::EnumSet<Flag> Flags;
        #else
        /* Done this way to be prepared for possible future diversion of 2D
           and 3D flags (e.g. introducing 3D-specific features) */
        typedef Implementation::DistanceFieldVectorGLFlag Flag;
        typedef Implementation::DistanceFieldVectorGLFlags Flags;
        #endif

        /**
         * @brief Constructor
         * @param flags     Flags
         *
         * While this function is meant mainly for the classic uniform
         * scenario (without @ref Flag::UniformBuffers set), it's equivalent to
         * @ref DistanceFieldVectorGL(Flags, UnsignedInt, UnsignedInt) with
         * @p materialCount and @p drawCount set to @cpp 1 @ce.
         */
        explicit DistanceFieldVectorGL(Flags flags = {});

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Construct for a multi-draw scenario
         * @param flags         Flags
         * @param materialCount Size of a @ref DistanceFieldVectorMaterialUniform
         *      buffer bound with @ref bindMaterialBuffer()
         * @param drawCount     Size of a @ref TransformationProjectionUniform2D
         *      / @ref TransformationProjectionUniform3D /
         *      @ref DistanceFieldVectorDrawUniform /
         *      @ref TextureTransformationUniform buffer bound with
         *      @ref bindTransformationProjectionBuffer(), @ref bindDrawBuffer()
         *      and @ref bindTextureTransformationBuffer()
         *
         * If @p flags contains @ref Flag::UniformBuffers, @p materialCount and
         * @p drawCount describe the uniform buffer sizes as these are required
         * to have a statically defined size. The draw offset is then set via
         * @ref setDrawOffset() and the per-draw materials are specified via
         * @ref DistanceFieldVectorDrawUniform::materialId.
         *
         * If @p flags don't contain @ref Flag::UniformBuffers,
         * @p materialCount and @p drawCount is ignored and the constructor
         * behaves the same as @ref DistanceFieldVectorGL(Flags).
         * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
         * @requires_gles30 Uniform buffers are not available in OpenGL ES 2.0.
         * @requires_webgl20 Uniform buffers are not available in WebGL 1.0.
         */
        /** @todo this constructor will eventually need to have also joint
            count, per-vertex weight count, view count for multiview and clip
            plane count ... and putting them in arbitrary order next to each
            other is too error-prone, so it needs some other solution
            (accepting pairs of parameter type and value like in GL context
            creation, e.g., which will probably need a new enum as reusing Flag
            for this might be too confusing) */
        explicit DistanceFieldVectorGL(Flags flags, UnsignedInt materialCount, UnsignedInt drawCount);
        #endif

        /**
         * @brief Construct without creating the underlying OpenGL object
         *
         * The constructed instance is equivalent to a moved-from state. Useful
         * in cases where you will overwrite the instance later anyway. Move
         * another object over it to make it useful.
         *
         * This function can be safely used for constructing (and later
         * destructing) objects even without any OpenGL context being active.
         * However note that this is a low-level and a potentially dangerous
         * API, see the documentation of @ref NoCreate for alternatives.
         */
        explicit DistanceFieldVectorGL(NoCreateT) noexcept: GL::AbstractShaderProgram{NoCreate} {}

        /** @brief Copying is not allowed */
        DistanceFieldVectorGL(const DistanceFieldVectorGL<dimensions>&) = delete;

        /** @brief Move constructor */
        DistanceFieldVectorGL(DistanceFieldVectorGL<dimensions>&&) noexcept = default;

        /** @brief Copying is not allowed */
        DistanceFieldVectorGL<dimensions>& operator=(const DistanceFieldVectorGL<dimensions>&) = delete;

        /** @brief Move assignment */
        DistanceFieldVectorGL<dimensions>& operator=(DistanceFieldVectorGL<dimensions>&&) noexcept = default;

        /**
         * @brief Flags
         * @m_since{2020,06}
         */
        Flags flags() const { return _flags; }

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Material count
         * @m_since_latest
         *
         * Statically defined size of the
         * @ref DistanceFieldVectorMaterialUniform uniform buffer. Has use only
         * if @ref Flag::UniformBuffers is set.
         * @see @ref bindMaterialBuffer()
         * @requires_gles30 Not defined on OpenGL ES 2.0 builds.
         * @requires_webgl20 Not defined on WebGL 1.0 builds.
         */
        UnsignedInt materialCount() const { return _materialCount; }

        /**
         * @brief Draw count
         * @m_since_latest
         *
         * Statically defined size of each of the
         * @ref TransformationProjectionUniform2D /
         * @ref TransformationProjectionUniform3D,
         * @ref DistanceFieldVectorDrawUniform and
         * @ref TextureTransformationUniform uniform buffers. Has use only if
         * @ref Flag::UniformBuffers is set.
         * @requires_gles30 Not defined on OpenGL ES 2.0 builds.
         * @requires_webgl20 Not defined on WebGL 1.0 builds.
         */
        UnsignedInt drawCount() const { return _drawCount; }
        #endif

        /** @{
         * @name Uniform setters
         *
         * Used only if @ref Flag::UniformBuffers is not set.
         */

        /**
         * @brief Set transformation and projection matrix
         * @return Reference to self (for method chaining)
         *
         * Initial value is an identity matrix.
         *
         * Expects that @ref Flag::UniformBuffers is not set, in that case fill
         * @ref TransformationProjectionUniform2D::transformationProjectionMatrix /
         * @ref TransformationProjectionUniform3D::transformationProjectionMatrix
         * and call @ref bindTransformationProjectionBuffer() instead.
         */
        DistanceFieldVectorGL<dimensions>& setTransformationProjectionMatrix(const MatrixTypeFor<dimensions, Float>& matrix);

        /**
         * @brief Set texture coordinate transformation matrix
         * @return Reference to self (for method chaining)
         * @m_since{2020,06}
         *
         * Expects that the shader was created with
         * @ref Flag::TextureTransformation enabled. Initial value is an
         * identity matrix.
         *
         * Expects that @ref Flag::UniformBuffers is not set, in that case fill
         * @ref TextureTransformationUniform::rotationScaling and
         * @ref TextureTransformationUniform::offset and call
         * @ref bindTextureTransformationBuffer() instead.
         */
        DistanceFieldVectorGL<dimensions>& setTextureMatrix(const Matrix3& matrix);

        /**
         * @brief Set fill color
         * @return Reference to self (for method chaining)
         *
         * Initial value is @cpp 0xffffffff_rgbaf @ce.
         *
         * Expects that @ref Flag::UniformBuffers is not set, in that case fill
         * @ref DistanceFieldVectorMaterialUniform::color and call
         * @ref bindMaterialBuffer() instead.
         * @see @ref setOutlineColor()
         */
        DistanceFieldVectorGL<dimensions>& setColor(const Color4& color);

        /**
         * @brief Set outline color
         * @return Reference to self (for method chaining)
         *
         * Initial value is @cpp 0x00000000_rgbaf @ce and the outline is not
         * drawn --- see @ref setOutlineRange() for more information.
         *
         * Expects that @ref Flag::UniformBuffers is not set, in that case fill
         * @ref DistanceFieldVectorMaterialUniform::outlineColor and call
         * @ref bindMaterialBuffer() instead.
         * @see @ref setOutlineRange(), @ref setColor()
         */
        DistanceFieldVectorGL<dimensions>& setOutlineColor(const Color4& color);

        /**
         * @brief Set outline range
         * @return Reference to self (for method chaining)
         *
         * The @p start parameter describes where fill ends and possible
         * outline starts. Initial value is @cpp 0.5f @ce, larger values will
         * make the vector art look thinner, smaller will make it look thicker.
         *
         * The @p end parameter describes where outline ends. If set to a value
         * larger than @p start, the outline is not drawn. Initial value is
         * @cpp 1.0f @ce.
         *
         * Expects that @ref Flag::UniformBuffers is not set, in that case fill
         * @ref DistanceFieldVectorMaterialUniform::outlineStart and
         * @ref DistanceFieldVectorMaterialUniform::outlineEnd and call
         * @ref bindMaterialBuffer() instead.
         * @see @ref setOutlineColor()
         */
        DistanceFieldVectorGL<dimensions>& setOutlineRange(Float start, Float end);

        /**
         * @brief Set smoothness radius
         * @return Reference to self (for method chaining)
         *
         * Larger values will make edges look less aliased (but blurry),
         * smaller values will make them look more crisp (but possibly
         * aliased). Initial value is @cpp 0.04f @ce.
         *
         * Expects that @ref Flag::UniformBuffers is not set, in that case fill
         * @ref DistanceFieldVectorMaterialUniform::smoothness and call
         * @ref bindMaterialBuffer() instead.
         */
        DistanceFieldVectorGL<dimensions>& setSmoothness(Float value);

        /**
         * @}
         */

        #ifndef MAGNUM_TARGET_GLES2
        /** @{
         * @name Uniform buffer binding and related uniform setters
         *
         * Used if @ref Flag::UniformBuffers is set.
         */

        /**
         * @brief Set a draw offset
         * @return Reference to self (for method chaining)
         * @m_since_latest
         *
         * Specifies which item in the @ref TransformationProjectionUniform2D /
         * @ref TransformationProjectionUniform3D,
         * @ref DistanceFieldVectorDrawUniform and
         * @ref TextureTransformationUniform buffers bound with
         * @ref bindTransformationProjectionBuffer(), @ref bindDrawBuffer() and
         * @ref bindTextureTransformationBuffer() should be used for current
         * draw. Expects that @ref Flag::UniformBuffers is set and @p offset is
         * less than @ref drawCount(). Initial value is @cpp 0 @ce.
         * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
         * @requires_gles30 Uniform buffers are not available in OpenGL ES 2.0.
         * @requires_webgl20 Uniform buffers are not available in WebGL 1.0.
         */
        DistanceFieldVectorGL<dimensions>& setDrawOffset(UnsignedInt offset);

        /**
         * @brief Set a transformation and projection uniform buffer
         * @return Reference to self (for method chaining)
         * @m_since_latest
         *
         * Expects that @ref Flag::UniformBuffers is set. The buffer is
         * expected to contain @ref drawCount() instances of
         * @ref TransformationProjectionUniform2D /
         * @ref TransformationProjectionUniform3D. At the very least you need
         * to call also @ref bindDrawBuffer() and @ref bindMaterialBuffer().
         * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
         * @requires_gles30 Uniform buffers are not available in OpenGL ES 2.0.
         * @requires_webgl20 Uniform buffers are not available in WebGL 1.0.
         */
        DistanceFieldVectorGL<dimensions>& bindTransformationProjectionBuffer(GL::Buffer& buffer);
        /**
         * @overload
         * @m_since_latest
         */
        DistanceFieldVectorGL<dimensions>& bindTransformationProjectionBuffer(GL::Buffer& buffer, GLintptr offset, GLsizeiptr size);

        /**
         * @brief Set a draw uniform buffer
         * @return Reference to self (for method chaining)
         * @m_since_latest
         *
         * Expects that @ref Flag::UniformBuffers is set. The buffer is
         * expected to contain @ref drawCount() instances of
         * @ref DistanceFieldVectorDrawUniform. At the very least you need to
         * call also @ref bindTransformationProjectionBuffer() and
         * @ref bindMaterialBuffer().
         * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
         * @requires_gles30 Uniform buffers are not available in OpenGL ES 2.0.
         * @requires_webgl20 Uniform buffers are not available in WebGL 1.0.
         */
        DistanceFieldVectorGL<dimensions>& bindDrawBuffer(GL::Buffer& buffer);
        /**
         * @overload
         * @m_since_latest
         */
        DistanceFieldVectorGL<dimensions>& bindDrawBuffer(GL::Buffer& buffer, GLintptr offset, GLsizeiptr size);

        /**
         * @brief Set a texture transformation uniform buffer
         * @return Reference to self (for method chaining)
         * @m_since_latest
         *
         * Expects that both @ref Flag::UniformBuffers and
         * @ref Flag::TextureTransformation is set. The buffer is expected to
         * contain @ref drawCount() instances of
         * @ref TextureTransformationUniform.
         * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
         * @requires_gles30 Uniform buffers are not available in OpenGL ES 2.0.
         * @requires_webgl20 Uniform buffers are not available in WebGL 1.0.
         */
        DistanceFieldVectorGL<dimensions>& bindTextureTransformationBuffer(GL::Buffer& buffer);
        /**
         * @overload
         * @m_since_latest
         */
        DistanceFieldVectorGL<dimensions>& bindTextureTransformationBuffer(GL::Buffer& buffer, GLintptr offset, GLsizeiptr size);

        /**
         * @brief Set a material uniform buffer
         * @return Reference to self (for method chaining)
         * @m_since_latest
         *
         * Expects that @ref Flag::UniformBuffers is set. The buffer is
         * expected to contain @ref materialCount() instances of
         * @ref DistanceFieldVectorMaterialUniform. At the very least you need
         * to call also @ref bindTransformationProjectionBuffer() and
         * @ref bindDrawBuffer().
         * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
         * @requires_gles30 Uniform buffers are not available in OpenGL ES 2.0.
         * @requires_webgl20 Uniform buffers are not available in WebGL 1.0.
         */
        DistanceFieldVectorGL<dimensions>& bindMaterialBuffer(GL::Buffer& buffer);
        /**
         * @overload
         * @m_since_latest
         */
        DistanceFieldVectorGL<dimensions>& bindMaterialBuffer(GL::Buffer& buffer, GLintptr offset, GLsizeiptr size);

        /**
         * @}
         */
        #endif

        /** @{
         * @name Texture binding
         */

        /**
         * @brief Bind vector texture
         * @return Reference to self (for method chaining)
         *
         * @see @ref DistanceFieldVectorGL::Flag::TextureTransformation,
         *      @ref VectorGL::Flag::TextureTransformation,
         *      @ref DistanceFieldVectorGL::setTextureMatrix(),
         *      @ref VectorGL::setTextureMatrix()
         */
        DistanceFieldVectorGL<dimensions>& bindVectorTexture(GL::Texture2D& texture);

        /**
         * @}
         */

    private:
        /* Prevent accidentally calling irrelevant functions */
        #ifndef MAGNUM_TARGET_GLES
        using GL::AbstractShaderProgram::drawTransformFeedback;
        #endif
        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        using GL::AbstractShaderProgram::dispatchCompute;
        #endif

        Flags _flags;
        #ifndef MAGNUM_TARGET_GLES2
        UnsignedInt _materialCount{}, _drawCount{};
        #endif
        Int _transformationProjectionMatrixUniform{0},
            _textureMatrixUniform{1},
            _colorUniform{2},
            _outlineColorUniform{3},
            _outlineRangeUniform{4},
            _smoothnessUniform{5};
        #ifndef MAGNUM_TARGET_GLES2
        /* Used instead of all other uniforms when Flag::UniformBuffers is set,
           so it can alias them */
        Int _drawOffsetUniform{0};
        #endif
};

/**
@brief Two-dimensional distance field vector OpenGL shader
@m_since_latest
*/
typedef DistanceFieldVectorGL<2> DistanceFieldVectorGL2D;

/**
@brief Three-dimensional distance field vector OpenGL shader
@m_since_latest
*/
typedef DistanceFieldVectorGL<3> DistanceFieldVectorGL3D;

#ifdef DOXYGEN_GENERATING_OUTPUT
/** @debugoperatorclassenum{DistanceFieldVectorGL,DistanceFieldVectorGL::Flag} */
template<UnsignedInt dimensions> Debug& operator<<(Debug& debug, DistanceFieldVectorGL<dimensions>::Flag value);

/** @debugoperatorclassenum{DistanceFieldVectorGL,DistanceFieldVectorGL::Flags} */
template<UnsignedInt dimensions> Debug& operator<<(Debug& debug, DistanceFieldVectorGL<dimensions>::Flags value);
#else
namespace Implementation {
    MAGNUM_SHADERS_EXPORT Debug& operator<<(Debug& debug, DistanceFieldVectorGLFlag value);
    MAGNUM_SHADERS_EXPORT Debug& operator<<(Debug& debug, DistanceFieldVectorGLFlags value);
    CORRADE_ENUMSET_OPERATORS(DistanceFieldVectorGLFlags)
}
#endif

}}

#endif
