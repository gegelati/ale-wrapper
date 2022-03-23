/**
* \brief Executable for translating a .dot into a c file.
*/

#include <getopt.h>
#include <iostream>

#include <gegelati.h>

#include "../ALEWrapper.h"
#include "../instructions.h"

int main(int argc, char** argv ){

    // Get option
    signed char option;
    unsigned int seed = 0;
    char inputFile[100];
    char paramFile[100];
    char rom[50];
    strcpy(rom, "frostbite");
    const std::string paramInfo("Unrecognised option. Valid options are \'-s seed\' \'-r romname\' \'-i inputFile.dot\' \'-c paramFile.json\'.");

    bool inputProvided = false;
    strcpy(paramFile, ROOT_DIR "/params.json");
    while((option = getopt(argc, argv, "s:r:i:c:")) != -1){
        switch (option) {
            case 's': seed= atoi(optarg); break;
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
    TPG::TPGGraph dotGraph(dotEnv, std::make_unique<TPG::TPGInstrumentedFactory>());

    // Load graph
    std::cout << "Loading dot file from " << inputFile << "." << std::endl;
    std::string filename(inputFile);
    File::TPGGraphDotImporter dot(filename.c_str(), dotEnv, dotGraph);
    dot.importGraph();
    const TPG::TPGVertex* root = dotGraph.getRootVertices().front();

    // Play the game once to identify useful edges & vertices
    TPG::TPGExecutionEngineInstrumented tee(dotEnv);
    int nbActions = 0;
    std::cout << "Play with TPG code" << std::endl;
    while(nbActions < 18000 && !le.isTerminal()){
        le.doAction(((TPG::TPGAction*)(tee.executeFromRoot(* root).back()))->getActionID());
        nbActions++;
    }
    std::cout << "Total score: " << le.getScore() << " in "  << nbActions << " actions." << std::endl;

    // Clean the unused vertices & teams
    ((const TPG::TPGInstrumentedFactory&)dotGraph.getFactory()).clearUnusedTPGGraphElements(dotGraph);

    // Play the game again to check the result remains the same.
    nbActions = 0;
    le.reset(0);
    std::cout << "Play with cleaned TPG code" << std::endl;
    while(nbActions < 18000 && !le.isTerminal()){
        le.doAction(((TPG::TPGAction*)(tee.executeFromRoot(* root).back()))->getActionID());
        nbActions++;
    }
    std::cout << "Total score: " << le.getScore() << " in "  << nbActions << " actions." << std::endl;

    // Get stats on graph to get the required stack size
    std::cout << "Analyze graph." << std::endl;
    TPG::PolicyStats ps;
    ps.setEnvironment(dotEnv);
    ps.analyzePolicy(dotGraph.getRootVertices().front());

    // Print in file
    char bestPolicyStatsPath[150];
    std::ofstream bestStats;
    sprintf(bestPolicyStatsPath, "out_best_stats_cleaned.%s.%d.t%02d.md", rom, seed, 48);
    bestStats.open(bestPolicyStatsPath);
    bestStats << ps;
    bestStats.close();

    // Print graph
    std::cout << "Printing C code." << std::endl;
	CodeGen::TPGGenerationEngineFactory factory(CodeGen::TPGGenerationEngineFactory::switchMode);
    std::unique_ptr<CodeGen::TPGGenerationEngine> tpggen = factory.create("ale", dotGraph, ROOT_DIR "/codegen/");
    tpggen->generateTPGGraph();

    // Export cleaned dot file
    std::cout << "Printing cleaned dot file." << std::endl;
    char bestDot[150];
    sprintf(bestDot, "out_best_cleaned.%s.%d.t%02d.dot", rom, seed, 48);
    File::TPGGraphDotExporter dotExporter(bestDot, dotGraph);
    dotExporter.print();

    return 0;
}
