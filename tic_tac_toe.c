#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BOARD_SIZE 3
#define HASH_SIZE 100


typedef enum { EASY, MEDIUM, IMPOSSIBLE } GameMode;
typedef enum { SINGLE_PLAYER, MULTI_PLAYER } GameType;

typedef struct 
{
    char playerName[50];
    int gamesPlayed;
    int gamesWon;
    int gamesLost;
    int gamesDrawn;
} PlayerStats;

typedef struct HashNode 
{
    PlayerStats stats;
    struct HashNode *next;
} HashNode;

HashNode *hashTable[HASH_SIZE];



char board[BOARD_SIZE][BOARD_SIZE];
int undoStack[9][2];
int undoTop = -1;
int aiMode;
int turn;


void initializeBoard();
void displayBoard();
int checkWin();
int isBoardFull();
void undoMove();
void recordMove(int row, int col);
int minimax(char board[BOARD_SIZE][BOARD_SIZE], int depth, int isMaximizing);
int findBlockingMove(int *blockRow, int *blockCol);
void easyMove();
void randomMove();
void impossibleMove();
void mediumMove();
int hash(char *name);
void insertPlayerStat(char *name);
void updatePlayerStat(char *name, int result);
void displayStatistics();
void singlePlayerGame(GameMode mode);
void multiplayerGame();
void clearScreen();

