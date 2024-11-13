#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifdef _WIN32
    #include <conio.h>  // Para Windows
    #define CLEAR "cls"
#else
    #include <termios.h>
    #include <unistd.h>
    #define CLEAR "clear"

    int _getch(void) {
        struct termios oldt, newt;
        int ch;
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        ch = getchar();
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        return ch;
    }
#endif

// Definições do tabuleiro
#define WIDTH 20
#define HEIGHT 20
#define MAX_TAIL_LENGTH 100
#define MAX_OBSTACLES 20

// Direções
enum Direction { STOP = 0, LEFT, RIGHT, UP, DOWN };
enum Direction dir;

// Variáveis do jogo
int x, y, fruitX, fruitY, score;
int tailX[MAX_TAIL_LENGTH], tailY[MAX_TAIL_LENGTH];
int nTail;  // Tamanho da cauda
int gameOver;
int level = 1;  // Nível inicial

// Obstáculos (as coordenadas dos obstáculos)
int obstacles[MAX_OBSTACLES][2]; // Até MAX_OBSTACLES obstáculos

// Função para inicializar o jogo
void Setup() {
    gameOver = 0;
    dir = STOP;
    x = WIDTH / 2;
    y = HEIGHT / 2;
    fruitX = rand() % WIDTH;
    fruitY = rand() % HEIGHT;
    score = 0;
    nTail = 0;
    
    // Inicializando obstáculos aleatórios
    for (int i = 0; i < level * 2; i++) {  // Aumenta o número de obstáculos a cada nível
        obstacles[i][0] = rand() % WIDTH;
        obstacles[i][1] = rand() % HEIGHT;
    }
}

// Função para desenhar o tabuleiro
void Draw() {
    system(CLEAR);
    
    // Mostrar bordas
    for (int i = 0; i < WIDTH + 2; i++) printf("#");
    printf("\n");

    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if (j == 0) printf("#");
            
            // Mostrar a cobra
            if (i == y && j == x)
                printf("O");  // Cabeça da cobra
            // Mostrar a fruta
            else if (i == fruitY && j == fruitX)
                printf("F");  // Fruta
            else {
                int print = 0;
                // Mostrar a cauda da cobra
                for (int k = 0; k < nTail; k++) {
                    if (tailX[k] == j && tailY[k] == i) {
                        printf("o");
                        print = 1;
                    }
                }
                if (!print) {
                    // Mostrar obstáculos
                    int isObstacle = 0;
                    for (int k = 0; k < level * 2; k++) {  // Aumenta o número de obstáculos a cada fase
                        if (obstacles[k][0] == j && obstacles[k][1] == i) {
                            printf("X");
                            isObstacle = 1;
                            break;
                        }
                    }
                    if (!isObstacle) printf(" "); // Espaço vazio
                }
            }
            if (j == WIDTH - 1) printf("#");
        }
        printf("\n");
    }

    // Mostrar bordas
    for (int i = 0; i < WIDTH + 2; i++) printf("#");
    printf("\n");
    
    // Mostrar a pontuação e fase
    printf("Score: %d | Fase: %d\n", score, level);
}

// Função para capturar entrada do jogador
void Input() {
    if (_kbhit()) {
        switch (_getch()) {
            case 'a':
                dir = LEFT;
                break;
            case 'd':
                dir = RIGHT;
                break;
            case 'w':
                dir = UP;
                break;
            case 's':
                dir = DOWN;
                break;
            case 'x':
                gameOver = 1;
                break;
        }
    }
}

// Função para a lógica do jogo
void Logic() {
    int prevX = tailX[0];
    int prevY = tailY[0];
    int prev2X, prev2Y;
    tailX[0] = x;
    tailY[0] = y;
    for (int i = 1; i < nTail; i++) {
        prev2X = tailX[i];
        prev2Y = tailY[i];
        tailX[i] = prevX;
        tailY[i] = prevY;
        prevX = prev2X;
        prevY = prev2Y;
    }
    switch (dir) {
        case LEFT:
            x--;
            break;
        case RIGHT:
            x++;
            break;
        case UP:
            y--;
            break;
        case DOWN:
            y++;
            break;
        default:
            break;
    }

    // Verificar colisões com a borda
    if (x >= WIDTH) x = 0; else if (x < 0) x = WIDTH - 1;
    if (y >= HEIGHT) y = 0; else if (y < 0) y = HEIGHT - 1;

    // Verificar colisão com a própria cauda
    for (int i = 0; i < nTail; i++) {
        if (tailX[i] == x && tailY[i] == y) gameOver = 1;
    }

    // Verificar colisão com os obstáculos
    for (int i = 0; i < level * 2; i++) {  // Aumenta o número de obstáculos a cada fase
        if (obstacles[i][0] == x && obstacles[i][1] == y) gameOver = 1;
    }

    // Verificar se pegou a fruta
    if (x == fruitX && y == fruitY) {
        score += 10;
        fruitX = rand() % WIDTH;
        fruitY = rand() % HEIGHT;
        nTail++;
        
        // Aumentar a fase a cada 50 pontos
        if (score >= 50 * level) {
            level++;
            // Aumentar a quantidade de obstáculos e reposicioná-los
            Setup();
            
            // Aumentar a dificuldade (exemplo: diminui o tempo de pausa)
            usleep(100000 - (level * 20000)); // Diminui a pausa com o aumento da fase
        }
    }
}

// Função principal
int main() {
    srand(time(0));
    Setup();
    while (!gameOver) {
        Draw();
        Input();
        Logic();
        usleep(200000);  // Pausa entre os quadros
    }
    return 0;
}
