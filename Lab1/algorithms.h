#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include <vector>
#include "job.h"

int sortByR(std::vector<Job> & jobs);
int sortByD(std::vector<Job> & jobs);
std::pair<std::vector<Job>, int> bruteForce(std::vector<Job> & jobs);

#endif