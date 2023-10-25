#include <stack>

#include "glm/glm.hpp"

class MatrixStack {
public:
    static std::stack<glm::mat4> model;
    static std::stack<glm::mat4> projection;
};
