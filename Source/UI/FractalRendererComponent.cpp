/*
  ==============================================================================

    FractalRendererComponent.cpp
    Created: 18 Mar 2025 4:37:05pm
    Author:  tri99er

  ==============================================================================
*/

#include <JuceHeader.h>
#include "FractalRendererComponent.h"

//==============================================================================
FractalRendererComponent::FractalRendererComponent()
{
    // Indicates that no part of this Component is transparent.
    setOpaque(true);

    // Set this instance as the renderer for the context.
    openGLContext.setRenderer(this);

    // Tell the context to repaint on a loop.
    openGLContext.setContinuousRepainting(true);

    // Finally - we attach the context to this Component.
    openGLContext.attachTo(*this);
}

FractalRendererComponent::~FractalRendererComponent()
{
    // Tell the context to stop using this Component.
    openGLContext.detach();
}

void FractalRendererComponent::paint (juce::Graphics& g)
{
}

void FractalRendererComponent::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}

void FractalRendererComponent::newOpenGLContextCreated()
{
    // Generate 1 buffer, using our vbo variable to store its ID.
    openGLContext.extensions.glGenBuffers(1, &vbo);

    // Generate 1 more buffer, this time using our IBO variable.
    openGLContext.extensions.glGenBuffers(1, &ibo);

    // Create 4 vertices each with a different colour.
    vertexBuffer = {
        // Vertex 0
        {
            { -1.f, 1.f },        // (-1, 1)
            { 1.f, 0.f, 0.f, 1.f }  // Red
        },
        // Vertex 1
        {
            { 1.f, 1.f },         // (1, 1)
            { 1.f, 0.5f, 0.f, 1.f } // Orange
        },
        // Vertex 2
        {
            { 1.f, -1.f },        // (1, -1)
            { 1.f, 1.f, 0.f, 1.f }  // Yellow
        },
        // Vertex 3
        {
            { -1.f, -1.f },       // (-1, -1)
            { 1.f, 0.f, 1.f, 1.f }  // Purple
        }
    };

    // We need 6 indices, 1 for each corner of the two triangles.
    indexBuffer = {
        0, 1, 2,
        0, 2, 3
    };

    // Bind the VBO.
    openGLContext.extensions.glBindBuffer(juce::gl::GL_ARRAY_BUFFER, vbo);

    // Send the vertices data.
    openGLContext.extensions.glBufferData(
        juce::gl::GL_ARRAY_BUFFER,                        // The type of data we're sending.           
        sizeof(Vertex) * vertexBuffer.size(),   // The size (in bytes) of the data.
        vertexBuffer.data(),                    // A pointer to the actual data.
        juce::gl::GL_STATIC_DRAW                          // How we want the buffer to be drawn.
    );

    // Bind the IBO.
    openGLContext.extensions.glBindBuffer(juce::gl::GL_ELEMENT_ARRAY_BUFFER, ibo);

    // Send the indices data.
    openGLContext.extensions.glBufferData(
        juce::gl::GL_ELEMENT_ARRAY_BUFFER,
        sizeof(unsigned int) * indexBuffer.size(),
        indexBuffer.data(),
        juce::gl::GL_STATIC_DRAW
    );

    vertexShader =
        R"(
            #version 400 compatibility
            uniform vec2 iResolution;

            void main() {
                gl_Position = vec4(gl_Vertex.xy, 0.0, 1.0);
            }
        )";

    fragmentShader =
        R"(
            #version 400 compatibility
            #extension GL_ARB_gpu_shader_fp64 : enable
            #pragma optionNV(fastmath off)
            #pragma optionNV(fastprecision off)

            #define FLOAT float
            #define VEC2 vec2
            #define VEC3 vec3
            #define AA_LEVEL 1
            #define ESCAPE 1000.0
            #define PI 3.141592653

            #define FLAG_DRAW_MSET ((iFlags & 0x01) == 0x01)
            #define FLAG_DRAW_JSET ((iFlags & 0x02) == 0x02)
            #define FLAG_USE_COLOR ((iFlags & 0x04) == 0x04)

            uniform vec2 iResolution;
            uniform vec2 iCam;
            uniform vec2 iJulia;
            uniform float iZoom;
            uniform int iType;
            uniform int iIters;
            uniform int iFlags;
            uniform int iTime;

            #define cx_one VEC2(1.0, 0.0)
            VEC2 cx_mul(VEC2 a, VEC2 b) {
                return VEC2(a.x*b.x - a.y*b.y, a.x*b.y + a.y*b.x);
            }
            VEC2 cx_sqr(VEC2 a) {
                FLOAT x2 = a.x*a.x;
                FLOAT y2 = a.y*a.y;
                FLOAT xy = a.x*a.y;
                return VEC2(x2 - y2, xy + xy);
            }
            VEC2 cx_cube(VEC2 a) {
                FLOAT x2 = a.x*a.x;
                FLOAT y2 = a.y*a.y;
                FLOAT d = x2 - y2;
                return VEC2(a.x*(d - y2 - y2), a.y*(x2 + x2 + d));
            }
            VEC2 cx_div(VEC2 a, VEC2 b) {
                FLOAT denom = 1.0 / (b.x*b.x + b.y*b.y);
                return VEC2(a.x*b.x + a.y*b.y, a.y*b.x - a.x*b.y) * denom;
            }
            VEC2 cx_sin(VEC2 a) {
                return VEC2(sin(a.x) * cosh(a.y), cos(a.x) * sinh(a.y));
            }
            VEC2 cx_cos(VEC2 a) {
                return VEC2(cos(a.x) * cosh(a.y), -sin(a.x) * sinh(a.y));
            }
            VEC2 cx_exp(VEC2 a) {
                return exp(a.x) * VEC2(cos(a.y), sin(a.y));
            }

            //Fractal equations
            VEC2 mandelbrot(VEC2 z, VEC2 c) {
                return cx_sqr(z) + c;
            }
            VEC2 burning_ship(VEC2 z, VEC2 c) {
                return VEC2(z.x*z.x - z.y*z.y, 2.0*abs(z.x * z.y)) + c;
            }
            VEC2 feather(VEC2 z, VEC2 c) {
                return cx_div(cx_cube(z), cx_one + z*z) + c;
            }
            VEC2 sfx(VEC2 z, VEC2 c) {
                return z * dot(z,z) - cx_mul(z, c*c);
            }
            VEC2 henon(VEC2 z, VEC2 c) {
                return VEC2(1.0 - c.x*z.x*z.x + z.y, c.y * z.x);
            }
            VEC2 duffing(VEC2 z, VEC2 c) {
                return VEC2(z.y, -c.y*z.x + c.x*z.y - z.y*z.y*z.y);
            }
            VEC2 ikeda(VEC2 z, VEC2 c) {
                FLOAT t = 0.4 - 6.0/(1.0 + dot(z,z));
                FLOAT st = sin(t);
                FLOAT ct = cos(t);
                return VEC2(1.0 + c.x*(z.x*ct - z.y*st), c.y*(z.x*st + z.y*ct));
            }
            VEC2 chirikov(VEC2 z, VEC2 c) {
                z.y += c.y*sin(z.x);
                z.x += c.x*z.y;
                return z;
            }

            #if 1
            #define DO_LOOP(name) \
                for (i = 0; i < iIters; ++i) { \
                    VEC2 ppz = pz; \
                    pz = z; \
                    z = name(z, c); \
                    if (dot(z, z) > ESCAPE) { break; } \
                    sumz.x += dot(z - pz, pz - ppz); \
                    sumz.y += dot(z - pz, z - pz); \
                    sumz.z += dot(z - ppz, z - ppz); \
                }
            #else
            #define DO_LOOP(name) \
                for (i = 0; i < iIters; ++i) { \
                    z = name(z, c); \
                    if (dot(z, z) > ESCAPE) { break; } \
                }
            #endif

            vec3 fractal(VEC2 z, VEC2 c) {
                VEC2 pz = z;
                VEC3 sumz = VEC3(0.0, 0.0, 0.0);
                int i;
                switch (iType) {
                case 0: DO_LOOP(mandelbrot); break;
                case 1: DO_LOOP(burning_ship); break;
                case 2: DO_LOOP(feather); break;
                case 3: DO_LOOP(sfx); break;
                case 4: DO_LOOP(henon); break;
                case 5: DO_LOOP(duffing); break;
                case 6: DO_LOOP(ikeda); break;
                case 7: DO_LOOP(chirikov); break;
                }

                if (i != iIters) {
                    float n1 = sin(float(i) * 0.1) * 0.5 + 0.5;
                    float n2 = cos(float(i) * 0.1) * 0.5 + 0.5;
                    return vec3(n1, n2, 1.0) * (1.0 - float(FLAG_USE_COLOR)*0.85);
                } else if (FLAG_USE_COLOR) {
                    sumz = abs(sumz) / iIters;
                    vec3 n1 = sin(abs(sumz * 5.0)) * 0.45 + 0.5;
                    return n1;
                } else {
                    return vec3(0.0, 0.0, 0.0);
                }
            }

            float rand(float s) {
                return fract(sin(s*12.9898) * 43758.5453);
            }

            void main() {
	            //Get normalized screen coordinate
	            vec2 screen_pos = gl_FragCoord.xy - (iResolution.xy * 0.5);

                vec3 col = vec3(0.0, 0.0, 0.0);
                for (int i = 0; i < AA_LEVEL; ++i) {
                    vec2 dxy = vec2(rand(i*0.54321 + iTime), rand(i*0.12345 + iTime));
                    VEC2 c = VEC2((screen_pos + dxy) * vec2(1.0, -1.0) / iZoom - iCam);

                    if (FLAG_DRAW_MSET) {
                        col += fractal(c, c);
                    }
                    if (FLAG_DRAW_JSET) {
                        col += fractal(c, iJulia);
                    }
                }

                col /= AA_LEVEL;
                if (FLAG_DRAW_MSET && FLAG_DRAW_JSET) {
                    col *= 0.5;
                }
                gl_FragColor = vec4(clamp(col, 0.0, 1.0), 1.0 / (iTime + 1.0));
            }
        )";

    // Create an instance of OpenGLShaderProgram
    shaderProgram.reset(new juce::OpenGLShaderProgram(openGLContext));

    // Compile and link the shader.
    // Each of these methods will return false if something goes wrong so we'll
    // wrap them in an if statement
    if (shaderProgram->addVertexShader(vertexShader)
        && shaderProgram->addFragmentShader(fragmentShader)
        && shaderProgram->link())
    {
        // No compilation errors - set the shader program to be active
        shaderProgram->use();
    }
    else
    {
        // Oops - something went wrong with our shaders!
        // Check the output window of your IDE to see what the error might be.
        jassertfalse;
    }

    juce::OpenGLShaderProgram::Uniform uResolution(*shaderProgram, "iResolution");
    uResolution.set(1280.f, 500.f);

    juce::OpenGLShaderProgram::Uniform uCam(*shaderProgram, "iCam");
    uCam.set(0.f, 0.f);

    juce::OpenGLShaderProgram::Uniform uJulia(*shaderProgram, "iJulia");
    uJulia.set(0.f, 0.f);

    juce::OpenGLShaderProgram::Uniform uZoom(*shaderProgram, "iZoom");
    uZoom.set(100.f);

    juce::OpenGLShaderProgram::Uniform uType(*shaderProgram, "iType");
    uType.set(0);

    juce::OpenGLShaderProgram::Uniform uIters(*shaderProgram, "iIters");
    uIters.set(1200);

    juce::OpenGLShaderProgram::Uniform uFlags(*shaderProgram, "iFlags");
    uFlags.set(0x01);

    juce::OpenGLShaderProgram::Uniform uTime(*shaderProgram, "iTime");
    uTime.set(juce::Time::getCurrentTime().getSeconds());
}

