# BreakOut

Projeto de PIF da AV2 feito por um grupo de 3 alunos da turma A do 2° período de ciência da computação da CESAR School.


 ## Executando o Jogo
  Para compilar os arquivos, primeiramente, certifique-se de que todas as bibliotecas necessárias e o arquivo main.c estejam no mesmo diretório em sua máquina local. Em seguida, execute o comando a seguir:

1. git clone https://github.com/PedroFerraz87/breakout.git
2. cd break-out
3. gcc src/keyboard.o src/main.o src/screen.o src/timer.o -o break-out
4. ./break-out
   
## Vídeo do jogo sendo executado

[Link para o Vídeo no YouTube](https://youtu.be/ztfKbOfI62Y?feature=shared)

## Descrição
O objetivo do BreakOut é, usando duas teclas para controlar a plataforma, rebater a bola e não deixar cair e quebrar os blocos na parte superior da tela. Além disso, implementamos um "poder" que surge aleatoriamente adicionando vidas (random).
### Como jogar
1. Pressione Enter para iniciar o jogo.
2. Use as teclas 'A' e 'D' para mover a plataforma.
3. Rebata as bolas para a parte superior da tela, movendo a plataforma.
4. Para quitar, pressione ESC, para pausar, pressione ENTER.
## Membros do grupo
- Pedro Valença Ferraz - pvf@cesar.school
- Caio Cabral da Mata Buonora - ccmb@cesar.school
- Rafael de Lima Cavalcanti Loyo Fernandes - rlclf@cesar.school
