#ifndef ALGORITHMS_HH
#define ALGORITHMS_HH

#include "FlowShopProblem.hh"
#include <vector>
#include <utility>

class Algorithms {
public:
    static std::pair<std::vector<int>, int> bruteForce(const FlowShopProblem& problem);
    static std::pair<std::vector<int>, int> neh(const FlowShopProblem& problem);

    // For m=2 only
    static std::pair<std::vector<int>, int> johnson(const FlowShopProblem& problem);

};

#endif