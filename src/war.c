#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <unistd.h>
#include "myFunctions.h"
#include "generic_stack.c"

#define FULL_SUIT 13
#define SUIT_COUNT 4
#define CARD_POINTS {14, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};
#define CARD_NUMBER_NAMES {"Ace", "Two", "Three", "Four", "Five", \
  "Six", "Seven", "Eight", "Nine", "Ten", "Jack", "Queen", "King"}
#define CARD_SUIT_NAMES {"Hearts", "Diamonds", "Spades", "Clubs"}
#define SAVE_FILE "SaveState.txt"

typedef struct Card{
  int value;
  int suit;
  int points;
  char name[64];
} Card;

typedef struct PlayerHand{
  Card* cards;
  bool* played;
} PlayerHand;

typedef struct{
  int playerCount;
  int rolloverPoints;
  int roundNum;
  PlayerHand* playerHands;
  int* points;
} LoadData;

DECL_STACK(CardStack, Card);

void populateDeck(CardStack* deck);
void printCard(Card card);
void printDeck(CardStack* deck);
void shuffleDeck(CardStack* deck);
void initPlayerHand(PlayerHand* hand);
void freePlayerHand(PlayerHand* hand);
void printPlayerHand(PlayerHand* hand);
void playGame(PlayerHand* playerHands, int playerCount, LoadData* loadData); 
bool* checkDuplicates(Card* cards, int len); 
int getMaxCardIndex(Card* cards, bool* duplicates, int len); 
int getRoundPoints(Card* cards, int len); 
void printCardsPlayed(Card* cards, bool* duplicates, int len);
void printLeaderboard(int* points, int len, int rolloverPoints); 
void printGameResults(int* points, int len); 
void saveGame(int playerCount, int rolloverPoints, int roundNum, PlayerHand* hands, int* points); 
LoadData* loadGame();
bool askYesNo(char* display); 
void populateCard(int value, int suit, Card* card);
void printDots(int dots, double seconds);

int main(){ 
  CardStack* deck = NULL; 
  int playerCount; 
  LoadData* loadData = NULL;
  PlayerHand* playerHands;

  bool load = askYesNo("Load saved game? (Y/N): ");
  if(load){
    printDots(3, 2);
    loadData = loadGame(); 
  }

  if(loadData){
    playerCount = loadData->playerCount;
    playerHands = loadData->playerHands;
  }else{
    deck = CardStack_create(FULL_SUIT * SUIT_COUNT);
    populateDeck(deck);
    shuffleDeck(deck);
    //printDeck(deck);

    playerCount = (int)getLong("How many people are playing?: ");
    while(playerCount < 2 || playerCount > 4){
      printf("Invalid player count, only 2-4 people can play.\n");
      playerCount = (int)getLong("How many people are playing?: ");
    }

    playerHands = (PlayerHand*)calloc(playerCount, sizeof(PlayerHand));

    for(int i = 0; i < playerCount; ++i){
      initPlayerHand(&playerHands[i]);
      //printf("Player %d hand:\n", i + 1);
      for(int j = 0; j < FULL_SUIT; ++j){
         CardStack_pop(deck, &playerHands[i].cards[j]);
        //printCard(playerHands[i].cards[j]);
      }
    }
  }

  playGame(playerHands, playerCount, loadData);

  //free all dynamically allocated memory
  for(int i = 0; i < playerCount; ++i)
    freePlayerHand(&playerHands[i]);
  free(playerHands);
  if(deck)
    CardStack_free(deck);
  if(loadData)
    free(loadData);

  return EXIT_SUCCESS;
}

void populateDeck(CardStack* deck){
  const int card_points[FULL_SUIT] = CARD_POINTS;
  const char card_number_names[FULL_SUIT][6] = CARD_NUMBER_NAMES;
  const char card_suit_names[4][10] = CARD_SUIT_NAMES;

  for(int i = 0; i < SUIT_COUNT; ++i){
    for(int j = 0; j < FULL_SUIT; ++j){
      Card card;
      populateCard(j, i, &card);
      CardStack_push(deck, card);

      //printCard(card);
      
      //debug
      /*
      Card cardToPrint;
      if(CardStack_peek(deck, &cardToPrint))
        printCard(cardToPrint);
      */
    }
  }
}

