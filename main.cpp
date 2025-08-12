// 30.05.25
// Charlot Eberlein
// inspired by this video: https://youtu.be/nr8biZfSZ3Y?si=WUcSGoKV80Fr1rsS (6:47)

// namespaces
#include <SFML/Graphics.hpp>
#include <cstdint>
#include <complex>
#include <optional>
#include <iostream>
#include <omp.h> // parallelisation using OpenMP
// constants
#define WIDTH 600
#define HEIGHT 750
#define MAXITER 20


// draw julia set
void fractal (std::vector<uint8_t>& raw_px, const std::vector<sf::Color>& palette, float left, float top, float xside, float yside, float cx, float cy)
{
    // determine scaling based on window dimensions
    float xscale = xside / WIDTH;
    float yscale = yside / HEIGHT; // keep aspect ratio of fractal

    #pragma omp parallel for // parallelisation using OpenMP
    for (int i=0; i<WIDTH*HEIGHT; i++) {
        // flattened 2D for loop
        int x = i % WIDTH;
        int y = i / WIDTH;
        // complex, z - not necessarily centered at (0,0)
        float zx = x * xscale + left; // re
        float zy = y * yscale + top; // im

        float tempx;

        int count = 0;
        // cache squared values for speed
        float zx2 = zx*zx;
        float zy2 = zy*zy;
        while ((zx2 + zy2 < 4.0f) && (count<MAXITER)) {
            // recurrence relationship: z = z*z + c
            // optimised to speed up calculations by not doing complex arithmetic
            tempx = zx2 - zy2 + cx;
            zy = 2.0f*zx*zy + cy;
            zx = tempx;
            zx2 = zx*zx;
            zy2 = zy*zy;
            count++;
        }

        // mapping iteration count to colour
        int j = 4 * (y * WIDTH + x);
        // use pre-computed look-up table
        auto& c = palette[count];
        raw_px[j] = c.r;
        raw_px[j + 1] = c.g;
        raw_px[j + 2] = c.b;
        raw_px[j + 3] = 255;
    }
}


// plotting the fractal
int main()
{
    // create window
    sf::RenderWindow window(sf::VideoMode({WIDTH,HEIGHT}), "Plotting Julia sets with C++");

    // default viewpoint parameters
    float left = -1.5f;
    float top = -1.5f;
    float xside = 3.0f;
    float yside = 3.0f * HEIGHT/WIDTH;
    float cx = -0.8f;
    float cy = 0.156f;

    // create look-up table for colour palette
    std::vector<sf::Color> palette(MAXITER+1);
    for (int i = 0; i <= MAXITER; ++i) {
        if (i == MAXITER) palette[i] = sf::Color::Black;
        else palette[i] = sf::Color(10*i,5*i,15*i);
    }

    // create pixel buffer & raw pixel data buffer
    std::vector<uint8_t> raw_px(WIDTH*HEIGHT*4);
    // compute initial fractal
    fractal(raw_px, palette, left, top, xside, yside, cx, cy);

    // initialise texture and sprite
    sf::Texture texture(sf::Vector2u(WIDTH,HEIGHT));
    sf::Sprite sprite(texture);
    // update initial texture
    texture.update(raw_px.data(), sf::Vector2u(WIDTH,HEIGHT), sf::Vector2u(0,0));

    // main program loop
    while (window.isOpen()) {
        while (const std::optional<sf::Event> event = window.pollEvent()) {
            // exit window when closing
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            if (const auto* e = (event->getIf<sf::Event::MouseMoved>())) {
                int mouseX = static_cast<int>(e->position.x);
                int mouseY = static_cast<int>(e->position.y);
                float cx = left + (mouseX/(float)WIDTH) * xside;
                float cy = top + (mouseY/(float)HEIGHT) * yside;

                // compute new fractal
                fractal(raw_px, palette, left, top, xside, yside, cx, cy);
                // update texture
                texture.update(raw_px.data(), sf::Vector2u(WIDTH,HEIGHT), sf::Vector2u(0,0));
            }
        }

        // clear previous state
        window.clear();
        // draw sprite on screen
        window.draw(sprite);
        // update window
        window.display();
    }
    return 0;
}