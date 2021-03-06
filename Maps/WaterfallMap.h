//
// Created by Igor Frank on 06.01.20.
//

#ifndef GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_WATERFALLMAP_H
#define GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_WATERFALLMAP_H

#include "BaseMap.h"
class WaterfallMap : public BaseMap{
public:
  void setup(SimulationCompute &simulation, glm::uvec3 tankSize) override;

  void operator()(SimulationCompute &simulation, Time time) override ;

  MapType getType() override ;
};

#endif // GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_WATERFALLMAP_H