void printCard(Card card){
    printf("Card: %s\n", card.name);
    printf("Number: %d\n", card.value);
    printf("Suit: %d\n", card.suit);
    printf("Points: %d\n\n", card.points);
}

void printDeck(CardStack* deck){
  printf("Deck print start:\n");
  while(!CardStack_isEmpty(deck)){
    Card card;
    CardStack_pop(deck, &card);

    printf("Card: %s\n", card.name);
    printf("Number: %d\n", card.value);
    printf("Suit: %d\n", card.suit);
    printf("Points: %d\n\n", card.points);
  }
  printf("Deck print end.\n");
}

void shuffleDeck(CardStack* deck){
  srand(time(NULL));

  for(int i = 0; i < 2000; ++i){
    Card temp;
    int index1 = rand() % 52, index2 = rand() % 52;

    temp = deck->array[index2];
    deck->array[index2] = deck->array[index1];
    deck->array[index1] = temp;
  }
  //printDeck(deck);

}

void initPlayerHand(PlayerHand* hand){
  hand->cards = (Card*)calloc(FULL_SUIT, sizeof(Card));
  hand->played = (bool*)calloc(FULL_SUIT, sizeof(bool));
}

void freePlayerHand(PlayerHand* hand){
  if(hand == NULL)
    return;
  free(hand->cards);
  free(hand->played);
}

void printPlayerHand(PlayerHand* hand){
  for(int i = 0; i < FULL_SUIT; ++i){
    char lastChar = i == FULL_SUIT - 1 ? '\0' : ',';
    if(!hand->played[i]){
      printf("%d: %s%c\n", i + 1, hand->cards[i].name, lastChar);
    }
  }
}

void playGame(PlayerHand* playerHands, int playerCount, LoadData* loadData){
  int* points;
  int rolloverPoints = 0;
  int round = 0;

  if(loadData){
    points = loadData->points;
    rolloverPoints = loadData->rolloverPoints;
    round = loadData->roundNum;

    printLeaderboard(points, playerCount, rolloverPoints);

  }else{
    points = (int*)calloc(playerCount, sizeof(int));
  }
  
  for(int i = round; i < FULL_SUIT; ++i){
    Card playedThisRound[playerCount];
    bool* duplicates;
    int maxCardIndex;

    printf("ROUND %d\n\n", i + 1);
    for(int j = 0; j < playerCount; ++j){
      printf("Player %d's deck:\n", j + 1);
      printPlayerHand(&playerHands[j]);

      int cardPlayed = (int)getLong("\nWhich card would you like to play?: ") - 1;
      while(playerHands[j].played[cardPlayed] || cardPlayed >= FULL_SUIT || cardPlayed < 0){
        printf("Invalid card number; choose a different card.\n");
        cardPlayed = (int)getLong("\nWhich card would you like to play?: ") - 1;
      }
      if(j != playerCount - 1){
        printf("Pass the computer over to the player %d!\n", j + 2);
        printDots(3, 2);
      }else{
        puts("");
      }

      playerHands[j].played[cardPlayed] = true;
      playedThisRound[j] = playerHands[j].cards[cardPlayed];
    }
    
    duplicates = checkDuplicates(playedThisRound, playerCount);
    printCardsPlayed(playedThisRound, duplicates, playerCount);
    maxCardIndex = getMaxCardIndex(playedThisRound, duplicates, playerCount);

    if(maxCardIndex == -1){
      rolloverPoints += getRoundPoints(playedThisRound, playerCount);
      printf("Nobody wins this round!\nPoints rolled over: %d\n", rolloverPoints);
    }else{
      int roundPoints = getRoundPoints(playedThisRound, playerCount);
      points[maxCardIndex] += roundPoints;
      points[maxCardIndex] += rolloverPoints;

      printf("Player %d wins this round!\n", maxCardIndex + 1);
      printf("Points added from this round: %d\n", roundPoints);
      printf("Points rolled over from previous rounds: %d\n\n", rolloverPoints);

      rolloverPoints = 0;
    }

    if(i == FULL_SUIT - 1){
      free(duplicates);
      break;
    }

    printLeaderboard(points, playerCount, rolloverPoints);

    bool cont = askYesNo("Continue? (Y/N): ");
    bool save = askYesNo("Save game? (Y/N): ");
    puts("");

    if(save)
      saveGame(playerCount, rolloverPoints, i, playerHands, points);
    if(!cont){
      free(duplicates);
      free(points);
      exit(0);
    }

    free(duplicates);
  }
  if(rolloverPoints != 0)
    printf("%d points were lost in battle!\n", rolloverPoints);

  remove(SAVE_FILE);
  printGameResults(points, playerCount);

  free(points);
}

