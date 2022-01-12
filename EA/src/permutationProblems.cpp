#include "pch.h"
#include "permutationProblems.h"
#include "graph.h"
#include "visualizer.h"
#include "utils.h"
#include "parserTSP.h"
#include "parser3SAT.h"
#include "lksearch.h"
#include "blackBoxEA.h"
#include <thread>
#include <unordered_map>
#include <omp.h>
namespace PermutationProblems 
{

  void StartVisualizer(Visualizer* visualizer)
  {
    if (visualizer != NULL)
      visualizer->StartVisualization();
  }

  void RunGraybox(const std::string& file)
  {

    struct Edge {
        Edge(uint32_t f, uint32_t t)
            : from(f), to(t) {}
        uint32_t from;
        uint32_t to;
    };
    struct Edge_hash {
        inline std::size_t operator()(const Edge& v) const {
            return v.from > v.to ? (v.from * 31 + v.to) : (v.to * 31 + v.from);
        }
    };
    struct Edge_equals {
        bool operator()(const Edge& v1, const Edge& v2) const {
            return (v1.from == v2.from && v1.to == v2.to) || (v1.from == v2.to && v1.to == v2.from);
        }
    };

    Graph graph = Utils::Parser::ParseTSPGraph(file);
    TSPpermutation permutation((unsigned int)graph.GetNumberOfVertices());
    Visualizer* visualizer = new Visualizer(graph, permutation.order);
    std::thread visualizerThread(StartVisualizer, visualizer);
    visualizerThread.detach();

    graph.UpdateNearNeighbors();

    //std::cout << "Ready" << "\n";
    //visualizer->WaitForSpace();

    uint32_t numberOfVertices = (uint32_t)graph.GetNumberOfVertices();


    const uint32_t populationSize = 10;
    const uint32_t maxNumberOfGenerations = 50;

    // create P1 population of random permutation
    std::vector<TSPpermutation> P1(populationSize, numberOfVertices);

    // create P2
    std::vector<TSPpermutation> P2;

    // offsprings
    std::vector<TSPpermutation> offsprings;

    // use LK search on every permutation in P1 popultation
#pragma omp parallel
    {
#pragma omp single
    {
        std::cout << "Running initial Lin-Kernighan on a population of size " << P1.size() << " with " << omp_get_num_threads() << " threads\n";
        std::cout << "Visualizer is showing one of these indiviuals\n";
    }
#pragma omp for
      for (int i = 0; i < P1.size(); i++) 
      {
        Visualizer* vis = (i == 0) ? visualizer : nullptr;
        TSPpermutation& perm = P1[i];
        perm.LinKernighan(graph, vis);
      }

#pragma omp single
      {
        std::cout << "Initial LK-Search done\n";
        std::cout << "From now on showing best solution so far\n";
      }
    }


    TSPpermutation bestSolutionFoundSoFar = P1[0];
    if (visualizer)
      visualizer->UpdatePermutation(P1[0].order);

    auto UpdateBestSolutionSoFar = [&graph, &bestSolutionFoundSoFar, &visualizer](const TSPpermutation& candidate) {
      if (candidate.GetFitness(graph) < bestSolutionFoundSoFar.GetFitness(graph)) {
        bestSolutionFoundSoFar = candidate;
        std::cout << "New Best Solution Fitness: " << bestSolutionFoundSoFar.GetFitness(graph) << "\n";
        if (visualizer)
          visualizer->UpdatePermutation(bestSolutionFoundSoFar.order);
      }
    };


    uint32_t generationNumber = 1;

    std::unordered_map<Edge, uint32_t, Edge_hash, Edge_equals> offspringEdgesCounters;

    while (generationNumber <= maxNumberOfGenerations) {

        std::cout << "Generation: " << generationNumber << "\n";
        
        // Find best permutation in P1
        double bestFitness = 1.7976931348623157E+308;
        uint32_t bestFitnessIndex = 0;
        for (uint32_t i = 0; i < P1.size(); i++) {
            if (P1[i].GetFitness(graph) < bestFitness) {
                bestFitness = P1[i].GetFitness(graph);
                bestFitnessIndex = i;
            }
        }

        // for all other permutations in P1, attempt GPX on best and current permutation
        TSPpermutation& bestPerm = P1[bestFitnessIndex];

        UpdateBestSolutionSoFar(bestPerm);

        for (uint32_t i = 0; i < P1.size(); i++) {
            if (i == bestFitnessIndex) {
                continue;
            }

            TSPpermutation& currentPerm = P1[i];

            auto optionalChildren = TSPpermutation::GPX(bestPerm, currentPerm, graph);
            if (optionalChildren.has_value()) {
                // children
                offsprings.emplace_back(std::move(optionalChildren->first));
                offsprings.emplace_back(std::move(optionalChildren->second));
                const TSPpermutation& greedyChild = offsprings[offsprings.size() - 2];
                const TSPpermutation& otherChild = offsprings.back();
                
                UpdateBestSolutionSoFar(greedyChild);
                UpdateBestSolutionSoFar(otherChild);

                for (uint32_t i = 1; i <= numberOfVertices; i++) {
                    Edge firstChildEdge(greedyChild.order[i - 1], greedyChild.order[i % numberOfVertices]);
                    offspringEdgesCounters[firstChildEdge]++;
                    Edge secondChildEdge(otherChild.order[i - 1], otherChild.order[i % numberOfVertices]);
                    offspringEdgesCounters[secondChildEdge]++;
                }

            }
            else {
                // if GPX not applicable, mutate currentPerm using double-bridge move and put in P2
                currentPerm.mutate_doubleBridge();
                currentPerm.updateFitness(graph);
                P2.push_back(currentPerm);

                UpdateBestSolutionSoFar(currentPerm);
            }
        }

        // place best solution found so far in P2
        P2.push_back(bestSolutionFoundSoFar);

        // calculate offspring scores
        std::vector<std::pair<TSPpermutation, double>> offspringAndScore(offsprings.size());
        for (uint32_t i = 0; i < offsprings.size(); i++) {
            double score = 0.0;
            for (uint32_t j = 1; j <= numberOfVertices; j++) {
                Edge firstChildEdge(offsprings[i].order[j - 1], offsprings[i].order[j % numberOfVertices]);
                score += 1.0/ ((double)offspringEdgesCounters[firstChildEdge]);
            }

            offspringAndScore[i] = std::make_pair(offsprings[i], score);
        }

        // sort offsprings after best diversity selection score
        std::sort(offspringAndScore.begin(), offspringAndScore.end(),
            [](const std::pair<TSPpermutation, double>& p1, const std::pair<TSPpermutation, double>& p2) {
                return p1.second > p2.second;
            }
        );

        // fill rest of P2 from the set of offsprings with best diversity selection score
        uint32_t indexCount = 0;
        while (P2.size() < populationSize) {
            P2.push_back(offspringAndScore[indexCount].first);
            indexCount++;
        }

        ASSERT(P1.size() == P2.size());

#pragma omp parallel
        {
#pragma omp for
          for (int i = 0; i < P2.size(); i++)
          {
            TSPpermutation& perm = P2[i];
            perm.LinKernighan(graph, nullptr);
          }
        }

        // Set P1=P2
        P1 = P2;
        P2.clear();
        generationNumber++;
    }

    for (const TSPpermutation& perm : P1)
      UpdateBestSolutionSoFar(perm);

    visualizer->UpdatePermutation(bestSolutionFoundSoFar.order, true);
    
    std::cout << "FITNESS OF BEST PERMUTATION FOUND: " << bestSolutionFoundSoFar.GetFitness(graph) << std::endl;
    


    visualizer->WaitForClose();
  }

