#include <windows.h>
#include <gl/gl.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include "spaceinvaders.h"
#include "SOIL.h"

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);

//===================================================================
bool isGameOver = false;
bool isGamePaused = true;
bool isGameStart = true;
bool isGameWin = false;
int dirAlien = 1; //Variável para a direção dos aliens
bool atirar = false, setaD = false, setaE = false; //Variáveis de reconhecimento de tecla
int alienTimer = ALIENTIMERDEFAULT, timerCount = 1;
int alienEspecialTimer = 1500;
int score = 0; //Variável da score
int cooldownTiroJogador = 0, cooldownTiroAlien = 0, timerFinal = 60;
float vel_jogador = 0.01, vel_alien = 0.04; //Setup das variáveis de velocidade
Nave *nave; //Ponteiro da nave
Alien *aliens[ALIENX][ALIENY]; //Ponteiros dos aliens
Alien *alienEspecial; //Ponteiro do alien especial
Tiro *tiroJogador;
Tiro *tirosAliens[NUMTIROSINIMIGOS];
//===================================================================

void inicializarJogo();
void resetarJogo();
void encerrarJogo();
void desenhaJogo();
void desenharInterfaceGrafica();
void logicaAliens();
void logicaTiros();
void updateTimer();

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
    WNDCLASSEX wcex;
    HWND hwnd;
    HDC hDC;
    HGLRC hRC;
    MSG msg;
    BOOL bQuit = FALSE;

    /* register window class */
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_OWNDC;
    wcex.lpfnWndProc = WindowProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = "GLSample";
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);;


    if (!RegisterClassEx(&wcex))
        return 0;

    /* create main window */
    hwnd = CreateWindowEx(0,
                          "GLSample",
                          "Space Invaders",
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT,
                          CW_USEDEFAULT,
                          1000,
                          1000,
                          NULL,
                          NULL,
                          hInstance,
                          NULL);

    ShowWindow(hwnd, nCmdShow);

    /* enable OpenGL for the window */
    EnableOpenGL(hwnd, &hDC, &hRC);

    /* Inicialização do Jogo */
    inicializarJogo();

    /* program main loop */
    while (!bQuit)
    {
        /* check for messages */
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            /* handle or dispatch messages */
            if (msg.message == WM_QUIT)
            {
                bQuit = TRUE;
            }
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {
            /* OpenGL animation code goes here */

            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            glPushMatrix();

            desenhaOverlay();
            desenharInterfaceGrafica();

            desenhaJogo();

            glPopMatrix();

            SwapBuffers(hDC);

            Sleep (1);
        }
    }

    encerrarJogo();

    /* shutdown OpenGL */
    DisableOpenGL(hwnd, hDC, hRC);

    /* destroy the window explicitly */
    DestroyWindow(hwnd);

    return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_CLOSE:
            PostQuitMessage(0);
        break;

        case WM_DESTROY:
            return 0;

        case WM_KEYDOWN:
        {
            switch (wParam)
            {
                case VK_LEFT:
                    setaE = true;
                    break;

                case VK_RIGHT:
                    setaD = true;
                    break;

                case VK_SPACE:
                    atirar = true;
                    break;

                case 'P':
                    if(!isGameOver) {
                        isGamePaused = !isGamePaused;
                        if(isGameStart)
                            isGameStart = false;
                    }
                    break;

                case 'R':
                {
                    if(isGamePaused || isGameOver || isGameWin)
                        resetarJogo();
                }
                break;

                case VK_ESCAPE:
                    PostQuitMessage(0);
                    break;
            }
        }
        break;

        case WM_KEYUP:
        {
            switch(wParam)
            {
                case VK_LEFT:
                    setaE = false;
                    break;

                case VK_RIGHT:
                    setaD = false;
                    break;

                case VK_SPACE:
                    atirar = false;
                    break;

            }
        }
        break;

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}

void EnableOpenGL(HWND hwnd, HDC* hDC, HGLRC* hRC)
{
    PIXELFORMATDESCRIPTOR pfd;

    int iFormat;

    /* get the device context (DC) */
    *hDC = GetDC(hwnd);

    /* set the pixel format for the DC */
    ZeroMemory(&pfd, sizeof(pfd));

    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW |
                  PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;

    iFormat = ChoosePixelFormat(*hDC, &pfd);

    SetPixelFormat(*hDC, iFormat, &pfd);

    /* create and enable the render context (RC) */
    *hRC = wglCreateContext(*hDC);

    wglMakeCurrent(*hDC, *hRC);
}

void DisableOpenGL (HWND hwnd, HDC hDC, HGLRC hRC)
{
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hRC);
    ReleaseDC(hwnd, hDC);
}

