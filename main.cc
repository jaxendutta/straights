#include "card.h"
#include "board.h"
#include "player.h"
#include "humanPlayer.h"
#include "easyComp.h"
#include "medComp.h"
#include "hardComp.h"
#include <string>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>
#include <deque>

void printCardVec(std::vector<Card> const &deck, bool cardmode) {
    int i = 0, j = 0, row = 0, sum = 0;
    if (!cardmode) {
        for (Card s : deck) {
            ++i;
            std::cout << s;
            if (i % 13 == 0) {
                std::cout << std::endl;
            } else {
                std::cout << ' ';
            }
        }
        if (i < 13) {
            std::cout << std::endl;
        }
    } else {
        j = deck.size();
        while (j > 0) {
            row = (j > 13)? 13:j;
            j -= 13;
            std::cout << " ";
            for (int d = 0; d < row; ++d) {
                std::cout << "_____    ";
            }
            std::cout << std:: endl;
            for (int d = 0; d < row; ++d) {
                if ((deck.at(d+sum)).getRank() == Rank::T) {
                    std::cout << "|10   |  ";
                } else {
                    std::cout << "|" << rank_to_string((deck.at(d+sum)).getRank()) << "    |  ";
                }
                //std::cout << "|     |  ";
            }
            std::cout << std::endl;
            for (int d = 0; d < row; ++d) {
                std::cout << "|  ";
                if ((deck.at(d+sum)).getSuit() == Suit::C) {
                    std::cout << "♣  |  ";
                } else if ((deck.at(d+sum)).getSuit() == Suit::D) {
                    std::cout << "♦  |  ";
                } else if ((deck.at(d+sum)).getSuit() == Suit::H) {
                    std::cout << "♥  |  ";
                } else {
                    std::cout << "♠  |  ";
                }
            }
            std::cout << std::endl;
                for (int d = 0; d < row; ++d) {
                std::cout << "|___";
                if ((deck.at(d+sum)).getRank() == Rank::T) {
                    std::cout << "10|  ";
                } else {
                    std::cout << "_" << rank_to_string((deck.at(d+sum)).getRank()) << "|  ";
                }
            }
            std::cout << std::endl;
            sum += row;
        }   
    }
    std::cout << std::endl;
}

int has7S(std::vector<Card> const playerCards[], const int numOfPlayers) {
    for (int i = 0; i < numOfPlayers; i++) {
        for (auto j : playerCards[i]) {
            if (j.getSuit() == Suit::S && j.getRank() == Rank::SEVEN) {
                return i;
            }
        }
    }
    return -1;
}

int getScore(std::vector<Card> discards) {
    int score = 0;
    for (auto i : discards) {
        score += static_cast<int>(i.getRank());
    }
    return score;
}

bool gameOver(int prevScore[], const int maxScore, const int numOfPlayers) {
    for (int i = 0; i < numOfPlayers; ++i) {
        if (prevScore[i] >= maxScore) {
            return true;
        }
    }
    return false;
}

