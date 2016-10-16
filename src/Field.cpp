//
// Created by Ricardo Delfin on 10/15/16.
//

#include "Field.h"

Field::Field(glm::vec2 min, glm::vec2 max, double step, double speed)
    : speed(speed), step(step) {
    glm::vec2 diff = max - min;
    glm::uvec2 totalSteps = glm::uvec2(diff.x / steps, diff.y / steps);
    val = std::vector<std::vector<double>>(totalSteps.x);
    valSpeed = std::vector<std::vector<double>>(totalSteps.x);
    for(int i = 0; i < totalSteps.x; i++) {
        val[i] = std::vector<double>(totalSteps.y);
        valSpeed[i] = std::vector<double>(totalSteps.y);
        for(int j = 0; j < totalSteps.y; j++) {
            val[i][j] = 0;
            valSpeed[i][j] = 0;
        }
    }

    this->min = min;
    this->max = min + step*totalSteps;
}

void Field::setField(std::vector<std::vector<double>> f0) {
    data = f0;
}

/* Computes the position of every pixel based on the wave equation */
const std::vector<std::vector<double>>& Field::update(double dt) {
    std::vector<std::vector<double>> newVals(val.size());
    for(int i = 0; i < val.size(); i++) {
        newVals[i] = std::vector<double>(val[i].size());
        for (int j = 0; j < val[i].size(); j++) {
            if(i == 0 || j == 0 || i == val.size() - 1 || j == val.size() - 1) {
                val[i][j] = 0;
            } else {
                double dfdxRight = (val[i + 1][j] - val[i][j]) / step;
                double dfdxLeft = (val[i][j] - val[i - 1][j]) / step;
                double d2fdx2 = (dfdxRight - dfdxRight) / step;

                double dfdyUp = (val[i][j + 1] - val[i][j]) / step;
                double dfdyDown = (val[i][j] - val[i][j - 1]) / step;
                double d2fdy2 = (dfdyUp - dfdyDown) / step;

                double d2fdt2 = speed * (d2fdy2 + d2fdx2);

                valSpeed[i][j] += d2fdt2 * dt;
                newVals[i][j] += valSpeed[i][j] * dt;
            }
        }
    }

    data = newVals;

    return data;
}

glm::vec2 Field::getMin() {
    return min;
}

double Field::getStep() {
    return step;
}

Field::~Field() {

}
