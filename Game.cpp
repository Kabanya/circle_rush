#include "Engine.h"
#include <stdlib.h>
#include <memory.h>
#include <math.h>
#include <stdint.h>
#include <time.h>
#include <stdio.h>
#include "DrawFunctions.h"

#define BLACK_COLOR 0x000000
#define WHITE_COLOR 0xFFFFFF

// --- Глобальные переменные ---
const int NUM_REC = 2; // Количество летящих прямоугольника (многоугольники)
const float REC_SIZE = 15.2f; // Размер прямоугольника
const int PLAYER_RADIUS = 18; // Радиус круга-игрока
const int ENEMY_SPEED = 320.0f; // Скорость противников
const float ROTATION_SPEED = 3.2f; // Скорость вращения кругов игрока
const int PROX_CIRCLES = 8; // Коэффициент близости кругов игрока
const float CENTER_ZONE_RADIUS = 215.0f; // Радиус центральной зоны, в которую летят прямоугольники
bool spaceKeyPressed = false; // Отслеживание нажатия пробела
bool clockwise = true; // Направление вращения
int score = 0; // Количество очков

// Структура для представления прямоугольников (противников)
struct Rectangle {
    float x, y; // Координаты центра
    float dx, dy; // Вектор направления
    uint32_t color;
};

Rectangle rectangles[NUM_REC]; // Массив прямоугольников
float playerAngle = 0.0f; // Угол вращения игрока


// --- Функции игры ---
/// <summary>Метод установки / сброса позиции для прямоугольника
/// <para>i - номер прямоугольника.</para>
/// </summary>
static void resetSquare(int i) {
    int side = rand() % 4; // сторона экрана; 0: вверх, 1: право, 2: низ, 3: лево

    // Установка начальной позиции прямоугольника в зависимости от выбранной стороны
    switch (side) {
    case 0:
        rectangles[i].x = rand() % SCREEN_WIDTH;
        rectangles[i].y = -REC_SIZE;
        break;
    case 1:
        rectangles[i].y = rand() % SCREEN_HEIGHT;
        rectangles[i].x = SCREEN_WIDTH + REC_SIZE;
        break;
    case 2:
        rectangles[i].x = rand() % SCREEN_WIDTH;
        rectangles[i].y = SCREEN_HEIGHT + REC_SIZE;
        break;
    case 3:
        rectangles[i].y = rand() % SCREEN_HEIGHT;
        rectangles[i].x = -REC_SIZE;
        break;
    }

    // SCREEN_WIDTH / 2 и SCREEN_HEIGHT / 2 задают центр экрана
    // rand() % (int)CENTER_ZONE_RADIUS создает случайное смещение в пределах радиуса центральной зоны
    // Вычитание CENTER_ZONE_RADIUS / 2 центрирует зону вокруг центра экрана
    float targetX = SCREEN_WIDTH / 2 + (rand() % (int)CENTER_ZONE_RADIUS) - CENTER_ZONE_RADIUS / 2; //
    float targetY = SCREEN_HEIGHT / 2 + (rand() % (int)CENTER_ZONE_RADIUS) - CENTER_ZONE_RADIUS / 2;

    // Расчет вектора направления движения
    float dx = targetX - rectangles[i].x;
    float dy = targetY - rectangles[i].y;
    float length = sqrt(dx * dx + dy * dy);

    // Установка скорости движения с небольшой случайной вариацией
    float speedFactor = 1.0f + (rand() % 5) * 0.1f; // Скорость будет меняться от 1.0 до 1.4
    rectangles[i].dx = (dx / length) * speedFactor;
    rectangles[i].dy = (dy / length) * speedFactor;

    // Случайный выбор цвета прямоугольника (белый или черный)
    rectangles[i].color = (rand() % 2 == 0) ? WHITE_COLOR : BLACK_COLOR;
}

/// <summary> Инициализация игры </summary>
void initialize() {
    srand(time(NULL));
    for (int i = 0; i < NUM_REC; ++i) {
        resetSquare(i);
    }
    score = 0;
    playerAngle = 0.0f;
    spaceKeyPressed = false;
}

