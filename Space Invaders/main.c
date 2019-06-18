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

//========================================================
int dirMovimento = 0, dirAlien = 1; //Vari�vel para o reconhecimento de tecla
int alienTimer = ALIENTIMERDEFAULT;
float borderX = 0.6, borderY = 0.8, vel_jogador = 0.01, vel_alien = 0.04; //Setup do personagem e do mapa jog�vel
Nave *nave; //Ponteiro da nave
Alien *aliens[ALIENX][ALIENY]; //Ponteiros dos aliens
//========================================================

void inicializarJogo();
void desenhaJogo();
void logicaAliens();
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
                          "OpenGL Sample",
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

    /* Inicializa��o do Jogo */
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

            glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            glPushMatrix();

            //Desenha o quadrado da �rea jog�vel
            glBegin(GL_QUADS);

                glColor3f(0.0f, 0.0f, 0.0f); glVertex2f(borderX + TAMANHO, borderY + TAMANHO);
                glColor3f(0.0f, 0.0f, 0.0f); glVertex2f(borderX + TAMANHO, -borderY - TAMANHO);
                glColor3f(0.0f, 0.0f, 0.0f); glVertex2f(-borderX - TAMANHO, -borderY - TAMANHO);
                glColor3f(0.0f, 0.0f, 0.0f); glVertex2f(-borderX - TAMANHO, borderY + TAMANHO);

            glEnd();

            desenhaJogo();

            glPopMatrix();

            SwapBuffers(hDC);
            Sleep (1);
        }
    }

    /* shutdown OpenGL */
    DisableOpenGL(hwnd, hDC, hRC);

    /* destroy the window explicitly */
    DestroyWindow(hwnd);

    free(nave);

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
                    dirMovimento = -1;
                    break;

                case VK_RIGHT:
                    dirMovimento = 1;
                    break;

                case VK_SPACE:
                    //dirAlien *= -1;
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
                    dirMovimento = 0;
                    break;

                case VK_RIGHT:
                    dirMovimento = 0;
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

    nave = nave_create(-0.5, -0.7, 3);

    int i, j , offset = 0, tipo = 1;
    float posX = -0.4, posY = 0.5; // Posições iniciais
    for(i = 0; i < ALIENX; i++) {

        if(i != 0 && i % 2 != 0) // Variar o tipo de alien de acordo com a linha
          tipo++;

        for(j = 0; j < ALIENY; j++) {
            aliens[i][j] = alien_create(posX, posY, offset, 4-tipo); // 4 - tipo para garantir os aliens corretos
            posX += 0.15; // Espacamento em X
        }
        posY -= 0.1; // Espaçamento em Y
        posX = -0.4; // Volta para a primeira coluna
        offset += 10; // Offset de animação
    }
}

void desenhaJogo() {

    //Funcao principal de movimento do personagem jogavel
    mover_nave(nave, dirMovimento, vel_jogador, borderX);

    //Desenha o personagem jogavel
    desenhaNave(nave);

    //Desenha e processa o movimento dos aliens
    logicaAliens();

    //Atualiza o timer
    updateTimer();
}

/*
    - Executa um cheque de posicoes (para decidir o movimento e desenhar os aliens vivos)
    - Se os aliens vai sair da borda, descer e inverter a direcao.
    - Se nao, mover os aliens de posicao.
    - Fazer tudo isso analisando o timer dos aliens (alienTimer) e os offsets
*/
void logicaAliens() {
    int i, j;
    int precisaDescer = false;

    // Cheque de posicoes
    for(i = 0; i < ALIENX; i++) {
        for(j = 0; j < ALIENY; j++) {

            if(alien_vivo(aliens[i][j])) {

                desenhaAlien(aliens[i][j]); //Funcao principal de desenho.

                // Se este if nao estiver aqui, os aliens descem antes que todos cheguem na borda.
                if(alienTimer - get_offset(aliens[i][j]) == 0) {

                    float pos = get_pos_alienX(aliens[i][j]);

                    //Ativar a flag de descida, inverter a direcao e sair do cheque de posicao, se o aliens chegarem na borda.
                    if(pos + 2 * TAMANHO > borderX && dirAlien == 1)
                    {
                        precisaDescer = true;
                        dirAlien = -1;
                        break;
                    }
                    else if(pos - 2 * TAMANHO < -borderX && dirAlien == -1)
                    {
                        precisaDescer = true;
                        dirAlien = 1;
                        break;
                    }

                }
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
                if(precisaDescer) {
                    descer_alien(aliens[i][j]);
                    alienTimer = ALIENTIMERDEFAULT;
                }
                else
                {
                    //Se nao precisar descer, mover normalmente (ja que a direcao ja foi invertida)
                    mover_alien(aliens[i][j], dirAlien, vel_alien, borderX, alienTimer);
                }
            }
        }
    }
}

// Timers
void updateTimer() {
    alienTimer -= 1;
    if(alienTimer < 0)
        alienTimer = ALIENTIMERDEFAULT;
    //printf("%d\n", alienTimer);
}
