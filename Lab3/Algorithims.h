#ifndef ALGORITHMS_H_
#define ALGORITHMS_H_

#include "Problem.h"
//3.0
Solution lsa(const Instance & inst);
Solution lpt(const Instance & inst);

//3.5
Solution dynamicProgP2(const Instance & inst);

//4.0
Solution bruteForceP2(const Instance & inst);

//4.5
Solution ptasP2(const Instance & inst);
Solution fptasP2(const Instance & inst);

#endif