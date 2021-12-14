#include <iostream>
#include <unordered_set>
#include <numeric>
#include <string>
#include <cfloat>
#include <inttypes.h>
#include <getopt.h>
#include <thread>

#include <chrono>
#include <ctime>

#include <gegelati.h>

#include "ALEWrapper.h"
#include "instructions.h"
#include "replay.h"

int main(int argc, char ** argv) {

    char option;
    uint64_t seed = 0;
    char rom[50];
    char paramFile[150];
    size_t nbThreads = std::thread::hardware_concurrency();
    strcpy(rom, "frostbite");
    strcpy(paramFile, ROOT_DIR "/params.json");
    while((option = getopt(argc, argv, "s:r:p:c:")) != -1){
        switch (option) {
            case 's': seed= atoi(optarg); break;
            case 'r': strcpy(rom, optarg); break;
            case 'p': nbThreads = atoi(optarg); break;
            case 'c': strcpy(paramFile, optarg); break;
            default: std::cout << "Unrecognised option. Valid options are \'-s seed\' \'-r romName\' \'-p nbThreads\' \'-c paramFile.json\'." << std::endl; exit(1);
        }
    }
    std::cout << "Selected seed : " << seed << std::endl;
    std::cout << "Selected ROM: "  << rom << std::endl;
    std::cout << "Selected NbThreads: " << nbThreads << std::endl;
    std::cout << "Selected params: " << paramFile << std::endl;

    // Create the instruction set for programs
    Instructions::Set set;
    fillInstructionSet(set);

    // Set the parameters for the learning process.
    // (Controls mutations probability, program lengths, and graph size
    // among other things)
    Learn::LearningParameters params;
    File::ParametersParser::loadParametersFromJson(paramFile,params);
    params.nbThreads = nbThreads;

    char romPath[50];
    sprintf(romPath, ROOT_DIR "/roms/%s", rom);
    ALEWrapper le(romPath, 18);


    // Instantiate and init the learning agent
    Learn::ParallelLearningAgent la(le, set, params);
    la.init(seed);

    // Basic Logger
    char logPath[50];
    sprintf(logPath, "out.%s.%d.t%02d.std", rom, seed, nbThreads);
    std::ofstream logStream;
    logStream.open(logPath);
    Log::LABasicLogger log(la, logStream);

    // File for logging best policy stat.
    char bestPolicyStatsPath[50];
    sprintf(bestPolicyStatsPath, "bestPolicyStats.%s.%d.t%02d.md", rom, seed, nbThreads);
    std::ofstream stats;
    stats.open(bestPolicyStatsPath);
    Log::LAPolicyStatsLogger logStats(la, stats);

    // Create an exporter for all graphs
    char dotPath[50];
    sprintf(dotPath, "out_0000.%s.%d.t%02d.dot", rom, seed, nbThreads);
    File::TPGGraphDotExporter dotExporter(dotPath, la.getTPGGraph());
    // Train for NB_GENERATIONS generations
    for (int i = 0; i < params.nbGenerations; i++) {
#define PRINT_ALL_DOT 0
#if PRINT_ALL_DOT
        sprintf(dotPath, "out_%04d.%s.%d.t%02d.dot", i, rom, seed, nbThreads);
        dotExporter.setNewFilePath(dotPath);
        dotExporter.print();
#endif
        la.trainOneGeneration(i);
    }

    // Keep best policy
    la.keepBestPolicy();

    // Clear introns instructions
    la.getTPGGraph().clearProgramIntrons();
    char bestDot[50];

    // Export the graph
    sprintf(bestDot, "out_best.%s.%d.t%02d.dot", rom, seed, nbThreads);
    dotExporter.setNewFilePath(bestDot);
    dotExporter.print();

    TPG::PolicyStats ps;
    ps.setEnvironment(la.getTPGGraph().getEnvironment());
    ps.analyzePolicy(la.getBestRoot().first);
    std::ofstream bestStats;
    sprintf(bestPolicyStatsPath, "out_best_stats.%s.%d.t%02d.md", rom, seed, nbThreads);
    bestStats.open(bestPolicyStatsPath);
    bestStats << ps;
    bestStats.close();
    stats.close();

    // cleanup
    for (unsigned int i = 0; i < set.getNbInstructions(); i++) {
        delete (&set.getInstruction(i));
    }

    // if we want to test the best agent
    if (false) {
        agentTest();
        return 0;
    }

    return 0;
}