//returns dynamically allocated array, must be freed
bool* checkDuplicates(Card* cards, int len){
  bool* duplicates = (bool*)calloc(len, sizeof(bool));

  for(int i = 0; i < len; ++i){
    for(int j = i + 1; j < len && !duplicates[i]; ++j){
      if(cards[i].value == cards[j].value){
        duplicates[i] = true;
        duplicates[j] = true;
      }
    }
  }
  //debug
  /*
  printf("From getDuplicates:\n");
  for(int i = 0; i < len; ++i)
    printf("%d,", duplicates[i]);
  printf("\n");
  */

  return duplicates;
}

int getMaxCardIndex(Card* cards, bool* duplicates, int len){
  int maxCardIndex = -1;
  int maxPoints = 0;

  for(int i = 0; i < len; ++i){
    if(!duplicates[i] && cards[i].points > maxPoints){
      maxCardIndex = i;
      maxPoints = cards[i].points;
    }
  }

  return maxCardIndex;
}

int getRoundPoints(Card* cards, int len){
  int points = 0;

  for(int i = 0; i < len; ++i){
    points += cards[i].points;
  }

  return points;
}

void printCardsPlayed(Card* cards, bool* duplicates, int len){
  printf("CARDS PLAYED THIS ROUND:\n");
  for(int i = 0; i < len; ++i){
    //printCard(cards[i]);
    printf("%d: %s - %s\n", i + 1, cards[i].name, duplicates[i] ? "DUPLICATED" : "Not Duplicated");
  }
  printf("\n");
}

void printLeaderboard(int* points, int len, int rolloverPoints){
  printf("LEADERBOARDS:\n\n");
  for(int i = 0; i < len; ++i)
    printf("Player %d: %d points\n", i + 1, points[i]);
  printf("\nRollover points: %d\n\n", rolloverPoints);
}

void printGameResults(int* points, int len){
  int maxPoints = 0, winner = -1;
  printf("\n---GAME RESULTS---\n\n");

  for(int i = 0; i < len; ++i){
    printf("Player %d: %d points\n", i + 1, points[i]);
    if(points[i] > maxPoints){
      maxPoints = points[i];
      winner = i;
    }
  }

  printf("\nWINNER: Player %d!\nPOINTS: %d\n", winner + 1, maxPoints);
}

void saveGame(int playerCount, int rolloverPoints, int roundNum, PlayerHand* hands, int* points){
  FILE* file = fopen(SAVE_FILE, "w");
  fprintf(file, "%d,%d,%d\n", playerCount, rolloverPoints, roundNum + 1);

  for(int i = 0; i < playerCount; ++i){
    char c = i == playerCount - 1 ? '\n' : ',';
    fprintf(file, "%d%c", points[i], c);
  }
  for(int i = 0; i < playerCount; ++i){
    for(int j = 0; j < FULL_SUIT; ++j){
      char c = j == FULL_SUIT - 1 ? '\n' : ',';
      fprintf(file, "%d%c", hands[i].cards[j].value, c);
    }
    for(int j = 0; j < FULL_SUIT; ++j){
      char c = j == FULL_SUIT - 1 ? '\n' : ',';
      fprintf(file, "%d%c", hands[i].cards[j].suit, c);
    }
    for(int j = 0; j < FULL_SUIT; ++j){
      char c = j == FULL_SUIT - 1 ? '\n' : ',';
      fprintf(file, "%d%c", hands[i].played[j], c);
    }
  }

  fclose(file);
}

