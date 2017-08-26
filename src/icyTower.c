#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
//#include <GLUT/glut.h> //   *** DISABLE THIS LINE FOR LINUX ***
#include <GL/glut.h>      *** ENABLE THIS LINE FOR LINUX ***
#include <stdio.h>
#include <math.h>

//#include <freeglut.h>
//#include <GL/gl.h>
//#include <GL/glut.h>

//#include <OpenGL/gl.h>
//#include <OpenGL/glu.h>
//#include <GLUT/GLUT.h>
//
//#include <OpenGL/gl.h>
//#include <OpenGL/glu.h>


#define TIMER_ID 0
#define TIMER_INTERVAL 25
#define MOVEMENTSPEED 0.4
#define NUMOFBLOCKS 7
#define STARTSCROLLING 1
#define BLUE 3
#define YELLOW 2
#define GREEN 0
#define RED 1
#define HEIGHT 740
#define WIDTH 640


struct point{
    int x,y;
};


static float currentX, currentY;    //current object coordinates
static float currentFloorCoord = -6; //current floor Y coordinate
static float vectorSpeedX, vectorSpeedY;   //speed vector coords
static int animation_ongoing;   //animation flag
static bool left = false;
static bool right = false;
static float border = 8.4;
static float borderTrough = 8.39;
int currBlocks = 0;
bool haveFloor = true;
char textScore[15];
float worldMovementSpeed = 0;
bool moveWorld = false;
int globalScore = 0;
bool bonusActivated = false;
int bonusJumps = 0;
int rotation = 0;
bool isGameOver = false;
int maxHeightIndex = NUMOFBLOCKS-1;
float density = 0.6;

static void onKeyboard(unsigned char key, int x, int y);
static void onKeyboard2(unsigned char key, int x, int y);

static void on_timer(int value);
static void on_display(void);

typedef struct {
    float currX;
    float length;
    float currY;
    int blockMode; //0 - normal | 1 - breakable | 2 - bonus | 3 - boost
    float yOffset;
    int bonus; //increase score by BONUS when jump on this block
} Block;

typedef struct {
    float r;
    float g;
    float b;
} RGB;



Block blocks[NUMOFBLOCKS];
void pickColor(bool x);
void drawAllTheBlocks();
void blockInit();


int main(int argc, char **argv)
{
    //glut init
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    
    //window create
    glutInitWindowSize(WIDTH, HEIGHT);
    glutInitWindowPosition(350, 100);
    glutCreateWindow(argv[0]);
    
    glutKeyboardFunc(onKeyboard);
    glutKeyboardUpFunc(onKeyboard2);
    glutDisplayFunc(on_display);
    
    srand(time(NULL));
    animation_ongoing = 0;
    
    //glClearColor(0, 0.6, 0.544, 0);
    pickColor(true);
    glEnable(GL_DEPTH_TEST);
    printf("Input debug:\n");
    
    //main loop
    blockInit();
    glutMainLoop();
    
    return 0;
}




float randBetween(float min, float max){
    //r = (rand() % (max + 1 - min)) + min
    float r = (rand() % (int)(max+1-min)) + min;
    //printf("RAND %lf", r);
    return r;
}

int getRandomBlock(){
    int x = randBetween(0, 100);
    //printf("RANDOM: %d\n", x);
    
    if(x<40){
        return 0;
    } else if(x>=40 && x<75){
        return 1;
    } else if(x>=75 && x<90){
        return 3;
    } else {
        return 2;
    }
}

int myRandom(int m) {
    return rand()%m;
    
}
// void test(){
//     currentX = -(1 - size / 2) + (2 - size);
//     currentY = -(1 - size / 2) + (2 - size);
//
//     vectorSpeedX = -size / 2 + size * MOVEMENTSPEED;
//     vectorSpeedY = -size / 2 + size * MOVEMENTSPEED;
// }

void moveLeft(){
    currentX -= MOVEMENTSPEED;
    //printf("\ta - move left\n");
}

void moveRight(){
    currentX += MOVEMENTSPEED;
    //printf("\td - move right\n");
}

void startMovingWorld(){
    worldMovementSpeed = 0.1;
    moveWorld = true;
}

void jump(){
    if(currentY >= 1){
        startMovingWorld();
    }
    
    if (currentY <= currentFloorCoord) {
        currentY = currentFloorCoord;
        //upspeed
        //printf("rot: %d\n", rotation);
        if(rotation==31)
            vectorSpeedY = 1.6;
        else
            vectorSpeedY = 1.2;
    }
    else{
        //downspeed
        if(vectorSpeedY>=-1){
            vectorSpeedY -= 0.1;
        }
    }
    if(currentY>=5 && vectorSpeedY>0){
        return;
    }
    currentY += vectorSpeedY;
}

