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

int Algorithms::calculateLowerBound(const FlowShopProblem& problem,
                                   const std::vector<int>& partial,
                                   const std::vector<bool>& used) {
    int m = problem.getNumMachines();
    int n = problem.getNumJobs();
    
    std::vector<int> completionTimes(m, 0);
    
    for (int job : partial) {
        completionTimes[0] += problem.getDuration(job, 0);
        for (int machine = 1; machine < m; ++machine) {
            completionTimes[machine] = std::max(completionTimes[machine], 
                                               completionTimes[machine - 1]) + 
                                       problem.getDuration(job, machine);
        }
    }
    
    // Lower bound: add sum of minimum processing times for remaining jobs
    for (int job = 0; job < n; ++job) {
        if (!used[job]) {
            int minDuration = INT_MAX;
            for (int machine = 0; machine < m; ++machine) {
                minDuration = std::min(minDuration, problem.getDuration(job, machine));
            }
            completionTimes[m - 1] += minDuration;
        }
    }
    
    return completionTimes[m - 1];
}

void Algorithms::branchAndBoundHelper(const FlowShopProblem& problem,
                                     std::vector<int>& current,
                                     std::vector<bool>& used,
                                     int currentCmax,
                                     int& bestCmax,
                                     std::vector<int>& bestSolution,
                                     int& nodeCount) {
    int n = problem.getNumJobs();
    
    nodeCount++;
    
    // Limit nodes to prevent excessive search
    if (nodeCount > 50000) {
        return;
    }
    
    // Base case: all jobs scheduled
    if ((int)current.size() == n) {
        if (currentCmax < bestCmax) {
            bestCmax = currentCmax;
            bestSolution = current;
        }
        return;
    }
    
    // Calculate lower bound for pruning
    int lowerBound = calculateLowerBound(problem, current, used);
    
    // Prune if lower bound exceeds best known solution
    if (lowerBound >= bestCmax) {
        return;
    }
    
    // Try adding each unscheduled job
    for (int job = 0; job < n; ++job) {
        if (!used[job]) {
            current.push_back(job);
            used[job] = true;
            
            int newCmax = problem.calculateCmax(current);
            
            branchAndBoundHelper(problem, current, used, newCmax, bestCmax, bestSolution, nodeCount);
            
            current.pop_back();
            used[job] = false;
        }
    }
}

std::pair<std::vector<int>, int> Algorithms::branchAndBound(const FlowShopProblem& problem) {
    int n = problem.getNumJobs();
    
    // Initialize with NEH heuristic as upper bound
    auto nehResult = neh(problem);
    int bestCmax = nehResult.second;
    std::vector<int> bestSolution = nehResult.first;
    
    // Start branch and bound search
    std::vector<int> current;
    std::vector<bool> used(n, false);
    int nodeCount = 0;
    
    branchAndBoundHelper(problem, current, used, 0, bestCmax, bestSolution, nodeCount);
    
    return {bestSolution, bestCmax};
}

std::pair<std::vector<int>, int> Algorithms::m1(const FlowShopProblem& problem) {
    int numJobs = problem.getNumJobs();
    
    // M1: Sort jobs by ascending order of first machine processing time
    std::vector<std::pair<int, int>> jobsWithTime; // (jobIdx, p_j^1)
    
    for (int j = 0; j < numJobs; ++j) {
        jobsWithTime.push_back({j, problem.getDuration(j, 0)});
    }
    
    // Sort in ascending order by first machine time
    std::sort(jobsWithTime.begin(), jobsWithTime.end(),
        [](const std::pair<int, int>& a, const std::pair<int, int>& b) {
            return a.second < b.second;
        });
    
    // Build permutation from sorted jobs
    std::vector<int> permutation;
    for (const auto& pair : jobsWithTime) {
        permutation.push_back(pair.first);
    }
    
    int cmax = problem.calculateCmax(permutation);
    return {permutation, cmax};
}

std::pair<std::vector<int>, int> Algorithms::qneh(const FlowShopProblem& problem) {
    int numJobs = problem.getNumJobs();
    int numMachines = problem.getNumMachines();
    
    // QNEH: Sort jobs by sum of all machine times (similar to NEH but without iterative insertion)
    std::vector<std::pair<int, int>> jobsWithTotalTime; // (jobIdx, totalTime)
    
    for (int j = 0; j < numJobs; ++j) {
        int totalTime = 0;
        for (int m = 0; m < numMachines; ++m) {
            totalTime += problem.getDuration(j, m);
        }
        jobsWithTotalTime.push_back({j, totalTime});
    }
    
    // Sort jobs in decreasing order of total processing time
    std::sort(jobsWithTotalTime.begin(), jobsWithTotalTime.end(),
        [](const std::pair<int, int>& a, const std::pair<int, int>& b) {
            return a.second > b.second;
        });
    
    // Build permutation from sorted jobs (no iterative insertion like NEH)
    std::vector<int> permutation;
    for (const auto& pair : jobsWithTotalTime) {
        permutation.push_back(pair.first);
    }
    
    int cmax = problem.calculateCmax(permutation);
    return {permutation, cmax};
}