#include <iostream>
#include <unordered_set>
#include <numeric>
#include <string>
#include <cfloat>
#include <inttypes.h>

#include <chrono>
#include <ctime>

#include <gegelati.h>

#include "ALEWrapper.h"

#ifndef NB_GENERATIONS
#define NB_GENERATIONS 300
#endif

// best speed : 7614.68s for 18 gene (42 vertices, 0-106.67-160 with best avg 148057 at G 5 time 1721.01 47 vertices)
int main() {
    // Create the instruction set for programs
    Instructions::Set set;
    auto minus = [](double a, double b)->double {return a - b; };
    auto cast = [](double a, double b)->double {return a; };
    auto add = [](double a, double b)->double {return a + b; };
    auto max = [](double a, double b)->double {return std::max(a, b); };
    auto nulltest = [](double a, double b)->double {return (a == 0.0) ? 10.0 : 0.0; };
    auto modulo = [](double a, double b)->double {
        if (b != 0.0) { return fmod(a, b); }
        else { return  DBL_MIN; }	};

    set.add(*(new Instructions::LambdaInstruction<double>(modulo)));
    set.add(*(new Instructions::LambdaInstruction<double>(minus)));
    set.add(*(new Instructions::LambdaInstruction<double>(add)));
    set.add(*(new Instructions::LambdaInstruction<double>(cast)));
    set.add(*(new Instructions::LambdaInstruction<double>(max)));
    set.add(*(new Instructions::LambdaInstruction<double>(nulltest)));

    // Set the parameters for the learning process.
    // (Controls mutations probability, program lengths, and graph size
    // among other things)
    Learn::LearningParameters params;
    params.mutation.tpg.maxInitOutgoingEdges = 3;
    params.mutation.tpg.nbRoots = 50;
    params.mutation.tpg.pEdgeDeletion = 0.7;
    params.mutation.tpg.pEdgeAddition = 0.7;
    params.mutation.tpg.pProgramMutation = 0.2;
    params.mutation.tpg.pEdgeDestinationChange = 0.1;
    params.mutation.tpg.pEdgeDestinationIsAction = 0.5;
    params.mutation.tpg.maxOutgoingEdges = 5;
    params.mutation.prog.pAdd = 0.5;
    params.mutation.prog.pDelete = 0.5;
    params.mutation.prog.pMutate = 1.0;
    params.mutation.prog.pSwap = 1.0;
    params.mutation.prog.maxProgramSize = 20;
    params.archiveSize = 50;
    params.maxNbActionsPerEval = 18000;
    params.nbIterationsPerPolicyEvaluation = 5;
    params.ratioDeletedRoots = 0.5;

    ALEWrapper le("roms/frostbite", 18);


    // Instantiate and init the learning agent
    Learn::LearningAgent la(le, set, params);

// parallel -> 0 36 0 44.44 160 202.323;         3 47 0 142.22 160 2447.6
// sequential -> 0 36 0 44.44 160 402.307
    la.init();


    // Create an exporter for all graphs
    File::TPGGraphDotExporter dotExporter("out_000.dot", la.getTPGGraph());

    auto start = std::chrono::system_clock::now();
    // Train for NB_GENERATIONS generations
    printf("Gen\tNbVert\tMin\tAvg\tMax\n");
    for (int i = 0; i < NB_GENERATIONS; i++) {
        char buff[12];
        sprintf(buff, "out_%03d.dot", i);
        dotExporter.setNewFilePath(buff);
        dotExporter.print();
        std::multimap<std::shared_ptr<Learn::EvaluationResult>, const TPG::TPGVertex*> result;
        result = la.evaluateAllRoots(i, Learn::LearningMode::VALIDATION);
        auto iter = result.begin();
        double min = iter->first->getResult();
        std::advance(iter, result.size() - 1);
        double max = iter->first->getResult();
        double avg = std::accumulate(result.begin(), result.end(), 0.0,
                                     [](double acc, std::pair<std::shared_ptr<Learn::EvaluationResult>, const TPG::TPGVertex*> pair)->double {return acc + pair.first->getResult(); });
        avg /= result.size();
        printf("%3d\t%4" PRIu64 "\t%1.2lf\t%1.2lf\t%1.2lf\n", i, la.getTPGGraph().getNbVertices(), min, avg, max);
        std::cout<<"elapsed time : "<<((std::chrono::duration<double>)(std::chrono::system_clock::now()-start)).count()<<std::endl;

        la.trainOneGeneration(i);

    }

    // Keep best policy
    la.keepBestPolicy();
    dotExporter.setNewFilePath("out_best.dot");
    dotExporter.print();

    // cleanup
    for (unsigned int i = 0; i < set.getNbInstructions(); i++) {
        delete (&set.getInstruction(i));
    }

    return 0;
}