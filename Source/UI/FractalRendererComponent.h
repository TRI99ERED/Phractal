/*
  ==============================================================================

    FractalRendererComponent.h
    Created: 18 Mar 2025 4:37:05pm
    Author:  tri99er

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class FractalRendererComponent  : public juce::Component, public juce::OpenGLRenderer
{
public:
    FractalRendererComponent();
    ~FractalRendererComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    void newOpenGLContextCreated() override;
    void renderOpenGL() override;
    void openGLContextClosing() override;

private:

    juce::OpenGLContext openGLContext;

    struct Vertex
    {
        float position[2];
        float colour[4];
    };

    std::vector<Vertex> vertexBuffer;
    std::vector<unsigned int> indexBuffer;

    GLuint vbo; // Vertex buffer object.
    GLuint ibo; // Index buffer object.

    juce::String vertexShader;
    juce::String fragmentShader;

    std::unique_ptr<juce::OpenGLShaderProgram> shaderProgram;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FractalRendererComponent)
};
