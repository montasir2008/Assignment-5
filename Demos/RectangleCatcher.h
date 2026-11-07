#pragma once

#include "Rectangle.h"
#include "GUI/Color.h"
#include "vector.h"
#include <ostream>
using MiniGUI::Color;

/* Type representing a rectangle drawn on the screen.
 */
struct DrawnRectangle {
    Rectangle rectangle;
    Color color;
};

/* A type that intercepts all drawn rectangles and writes them down for
 * future inspection.
 */
class RectangleCatcher {
public:
    RectangleCatcher();
    ~RectangleCatcher();

    /* Returns how many rectangles were drawn. */
    int numDrawn() const;

    /* Returns the rectangle at a given index. */
    DrawnRectangle operator[] (int index) const;

    /* Clears all memory of the rectangles seen thus far. */
    void reset();

private:
    Vector<DrawnRectangle> drawn;
};

/* Printer. */
std::ostream& operator<< (std::ostream& out, const DrawnRectangle& particle);

/* Comparators. */
bool operator== (const DrawnRectangle& lhs, const DrawnRectangle& rhs);
bool operator!= (const DrawnRectangle& lhs, const DrawnRectangle& rhs);
bool operator<  (const DrawnRectangle& lhs, const DrawnRectangle& rhs);
