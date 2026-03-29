#include "algorithms.h"
#include <iostream>

int main(int argc, char* argv[]){
    std::vector<Job> jobs = loadData("../Data/test1.txt");
    for(const auto& j : jobs){
        std::cout << "ID: " << j.id << std::endl;
        std::cout << "R: " << j.r << std::endl;
        std::cout << "P: " << j.p << std::endl;
        std::cout << "D: " << j.d << std::endl;
        std::cout << std::endl;
    }
    
    // int LMaxR = sortByR(jobs);
    // std::cout << "Sorted by R:" << LMaxR << std::endl;
    // for(const auto& j : jobs){
    //     std::cout << "ID: " << j.id << std::endl;
    //     std::cout << "R: " << j.r << std::endl;
    //     std::cout << "P: " << j.p << std::endl;
    //     std::cout << "D: " << j.d << std::endl;
    //     std::cout << std::endl;
    // }

    // int LMaxD = sortByD(jobs);
    // std::cout << "Sorted by D:" << LMaxD << std::endl;
    // for(const auto& j : jobs){
    //     std::cout << "ID: " << j.id << std::endl;
    //     std::cout << "R: " << j.r << std::endl;
    //     std::cout << "P: " << j.p << std::endl;
    //     std::cout << "D: " << j.d << std::endl;
    //     std::cout << std::endl;
    // }

    std::pair<std::vector<Job>, int> bestPerm = bruteForce(jobs);
    std::cout << "Brute Force: " << bestPerm.second << std::endl;
    for(const auto& j : bestPerm.first){
        std::cout << "ID: " << j.id << std::endl;
        std::cout << "R: " << j.r << std::endl;
        std::cout << "P: " << j.p << std::endl;
        std::cout << "D: " << j.d << std::endl;
        std::cout << std::endl;
    }




    return 0;
}