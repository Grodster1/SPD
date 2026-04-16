#include "Problem.h"
#include <fstream>
#include <iostream>
#include <climits>
#include <algorithm>


Instance Instance::fromFile(const std::string& filename){
    std::ifstream file(filename);
    if(!file.is_open()){
        std::cerr <<"Can't open file: " << filename << std::endl;
        return {};
    }

    Instance inst;
    int n;
    file >> n;
    inst.jobs.resize(n);
    for(int i =0; i < n; ++i){
        inst.jobs[i].id = i;
        file >> inst.jobs[i].r >> inst.jobs[i].p >> inst.jobs[i].d;
    }
    return inst;
}

Instance Instance::fromFile(const std::string& filename, int n){
    std:: ifstream file(filename);
    if(!file.is_open()){
        std::cerr << "Can't open file: " << filename << std::endl;
        return {};
    }

    //Struktura dla plików testowych
    int total;
    file >> total;
    if (n > total) {
        std::cerr << "Requested " << n << " jobs, but file has " << total << std::endl;
        return {};
    }
 
    Instance inst;
    inst.jobs.resize(n);
 
    for (int i = 0; i < n; ++i) {
        inst.jobs[i].id = i;
        file >> inst.jobs[i].r >> inst.jobs[i].p >> inst.jobs[i].d;
    }
 
    return inst;
}


void Instance::print() const{
    std::cout << "Instance (" << size() << " jobs):" << std::endl;
    for(const auto& j:jobs){
        std::cout << " Job " << j.id << " r=" << j.r << " p=" << j.p << " d=" << j.d << std::endl;
    }
}

void Solution::computeLmax(){
    int t = 0;
    lmax = INT_MIN;
    for(const auto& j: schedule){
        t = std::max(t, j.r);
        t += j.p;
        int L = t-j.d;
        lmax = std::max(lmax, L);
    }
}