void throughWall(){
    if(currentX < -borderTrough){
        currentX = borderTrough;
    }
    
    if(currentX > borderTrough){
        currentX = -borderTrough;
    }
}

//void drawPlatform(){
//#define glutCube glutWireCube
//    glPushMatrix();
//    glScalef(0.2, 1, 1);
//    glutCube(1);
//    glPopMatrix();
//#undef glutCube
//}

void generateRandBlockCoord(int x){
    float blockY = randBetween(3.2,7.1);
    float blockX = randBetween(-8.0, 8.0);
}


void setNewBlockCoords(){
    int minHeight = (maxHeightIndex+1)%NUMOFBLOCKS;
    //printf("min height: %d\n", minHeight);
    if(blocks[minHeight].currY<=-9){
        blocks[minHeight].currY = blocks[maxHeightIndex].currY + randBetween(5, 7) * density;
        blocks[minHeight].length = randBetween(2, 7);
        blocks[minHeight].currX = randBetween(-8.3, 8.3);
        blocks[minHeight].blockMode = getRandomBlock();
        blocks[minHeight].yOffset = 0;
        if(blocks[minHeight].blockMode==BLUE){
            blocks[minHeight].bonus = 5;
        } else if(blocks[minHeight].blockMode==RED){
            blocks[minHeight].bonus = 2;
        } else {
            blocks[minHeight].bonus = 1;
        }
        maxHeightIndex = minHeight;
    }
    
}


void blockInit(){
    int tmpY = 7;
    for(int i=0; i<NUMOFBLOCKS; i++){
        switch (i) {
            case 0:
                blocks[i].currX = -5;
                blocks[i].currY = -3;
                blocks[i].length = 3;
                blocks[i].blockMode = RED;
                blocks[i].bonus = 1;
                break;
            case 1:
                blocks[i].currX = 0;
                blocks[i].currY = -8;
                blocks[i].length = 11;
                blocks[i].blockMode = GREEN;
                blocks[i].bonus = 0;
                break;
            case 2:
                blocks[i].currX = 5;
                blocks[i].currY = 2;
                blocks[i].length = 5;
                blocks[i].blockMode = GREEN;
                blocks[i].bonus = 1;
                break;
            case 3:
                blocks[i].currX = 3;
                blocks[i].currY = 7;
                blocks[i].length = 4;
                blocks[i].blockMode = BLUE;
                blocks[i].bonus = 5;
                break;
        }
        if(i>3){
            tmpY += randBetween(5, 7);
            blocks[i].currY = tmpY;
            blocks[i].currX = randBetween(-8.3, 8.3);
            blocks[i].blockMode = getRandomBlock();
            blocks[i].length = randBetween(2, 7);
            blocks[i].yOffset = 0;
            if(blocks[i].blockMode==BLUE){
                blocks[i].bonus = 5;
            } else if(blocks[i].blockMode==RED) {
                blocks[i].bonus = 2;
            } else {
                blocks[i].bonus = 1;
            }
            
        }
        blocks[i].yOffset=0;
        
    }
    
}

void drawBlock(int index){
    GLfloat diffuse_coeffs[4];
    if(blocks[index].blockMode==1){
        diffuse_coeffs[0] = 1;
        diffuse_coeffs[1] = .2;
        diffuse_coeffs[2] = .2;
        diffuse_coeffs[3] = 1;
    } else if(blocks[index].blockMode==2){
        diffuse_coeffs[0] = 1;
        diffuse_coeffs[1] = .8;
        diffuse_coeffs[2] = 0;
        diffuse_coeffs[3] = 1;
    } else if(blocks[index].blockMode==3){
        diffuse_coeffs[0] = .2;
        diffuse_coeffs[1] = .2;
        diffuse_coeffs[2] = 1;
        diffuse_coeffs[3] = 1;
    } else {
        diffuse_coeffs[0] = .2;
        diffuse_coeffs[1] = 1;
        diffuse_coeffs[2] = .2;
        diffuse_coeffs[3] = 1;
        
    }
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs);
    
    glPopMatrix();
    glPushMatrix();
    glTranslatef(blocks[index].currX, blocks[index].currY+blocks[index].yOffset, 0);
    glColor3ub(255, 0, 0);
    glScalef(blocks[index].length,0.2,1);
    glutSolidCube(1);
    glPopMatrix();
    
    glutPostRedisplay();
}


