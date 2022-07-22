#include <glm/glm.hpp>
#include <stdio.h>

int main() {
    glm::vec3 tmp(1, 2, 3);
    glm::vec3 tmp2(4, 5, 6);

    tmp *= tmp2;

    printf("%f %f %f\n", tmp.x, tmp.y, tmp.z);

    return 0;
}