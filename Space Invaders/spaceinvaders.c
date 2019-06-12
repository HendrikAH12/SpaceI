#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <windows.h>
#include <gl/gl.h>
#include "spaceinvaders.h"

struct Point {
    float x, y;
};

struct TNave {
    bool vivo;
    int vidas, pontos;
    float tamanho;
    struct Point pos;
};

struct TAlien {
    bool vivo;
    struct Point pos;
};

struct Tiro {
    struct Point pos;
};

struct TiroInimigo {
    struct Point pos;
};

// Aloca espaço da memória para uma nave, e associa os valores dados à ela.
Nave* nave_create(float _x, float _y, float naveTamanho, int numVidas) {
    Nave* _nave = malloc(sizeof(Nave));
    if(_nave != NULL) {
        _nave->vivo = true;
        _nave->pos.x = _x;
        _nave->pos.y = _y;
        _nave->tamanho = naveTamanho;
        _nave->vidas = numVidas;
    }
    return _nave;
}

int quantas_vidas(Nave *_nave) {
    return _nave->vidas;
}

void mover_nave(Nave *_nave, int direcao_movimento, float vel_movimento, float limite) {

    /*
    Controle da posição:
    - Em cada frame, checar se o personagem está dentro da borda (quadrado de border x border de tamanho)
        - Se sim, analisar a variável direcao_movimento (controlada pelas setas)
            - Se a variável for igual a 1, mover para a direita em vel_movimento unidades por frame.
            - Se for igual a -1, mover para a esquerda em vel_movimento unidades por frame.
            - Se nem um nem outro, fazer nada.
    - Depois, garantir que o personagem esteja dentro das bordas.
    */

    if(_nave->pos.x >= -limite && _nave->pos.x <= limite) {
        switch(direcao_movimento) {
            case 1:
                _nave->pos.x += vel_movimento;
                break;
            case -1:
                _nave->pos.x -= vel_movimento;
                break;
            default:
                break;
        }
    }

    // Manter o personagem dentro das borders
    if(_nave->pos.x < -limite)
        _nave->pos.x = -limite;
    if(_nave->pos.x > limite)
        _nave->pos.x = limite;
}

void desenhaNave(Nave *_nave) {

    if(_nave != NULL && _nave->vivo) {

        float posX = _nave->pos.x;
        float posY = _nave->pos.y;
        float tam =  _nave->tamanho;

        /*
                    ---------------
                    |             |       O é o centro (marcado pelo Point pos da nave.
                    |             |       A caixa do sprite tem um lado = 2 * tamanho.
                  - |      O      |
              tam | |             |
                  | |             |
                  - ---------------
                            |-----|
                               tam
        */

        glBegin(GL_QUADS);

            glColor3f(1, 0.0f, 0.0f);   glVertex2f(posX + tam, posY + tam);
            glColor3f(0.0f, 1, 0.0f);   glVertex2f(posX + tam, posY - tam);
            glColor3f(0.0f, 0.0f, 1);   glVertex2f(posX - tam, posY - tam);
            glColor3f(1, 0.0f, 1);   glVertex2f(posX - tam, posY + tam);

        glEnd();

    }
}

void dano_nave(Nave *_nave) {
    _nave->vidas -= 1;

    if(_nave->vidas == 0) {
        _nave->vivo = false;
    }
}

void get_pos_nave(Nave *_nave) {
    printf("X: %f  /  Y: %f\n", _nave->pos.x, _nave->pos.y);
}

void set_pos_nave(Nave *_nave, float posX, float posY) {
    _nave->pos.x = posX;
    _nave->pos.y = posY;
}

void desenhaSprite() {

}


