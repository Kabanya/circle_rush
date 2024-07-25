#include "DrawFunctions.h"

/// <summary> Отрисовка цифры на экране. </summary>
/// <param name="x"> Координата X левого верхнего угла цифры. </param>
/// <param name="y"> Координата Y левого верхнего угла цифры. </param>
/// <param name="digit"> Цифра для отрисовки (от 0 до 9). </param>
/// <param name="color"> Цвет цифры. </param>
void draw_digit(int x, int y, int digit, uint32_t color) {
    if (digit < 0 || digit > 9) return;

    for (int row = 0; row < 10; row++) {
        uint8_t bits = font[digit][row]; // Получение битовой маски для текущей строки цифры
        for (int col = 0; col < 8; col++) {
            if (bits & (1 << (7 - col))) { 
                int px = x + col * 2;     // Увеличиваем ширину
                int py = y + row * 2;     // Увеличиваем высоту

                // Рисуем увеличенный пиксель (2x2)
                for (int dy = 0; dy < 2; dy++) {
                    for (int dx = 0; dx < 2; dx++) {
                        int drawX = px + dx;
                        int drawY = py + dy;

                        // Проверка границ экрана
                        if (drawX >= 0 && drawX < SCREEN_WIDTH && drawY >= 0 && drawY < SCREEN_HEIGHT) {
                            buffer[drawY][drawX] = color;
                        }
                    }
                }
            }
        }
    }
}

/// <summary> Отрисовка числа на экране. </summary>
/// <param name="x"> Координата X левого верхнего угла числа. </param>
/// <param name="y"> Координата Y левого верхнего угла числа. </param>
/// <param name="number"> Число для отрисовки. </param>
/// <param name="color"> Цвет числа. </param>
void draw_number(int x, int y, int number, uint32_t color) {
    char str[12];
    snprintf(str, sizeof(str), "%d", number);

    int dx = 0;
    for (char* p = str; *p; p++) {
        draw_digit(x + dx, y, *p - '0', color);
        dx += 20; // Увеличиваем расстояние между цифрами
    }
}

/// <summary> Отрисовка круга на экране. </summary>
/// <param name="x"> Координата X центра круга. </param>
/// <param name="y"> Координата Y центра круга. </param>
/// <param name="radius"> Радиус круга. </param>
/// <param name="color"> Цвет круга. </param>
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

/// <summary> Отрисовка прямоугольника на экране. </summary>
/// <param name="x"> Координата X левого верхнего угла прямоугольника. </param>
/// <param name="y"> Координата Y левого верхнего угла прямоугольника. </param>
/// <param name="width"> Ширина прямоугольника. </param>
/// <param name="height"> Высота прямоугольника. </param>
/// <param name="color"> Цвет прямоугольника. </param>
void drawRect(int x, int y, int width, int height, uint32_t color) {
    for (int dy = 0; dy < height; ++dy) {
        for (int dx = 0; dx < width; ++dx) {
            int drawX = x + dx;
            int drawY = y + dy;
            // Проверка границ экрана
            if (drawX >= 0 && drawX < SCREEN_WIDTH && drawY >= 0 && drawY < SCREEN_HEIGHT) {
                buffer[drawY][drawX] = color;
            }
        }
    }
}