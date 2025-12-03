#include <stdio.h>
#include <SDL2/SDL.h>
#include <math.h>
#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 1000

float CubeOriginal[8][3] = {
    {-1, -1, -1}, {1, -1, -1}, {1, 1, -1}, {-1, 1, -1}, 
    {-1, -1, 1}, {1, -1, 1}, {1, 1, 1}, {-1, 1, 1} 
};

int edges[12][2] = {
    {0,1}, {1,2}, {2,3}, {3,0}, 
    {4,5}, {5,6}, {6,7}, {7,4},
    {0,4}, {1,5}, {2,6}, {3,7} 
};

float MatrixCoord[4][4] = {0};

void UpdateMatrix(float CoordMatrix[4][4], float rezultMatrix[4][4]){
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            CoordMatrix[i][j] = rezultMatrix[i][j];
        }
    }
}

void Matrix(float matrix1[4][4], float matrix2[4][4], float rezultMatrix[4][4]){
    for (int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            rezultMatrix[i][j] = 0.0;
            for(int k = 0; k < 4; k++){
                rezultMatrix[i][j] += matrix1[i][k] * matrix2[k][j];
            }
        }
    }
}

void edMatrix(float matrix[4][4]){
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            matrix[i][j] = (i == j) ? 1.0 : 0.0;
        }
    }
}

void translateMatrix(float trx, float try, float trz){
    float temp[4][4];
    edMatrix(temp);
    temp[0][3] = trx;
    temp[1][3] = try;
    temp[2][3] = trz;
    float result[4][4];
    Matrix(MatrixCoord, temp, result);
    UpdateMatrix(MatrixCoord, result);
}

void rotateXMatrix(float angle){
    float temp[4][4];
    edMatrix(temp);
    float Sin = sinf(angle);
    float Cos = cosf(angle);
    temp[1][1] = Cos;
    temp[1][2] = -Sin;
    temp[2][1] = Sin;
    temp[2][2] = Cos;
    float result[4][4];
    Matrix(MatrixCoord, temp, result);
    UpdateMatrix(MatrixCoord, result);
}

void rotateYMatrix(float angle){
    float temp[4][4];
    edMatrix(temp);
    float Sin = sinf(angle);
    float Cos = cosf(angle);
    temp[0][0] = Cos;
    temp[0][2] = Sin;
    temp[2][0] = -Sin;
    temp[2][2] = Cos;
    float result[4][4];
    Matrix(MatrixCoord, temp, result);
    UpdateMatrix(MatrixCoord, result);
}

void rotateZMatrix(float angle){
    float temp[4][4];
    edMatrix(temp);
    float Sin = sinf(angle);
    float Cos = cosf(angle);
    temp[0][0] = Cos;
    temp[0][1] = -Sin;
    temp[1][0] = Sin;
    temp[1][1] = Cos;
    float result[4][4];
    Matrix(MatrixCoord, temp, result);
    UpdateMatrix(MatrixCoord, result);
}

void scaleMatrix(float scale_val){
    float temp[4][4];
    edMatrix(temp);
    temp[0][0] = scale_val;
    temp[1][1] = scale_val;
    temp[2][2] = scale_val;
    float result[4][4];
    Matrix(MatrixCoord, temp, result);
    UpdateMatrix(MatrixCoord, result);
}

void transformPoint(float originalMatrix[3], float coordMatrix[4][4], float result[3]){
    float temp[4] = {originalMatrix[0], originalMatrix[1], originalMatrix[2], 1.0};
    float result4d[4] = {0};
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            result4d[i] += coordMatrix[i][j] * temp[j];
        }
    }
    result[0] = result4d[0];
    result[1] = result4d[1];
    result[2] = result4d[2];
}

void project3DTo2D(float point3d[3], int point2d[2]) {
    float scale_3d = 100.0f;
    point2d[0] = (int)(point3d[0] * scale_3d + SCREEN_WIDTH / 2);
    point2d[1] = (int)(point3d[1] * scale_3d + SCREEN_HEIGHT / 2);
}

void drawCube(SDL_Renderer* renderer) {
    float transformedCube3d[8][3];
    int transformedCubeTo2d[8][2];
    
    for(int i = 0; i < 8; i++){
        transformPoint(CubeOriginal[i], MatrixCoord, transformedCube3d[i]);
        project3DTo2D(transformedCube3d[i], transformedCubeTo2d[i]);
    }
    
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    
    for(int i = 0; i < 12; i++){
        int start = edges[i][0];
        int end = edges[i][1];
        SDL_RenderDrawLine(renderer, 
                          transformedCubeTo2d[start][0], transformedCubeTo2d[start][1],
                          transformedCubeTo2d[end][0], transformedCubeTo2d[end][1]);
    }
    
    SDL_RenderPresent(renderer);
}

int main(void){
    edMatrix(MatrixCoord);
    
    if(SDL_Init(SDL_INIT_VIDEO) != 0){
        printf("Could not initialize SDL");
        return 1;
    }
    
    SDL_Window *window = SDL_CreateWindow("3D Cube", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
                             SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL){
        printf("Could not create window");
        SDL_Quit();
        return 1;
    }
    
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
    if(renderer == NULL){
        printf("Could not create renderer");
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    
    SDL_Event e;
    int running = 1;
    
    printf("=====Controls=====\n"
           "w - up\n"
           "s - down\n"
           "a - left\n"
           "d - right\n"
           "z - zoom in\n"
           "x - zoom out\n"
           "i/o - rotate Y\n"
           "j/k - rotate X\n"
           "n/m - rotate Z\n"
           "r - reset\n"
           "esc - exit\n");
    
    while (running){
        while (SDL_PollEvent(&e)){
            switch (e.type){
                case SDL_QUIT:
                    running = 0;
                    break;
                case SDL_KEYDOWN:
                    if (e.key.keysym.sym == SDLK_i) rotateYMatrix(-0.02);
                    if (e.key.keysym.sym == SDLK_o) rotateYMatrix(0.02);
                    if (e.key.keysym.sym == SDLK_j) rotateXMatrix(-0.02);
                    if (e.key.keysym.sym == SDLK_k) rotateXMatrix(0.02);
                    if (e.key.keysym.sym == SDLK_n) rotateZMatrix(-0.02);
                    if (e.key.keysym.sym == SDLK_m) rotateZMatrix(0.02);
                    if (e.key.keysym.sym == SDLK_w) translateMatrix(0, -0.05, 0);
                    if (e.key.keysym.sym == SDLK_s) translateMatrix(0, 0.05, 0);
                    if (e.key.keysym.sym == SDLK_a) translateMatrix(-0.05, 0, 0);
                    if (e.key.keysym.sym == SDLK_d) translateMatrix(0.05, 0, 0);
                    if (e.key.keysym.sym == SDLK_z) scaleMatrix(1.1);
                    if (e.key.keysym.sym == SDLK_x) scaleMatrix(0.9);
                    if (e.key.keysym.sym == SDLK_r) edMatrix(MatrixCoord);
                    if (e.key.keysym.sym == SDLK_ESCAPE) running = 0;
                    break;
                default:
                    break;
            }
        }
        drawCube(renderer);
    }
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
