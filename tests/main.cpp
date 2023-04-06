#include <stdio.h>
#include <stdlib.h>

#include "mandelbrot/calculations.h"

#include "helpers/test_macros.h"

struct test_args
{
    const ScreenState* state;
    PixelColor* pixels;
};

#define EXPAND_ARGS(args) (args.state, args.pixels)
#define ADAPTER(function) function EXPAND_ARGS

int main()
{
    const ScreenState state = {
        .size_x_px = 1024,
        .size_y_px = 720,
        .unit_len_px = 300,

        .max_iterations = 256,
        .escape_radius = 10.f,

        .scale = { 1, 1 },
        .center = { 0, 0 }
    };

    const size_t sample_size = 500;
    size_t* test_data = (size_t*) calloc(sample_size, sizeof(*test_data));
    PixelColor* pixels = (PixelColor*) calloc(state.size_x_px*state.size_y_px,
                                                sizeof(*pixels));
    const test_args args = { .state = &state, .pixels = pixels };

    COLLECT_DATA(ADAPTER(calculate_pixels_simple), args,
                    test_data, sample_size);

    const double simple_average = get_average_time(test_data, sample_size);
    const double simple_stddev  = get_time_standard_deviation(simple_average,
                                                        test_data, sample_size);

    COLLECT_DATA(ADAPTER(calculate_pixels_optimized), args,
                    test_data, sample_size);

    const double opt_average = get_average_time(test_data, sample_size);
    const double opt_stddev  = get_time_standard_deviation(opt_average,
                                                        test_data, sample_size);

    const double simple_rel_error = simple_stddev / simple_average;
    const double opt_rel_error    = opt_stddev    / opt_average;
    const double combined_rel_error = simple_rel_error + opt_rel_error;
    const double faster = simple_average / opt_average;
    const double faster_err = faster * combined_rel_error;


    printf("without SIMD: %.2lfms (~%.2lfms)\n", simple_average, simple_stddev);
    printf("   with SIMD: %.2lfms (~%.2lfms)\n", opt_average, opt_stddev);
    puts("");
    printf("Performance increase: %.2lf (~%.2lf)\n", faster, faster_err);
    
    free(test_data);
    free(pixels);

}