void drawText(const char *text,int length , int x , int y) {
    if(isGameOver){
        text = "GAME OVER\0";
        length = 19;
        x = WIDTH/2;
        y = HEIGHT/2;
    }
    if(globalScore==0){
        text = "SCORE: 0\0";
        length = 8;
    }
    if(bonusActivated){
        text = "SCORE x 2: \0";
        length = 10;
    }
    
    
    glMatrixMode(GL_PROJECTION);
    double matrix[16];
    glGetDoublev(GL_PROJECTION_MATRIX,matrix);
    glLoadIdentity();
    glOrtho(0,800,0,600,-5,5);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glPushMatrix();
    glLoadIdentity();
    glRasterPos2i(x,y);
    
    
    for ( int i = 0 ; i < length ; i++)
        glutBitmapCharacter( GLUT_BITMAP_HELVETICA_18,(int)text[i]);
    
    //score
    int nDigits = floor(log10(abs(globalScore))) + 1;
    char str[20];
    sprintf(str, "%d", globalScore);
    
    if(!isGameOver){
        for ( int i = 0 ; i < nDigits ; i++)
            glutBitmapCharacter( GLUT_BITMAP_HELVETICA_18,(int)str[i]);
    } else {
        char *finalStr = "final score: ";
        glRasterPos2i(x,y-15);
        for ( int i = 0 ; i < strlen(finalStr) ; i++)
            glutBitmapCharacter( GLUT_BITMAP_HELVETICA_18,(int)finalStr[i]);
        for ( int i = 0 ; i < nDigits ; i++)
            glutBitmapCharacter( GLUT_BITMAP_HELVETICA_18,(int)str[i]);
    }
    
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixd(matrix);
    glMatrixMode(GL_MODELVIEW);
}

void gameOver(){
    animation_ongoing = 0;
    glClearColor(1.0f, 0.4f, 0.3f, 1.0f );
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //drawText("GAME OVER", 9, 0, 0);
    isGameOver = true;
    printf("GAME OVER\n");
}

void drawAllTheBlocks(){
    for(int i=0; i<NUMOFBLOCKS; i++){
        drawBlock(i);
    }
}

void removeIfBreakable(index){
    if(blocks[index].blockMode==1 && blocks[index].yOffset < 0){
        blocks[index].yOffset += -0.5;
    }
}

void rotationIncrement(){
    if(rotation>0 && rotation<360){
        rotation += 30;
    }
    if(rotation>=360){
        rotation = 0;
    }
}

void checkColision(int index){
    if(vectorSpeedY>=0){
        return;
    }
    float localCurrY = currentY - 0.8;
    float blockY = blocks[index].currY+0.1;
    float blockX = blocks[index].currX;
    float blockLen = blocks[index].length/2;
    //currentFloorCoord = -10;
    //printf("blockX=%lf  currentX=%lf\n", blockX, currentX);
    //printf("blockY=%lf  currentY=%lf offset=%lf\n", blockY, currentY, blocks[index].yOffset);
    if(localCurrY <= blockY && localCurrY >= blockY-1){ //1.5 instead of 1
        if(currentX >= blockX-blockLen && currentX <= blockX+blockLen){
            if(blocks[index].blockMode==RED){
                //red falling block
                currentFloorCoord = blockY+0.8;
                blocks[index].yOffset = -.01;
            } else if(blocks[index].blockMode == YELLOW){
                //yellow bonus block
                currentFloorCoord = blockY+0.8;
                printf("BONUS!\n");
                bonusActivated = true;
                bonusJumps = 5;
            } else if(blocks[index].blockMode == BLUE){
                //blue boost block
                currentFloorCoord = blockY+0.8;
                rotation = 1;
                
            } else {
                currentFloorCoord = blockY+0.8;
            }
            if(bonusActivated){
                if(bonusJumps>0){
                    bonusJumps--;
                    globalScore +=  blocks[index].bonus*2;
                } else {
                    printf("BONUS ENDED :(\n");
                    bonusActivated = false;
                    globalScore += blocks[index].bonus;
                }
                
            } else {
                globalScore +=  blocks[index].bonus;
            }
            blocks[index].bonus = 0;
            printf("SCORE: %d\n", globalScore);
            
        }
    }
}

static void onKeyboard2(unsigned char key, int x, int y){
    if(key=='d' || key=='D'){
        right=false;
    } else if(key=='a' || key=='A'){
        left=false;
    }
}

void drawPlayer(){
    //jumping cube
    GLfloat diffuse_coeffs[4];
    if(bonusActivated){
        diffuse_coeffs[0] = 1;
        diffuse_coeffs[1] = .8;
        diffuse_coeffs[2] = 0;
        diffuse_coeffs[3] = 1;
    } else {
        diffuse_coeffs[0] = 1;
        diffuse_coeffs[1] = 1;
        diffuse_coeffs[2] = 1;
        diffuse_coeffs[3] = 1;
    }
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs);
    
    
    glPushMatrix();
    
    glTranslatef(currentX,currentY,0);
    glRotatef(rotation,0.0,0.0,1.0);
    
    glColor3ub(255, 193, 7);
    glScalef(1,1,1);
    
    glutSolidCube(1);
    
    
    glPopMatrix();
    //end for jumping cube
}

