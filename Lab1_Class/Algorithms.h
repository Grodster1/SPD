#ifndef ALGORITHMS_H_
#define ALGORITHMS_H_

#include "Problem.h"

Solution sortByR(const Instance& inst);
Solution sortByD(const Instance& inst);
Solution bruteForce(const Instance& inst);
Solution schrage(const Instance& inst);
Solution vegasSort(const Instance& inst);

#endif