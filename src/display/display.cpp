#include "logger/logger.h"

#include "display.h"

#define FONTNAME "LobsterTwo-BoldItalic"

int sfml_scene_init(RenderScene* scene, const RenderConfig* config)
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
        .unit_len_px = (float) config->window_height / 2.5f,

        .max_iterations = (size_t) config->quality,
        .escape_radius = 10.f,

        .scale = { 1, 1 },
        .center = { -0.7f, 0 }
    };

    scene->texture_pixels = (PixelColor*) calloc(
            config->window_height*config->window_width,
            sizeof(*scene->texture_pixels));

    LOG_ASSERT_ERROR(scene->texture_pixels, return -1,
                    "Failed to allocate memory", NULL);
    
    return 0;
}

