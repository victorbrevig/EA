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
    //Visualizer* visualizer = new Visualizer(graph, permutation.order);
    //std::thread visualizerThread(StartVisualizer, visualizer);
    //visualizerThread.detach();

    graph.UpdateNearNeighbors();

    //std::cout << "Ready" << "\n";
    //visualizer->WaitForSpace();

    uint32_t numberOfVertices = graph.GetNumberOfVertices();


    const uint32_t populationSize = 10;
    const uint32_t maxNumberOfGenerations = 5;

    // create P1 population of random permutation
    std::vector<TSPpermutation> P1(populationSize);
    for (uint32_t i = 0; i < populationSize; i++) {
        P1[i] = TSPpermutation(numberOfVertices);
    }


    // create P2
    std::vector<TSPpermutation> P2;

    // offsprings
    std::vector<TSPpermutation> offsprings;

    
    std::unordered_map<Edge, uint32_t, Edge_hash, Edge_equals> offspringEdgesCounters;

    // use LK search on every permutation in P1 popultation

    for (TSPpermutation& perm : P1) {
        perm.LinKernighan(graph, nullptr);
    }

    TSPpermutation bestSolutionFoundSoFar = P1[0];
    
    uint32_t generationNumber = 1;

    while (generationNumber < maxNumberOfGenerations) {
        // Find best permutation in P1
        double bestFitness = 1.7976931348623157E+308;
        uint32_t bestFitnessIndex = 0;
        for (uint32_t i = 0; i < P1.size(); i++) {
            if (P1[i].GetFitness() < bestFitness) {
                bestFitness = P1[i].GetFitness();
                bestFitnessIndex = i;
            }
        }

        // for all other permutations in P1, attempt GPX on best and current permutation
        TSPpermutation& bestPerm = P1[bestFitnessIndex];

        if (bestPerm.GetFitness() < bestSolutionFoundSoFar.GetFitness()) {
            bestSolutionFoundSoFar = bestPerm;
        }

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
                

                if (greedyChild.GetFitness() < bestSolutionFoundSoFar.GetFitness()) {
                    bestSolutionFoundSoFar = greedyChild;
                }
                if (otherChild.GetFitness() < bestSolutionFoundSoFar.GetFitness()) {
                    bestSolutionFoundSoFar = otherChild;
                }

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

                if (currentPerm.GetFitness() < bestSolutionFoundSoFar.GetFitness()) {
                    bestSolutionFoundSoFar = currentPerm;
                }

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

        // apply LK to every permutation in P2
        for (TSPpermutation& perm : P2) {
            perm.LinKernighan(graph, nullptr);
        }

        // Set P1=P2
        P1 = P2;
        P2.clear();
        generationNumber++;
    }

    
    std::cout << "FITNESS OF BEST PERMUTATION FOUND: " << bestSolutionFoundSoFar.GetFitness() << std::endl;
    


    
    

    

    

    /*
    while (true)
    {
      TSPpermutation parent1((unsigned int)graph.GetNumberOfVertices());
      parent1.LinKernighan(graph, visualizer);
      std::cout << "Parent 1 fitness: " << parent1.GetFitness() << "\n";
      visualizer->UpdatePermutation(parent1.order);
      visualizer->WaitForSpace();

      TSPpermutation parent2((unsigned int)graph.GetNumberOfVertices());
      parent2.LinKernighan(graph, visualizer);
      std::cout << "Parent 2 fitness: " << parent2.GetFitness() << "\n";
      visualizer->UpdatePermutation(parent2.order);
      visualizer->WaitForSpace();

      visualizer->UpdatePermutation(std::vector<std::vector<uint32_t>>({ parent1.order, parent2.order }));
      visualizer->WaitForSpace();


      auto optionalChild = TSPpermutation::GPX(parent1, parent2, graph);
      if (optionalChild.has_value())
      {
        std::pair<TSPpermutation, TSPpermutation> children = *optionalChild;
        TSPpermutation greedyChild = children.first;
        TSPpermutation otherChild = children.second;
        greedyChild.updateFitness(graph);
        std::cout << "Child fitness: " << greedyChild.GetFitness() << "\n";
        visualizer->UpdatePermutation(greedyChild.order);
        visualizer->WaitForSpace();

        visualizer->UpdatePermutation(otherChild.order);
        visualizer->WaitForSpace();

        std::cout << "Parent 1 fitness: " << parent1.GetFitness() << "\n";
        visualizer->UpdatePermutation(parent1.order);
        visualizer->WaitForSpace();

        std::cout << "Parent 2 fitness: " << parent2.GetFitness() << "\n";
        visualizer->UpdatePermutation(parent2.order);
        visualizer->WaitForSpace();

        visualizer->UpdatePermutation(std::vector<std::vector<uint32_t>>({ parent1.order, parent2.order }));
        visualizer->WaitForSpace();

        std::cout << "Child fitness: " << greedyChild.GetFitness() << "\n";
        visualizer->UpdatePermutation(greedyChild.order);
        visualizer->WaitForSpace();
      }
    }

    visualizer->WaitForClose();

    delete visualizer;

    */
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
    BlackBoxEA<TSPpermutation>::Run(graph, parameters, visualizer);


    visualizer->WaitForClose();
    delete visualizer;
  }
};
