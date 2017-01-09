// External libs
#include "mbed.h"
#include "wave_player.h"
#include "MMA8452.h"

// Projet includes
#include "globals.h"
#include "city_landscape_public.h"
#include "missile_public.h"
#include "player_public.h"
#include "testbench.h"
//#include <math.h>


#define CITY_HIT_MARGIN 1
#define CITY_UPPER_BOUND (SIZE_Y-(LANDSCAPE_HEIGHT+MAX_BUILDING_HEIGHT))

// Helper function declarations
void playSound(char* wav);
void checkCollisions(void);
void clearMissiles(void);
void nextLevel(void);
void getLevelInfo(void);
void checkCityCollisions(void);
void levelSetup(void);
void play(void);
void loadGame(void);
void gameOver(void);


// Console output
Serial pc(USBTX,USBRX);

// Accelerometer
MMA8452 accel(p28, p27, 100000);

// Control buttons
DigitalIn left_pb(p21);
DigitalIn right_pb(p22); 
DigitalIn fire_pb(p23);
DigitalIn pb(p24);

// Screen
uLCD_4DGL uLCD(p9,p10,p11); // serial tx, serial rx, reset pin;

// Speaker
AnalogOut DACout(p18);
PwmOut speaker(p25);
wave_player waver(&DACout);

// SD Card
SDFileSystem sd(p5, p6, p7, p8, "sd"); // mosi, miso, sck, cs

//player
PLAYER thisPlayer;

//LEDs
DigitalOut led1(LED1);
DigitalOut led2(LED2);
DigitalOut led3(LED3);

//Other useful Variables
int numMissilesDestroyed = 0;
int level = 1;
int numCities = 4;
int missileMissileTolerance;
int missilePlayerTolerance;
int isGameOver = 0;
int score = 0;
int numLives = 3;
int highScore = 0;
// ===User implementations start===
int main()
{
        
    // Replace <outfile> with your student ID
    //test_dlinkedlist(
//        "/sd/tests/dll_test.txt",
//        "/sd/tests/dll_test_output.txt"
//    );
    // Test the speaker
    //playSound("/sd/wavfiles/BUZZER.wav");
    
    //Initialize hardware buttons
    left_pb.mode(PullUp); 
    right_pb.mode(PullUp);
    fire_pb.mode(PullUp);
    pb.mode(PullUp);
    
    
    
    // ===User implementations start===
    int waiting = 1;
    while(waiting)
    {
        uLCD.locate(4,0);
        uLCD.printf("Now Playing");
        uLCD.locate(1,1);
        uLCD.printf("Missile Command");
        uLCD.locate(0,3);
        uLCD.printf("Press Up to Begin");
        uLCD.locate(0,5);
        uLCD.printf("Press Down for the");
        uLCD.locate(0,6);
        uLCD.printf("Level Selection");
        if(!fire_pb)
        {
            uLCD.cls();
            waiting = 0;
            level = 1;
            play();
        }
        else if(!pb){
            uLCD.cls();
            levelSetup();
            waiting = 0;
            play();
        }  
    }
    uLCD.locate(5,1);
    uLCD.printf("");
    
    // Initialization goes here
    
    // Initialization goes here
    
    // ===User implementations end===
}

// ===User implementations start===
void levelSetup()
{
       int waiting = 1;
       int lvl = 1;
       while(waiting)
       {
            uLCD.locate(0,0);
            uLCD.printf("Level Selection:");
            uLCD.locate(0,2);
            uLCD.printf("Level: %d", lvl);
            uLCD.locate(0,3);
            uLCD.printf("Value must be set between 1 and 4");
            uLCD.locate(0,5);
            uLCD.printf("Right to Add");
            uLCD.locate(0,6);
            uLCD.printf("Left to Subtract");
            uLCD.locate(0,8);
            uLCD.printf("Press Up to Start");
            if(!left_pb)
            {
                lvl--;
            }
            else if(!right_pb)
                lvl++;
            else if(!fire_pb)
            {
                uLCD.cls();
                waiting = 0;
            }
       }
       level = lvl;
}

void loadGame()
{
    uLCD.cls();
    level = 1;
    getLevelInfo();
    play(); 
}

