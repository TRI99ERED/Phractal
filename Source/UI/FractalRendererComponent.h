/*
  ==============================================================================

    FractalRendererComponent.h
    Created: 18 Mar 2025 4:37:05pm
    Author:  tri99er

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../PluginProcessor.h"

static const int target_fps = 60;
static const int sample_rate = 48000;
static const int max_freq = 4000;
static const int window_w_init = 1280;
static const int window_h_init = 720;
static const int starting_fractal = 0;
static const int max_iters = 1200;
static const double escape_radius_sq = 1000.0;
static const char window_name[] = "Fractal Sound Explorer";

void mandelbrot(float& x, float& y, float cx, float cy);
void burning_ship(float& x, float& y, float cx, float cy);
void feather(float& x, float& y, float cx, float cy);
void sfx(float& x, float& y, float cx, float cy);
void henon(float& x, float& y, float cx, float cy);
void duffing(float& x, float& y, float cx, float cy);
void ikeda(float& x, float& y, float cx, float cy);
void chirikov(float& x, float& y, float cx, float cy);

typedef void (*Fractal)(float&, float&, float, float);
static Fractal fractal = nullptr;

static const Fractal all_fractals[] = {
    mandelbrot,
    burning_ship,
    feather,
    sfx,
    henon,
    duffing,
    ikeda,
    chirikov,
};

//==============================================================================
/*
*/
class FractalRendererComponent  : public juce::Component, public juce::OpenGLRenderer
{
public:
    FractalRendererComponent(const PhractalAudioProcessor& pap);
    ~FractalRendererComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    void newOpenGLContextCreated() override;
    void renderOpenGL() override;
    void openGLContextClosing() override;

    void mouseMove(const juce::MouseEvent& event) override;
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseUp(const juce::MouseEvent& event) override;
    void mouseWheelMove(const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel) override;
    bool keyPressed(const juce::KeyPress& key) override;
    bool keyStateChanged(bool isKeyDown) override;

    void ScreenToPt(int x, int y, float& px, float& py) {
        px = float(x - getLocalBounds().getWidth() / 2) / cam_zoom - cam_x;
        py = float(y - getLocalBounds().getHeight() / 2) / cam_zoom - cam_y;
    }
    void PtToScreen(float px, float py, int& x, int& y) {
        x = int(cam_zoom * (px + cam_x)) + getLocalBounds().getWidth() / 2;
        y = int(cam_zoom * (py + cam_y)) + getLocalBounds().getHeight() / 2;
    }

    void SetFractal(int type) {
        jx = jy = 1e8;
        fractal = all_fractals[type];
        normalized = (type == 0);
        hide_orbit = true;
        frame = 0;
    }

private:
    const PhractalAudioProcessor& audioProcessor;

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

    juce::Point<int> mousePos;
    float cam_x = 0.0;
    float cam_y = 0.0;
    float cam_zoom = 100.0;
    int cam_x_fp = 0;
    int cam_y_fp = 0;
    float cam_x_dest = cam_x;
    float cam_y_dest = cam_y;
    float cam_zoom_dest = cam_zoom;
    bool normalized = true;
    bool use_color = false;
    bool hide_orbit = true;
    float jx = 1e8;
    float jy = 1e8;
    int frame = 0;

    float px, py, orbit_x, orbit_y;
    bool leftPressed = false;
    bool dragging = false;
    bool juliaDrag = false;
    juce::Point<float> prevDrag;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FractalRendererComponent)
};
