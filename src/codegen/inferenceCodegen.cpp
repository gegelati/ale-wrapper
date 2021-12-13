//
// Created by kdesnos on 07/12/2021.
//

extern "C" {
#include "externHeader.h"
#include "ale.h"
/// instantiate global variable used to communicate between the TPG and the environment
double* in1;
}

#include "../ALEWrapper.h"

int main(){

    // Instantiate the LearningEnvironment
    auto le = ALEWrapper(ROOT_DIR "/roms/frostbite",18,false);
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
    std::cout << "Play with generated code" << std::endl;
    auto start = std::chrono::system_clock::now();
    while(nbActions < 18000 && !le.isTerminal()){
        actions[nbActions] = inferenceTPG();
        le.doAction(actions[nbActions]);
        nbActions++;
    }

    auto stop = std::chrono::system_clock::now();

    std::cout << "Total score: " << le.getScore() << std::endl;

    // do a replay to subtract non-inference time
    size_t iter = 0;
    le.reset(0);

    std::cout << "Replay environment without TPG" << std::endl;
    auto startReplay = std::chrono::system_clock::now();
    while (iter < nbActions) {
        // Do the action
        le.doAction(actions[iter]);

        iter++;
    }
    auto stopReplay = std::chrono::system_clock::now();

    std::cout << "Total replay score: " << le.getScore() << std::endl;

    auto totalTime = ((std::chrono::duration<double>)(stop - start)).count();
    auto replayTime = ((std::chrono::duration<double>)(stopReplay - startReplay)).count();
    std::cout << std::setprecision(6) << " Total time: " << totalTime << std::endl;
    std::cout << std::setprecision(6) << "  Env. time: " << replayTime << std::endl;
    std::cout << std::setprecision(6) << "Infer. time: " << totalTime-replayTime << std::endl;
}