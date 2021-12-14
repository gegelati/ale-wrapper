//
// Created by kdesnos on 07/12/2021.
//
#include <getopt.h>
#include <iostream>

#include <gegelati.h>

#include "../ALEWrapper.h"
#include "../instructions.h"

int main(int argc, char** argv ){

    // Get option
    char option;
    char inputFile[100];
    char paramFile[100];
    char rom[50];
    auto paramInfo = "Unrecognised option. Valid options are \'-i inputFile.dot\' \'-c paramFile.json\'.";

    bool inputProvided = false;
    strcpy(paramFile, ROOT_DIR "/params.json");
    strcpy(rom, "frostbite");
    while((option = getopt(argc, argv, "r:i:c:")) != -1){
        switch (option) {
            case 'r': strcpy(rom, optarg); break;
            case 'i': strcpy(inputFile, optarg); inputProvided = true; break;
            case 'c': strcpy(paramFile, optarg); break;
            default: std::cout << paramInfo << std::endl; exit(1);
        }
    }

    if(!inputProvided) {
        std::cout << paramInfo << std::endl;
        exit(1);
    }

    // Instantiate the LearningEnvironment
    char romPath[50];
    sprintf(romPath, ROOT_DIR "/roms/%s", rom);
    ALEWrapper le(romPath,18,false);
    le.reset(0);

    // Create the instruction set for programs
    Instructions::Set set;
    fillInstructionSet(set);

    // Load parameters
    Learn::LearningParameters params;
    File::ParametersParser::loadParametersFromJson(
            paramFile, params);
    Environment dotEnv(set, le.getDataSources(), params.nbRegisters, params.nbProgramConstant);
    TPG::TPGGraph dotGraph(dotEnv);


    // Load graph
    std::cout << "Loading dot file from " << inputFile << "." << std::endl;
    std::string filename(inputFile);
    File::TPGGraphDotImporter dot(filename.c_str(), dotEnv, dotGraph);
    dot.importGraph();
    const TPG::TPGVertex* root = dotGraph.getRootVertices().front();

    // Play the game
    TPG::TPGExecutionEngine tee(dotEnv);
    int nbActions = 0;
    uint64_t actions[18000];
    // measure time
    std::cout << "Play with generated code" << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    while(nbActions < 18000 && !le.isTerminal()){
        actions[nbActions]  = ((TPG::TPGAction*)(tee.executeFromRoot(* root).back()))->getActionID();
        le.doAction(actions[nbActions]);
        nbActions++;
    }

    auto stop = std::chrono::high_resolution_clock::now();

    std::cout << "Total score: " << le.getScore() << " in "  << nbActions << " actions." << std::endl;

    // do a replay to subtract non-inference time
    size_t iter = 0;
    le.reset(0);

    std::cout << "Replay environment without TPG" << std::endl;
    auto startReplay = std::chrono::high_resolution_clock::now();
    while (iter < nbActions) {
        // Do the action
        le.doAction(actions[iter]);

        iter++;
    }
    auto stopReplay = std::chrono::high_resolution_clock::now();

    std::cout << "Total replay score: " << le.getScore() << " in "  << nbActions << " actions." << std::endl;

    auto totalTime = ((std::chrono::duration<double>)(stop - start)).count();
    auto replayTime = ((std::chrono::duration<double>)(stopReplay - startReplay)).count();
    std::cout << std::setprecision(6) << " Total time: " << totalTime << std::endl;
    std::cout << std::setprecision(6) << "  Env. time: " << replayTime << std::endl;
    std::cout << std::setprecision(6) << "Infer. time: " << totalTime-replayTime << std::endl;
}