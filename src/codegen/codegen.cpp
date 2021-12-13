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
    char option;
    char inputFile[100];
    char paramFile[100];
    const std::string paramInfo("Unrecognised option. Valid options are \'-i inputFile.dot\' \'-c paramFile.json\'.");

    bool inputProvided = false;
    strcpy(paramFile, ROOT_DIR "/params.json");
    while((option = getopt(argc, argv, "i:c:")) != -1){
        switch (option) {
            case 'i': strcpy(inputFile, optarg); inputProvided = true; break;
            case 'c': strcpy(paramFile, optarg); break;
            default: std::cout << paramInfo << std::endl; exit(1);
        }
    }

    if(!inputProvided) {
        std::cout << paramInfo << std::endl;
        exit(1);
    }



    // Create the instruction set for programs
    Instructions::Set set;
    fillInstructionSet(set);

    // Fake data source for environment re-creation
    Data::PrimitiveTypeArray<double> fakeScreen(ALEWrapper::SCREEN_SIZE);
    std::vector<std::reference_wrapper<const Data::DataHandler>> data = { fakeScreen };

    // Load parameters
    Learn::LearningParameters params;
    File::ParametersParser::loadParametersFromJson(
            paramFile, params);
    Environment dotEnv(set, data, params.nbRegisters, params.nbProgramConstant);
    TPG::TPGGraph dotGraph(dotEnv);

    // Load graph
    std::cout << "Loading dot file from " << inputFile << "." << std::endl;
    std::string filename(inputFile);
    File::TPGGraphDotImporter dot(filename.c_str(), dotEnv, dotGraph);
    dot.importGraph();

    // Print graph
    std::cout << "Printing C code." << std::endl;
    CodeGen::TPGGenerationEngine tpggen("ale", dotGraph, ROOT_DIR "/codegen/");
    tpggen.generateTPGGraph();

    return 0;
}