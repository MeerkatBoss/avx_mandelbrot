/**
 * @file definitions.h
 * @author MeerkatBoss (solodovnikov.ia@phystech.edu)
 * 
 * @brief Definition of project common structures
 *
 * @version 0.1
 * @date 2023-04-02
 *
 * @copyright Copyright MeerkatBoss (c) 2023
 */
#ifndef __MANDELBROT_DEFINITIONS_H
#define __MANDELBROT_DEFINITIONS_H

struct PixelColor
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t alpha;
};

struct Vector2D
{
    float x;
    float y;
};

struct ScreenState
{
    size_t size_x_px;           /* Screen width in pixels */
    size_t size_y_px;           /* Screen height in pixels */
    float  unit_len_px;         /* Length of unit vector in pixels */

    unsigned max_iterations;    /* Max number of iterations to be performed */
    float    escape_radius;     /* Distance to 'infinitely far` points */

    Vector2D scale;             /* Scaling coefficients */
    Vector2D center;            /* Center coordinates */
};

enum RenderQuality
{
    QUALITY_LOW    = 128,
    QUALITY_MEDIUM = 256,
    QUALITY_HIGH   = 512
};

struct RenderConfig
{
    size_t window_width;
    size_t window_height;

    RenderQuality quality;
};

#endif /* definitions.h */
