//
// Created by Igor Frank on 13.12.19.
//

#include "SimulationCompute.h"
#include <Cell.h>
#include <IboBuffer.h>
#include <Utilities.h>
#include <geGL/StaticCalls.h>
#include <geGL_utils.h>
#include <glm/gtx/component_wise.hpp>
#include <shader_literals.h>
#include <types/Range.h>
#include <utility>

using namespace ge::gl;

SimulationCompute::SimulationCompute(const glm::uvec3 tankSize, BufferPtr ibo, BufferPtr positionBuffer)
    : ibo(std::move(ibo)), positionsBuffer(std::move(positionBuffer)), tankSize(tankSize) {
  using namespace ShaderLiterals;
  horizontalProgram = std::make_shared<ge::gl::Program>("basic-horizontal"_comp);
  verticalProgram = std::make_shared<ge::gl::Program>("basic-vertical"_comp);

  initBuffers(glm::compMul(tankSize));
}

void SimulationCompute::simulate() {
  const glm::uvec3 localSizes{2, 2, 2};
  /*  [[maybe_unused]] Cell *ptrRD;
    [[maybe_unused]] Cell *ptrWR;
    ptrWR = reinterpret_cast<Cell *>(cellBuffers[0]->map(GL_READ_WRITE));
    ptrRD = reinterpret_cast<Cell *>(cellBuffers[1]->map(GL_READ_WRITE));

    cellBuffers[0]->unmap();
    cellBuffers[1]->unmap();*/

  horizontalProgram->use();
  cellBuffers[0]->bindBase(GL_SHADER_STORAGE_BUFFER, 0);
  cellBuffers[1]->bindBase(GL_SHADER_STORAGE_BUFFER, 1);
  positionsBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 2);
  ibo->bindBase(GL_SHADER_STORAGE_BUFFER, 3);

  glDispatchCompute(tankSize.x / localSizes.x, tankSize.y / localSizes.y, tankSize.z / localSizes.z);

  glMemoryBarrier(GL_COMMAND_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);

  /*  ptrWR = reinterpret_cast<Cell *>(cellBuffers[0]->map(GL_READ_WRITE));
    ptrRD = reinterpret_cast<Cell *>(cellBuffers[1]->map(GL_READ_WRITE));

    cellBuffers[0]->unmap();
    cellBuffers[1]->unmap();*/

  verticalProgram->use();
  swapBuffers();
  cellBuffers[0]->bindBase(GL_SHADER_STORAGE_BUFFER, 0);
  cellBuffers[1]->bindBase(GL_SHADER_STORAGE_BUFFER, 1);
  glDispatchCompute(tankSize.x / localSizes.x, tankSize.y / localSizes.y, tankSize.z / localSizes.z);

  glMemoryBarrier(GL_COMMAND_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);

  /*  ptrWR = reinterpret_cast<Cell *>(cellBuffers[0]->map(GL_READ_WRITE));
    ptrRD = reinterpret_cast<Cell *>(cellBuffers[1]->map(GL_READ_WRITE));

    cellBuffers[0]->unmap();
    cellBuffers[1]->unmap();*/

  glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void SimulationCompute::initBuffers(int size) {
  auto cells = std::vector<Cell>(size);
  cellBuffers = {createBuffer(cells, GL_DYNAMIC_COPY), createBuffer(cells, GL_DYNAMIC_COPY)};
}

void SimulationCompute::setFluidVolume(int index, float volume) {
  auto index3D = Utilities::from1Dto3Dindex(index, tankSize);
  setFluidVolume(std::vector<glm::uvec3>{index3D}, std::vector<float>{volume});
}

void SimulationCompute::setFluidVolume(glm::vec3 index, float volume) {
  setFluidVolume(std::vector<glm::uvec3>{index}, std::vector<float>{volume});
}

void SimulationCompute::setFluidVolume(std::vector<glm::uvec3> indices, std::vector<float> volumes) {
  using namespace MakeRange;

  auto ptrWR = reinterpret_cast<Cell *>(cellBuffers[0]->map(GL_READ_WRITE));
  auto ptrRD = reinterpret_cast<Cell *>(cellBuffers[1]->map(GL_READ_WRITE));
  auto ptrPos = reinterpret_cast<glm::vec4 *>(positionsBuffer->map(GL_READ_WRITE));
  auto ptrIbo = reinterpret_cast<DrawElementsIndirectCommand *>(ibo->map(GL_READ_WRITE));

  auto count = ptrIbo->instanceCount;
  for (auto i : range(indices.size())) {
    auto linearIndex = indices[i].x + indices[i].y * tankSize.x + indices[i].z * tankSize.y * tankSize.z;
    ptrRD[linearIndex].setFluidVolume(volumes[i]);
    ptrWR[linearIndex].setFluidVolume(volumes[i]);
    if (volumes[i] > 0.0) {
      for (auto j : range<unsigned int>(0, count)) {
        if (ptrPos[j].x == indices[i].x && ptrPos[j].y == indices[i].y && ptrPos[j].z == indices[i].z) {
          ptrPos[j].w = volumes[i];
          break;
        }
        if (count == j) {
          ptrPos[count + i] = glm::vec4(indices[i], volumes[i]);
          ptrIbo->instanceCount += 1;
        }
      }
    } else {
      std::vector<glm::vec4> newPos{glm::compMul(tankSize)};
      for (auto i : range(count)) {
        if (ptrPos[i].x == indices[i].x && ptrPos[i].y == indices[i].y && ptrPos[i].z == indices[i].z) {
          ptrIbo->instanceCount -= 1;
          continue;
        } else {
          newPos.emplace_back(ptrPos[i]);
        }
      }
      ptrPos = newPos.data();
    }
  }

  cellBuffers[0]->unmap();
  cellBuffers[1]->unmap();
  positionsBuffer->unmap();
  ibo->unmap();
}

void SimulationCompute::swapBuffers() { std::swap(cellBuffers[0], cellBuffers[1]); }

void SimulationCompute::reset() {
  initBuffers(glm::compMul(tankSize));
  [[maybe_unused]] auto ptrPos = reinterpret_cast<glm::vec4 *>(positionsBuffer->map(GL_READ_WRITE));
  [[maybe_unused]] auto ptrIbo = reinterpret_cast<DrawElementsIndirectCommand *>(ibo->map(GL_READ_WRITE));

  ptrPos = std::vector<glm::vec4>{glm::compMul(tankSize), glm::vec4(-1)}.data();
  ptrIbo->instanceCount = 0;

  positionsBuffer->unmap();
  ibo->unmap();
}