void printHelp() {
    std::cout << " ** OBJECTIVES **" << std::endl;
    std::cout << "Straights is a four-player game. The objective is to get the fewest number of points among the players." << std::endl;
    std::cout << "The game ends when one player accumulates 80 points or more, and the player with the lowest score is declared the winner." << std::endl;
    std::cout << "If the lowest score is a tie, then all players with that score win." << std::endl << std::endl;
    std::cout << " ** THE DECK **" << std::endl;
    std::cout << "Straights uses a standard 52-card deck, without the jokers. Each player is dealt 13 cards at the beginning of each round." << std::endl;
    std::cout << "In this game, the Ace is considered to be the lowest card in each suit (rank of 1), while the King is the highest (rank of 13)." << std::endl;
    std::cout << "A Jack has a rank of 11, while the Queen has a rank of 12. The rank of all other cards is their numeric value e.g. 2 has a rank of 2." << std::endl << std::endl;
    std::cout << " ** GAMEPLAY **" << std::endl << "  * LEGAL PLAYS *" << std::endl;
    std::cout << "Immediately following the deal, the player with the 7 of spades goes first." << std::endl;
    std::cout << "This player must play the 7 of spades in the centre of the table. After the 7 of spades, the players take turns to play cards on the table." << std::endl;
    std::cout << "At this point, the players must play cards that constitute legal plays. The following cards are legal:" << std::endl;
    std::cout << "* A 7 of any suit. This card starts a new pile on the table." << std::endl;
    std::cout << "* A card with the same suit and adjacent rank (2A card has adjacent rank if its face value is one more or one less than the rank of card under consideration." << std::endl;
    std::cout << "The King and Ace of a suit are not considered to be adjacent ranks to each other.) as another card that has already been played." << std::endl;
    std::cout << "It must be played on the pile of the appropriate suit. (Note that the “pile” is spread across the table, so that play can proceed at either end.)" << std::endl;
    std::cout << std::endl << "For example, if the 7 of spades is the only card on the table, then the legal plays are: the 7 of diamonds, the 7 of hearts, the 7 of clubs," << std::endl;
    std::cout << "the 8 of spades, and the 6 of spades. Once the 8 of spades is played, the next legal plays are: the 9 of spades, the 6 of spades, the 7 of diamonds, the 7 of hearts, and the 7 of clubs.";
    std::cout << std::endl << std::endl;
    std::cout << "  * DISCARDS *" << std::endl;
    std::cout << "When a player has no legal plays, they must then choose a card in their hand, and place it face down in front of them. This is a discard." << std::endl;
    std::cout << "Note that if a player has at least one legal play in their hand, then they must make a legal play; they may not discard in this case" << std::endl << std::endl;
    std::cout << " ** SCORING **" << std::endl;
    std::cout << "The round ends when all the cards have either been played or discarded. For each player, their score for the round is the sum of all the ranks of the player’s discards." << std::endl;
    std::cout << "Jacks, Queens, and Kings count as 11, 12, and 13 points, respectively. For example, if a player discarded an Ace, a Six, and a King, the number of points would be 1 + 6 + 13 = 20." << std::endl;
    std::cout << "Each player’s game score is of the sum of their scores in each round. If no player has accumulated 80 or more points at the end of a round, then the deck is reshuffled and another round begins." << std::endl;
    std::cout << std::endl << std::endl << " ** COMMANDDS **" << std::endl;
    std::cout << "Use the command 'play' followed by a card (Rank first, Suit second) to play the card. Example: play QC" << std::endl;
    std::cout << "Use the command 'discard' followed by a card (Rank first, Suit second) to discard the card from your hand. Example: discard 7D" << std::endl;
    std::cout << "Use the command 'deck' to view how the deck was shuffled. WARNING: This shows you the cards of all the players." << std::endl;
    std::cout << "Use the command 'cardmode' to turn on or off the enhanced Card View feature. Example: cardmode on" << std::endl;
    std::cout << "Use command 'change' to change the Difficulty mode of a Computer Player. Example: charge 3 e" << std::endl;
    std::cout << "If Player3 is a Computer Player, its Difficulty will change to an Easy Computer Player level." << std::endl;
    std::cout << "Use the command 'ragequit' to hand over your cards to a computer (Medium Difficulty) to play for you." << std::endl;
    std::cout << "Use the command 'quit' to terminate the game immediately. A seocnd confirmaton will be required." << std::endl;
    std::cout << "Use the command 'help' to get the rules, objectives and command guide for the game." << std::endl;
}

