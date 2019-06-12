
typedef struct Point Point;
typedef struct TNave Nave;
typedef struct Tiro Tiro;
typedef struct TiroInimigo TiroInimigo;

//============ Nave =========================================================================

Nave* nave_create(float _x, float _y, float naveTamanho, int numVidas);
int quantas_vidas(Nave *_nave);
void desenhaNave(Nave *_nave);
void mover_nave(Nave *_nave, int direcao_movimento, float vel_movimento, float limite);
void dano_nave(Nave *_nave);

void get_pos_nave(Nave *_nave);
void set_pos_nave(Nave *_nave, float posX, float posY);

//============================================================================================