  void RunBlackboxGenerational(const std::string& file)
  {
      Graph graph = Utils::Parser::ParseTSPGraph(file);
      TSPpermutation permutation((unsigned int)graph.GetNumberOfVertices());
      Visualizer* visualizer = new Visualizer(graph, permutation.order);
      std::thread visualizerThread(StartVisualizer, visualizer);
      visualizerThread.detach();

      BlackBoxEA<TSPpermutation>::Parameters parameters;
      parameters.iterations = (uint32_t)3e6;
      parameters.population = 50;
      parameters.mutationProb = 1.0;
      parameters.crossoverProb = 1.0 / parameters.population;
      BlackBoxEA<TSPpermutation>::Run(graph, parameters, true, visualizer);

      visualizer->WaitForClose();
      delete visualizer;
  }

  void RunBlackbox1(const std::string& file)
  {
    Graph graph = Utils::Parser::ParseTSPGraph(file);
    TSPpermutation permutation((unsigned int)graph.GetNumberOfVertices());
    Visualizer* visualizer = new Visualizer(graph, permutation.order);
    std::thread visualizerThread(StartVisualizer, visualizer);
    visualizerThread.detach();

    BlackBoxEA<TSPpermutation>::Parameters parameters;
    parameters.iterations = (uint32_t)3e6;
    parameters.population = 50;
    parameters.mutationProb = 1.0;
    parameters.crossoverProb = 1.0 / parameters.population;
    BlackBoxEA<TSPpermutation>::Run(graph, parameters, false, visualizer);

    visualizer->WaitForClose();
    delete visualizer;
  }
};