int main(int argc, char **argv) {

    // INITIALISING RELEVANT VARIABLES
    std::vector<Card> deck; // main deck
    const int numOfPlayers = 4; // number of players
    std::vector<Card> playerCards[numOfPlayers]; // array of sets of cards in the hands of every player
    std::vector<Card> playerDiscards[numOfPlayers]; // array of sets of discards of every player
    std::vector<std::unique_ptr<Player>> players; // vector of unique pointers to players
    std::vector<Card> legals;
    std::string command = ""; // stores command
    std::string hc; // stores player mode input
    int prevScore[numOfPlayers]; // accumulation of previous round scores
    for (int i = 0; i < numOfPlayers; ++i) { // initialise to 0
        prevScore[i] = 0;
    }
    int turns = 13; // number of turns every round
    std::string ch[4]; // stores player mode
    const int maxScore = 80; // score at game ends
    int min = maxScore; // to store the least score in the game
    Board board; // main game board initialisation
    int round = 0; // round counter
    int currPlayer = 0; // keeps track of the current player
    std::string mode = ""; // to store computer player mode
    int temp = 0; // temporary int variable to carry out transfers
    int winners = 0;
    std::string cardmode = "";
    bool cardview = true;
    int numLegal = 0;
    std::string quitSure = "";
    std::string compPlayer = "";
    std::string toMode = "";
    int compNum = -1;

    // FILLING UP DECK WITH CARDS IN ORDER
    for (int i = 0; i < numOfPlayers; ++i) {
        for (int j = 1; j < 14; ++j) {
            deck.push_back(Card(static_cast<Suit>(i), static_cast<Rank>(j)));
        }
    }

    // SHUFFLING THE DECK WITH A SEED
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    if (argc > 1) {
        try {
            seed = std::stoi(std::string{argv[1]});
        }
        catch (std::invalid_argument &e) {
            std::cerr << e.what() << std::endl;
            return 1;
        }
        catch (std::out_of_range &e) {
            std::cerr << e.what() << std::endl;
            return -1;
        }
    }

    // SETTING UP PLAYERS (HUMAN/COMPUTER)
    std::cout << std::endl << "*** WELCOME TO THE FOUR-PLAYER CARD GAME OF STRAIGHTS ***" << std::endl << std::endl;
    printHelp();
    std::cout << std::endl << "Let's set up the players!" << std::endl << std::endl;
    for (int i = 0; i < numOfPlayers; ++i) {
        std::cout << "Is Player" << (i + 1) << " a human (h) or a computer (c)?" << std::endl;
        std::cin >> hc;
        while (hc != "h" && hc != "c") {
            std::cout << "Please enter 'h' (for a human player) or 'c' (for a computer player) for Player" << (i + 1) << "!" << std::endl;
            std::cin >> hc;
        }
        ch[i] = hc;
        if (hc == "h") {
            players.emplace_back(std::make_unique<HumanPlayer>(i, board, playerCards[i], playerDiscards[i], 0));
        }
        else if (hc == "c") {
            std::cout << "Select computer player difficulty: EASY(e)  MEDIUM(m)  HARD(h)" << std::endl;
            std::cout << "Enter your choice: ";
            std::cin >> mode;
            if (mode != "e" && mode != "m" && mode != "h") {
                std::cout << "Please enter a valid mode. Enter e for Easy, m for Medium, or h for Hard mode: ";
                std::cin >> mode;
            }
            if (mode == "e")  {
                players.emplace_back(std::make_unique<EasyComp>(i, board, playerCards[i], playerDiscards[i], 0));
            } else if (mode == "m") {
                players.emplace_back(std::make_unique<MedComp>(i, board, playerCards[i], playerDiscards[i], 0));
            } else {
                players.emplace_back(std::make_unique<HardComp>(i, board, playerCards[i], playerDiscards[i], 0));
            }
        }
        std::cout << std::endl;
    }

    // GAME PLAY
    while (!gameOver(prevScore, maxScore, numOfPlayers)) {
        
        // LAST ROUND ENDS -- NEW ROUND SET UP
        if (turns == 13) {
            std::shuffle(deck.begin(), deck.end(), std::default_random_engine(seed));

            // INITIALISING A MAIN PUBLIC BOARD
            for (int i = 0; i < 4; ++i) {
                board.moves.limit[i][0] = 7;
                board.moves.limit[i][1] = 7;
            }
            for (int i = 0; i < numOfPlayers; ++i) {
                playerCards[i].clear();
            }
            board.clearBoard();

            // DEALING THE CARDS TO PLAYERS
            for (int i = 0; i < numOfPlayers; ++i) {
                for (int j = 0; j < 13; ++j) {
                    playerCards[i].emplace_back(deck[13 * i + j]);
                }
            }

            // SETTING CURRENT PLAYER TO THE ONE WITH 7S
            int player7S = has7S(playerCards, numOfPlayers);
            currPlayer = player7S;
            if (round != 0) {
                for (int i = 0; i < numOfPlayers; ++i) {
                    std::cout << "Player" << (i + 1) << "'s discarded cards: " << std::endl;
                    printCardVec(playerDiscards[i], cardview);
                    /*for (auto j : playerDiscards[i]) {
                        std::cout << j << " ";
                    }*/
                    std::cout << "Player" << (i + 1) << "'s score: ";
                    temp = getScore(playerDiscards[i]);
                    std::cout << prevScore[i] << " + " << temp << " = " << (temp + prevScore[i]) << std::endl << std::endl;
                    prevScore[i] += temp;
                    playerDiscards[i].clear();
                }
                if (gameOver(prevScore, maxScore, numOfPlayers)) {
                    break;
                }
            }
            round++;
            turns = 0;
            std::cout << "A new round begins. It’s Player" << (player7S+1) << "’s turn to play." << std::endl;
        }

        // BOARD
        std::cout << "Cards on the table:" << std::endl << std::endl << "Clubs: ";
        for (auto x : board.getClubs()) {
            std::cout << rank_to_string(x.getRank()) << " ";
        }
        std::cout << std::endl << "Diamonds: ";
        for (auto x : board.getDiamonds()) {
            std::cout << rank_to_string(x.getRank()) << " ";
        }
        std::cout << std::endl << "Hearts: ";
        for (auto x : board.getHearts()) {
            std::cout << rank_to_string(x.getRank()) << " ";
        }
        std::cout << std::endl << "Spades: ";
        for (auto x : board.getSpades()) {
            std::cout << rank_to_string(x.getRank()) << " ";
        }
        std::cout << std::endl << std::endl << "Your hand: ";
        if (cardview) {
            std::cout << std::endl;
        }
        printCardVec(playerCards[currPlayer], cardview);
        std::cout << "Legal plays: ";
        if (cardview) {
            std::cout << std::endl;
        }
        for (auto x : playerCards[currPlayer]) {
            if (board.validMove(x)) {
                if (cardview == false) {
                    std::cout << x << " ";
                } else {
                    numLegal++;
                    legals.emplace_back(x);
                }
            }
        }
        printCardVec(legals, cardview);
        legals.clear();
        
        // HUMAN PLAYER HANDLE
        if (ch[currPlayer] == "h") {
            std::cin >> command;
            if (command == "deck") {
                printCardVec(deck, cardview);
            } else if (command == "play") {
                if (!players[currPlayer]->play()) {
                    std::cout <<  "Invalid play. Please enter a valid play." << std::endl;
                    continue;
                }
            } else if (command == "discard") {
                if (!players[currPlayer]->discard()) {
                    continue;
                }
            } else if (command == "quit") {
                std::cout << "Are you sure you want to quit the game? (y/n)" << std::endl;
                std::cin >> quitSure;
                while (quitSure != "y" && quitSure != "n") {
                    std::cout << "Please enter y for YES and n for NO." << std::endl;
                    std::cin >> quitSure;
                }
                if (quitSure == "y") {
                    std::cout << "EXITING GAME ... GOOD BYE ~" << std::endl;
                    exit(0);
                } else {
                    std::cout << "RESUMING GAME ... " << std::endl << "Please continue with your next command." << std::endl;
                }
                
            } else if (command == "ragequit") {
                std::unique_ptr<Player> p = std::make_unique<MedComp>(currPlayer, board, playerCards[currPlayer], playerDiscards[currPlayer], getScore(playerDiscards[currPlayer]));
                players[currPlayer] = std::move(p);
                ch[currPlayer] = "c";
                std::cout << "Player " << (currPlayer + 1) << " ragequits. A computer will now take over." << std::endl;
            } else if (command == "cardmode") {
                std::cin >> cardmode;
                if (cardmode == "on") {
                    if (cardview == true) {
                        std::cout << "Card View Mode has been on." << std::endl;
                    } else {
                        cardview = true;
                        std::cout << "Card View Mode has been activated." << std::endl;
                    }
                } else if (cardmode == "off") {
                    if (cardview == false) {
                        std::cout << "Card View Mode has been off." << std::endl;
                    } else {
                        cardview = false;
                        std::cout << "Card View Mode has been deactivated. Cards will appear in classic view now." << std::endl;
                    }
                }
            } else if (command == "help") {
                printHelp();
            } else if (command == "change") {
                std::cin >> compPlayer;
                std::cin >> toMode;
                compNum = std::stoi(compPlayer);
                if (compNum < 1 || compNum > 4 || ch[compNum-1] != "c") {
                    std::cout << "Please enter a valid computer player." << std::endl;
                } else {
                    if (toMode == "h") {
                        players[compNum-1] = std::move(std::make_unique<HardComp>(compNum-1, board, playerCards[compNum -1], playerDiscards[compNum-1], getScore(playerDiscards[compNum-1])));
                        std::cout << "Player" << (compNum-1) << " is now playing in " << toMode << "-mode." << std::endl;
                    } else if (toMode == "e") {
                        players[compNum-1] = std::move(std::make_unique<EasyComp>(compNum-1, board, playerCards[compNum-1], playerDiscards[compNum-1], getScore(playerDiscards[compNum-1])));
                        std::cout << "Player" << (compNum-1) << " is now playing in " << toMode << "-mode." << std::endl;
                    } else if (toMode == "m") {
                        players[compNum-1] = std::move(std::make_unique<MedComp>(compNum-1, board, playerCards[compNum-1], playerDiscards[compNum-1], getScore(playerDiscards[compNum-1])));
                        std::cout << "Player" << compNum << " is now playing in " << toMode << "-mode." << std::endl;
                    } else {
                        std::cout << "Please enter a valid difficulty setting from Easy(e), Medium(m), or Hard(h)." << std::endl;
                    }
                }
            } else {
                std::cout << "Invalid command. Please enter again." << std::endl;
            }
            if (command == "play" || command == "discard") {
                currPlayer++;
                if (currPlayer == 4) {
                    turns++;
                }
                currPlayer = currPlayer % 4;
            }
        }
        
        // COMPUTER PLAYER HANDLE
        else if (ch[currPlayer] == "c") {
            players[currPlayer]->play();
            currPlayer++;
            if (currPlayer == 4) {
                    turns++;
            }
            currPlayer = currPlayer % 4;
        }
        std::cout << std::endl;
    }

    // FINDING THE LOWEST SCORE
    for (int i = 0; i < numOfPlayers; ++i) {
        if (prevScore[i] < min) {
            min = prevScore[i];
        }
    }

    // DECLARING THE WINNER
    for (int i = 0; i < numOfPlayers; ++i) {
        if (prevScore[i] == min) {
            std::cout << std::endl << "Player" << (i + 1) << " wins! Congratulations!" << std::endl;
            winners++;
        }
    }
    if (winners > 1) {
        std::cout << "It was a " << winners << "-way tie!" << std::endl;
    }
    std::cout << "Thank you for playing! <3" << std::endl;
    std::cout << " *** GAME OVER *** " << std::endl;
}