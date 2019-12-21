//
// Created by Igor Frank on 13.12.19.
//

#ifndef GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_CELLRENDERER_H
#define GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_CELLRENDERER_H

#include <Camera.h>
#include <geGL/geGL.h>
#include <glm/glm.hpp>
#include <memory>
#include <vector>

class CellRenderer {
  using ProgramPtr = std::shared_ptr<ge::gl::Program>;
  using BufferPtr = std::shared_ptr<ge::gl::Buffer>;
  using VertexArrayPtr = std::shared_ptr<ge::gl::VertexArray>;

public:
  CellRenderer(const std::string &cellModelPath, const glm::mat4 &projectionMat, const glm::uvec3 &tankSize);

  void draw(const glm::mat4 &viewMat, const glm::vec3 &cameraPos, float cellSize = 0.5);

  [[nodiscard]] const BufferPtr &getIbo() const;

  [[nodiscard]] const BufferPtr &getPositionsBuffer() const;

private:
  ProgramPtr program;
  BufferPtr vbo;
  BufferPtr ebo;
  BufferPtr ibo;
  BufferPtr positionsBuffer;
  VertexArrayPtr vao = std::make_shared<ge::gl::VertexArray>();

  glm::mat4 projectionMat{};
};

#endif // GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_CELLRENDERER_H
