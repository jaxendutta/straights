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
#include <limits>
#include <functional>

#ifndef WASM
template<typename T>
T inputTaker(std::function<bool(T const&)> pred, std::string const& error_msg) {
    while(true) {
        T input;
        std::cout << "> ";
        std::cin >> input;
        bool fail = std::cin.fail() || !pred(input);
        if(fail) {
            std::cin.clear();
            std::cout << error_msg << "\n";
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>().max(), '\n');
        if(!fail) return input;
    }
    // Unreachable
}
#endif

bool strEquals(std::string const& A, std::string const& B) {
    if(A.size() != B.size()) return false;
    for(size_t i = 0; i < A.size(); i++)
        if(std::tolower(A[i]) != std::tolower(B[i]))
            return false;
    return true;
}

bool inpCheck(std::vector<std::string> const& A, std::string const& B) {
    for (int i = 0; i < A.size(); i++) {
        if (strEquals(A[i], B))
            return true;
    }
    return false;
}

void printCardVec(std::vector<Card> const &deck, bool cardmode) {
    int i = 0, j = 0, row = 0, sum = 0;
    if (!cardmode) {
        for (Card s : deck) {
            ++i;
            std::cout << s;
            if (i % 13 == 0) {
                std::cout << "\n";
            } else {
                std::cout << ' ';
            }
        }
        if (i < 13) {
            std::cout << "\n";
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
            std::cout << "\n";
            for (int d = 0; d < row; ++d) {
                if ((deck.at(d+sum)).getRank() == Rank::T) {
                    std::cout << "|10   |  ";
                } else {
                    std::cout << "|" << rank_to_string((deck.at(d+sum)).getRank()) << "    |  ";
                }
                //std::cout << "|     |  ";
            }
            std::cout << "\n";
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
            std::cout << "\n";
                for (int d = 0; d < row; ++d) {
                std::cout << "|___";
                if ((deck.at(d+sum)).getRank() == Rank::T) {
                    std::cout << "10|  ";
                } else {
                    std::cout << "_" << rank_to_string((deck.at(d+sum)).getRank()) << "|  ";
                }
            }
            std::cout << "\n";
            sum += row;
        }   
    }
    std::cout << "\n";
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
    std::cout << "╔═══════════╗\n";
    std::cout << "║ OBJECTIVE ║\n";
    std::cout << "╚═══════════╝\n\n";
    std::cout << "Straights is a four-player game. The objective is to get the fewest number of points among the players.\n";
    std::cout << "The game ends when one player accumulates 80 points or more, and the player with the lowest score is declared the winner.\n";
    std::cout << "If the lowest score is a tie, then all players with that score win.\n\n";
    std::cout << "╔══════════╗\n";
    std::cout << "║ THE DECK ║\n";
    std::cout << "╚══════════╝\n\n";
    std::cout << "Straights uses a standard 52-card deck, without the jokers. Each player is dealt 13 cards at the beginning of each round.\n";
    std::cout << "In this game, the Ace is considered to be the lowest card in each suit (rank of 1), while the King is the highest (rank of 13).\n";
    std::cout << "A Jack has a rank of 11, while the Queen has a rank of 12. The rank of all other cards is their numeric value e.g. 2 has a rank of 2.\n\n";
    std::cout << "╔══════════╗\n";
    std::cout << "║ GAMEPLAY ║\n";
    std::cout << "╚══════════╝\n";
    std::cout << "  ┌─────────────┐\n";
    std::cout << "  │ LEGAL PLAYS │\n";
    std::cout << "  └─────────────┘\n";
    std::cout << "Immediately following the deal, the player with the 7 of spades goes first.\n";
    std::cout << "This player must play the 7 of spades in the centre of the table. After the 7 of spades, the players take turns to play cards on the table.\n";
    std::cout << "At this point, the players must play cards that constitute legal plays. The following cards are legal:\n";
    std::cout << "* A 7 of any suit. This card starts a new pile on the table.\n";
    std::cout << "* A card with the same suit and adjacent rank (2A card has adjacent rank if its face value is one more or one less than the rank of card under consideration.\n";
    std::cout << "The King and Ace of a suit are not considered to be adjacent ranks to each other.) as another card that has already been played.\n";
    std::cout << "It must be played on the pile of the appropriate suit. (Note that the “pile” is spread across the table, so that play can proceed at either end.)\n";
    std::cout << "For example, if the 7 of spades is the only card on the table, then the legal plays are: the 7 of diamonds, the 7 of hearts, the 7 of clubs,\n\n";
    std::cout << "the 8 of spades, and the 6 of spades. Once the 8 of spades is played, the next legal plays are: the 9 of spades, the 6 of spades, the 7 of diamonds, the 7 of hearts, and the 7 of clubs.\n\n";
    std::cout << "  ┌──────────┐\n";
    std::cout << "  │ DISCARDS │\n";
    std::cout << "  └──────────┘\n";
    std::cout << "When a player has no legal plays, they must then choose a card in their hand, and place it face down in front of them. This is a discard.\n";
    std::cout << "Note that if a player has at least one legal play in their hand, then they must make a legal play; they may not discard in this case\n\n";
    std::cout << "╔═════════╗\n";
    std::cout << "║ SCORING ║\n";
    std::cout << "╚═════════╝\n";
    std::cout << "The round ends when all the cards have either been played or discarded. For each player, their score for the round is the sum of all the ranks of the player's discards.\n";
    std::cout << "Jacks, Queens, and Kings count as 11, 12, and 13 points, respectively. For example, if a player discarded an Ace, a Six, and a King, the number of points would be 1 + 6 + 13 = 20.\n";
    std::cout << "Each player's game score is of the sum of their scores in each round. If no player has accumulated 80 or more points at the end of a round, then the deck is reshuffled and another round begins.\n\n";
    std::cout << "╔══════════╗\n";
    std::cout << "║ COMMANDS ║\n";
    std::cout << "╚══════════╝\n\n";
    std::cout << "play <card>                       plays the card. The card must be of the format <rank><suit>. Example: play QC\n";
    std::cout << "discard <card>                    discards the card. The card must be of the format <rank><suit>. Example: discard 7D\n";
    std::cout << "deck                              shows how the deck was shuffled. WARNING: This shows you the cards of all the players.\n";
    std::cout << "cardmode <on/off>                 turns the enhanced Card View feature on or off. Example: cardmode on\n";
    std::cout << "change <player #> <difficulty>    changes the Difficulty mode of a Computer Player. Example: charge 3 e\n";
    std::cout << "                                  If Player3 is a Computer Player, its Difficulty will change to an Easy Computer Player level.\n";
    std::cout << "ragequit                          hands over your cards to a computer (Medium Difficulty) to play for you.\n";
    std::cout << "quit                              terminates the game immediately. A second confirmaton will be required.\n";
    std::cout << "help                              shows the rules, objectives, and command guide for the game.\n";
}

