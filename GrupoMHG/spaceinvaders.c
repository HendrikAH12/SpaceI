/*
    Grupo MHG
    Integrantes:
        - Mateus Carmo de Oliveira (11911BCC026)
        - Hendrik Abdalla Hermann (11911BCC034)
        - Gabriel Joshua Calixto Naves dos Santos (11911BCC052)
*/

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
    int morteTimer;
    struct Point pos;
};

struct TAlien {
    bool vivo;
    int tipo, morteTimer;
    struct Point pos;
};

struct Tiro {
    bool ativo;
    struct Point pos;
};

GLuint charSprites[6];
GLuint morteSprites[3];
GLuint spritesUI[5];
GLuint spritesNums[10];

static void desenhaSprite(float x, float y, float tamanho, GLuint tex);
static void desenhaSpriteJogador(float x, float y, float tamanho, GLuint tex);
static GLuint carregaArqTextura(char *str);

//Setup das texturas
void carregarTexturas() {
    char str[50] = ".//Sprites//nave.png";
    charSprites[0] = carregaArqTextura(str);

    int i;

    for(i = 1; i <= 5; i++) {
        sprintf(str, ".//Sprites//inimigo%d.png", i);
        charSprites[i] = carregaArqTextura(str);
    }

    for(i = 0; i < 3; i++) {
        sprintf(str, ".//Sprites//morte%d.png", i+1);
        morteSprites[i] = carregaArqTextura(str);
    }

    spritesUI[0] = carregaArqTextura(".//Sprites//Overlay.png");
    spritesUI[1] = carregaArqTextura(".//Sprites//Score//score.png");
    spritesUI[2] = carregaArqTextura(".//Sprites//start.png");
    spritesUI[3] = carregaArqTextura(".//Sprites//win.png");
    spritesUI[4] = carregaArqTextura(".//Sprites//gameover.png");

    for(i = 0; i < 10; i++) {
        sprintf(str, ".//Sprites//Score//sprite%d.png", i);
        spritesNums[i] = carregaArqTextura(str);
    }
}

