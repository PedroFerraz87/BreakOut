/*
  Para compilar os arquivos
  Primeiramente, certifique-se de que todas as bibliotecas necessárias e o arquivo main.c estejam no mesmo diretório em sua máquina local, conforme disponibilizado no GitHub. Em seguida, execute o comando a seguir:

1. cd break-out
2.  make
3. ./break-out
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "keyboard.h"
#include "screen.h"
#include "timer.h"

#define COLUNA 52
#define LINHA 20

typedef struct Cord{
int x;
int y;
struct Cord *next;
}Cord;

int ballPosition = 0;

void telaInicio();
void DesenhaMapa(char **mapa);
void moveBarraA(int *x);
void moveBarraD(int *x);
void moveBola(Cord *bola, int barra, Cord*dir, int *pontos, int *vidas, char **mapa);

int main() {
  int offsetX = (MAXX - COLUNA) / 2;
  char **mapa, ch;
  int i;
  int  vidas = 3;
  int pontos = 0;
  
  Cord *bola = (Cord*)malloc(sizeof(Cord));
  Cord *dir = (Cord*)malloc(sizeof(Cord));
  bola->x = offsetX + 26; 
  bola->y = 19;
  dir->x = 0;
  dir->y = 0;

int barra = offsetX + 23;

  screenInit(1);
  telaInicio();
  screenClear();

  mapa = (char **)calloc(LINHA, sizeof(char*)); // Alocando dinamicamente a matriz do labirinto
  for (i = 0; i < LINHA; i++) {
      mapa[i] = (char *)calloc(COLUNA + 1, sizeof(char));
  }

  char mapa_init[LINHA][COLUNA + 1] = { // interface dos blocos a serem quebrados e a plataforma
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
  while (1){ // while true
      if (keyhit()){
          ch = readch();
        if (ch == 27){
            FILE *score;
            score = fopen("score.txt", "w");
            if(score == NULL) {
                printf("Error opening file for writing!\n");
                return -1;
            }
            fprintf(score, "%d", pontos);
            fclose(score);
            screenGotoxy(offsetX, 22);
            screenUpdate();
            break;
        } else if (ch == 10){
          while(1){
            screenGotoxy(LINHA+3,3);
            printf("Pressione ENTER para despausar");
            screenUpdate();
            ch = readch();
            if (ch == 10){
              screenGotoxy(LINHA+3,3);
              printf("                              ");
              screenUpdate();
              break;
            }
          }
        } else if (ch ==97){
          if (barra-2>offsetX){
            moveBarraA(&barra);
          }
        } else if (ch ==100){
          if (barra+8<MAXX-offsetX){
          moveBarraD(&barra);
          }
        }
      }
      if (timerTimeOver()){ 
        timerUpdateTimer(200);
        moveBola(bola, barra, dir, &pontos, &vidas, mapa);

        screenGotoxy(offsetX+1,3);
        screenSetColor(RED, BLACK);
        printf("%d",vidas);

        screenGotoxy(MAXX-offsetX-4,3);
        screenSetColor(YELLOW, BLACK);
        printf("%d",pontos);

        if (vidas == 0){
          FILE *score;
          score = fopen("score.txt", "a");
          fseek(score, 0, SEEK_SET);
          fprintf(score, "%d\n", pontos);
          fclose(score);
          
          screenGotoxy(LINHA+30,3);
          printf("Score final:");
          
          screenGotoxy(offsetX,22);
          screenUpdate();
          break;
        }
      }
  }
  timerDestroy();
  keyboardDestroy(); 

  return 0;
}

void telaInicio() {
  screenClear();
  int offsetX = (MAXX) / 2;
  int offsetY = (MAXY-6) / 2;

  screenGotoxy(offsetX, offsetY - 1);
  printf("BreakOut");

  screenGotoxy(offsetX-2, offsetY);
  printf("Instruções:\n");

  screenGotoxy(offsetX-20, offsetY + 2);
  printf(" - Use as teclas A e D para mover a plataforma");
  screenGotoxy(offsetX-20, offsetY + 3);
  printf(" - Pressione ENTER para começar o jogo");
  screenGotoxy(offsetX-20, offsetY + 4);
  printf(" - Quebre os blocos com a bola");
  screenGotoxy(offsetX-20, offsetY + 5);
  printf(" - 2 poderes podem apareçer (1- mais vidas, 2-multiplicador de pontos)\n");
  screenGotoxy(offsetX-20, offsetY + 6);
  printf(" -Para sair no meio do jogo, pressione ESC, para pausar pressione ENTER\n");
  screenGotoxy(offsetX, offsetY + 8);
  printf("Boa sorte!");

  getchar();
}

void DesenhaMapa(char **mapa) {

  screenClear();

  int offsetX = (MAXX - COLUNA) / 2;
  int offsetY = (MAXY - LINHA) / 2;

  for (int y = 0; y < LINHA; y++) {
    screenGotoxy(offsetX + 1, offsetY + y + 1);
    for (int x = 0; x < COLUNA; x++) {
      char ch = mapa[y][x];
      if (ch == '-') {
        screenSetColor(WHITE, WHITE);
      } else if (ch == '=') {
        screenSetColor(WHITE, WHITE);
      } else if (ch == '*') {
        screenSetColor(GREEN, BLACK);
      } else {
        screenSetColor(BLACK, BLACK);
      }
      printf("%c", ch);
    }
  }
  screenUpdate();
}

void moveBarraA(int *x){
  screenSetColor(WHITE, WHITE);
  screenGotoxy((*x)-1, 20);
  printf("-");
  screenGotoxy((*x)+6, 20);
  printf(" ");
  (*x)--;
  screenUpdate();
}

void moveBarraD(int *x){
  screenSetColor(WHITE, WHITE);
  screenGotoxy((*x)+6, 20);
  printf("-");
  screenGotoxy((*x)-1, 20);
  printf(" ");
  (*x)++;
  screenUpdate();
  }

