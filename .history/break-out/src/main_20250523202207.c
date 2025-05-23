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

int ballPosition = 0;

typedef struct Cord{  // Estrutura para coordenadas da bola e direção
int x;
int y;
}Cord;

typedef struct Node { // Estrutura da lista encadeada (contagem de blocos destruídos)
  struct Node* next;
} Node;

// Protótipos das funções
void telaInicio();
void DesenhaMapa(char **mapa);
void moveBarraA(int *x);
void moveBarraD(int *x);
void moveBola(Cord *bola, int barra, Cord*dir, int *pontos, int *vidas, char **mapa, Node **destroyedBlocks);

Node* createNode() { // Cria um novo nó para a lista de blocos destruídos
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->next = NULL;
    return newNode;
}
  
void insertNode(Node** head) { // Insere um novo nó no início da lista
    Node* newNode = createNode();
    newNode->next = *head;
    *head = newNode;
}

void printDestroyedCount(Node* head) { // Conta e exibe o total de blocos destruídos
    int count = 0;
    Node* current = head;
    while (current != NULL) {
        count++;
        current = current->next;
    }
    printf("\nTotal de blocos destruidos: %d\n", count);
}

void freeList(Node* head) { // Libera a memória da lista encadeada
    Node* tmp;
    while (head != NULL) {
        tmp = head;
        head = head->next;
        free(tmp);
    }
}

