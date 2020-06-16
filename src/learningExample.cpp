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
#include "replay.h"

#ifndef NB_GENERATIONS
#define NB_GENERATIONS 2000
#endif

int main() {
    // Create the instruction set for programs
    Instructions::Set set;
    auto minus = [](double a, double b) -> double { return a - b; };
    auto add = [](double a, double b) -> double { return a + b; };
    auto times = [](double a, double b) -> double { return a * b; };
    auto divide = [](double a, double b) -> double { return a / b; };
    auto cond = [](double a, double b) -> double { return a < b ? -a : a; };
    auto cos = [](double a) -> double { return std::cos(a); };
    auto ln = [](double a) -> double { return std::log(a); };
    auto exp = [](double a) -> double { return std::exp(a); };

    set.add(*(new Instructions::LambdaInstruction<double, double>(minus)));
    set.add(*(new Instructions::LambdaInstruction<double, double>(add)));
    set.add(*(new Instructions::LambdaInstruction<double, double>(times)));
    set.add(*(new Instructions::LambdaInstruction<double, double>(divide)));
    set.add(*(new Instructions::LambdaInstruction<double, double>(cond)));
    set.add(*(new Instructions::LambdaInstruction<double>(cos)));
    set.add(*(new Instructions::LambdaInstruction<double>(ln)));
    set.add(*(new Instructions::LambdaInstruction<double>(exp)));

    // Set the parameters for the learning process.
    // (Controls mutations probability, program lengths, and graph size
    // among other things)
    Learn::LearningParameters params;
    params.mutation.tpg.maxInitOutgoingEdges = 5;
    params.mutation.tpg.nbRoots = 360;
    params.mutation.tpg.pEdgeDeletion = 0.7;
    params.mutation.tpg.pEdgeAddition = 0.7;
    params.mutation.tpg.pProgramMutation = 0.2;
    params.mutation.tpg.pEdgeDestinationChange = 0.1;
    params.mutation.tpg.pEdgeDestinationIsAction = 0.5;
    params.mutation.tpg.maxOutgoingEdges = 999;
    params.mutation.prog.pAdd = 0.5;
    params.mutation.prog.pDelete = 0.5;
    params.mutation.prog.pMutate = 1.0;
    params.mutation.prog.pSwap = 1.0;
    params.mutation.prog.maxProgramSize = 96;
    params.archiveSize = 50;
    params.maxNbActionsPerEval = 18000;
    params.nbIterationsPerPolicyEvaluation = 5;
    params.maxNbEvaluationPerPolicy = 10;
    params.ratioDeletedRoots = 0.8;

    ALEWrapper le("roms/frostbite", 18);


    // Instantiate and init the learning agent
    Learn::LearningAgent la(le, set, params,8);
    la.init();


    // Create an exporter for all graphs
    File::TPGGraphDotExporter dotExporter("out_000.dot", la.getTPGGraph());

    auto start = std::chrono::system_clock::now();
    // Train for NB_GENERATIONS generations
    printf("Gen\tNbVert\tMin\tAvg\tMax\tDuration(eval)\tDuration(training)\tTotal_time\n");
    for (int i = 0; i < NB_GENERATIONS; i++) {
        auto evalBegin = std::chrono::system_clock::now();


        char buff[16];
        sprintf(buff, "out_%03d.dot", i);
        dotExporter.setNewFilePath(buff);
        dotExporter.print();
        std::multimap<std::shared_ptr<Learn::EvaluationResult>, const TPG::TPGVertex *> result;
        result = la.evaluateAllRoots(i, Learn::LearningMode::VALIDATION);
        auto iter = result.begin();
        double min = iter->first->getResult();
        std::advance(iter, result.size() - 1);
        double max = iter->first->getResult();
        double avg = std::accumulate(result.begin(), result.end(), 0.0,
                                     [](double acc,
                                        std::pair<std::shared_ptr<Learn::EvaluationResult>, const TPG::TPGVertex *> pair) -> double {
                                         return acc + pair.first->getResult();
                                     });
        avg /= result.size();

        auto evalEnd = std::chrono::system_clock::now();
        double tEval = ((std::chrono::duration<double>) (evalEnd - evalBegin)).count();

        la.trainOneGeneration(i);

        double tTraining = ((std::chrono::duration<double>) (std::chrono::system_clock::now() - evalEnd)).count();
        double tTotal = ((std::chrono::duration<double>) (std::chrono::system_clock::now() - start)).count();
        printf("%3d\t%4" PRIu64 "\t%1.2lf\t%1.2lf\t%1.2lf\t%1.2lf\t%1.2lf\t%1.2lf\n", i,
               la.getTPGGraph().getNbVertices(), min, avg,
               max, tEval, tTraining, tTotal);
    }

    // Keep best policy
    la.keepBestPolicy();
    dotExporter.setNewFilePath("out_best.dot");
    dotExporter.print();

    // cleanup
    for (unsigned int i = 0; i < set.getNbInstructions(); i++) {
        delete (&set.getInstruction(i));
    }

    // if we want to test the best agent
    if (true) {
        agentTest();
        return 0;
    }

    return 0;
}