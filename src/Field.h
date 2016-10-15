//
// Created by Ricardo Delfin on 10/15/16.
//

#ifndef WAVEEQUATION_FIELD_H
#define WAVEEQUATION_FIELD_H

#include <vector>
#include <glm/glm.hpp>

/**
 * This class represents a field that satisfied the wave equation ∂^2f/∂^2t = v^2 (∂^2f/∂^2x + ∂^2f/∂^2y)
 * It contains an array of discrete points on the field.
 */
class Field {
public:
    Field(glm::vec2 min, glm::vec2 max, double step, double speed);

    void setField(std::vector<std::vector<double>> f0);

    void update(double dt);

    ~Field();

private:
    std::vector<std::vector<double>> val, valSpeed;
    glm::vec2 min, max;
    double speed, step;
};


#endif //WAVEEQUATION_FIELD_H
