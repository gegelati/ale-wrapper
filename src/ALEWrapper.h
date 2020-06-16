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
    ALEWrapper(std::string ROM_NAME, int actions, bool display = false) : LearningEnvironment(actions), screen(1344) {
        setAle(ROM_NAME, display);

        this->reset(0);
    };


    /// Copy constructor, ale is not trivially copyable so we create another one
    ALEWrapper(const ALEWrapper *other) : LearningEnvironment(other->legal_actions.size()), screen(other->screen),
                                          legal_actions(other->legal_actions) {
        setAle(other->ale.theOSystem->romFile(), false);
        this->reset(0);
    }

    /// Sets the configs of the ALE
    void setAle(std::string ROM_NAME, bool display) {
        // Mute ALE to enable a lot of ROM loading without flood
        ale::Logger::setMode(ale::Logger::Error);
        // Set the desired settings
        ale.setInt("random_seed", 123);
        ale.setBool("display_screen", display);
        ale.setBool("sound", display);
        ale.loadROM(ROM_NAME);
        ale.setMode(ale.getAvailableModes()[0]);
        ale.setDifficulty(ale.getAvailableDifficulties()[0]);

        // putting the game in SECAM format will let only 8 colors remaining
        ale.theOSystem->colourPalette().setPalette("standard", "SECAM");

        // fianaly gets the legal actions set
        legal_actions = ale.getLegalActionSet();
    }

    /// Destructor
    ~ALEWrapper() {};

    /// Does a given move; used when not learning
    virtual void play(uint64_t actionID);

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

    /// Inherited via LearningEnvironment
    virtual bool isCopyable() const override;

    /// Inherited via LearningEnvironment
    virtual LearningEnvironment *clone() const override;

    /// Updates the screen according to the current frame of the game
    void updateScreen();

    /// returns the screen in unicode U25A0, used for debug purpose
    std::string toString() const;

};

#endif