void inicializarJogo() {

    carregarTexturas();

    time_t t;
    srand((unsigned)time(&t)); // Inicializa o random

    nave = nave_create(-0.5, -0.7);

    tiroJogador = instanciar_tiro(0, 0, true);

    int i, j, tipo = 1;
    float posX = -0.45 - OFFSET, posY = 0.6; // Posições iniciais
    for(i = 0; i < ALIENX; i++) {

        if(i != 0 && i % 2 != 0) // Variar o tipo de alien de acordo com a linha
          tipo++;

        for(j = 0; j < ALIENY; j++) {
            aliens[i][j] = alien_create(posX, posY, 4-tipo); // 4 - tipo para garantir os aliens corretos
            posX += 0.13; // Espaçamento em X
        }
        posY -= 0.1; // Espaçamento em Y
        posX = -0.45 - OFFSET;
    }

    alienEspecial = alien_create(1, 1, 4);
    alien_set_estado(alienEspecial, false);
}

void desenhaJogo() {

    //Desenha o personagem jogável
    desenhaNave(nave);

    int i, j;
    for(i = 0; i < ALIENX; i++) {
        for(j = 0; j < ALIENY; j++) {
            desenhaAlien(aliens[i][j]); //Funcao principal de desenho dos aliens
        }
    }

    //Desenha o alien de pontos extras
    desenhaAlien(alienEspecial);

    if(!isGamePaused && !isGameOver) {

        //Coloca o alien de pontos extras e reseta o timer dele aleatoriamente
        if(alienEspecialTimer == 0 && !alien_vivo(alienEspecial)) {
            iniciarAlienEspecial();

            int timerReset = 800 + rand()%1000+1;
            alienEspecialTimer = timerReset;

        } else {
            alienEspecialTimer -= 1;
        }

        //Funcao principal de movimento do personagem jogavel
        mover_nave(nave, setaD, setaE, vel_jogador);

        //Desenha e processa o movimento dos aliens
        logicaAliens();

        //Desenha o tiro do jogador
        if(tiro_ativo(tiroJogador))
            desenhaTiro(tiroJogador);

        //Processa e movimenta o tiro dos aliens e dos aliados
        logicaTiros();

        //Atualiza o timer
        updateTimer();
    }
}

/*
    - Executa um cheque de posicoes (para decidir o movimento e desenhar os aliens vivos)
    - Se os aliens vai sair da borda, descer e inverter a direcao.
    - Se nao, mover os aliens de posicao.
    - Fazer tudo isso analisando o timer dos aliens (alienTimer) e os offsets
*/
void logicaAliens() {

    if(alien_vivo(alienEspecial)) {
        mover_alien_especial(alienEspecial);
    }

    int i, j, aliensMortos = 0;
    int precisaDescer = false;

    // Cheque de posicoes
    for(i = 0; i < ALIENX; i++) {
        for(j = 0; j < ALIENY; j++) {

            if(alien_vivo(aliens[i][j])) {

                //Morte do jogador quando os aliens chegam na terra
                if(get_pos_alienY(aliens[i][j]) - 2*TAMANHO <= -0.7) {
                    nave_set_estado(nave, false);
                    isGameOver = true;
                }

                // Se este if nao estiver aqui, os aliens descem antes que todos cheguem na borda.
                // Além disso, sincroniza para eles descerem na hora que o timer resetar.
                if(alienTimer == 0) {

                    float pos = get_pos_alienX(aliens[i][j]);

                    //Ativar a flag de descida, inverter a direcao e sair do cheque de posicao, se o aliens chegarem na borda.
                    if(pos + TAMANHO > BORDAX - OFFSET && dirAlien == 1)
                    {
                        precisaDescer = true;
                        dirAlien = -1;
                        break;
                    }
                    else if(pos - TAMANHO < -BORDAX - OFFSET && dirAlien == -1)
                    {
                        precisaDescer = true;
                        dirAlien = 1;
                        break;
                    }
                }
            } else {
                aliensMortos++;
            }
        }
        // Sair do cheque
        if(precisaDescer) {
            break;
        }
    }

    // Movimentacao dos aliens
    for(i = 0; i < ALIENX; i++) {
        for(j = 0; j < ALIENY; j++) {

            if(alien_vivo(aliens[i][j])) {

                // Se precisar descer, descer os aliens e resetar o timer (evita desincronismo)
                if(precisaDescer && !isGameOver) {
                    descer_alien(aliens[i][j]);
                    alienTimer = ALIENTIMERDEFAULT;
                }
                else
                {

                    //Se não precisar descer, mover normalmente (já que a direção já foi invertida)
                    //Mover em determinado momento do timer de acordo com a linha
                    if(i == 0 && alienTimer == 60)
                        mover_alien(aliens[i][j], dirAlien, vel_alien);
                    if(i == 1 && alienTimer == 50)
                        mover_alien(aliens[i][j], dirAlien, vel_alien);
                    if(i == 2 && alienTimer == 40)
                        mover_alien(aliens[i][j], dirAlien, vel_alien);
                    if(i == 3 && alienTimer == 30)
                        mover_alien(aliens[i][j], dirAlien, vel_alien);
                    if(i == 4 && alienTimer == 20)
                        mover_alien(aliens[i][j], dirAlien, vel_alien);


                }
            }
        }
    }

    //Aumenta a velocidade com o número de aliens mortos
    if(aliensMortos == 14 && timerCount != 2) {
        timerCount = 2;
    }
    if(aliensMortos == 26 && timerCount != 5) {
        timerCount = 5;
    }
    if(aliensMortos == 34 && timerCount != 10) {
        timerCount = 10;
    }

    if(aliensMortos >= 35) {
        isGameWin = true; //Finaliza o jogo
    }
}

