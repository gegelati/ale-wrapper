#include "ALEWrapper.h"

void ALEWrapper::play(uint64_t actionID) {
    ale.act(legal_actions[actionID]);
}

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
    // grayscale enables us to have a 8bit int defining the color
    ale.getScreenGrayscale(output_rgb_buffer);

    // original grid
    int oWidth = 160;
    int oHeight = 210;

    // new grid
    int nWidth = 32; // oWidth / 5
    int nHeight = 42; // oHeight / 5


    // applying the 2 transformations described in :
    // Kelly Stepje, SCALING GENETIC PROGRAMMING TO CHALLENGING REINFORCEMENT TASKS THROUGH EMERGENT MODULARITY, pages 98-99, 2018


    // coordinates evolving in a 42*32 grid
    for (int r = 0; r < nHeight; r++) {
        for (int c = 0; c < nWidth; c++) {
            int res = 0;

            // coordinates in the original 210*160 grid
            for (int y = r * 5; y < (r + 1) * 5; y++) {
                // we take only half of the screen into consideration in a cross configuration
                // it means we will increment x by 2 and it doesn't start at the same coordinate at each time
                // drawing a simple 10x10 grid as example can demonstrate it
                int xDepart = (y + c) % 2 == 0 ? c * 5 + 1 : c * 5;
                for (int x = xDepart; x < (c + 1) * 5; x +=2) {
                    // now, define each bit of res according to the presence of a color
                    switch (output_rgb_buffer[y*oWidth  + x]) {
                        case 255:
                            res |= 128;
                            break;
                        case 233:
                            res |= 64;
                            break;
                        case 217:
                            res |= 32;
                            break;
                        case 188:
                            res |= 16;
                            break;
                        case 152:
                            res |= 8;
                            break;
                        case 121:
                            res |= 4;
                            break;
                        case 58:
                            res |= 2;
                            break;
                        case 0:
                            res |= 1;
                            break;
                        default:
                            break;
                    }
                }
            }
            screen.setDataAt(typeid(double), r*nWidth + c, (double) res);
        }
    }

    auto result = std::vector<std::reference_wrapper<const Data::DataHandler>>();
    result.push_back(screen);
    /*if(ale.getFrameNumber()==160)
    std::cout<<toString()<<std::endl;*/

    return result;
}


double ALEWrapper::getScore() const {
    return this->totalReward;
}

bool ALEWrapper::isTerminal() const {
    return ale.game_over();
}

bool ALEWrapper::isCopyable() const {
    return true;
}

Learn::LearningEnvironment *ALEWrapper::clone() const {
    // load rom : 5.6/s ! (=> 179 ms/)
    // 1 x / thread : acceptable => OK !
    return new ALEWrapper(this);
}


std::string ALEWrapper::toString() const {
    std::stringstream res;

    for (int r = 0; r < 42; r++) {
        for (int c = 0; c < 32; c++) {
            double val = (double) *(screen.getDataAt(typeid(double), c + r*32).getSharedPointer<const double>());
            std::string toAdd = val <= 2.0 ? "■" : " ";
            res << toAdd << " ";

        }
        res << std::endl;
    }
    return res.str();
}