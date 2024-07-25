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

// --- Global variables ---
const int NUM_REC = 2; // Number of flying rectangles (polygons)
const float REC_SIZE = 15.2f; // Size of the rectangle
const int PLAYER_RADIUS = 18; // Radius of the player circle
const int ENEMY_SPEED = 320.0f; // Enemy speed
const float ROTATION_SPEED = 3.2f; // Rotation speed of the player's circles
const int PROX_CIRCLES = 8; // Proximity coefficient of player's circles
const float CENTER_ZONE_RADIUS = 215.0f; // Radius of the central zone where rectangles fly
bool spaceKeyPressed = false; // Spacebar press tracking
bool clockwise = true; // Direction of rotation
int score = 0; // Number of points

// Structure for representing rectangles (enemies)
struct Rectangle {
    float x, y; // Center coordinates
    float dx, dy; // Direction vector
    uint32_t color;
};

Rectangle rectangles[NUM_REC]; // Array of rectangles
float playerAngle = 0.0f; // Player rotation angle


// --- Game functions ---
/// <summary> Method for setting / resetting the position of a rectangle
/// <para>i - rectangle number.</para>
/// </summary>
static void resetSquare(int i) {
    int side = rand() % 4; // screen side; 0: up, 1: right, 2: down, 3: left

    // Setting the initial position of the rectangle depending on the selected side
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

    // SCREEN_WIDTH / 2 and SCREEN_HEIGHT / 2 set the center of the screen
    // rand() % (int)CENTER_ZONE_RADIUS creates a random offset within the radius of the central zone
    // Subtracting CENTER_ZONE_RADIUS / 2 centers the zone around the center of the screen
    float targetX = SCREEN_WIDTH / 2 + (rand() % (int)CENTER_ZONE_RADIUS) - CENTER_ZONE_RADIUS / 2; //
    float targetY = SCREEN_HEIGHT / 2 + (rand() % (int)CENTER_ZONE_RADIUS) - CENTER_ZONE_RADIUS / 2;

    // Calculation of the direction vector of motion
    float dx = targetX - rectangles[i].x;
    float dy = targetY - rectangles[i].y;
    float length = sqrt(dx * dx + dy * dy);

    // Setting the speed of movement with a small random variation
    float speedFactor = 1.0f + (rand() % 5) * 0.1f; // Speed will vary from 1.0 to 1.4
    rectangles[i].dx = (dx / length) * speedFactor;
    rectangles[i].dy = (dy / length) * speedFactor;

    // Random selection of the color of the rectangle (white or black)
    rectangles[i].color = (rand() % 2 == 0) ? WHITE_COLOR : BLACK_COLOR;
}

/// <summary> Game initialization </summary>
void initialize() {
    srand(time(NULL));
    for (int i = 0; i < NUM_REC; ++i) {
        resetSquare(i);
    }
    score = 0;
    playerAngle = 0.0f;
    spaceKeyPressed = false;
}

/// <summary> Updating the state of the game (logic) </summary>
/// <param name="dt">- Delta Time, The time interval between frames </param>
void act(float dt) {
    if (is_key_pressed(VK_ESCAPE))
        schedule_quit_game();

    // Space to change the direction of rotation
    if (is_key_pressed(VK_SPACE)) {
        if (!spaceKeyPressed) {
            clockwise = !clockwise;
            spaceKeyPressed = true;
        }
    }
    else {
        spaceKeyPressed = false;
    }

    // Update player rotation angle
    playerAngle += (clockwise ? 1 : -1) * ROTATION_SPEED * dt;

    // Update rectangle positions
    for (int i = 0; i < NUM_REC; ++i) {
        rectangles[i].x += rectangles[i].dx * ENEMY_SPEED * dt;
        rectangles[i].y += rectangles[i].dy * ENEMY_SPEED * dt;

        // If the center of the rectangle is further than REC_SIZE behind any of the edges of the screen,
        // it is considered that the rectangle has left the playing field
        if (rectangles[i].x < -REC_SIZE || rectangles[i].x > SCREEN_WIDTH + REC_SIZE ||
            rectangles[i].y < -REC_SIZE || rectangles[i].y > SCREEN_HEIGHT + REC_SIZE) {
            resetSquare(i);
        }
    }

    // j == 0 corresponds to the first circle, j == 1 - to the second
    // SCREEN_WIDTH / 2 and SCREEN_HEIGHT / 2 set the center of rotation (center of the screen)
    // (SCREEN_WIDTH / PROX_CIRCLES) determines the radius of rotation
    // cos(playerAngle) and sin(playerAngle) calculate the position on the circle
    for (int j = 0; j < 2; ++j) {
        int playerX = SCREEN_WIDTH / 2 + (j == 0 ? -1 : 1) * (SCREEN_WIDTH / PROX_CIRCLES) * cos(playerAngle);
        int playerY = SCREEN_HEIGHT / 2 + (j == 0 ? -1 : 1) * (SCREEN_WIDTH / PROX_CIRCLES) * sin(playerAngle);

        for (int i = 0; i < NUM_REC; ++i) {
            // Checking the distance between the player's circle and the rectangle
            float dx = playerX - rectangles[i].x;
            float dy = playerY - rectangles[i].y;
            float distanceSquared = dx * dx + dy * dy;

            if (distanceSquared < pow(REC_SIZE + PLAYER_RADIUS, 2)) {
                // Collision handling
                if (rectangles[i].color == WHITE_COLOR) {
                    score += 5; // Increase the score when colliding with a white rectangle
                }
                else {
                    initialize(); // Reset the game when colliding with a black rectangle
                    return;
                }
                resetSquare(i);
                break;
            }
        }
    }
}

/// <summary> Drawing the text with points on the screen </summary>
void show_score() {
    draw_number(SCREEN_WIDTH - 150, 50, score, WHITE_COLOR); // Adjust the position for large numbers
}

/// <summary> Drawing all game elements on the screen </summary>
void draw() {
    uint32_t gray = 0x808080; // background color is gray
    memset(buffer, gray, SCREEN_HEIGHT * SCREEN_WIDTH * sizeof(uint32_t));

    // Drawing rectangles
    for (int i = 0; i < NUM_REC; ++i) {
        drawRect(rectangles[i].x - REC_SIZE, rectangles[i].y - REC_SIZE, 2 * REC_SIZE, 2 * REC_SIZE, rectangles[i].color);
    }

    // Drawing player circles
    for (int i = 0; i < 2; ++i) {
        int playerX = SCREEN_WIDTH / 2 + (i == 0 ? -1 : 1) * (SCREEN_WIDTH / PROX_CIRCLES) * cos(playerAngle);
        int playerY = SCREEN_HEIGHT / 2 + (i == 0 ? -1 : 1) * (SCREEN_WIDTH / PROX_CIRCLES) * sin(playerAngle);
        drawCircle(playerX, playerY, PLAYER_RADIUS, WHITE_COLOR);
    }

    show_score();
}

void finalize() {
    //  There are no dynamic data structures and objects created on the heap
}