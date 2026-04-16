#ifndef PROBLEM_H_
#define PROBLEM_H_

#include <vector>
#include <string>

struct Job{
    int id;
    int r; //release date
    int p; //processing time
    int d; //due date
};

class Instance{
public:
    std::vector<Job> jobs;
    static Instance fromFile(const std::string& filename);
    static Instance fromFile(const std::string& filename, int n);
    int size() const {return jobs.size();}
    void print() const;

};

class Solution{
public: 
    std::vector<Job> schedule;
    int lmax;

    void computeLmax();
    void print() const;
};




#endif