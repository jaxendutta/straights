#include "humanPlayer.h"

HumanPlayer::HumanPlayer(int playerNum, Board & board, std::vector<Card> & cards, std::vector<Card> & discards, int score):
Player(playerNum, board, cards, discards, score) {}

bool HumanPlayer::play() {
    Card cardChoice = cards.front();
    try {
        std::cin >> cardChoice;
    } catch (std::invalid_argument &e) {
        std::cerr << e.what() << std::endl;
        return false;
    }
    
    if (!(board.validMove(cardChoice))) {
        return false;
    }

    auto it = find(cards.begin(), cards.end(), cardChoice);
    if (it != cards.end()) {
        std::cout << "Player" << (playerNum+1) << " plays " << cardChoice << "." << std::endl;
        board.play(cardChoice);
        cards.erase(it);
        return true;
    }
    else {
        return false;
    }
}

bool HumanPlayer::discard() {
    Card cardChoice = cards.front();
    int counter = -1;
    int index = -1;
    try {
        std::cin >> cardChoice;
    } catch (std::invalid_argument &e) {
        std::cerr << e.what() << std::endl;
        return false;
    }

    // CHECKING IF LEGAL MOVES EXIST
    for (auto x : cards) {
        if (board.validMove(x)) {
            std::cout << "You have a legal play. You may not discard." << std::endl;
            return false;
        }
    }

    // CHECKING IF THE CARD IS OWNED
    for (auto x : cards) {
        counter++;
        if (x.getSuit() == cardChoice.getSuit() && x.getRank() == cardChoice.getRank()) {
            index = counter;
        }
    }

    // DISCARDING CARD
    if (index == -1) {
        std::cout << "Please discard a valid card." << std::endl;
        return false;
    } else {
        std::cout << "Player" << (playerNum+1) << " discards " << cardChoice << "." << std::endl;
        discards.emplace_back(cardChoice);
        cards.erase(cards.begin()+index);
        return true;
    }
}