/// <summary> Обновление состояния игры (логика) </summary>
/// <param name="dt">- Delta Time, Промежуток времени между кадрами </param>
void act(float dt) {
    if (is_key_pressed(VK_ESCAPE))
        schedule_quit_game();
    
    // Пробел для изменения направления вращения
    if (is_key_pressed(VK_SPACE)) {
        if (!spaceKeyPressed) {
            clockwise = !clockwise;
            spaceKeyPressed = true;
        }
    }
    else {
        spaceKeyPressed = false;
    }

    // Обновление угла поворота игрока
    playerAngle += (clockwise ? 1 : -1) * ROTATION_SPEED * dt;

    // Обновление позиций прямоугольников
    for (int i = 0; i < NUM_REC; ++i) {
        rectangles[i].x += rectangles[i].dx * ENEMY_SPEED * dt;
        rectangles[i].y += rectangles[i].dy * ENEMY_SPEED * dt;
        
        // Если центр прямоугольника находится дальше, чем REC_SIZE за любым из краёв экрана,
        // считается, что прямоугольник покинул игровое поле
        if (rectangles[i].x < -REC_SIZE || rectangles[i].x > SCREEN_WIDTH + REC_SIZE ||
            rectangles[i].y < -REC_SIZE || rectangles[i].y > SCREEN_HEIGHT + REC_SIZE) {
            resetSquare(i);
        }
    }

    // j == 0 соответствует первому кругу, j == 1 - второму
    // SCREEN_WIDTH / 2 и SCREEN_HEIGHT / 2 задают центр вращения (центр экрана)
    // (SCREEN_WIDTH / PROX_CIRCLES) определяет радиус вращения
    // cos(playerAngle) и sin(playerAngle) вычисляют позицию на окружности
    for (int j = 0; j < 2; ++j) {
        int playerX = SCREEN_WIDTH / 2 + (j == 0 ? -1 : 1) * (SCREEN_WIDTH / PROX_CIRCLES) * cos(playerAngle);
        int playerY = SCREEN_HEIGHT / 2 + (j == 0 ? -1 : 1) * (SCREEN_HEIGHT / PROX_CIRCLES) * sin(playerAngle);

        for (int i = 0; i < NUM_REC; ++i) {
            // Проверка расстояния между кругом игрока и прямоугольником
            float dx = playerX - rectangles[i].x;
            float dy = playerY - rectangles[i].y;
            float distanceSquared = dx * dx + dy * dy;

            if (distanceSquared < pow(REC_SIZE + PLAYER_RADIUS, 2)) {
                // Обработка столкновения
                if (rectangles[i].color == WHITE_COLOR) {
                    score += 5; // Увеличение счета при столкновении с белым прямоугольником
                }
                else {
                    initialize(); // Сброс игры при столкновении с черным прямоугольником
                    return;
                }
                resetSquare(i);
                break;
            }
        }
    }
}

/// <summary> Отрисовка текста с очками на экране </summary>
void show_score() {
    draw_number(SCREEN_WIDTH - 150, 50, score, WHITE_COLOR); // Корректируем позицию для больших чисел
}

/// <summary> Отрисовка всех элементов игры на экране </summary>
void draw() {
    uint32_t gray = 0x808080; // цвет фона серый
    memset(buffer, gray, SCREEN_HEIGHT * SCREEN_WIDTH * sizeof(uint32_t));

    // Отрисовка прямоугольников
    for (int i = 0; i < NUM_REC; ++i) {
        drawRect(rectangles[i].x - REC_SIZE, rectangles[i].y - REC_SIZE, 2 * REC_SIZE, 2 * REC_SIZE, rectangles[i].color);
    }

    // Отрисовка кругов игрока
    for (int i = 0; i < 2; ++i) {
        int playerX = SCREEN_WIDTH / 2 + (i == 0 ? -1 : 1) * (SCREEN_WIDTH / PROX_CIRCLES) * cos(playerAngle);
        int playerY = SCREEN_HEIGHT / 2 + (i == 0 ? -1 : 1) * (SCREEN_HEIGHT / PROX_CIRCLES) * sin(playerAngle);
        drawCircle(playerX, playerY, PLAYER_RADIUS, WHITE_COLOR);
    }

    show_score();
}

void finalize() {
    //  Нет динамических структур данных и объектов, созданных в куче
}