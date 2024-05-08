#include <iostream>
#include <vector>
#include <random>
#include "../include/SDL2/SDL.h"


#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 800
#define CELL_SIZE 5

const int ROW_COUNT = SCREEN_HEIGHT / CELL_SIZE;
const int CELL_COUNT = SCREEN_WIDTH / CELL_SIZE;


std::random_device rd;
std::mt19937 gen(rd());
std::bernoulli_distribution dis(0.5); // генерация случайного бита с вероятностью 0.5


struct Direction {
    int row;
    int cell;
};

Direction directions[8] = {
    {-1, -1}, {-1, 0}, {-1, 1},
    {0, -1}, {0, 1},
    {1, -1}, {1, 0}, {1, 1},
};


int calculateNeighbors(int** matrix, int row, int cell) {
    int acc = 0;

    for (int i = 0; i < sizeof(directions) / sizeof(Direction); i++) {

        int indexRow = (row + directions[i].row + ROW_COUNT) % ROW_COUNT;
        int indexCell = (cell + directions[i].cell + CELL_COUNT) % CELL_COUNT;
        acc = acc + matrix[indexRow][indexCell];
    }

    return acc;
}

int** initMatrix(int initValue) {
    int** matrix;

    matrix = new int*[ROW_COUNT];

    for (int i = 0; i < ROW_COUNT; i++) {
        matrix[i] = new int[CELL_COUNT];
    }


    for (int i = 0; i < ROW_COUNT; i++) {
        for (int j = 0; j < CELL_COUNT; j++){
            matrix[i][j] = initValue;
        }
    }

    return matrix;
}


int** initMatrixRandom() {
    int** matrix;

    matrix = new int*[ROW_COUNT];

    for (int i = 0; i < ROW_COUNT; i++) {
        matrix[i] = new int[CELL_COUNT];
    }


    for (int i = 0; i < ROW_COUNT; i++) {
        for (int j = 0; j < CELL_COUNT; j++){
            int random_number = dis(gen);
            matrix[i][j] = random_number;
        }
    }

    return matrix;
}




void displayMatrix(int** matrix, SDL_Renderer *pRenderer) {
    // std::cout << matrix.size() << std::endl;
    for (int i = 0; i < ROW_COUNT; i++) {
        for (int j = 0; j < CELL_COUNT; j++) {
            if (matrix[i][j] == 1){
                SDL_Rect rect;
                rect.h = CELL_SIZE - 1;
                rect.w = CELL_SIZE - 1;
        
                rect.x = CELL_SIZE * j;
                rect.y = CELL_SIZE * i;
                SDL_RenderFillRect(pRenderer, &rect);
            }
        }
    }
}

int** updateState(int** matrix) {
    std::cout << "updateState()" << std::endl;
    int** newMatrix = initMatrix(0);

    for (int i = 0; i < ROW_COUNT; i++) {
        for (int j = 0; j < CELL_COUNT; j++) {
            int liveNeighbors = calculateNeighbors(matrix, i, j);

            if (matrix[i][j] == 1) {
                if (liveNeighbors > 3 || liveNeighbors < 2) {
                    newMatrix[i][j] = 0;
                } else {
                    newMatrix[i][j] = 1;
                }
            } else {
                if (liveNeighbors == 3) {
                    newMatrix[i][j] = 1;
                } else {
                    newMatrix[i][j] = 0;
                }
            }
        }
    }

    return newMatrix;
}


int main() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "sdl init error" << std::endl;
        return 1;
    }

    SDL_Window *pWindow = SDL_CreateWindow("conway life", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

    if (pWindow == NULL) {
        std::cout << "window error" << std::endl;
        return 1;
    }

    SDL_Renderer *pRenderer = SDL_CreateRenderer(pWindow, -1, SDL_RENDERER_ACCELERATED);

    if (pRenderer == NULL) {
        std::cout << "renderer error" << std::endl;
        return 1;
    }

    int** matrix = initMatrixRandom();

    bool run = true;

    SDL_Event e;


    while (run) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                run = false;
            }
        }
        std::cout << "loop()" << std::endl;

        SDL_SetRenderDrawColor(pRenderer, 0, 0, 0, 0);
        // give us clear canvas
        SDL_RenderClear(pRenderer);


        // set color for rect
        SDL_SetRenderDrawColor(pRenderer, 0, 255, 0, 0);
        //display matrix
        displayMatrix(matrix, pRenderer);

        matrix = updateState(matrix);

        // draw on display
        SDL_RenderPresent(pRenderer);
    }

    SDL_DestroyWindow(pWindow);
    SDL_DestroyRenderer(pRenderer);
    pWindow = nullptr;
    pRenderer = nullptr;
    SDL_Quit();
    return 0;
}