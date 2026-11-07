#include "Sample.h"
#include <cmath>
using namespace std;

Sample::Sample(double value) : value(value) {

}

Sample::operator double& () {
    return value;
}

Sample::operator double () const {
    return value;
}

bool operator== (const Sample& lhs, const Sample& rhs) {
    static const double kTolerance = 1e-8;
    return fabs(lhs - rhs) <= kTolerance;
}

bool operator!= (const Sample& lhs, const Sample& rhs) {
    return !(lhs == rhs);
}
