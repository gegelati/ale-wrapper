//
// Created by kdesnos on 07/12/2021.
//

#include <getopt.h>

extern "C" {
#include "externHeader.h"
#include "ale.h"
/// instantiate global variable used to communicate between the TPG and the environment
double* in1;
}

#include "../ALEWrapper.h"

#define CSV_OUTPUT 1

int main(int argc, char** argv){

    char option;
    int nbGames = 1;
    char rom[50];
    strcpy(rom, "frostbite");
    while((option = getopt(argc, argv, "r:")) != -1){
        switch (option) {
            case 'r': strcpy(rom, optarg); break;
            default: std::cout << "Unrecognised option. Valid options are \'-r romName\'." << std::endl; exit(1);
        }
    }

    // Instantiate the LearningEnvironment
    char romPath[150];
    sprintf(romPath, ROOT_DIR "/roms/%s", rom);
    auto le = ALEWrapper(romPath,18,false);
    le.reset(0);

    // fetch data in the environment
    auto dataSources = le.getDataSources();
    auto& st = dataSources.at(0).get();
    auto dataSharedPointer = st.getDataAt(typeid(double), 0).getSharedPointer<double>();
    in1 = dataSharedPointer.get();

    // Play the game
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
            uint64_t action = inferenceTPG();
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

    std::cout << " ; " << globTime/(double)nbGames;
    std::cout << " ; " << envTime/(double)nbGames ;
    std::cout << " ; " << tpgTime/(double)nbGames << std::endl;
#endif
}