void mainMenu() 
{
    int choice;
    clearScreen();

    while (1) 
    {
        printf("\n Tic-Tac-Toe Game  \n\n");
        printf("--- Main Menu ---\n");
        printf("1. Play Game\n");
        printf("2. View Statistics\n");
        printf("3. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);
        
        switch (choice) 
        {
            case 1: 
            {
                clearScreen();
                int gameChoice;
                printf("\n--- Select Game Type ---\n");
                printf("1. Single Player\n");
                printf("2. Multiplayer\n");
                printf("3. Back\n");
                printf("Enter choice: ");
                scanf("%d", &gameChoice);
                clearScreen();
                
                if (gameChoice == 1) 
                {
                    int modeChoice;
                    printf("\n--- Select Difficulty ---\n");
                    printf("1. Easy\n");
                    printf("2. Medium\n");
                    printf("3. Impossible\n");
                    printf("Enter choice: ");
                    scanf("%d", &modeChoice);
                    clearScreen();
                    
                    switch (modeChoice) 
                    {
                        case 1: singlePlayerGame(EASY); break;
                        case 2: singlePlayerGame(MEDIUM); break;
                        case 3: 
                        {
                            int undoChoice;
                            printf("\nEnable Undo in Impossible Mode? (1 = Yes, 0 = No): ");
                            scanf("%d", &undoChoice);
                            clearScreen();

                            aiMode = undoChoice ? 1 : 0;
                            singlePlayerGame(IMPOSSIBLE);
                            break;
                        }
                    }
                } 
                else if (gameChoice == 2) 
                {
                    multiplayerGame();
                }
                break;
            }
            case 2:
                displayStatistics();
                break;
            case 3:
                printf("Exiting game.\n");
                exit(0);
            default:
                printf("Invalid choice. Please try again.\n");
        }
    }
}

void clearScreen() 
{

    #ifdef _WIN32
        system("cls");  
    #else
        system("clear");  
    #endif
}

int hash(char *name) 
{
    int sum = 0;
    for (int i = 0; name[i] != '\0'; i++) 
    {
        sum += name[i];
    }
    return sum % HASH_SIZE;
}


void insertPlayerStat(char *name) 
{
    int idx = hash(name);
    HashNode *current = hashTable[idx];
    

    while (current != NULL) 
    {
        if (strcmp(current->stats.playerName, name) == 0) 
        {
            return;  
        }
        current = current->next;
    }
    
    HashNode *newNode = malloc(sizeof(HashNode));
    strcpy(newNode->stats.playerName, name);
    newNode->stats.gamesPlayed = newNode->stats.gamesWon = newNode->stats.gamesLost = newNode->stats.gamesDrawn = 0;
    newNode->next = hashTable[idx];
    hashTable[idx] = newNode;
}

void updatePlayerStat(char *name, int result) 
{
    int idx = hash(name);
    HashNode *current = hashTable[idx];
    

    while (current != NULL) {
        if (strcmp(current->stats.playerName, name) == 0) {
            current->stats.gamesPlayed++;
            if (result == 1)
                current->stats.gamesWon++; 
            else if (result == -1)
                current->stats.gamesLost++; 
            else
                current->stats.gamesDrawn++;
            return; 
        }
        current = current->next;
    }
    
   
    insertPlayerStat(name); 
    updatePlayerStat(name, result); 
}

void displayStatistics() 
{
    printf("\n--- Player Statistics ---\n");
    for (int i = 0; i < HASH_SIZE; i++) 
    {
        HashNode *current = hashTable[i];
        while (current != NULL)
         {
            printf("Player: %s\n", current->stats.playerName);
            printf("Games Played: %d, Won: %d, Lost: %d, Drawn: %d\n",
                   current->stats.gamesPlayed, current->stats.gamesWon,
                   current->stats.gamesLost, current->stats.gamesDrawn);
            if (current->stats.gamesPlayed > 0) 
            {
                double winPercentage = (double)current->stats.gamesWon / current->stats.gamesPlayed * 100;
                printf("Win Percentage: %.2f%%\n", winPercentage);
            } 
            else 
            {
                printf("Win Percentage: 0.00%%\n\n");
            }
        

            current = current->next;
        }
    }
        printf("\nPress Enter to continue...");
    getchar();
    getchar(); 
    clearScreen();
}

void initializeBoard() 
{
    for (int i = 0; i < BOARD_SIZE; i++)
        for (int j = 0; j < BOARD_SIZE; j++)
            board[i][j] = ' ';
}

void displayBoard() 
{
    printf("\n");
    for (int i = 0; i < BOARD_SIZE; i++) 
    {
        for (int j = 0; j < BOARD_SIZE; j++) {
            printf(" %c ", board[i][j]);
            if (j < BOARD_SIZE - 1) printf("|");
        }
        printf("\n");
        if (i < BOARD_SIZE - 1) printf("---+---+---\n");
    }
    printf("\n");
}



int checkWin() 
{
    for (int i = 0; i < BOARD_SIZE; i++) 
    {
        if (board[i][0] == board[i][1] && board[i][1] == board[i][2] && board[i][0] != ' ') 
        {
            return board[i][0] == 'X' ? 1 : -1;
        }
        if (board[0][i] == board[1][i] && board[1][i] == board[2][i] && board[0][i] != ' ') 
        {
            return board[0][i] == 'X' ? 1 : -1;
        }
    }

    if (board[0][0] == board[1][1] && board[1][1] == board[2][2] && board[0][0] != ' ') 
    {
        return board[0][0] == 'X' ? 1 : -1;
    }
    if (board[0][2] == board[1][1] && board[1][1] == board[2][0] && board[0][2] != ' ') 
    {
        return board[0][2] == 'X' ? 1 : -1;
    }

    return 2; 
}



int isBoardFull() 
{
    for (int i = 0; i < BOARD_SIZE; i++) 
    {
        for (int j = 0; j < BOARD_SIZE; j++) 
        {
            if (board[i][j] == ' ') {
                return 0;  
            }
        }
    }
    return 1;  
}


void undoMove() {
    if (undoTop < 1) 
    { 
        printf("No moves to undo.\n");
        return;
    }


    int aiRow = undoStack[undoTop][0];
    int aiCol = undoStack[undoTop][1];
    board[aiRow][aiCol] = ' ';

  
    int playerRow = undoStack[undoTop - 1][0];
    int playerCol = undoStack[undoTop - 1][1];
    board[playerRow][playerCol] = ' ';


    undoTop -= 2;
    printf("Last move undone.\n");
}



void recordMove(int row, int col) 
{
    if (undoTop >= 9) 
    { 
        printf("Undo stack full.\n");
        return;
    }
    undoTop++;
    undoStack[undoTop][0] = row;
    undoStack[undoTop][1] = col;
}

int findBlockingMove(int *blockRow, int *blockCol) 
{
    for (int i = 0; i < BOARD_SIZE; i++) 
    {
        for (int j = 0; j < BOARD_SIZE; j++) 
        {
            if (board[i][j] == ' ') 
            { 
                board[i][j] = 'X';   
                int winCheck = checkWin(); 
                board[i][j] = ' ';   

                if (winCheck == 1) 
                {
                    *blockRow = i;
                    *blockCol = j;
                    return 1;        
                }
            }
        }
    }
    return 0; 
}


void easyMove() {
    int row, col;
    if (findBlockingMove(&row, &col)) 
    {
        board[row][col] = 'O'; 
        printf("AI move is (%d, %d)\n", row + 1, col + 1);
    } 
    else 
    {
        randomMove();
    }
    recordMove(row, col);
}

void randomMove() 
{
    int row, col;
    do {
        row = rand() % BOARD_SIZE;
        col = rand() % BOARD_SIZE;
    } while (board[row][col] != ' '); 

    board[row][col] = 'O';
    printf("AI move is (%d, %d)\n", row + 1, col + 1);
    recordMove(row, col);
}


void mediumMove() 
{
    easyMove();
}

void impossibleMove() 
{
    int bestScore = -1000, moveRow = -1, moveCol = -1;

    for (int i = 0; i < BOARD_SIZE; i++) 
    {
        for (int j = 0; j < BOARD_SIZE; j++) 
        {
            if (board[i][j] == ' ') 
            { 
                board[i][j] = 'O'; 
                int score = minimax(board, 0, 0); 
                board[i][j] = ' '; 

                if (score > bestScore) 
                { 
                    bestScore = score;
                    moveRow = i;
                    moveCol = j;
                }
            }
        }
    }


    board[moveRow][moveCol] = 'O';
    recordMove(moveRow, moveCol);

    int result = checkWin();
    if (result == -1 && aiMode) 
    {
        displayBoard();
        printf("AI wins! Would you like to redo the last move? (1 for Yes, 0 for No): ");
        int redoChoice;
        scanf("%d", &redoChoice);

        if (redoChoice == 1)
        {
            undoMove(); 
            printf("Redo applied. Your turn again.\n");
        } 
        else 
        {
            printf("Game over. AI wins!\n");
        }
    }
}


int minimax(char b[BOARD_SIZE][BOARD_SIZE], int depth, int isMaximizing) 
{
    int score = checkWin();

    if (score == 1) return -10 + depth; 
    if (score == -1) return 10 - depth; 
    if (isBoardFull()) return 0; 

    int bestScore = isMaximizing ? -1000 : 1000;

    for (int i = 0; i < BOARD_SIZE; i++) 
    {
        for (int j = 0; j < BOARD_SIZE; j++) 
        {
            if (b[i][j] == ' ') 
            {
                b[i][j] = isMaximizing ? 'O' : 'X';
                int currentScore = minimax(b, depth + 1, !isMaximizing);
                b[i][j] = ' '; 

                if (isMaximizing) 
                {
                    bestScore = currentScore > bestScore ? currentScore : bestScore;
                } 
                else 
                {
                    bestScore = currentScore < bestScore ? currentScore : bestScore;
                }
            }
        }
    }

    return bestScore;
}


void singlePlayerGame(GameMode mode)
{
    char playerName[50];
    printf("Enter your name: ");
    scanf("%s", playerName);
    clearScreen();

    insertPlayerStat(playerName);

    int result;
    initializeBoard();
    undoTop = -1;


    while (1) 
    {
        int row, col;
        displayBoard();
        printf("Your move 'X'(row col): ");
        if (scanf("%d %d", &row, &col) != 2) 
        {
            printf("Invalid input. Please enter numeric values for row and column.\n");
            while (getchar() != '\n');  
            continue;
        }
        row--;
        col--;

        if (row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE || board[row][col] != ' ') 
        {
            printf("Invalid move. Try again.\n");
            continue;
        }

        board[row][col] = 'X';
        recordMove(row, col);

        result = checkWin();
        if (result == 1) 
        {
            displayBoard();
            printf("Congratulations, you won!\n");
            updatePlayerStat(playerName, 1);
            break;
        }

        if (isBoardFull()) 
        {
            displayBoard();
            printf("It's a draw.\n");
            updatePlayerStat(playerName, 0);
            break;
        }

        if (mode == EASY) 
        {
            easyMove();
        } 
        else if (mode == MEDIUM) 
        {
            mediumMove();
        } 
        else 
        {
            impossibleMove();
        }

        result = checkWin();
        if (result == -1) 
        {
            displayBoard();
            printf("You lost!\n");
            updatePlayerStat(playerName, -1);
            break;
        }

        if (isBoardFull()) 
        {
            displayBoard();
            printf("It's a draw.\n");
            updatePlayerStat(playerName, 0);
            break;
        }

        if (mode == EASY) 
        {
            char undoChoice;
            displayBoard();
            printf("Undo move? (y/n): ");
            scanf(" %c", &undoChoice);
            if (undoChoice == 'y') 
            {
                undoMove(); 
                continue;   
            }
        }
    }
    printf("\nPress Enter to continue...");
    getchar(); 
    getchar(); 
    clearScreen();
     
}

void multiplayerGame() 
{
    char player1[50], player2[50];
    printf("Enter name of Player 1 'X': ");
    scanf("%s", player1);
    printf("Enter name of Player 2 'O': ");
    scanf("%s", player2);
    insertPlayerStat(player1);
    insertPlayerStat(player2);

    int result;
    initializeBoard();
    turn = 1; 

    while (1) 
    {
        int row, col;
        displayBoard();
        printf("%s's move (row col): ", turn ? player1 : player2);
        if (scanf("%d %d", &row, &col) != 2) 
        {
            printf("Invalid input. Please enter numeric values for row and column.\n");
            while (getchar() != '\n');  
            continue;
        }
        row--;
        col--;

        if (row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE || board[row][col] != ' ') 
        {
            printf("Invalid move. Try again.\n");
            continue;
        }

        board[row][col] = turn ? 'X' : 'O';

        result = checkWin();
        if (result == 1 || result == -1) 
        {
            displayBoard();
            if (turn) 
            {
                printf("Player 1 (%s) wins!\n", player1);
                updatePlayerStat(player1, 1);
                updatePlayerStat(player2, -1);
            }
            else 
            {
                printf("Player 2 (%s) wins!\n", player2);
                updatePlayerStat(player2, 1);
                updatePlayerStat(player1, -1);
            }
            break;
        }


        if (isBoardFull()) 
        {
            displayBoard();
            printf("It's a draw.\n");
            updatePlayerStat(player1, 0);
            updatePlayerStat(player2, 0);
            break;
        }
        turn = !turn;
    }
    printf("\nPress Enter to continue...");
    getchar(); 
    getchar(); 
    clearScreen();
}

int main() 
{
    mainMenu();
    return 0;
}