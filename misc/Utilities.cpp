//
// Created by Igor Frank on 05.12.19.
//

#include "Utilities.h"
#include <fstream>

std::string Utilities::readFile(const std::string &filename) {
  std::ifstream file(filename, std::ios::ate | std::ios::binary);

  if (!file.is_open()) {
    throw std::runtime_error("failed to open file!");
  }

  size_t fileSize = static_cast<size_t>(file.tellg());
  std::string buffer(fileSize, ' ');

  file.seekg(0);
  file.read(buffer.data(), fileSize);

  file.close();

  return buffer;
}

glm::uvec3 Utilities::from1Dto3Dindex(unsigned int index, glm::uvec3 size) {
    return glm::vec3(index % size.x, (index / size.x) % size.y, index / ( size.x * size.y));
}