void iniciarAlienEspecial() {
    set_pos_alien(alienEspecial, -BORDAX-OFFSET+TAMANHO, 0.5);
    alien_set_estado(alienEspecial, true);
}

void logicaTiros() {
    if(atirar) {
        if(!tiro_ativo(tiroJogador) && cooldownTiroJogador == 0) {
            nave_atira(nave, tiroJogador);
            cooldownTiroJogador = COOLDOWN;
        }
    }

    int i, j;
    if(tiro_ativo(tiroJogador)) {
        mover_tiro(tiroJogador);
        for (i = 0; i < ALIENX; i++) {
            for (j = 0; j < ALIENY; j++) {
                detectar_colisao_alien(aliens[i][j], tiroJogador, &score);
            }
        }

        detectar_colisao_alien(alienEspecial, tiroJogador, &score);
    }

}

// Timers
void updateTimer() {

    alienTimer -= timerCount;
    if(alienTimer < 0)
        alienTimer = ALIENTIMERDEFAULT; //Reseta o timer

    if(cooldownTiroJogador > 0) {
        cooldownTiroJogador -= 1;
    }
}

void desenharInterfaceGrafica() {

    if(isGameOver && get_nave_morteTimer(nave) == 0) {
        float posX = -OFFSET, posY = 0;
        desenhaTextos(-OFFSET, 0.1, 0.4, 4); //Desenha texto de game over
    }
    if(isGameWin) {
        desenhaTextos(-OFFSET, 0.1, 0.4, 3); //Desenha texto de vitória
    }
    if(isGameStart) {
        desenhaTextos(-OFFSET, 0.05, 0.6, 2); //Desenha texto de início do jogo
    }

    desenhaScore(score, 4); //Desenha 4 digitos de score

}

// Reseta as variáveis globais, reorganiza os aliens e o jogador e os tiros
void resetarJogo() {

    score = 0;
    alienTimer = ALIENTIMERDEFAULT;
    timerCount = 1;
    alienEspecialTimer = 1500;
    dirAlien = 1;
    cooldownTiroJogador = cooldownTiroAlien = 0;

    nave_set_estado(nave, true); //Revive o jogador
    set_pos_nave(nave, -0.5, -0.7); //Reposiciona o jogador

    if(tiro_ativo(tiroJogador)) {
        guardar_tiro(tiroJogador); //Guarda os tiros (caso haja algum na tela)
    }

    int i, j;
    float posX = -0.45 - OFFSET, posY = 0.6; // Posições iniciais
    for(i = 0; i < ALIENX; i++) {

        for(j = 0; j < ALIENY; j++) {
            set_pos_alien(aliens[i][j], posX, posY);
            if(!alien_vivo(aliens[i][j]))
                alien_set_estado(aliens[i][j], true);
            posX += 0.13; // Espaçamento em X
        }
        posY -= 0.1; // Espaçamento em Y
        posX = -0.45 - OFFSET; // Volta para a primeira coluna
    }

    if(isGamePaused)
        isGamePaused = false; //Despausa (caso esteja pausado)

    if(isGameOver)
        isGameOver = false; //Reinicia o jogo (caso o jogador tenha morrido)

    if(isGameWin)
        isGameWin = false; //Reinicia o jogo (caso o jogador tenha ganhado)
}

// Desaloca a memória utilizada para garantir uma finalização boa
void encerrarJogo() {
    nave_destroy(nave);

    int i, j;
    for(i = 0; i < ALIENX; i++) {
        for(j = 0; j < ALIENY; j++) {
            alien_destroy(aliens[i][j]);
        }
    }

    tiro_destroy(tiroJogador);

    for(i = 0; i < NUMTIROSINIMIGOS; i++) {
        tiro_destroy(tirosAliens[i]);
    }
}