LoadData* loadGame(){
  FILE* file = fopen(SAVE_FILE, "r");
  LoadData* loadData = malloc(sizeof(LoadData));
  int matched;

  if(!file){
    printf("Failed to load game data.\n");
    free(loadData);
    return NULL;
  }

  matched = fscanf(file, "%d,%d,%d\n", &loadData->playerCount, &loadData->rolloverPoints, &loadData->roundNum);

  if(matched != 3){
    printf("Failed to load game data.\n");
    free(loadData);
    return NULL;
  }

  if(loadData->playerCount < 2 || loadData->playerCount > 4){
    printf("Player count in load data is too large or too big.\n");
    return NULL;
  }

  if(loadData->roundNum < 0 || loadData->roundNum > 12){
    printf("Round number in load data is invalid.\n");
    return NULL;
  }

  loadData->points = (int*)calloc(loadData->playerCount, sizeof(int));

  for(int i = 0; i < loadData->playerCount; ++i){
    matched = fscanf(file, "%d%*c", &loadData->points[i]);
    if(matched != 1){
      free(loadData->points);
      free(loadData);
      printf("Failed to load game data.\n");
      return NULL;
    }
  }

  loadData->playerHands = (PlayerHand*)calloc(loadData->playerCount, sizeof(PlayerHand));

  for(int i = 0; i < loadData->playerCount; ++i){
    initPlayerHand(&loadData->playerHands[i]);
    int vals[FULL_SUIT], suits[FULL_SUIT];
    bool err = false;

    for (int j = 0; j < FULL_SUIT; j++) {
      matched = fscanf(file, "%d%*c", &vals[j]);
      if(matched != 1)
        err = true;  
    }
    for (int j = 0; j < FULL_SUIT; j++) {
      matched = fscanf(file, "%d%*c", &suits[j]);
      if(matched != 1)
        err = true;
    }

    for(int j = 0; j < FULL_SUIT; ++j){
      populateCard(vals[j], suits[j], &loadData->playerHands[i].cards[j]);
    }

    for (int j = 0; j < FULL_SUIT; j++) {
      matched = fscanf(file, "%d%*c", &loadData->playerHands[i].played[j]);
      if(matched != 1)
        err = true;
    }

    if(err){
      free(loadData->points);
      for(int k = i; k >= 0; --k) //this was originally in the j loops hence k
        freePlayerHand(&loadData->playerHands[k]);
      free(loadData->playerHands);  
      free(loadData);
      printf("Failed to load game data.\n");
      return NULL;
    }

  }

  fclose(file);
  return loadData;
}

bool askYesNo(char* display){
  printf(display);
  char c = toupper(getchar());
  clearKBBuff();

  if(c == 'Y')
    return true;
  return false;
}

void populateCard(int value, int suit, Card* card){
  const int card_points[FULL_SUIT] = CARD_POINTS;
  const char card_number_names[FULL_SUIT][6] = CARD_NUMBER_NAMES;
  const char card_suit_names[4][10] = CARD_SUIT_NAMES;

  card->value = value;
  card->suit = suit;
  card->points = card_points[card->value];
  card->name[0] = '\0';
  strcat(card->name, card_number_names[card->value]);
  strcat(card->name, " of ");
  strcat(card->name, card_suit_names[card->suit]);
  strcat(card->name, "\0");
}


void printDots(int dots, double seconds){
  printf("\n");
  for(int i = 0; i < dots; ++i){
    putchar('.');
    fflush(stdout);
    //for some reason it works with the normal linux sleep functions 
    //but not with the one i wrote to be platform independent that all it does is call those functions?
    //my function works fine otherwise
    //but unfortunately this wont work for windows
    usleep((useconds_t)(1e6 * seconds / dots));
    //sleep_s(seconds / dots);
  }
  printf("\n\n");
}
