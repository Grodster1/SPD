#ifndef ALGORITHMS_HH
#define ALGORITHMS_HH

#include "FlowShopProblem.hh"
#include <vector>
#include <utility>

class Algorithms {
public:
    static std::pair<std::vector<int>, int> bruteForce(const FlowShopProblem& problem);
    static std::pair<std::vector<int>, int> neh(const FlowShopProblem& problem);
    static std::pair<std::vector<int>, int> qneh(const FlowShopProblem& problem);
    static std::pair<std::vector<int>, int> branchAndBound(const FlowShopProblem& problem);
    static std::pair<std::vector<int>, int> m1(const FlowShopProblem& problem);

    // For m=2 only
    static std::pair<std::vector<int>, int> johnson(const FlowShopProblem& problem);

private:
    static int calculateLowerBound(const FlowShopProblem& problem, 
                                   const std::vector<int>& partial,
                                   const std::vector<bool>& used);
    static void branchAndBoundHelper(const FlowShopProblem& problem,
                                     std::vector<int>& current,
                                     std::vector<bool>& used,
                                     int currentCmax,
                                     int& bestCmax,
                                     std::vector<int>& bestSolution,
                                     int& nodeCount);

};

#endif