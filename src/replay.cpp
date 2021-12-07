#include <iostream>
#include <unordered_set>
#include <numeric>
#include <string>
#include <cfloat>
#include <inttypes.h>

#include <gegelati.h>

#include "ALEWrapper.h"
#include "replay.h"
#include "instructions.h"

int agentTest() {
    // Create the instruction set for programs
    Instructions::Set set;
    fillInstructionSet(set);

    // Instantiate the LearningEnvironment
    auto le = ALEWrapper("roms/frostbite",18,true);


    Environment env(set, le.getDataSources(), 8);
    auto tpg = TPG::TPGGraph(env);

    TPG::TPGExecutionEngine tee(env);

    // Create an importer for the best graph
    File::TPGGraphDotImporter dotImporter("out_686.dot", env, tpg);

    dotImporter.importGraph();

    auto root = tpg.getRootVertices().front();

    int x = 0;
    while(x<18000){
        le.getDataSources();
        uint64_t action = ((TPG::TPGAction*)(tee.executeFromRoot(* root).back()))->getActionID();
        le.play(action);
        //ale.act(ale.getLegalActionSet()[action]);
        x++;
    }


    // cleanup
    for (unsigned int i = 0; i < set.getNbInstructions(); i++) {
        delete (&set.getInstruction(i));
    }

    return 0;
}
