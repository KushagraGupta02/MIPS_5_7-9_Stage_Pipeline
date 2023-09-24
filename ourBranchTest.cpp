#include <iostream>
#include <fstream>
#include "BranchPredictor.hpp"

int main() {
    std::ifstream inputFile("TestTrace_given");
    if (!inputFile) {
        std::cerr << "Error: Could not open input file" << std::endl;
        return 1;
    }
    SaturatingBHRBranchPredictor predictor(3, 1<<16);
    //BHRBranchPredictor predictor(3);
    //SaturatingBranchPredictor predictor(3);
    
    uint32_t pc;
    bool outcome, prediction;
    uint32_t numCorrect = 0, numTotal = 0;
    
    while (inputFile >> std::hex >> pc >> outcome) {
        prediction = predictor.predict(pc);
        if (prediction == outcome) {
            numCorrect++;
        }
        predictor.update(pc, outcome);
        numTotal++;
    }
    double accuracy = static_cast<double>(numCorrect) / numTotal * 100;
    std::cout << "Accuracy: " << accuracy << "%" << std::endl;
    return 0;
}