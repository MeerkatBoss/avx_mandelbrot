#include <math.h>

#include "logger/logger.h"
#include "mandelbrot/calculations.h"

#include "display.h"

#define FONTNAME "LobsterTwo-BoldItalic"

int render_scene_init(RenderScene* scene, const RenderConfig* config)
{
    new (&scene->window) sf::RenderWindow(
            sf::VideoMode(
                (unsigned) config->window_width,
                (unsigned) config->window_height),
            "Mandelbrot Set",
            sf::Style::Titlebar | sf::Style::Close);

    scene->mandelbrot_texture.create((unsigned) config->window_width,
                                     (unsigned) config->window_height);

    scene->mandelbrot_sprite.setTexture(scene->mandelbrot_texture);

    LOG_ASSERT_ERROR(
        scene->fps_font.loadFromFile("assets/" FONTNAME ".ttf"),
        return -1,
        "Failed to load font '" FONTNAME "'", NULL); 

    scene->fps_text.setFont(scene->fps_font);
    scene->fps_text.setFillColor(sf::Color::White);

    new (&scene->screen) ScreenState {
        .size_x_px = config->window_width,
        .size_y_px = config->window_height,
        .unit_len_px = (float) config->window_height / 2.5e-4f,

        .max_iterations = (size_t) config->quality,
        .escape_radius = 3.f,

        .scale = { 1, 1 },
        .center = { -1.7812f, 0 }
    };

    scene->texture_pixels = (PixelColor*) calloc(
            config->window_height*config->window_width,
            sizeof(*scene->texture_pixels));

    LOG_ASSERT_ERROR(scene->texture_pixels, return -1,
                    "Failed to allocate memory", NULL);
    
    return 0;
}

void render_scene_dispose(RenderScene* scene)
{
    free(scene->texture_pixels);
    scene->texture_pixels = NULL;
}

void run_main_loop(RenderScene* scene)
{
    char buffer[16] = "";

    sf::Clock clock;
    while (scene->window.isOpen())
    {
        sf::Event event;
        while (scene->window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                scene->window.close();
        }

        scene->window.clear(sf::Color::White);
        float timeDelta = clock.restart().asSeconds();
        snprintf(buffer, 16, "%.1f FPS", 1.f/timeDelta);
        scene->fps_text.setString(buffer);

        calculate_pixels_optimized(&scene->screen, scene->texture_pixels);
        scene->mandelbrot_texture.update(
                        (const sf::Uint8*) scene->texture_pixels);

        scene->window.draw(scene->mandelbrot_sprite);
        scene->window.draw(scene->fps_text);

        if (scene->screen.unit_len_px > scene->screen.size_y_px / 3.f)
        {
            float pow = expf(timeDelta);
            scene->screen.unit_len_px *= 1.f/pow;  /* TODO: You literally have
                                                        scene->screen.scale
                                                        for this exact purpose*/ 
        }
        else
        {
            scene->screen.unit_len_px = scene->screen.size_y_px / 2.5e-4f;
        }

        scene->window.display();
    }
}

