#include "Algorithms.hh"
#include <algorithm>
#include <numeric>
#include <iostream>
#include <climits>

// Johnson's Algorithm for 2-machine flow shop
// How it works: Divides jobs into two groups based on processing times,
// sorts them (ascending for first machine, descending for second),
// then concatenates the groups to create the optimal schedule for m=2.
std::pair<std::vector<int>, int> Algorithms::johnson(const FlowShopProblem& problem) {
    // Johnson's algorithm is optimal only for 2-machine problems.
    if (problem.getNumMachines() != 2) {
        std::cerr << "Johnson requires 2 machines!\n";
        return {{}, -1};
    }

    // Partition jobs into two sets.
    // JL: jobs where processing time on machine 1 is less than on machine 2.
    // JR: all other jobs.
    std::vector<int> jl, jr;
    for (int j = 0; j < problem.getNumJobs(); ++j) {
        if (problem.getDuration(j, 0) < problem.getDuration(j, 1)) {
            jl.push_back(j);
        } else {
            jr.push_back(j);
        }
    }

    // Sort JL in ascending order of processing time on machine 1.
    std::sort(jl.begin(), jl.end(), [&](int a, int b) {
        return problem.getDuration(a, 0) < problem.getDuration(b, 0);
    });

    // Sort JR in descending order of processing time on machine 2.
    std::sort(jr.begin(), jr.end(), [&](int a, int b) {
        return problem.getDuration(a, 1) > problem.getDuration(b, 1);
    });

    // The optimal sequence is the concatenation of sorted JL and sorted JR.
    std::vector<int> resultPermutation = jl;
    resultPermutation.insert(resultPermutation.end(), jr.begin(), jr.end());

    // Calculate the makespan (Cmax) for the optimal permutation.
    int cmax = problem.calculateCmax(resultPermutation);
    return {resultPermutation, cmax};
}

// Brute Force Algorithm
// How it works: Generates all possible job permutations and evaluates each one,
// keeping track of the best (minimum Cmax) solution found. 
// Optimal but extremely slow for large instances (exponential complexity).
std::pair<std::vector<int>, int> Algorithms::bruteForce(const FlowShopProblem& problem) {
    int numJobs = problem.getNumJobs();
    
    // Create an initial permutation (e.g., 0, 1, 2, ..., n-1).
    std::vector<int> permutation(numJobs);
    std::iota(permutation.begin(), permutation.end(), 0);
    
    // Calculate Cmax for the initial permutation and set it as the best found so far.
    int bestCmax = problem.calculateCmax(permutation);
    std::vector<int> bestPermutation = permutation;
    
    // Iterate through all possible permutations of the jobs.
    while (std::next_permutation(permutation.begin(), permutation.end())) {
        // Calculate Cmax for the current permutation.
        int cmax = problem.calculateCmax(permutation);
        // If the current permutation is better than the best one found so far, update the best.
        if (cmax < bestCmax) {
            bestCmax = cmax;
            bestPermutation = permutation;
        }
    }
    
    // Return the best permutation and its Cmax.
    return {bestPermutation, bestCmax};
}

