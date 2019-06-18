#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <windows.h>
#include <gl/gl.h>
#include "spaceinvaders.h"
#include "SOIL.h"

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
    int offsetTimer;
    float tamanho;
    struct Point pos;
};

struct Tiro {
    struct Point pos;
};

struct TiroInimigo {
    struct Point pos;
};

GLuint textura2d;

static GLuint carregaArqTextura(char *str){
    // http://www.lonesock.net/soil.html
    GLuint tex = SOIL_load_OGL_texture
        (
            ".//Sprites//inimigo.png",
            SOIL_LOAD_AUTO,
            SOIL_CREATE_NEW_ID,
            SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y |
            SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
        );

    /* check for an error during the load process */
    if(0 == tex){
        printf( "SOIL loading error: '%s'\n", SOIL_last_result() );
    }
    return tex;
}

void desenhaSprite(float x, float y, float tamanho, GLuint tex){

    glPushMatrix();

    glColor3f(1.0, 1.0, 1.0);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBegin(GL_QUADS);
        glTexCoord2f(0.0f,1.0f); glVertex2f(x - tamanho, y + tamanho);
        glTexCoord2f(1.0f,1.0f); glVertex2f(x + tamanho, y + tamanho);
        glTexCoord2f(1.0f,0.0f); glVertex2f(x + tamanho, y - tamanho);
        glTexCoord2f(0.0f,0.0f); glVertex2f(x - tamanho, y - tamanho);
    glEnd();

    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);

    glPopMatrix();

}

void createTeste() {
    textura2d = carregaArqTextura("");
}

void desenhaTeste() {
    desenhaSprite(0, 0, 0.2f, textura2d);
}

//===============================================================================

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

    if(_nave->pos.x >= -limite && _nave->pos.x <= limite && _nave->vivo) {
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

//===============================================================================

Alien* alien_create(float _x, float _y, float _tamanho, int offset) {
    Alien* _alien = malloc(sizeof(Alien));
    if(_alien != NULL) {
        _alien->vivo = true;
        _alien->pos.x = _x;
        _alien->pos.y = _y;
        _alien->tamanho = _tamanho;
        _alien->offsetTimer = offset;
    }
    return _alien;
}

bool alien_vivo(Alien *_alien) {
    return _alien->vivo;
}

void desenhaAlien(Alien *_alien) {

    if(_alien != NULL && _alien->vivo) {

        float posX = _alien->pos.x;
        float posY = _alien->pos.y;
        float tam =  _alien->tamanho;

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

/*
    - Move o alien na direção inserida, se ele estiver no intervalo determinado por borda.
    - Se ele sair do intervalo, voltar para a posição mais extrema (borda ou -borda)
    - Fazer isso a cada 60 ticks.
*/
void mover_alien(Alien *_alien, int direcao, float velocidade, float borda, int timer) {
    if(timer - _alien->offsetTimer == 0) {

        if(direcao > 0) {

            if(_alien->pos.x < borda)
                _alien->pos.x += velocidade;
            else
                _alien->pos.x = borda;

        }
        else {

            if(_alien->pos.x > -borda)
                _alien->pos.x -= velocidade;
            else
                _alien->pos.x = -borda;

        }
    }

}

int get_offset(Alien *_alien) {
    return _alien->offsetTimer;
}

float get_pos_alienX(Alien *_alien) {
    return _alien->pos.x;
}

void set_pos_alienX(Alien *_alien, float _x) {
    _alien->pos.x = _x;
}

void descer_alien(Alien *_alien) {
    _alien->pos.y -= 0.1f;
}

void matar_alien(Alien *_alien) {
    _alien->vivo = false;
    free(_alien);
}
