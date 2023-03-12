#pragma once

#include <glm/glm.hpp>
#include <variant>

using UniformValue = std::variant<unsigned,
                                  int,
                                  float,
                                  glm::vec2,
                                  glm::vec3,
                                  glm::vec4,
                                  glm::mat2,
                                  glm::mat3,
                                  glm::mat4>;