void play() {
    city_landscape_init(numCities);
    draw_cities();
    draw_landscape();
    player_init();
    thisPlayer = player_get_info();
    missile_init();
    double x;
    double y;
    double z;
    int readXYZ;
    
    int active = accel.activate();
    
    
    

    // Main game loop
    while(!isGameOver)
    {
        //Display to screen level info and number of missiles destroyed. 
        uLCD.locate(0,0);
        uLCD.printf("Level: %d", level);
        uLCD.locate(14,0);
        uLCD.printf("%d", numMissilesDestroyed);
        
        
        //PLAYER player = player_get_info();
        //led1 = !led1;
        getLevelInfo();
        
        // 1. Update missiles
        missile_generator();
        
        // 2. Read input
        readXYZ = accel.readXYZGravity(&x, &y, &z);
        //printf("x: %d y: %d z: %d\n\r", x,y,z);
        if(!fire_pb) {
            player_fire();   
        }
        player_missile_draw();
        // 3. Update player position
        if(x < -0.5) {
            player_moveLeft();   
        }
        else if( x > 0.5) {
            player_moveRight();   
        }
        // 4. Check for collisions
        checkCollisions();
        checkCityCollisions();
        // 5. Redraw city landscape
        draw_cities();
        draw_landscape();
        // 6. Check for endgame
        if(thisPlayer.status == DESTROYED || numCities <= 0)
            isGameOver = 1;
        if((numMissilesDestroyed >= 10 || (!left_pb && !right_pb)) && level < 4)
            nextLevel();
    }
    score = (level*10) + numMissilesDestroyed;
    gameOver();
}

void gameOver() {
    uLCD.cls();
    uLCD.locate(0,0);
    uLCD.printf("Game Over!");
    if(score > highScore) {
        highScore = score;
        uLCD.locate(0,1);
        uLCD.printf("New High Score!");   
        playSound("/sd/wavfiles/NewHighScore.wav");
    }
    
    uLCD.locate(0,2);
    uLCD.printf("Final Score: %d", score);
    uLCD.locate(0,4);
    uLCD.printf("Press [Fire] to \nplay again.");
    
    int playAgain = 0;
    while(!playAgain) {
        if(!fire_pb) {
            playAgain = 1;
        }   
    }
    loadGame();
}

void checkCollisions() {
    int playerX;// = ((PLAYER_MISSILE*) (((thisPlayer.playerMissiles)->head)->data))->x;
    int playerY;// = ((PLAYER_MISSILE*) (((thisPlayer.playerMissiles)->head)->data))->y;
    int missileX;
    int missileY;
    double distance;
    int midX, midY;
    CITY city;
    int cityMinX, cityMaxX, cityMaxY;
    
    //Check missile collisions with other missiles. 
    PLAYER_MISSILE* pMissile = (PLAYER_MISSILE*) getHead(thisPlayer.playerMissiles);
    MISSILE* eMissile = (MISSILE*) getHead(get_missile_list());
    while (eMissile != NULL) {
        missileX = eMissile->x;
        missileY = eMissile->y;
        while(pMissile != NULL) {
            playerX = pMissile->x;
            playerY = pMissile->y;
            //calculations
            distance = (playerX-missileX)*(playerX-missileX) + (playerY-missileY)*(playerY-missileY);
            midX = (playerX + missileX)/2;
            midY = (playerY + missileY)/2;
            //Missile Collision happened.
            if(distance < missileMissileTolerance) { 
                eMissile->status = MISSILE_EXPLODED;
                pMissile->status = PMISSILE_EXPLODED;
                uLCD.circle(midX,midY,3,0x800003);
                wait(.1);
                uLCD.circle(midX,midY,3,0x000000);
                wait(.1);
                uLCD.circle(midX,midY,5,0x800003);
                wait(.1);
                uLCD.circle(midX,midY,5,0x000000);
                numMissilesDestroyed++;
            }
            pMissile = (PLAYER_MISSILE*) getNext(thisPlayer.playerMissiles);
        }
        
        //check collisions with player
        //(player.x)-(missile->x)<1 && missile->x - player.x <playerMissileRad  && (player.y)-(missile->y)<5 && missile->y - player.y <1
        //(player.x)-(missile->x)<1 && missile->x - player.x <pMRad  && (player.y)-(missile->y)<5 && missile->y - player.y <1)
        if((missileY - thisPlayer.y < 1 && (thisPlayer.y - missileY) < 5) && ((thisPlayer.x - missileX) < 1 && (missileX - thisPlayer.x) < 5)) {
            numLives--;
            if(numLives == 0)
                thisPlayer.status = DESTROYED;
            eMissile->status = MISSILE_EXPLODED;
        }
        
        switch(numLives) {
            case 1:
                led1 = 1;
                led2 = 0;
                led3 = 0;
                break;
            case 2:
                led1 = 1;
                led2 = 1;
                led3 = 0;
                break;
            case 3:
                led1 = 1;
                led2 = 1;
                led3 = 1;
                break;
            default:
                led1 = 0;
                led2 = 0;
                led3 = 0;
                break;   
        }
        
        if(missileY > 128)
            eMissile->status = MISSILE_EXPLODED;
        
        eMissile = (MISSILE*) getNext(get_missile_list());   
    }
}

