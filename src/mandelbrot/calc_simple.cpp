#include <math.h>

#include "calculations.h"

void calculate_pixels_simple(const ScreenState* screen, PixelColor* pixels)
{
    const float radius_sq = screen->escape_radius * screen->escape_radius;

    const float delta_x = screen->scale.x / screen->unit_len_px;
    const float delta_y = screen->scale.y / screen->unit_len_px;

    const float start_x = screen->center.x
                        - (float) screen->size_x_px / (2 * screen->unit_len_px);
    const float start_y = screen->center.y
                        + (float) screen->size_y_px / (2 * screen->unit_len_px);

    float cur_x = start_x;
    float cur_y = start_y;

    const size_t max_iterations = screen->max_iterations;

    PixelColor* line = pixels;
    for (size_t row = 0;
         row < screen->size_y_px;
         row++, cur_y -= delta_y, line += screen->size_x_px)
    {
        cur_x = start_x;
        for (size_t col = 0;
             col < screen->size_x_px;
             col++, cur_x += delta_x)
        {
            const float x0 = cur_x;
            const float y0 = cur_y;
            
            float x = x0;
            float y = y0;
            unsigned it = 0;
            while (it++ < max_iterations)
            {
                const float x2 = x*x;
                const float y2 = y*y;
                const float xy = x*y;
                
                if (x2 + y2 >= radius_sq) break;

                x = x2 - y2 + x0;
                y = 2*xy    + y0;
            }

            if (it > max_iterations) it = 0;       // Didn't escape

            float color_base = (float) it / (float) max_iterations;
            float base_root = sqrt(color_base);
            float base_pow2 = color_base * color_base;

            line[col] = {
                .red   = (uint8_t)(int)(4*255 * (color_base - base_pow2)),
                .green = (uint8_t)(int)(255 * (base_root - base_root*color_base)),
                .blue  = (uint8_t)(int)(255 * sqrt(base_root - color_base)),
                .alpha = (uint8_t)      255
            };
        }
    }
}
