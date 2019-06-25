#define ALIENX 5
#define ALIENY 7
#define ALIENTIMERDEFAULT 80
#define ALIENMORTE 15
#define COOLDOWN 80
#define TAMANHO 0.045
#define NUMTIROSALIADOS 2
#define NUMTIROSINIMIGOS 10
#define VELOCIDADETIRO 0.015
#define BORDAX 0.6
#define BORDAY 0.8
#define OFFSET 0.25

typedef struct Point Point;
typedef struct TNave Nave;
typedef struct TAlien Alien;
typedef struct Tiro Tiro;

void carregarTexturas();

//============ Nave =======================================================================

Nave* nave_create(float _x, float _y, int numVidas);
int quantas_vidas(Nave *_nave);
void desenhaNave(Nave *_nave);
void mover_nave(Nave *_nave, bool setaDireita, bool setaEsquerda, float vel_movimento);
void dano_nave(Nave *_nave);
void nave_atira(Nave *_nave, Tiro *_tiro);

void get_pos_nave(Nave *_nave);
void set_pos_nave(Nave *_nave, float posX, float posY);

//============ Alien =======================================================================

Alien* alien_create(float _x, float _y, int alienTipo);
bool alien_vivo(Alien *_alien);
void desenhaAlien(Alien *_alien);
void mover_alien(Alien *_alien, int direcao, float velocidade);
float get_pos_alienX(Alien *_alien);
void set_pos_alienX(Alien *_alien, float _x);
void descer_alien(Alien *_alien);

//============ Tiro =======================================================================

Tiro* instanciar_tiro(float posX, float posY, bool _aliado);
bool tiro_ativo(Tiro *_tiro);
void desenhaTiro(Tiro *_tiro);
void mover_tiro(Tiro *_tiro);
void detectar_colisao_alien(Alien *_alien, Tiro *_tiro, int *score);
