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
    uint64_t actions[18000];
    // measure time
    std::cout << "Play "<<nbGames<<" games with generated code" << std::endl;
    auto start = std::chrono::high_resolution_clock::now();

    for(unsigned int repet = 0; repet<nbGames ; repet++) {
        nbActions = 0;
        le.reset(0);
        while (nbActions < 18000 && !le.isTerminal()) {
            actions[nbActions] = inferenceTPG();
            le.doAction(actions[nbActions]);
            nbActions++;
        }
    }

    auto stop = std::chrono::high_resolution_clock::now();

    std::cout << "Total score: " << le.getScore() << " in "  << nbActions << " actions." << std::endl;

    // do a replay to subtract non-inference time
    std::cout << "Replay environment without TPG" << std::endl;
    auto startReplay = std::chrono::high_resolution_clock::now();

    for(unsigned int repet = 0; repet<nbGames ; repet++) {
        size_t iter = 0;
        le.reset(0);
        while (iter < nbActions) {
            // Do the action
            le.doAction(actions[iter]);

            iter++;
        }
    }
    auto stopReplay = std::chrono::high_resolution_clock::now();

    std::cout << "Total replay score: " << le.getScore() << " in "  << nbActions << " actions." << std::endl;

    auto totalTime = ((std::chrono::duration<double>)(stop - start)).count();
    auto replayTime = ((std::chrono::duration<double>)(stopReplay - startReplay)).count();
    std::cout << std::setprecision(6) << " Total time: " << totalTime/(double)nbGames << std::endl;
    std::cout << std::setprecision(6) << "  Env. time: " << replayTime/(double)nbGames << std::endl;
    std::cout << std::setprecision(6) << "Infer. time: " << (totalTime-replayTime)/(double)nbGames << std::endl;
}