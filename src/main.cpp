#include <SFML/Graphics.hpp>

#include <assert.h>
#include <stdio.h>

#include "mandelbrot/calculations.h"

static const size_t window_size_x = 1024;
static const size_t window_size_y = 720;

int main()
{
    char buffer[16] = "";
 // TODO: Extract some of SFML logic from here
    sf::RenderWindow window(
            sf::VideoMode(window_size_x, window_size_y),
            "Mandelbrot Set",
            sf::Style::Titlebar | sf::Style::Close);

    sf::Texture texture;
    texture.create(window_size_x, window_size_y);
    sf::Sprite sprite;
    sprite.setTexture(texture);

    sf::Font font;
    font.loadFromFile("assets/LobsterTwo-BoldItalic.ttf");
    sf::Text fps;
    fps.setFont(font);
    fps.setFillColor(sf::Color::White);

    ScreenState state = {
        .size_x_px = window_size_x,
        .size_y_px = window_size_y,
        .unit_len_px = 600.f,

        .max_iterations = 256,
        .escape_radius = 10.f,

        .scale = { 1, 1 },
        .center = { -0.7f, 0 }
    };

    PixelColor* pixels = NULL;
    posix_memalign((void**)&pixels, BUFFER_ALIGNMENT,
                    window_size_x * window_size_y * sizeof(*pixels));
    sf::Clock clock;
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear(sf::Color::White);
        float timeDelta = clock.restart().asSeconds();
        snprintf(buffer, 16, "%.1f FPS", 1.f/timeDelta);
        fps.setString(buffer);

        calculate_pixels_optimized(&state, pixels);
        texture.update((const sf::Uint8*) pixels);

        window.draw(sprite);
        window.draw(fps);
        window.display();
    }

    free(pixels);
}