void checkCityCollisions()
{
    int i, xMin, xMax, y;
    MISSILE* eMissile = (MISSILE*) getHead(get_missile_list());
    while(eMissile != NULL)
    {
        for(i = 0; i <= 4; i++)
        {
            CITY city = city_get_info(i);
            if(city.status == DESTORIED)
                continue;
            xMin = city.x;
            xMax = xMin + city.width;
            y = 128-city.height;    
            if(xMin-(eMissile->x)<1 && eMissile->x - xMax <1  && y-(eMissile->y)<1 && 128-eMissile->y>0){
                eMissile->status = MISSILE_EXPLODED;
                city.status = DESTORIED; 
                city_destory(i);
                numCities--;
                int X = (xMin + xMax)/2;
                int Y = y;
                uLCD.circle(X,Y,3,0x800003);
                wait(.1);
                uLCD.circle(X,Y,3,0x000000);
                wait(.1);
                uLCD.circle(X,Y,5,0x800003);
                wait(.1);
                uLCD.circle(X,Y,5,0x000000);
                wait(.1);
                uLCD.circle(X,Y,7,0x800003);
                wait(.1);
                uLCD.circle(X,Y,7,0x000000);
                wait(.1);
                uLCD.circle(X,Y,10,0x800003);
                wait(.1);
                uLCD.circle(X,Y,10,0x000000);
            }
        } 
        eMissile = (MISSILE*) getNext(get_missile_list());   
    }
}

void nextLevel() {
    //clear the missiles from the screen. 
    MISSILE* eMissile = (MISSILE*) getHead(get_missile_list());
    while(eMissile != NULL) {
        eMissile->status = MISSILE_EXPLODED;
        eMissile = (MISSILE*) getNext(get_missile_list());
    }
    if(level != 4)
        level++;
    numMissilesDestroyed = 0;
    getLevelInfo();
}

void getLevelInfo() {
    switch(level) {
        case 1: //level 1
            set_missile_speed(7);
            set_missile_interval(60);
            missileMissileTolerance = 90;
            missilePlayerTolerance = 10;
            break;
        case 2: //level 2
            set_missile_speed(5);
            set_missile_interval(40);
            missileMissileTolerance = 80;
            missilePlayerTolerance = 12;
            break;
        case 3: //level 3
            set_missile_speed(3);
            set_missile_interval(20);
            missileMissileTolerance = 60;
            missilePlayerTolerance = 13;
            break;
        case 4: //level 4
            set_missile_speed(1);
            set_missile_interval(10);
            missileMissileTolerance = 40;
            missilePlayerTolerance = 15;
            break;
        default: //level 2 is medium
            set_missile_speed(5);
            set_missile_interval(40);
            missileMissileTolerance = 80;
            missilePlayerTolerance = 12;
    }
}


// ===User implementations end===

// Plays a wavfile
void playSound(char* wav) {
    //open wav file
    FILE *wave_file;
    wave_file=fopen(wav,"r");
    
    if(wave_file != NULL) 
    {
        printf("File opened successfully\n");

        //play wav file
        printf("Sound playing...\n");
        waver.play(wave_file);
    
        //close wav file
        printf("Sound stopped...\n");
        fclose(wave_file);
        return;
    }
    
    printf("Could not open file for reading - %s\n", wav);
    return;
}
