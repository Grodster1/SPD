#ifndef INSTANCE_GENERATOR_HH
#define INSTANCE_GENERATOR_HH

#include "FlowShopProblem.hh"
#include <string>

class InstanceGenerator {
public:
    static FlowShopProblem generateAndSave(int jobs, int machines, int minTime, int maxTime, const std::string& filename);
};

#endif