// NEH Algorithm
// How it works: Sorts jobs by total processing time (descending), then iteratively
// inserts each job into the best position in the partial schedule to minimize Cmax.
// Fast heuristic that produces near-optimal solutions.
std::pair<std::vector<int>, int> Algorithms::neh(const FlowShopProblem& problem) {
    int numJobs = problem.getNumJobs();
    int numMachines = problem.getNumMachines();
    
    // Create a list of jobs with their total processing times across all machines.
    std::vector<std::pair<int, int>> jobsWithTotalTime;
    
    for (int j = 0; j < numJobs; ++j) {
        int totalTime = 0;
        for (int m = 0; m < numMachines; ++m) {
            totalTime += problem.getDuration(j, m);
        }
        jobsWithTotalTime.push_back({j, totalTime});
    }
    
    // Sort jobs in descending order of their total processing time.
    std::sort(jobsWithTotalTime.begin(), jobsWithTotalTime.end(), 
        [](const std::pair<int, int>& a, const std::pair<int, int>& b) {
            return a.second > b.second;
        });
    
    // Start with an empty sequence and add jobs one by one.
    // The first job in the sequence is the one with the highest total processing time.
    std::vector<int> sequence;
    sequence.push_back(jobsWithTotalTime[0].first);
    
    // For each subsequent job from the sorted list...
    for (int i = 1; i < numJobs; ++i) {
        int jobToInsert = jobsWithTotalTime[i].first;
        
        int bestPos = 0;
        int bestCmax = INT_MAX;
        
        // ...find the best position to insert it in the current sequence.
        // This involves checking every possible insertion point.
        for (int pos = 0; pos <= (int)sequence.size(); ++pos) {
            // Calculate the Cmax that would result from inserting the job at this position.
            std::vector<std::vector<int>> completionTimes(numJobs, std::vector<int>(numMachines, 0));
            int cmax = calculateCmaxInsertion(problem, sequence, jobToInsert, pos, completionTimes);
            
            // If this position gives a better Cmax, update the best position and Cmax.
            if (cmax < bestCmax) {
                bestCmax = cmax;
                bestPos = pos;
            }
        }
        
        // Insert the job into the best position found.
        sequence.insert(sequence.begin() + bestPos, jobToInsert);
    }
    
    // After all jobs are inserted, calculate the final Cmax and return the sequence.
    int cmax = problem.calculateCmax(sequence);
    return {sequence, cmax};
}

// QNEH (Quick NEH) Algorithm
// How it works: Similar to NEH, sorts jobs by total processing time (descending),
// then iteratively inserts each job into the best position.
// Same quality as NEH but optimized implementation with matrix caching.
std::pair<std::vector<int>, int> Algorithms::qneh(const FlowShopProblem& problem) {
    int numJobs = problem.getNumJobs();
    int numMachines = problem.getNumMachines();
    
    // Calculate total processing time for each job.
    std::vector<std::pair<int, int>> jobsWithTotalTime;
    for (int j = 0; j < numJobs; ++j) {
        int totalTime = 0;
        for (int m = 0; m < numMachines; ++m) {
            totalTime += problem.getDuration(j, m);
        }
        jobsWithTotalTime.push_back({j, totalTime});
    }
    std::sort(jobsWithTotalTime.begin(), jobsWithTotalTime.end(),
        [](const std::pair<int, int>& a, const std::pair<int, int>& b) {
            return a.second > b.second;
        });
    
    std::vector<int> sequence;
    sequence.push_back(jobsWithTotalTime[0].first);
    
    for (int i = 1; i < numJobs; ++i) {
        int jobToInsert = jobsWithTotalTime[i].first;
        int k = sequence.size();
        
        // E Matrix
        std::vector<std::vector<int>> E(k + 1, std::vector<int>(numMachines, 0));
        for (int p = 1; p <= k; ++p) {
            int job = sequence[p - 1];
            E[p][0] = E[p - 1][0] + problem.getDuration(job, 0);
            for (int m = 1; m < numMachines; ++m) {
                E[p][m] = std::max(E[p][m - 1], E[p - 1][m]) + problem.getDuration(job, m);
            }
        }
        
        // Q Matrix
        std::vector<std::vector<int>> Q(k + 2, std::vector<int>(numMachines, 0));
        for (int p = k; p >= 1; --p) {
            int job = sequence[p - 1];
            Q[p][numMachines - 1] = Q[p + 1][numMachines - 1] + problem.getDuration(job, numMachines - 1);
            for (int m = numMachines - 2; m >= 0; --m) {
                Q[p][m] = std::max(Q[p][m + 1], Q[p + 1][m]) + problem.getDuration(job, m);
            }
        }
        
        int bestPos = 0;
        int bestCmax = INT_MAX;
        
        for (int pos = 0; pos <= k; ++pos) {
            int currentCmax = 0;
            int jobEndOnMachine = 0;
            
            for (int m = 0; m < numMachines; ++m) {
                int startOnM = std::max(jobEndOnMachine, E[pos][m]);
                jobEndOnMachine = startOnM + problem.getDuration(jobToInsert, m);
                
                int possibleCmax = jobEndOnMachine + Q[pos + 1][m];
                if (possibleCmax > currentCmax) {
                    currentCmax = possibleCmax;
                }
            }
            
            if (currentCmax < bestCmax) {
                bestCmax = currentCmax;
                bestPos = pos;
            }
        }
        
        sequence.insert(sequence.begin() + bestPos, jobToInsert);
    }
    
    int finalCmax = problem.calculateCmax(sequence);
    return {sequence, finalCmax};
}

