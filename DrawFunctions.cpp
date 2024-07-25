#include "DrawFunctions.h"

/// <summary> ��������� ����� �� ������. </summary>
/// <param name="x"> ���������� X ������ �������� ���� �����. </param>
/// <param name="y"> ���������� Y ������ �������� ���� �����. </param>
/// <param name="digit"> ����� ��� ��������� (�� 0 �� 9). </param>
/// <param name="color"> ���� �����. </param>
void draw_digit(int x, int y, int digit, uint32_t color) {
    if (digit < 0 || digit > 9) return;

    for (int row = 0; row < 10; row++) {
        uint8_t bits = font[digit][row]; // ��������� ������� ����� ��� ������� ������ �����
        for (int col = 0; col < 8; col++) {
            if (bits & (1 << (7 - col))) { 
                int px = x + col * 2;     // ����������� ������
                int py = y + row * 2;     // ����������� ������

                // ������ ����������� ������� (2x2)
                for (int dy = 0; dy < 2; dy++) {
                    for (int dx = 0; dx < 2; dx++) {
                        int drawX = px + dx;
                        int drawY = py + dy;

                        // �������� ������ ������
                        if (drawX >= 0 && drawX < SCREEN_WIDTH && drawY >= 0 && drawY < SCREEN_HEIGHT) {
                            buffer[drawY][drawX] = color;
                        }
                    }
                }
            }
        }
    }
}

/// <summary> ��������� ����� �� ������. </summary>
/// <param name="x"> ���������� X ������ �������� ���� �����. </param>
/// <param name="y"> ���������� Y ������ �������� ���� �����. </param>
/// <param name="number"> ����� ��� ���������. </param>
/// <param name="color"> ���� �����. </param>
void draw_number(int x, int y, int number, uint32_t color) {
    char str[12];
    snprintf(str, sizeof(str), "%d", number);

    int dx = 0;
    for (char* p = str; *p; p++) {
        draw_digit(x + dx, y, *p - '0', color);
        dx += 20; // ����������� ���������� ����� �������
    }
}

/// <summary> ��������� ����� �� ������. </summary>
/// <param name="x"> ���������� X ������ �����. </param>
/// <param name="y"> ���������� Y ������ �����. </param>
/// <param name="radius"> ������ �����. </param>
/// <param name="color"> ���� �����. </param>
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

/// <summary> ��������� �������������� �� ������. </summary>
/// <param name="x"> ���������� X ������ �������� ���� ��������������. </param>
/// <param name="y"> ���������� Y ������ �������� ���� ��������������. </param>
/// <param name="width"> ������ ��������������. </param>
/// <param name="height"> ������ ��������������. </param>
/// <param name="color"> ���� ��������������. </param>
void drawRect(int x, int y, int width, int height, uint32_t color) {
    for (int dy = 0; dy < height; ++dy) {
        for (int dx = 0; dx < width; ++dx) {
            int drawX = x + dx;
            int drawY = y + dy;
            // �������� ������ ������
            if (drawX >= 0 && drawX < SCREEN_WIDTH && drawY >= 0 && drawY < SCREEN_HEIGHT) {
                buffer[drawY][drawX] = color;
            }
        }
    }
}