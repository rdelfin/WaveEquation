//
// Created by Ricardo Delfin on 10/15/16.
//

#ifndef WAVEEQUATION_FIELD_H
#define WAVEEQUATION_FIELD_H

#include <vector>
#include <glm/glm.hpp>

/**
 * This class represents a field that satisfied the wave equation ∂f/∂t = v^2 ∂f/∂x
 * It contains an array of discrete points on the field.
 */
class Field {
public:
    Field(glm::vec2 min, glm::vec2 max, double step, double speed);

    void setField(std::vector<std::vector<double>> f0);

    void update(double dt);

    ~Field();

private:
    std::vector<std::vector<double>> data;
    glm::vec2 min, max;
    double speed;
};


#endif //WAVEEQUATION_FIELD_H