std::pair<std::vector<int>, int> Algorithms::tabuSearch(const FlowShopProblem& problem, int maxIterations, int tabuTenure) {
    int numJobs = problem.getNumJobs();
    
    auto initialSol = qneh(problem); 
    std::vector<int> bestSeq = initialSol.first;
    int bestCmax = initialSol.second;
    
    std::vector<int> currentSeq = bestSeq;
    
    std::vector<std::vector<int>> tabuList(numJobs, std::vector<int>(numJobs, 0));
    
    for (int iter = 0; iter < maxIterations; ++iter) {
        int bestNeighborCmax = INT_MAX;
        int bestSwapI = -1;
        int bestSwapJ = -1;
        
        for (int i = 0; i < numJobs - 1; ++i) {
            for (int j = i + 1; j < numJobs; ++j) {
                
                std::vector<int> neighbor = currentSeq;
                std::swap(neighbor[i], neighbor[j]);
                int neighborCmax = problem.calculateCmax(neighbor);
                
                int job1 = currentSeq[i];
                int job2 = currentSeq[j];
                
                bool isTabu = (tabuList[job1][job2] > iter);
            
                if (!isTabu || neighborCmax < bestCmax) {
                    if (neighborCmax < bestNeighborCmax) {
                        bestNeighborCmax = neighborCmax;
                        bestSwapI = i;
                        bestSwapJ = j;
                    }
                }
            }
        }
        
        // If a valid best neighbor was found...
        if (bestSwapI != -1) {
            int job1 = currentSeq[bestSwapI];
            int job2 = currentSeq[bestSwapJ];
            
            // ...move to that neighbor.
            std::swap(currentSeq[bestSwapI], currentSeq[bestSwapJ]);
            
            // Add the reverse move to the tabu list to forbid it for a number of iterations.
            tabuList[job1][job2] = iter + tabuTenure;
            tabuList[job2][job1] = iter + tabuTenure;
            
            // If the new solution is the best found so far in the entire search, update the best solution.
            if (bestNeighborCmax < bestCmax) {
                bestCmax = bestNeighborCmax;
                bestSeq = currentSeq;
            }
        } else {
            // If no non-tabu improving neighbor is found, stop the search.
            break; 
        }
    }
    
    // Return the best solution found during the search.
    return {bestSeq, bestCmax};
}

// Lower Bound Calculation for Branch & Bound
// How it works: Computes an optimistic lower bound for a partial schedule
// by adding actual completion times of scheduled jobs and minimum processing
// times for remaining unscheduled jobs.
int Algorithms::calculateLowerBound(const FlowShopProblem& problem,
                                   const std::vector<int>& partial,
                                   const std::vector<bool>& used) {
    int m = problem.getNumMachines();
    int n = problem.getNumJobs();
    
    // First, calculate the Cmax of the already scheduled (partial) sequence.
    std::vector<int> completionTimes(m, 0);
    
    for (int job : partial) {
        completionTimes[0] += problem.getDuration(job, 0);
        for (int machine = 1; machine < m; ++machine) {
            completionTimes[machine] = std::max(completionTimes[machine], 
                                               completionTimes[machine - 1]) + 
                                       problem.getDuration(job, machine);
        }
    }
    
    // To get a lower bound, add an optimistic estimate for the remaining jobs.
    // Here, we sum the minimum processing time of each unscheduled job.
    // This is a simple, weak lower bound, but still valid.
    for (int job = 0; job < n; ++job) {
        if (!used[job]) {
            int minDuration = INT_MAX;
            for (int machine = 0; machine < m; ++machine) {
                minDuration = std::min(minDuration, problem.getDuration(job, machine));
            }
            completionTimes[m - 1] += minDuration;
        }
    }
    
    // The result is the Cmax of the partial schedule plus the optimistic estimate for the rest.
    return completionTimes[m - 1];
}

