#include "DrawFunctions.h"

/// <summary> Draws a digit on the screen. </summary>
/// <param name="x"> X coordinate of the top left corner of the digit. </param>
/// <param name="y"> Y coordinate of the top left corner of the digit. </param>
/// <param name="digit"> Digit to draw (from 0 to 9). </param>
/// <param name="color"> Digit color. </param>
void draw_digit(int x, int y, int digit, uint32_t color) {
    if (digit < 0 || digit > 9) return;

    for (int row = 0; row < 10; row++) {
        uint8_t bits = font[digit][row]; // Getting the bit mask for the current row of the digit
        for (int col = 0; col < 8; col++) {
            if (bits & (1 << (7 - col))) {
                int px = x + col * 2;     // Increase the width
                int py = y + row * 2;     // Increase the height

                // Drawing an enlarged pixel (2x2)
                for (int dy = 0; dy < 2; dy++) {
                    for (int dx = 0; dx < 2; dx++) {
                        int drawX = px + dx;
                        int drawY = py + dy;

                        // Checking screen bounds
                        if (drawX >= 0 && drawX < SCREEN_WIDTH && drawY >= 0 && drawY < SCREEN_HEIGHT) {
                            buffer[drawY][drawX] = color;
                        }
                    }
                }
            }
        }
    }
}

/// <summary> Draws a number on the screen. </summary>
/// <param name="x"> X coordinate of the top left corner of the number. </param>
/// <param name="y"> Y coordinate of the top left corner of the number. </param>
/// <param name="number"> Number to draw. </param>
/// <param name="color"> Number color. </param>
void draw_number(int x, int y, int number, uint32_t color) {
    char str[12];
    snprintf(str, sizeof(str), "%d", number);

    int dx = 0;
    for (char* p = str; *p; p++) {
        draw_digit(x + dx, y, *p - '0', color);
        dx += 20; // Increase the distance between digits
    }
}

/// <summary> Draws a circle on the screen. </summary>
/// <param name="x"> X coordinate of the center of the circle. </param>
/// <param name="y"> Y coordinate of the center of the circle. </param>
/// <param name="radius"> Circle radius. </param>
/// <param name="color"> Circle color. </param>
void drawCircle(int x, int y, int radius, uint32_t color) {
    for (int dy = -radius; dy <= radius; ++dy) {
        for (int dx = -radius; dx <= radius; ++dx) {
            if (dx * dx + dy * dy <= radius * radius) {
                int drawX = x + dx;
                int drawY = y + dy;
                if (drawX >= 0 && drawX < SCREEN_WIDTH && drawY >= 0 && drawY < SCREEN_HEIGHT) {
                    buffer[drawY][drawX] = color;
                }
            }
        }
    }
}

/// <summary> Draws a rectangle on the screen. </summary>
/// <param name="x"> X coordinate of the top left corner of the rectangle. </param>
/// <param name="y"> Y coordinate of the top left corner of the rectangle. </param>
/// <param name="width"> Rectangle width. </param>
/// <param name="height"> Rectangle height. </param>
/// <param name="color"> Rectangle color. </param>
void drawRect(int x, int y, int width, int height, uint32_t color) {
    for (int dy = 0; dy < height; ++dy) {
        for (int dx = 0; dx < width; ++dx) {
            int drawX = x + dx;
            int drawY = y + dy;
            // Checking screen bounds
            if (drawX >= 0 && drawX < SCREEN_WIDTH && drawY >= 0 && drawY < SCREEN_HEIGHT) {
                buffer[drawY][drawX] = color;
            }
        }
    }
}