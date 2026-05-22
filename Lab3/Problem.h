#ifndef PROBLEM_H_
#define PROBLEM_H_

#include <vector>
#include <string>

struct Job{
    int id;
    int p; //processing time
};

class Instance{
public:
    std::vector<Job> jobs;
    int m;
    static Instance fromFile(const std::string& filename);
    static Instance fromFile(const std::string& filename, int n);
    static Instance generateRandom(int n, int m, int maxP);
    int size() const {return jobs.size();}
    int totalP() const;
    void print() const;
};

class Solution{
public: 
    int cmax;
    int m;

    std::vector<int> assignment;
    std::vector<int> loads;

    Solution() : cmax(0), m(0) {};
    Solution(int numMachines);

    void assign(int jobID, int p, int machine);
    void computeCMax(const std::vector<Job>& jobs);
    void print() const;
};




#endif