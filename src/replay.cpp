#include <iostream>
#include <unordered_set>
#include <numeric>
#include <string>
#include <cfloat>
#include <inttypes.h>

#include <gegelati.h>

#include "ALEWrapper.h"
#include "replay.h"


int agentTest() {
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
