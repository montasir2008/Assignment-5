/* File: Ball.cpp
 * Author: Julie Zelenski (zelenski@cs.stanford.edu), with updates by
 *         Keith Schwarz (htiek@cs.stanford.edu)
 *
 * Implementation of the Ball type, along with the bounceBalls()
 * function.
 */
#include "Ball.h"
#include "random.h"
using namespace std;

/* Constants used by the Ball class. */
static const int kBallSize    = 50;
static const int kMinVelocity = -5;
static const int kMaxVelocity = 5;

/* Constructs a new ball, assigning it a random position and velocity. */
Ball::Ball(int ballID, const Rectangle& bounds) {
    _id = ballID;
    _bounceBounds = bounds;

    /* Choose (x, y) coordinate. */
    _x = randomInteger(bounds.x, bounds.x + bounds.width);
    _y = randomInteger(bounds.y, bounds.y + bounds.height);

    /* Choose x and y velocity. */
    _vx = randomInteger(kMinVelocity, kMaxVelocity);
    _vy = randomInteger(kMinVelocity, kMaxVelocity);
}

/* Draws the ball on screen as a filled oval with a text label on top containing
 * the ball ID number.
 */
void Ball::draw() const {
    drawOval(_x, _y, kBallSize, kBallSize);
    drawCenteredText(_x, _y, kBallSize, kBallSize, integerToString(_id));
}

/* Moves the ball one step in the direction indicated by its velocity.
 * If the ball leaves the bounding box, "bounce" the ball by reflecting
 * the velocity in the x and/or y directions.
 */
void Ball::move() {
    _x += _vx;
    _y += _vy;

    /* If we moved off the left or right side of the bounding box, reverse the
     * x velocity.
     */
    if (_x < _bounceBounds.x || _x + kBallSize > _bounceBounds.x + _bounceBounds.width) {
        _vx = -_vx;
    }

    /* Same for y velocity if we moved off the top or bottom. */
    if (_y < _bounceBounds.y || _y + kBallSize > _bounceBounds.y + _bounceBounds.height) {
        _vy = -_vy;
    }
}

/* Runs a demo of bouncing a collection of balls around the specified rectangle.
 * numBalls controls how many balls there are. numSteps dictates how many steps
 * we need to run the simulation for. bounceBounds dictates the region where the
 * balls will bounce.
 */
void bounceBalls(int numBalls, int numSteps, const Rectangle& bounceBounds) {
    // setRandomSeed(<your number here>);

    /* Create a bunch of balls, stashing them in a Vector for later. */
    Vector<Ball> allBalls;
    for (int i = 0; i < numBalls; i++) {
        Ball ball(i, bounceBounds);
        allBalls.add(ball);
    }

    /* Animation loop: Move and draw all balls. */
    for (int i = 0; i < numSteps; i++) {
        /* Clear the display so we only see the balls in the current frame. */
        clearWindow();

        /* Move and draw all balls. */
        for (int i = 0; i < allBalls.size(); i++) {
            allBalls[i].move();
            allBalls[i].draw();
        }

        /* Make the contents we just drew visible; by default, nothing displays
         * until we explicitly tell the window it's time to draw.
         */
        updateWindow();

        /* Pause for a bit so that we can see what's happening. */
        pause(20);
    }
}
