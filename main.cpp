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
#define MAXITER 30


// draw Julia/Mandelbrot set
std::vector<sf::Color> fractal (std::vector<sf::Color> im, float left, float top, float xside, float yside, float cx, float cy)
{
    // determine scaling based on window dimensions
    float xscale = xside / WIDTH;
    float yscale = yside / HEIGHT; // keep aspect ratio of fractal

    // computate each pixel and corresponding complex number (x + yi)
    #pragma omp parallel for // parallelisation using OpenMP
    for (int y=0; y<HEIGHT; y++) {
        for (int x=0; x<WIDTH; x++) {
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
                // optimised to speed up calculations
                tempx = zx2 - zy2 + cx;
                zy = 2.0f*zx*zy + cy;
                zx = tempx;
                zx2 = zx*zx;
                zy2 = zy*zy;
                count++;
            }

            // mapping iteration count to colour
            sf::Color colour;
            if (count == MAXITER) { // black if in the set
                colour = sf::Color::Black;
            } else { // distance from set
                colour = sf::Color(10*count, 5*count, 15*count); 
            }
            im[y*WIDTH + x] = colour;
        }
    }
    return im;
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

    // create blank image
    std::vector<sf::Color> pixels(WIDTH*HEIGHT);
    sf::Image image({WIDTH, HEIGHT}, sf::Color::Black);
    sf::Texture texture;
    sf::Sprite sprite(texture);

    // draw mandelbrot set
    pixels = fractal(pixels, left, top, xside, yside, cx, cy);
    for (uint32_t y = 0; y < HEIGHT; ++y) {
        for (uint32_t x = 0; x < WIDTH; ++x) {
            image.setPixel({x,y}, pixels[y*WIDTH + x]);
        }
    }
    texture.loadFromImage(image);
    sprite.setTexture(texture, true);

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

                // compute fractal
                pixels = fractal(pixels, left, top, xside, yside, cx, cy);
                for (uint32_t y = 0; y < HEIGHT; ++y) {
                    for (uint32_t x = 0; x < WIDTH; ++x) {
                        image.setPixel({x,y}, pixels[y*WIDTH + x]);
                    }
                }
                texture.loadFromImage(image);
                sprite.setTexture(texture, true);
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