// Branch & Bound Helper (Recursive Search)
// How it works: Recursively builds partial schedules by adding one job at a time.
// Uses lower bound to prune branches that cannot lead to better solutions than current best.
// Limits node count to prevent excessive computation.
void Algorithms::branchAndBoundHelper(const FlowShopProblem& problem,
                                     std::vector<int>& current,
                                     std::vector<bool>& used,
                                     int currentCmax,
                                     int& bestCmax,
                                     std::vector<int>& bestSolution,
                                     int& nodeCount) {
    int n = problem.getNumJobs();
    
    // Increment the count of explored nodes.
    nodeCount++;
    
    // Stop the search if it's taking too long (exploring too many nodes).
    if (nodeCount > 50000) {
        return;
    }
    
    // Base case: if a full permutation has been created.
    if ((int)current.size() == n) {
        // If this complete solution is better than the best one found so far, update the best.
        if (currentCmax < bestCmax) {
            bestCmax = currentCmax;
            bestSolution = current;
        }
        return;
    }
    
    // Pruning step: calculate a lower bound for any solution that can be built from this partial schedule.
    int lowerBound = calculateLowerBound(problem, current, used);
    
    // If the lower bound is already worse than our best solution, there's no point exploring this branch further.
    if (lowerBound >= bestCmax) {
        return;
    }
    
    // Recursive step: try adding each unscheduled job to the current partial solution.
    for (int job = 0; job < n; ++job) {
        if (!used[job]) {
            // Add the job to the current sequence.
            current.push_back(job);
            used[job] = true;
            
            // Calculate the Cmax of the new, longer partial sequence.
            int newCmax = problem.calculateCmax(current);
            
            // Recursively call the function to continue building the sequence.
            branchAndBoundHelper(problem, current, used, newCmax, bestCmax, bestSolution, nodeCount);
            
            // Backtrack: remove the job to explore other possibilities.
            current.pop_back();
            used[job] = false;
        }
    }
}

// Branch & Bound Algorithm
// How it works: Systematic search using DFS that explores the space of all possible
// job orderings. Uses lower bounds to prune unpromising branches early.
// Initializes upper bound with NEH heuristic for faster convergence.
std::pair<std::vector<int>, int> Algorithms::branchAndBound(const FlowShopProblem& problem) {
    int n = problem.getNumJobs();
    
    // Get a good initial solution (upper bound) using a fast heuristic.
    // This makes pruning much more effective.
    auto nehResult = neh(problem);
    int bestCmax = nehResult.second;
    std::vector<int> bestSolution = nehResult.first;
    
    // Initialize structures for the recursive search.
    std::vector<int> current;
    std::vector<bool> used(n, false);
    int nodeCount = 0;
    
    // Start the recursive search process.
    branchAndBoundHelper(problem, current, used, 0, bestCmax, bestSolution, nodeCount);
    
    // Return the best solution found.
    return {bestSolution, bestCmax};
}

// Calculates Cmax for a sequence with a new job inserted at a specific position.
// This is a helper for the basic NEH algorithm.
int Algorithms::calculateCmaxInsertion(const FlowShopProblem& problem,
                                       const std::vector<int>& sequence,
                                       int jobToInsert,
                                       int position,
                                       std::vector<std::vector<int>>&) { // The completionTimes matrix is unused.
    
    // Create a temporary copy of the current sequence.
    std::vector<int> tempSequence = sequence;
    // Insert the new job at the specified position.
    tempSequence.insert(tempSequence.begin() + position, jobToInsert);
    
    // Calculate Cmax for the new full sequence from scratch.
    return problem.calculateCmax(tempSequence);
}