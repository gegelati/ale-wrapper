#ifndef TIC_TAC_TOE_WITH_OPPONENT_H
#define TIC_TAC_TOE_WITH_OPPONENT_H

#include <random>

#include <gegelati.h>
#include <ale/ale_interface.hpp>


/**
* LearningEnvironment to play a game from ALE (Arcade Learning Environment : https://github.com/mgbellemare/Arcade-Learning-Environment)
*
* In this LearningEnvironment, the trained agent plays against a "smart" algo
* that plays the optimal (winning) strategy, with a controlled error rate
* (defined as an ugly constant attribute).
*/
class ALEWrapper : public Learn::LearningEnvironment {
protected:
    /// ALE handler
    ale::ALEInterface ale;

    /// Vector containing the screen of the atari games (eventually preprocessed)
    Data::PrimitiveTypeArray<double> screen;

    /// vector of available actions
    ale::ActionVect legal_actions;

    /// current reward obtained in the game
    float totalReward;


public:
    /**
    * Constructor.
    */
    ALEWrapper(std::string ROM_NAME, int actions) : LearningEnvironment(actions), screen(100800) {
        setAle(ROM_NAME);

        this->reset(0);
    };



    void setAle(std::string ROM_NAME){
        // Set the desired settings
        //ale::Logger::setMode(ale::Logger::Error);
        ale.loadROM(ROM_NAME);
        ale.setInt("random_seed", 123);
        //The default is already 0.25, this is just an example
        ale.setFloat("repeat_action_probability", 0.25);
        ale.setBool("display_screen", false);
        ale.setBool("sound", false);

        // fianlly gets the legal actions set
        legal_actions = ale.getLegalActionSet();
    }

    /// Destructor
    ~ALEWrapper() {};

    /// Inherited via LearningEnvironment
    virtual void doAction(uint64_t actionID) override;

    /// Inherited via LearningEnvironment
    virtual void reset(size_t seed = 0, Learn::LearningMode mode = Learn::LearningMode::TRAINING) override;

    /// Inherited via LearningEnvironment
    virtual std::vector<std::reference_wrapper<const Data::DataHandler>> getDataSources() override;

    /**
    * Inherited from LearningEnvironment.
    *
    * The score will be directly given by totalReward
    */
    virtual double getScore() const override;

    /// Inherited via LearningEnvironment
    virtual bool isTerminal() const override;


};

#endif