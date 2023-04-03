#include <SFML/Graphics.hpp>

#include <assert.h>
#include <stdio.h>

#include "logger/logger.h"
#include "mandelbrot/calculations.h"
#include "display/display.h"

static const size_t window_size_x = 1024;
static const size_t window_size_y = 720;

int main()
{
    add_default_file_logger();
    add_logger({
        .name = "Console logger",
        .stream = stderr,
        .logging_level = LOG_ERROR,
        .settings_mask = LGS_USE_ESCAPE | LGS_KEEP_OPEN
    });

    char buffer[16] = "";

    RenderScene scene = {};
    RenderConfig config = {
        .window_width = window_size_x,
        .window_height = window_size_y,
        .quality = QUALITY_MEDIUM
    };

    int init_status = sfml_scene_init(&scene, &config);
    LOG_ASSERT_ERROR(init_status == 0, return 1,
                    "Failed to initialize scene", NULL);
    sf::Clock clock;
    while (scene.window.isOpen())
    {
        sf::Event event;
        while (scene.window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                scene.window.close();
        }

        scene.window.clear(sf::Color::White);
        float timeDelta = clock.restart().asSeconds();
        snprintf(buffer, 16, "%.1f FPS", 1.f/timeDelta);
        scene.fps_text.setString(buffer);

        calculate_pixels_optimized(&scene.screen, scene.texture_pixels);
        scene.mandelbrot_texture.update(
                        (const sf::Uint8*) scene.texture_pixels);

        scene.window.draw(scene.mandelbrot_sprite);
        scene.window.draw(scene.fps_text);
        scene.window.display();
    }

    free(scene.texture_pixels);
}
