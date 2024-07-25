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

// --- ���������� ���������� ---
const int NUM_REC = 2; // ���������� ������� �������������� (��������������)
const float REC_SIZE = 15.2f; // ������ ��������������
const int PLAYER_RADIUS = 18; // ������ �����-������
const int ENEMY_SPEED = 320.0f; // �������� �����������
const float ROTATION_SPEED = 3.2f; // �������� �������� ������ ������
const int PROX_CIRCLES = 8; // ����������� �������� ������ ������
const float CENTER_ZONE_RADIUS = 215.0f; // ������ ����������� ����, � ������� ����� ��������������
bool spaceKeyPressed = false; // ������������ ������� �������
bool clockwise = true; // ����������� ��������
int score = 0; // ���������� �����

// ��������� ��� ������������� ��������������� (�����������)
struct Rectangle {
    float x, y; // ���������� ������
    float dx, dy; // ������ �����������
    uint32_t color;
};

Rectangle rectangles[NUM_REC]; // ������ ���������������
float playerAngle = 0.0f; // ���� �������� ������


// --- ������� ���� ---
/// <summary>����� ��������� / ������ ������� ��� ��������������
/// <para>i - ����� ��������������.</para>
/// </summary>
static void resetSquare(int i) {
    int side = rand() % 4; // ������� ������; 0: �����, 1: �����, 2: ���, 3: ����

    // ��������� ��������� ������� �������������� � ����������� �� ��������� �������
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

    // SCREEN_WIDTH / 2 � SCREEN_HEIGHT / 2 ������ ����� ������
    // rand() % (int)CENTER_ZONE_RADIUS ������� ��������� �������� � �������� ������� ����������� ����
    // ��������� CENTER_ZONE_RADIUS / 2 ���������� ���� ������ ������ ������
    float targetX = SCREEN_WIDTH / 2 + (rand() % (int)CENTER_ZONE_RADIUS) - CENTER_ZONE_RADIUS / 2; //
    float targetY = SCREEN_HEIGHT / 2 + (rand() % (int)CENTER_ZONE_RADIUS) - CENTER_ZONE_RADIUS / 2;

    // ������ ������� ����������� ��������
    float dx = targetX - rectangles[i].x;
    float dy = targetY - rectangles[i].y;
    float length = sqrt(dx * dx + dy * dy);

    // ��������� �������� �������� � ��������� ��������� ���������
    float speedFactor = 1.0f + (rand() % 5) * 0.1f; // �������� ����� �������� �� 1.0 �� 1.4
    rectangles[i].dx = (dx / length) * speedFactor;
    rectangles[i].dy = (dy / length) * speedFactor;

    // ��������� ����� ����� �������������� (����� ��� ������)
    rectangles[i].color = (rand() % 2 == 0) ? WHITE_COLOR : BLACK_COLOR;
}

/// <summary> ������������� ���� </summary>
void initialize() {
    srand(time(NULL));
    for (int i = 0; i < NUM_REC; ++i) {
        resetSquare(i);
    }
    score = 0;
    playerAngle = 0.0f;
    spaceKeyPressed = false;
}

/// <summary> ���������� ��������� ���� (������) </summary>
/// <param name="dt">- Delta Time, ���������� ������� ����� ������� </param>
void act(float dt) {
    if (is_key_pressed(VK_ESCAPE))
        schedule_quit_game();
    
    // ������ ��� ��������� ����������� ��������
    if (is_key_pressed(VK_SPACE)) {
        if (!spaceKeyPressed) {
            clockwise = !clockwise;
            spaceKeyPressed = true;
        }
    }
    else {
        spaceKeyPressed = false;
    }

    // ���������� ���� �������� ������
    playerAngle += (clockwise ? 1 : -1) * ROTATION_SPEED * dt;

    // ���������� ������� ���������������
    for (int i = 0; i < NUM_REC; ++i) {
        rectangles[i].x += rectangles[i].dx * ENEMY_SPEED * dt;
        rectangles[i].y += rectangles[i].dy * ENEMY_SPEED * dt;
        
        // ���� ����� �������������� ��������� ������, ��� REC_SIZE �� ����� �� ���� ������,
        // ���������, ��� ������������� ������� ������� ����
        if (rectangles[i].x < -REC_SIZE || rectangles[i].x > SCREEN_WIDTH + REC_SIZE ||
            rectangles[i].y < -REC_SIZE || rectangles[i].y > SCREEN_HEIGHT + REC_SIZE) {
            resetSquare(i);
        }
    }

    // j == 0 ������������� ������� �����, j == 1 - �������
    // SCREEN_WIDTH / 2 � SCREEN_HEIGHT / 2 ������ ����� �������� (����� ������)
    // (SCREEN_WIDTH / PROX_CIRCLES) ���������� ������ ��������
    // cos(playerAngle) � sin(playerAngle) ��������� ������� �� ����������
    for (int j = 0; j < 2; ++j) {
        int playerX = SCREEN_WIDTH / 2 + (j == 0 ? -1 : 1) * (SCREEN_WIDTH / PROX_CIRCLES) * cos(playerAngle);
        int playerY = SCREEN_HEIGHT / 2 + (j == 0 ? -1 : 1) * (SCREEN_HEIGHT / PROX_CIRCLES) * sin(playerAngle);

        for (int i = 0; i < NUM_REC; ++i) {
            // �������� ���������� ����� ������ ������ � ���������������
            float dx = playerX - rectangles[i].x;
            float dy = playerY - rectangles[i].y;
            float distanceSquared = dx * dx + dy * dy;

            if (distanceSquared < pow(REC_SIZE + PLAYER_RADIUS, 2)) {
                // ��������� ������������
                if (rectangles[i].color == WHITE_COLOR) {
                    score += 5; // ���������� ����� ��� ������������ � ����� ���������������
                }
                else {
                    initialize(); // ����� ���� ��� ������������ � ������ ���������������
                    return;
                }
                resetSquare(i);
                break;
            }
        }
    }
}

/// <summary> ��������� ������ � ������ �� ������ </summary>
void show_score() {
    draw_number(SCREEN_WIDTH - 150, 50, score, WHITE_COLOR); // ������������ ������� ��� ������� �����
}

/// <summary> ��������� ���� ��������� ���� �� ������ </summary>
void draw() {
    uint32_t gray = 0x808080; // ���� ���� �����
    memset(buffer, gray, SCREEN_HEIGHT * SCREEN_WIDTH * sizeof(uint32_t));

    // ��������� ���������������
    for (int i = 0; i < NUM_REC; ++i) {
        drawRect(rectangles[i].x - REC_SIZE, rectangles[i].y - REC_SIZE, 2 * REC_SIZE, 2 * REC_SIZE, rectangles[i].color);
    }

    // ��������� ������ ������
    for (int i = 0; i < 2; ++i) {
        int playerX = SCREEN_WIDTH / 2 + (i == 0 ? -1 : 1) * (SCREEN_WIDTH / PROX_CIRCLES) * cos(playerAngle);
        int playerY = SCREEN_HEIGHT / 2 + (i == 0 ? -1 : 1) * (SCREEN_HEIGHT / PROX_CIRCLES) * sin(playerAngle);
        drawCircle(playerX, playerY, PLAYER_RADIUS, WHITE_COLOR);
    }

    show_score();
}

void finalize() {
    //  ��� ������������ �������� ������ � ��������, ��������� � ����
}