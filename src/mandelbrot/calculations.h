/**
 * @file calculations.h
 * @author MeerkatBoss (solodovnikov.ia@phystech.edu)
 * 
 * @brief Calculation functions for Mandelbrot set
 *
 * @version 0.1
 * @date 2023-03-30
 *
 * @copyright Copyright MeerkatBoss (c) 2023
 */
#ifndef __MANDELBROT_CALCULATIONS_H
#define __MANDELBROT_CALCULATIONS_H

#include <stddef.h>
#include <stdint.h>

#include "commons/definitions.h"

/**
 * @brief Calculate (inefficiently) colors of each pixel on screen
 *
 * @param[in]  screen    - Current state of screen
 * @param[out] pixels    - Pixel colors which can be used to draw on screen
 */
void calculate_pixels_simple   (const ScreenState* screen, PixelColor* pixels);

/**
 * @brief Calculate (with AVX optimizations) colors of each pixel on screen
 *
 * @param[in]  screen    - Current state of screen
 * @param[out] pixels    - Pixel colors which can be used to draw on screen
 */
void calculate_pixels_optimized(const ScreenState* screen, PixelColor* pixels);


#endif /* calculations.h */
