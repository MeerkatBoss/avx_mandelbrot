#include <immintrin.h>
#include "calculations.h"

union m512i
{
    int scalars[16];
    __m512i vector;
};

void calculate_pixels_optimized(const ScreenState* screen, PixelColor* pixels)
{
    // const float radius_sq = screen->escape_radius * screen->escape_radius;
    const float r_tmp = screen->escape_radius * screen->escape_radius;
    const __m512 radius_sq = _mm512_set1_ps(r_tmp);


    const float delta_x = screen->scale.x / screen->unit_len_px;
    const float delta_y = screen->scale.y / screen->unit_len_px;
    const __m512 vec_delta_x = _mm512_set1_ps(delta_x * 16);
    const __m512 vec_delta_y = _mm512_set1_ps(delta_y);

    const float start_x = screen->center.x
                        - (float) screen->size_x_px / (2 * screen->unit_len_px);
    const float start_y = screen->center.y
                        + (float) screen->size_y_px / (2 * screen->unit_len_px);

    const __m512 vec_start_x = _mm512_set_ps(
            start_x +  0*delta_x, start_x +  1*delta_x,
            start_x +  2*delta_x, start_x +  3*delta_x,
            start_x +  4*delta_x, start_x +  5*delta_x,
            start_x +  6*delta_x, start_x +  7*delta_x,
            start_x +  8*delta_x, start_x +  9*delta_x,
            start_x + 10*delta_x, start_x + 11*delta_x,
            start_x + 12*delta_x, start_x + 13*delta_x,
            start_x + 14*delta_x, start_x + 15*delta_x);

    const __m512 vec_start_y = _mm512_set1_ps(start_y);

    __m512 cur_x = vec_start_x;
    __m512 cur_y = vec_start_y;

    const __m512i max_iterations = _mm512_set1_epi32(
                                        (int) screen->max_iterations);

    PixelColor* line = pixels;
    for (size_t row = 0;
         row < screen->size_y_px;
         row++, line += screen->size_x_px)
    {
        cur_x = vec_start_x;
        for (size_t col = 0;
             col < screen->size_x_px;
             col += 16)
        {
            const __m512 x0 = cur_x;
            const __m512 y0 = cur_y;
            
            __m512 x = x0;
            __m512 y = y0;

            __m512i it = _mm512_set1_epi32(0);
            __m512i increment = _mm512_set1_epi32(1);
            __mmask16 it_mask = _cvtu32_mask16(0xFFFF & (-1));
            for (;;)
            {
                __mmask16 cmp_mask = _mm512_mask_cmp_epi32_mask(it_mask,
                                                it, max_iterations, 1); // LT
                it_mask = _mm512_kand(it_mask, cmp_mask);

                if (!_mm512_mask2int(it_mask)) break; // No counters left

                it = _mm512_mask_add_epi32(it, it_mask, it, increment);

                const __m512 x2 = _mm512_mul_ps(x, x);
                const __m512 y2 = _mm512_mul_ps(y, y);
                const __m512 xy = _mm512_mul_ps(x, y);
                
                __m512 dist_sq = _mm512_add_ps(x2, y2);

                cmp_mask = _mm512_cmp_ps_mask(dist_sq, radius_sq, 1); // LT

                it_mask = _mm512_kand(it_mask, cmp_mask);
                if (!_mm512_mask2int(it_mask)) break; // All outside

                x = _mm512_sub_ps(x2, y2);
                x = _mm512_add_ps(x,  x0);

                y = _mm512_add_ps(xy, xy);
                y = _mm512_add_ps(y,  y0);
            }

            __mmask16 cmp_mask = _mm512_cmp_epi32_mask(
                                    it, max_iterations, 0); // EQ

            // set to 0 all not escaped
            it = _mm512_mask_set1_epi32(it, cmp_mask, 0);

            __m512 color_base = _mm512_div_ps(
                    _mm512_cvt_roundepi32_ps(it,
                        _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC),
                    _mm512_cvt_roundepi32_ps(max_iterations,
                        _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC));

            __m512 base_root = _mm512_sqrt_ps(color_base);
            __m512 base_pow2 = _mm512_mul_ps(
                                    color_base,
                                    color_base);
            
            __m512 red   = _mm512_sub_ps(color_base, base_pow2);
            __m512 green = _mm512_sub_ps(base_root,
                                _mm512_mul_ps(base_root, color_base));
            __m512 blue  = _mm512_sqrt_ps(
                                _mm512_sub_ps(base_root, color_base));
            
            red   = _mm512_mul_ps(red,   _mm512_set1_ps(4*255));
            green = _mm512_mul_ps(green, _mm512_set1_ps(255));
            blue  = _mm512_mul_ps(blue,  _mm512_set1_ps(255));

            m512i red_int   = { .vector = _mm512_cvt_roundps_epi32(red,
                                _MM_FROUND_TO_NEAREST_INT |_MM_FROUND_NO_EXC) };
            m512i green_int = { .vector = _mm512_cvt_roundps_epi32(green,
                                _MM_FROUND_TO_NEAREST_INT |_MM_FROUND_NO_EXC) };
            m512i blue_int  = { .vector = _mm512_cvt_roundps_epi32(blue,
                                _MM_FROUND_TO_NEAREST_INT |_MM_FROUND_NO_EXC) };

            for (size_t i = 0; i < 16; i++)
                line[col + i] = {
                    .red   = (uint8_t) (red_int  .scalars[15 - i]),
                    .green = (uint8_t) (green_int.scalars[15 - i]),
                    .blue  = (uint8_t) (blue_int .scalars[15 - i]),
                    .alpha = (uint8_t) (255)
                };
            
            cur_x = _mm512_add_ps(cur_x, vec_delta_x);
        }
        cur_y = _mm512_sub_ps(cur_y, vec_delta_y);
    }

}