void gameLoop(unsigned seed) {
    // INITIALISING RELEVANT VARIABLES
    std::vector<Card> deck; // main deck
    const int numOfPlayers = 4; // number of players
    std::vector<Card> playerCards[numOfPlayers]; // array of sets of cards in the hands of every player
    std::vector<Card> playerDiscards[numOfPlayers]; // array of sets of discards of every player
    std::vector<std::unique_ptr<Player>> players; // vector of unique pointers to players
    std::vector<Card> legals;
    std::string command = ""; // stores command
    std::string hc; // stores player mode input
    std::vector<std::string> playerNames; // player names
    int prevScore[numOfPlayers]; // accumulation of previous round scores
    for (int i = 0; i < numOfPlayers; ++i) { // initialise to 0
        prevScore[i] = 0;
    }
    int turns = 13; // number of turns every round
    std::string ch[4]; // stores player mode
    int maxScore = 0; // score at game ends
    int min = std::numeric_limits<int>::max(); // to store the least score in the game
    Board board; // main game board initialisation
    int round = 0; // round counter
    int currPlayer = 0; // keeps track of the current player
    std::string mode = ""; // to store computer player mode
    int temp = 0; // temporary int variable to carry out transfers
    int winners = 0;
    int tempWinners = 0;
    std::string cardmode = "";
    bool cardview = true;
    int numLegal = 0;
    std::string quitSure = "";
    std::string compPlayer = "";
    std::string toMode = "";
    int compNum = -1;
    std::vector<std::string> diffType = {"e", "easy", "m", "medium", "h", "hard"};
    std::vector<std::string> viewType = {"y", "yes", "n", "no"};
    std::vector<std::string> commType = {"play", "discard", "deck", "cardmode", "change", "ragequit", "quit", "help"};

    // FILLING UP DECK WITH CARDS IN ORDER
    for (int i = 0; i < numOfPlayers; ++i) {
        for (int j = 1; j < 14; ++j) {
            deck.push_back(Card(static_cast<Suit>(i), static_cast<Rank>(j)));
        }
    }

    // SETTING UP PLAYERS (HUMAN/COMPUTER)
    for (int i = 0; i < numOfPlayers; i++) {
        playerNames.emplace_back("Robot "+std::to_string(i+1));
    }
    std::cout << "\n╔═════════════════════════════════════════════════╗\n";
    std::cout <<   "║ WELCOME TO STRAIGHTS: THE FOUR-PLAYER CARD GAME ║\n";
    std::cout <<   "╚═════════════════════════════════════════════════╝\n\n";
    printHelp();
    std::cout << "┌───────────────────────────┐\n";
    std::cout << "│ LET'S SET UP THE PLAYERS! │\n";
    std::cout << "└───────────────────────────┘\n\n";
    for (int i = 0; i < numOfPlayers; ++i) {
        std::cout << "\nIs Player" << (i + 1) << " a human (h) or a computer (c)?\n";
        hc = inputTaker<std::string>([](std::string const& hc) {
            return hc == "h" || hc == "c";
        }, "Invalid input! Expected 'h' or 'c'.");
        ch[i] = hc;
        if (hc == "h") {
            std::cout << "What's your name? Please don't use any spaces! Only the first connected name would be taken!\n";
            playerNames[i] = std::move(inputTaker<std::string>(
                [](std::string const& hc) { return true; }, "Invalid input! Expected a string."));
            players.emplace_back(std::make_unique<HumanPlayer>(i, playerNames[i], board, playerCards[i], playerDiscards[i], 0));
        }
        else if (hc == "c") {
            std::cout << "Select computer player difficulty: EASY(e)  MEDIUM(m)  HARD(h). Enter your choice below.\n";
            mode = std::move(inputTaker<std::string>(
                [&diffType](std::string const& mode) { 
                    return inpCheck(diffType, mode); },
                    "Invalid input! Expected one of: e, m, h."));
            if (strEquals(mode, "e") || strEquals(mode, "easy"))  {
                players.emplace_back(std::make_unique<EasyComp>(i, playerNames[i], board, playerCards[i], playerDiscards[i], 0));
            } else if (strEquals(mode, "m") || strEquals(mode, "medium")) {
                players.emplace_back(std::make_unique<MedComp>(i, playerNames[i], board, playerCards[i], playerDiscards[i], 0));
            } else {
                players.emplace_back(std::make_unique<HardComp>(i, playerNames[i], board, playerCards[i], playerDiscards[i], 0));
            }
        }
    }
    std::cout << "\nLet's set up the winning threshold! A regular game of straights has the minimum points to win set as 80. A lower threshold will make the game shorter, and vice-versa. What would you like it to be?\n";
    maxScore = inputTaker<int>([](int x){
        return x >= 1;
    }, "Invalid input! Expected threshold should be more than one (>= 1).");

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
                std::cout << "────────────────────────────────SCORE BOARD────────────────────────────────\n\n";
                for (int i = 0; i < numOfPlayers; ++i) {
                    std::cout << playerNames[i] << " (Player " << (i+1) <<")'s discarded cards: ";
                    if (playerDiscards[i].size() == 0) {
                        std::cout << "NONE\n";
                    } else {
                        std::cout << "\n";
                        printCardVec(playerDiscards[i], cardview);
                    }
                    std::cout << playerNames[i] << " (Player " << (i+1) <<")'s score: ";
                    temp = getScore(playerDiscards[i]);
                    std::cout << prevScore[i] << " + " << temp << " = " << (temp + prevScore[i]);
                    std::cout << "\n───────────────────────────────────────────────────────────────────\n";
                    prevScore[i] += temp;
                    playerDiscards[i].clear();
                }
                if (gameOver(prevScore, maxScore, numOfPlayers)) {
                    break;
                }
            }
            round++;
            turns = 0;
            std::cout << "\nThe winning threshold hasn't been met yet! Setting up new deck and table . . .\n\n";
            std::string roundNum = (round < 10)? "0" + std::to_string(round) : std::to_string(round);
            std::cout << " ╔════════════╗\n";
            std::cout << " ║  ROUND " << roundNum << "  ║\n";
            std::cout << " ╚════════════╝\n\n";
            std::cout << playerNames[player7S] << " (Player " << (player7S+1) << ") starts the round.\n\n";
        }

        // BOARD
        std::cout << " ╔════════════════════╗\n";
        std::cout << " ║ CARDS ON THE TABLE ║\n";
        std::cout << " ╚════════════════════╝\n";
        std::cout << "\nClubs: ";
        for (auto x : board.getClubs()) {
            std::cout << rank_to_string(x.getRank()) << " ";
        }
        std::cout << "\nDiamonds: ";
        for (auto x : board.getDiamonds()) {
            std::cout << rank_to_string(x.getRank()) << " ";
        }
        std::cout << "\nHearts: ";
        for (auto x : board.getHearts()) {
            std::cout << rank_to_string(x.getRank()) << " ";
        }
        std::cout << "\nSpades: ";
        for (auto x : board.getSpades()) {
            std::cout << rank_to_string(x.getRank()) << " ";
        }
        std::cout << "\n\nIt is " << playerNames[currPlayer] << " (Player " << currPlayer+1 << ")'s turn. Your hand: ";
        if (cardview) {
            std::cout << "\n";
        }
        printCardVec(playerCards[currPlayer], cardview);
        std::cout << "Your legal plays: ";
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
        if (legals.size() == 0) {
            std::cout << "NONE! You must discard!\n";
        } else {
            if (cardview) {
                std::cout << "\n";
            }
            printCardVec(legals, cardview);
            legals.clear();
        }
        
        // HUMAN PLAYER HANDLE
        if (ch[currPlayer] == "h") {
            command = std::move(inputTaker<std::string>(
                [&commType](std::string const& command) { 
                    return inpCheck(commType, command); },
                    "Invalid command. Please enter again."));
            
            // PRINT SHUFFLED DECK
            if (command == "deck") {
                printCardVec(deck, cardview);
            } 
            
            // PLAY OPTION
            else if (command == "play") {
                if (!players[currPlayer]->play()) {
                    std::cout <<  "Invalid play. Please enter a valid play.\n";
                    continue;
                }
            } 
            
            // DISCARD ACTION
            else if (command == "discard") {
                if (!players[currPlayer]->discard()) {
                    continue;
                }
            } 
            
            // QUIT / TERMINATE GAME
            else if (command == "quit") {
                std::cout << "Are you sure you want to quit the game? (y/n)\n";
                quitSure = inputTaker<std::string>([](std::string const& inp){
                    return strEquals(inp, "yes") || strEquals(inp, "y") || strEquals(inp, "no") || strEquals(inp, "n");
                }, "Please enter y for YES and n for NO.");
                if (strEquals(quitSure, "y") || strEquals(quitSure, "yes")) {
                    std::cout << "Exiting Game ... Shut down complete.\n";
                    exit(0);
                } else {
                    std::cout << "Resuming Game ... \nPlease continue with your next command.\n";
                }
            }

            // RAGEQUIT OPTION   
            else if (command == "ragequit") {
                std::unique_ptr<Player> p = std::make_unique<MedComp>(currPlayer, playerNames[currPlayer], board, playerCards[currPlayer], playerDiscards[currPlayer], getScore(playerDiscards[currPlayer]));
                players[currPlayer] = std::move(p);
                ch[currPlayer] = "c";
                std::cout << playerNames[currPlayer] << " (Player " << (currPlayer + 1) << ") ragequits. A computer will now take over.\n";
            } 
            
            // CHANGE CARD VIEW MODE
            else if (strEquals(command, "cardmode")) {
                cardmode = inputTaker<std::string>([&viewType](std::string const& cardmode){
                    return inpCheck(viewType, cardmode);
                }, "Please use the cardmode command followed by on or off.");
                if (strEquals(cardmode, "on")) {
                    if (cardview) {
                        std::cout << "Card View Mode has been on.\n";
                    } else {
                        cardview = true;
                        std::cout << "Card View Mode has been activated.\n";
                    }
                } else {
                    if (!cardview) {
                        std::cout << "Card View Mode has been off.\n";
                    } else {
                        cardview = false;
                        std::cout << "Card View Mode has been deactivated. Cards will appear in classic view now.\n";
                    }
                }
            } 
            
            // PRINT COMMAND HELP
            else if (strEquals(command, "help")) {
                printHelp();
            }
            
            // CHANGE COMPUTER DIFFICULTY
            else if (command == "change") {
                compNum = inputTaker<int>([&ch](int x){
                    return x >= 1 && x <= 4 && ch[x-1] == "c";
                }, "Invalid input! Please enter a valid computer player!");
                toMode = inputTaker<std::string>([&diffType](std::string const& toMode){
                    return inpCheck(diffType, toMode);
                }, "Invalid input! Expected a difficulty that is one of: e, m, h.");

                if (strEquals(toMode, "h") || strEquals(toMode, "hard")) {
                    players[compNum-1] = std::move(std::make_unique<HardComp>(compNum-1, playerNames[compNum-1], board, playerCards[compNum -1], playerDiscards[compNum-1], getScore(playerDiscards[compNum-1])));
                } else if (strEquals(toMode, "e") || strEquals(toMode, "easy")) {
                    players[compNum-1] = std::move(std::make_unique<EasyComp>(compNum-1, playerNames[compNum-1], board, playerCards[compNum-1], playerDiscards[compNum-1], getScore(playerDiscards[compNum-1])));
                } else if (strEquals(toMode, "m") || strEquals(toMode, "medium")) {
                    players[compNum-1] = std::move(std::make_unique<MedComp>(compNum-1, playerNames[compNum-1], board, playerCards[compNum-1], playerDiscards[compNum-1], getScore(playerDiscards[compNum-1])));
                }
                std::cout << "Player" << (compNum-1) << " is now playing in " << toMode << "-mode.\n";
            } 

            // INCREMENT TURN AFTER PLAYED
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
        std::cout << "\n";
    }

    // FINDING THE LOWEST SCORE
    for (int i = 0; i < numOfPlayers; ++i) {
        if (prevScore[i] < min) {
            min = prevScore[i];
            winners = 1;
        } else if (prevScore[i] == min) {
            winners++;
        }
    }

    // DECLARING THE WINNER
    std::cout << "╔═════════════════╗\n";
    std::cout << "║ CONGRATULATIONS ║\n";
    std::cout << "╚═════════════════╝\n\n";
    tempWinners = winners;
    for (int i = 0; i < numOfPlayers; ++i) {
        if (prevScore[i] == min) {
            std::cout << playerNames[i] << " ";
            tempWinners--;
            if (tempWinners > 0) {
                std::cout << "and ";
            }
        }
    }
    if (winners > 1) {
        std::cout << "win!\n\n";
    } else {
        std::cout << "wins!\n\n";
    }
    if (winners > 1) {
        std::cout << "It was a " << winners << "-way tie!\n\n";
    }
    std::cout << "Thanks for playing " << playerNames[0] << ", " << playerNames[1] << ", " << playerNames[2] << ", and " << playerNames[3] << "! <3\n\n";
    std::cout << "╔═══════════╗\n";
    std::cout << "║ GAME OVER ║\n";
    std::cout << "╚═══════════╝\n\n";
}

#ifndef WASM
int main(int argc, char **argv) {
    // SHUFFLING THE DECK WITH A SEED
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    if (argc > 1) {
        try {
            seed = std::stoi(std::string{argv[1]});
        }
        catch (std::invalid_argument &e) {
            std::cerr << e.what() << "\n";
            return 1;
        }
        catch (std::out_of_range &e) {
            std::cerr << e.what() << "\n";
            return -1;
        }
    }

    gameLoop(seed);
}
#endif

// ┌─────────────┐
// │ SAMPLE TEXT │
// └─────────────┘

// ╔═════════════╗
// ║ SAMPLE TEXT ║
// ╚═════════════╝