#ifndef JOB_H
#define JOB_H
#include <vector>
#include <string>

struct Job
{
    int id;
    int r; //release date
    int p; //processing time
    int d; //due date
};

int computeLMax(const std::vector<Job>& schedule);
std::vector<Job> loadData(const std::string& fileName);

#endif