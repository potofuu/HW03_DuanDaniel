typedef struct
{
    int x;
    int y;
    int oldx;
    int oldy;
    int velocity;
    int width;
    int height;
    unsigned short color;
} PLAYER;

typedef struct
{
    int x;
    int y;
    int oldx;
    int oldy;
    int velocity;
    unsigned short color;
    int active;
} BULLET;

typedef struct
{
    int x;
    int y;
    int oldx;
    int oldy;
    int velocity;
    unsigned short color;
    int active;
} ENEMIES;

typedef struct
{
    int x;
    int y;
    int oldx;
    int oldy;
    int velocity;
    int veloY;
    unsigned short color;
    int lives;
    int active;
} BOSS;

typedef struct
{
    int x;
    int y;
    unsigned short color;
    int speedup;
    int change;
} SPEED;

typedef struct 
{
    int x;
    int oldx;
    unsigned short color;
} ZONE;


#define ENEMYCOUNT 6


extern PLAYER player;
extern BULLET bullet;
extern ENEMIES enemyOne[ENEMYCOUNT];
extern BOSS boss;
extern int score;


void initGame();
void initExtra();
void initPlayer();
void initBullets();
void initZone();
void updateZone();
void updateGame();
void updatePlayer();
void updateExtra();
void drawExtra();
void drawBoss();
void drawGame();
void drawZone();
void drawPlayer();
void drawBullet();
void updateBullets();
void initEnemies();
void initBoss();
void drawEnemies(ENEMIES* enemy);
void updateEnemies(ENEMIES* enemey);
void updateBoss();