void pickColor(bool x){
    //139 194 74
    RGB wallColors[] = {{0,0.6,0.544},{0.42,0.27,0.75},{1,0.61,0},{0.583,0.799,.31},{0,0.76,0.851}};
    RGB currentWall;
    if(x){
        currentWall = wallColors[0];
    } else {
        currentWall = wallColors[(int)randBetween(0,4)];
    }
    glClearColor(currentWall.r, currentWall.g, currentWall.b, 0);
}

void restartGame(){
    
    currentX = 0;
    currentY = 0;
    globalScore = 0;
    moveWorld = false;
    bonusActivated = false;
    pickColor(false);
    density = 0.6;
    isGameOver = false;
    blockInit();
    maxHeightIndex = NUMOFBLOCKS-1;
    printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\nNEW GAME: ");
}

static void onKeyboard(unsigned char key, int x, int y)
{
    switch (key) {
        case 27:
            //exit game
            exit(0);
            break;
            
        case 'o':
        case 'O':
            //start game
            printf("\to - start game\n");
            if (!animation_ongoing) {
                glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
                animation_ongoing = 1;
            }
            break;
            
        case 'p':
        case 'P':
            printf("\tp - pause game\n");
            //pause game
            animation_ongoing = 0;
            break;
            
        case 'a':
        case 'A':
            //move left
            left = true;
            break;
            
        case 'd':
        case 'D':
            //move right
            right = true;
            break;
            
        case 'r':
        case 'R':
            //restart game
            restartGame();
            break;
            
            
        case 'l':
        case 'L':
            //change background color
            glClearColor((double)myRandom(255)/255, (double)myRandom(255)/255, (double)myRandom(255)/255, (double)myRandom(255)/255);
            glutPostRedisplay();
            haveFloor = false;
            break;
    }
}

void decreaseDensity(){
    if(globalScore>50){
        density = 0.7;
    } else if(globalScore>100){
        density = 0.8;
    } else if(globalScore>150){
        density = 0.9;
    } else if(globalScore>200){
        density = 1;
    }
}

static void on_timer(int value)
{
    
    if (value != TIMER_ID)
        return;
    
    jump();
    throughWall();
    decreaseDensity();
    
    if(currentY<-15){
        gameOver();
    }
    
    
    
    //printf("x=%f  y=%f\n",currentX, currentY);
    if(left){
        moveLeft();
    }
    
    if(right){
        moveRight();
    }
    
    
    
    
    //currentFloorCoord = (haveFloor ? -6 : -100);
    currentFloorCoord = -50;
    for(int i=0; i<NUMOFBLOCKS; i++){
        
        //remove colision if block is breakable and removed from map
        if(blocks[i].yOffset==0){
            checkColision(i);
            
        }
        removeIfBreakable(i);
        
    }
    
    rotationIncrement();
    
    if(moveWorld){
        for(int i=0; i<NUMOFBLOCKS; i++){
            if(currentY>=5 && vectorSpeedY>0){
                blocks[i].currY -= vectorSpeedY * 1.001; //*1.2;
                
            } else {
                blocks[i].currY -= worldMovementSpeed;
            }
        }
        setNewBlockCoords();
        
    }
    
    
    glutPostRedisplay();
    
    
    if (animation_ongoing) {
        glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
    }
}



static void on_display(void)
{
    //start for jumping cube
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    
    GLfloat light_position[] = { 0, 2.3, -2.3, 1 };
    
    //lights
    GLfloat light_ambient[] = { .1, .1, .1, 1 };
    GLfloat light_diffuse[] = { .5, .5, .5, 1 };
    GLfloat light_specular[] = { .6, .6, .6, 1 };
    
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    
    GLfloat ambient_coeffs[] = { .7, .7, .7, 1 };
    GLfloat specular_coeffs[] = { .6, .5, .6, 1 };
    GLfloat shininess = 80;
    
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_coeffs);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular_coeffs);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);
    //lights
    
    gluLookAt(15,0,0,0,0,0,0,1,0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, 1, 1, 100);
    
    
    drawPlayer();
    
    //start for floor
    //glPushMatrix();
    //glTranslatef(0, -8, 0);
    //glColor3ub(11, 11, 11);
    //glScalef(20,1,1);
    
    //glutWireCube(1);
    //glPopMatrix();
    //end for floor
    
    
    //blockInit();
    //drawImage();
    
    drawAllTheBlocks();
    drawText("SCORE: ", 10, 1, 1);
    
    glFlush();
    glutSwapBuffers();
}
