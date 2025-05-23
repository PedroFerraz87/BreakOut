#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "keyboard.h"
#include "screen.h"
#include "timer.h"

#define COLUNA 52
#define LINHA 20

typedef struct Cord {
    int x;
    int y;
} Cord;

// Lista encadeada de pontuações
typedef struct Node {
    int score;
    struct Node* next;
} Node;

int ballPosition = 0;

// Protótipos
void telaInicio(Node* head);
void DesenhaMapa(char **mapa);
void moveBarraA(int *x);
void moveBarraD(int *x);
void moveBola(Cord *bola, int barra, Cord*dir, int *pontos, int *vidas, char **mapa);
Node* carregarScores();
void liberarLista(Node* head);

int main() {
    int offsetX = (MAXX - COLUNA) / 2;
    char **mapa, ch;
    int i;
    int vidas = 3;
    int pontos = 0;

    struct timeval start;
    gettimeofday(&start, NULL);
    srand(start.tv_usec);

    Cord *bola = (Cord*)malloc(sizeof(Cord));
    Cord *dir = (Cord*)malloc(sizeof(Cord));
    bola->x = offsetX + 26; 
    bola->y = 19;
    dir->x = 0;
    dir->y = 0;

    int barra = offsetX + 23;

    screenInit(1);
    
    Node* listaScores = carregarScores();
    telaInicio(listaScores);
    liberarLista(listaScores);
    
    screenClear();

    mapa = (char **)calloc(LINHA, sizeof(char*)); 
    for (i = 0; i < LINHA; i++) {
        mapa[i] = (char *)calloc(COLUNA + 1, sizeof(char));
    }

    char mapa_init[LINHA][COLUNA + 1] = {
        "                                                   ",
        "=== === === === === === === === === === === === ===",
        "=== === === === === === === === === === === === ===",
        "=== === === === === === === === === === === === ===",
        "=== === === === === === === === === === === === ===",
        "=== === === === === === === === === === === === ===",
        "=== === === === === === === === === === === === ===",
        "=== === === === === === === === === === === === ===",
        "=== === === === === === === === === === === === ===",
        "=== === === === === === === === === === === === ===",
        "=== === === === === === === === === === === === ===",
        "=== === === === === === === === === === === === ===",
        "=== === === === === === === === === === === === ===",
        "                                                   ",
        "                                                   ",
        "                                                   ",
        "                                                   ",
        "                      -------                      ",
        "                                                   "};

    for (i = 0; i < LINHA; i++) {
        strcpy(mapa[i], mapa_init[i]);
    }
    DesenhaMapa(mapa);
    keyboardInit();
    timerInit(200);
    screenGotoxy(offsetX, 21);
    while (1) {
        if (keyhit()) {
            ch = readch();
            if (ch == 27) {
                FILE *score;
                score = fopen("score.txt", "a");
                if (score == NULL) {
                    printf("Error opening file for writing!\n");
                    return -1;
                }
                fprintf(score, "%d\n", pontos);
                fclose(score);
                screenGotoxy(offsetX, 22);
                screenUpdate();
                break;
            } else if (ch == 10) {
                while (1) {
                    screenGotoxy(LINHA + 3, 3);
                    printf("Pressione ENTER para despausar");
                    screenUpdate();
                    ch = readch();
                    if (ch == 10) {
                        screenGotoxy(LINHA + 3, 3);
                        printf("                              ");
                        screenUpdate();
                        break;
                    }
                }
            } else if (ch == 97) {
                if (barra - 2 > offsetX) {
                    moveBarraA(&barra);
                }
            } else if (ch == 100) {
                if (barra + 8 < MAXX - offsetX) {
                    moveBarraD(&barra);
                }
            }
        }
        if (timerTimeOver()) {
            timerUpdateTimer(200);
            moveBola(bola, barra, dir, &pontos, &vidas, mapa);

            screenGotoxy(offsetX + 1, 3);
            screenSetColor(RED, BLACK);
            printf("%d", vidas);

            screenGotoxy(MAXX - offsetX - 4, 3);
            screenSetColor(YELLOW, BLACK);
            printf("%d", pontos);

            if (vidas == 0) {
                FILE *score;
                score = fopen("score.txt", "a");
                fprintf(score, "%d\n", pontos);
                fclose(score);

                screenGotoxy(LINHA + 30, 3);
                printf("Score final:");

                screenGotoxy(offsetX, 22);
                screenUpdate();
                break;
            }
        }
    }
    timerDestroy();
    keyboardDestroy();

    return 0;
}

// Tela de início mostrando instruções e pontuações anteriores
void telaInicio(Node* head) {
    screenClear();
    int offsetX = (MAXX) / 2;
    int offsetY = (MAXY - 6) / 2;

    screenGotoxy(offsetX, offsetY - 1);
    printf("BreakOut");

    screenGotoxy(offsetX - 2, offsetY);
    printf("Instruções:\n");

    screenGotoxy(offsetX - 20, offsetY + 2);
    printf(" - Use as teclas A e D para mover a plataforma");
    screenGotoxy(offsetX - 20, offsetY + 3);
    printf(" - Pressione ENTER para começar o jogo");
    screenGotoxy(offsetX - 20, offsetY + 4);
    printf(" - Quebre os blocos com a bola");
    screenGotoxy(offsetX - 20, offsetY + 5);
    printf(" - 2 poderes podem apareçer (1- mais vidas, 2-multiplicador de pontos)");
    screenGotoxy(offsetX - 20, offsetY + 6);
    printf(" - Para sair no meio do jogo, pressione ESC, para pausar pressione ENTER");
    screenGotoxy(offsetX, offsetY + 8);
    printf("Boa sorte!");

    // Mostrar pontuações anteriores
    screenGotoxy(offsetX - 10, offsetY + 10);
    printf("Pontuações anteriores:");
    int linha = offsetY + 11;
    Node* atual = head;
    while (atual != NULL) {
        screenGotoxy(offsetX - 10, linha++);
        printf("- %d", atual->score);
        atual = atual->next;
    }

    getchar();
}

// Lista encadeada para carregar scores
Node* carregarScores() {
    FILE *file = fopen("score.txt", "r");
    if (!file) {
        return NULL;
    }
    Node* head = NULL;
    Node* tail = NULL;
    int value;
    while (fscanf(file, "%d", &value) != EOF) {
        Node* novo = (Node*)malloc(sizeof(Node));
        novo->score = value;
        novo->next = NULL;
        if (head == NULL) {
            head = novo;
            tail = novo;
        } else {
            tail->next = novo;
            tail = novo;
        }
    }
    fclose(file);
    return head;
}

void liberarLista(Node* head) {
    Node* tmp;
    while (head != NULL) {
        tmp = head;
        head = head->next;
        free(tmp);
    }
}