void FractalRendererComponent::renderOpenGL()
{
    // Clear the screen by filling it with black.
    juce::OpenGLHelpers::clear(juce::Colours::black);

    // Tell the renderer to use this shader program
    shaderProgram->use();

    juce::OpenGLShaderProgram::Uniform uResolution(*shaderProgram, "iResolution");
    uResolution.set(1280.f, 500.f);

    juce::OpenGLShaderProgram::Uniform uCam(*shaderProgram, "iCam");
    uCam.set(0.f, 0.f);

    juce::OpenGLShaderProgram::Uniform uJulia(*shaderProgram, "iJulia");
    uJulia.set(0.f, 0.f);

    juce::OpenGLShaderProgram::Uniform uZoom(*shaderProgram, "iZoom");
    uZoom.set(100.f);

    juce::OpenGLShaderProgram::Uniform uType(*shaderProgram, "iType");
    uType.set(0);

    juce::OpenGLShaderProgram::Uniform uIters(*shaderProgram, "iIters");
    uIters.set(1200);

    juce::OpenGLShaderProgram::Uniform uFlags(*shaderProgram, "iFlags");
    uFlags.set(0x01);

    juce::OpenGLShaderProgram::Uniform uTime(*shaderProgram, "iTime");
    uTime.set(juce::Time::getCurrentTime().getSeconds());

    openGLContext.extensions.glBindBuffer(juce::gl::GL_ARRAY_BUFFER, vbo);
    openGLContext.extensions.glBindBuffer(juce::gl::GL_ELEMENT_ARRAY_BUFFER, ibo);

    // Enable the position attribute.
    openGLContext.extensions.glVertexAttribPointer(
        0,              // The attribute's index (AKA location).
        2,              // How many values this attribute contains.
        juce::gl::GL_FLOAT,       // The attribute's type (float).
        juce::gl::GL_FALSE,       // Tells OpenGL NOT to normalise the values.
        sizeof(Vertex), // How many bytes to move to find the attribute with
        // the same index in the next vertex.
        nullptr         // How many bytes to move from the start of this vertex
                        // to find this attribute (the default is 0 so we just
                        // pass nullptr here).
    );
    openGLContext.extensions.glEnableVertexAttribArray(0);

    // Enable to colour attribute.
    openGLContext.extensions.glVertexAttribPointer(
        1,                              // This attribute has an index of 1
        4,                              // This time we have four values for the
        // attribute (r, g, b, a)
        juce::gl::GL_FLOAT,
        juce::gl::GL_FALSE,
        sizeof(Vertex),
        (GLvoid*)(sizeof(float) * 2)    // This attribute comes after the
        // position attribute in the Vertex
        // struct, so we need to skip over the
        // size of the position array to find
        // the start of this attribute.
    );
    openGLContext.extensions.glEnableVertexAttribArray(1);

    juce::gl::glDrawElements(
        juce::gl::GL_TRIANGLES,       // Tell OpenGL to render triangles.
        indexBuffer.size(), // How many indices we have.
        juce::gl::GL_UNSIGNED_INT,    // What type our indices are.
        nullptr             // We already gave OpenGL our indices so we don't
                            // need to pass that again here, so pass nullptr.
    );

    openGLContext.extensions.glDisableVertexAttribArray(0);
    openGLContext.extensions.glDisableVertexAttribArray(1);
}

void FractalRendererComponent::openGLContextClosing()
{
}