int main() {
  int offsetX = (MAXX - COLUNA) / 2;
  char **mapa, ch;
  int i;
  int  vidas = 2;
  int pontos = 0;
  Node* destroyedBlocks = NULL; // Inicializa a lista encadeada vazia

  struct timeval start;
  gettimeofday(&start, NULL);
  srand(start.tv_usec);
    // Inicializa a bola e sua direção
  Cord *bola = (Cord*)malloc(sizeof(Cord));
  Cord *dir = (Cord*)malloc(sizeof(Cord));
  bola->x = offsetX + 26; 
  bola->y = 19;
  dir->x = 0;
  dir->y = 0;

  int barra = offsetX + 23; // posição inicial da barra

  screenInit(1);
  telaInicio();
  screenClear();

  mapa = (char **)calloc(LINHA, sizeof(char*));   // Aloca o mapa
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
      strcpy(mapa[i], mapa_init[i]); // Copia o mapa para a estrutura alocada
  }
  DesenhaMapa(mapa);
  keyboardInit();
  timerInit(200);
  screenGotoxy(offsetX, 21);
  while (1){     // Loop principal do jogo
      if (keyhit()){
          ch = readch();
          if (ch == 27){ // ESC: sair do jogo
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
          } else if (ch == 10){ // ENTER: pausa e despausa
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
          } else if (ch == 'a'){  // mover para a esquerda
              if (barra-2>offsetX){
                  moveBarraA(&barra);
              }
          } else if (ch == 'd'){ // mover para a direita
              if (barra+8<MAXX-offsetX){
                  moveBarraD(&barra);
              }
          }
      }
      if (timerTimeOver()){ 
          timerUpdateTimer(200);
          moveBola(bola, barra, dir, &pontos, &vidas, mapa, &destroyedBlocks);
            // Exibe vidas e pontos
          screenGotoxy(offsetX+1,3);
          screenSetColor(RED, BLACK);
          printf("%d",vidas);

          screenGotoxy(MAXX-offsetX-4,3);
          screenSetColor(YELLOW, BLACK);
          printf("%d",pontos);

          if (vidas == 0){ // Fim de jogo
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
  printDestroyedCount(destroyedBlocks);
  freeList(destroyedBlocks);

  return 0;
}

void telaInicio() { // Instruções
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
  printf(" - Vidas extras podem aparecer durante o jogo, portanto, quebre o máximo de blocos possível\n");
  screenGotoxy(offsetX-20, offsetY + 6);
  printf(" -Para sair no meio do jogo, pressione ESC, para pausar pressione ENTER\n");
  screenGotoxy(offsetX, offsetY + 8);
  printf("Boa sorte!");

  getchar();  // Espera o jogador apertar ENTER
}

void DesenhaMapa(char **mapa) {
  screenClear();

  int offsetX = (MAXX - COLUNA) / 2;
  int offsetY = (MAXY - LINHA) / 2;

  for (int y = 0; y < LINHA; y++) {  // Percorre cada linha e coluna do mapa para desenhar na tela
    screenGotoxy(offsetX + 1, offsetY + y + 1);
    for (int x = 0; x < COLUNA; x++) {
      char ch = mapa[y][x];
      if (ch == '-') {   // Define a cor dependendo do caractere
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
  screenUpdate();  // Atualiza a tela com o novo estado do mapa
}

void moveBarraA(int *x){ // Mover plataforma pra esquerda
  screenSetColor(WHITE, WHITE);
  screenGotoxy((*x)-1, 20);
  printf("-");
  screenGotoxy((*x)+6, 20);
  printf(" ");
  (*x)--;
  screenUpdate();
}

void moveBarraD(int *x){ // Mover plataforma pra direita
  screenSetColor(WHITE, WHITE);
  screenGotoxy((*x)+6, 20);
  printf("-");
  screenGotoxy((*x)-1, 20);
  printf(" ");
  (*x)++;
  screenUpdate();
}

void moveBola(Cord *bola, int barra, Cord*dir, int *pontos, int *vidas, char **mapa, Node **destroyedBlocks){
    int offsetX = (MAXX - COLUNA) / 2;
    int offsetY = (MAXY - LINHA) / 2;
    int convx = bola->x - (offsetX + 1); // Converte coordenada da bola para índice no array do mapa
    int convy = bola->y - (offsetY + 1);

    if (bola->y == 19 && (bola->x - barra)<=6 && (bola->x - barra)>=0){  // Verifica colisão com a barra
        dir->y=-1;
        if (barra+3<bola->x){
            dir->x = 1;
        }else if (barra+3>bola->x){
            dir->x = -1;
        }else{
            dir->x = 0;
        }
    }else{ // Verifica colisão com blocos dentro dos limites do mapa
        if (convy >= 0 && convy < LINHA && convx >= 0 && convx < COLUNA) {
            if (mapa[convy][convx] == '=') { // Se bateu em um bloco
                int inicio = convx;
                while (inicio > 0 && mapa[convy][inicio - 1] == '=') {
                    inicio--;
                }

                for (int k = 0; k < 3; k++) { // Remove os 3 caracteres que formam o bloco
                    if ((inicio + k) < COLUNA && mapa[convy][inicio + k] == '=') {
                        mapa[convy][inicio + k] = ' ';
                    }
                }

                screenGotoxy(offsetX + 1 + inicio, offsetY + 1 + convy);  // Atualiza o mapa apagando o bloco destruído
                printf("   ");

                *pontos += 10;
                insertNode(destroyedBlocks);

                int random = rand() % 15;   
                if (random == 1){
                    (*vidas)++;
                }

                dir->y *= -1; // Inverte direção vertical (rebate)
            }
        }

        if (bola->x==offsetX+2){ // Verifica colisões com as paredes laterais
            dir->x = 1;
        }else if (bola->x==MAXX-offsetX-1){
            dir->x = -1;  
        }if (bola->y==4){  // Verifica colisão com o topo
            dir->y = 1;
        }if (bola->y==21){ // Verifica se caiu no fundo
            (*vidas)--;
            dir->y = -1;
        }
    }
    screenGotoxy(bola->x, bola->y);
    printf(" ");
    bola->x += dir->x;   // Atualiza a posição da bola
    bola->y += dir->y;
    screenGotoxy(bola->x, bola->y);
    screenSetColor(GREEN, BLACK);
    printf("*");
    screenUpdate();
}
