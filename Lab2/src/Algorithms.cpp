#include "Algorithms.hh"
#include <algorithm>
#include <numeric>
#include <iostream>
#include <climits>

std::pair<std::vector<int>, int> Algorithms::johnson(const FlowShopProblem& problem) {
    if (problem.getNumMachines() != 2) {
        std::cerr << "Johnson requires 2 machines!\n";
        return {{}, -1};
    }

    std::vector<int> jl, jr;
    for (int j = 0; j < problem.getNumJobs(); ++j) {
        if (problem.getDuration(j, 0) < problem.getDuration(j, 1)) {
            jl.push_back(j);
        } else {
            jr.push_back(j);
        }
    }

    // Sort JL in ascending order
    std::sort(jl.begin(), jl.end(), [&](int a, int b) {
        return problem.getDuration(a, 0) < problem.getDuration(b, 0);
    });

    // Sort JR in descending order
    std::sort(jr.begin(), jr.end(), [&](int a, int b) {
        return problem.getDuration(a, 1) > problem.getDuration(b, 1);
    });

    // Concatenate
    std::vector<int> resultPermutation = jl;
    resultPermutation.insert(resultPermutation.end(), jr.begin(), jr.end());

    int cmax = problem.calculateCmax(resultPermutation);
    return {resultPermutation, cmax};
}

std::pair<std::vector<int>, int> Algorithms::bruteForce(const FlowShopProblem& problem) {
    int numJobs = problem.getNumJobs();
    
    // Create initial permutation [0, 1, 2, ..., n-1]
    std::vector<int> permutation(numJobs);
    std::iota(permutation.begin(), permutation.end(), 0);
    
    int bestCmax = problem.calculateCmax(permutation);
    std::vector<int> bestPermutation = permutation;
    
    // Generate all permutations and find the best one
    while (std::next_permutation(permutation.begin(), permutation.end())) {
        int cmax = problem.calculateCmax(permutation);
        if (cmax < bestCmax) {
            bestCmax = cmax;
            bestPermutation = permutation;
        }
    }
    
    return {bestPermutation, bestCmax};
}

std::pair<std::vector<int>, int> Algorithms::neh(const FlowShopProblem& problem) {
    int numJobs = problem.getNumJobs();
    
    // Create job list with their total processing times
    std::vector<std::pair<int, int>> jobsWithTotalTime; // (jobIdx, totalTime)
    
    for (int j = 0; j < numJobs; ++j) {
        int totalTime = 0;
        for (int m = 0; m < problem.getNumMachines(); ++m) {
            totalTime += problem.getDuration(j, m);
        }
        jobsWithTotalTime.push_back({j, totalTime});
    }
    
    // Sort jobs in decreasing order of total processing time
    std::sort(jobsWithTotalTime.begin(), jobsWithTotalTime.end(), 
        [](const std::pair<int, int>& a, const std::pair<int, int>& b) {
            return a.second > b.second;
        });
    
    // Build partial permutation
    std::vector<int> sequence;
    
    // Insert first job
    sequence.push_back(jobsWithTotalTime[0].first);
    
    // Insert remaining jobs one by one
    for (int i = 1; i < numJobs; ++i) {
        int jobToInsert = jobsWithTotalTime[i].first;
        
        int bestPos = 0;
        int bestCmax = INT_MAX;
        
        // Try inserting the job at each position
        for (int pos = 0; pos <= sequence.size(); ++pos) {
            std::vector<int> tempSequence = sequence;
            tempSequence.insert(tempSequence.begin() + pos, jobToInsert);
            
            int cmax = problem.calculateCmax(tempSequence);
            if (cmax < bestCmax) {
                bestCmax = cmax;
                bestPos = pos;
            }
        }
        
        // Insert at the best position
        sequence.insert(sequence.begin() + bestPos, jobToInsert);
    }
    
    int cmax = problem.calculateCmax(sequence);
    return {sequence, cmax};
}