static GLuint carregaArqTextura(char *str){
    // http://www.lonesock.net/soil.html
    GLuint tex = SOIL_load_OGL_texture
        (
            str,
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

void desenhaSpriteJogador(float x, float y, float tamanho, GLuint tex){

    glPushMatrix();

    glColor3f(0.0, 1.0, 0.0);
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

//========================================================================================================
//======== Nave ==========================================================================================
//========================================================================================================

// Aloca espaço da memória para uma nave, e associa os valores dados à ela.
Nave* nave_create(float _x, float _y) {
    Nave* _nave = malloc(sizeof(Nave));
    if(_nave != NULL) {
        _nave->vivo = true;
        _nave->pos.x = _x;
        _nave->pos.y = _y;
        _nave->morteTimer = 60;
    }
    return _nave;
}

void mover_nave(Nave *_nave, bool setaDireita, bool setaEsquerda, float vel_movimento) {

    /*
    Controle da posição:
    - Em cada frame, checar se o personagem está dentro da borda (quadrado de border x border de tamanho)
        - Se sim, analisar a variável direcao_movimento (controlada pelas setas)
            - Se a variável for igual a 1, mover para a direita em vel_movimento unidades por frame.
            - Se for igual a -1, mover para a esquerda em vel_movimento unidades por frame.
            - Se nem um nem outro, fazer nada.
    - Depois, garantir que o personagem esteja dentro das bordas.
    */

    if(_nave->pos.x >= -BORDAX - OFFSET && _nave->pos.x <= BORDAX - OFFSET && _nave->vivo) {
        if(setaDireita) {
            _nave->pos.x += vel_movimento;
        }
        if(setaEsquerda) {
            _nave->pos.x -= vel_movimento;
        }
    }

    // Manter o personagem dentro das borders
    if(_nave->pos.x < -BORDAX - OFFSET)
        _nave->pos.x = -BORDAX + 0.001 - OFFSET;
    if(_nave->pos.x > BORDAX - OFFSET)
        _nave->pos.x = BORDAX - 0.001 - OFFSET;
}

void desenhaNave(Nave *_nave) {

    if(_nave != NULL) {

        float posX = _nave->pos.x;
        float posY = _nave->pos.y;

        if(_nave->vivo) {

            /*
                        ---------------
                        |             |       X é o centro (marcado pelo Point pos da nave.
                        |             |       A caixa do sprite tem um lado = 2 * tamanho.
                      - |      X      |
                  tam | |             |
                      | |             |
                      - ---------------
                                |-----|
                                   tam
            */
            desenhaSpriteJogador(posX, posY, TAMANHO, charSprites[0]);
        }
        else if(_nave->morteTimer != 0) {

            if(_nave->morteTimer > 40 && _nave->morteTimer <= 60)
                desenhaSpriteJogador(posX, posY, TAMANHO, morteSprites[0]);

            if(_nave->morteTimer > 20 && _nave->morteTimer <= 40)
                desenhaSpriteJogador(posX, posY, TAMANHO, morteSprites[1]);

            if(_nave->morteTimer > 0 && _nave->morteTimer <= 20)
                desenhaSpriteJogador(posX, posY, TAMANHO, morteSprites[2]);

            _nave->morteTimer -= 1;
        }
    }
}

void nave_atira(Nave *_nave, Tiro *_tiro) {
    _tiro->pos.x = _nave->pos.x;
    _tiro->pos.y = _nave->pos.y;
    _tiro->ativo = true;
}

void nave_set_estado(Nave *_nave, bool estado) {
    _nave->vivo = estado;
}

void nave_destroy(Nave *_nave) {
    free(_nave);
}

void get_pos_nave(Nave *_nave) {
    printf("X: %f  /  Y: %f\n", _nave->pos.x, _nave->pos.y);
}

void set_pos_nave(Nave *_nave, float posX, float posY) {
    _nave->pos.x = posX;
    _nave->pos.y = posY;
}

//========================================================================================================
//========= Alien ========================================================================================
//========================================================================================================

Alien* alien_create(float _x, float _y, int alienTipo) {
    Alien* _alien = malloc(sizeof(Alien));
    if(_alien != NULL) {
        _alien->vivo = true;
        _alien->pos.x = _x;
        _alien->pos.y = _y;
        _alien->tipo = alienTipo;
        _alien->morteTimer = 20;
    }
    return _alien;
}

void alien_destroy(Alien *_alien) {
    free(_alien);
}

bool alien_vivo(Alien *_alien) {
    return _alien->vivo;
}

int get_nave_morteTimer(Nave *_nave) {
    return _nave->morteTimer;
}

void desenhaAlien(Alien *_alien) {

    if(_alien != NULL) {

        float posX = _alien->pos.x;
        float posY = _alien->pos.y;

        if(_alien->vivo) {

            int alienTipo = _alien->tipo;

            /*
                        ---------------
                        |             |       X é o centro (marcado pelo Point pos da nave.
                        |             |       A caixa do sprite tem um lado = 2 * tamanho.
                      - |      X      |
                  tam | |             |
                      | |             |
                      - ---------------
                                |-----|
                                   tam
            */
            desenhaSprite(posX, posY, TAMANHO, charSprites[alienTipo]);
        }
        else if(_alien->morteTimer != 0) {

            if(_alien->morteTimer > 14 && _alien->morteTimer <= 20)
                desenhaSprite(posX, posY, TAMANHO, morteSprites[0]);

            if(_alien->morteTimer > 7 && _alien->morteTimer <= 14)
                desenhaSprite(posX, posY, TAMANHO, morteSprites[1]);

            if(_alien->morteTimer > 0 && _alien->morteTimer <= 7)
                desenhaSprite(posX, posY, TAMANHO, morteSprites[2]);

            _alien->morteTimer -= 1;
        }
    }
}

/*
    - Move o alien na direção inserida, se ele estiver no intervalo determinado por borda.
    - Se ele sair do intervalo, voltar para a posição mais extrema (BORDAX ou -BORDAX, levando em conta o OFFSET)
    - Fazer isso a cada 60 ticks.
*/
void mover_alien(Alien *_alien, int direcao, float velocidade) {

    if(direcao > 0) {
        if(_alien->pos.x < BORDAX - OFFSET)
            _alien->pos.x += velocidade;
        else
            _alien->pos.x = BORDAX - OFFSET;

    }
    else {

        if(_alien->pos.x > -BORDAX - OFFSET)
            _alien->pos.x -= velocidade;
        else
            _alien->pos.x = -BORDAX - OFFSET;
    }
}

void mover_alien_especial(Alien *_alien) {

    if(_alien->pos.x >= -BORDAX-OFFSET && _alien->pos.x <= BORDAX-OFFSET) {
        _alien->pos.x += 0.005;
    } else {
        _alien->pos.x += 1.2;
        _alien->pos.y += 1.2;
        alien_set_estado(_alien, false);
    }

}

float get_pos_alienX(Alien *_alien) {
    return _alien->pos.x;
}

float get_pos_alienY(Alien *_alien) {
    return _alien->pos.y;
}
void set_pos_alien(Alien *_alien, float _x, float _y) {
    _alien->pos.x = _x;
    _alien->pos.y = _y;
}

void descer_alien(Alien *_alien) {
    _alien->pos.y -= 0.1f;
}

void alien_set_estado(Alien *_alien, bool estado) {
    if(_alien->vivo == false && estado == true) {
        _alien->morteTimer = 20;
    }
    _alien->vivo = estado;
}

//========================================================================================================
//=========== Tiro =========================================================================================
//========================================================================================================


// Cria uma instância de tiro.
Tiro* instanciar_tiro(float posX, float posY) {
    Tiro* tiro = malloc(sizeof(Tiro));
    if(tiro != NULL) {
        tiro->pos.x = posX;
        tiro->pos.y = posY;
        tiro->ativo = false;
    }
    return tiro;
}

void tiro_destroy(Tiro *tiro) {
    free(tiro);
}

bool tiro_ativo(Tiro *_tiro) {
    return _tiro->ativo;
}

void desenhaTiro(Tiro *_tiro) {

    if(_tiro != NULL) {
        float posX = _tiro->pos.x;
        float posY = _tiro->pos.y;

        glBegin(GL_QUADS);

        glColor3f(0.0f, 1.0f, 0.0f); glVertex2f(posX + 0.0045, posY + 0.025);
        glColor3f(0.0f, 1.0f, 0.0f); glVertex2f(posX + 0.0045, posY - 0.025);
        glColor3f(0.0f, 1.0f, 0.0f); glVertex2f(posX - 0.0045, posY - 0.025);
        glColor3f(0.0f, 1.0f, 0.0f); glVertex2f(posX - 0.0045, posY + 0.025);

        glEnd();
    }
}

// Armazena o tiro fora da tela para uso posterior (evita a necessidade de criar novos tiros)
void guardar_tiro(Tiro *_tiro) {
    _tiro->pos.x = 1;
    _tiro->pos.y = 1;
    _tiro->ativo = false;
}

void mover_tiro(Tiro *_tiro) {

    _tiro->pos.y += VELOCIDADETIRO;

    if(_tiro->pos.y + 0.04 >= BORDAY || _tiro->pos.y <= -BORDAY) {
        guardar_tiro(_tiro);
    }

}

/*
    Analisa a posição das bordas dos alienígenas, criando um intervalo em x e em y.
    Depois disso, compara a parte superior e inferior do tiro para analisar se o tiro
    colidiu com o alien.
    Se sim, mata o alien, aumenta a score e guarda o tiro.
*/
void detectar_colisao_alien(Alien *_alien, Tiro *_tiro, int *score) {

    float limiteAlienCima = _alien->pos.y + TAMANHO;
    float limiteAlienBaixo = _alien->pos.y - TAMANHO;
    float limiteAlienEsquerda = _alien->pos.x - TAMANHO;
    float limiteAlienDireita = _alien->pos.x + TAMANHO;

    if(_tiro->pos.x >= limiteAlienEsquerda && _tiro->pos.x <= limiteAlienDireita) {

        if(_tiro->pos.y + 0.03 <= limiteAlienCima && _tiro->pos.y + 0.03 >= limiteAlienBaixo && _alien->vivo) {
            alien_set_estado(_alien, false);
            if(_alien->tipo == 4 || _alien->tipo == 5)
                *score += 1000;
            else
                *score += _alien->tipo * 10;
            guardar_tiro(_tiro);
        }

        if(_tiro->pos.y - 0.03 <= limiteAlienCima && _tiro->pos.y - 0.03 >= limiteAlienBaixo && _alien->vivo) {
            alien_set_estado(_alien, false);
            if(_alien->tipo == 4 || _alien->tipo == 5)
                *score += 1000;
            else
                *score += _alien->tipo * 10;
            guardar_tiro(_tiro);
        }
    }
}

//========================================================================================================
//============== UI ========================================================================================
//========================================================================================================

void desenhaOverlay() {
    desenhaSprite(0, 0, 1, spritesUI[0]);
}

void desenhaScore(int score, int numDigitos, bool segredo) {
    float posX = -OFFSET-0.1, posY = 0.8;
    if(segredo)
        desenhaSpriteJogador(posX, posY, 0.1, spritesUI[1]);
    else
        desenhaSprite(posX, posY, 0.1, spritesUI[1]);

    char scoreStr[15] = "";
    sprintf(scoreStr, "%d", score);

    int i;
    float offsetDigito = 0.15, tamanhoNums = 0.025;
    for(i = 0; i < numDigitos; i++) {
        switch (scoreStr[i]) {
            case '0':
                desenhaSprite(posX + offsetDigito, posY, tamanhoNums, spritesNums[0]);
                break;
            case '1':
                desenhaSprite(posX + offsetDigito, posY, tamanhoNums, spritesNums[1]);
                break;
            case '2':
                desenhaSprite(posX + offsetDigito, posY, tamanhoNums, spritesNums[2]);
                break;
            case '3':
                desenhaSprite(posX + offsetDigito, posY, tamanhoNums, spritesNums[3]);
                break;
            case '4':
                desenhaSprite(posX + offsetDigito, posY, tamanhoNums, spritesNums[4]);
                break;
            case '5':
                desenhaSprite(posX + offsetDigito, posY, tamanhoNums, spritesNums[5]);
                break;
            case '6':
                desenhaSprite(posX + offsetDigito, posY, tamanhoNums, spritesNums[6]);
                break;
            case '7':
                desenhaSprite(posX + offsetDigito, posY, tamanhoNums, spritesNums[7]);
                break;
            case '8':
                desenhaSprite(posX + offsetDigito, posY, tamanhoNums, spritesNums[8]);
                break;
            case '9':
                desenhaSprite(posX + offsetDigito, posY, tamanhoNums, spritesNums[9]);
                break;
        }
        offsetDigito += 0.042;
    }
}

void desenhaTextos(float posX, float posY, float tam, int idTexto) {
    desenhaSprite(posX, posY, tam, spritesUI[idTexto]);
}
