/* File: Ball.h
 * Author: Julie Zelenski (zelenski@cs.stanford.edu), with updates
 *         and revisions by Keith Schwarz (htiek@cs.stanford.edu).
 *
 * Type representing a ball that bounces within a set of rectangular
 * bounds.
 */
#pragma once
#include "Demos/Rectangle.h"
#include "GUI/GUIUtils.h"
#include "GUI/Font.h"
#include "gwindow.h"
#include "gfont.h"
#include <string>

class Ball {
public:
    /* Creates a new ball with the specified ID number that bounces
     * inside of the specified rectangle.
     */
    Ball(int ballID, const Rectangle& bounceBounds);

    /* Draws the ball on the screen. */
    void draw() const;

    /* Moves the ball one step. */
    void move();

private:
    /* Ball x and y coordinate. */
    int _x;
    int _y;

    /* Ball velocity: _x and _y are updated by adding _vx and _vy each time
     * move() is called.
     */
    int _vx;
    int _vy;

    /* Which number displays on this ball. */
    int _id;

    /* Rectangular area the ball must stay inside while bouncing. */
    Rectangle _bounceBounds;
};

/* Graphics helper functions. */
void drawOval(int x, int y, int width, int height);
void drawCenteredText(double x, double y, double width, double height, const std::string& text);
void clearWindow();
void updateWindow();

/* Creates a bunch of balls and bounces them around. */
void bounceBalls(int numBalls, int numSteps, const Rectangle& bounceArea);
