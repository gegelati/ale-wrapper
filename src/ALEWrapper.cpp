#include "ALEWrapper.h"

void ALEWrapper::doAction(uint64_t actionID) {
    float reward = ale.act(legal_actions[actionID]);
    this->totalReward += reward;
}

void ALEWrapper::reset(size_t seed, Learn::LearningMode mode) {
    this->totalReward = 0.0;
    // resets the game but not the full system
    ale.reset_game();
}

std::vector<std::reference_wrapper<const Data::DataHandler>> ALEWrapper::getDataSources() {
    std::vector<unsigned char> output_rgb_buffer;
    ale.getScreenRGB(output_rgb_buffer);


    for (int i = 0; i < output_rgb_buffer.size(); i++) {
        screen.setDataAt(typeid(double), i, (double) (output_rgb_buffer[i]));
        //printf("%d -> %lf\n", i, (double) (output_rgb_buffer[i]));
    }

    auto result = std::vector<std::reference_wrapper<const Data::DataHandler>>();
    result.push_back(screen);
    return result;
}

double ALEWrapper::getScore() const {
    return this->totalReward;
}

bool ALEWrapper::isTerminal() const {
    return ale.game_over();
}
