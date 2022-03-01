//
// Created by kdesnos on 07/12/2021.
//
#include <getopt.h>
#include <iostream>

#include <gegelati.h>

#include "../ALEWrapper.h"
#include "../instructions.h"

#define CSV_OUTPUT 1

int main(int argc, char** argv ){

    // Get option
    signed char option;
    int nbGames = 1;
    char inputFile[100];
    char paramFile[100];
    char rom[150];
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
    char romPath[150];
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
#ifndef CSV_OUTPUT
    std::cout << "Loading dot file from " << inputFile << "." << std::endl;
#endif
    std::string filename(inputFile);
    File::TPGGraphDotImporter dot(filename.c_str(), dotEnv, dotGraph);
    dot.importGraph();
    const TPG::TPGVertex* root = dotGraph.getRootVertices().front();

    // Play the game
    TPG::TPGExecutionEngine tee(dotEnv);
    int nbActions = 0;
    // uint64_t actions[18000];
    // measure time
#ifndef CSV_OUTPUT
    std::cout << "Play "<<nbGames<<" games with generated code" << std::endl;
#endif
    double tpgTime = 0.0;
    double envTime = 0.0;
    double globTime = 0.0;

    for(unsigned int repet = 0; repet<nbGames ; repet++) {
        nbActions = 0;
        le.reset(0);
        auto startGlobal = std::chrono::high_resolution_clock::now();
        while (nbActions < 18000 && !le.isTerminal()) {

            auto start = std::chrono::high_resolution_clock::now();
            uint64_t action  = ((TPG::TPGAction*)(tee.executeFromRoot(* root).back()))->getActionID();
            auto stop = std::chrono::high_resolution_clock::now();
            tpgTime +=((std::chrono::duration<double>)(stop - start)).count();

            start = std::chrono::high_resolution_clock::now();
            le.doAction(action);
            stop = std::chrono::high_resolution_clock::now();
            envTime +=((std::chrono::duration<double>)(stop - start)).count();
            nbActions++;
        }
        auto stopGlobal = std::chrono::high_resolution_clock::now();
        globTime += ((std::chrono::duration<double>)(stopGlobal - startGlobal)).count();
    }

#ifndef CSV_OUTPUT
    std::cout << "Total score: " << le.getScore() << " in "  << nbActions << " actions." << std::endl;

    std::cout << std::setprecision(6) << " Total time: " << globTime/(double)nbGames << std::endl;
    std::cout << std::setprecision(6) << "  Env. time: " << envTime/(double)nbGames << std::endl;
    std::cout << std::setprecision(6) << "Infer. time: " << tpgTime/(double)nbGames << std::endl;
#else
    std::cout << le.getScore() << " ; "  << nbActions ;

    std::cout  << " ; " << globTime/(double)nbGames;
    std::cout  << " ; " << envTime/(double)nbGames ;
    std::cout  << " ; " << tpgTime/(double)nbGames << std::endl;
#endif
}
