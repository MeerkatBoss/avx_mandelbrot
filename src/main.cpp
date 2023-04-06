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

    RenderScene scene = {};
    RenderConfig config = {
        .window_width = window_size_x,
        .window_height = window_size_y,
        .quality = QUALITY_MEDIUM
    };

    int init_status = render_scene_init(&scene, &config);
    LOG_ASSERT_ERROR(init_status == 0, return 1,
                    "Failed to initialize scene", NULL);

    run_main_loop(&scene);

    render_scene_dispose(&scene);
}
