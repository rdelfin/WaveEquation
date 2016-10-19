//
// Created by Ricardo Delfin on 10/15/16.
//

#include <iostream>
#include "Field.h"

Field::Field(glm::dvec2 min, glm::dvec2 max, double step, double speed)
    : speed(speed), step(step) {
    glm::dvec2 diff = max - min;
    glm::uvec2 totalSteps = glm::uvec2(diff.x / step, diff.y / step);
    val = std::vector<std::vector<double>>(totalSteps.x);
    valSpeed = std::vector<std::vector<double>>(totalSteps.x);
    for(unsigned long i = 0; i < totalSteps.x; i++) {
        val[i] = std::vector<double>(totalSteps.y);
        valSpeed[i] = std::vector<double>(totalSteps.y);
        for(unsigned long j = 0; j < totalSteps.y; j++) {
            val[i][j] = 0;
            valSpeed[i][j] = 0;
        }
    }

    this->min = min;
    this->max = min + step*glm::dvec2(totalSteps.x, totalSteps.y);
}

void Field::setField(std::vector<std::vector<double>> f0) {
    val = f0;
}

/* Computes the position of every pixel based on the wave equation */
void Field::update(double dt) {
    std::vector<std::vector<double>> newVals(val.size());
    for(unsigned long i = 0; i < val.size(); i++) {
        newVals[i] = std::vector<double>(val[i].size());
        for (unsigned long j = 0; j < val[i].size(); j++) {
            if(i == 0 || j == 0 || i == val.size() - 1 || j == val.size() - 1) {
                newVals[i][j] = 0;
            } else {
                double dfdxRight = (val[i + 1][j] - val[i][j]) / step;
                double dfdxLeft = (val[i][j] - val[i - 1][j]) / step;
                double d2fdx2 = (dfdxRight - dfdxLeft) / step;

                double dfdyUp = (val[i][j + 1] - val[i][j]) / step;
                double dfdyDown = (val[i][j] - val[i][j - 1]) / step;
                double d2fdy2 = (dfdyUp - dfdyDown) / step;

                double d2fdt2 = speed * (d2fdy2 + d2fdx2);

                valSpeed[i][j] += d2fdt2 * dt;
                newVals[i][j] = val[i][j] + valSpeed[i][j] * dt;
            }
        }
    }

    val = newVals;
}


double Field::getValAt(glm::dvec2 v) {

    if(v.x < min.x || v.y < min.y || v.x > max.x || v.y > max.y) {
        return 0;
    }

    glm::dvec2 scaledPos = (v - min) / step;

    double bottomLeft = getAtIndex(glm::uvec2((int) scaledPos.x, (int) scaledPos.y));
    double topLeft = getAtIndex(glm::uvec2((int) scaledPos.x, (int) scaledPos.y + 1));
    double bottomRight = getAtIndex(glm::uvec2((int) scaledPos.x + 1, (int) scaledPos.y));
    double topRight = getAtIndex(glm::uvec2((int) scaledPos.x + 1, (int) scaledPos.y + 1));

    double leftDiff = scaledPos.x - (int)scaledPos.x;
    double rightDiff = ((int)scaledPos.x + 1) - scaledPos.x;
    double bottomDiff = scaledPos.y - (int)scaledPos.y;
    double topDiff = ((int)scaledPos.y + 1) - scaledPos.y;

    double topAvg = rightDiff*topRight + leftDiff*topLeft;
    double bottomAvg = rightDiff*bottomRight + leftDiff*bottomLeft;

    return topDiff*topAvg + bottomDiff*bottomAvg;
}

double Field::getAtIndex(glm::uvec2 v) {
    if(v.x < 0 || v.y < 0 || v.x > val.size() || v.y > val[0].size()) {
        return 0;
    }

    return val[v.x][v.y];
}

glm::dvec2 Field::getMin() {
    return min;
}

double Field::getStep() {
    return step;
}

Field::~Field() {

}
