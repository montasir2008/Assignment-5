#include "RectangleCatcher.h"
#include "DrawRectangle.h"
#include <sstream>
#include <limits>
#include <iomanip>
#include "gmath.h"
#include "collections.h"
using namespace std;

RectangleCatcher::RectangleCatcher() {
    /* Stash what's drawn here. */
    setDrawFunction([this] (const Rectangle& rect, Color c) {
        drawn.add({ rect, c });
    });
}

RectangleCatcher::~RectangleCatcher() {
    setDrawFunction(nullptr);
}

int RectangleCatcher::numDrawn() const {
    return drawn.size();
}

DrawnRectangle RectangleCatcher::operator [](int index) const {
    if (index < 0 || index >= numDrawn()) {
        error("RectangleCatcher: Index is out of range.");
    }
    return drawn[index];
}

void RectangleCatcher::reset() {
    drawn.clear();
}

/* Printer. */
ostream& operator<< (ostream& out, const DrawnRectangle& particle) {
    ostringstream builder;
    builder << "{ "
            << particle.rectangle << ", "
            << particle.color
            << " }";
    return out << builder.str();
}

/* Comparators. */
bool operator== (const DrawnRectangle& lhs, const DrawnRectangle& rhs) {
    return lhs.color == rhs.color && lhs.rectangle == rhs.rectangle;
}
bool operator!= (const DrawnRectangle& lhs, const DrawnRectangle& rhs) {
    return !(lhs == rhs);
}
bool operator<  (const DrawnRectangle& lhs, const DrawnRectangle& rhs) {
    return stanfordcpplib::collections::compareTo(lhs.rectangle, rhs.rectangle, lhs.color, rhs.color);
}
