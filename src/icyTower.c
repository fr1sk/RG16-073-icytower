#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <GLUT/glut.h> //   *** DISABLE THIS LINE FOR LINUX ***
//#include <GL/glut.h>      *** ENABLE THIS LINE FOR LINUX ***
#include <stdio.h>


//#include <OpenGL/gl.h>
//#include <OpenGL/glu.h>


#define TIMER_ID 0
#define TIMER_INTERVAL 25
#define MOVEMENTSPEED 0.4
#define NUMOFBLOCKS 3
#define STARTSCROLLING 1
#define BLUE 3
#define YELLOW 2
#define GREEN 0
#define RED 1


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

Block blocks[NUMOFBLOCKS];
void drawAllTheBlocks();
void blockInit();

int main(int argc, char **argv)
{
    //glut init
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    
    //window create
    glutInitWindowSize(640, 740);
    glutInitWindowPosition(350, 100);
    glutCreateWindow(argv[0]);
    
    glutKeyboardFunc(onKeyboard);
    glutKeyboardUpFunc(onKeyboard2);
    glutDisplayFunc(on_display);
    
    srand(time(NULL));
    animation_ongoing = 0;
    
    glClearColor(0, 0.6, 0.544, 0);
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

GLuint loadTexture( const char * filename )
{
    
    GLuint texture;
    
    int width, height;
    
    unsigned char * data;
    
    FILE * file;
    
    file = fopen( filename, "rb" );
    
    if ( file == NULL ) return 0;
    width = 1024;
    height = 512;
    data = (unsigned char *)malloc( width * height * 3 );
    //int size = fseek(file,);
    fread( data, width * height * 3, 1, file );
    fclose( file );
    
    for(int i = 0; i < width * height ; ++i)
    {
        int index = i*3;
        unsigned char B,R;
        B = data[index];
        R = data[index+2];
        
        data[index] = R;
        data[index+2] = B;
        
    }
    
    
    glGenTextures( 1, &texture );
    glBindTexture( GL_TEXTURE_2D, texture );
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,GL_MODULATE );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST );
    
    
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,GL_REPEAT );
    gluBuild2DMipmaps( GL_TEXTURE_2D, 3, width, height,GL_RGB, GL_UNSIGNED_BYTE, data );
    free(data);
    
    return texture;
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
    for(int i=0; i<NUMOFBLOCKS; i++){
        if(blocks[i].currY<=-9){
            blocks[i].currY = 10;
            blocks[i].length = randBetween(2, 7);
            blocks[i].currX = randBetween(-8.3, 8.3);
            blocks[i].blockMode = getRandomBlock();
            blocks[i].yOffset = 0;
            if(blocks[i].blockMode==BLUE){
                blocks[i].bonus = 5;
            } else {
                blocks[i].bonus = 1;
            }
            
            
        }
    }
}


void blockInit(){
    for(int i=0; i<NUMOFBLOCKS; i++){
        switch (i) {
            case 0:
                blocks[i].currX = -5;
                blocks[i].currY = -3;
                blocks[i].length = 3;
                blocks[i].blockMode = 1;
                blocks[i].bonus = 1;
                break;
            case 1:
                blocks[i].currX = 0;
                blocks[i].currY = -8;
                blocks[i].length = 11;
                blocks[i].blockMode = 0;
                blocks[i].bonus = 0;
                break;
            case 2:
                blocks[i].currX = 5;
                blocks[i].currY = 2;
                blocks[i].length = 5;
                blocks[i].blockMode = GREEN;
                blocks[i].bonus = 1;
                break;
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


void scoreInit(){
    strcpy(textScore,"Score: 0");
    return;
}

void RenderString(float x, float y, void *font, const char* string, float r, float g, float b)
{
    GLfloat emission_coeffs[] = { r, g, b, 1 };
    glMaterialfv(GL_FRONT, GL_EMISSION, emission_coeffs);
    glRasterPos3f(x, y, .3);
    //glutBitmapString(font, string);
    emission_coeffs[0] = 0;
    emission_coeffs[1] = 0;
    emission_coeffs[2] = 0;
    emission_coeffs[3] = 1;
    glMaterialfv(GL_FRONT, GL_EMISSION, emission_coeffs);
}

void drawScore(){
    float text_x = .73;
    float text_y = 1.52;
    float text_z = .3;
    sprintf(textScore, "Score: %d", 123);
    RenderString(text_x, text_y, GLUT_BITMAP_HELVETICA_18, textScore, .6, .6, .6);
}

void gameOver(){
    animation_ongoing = 0;
    glClearColor(1.0f, 0.4f, 0.3f, 1.0f );
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawScore();
    
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
    if(rotation==360){
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
                currentFloorCoord = blockY+5.8;
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

void restartGame(){
    blockInit();
    currentX = 0;
    currentY = 0;
    globalScore = 0;
    moveWorld = false;
    bonusActivated = false;
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
            
            
            
        case 'l':
        case 'L':
            //change background color
            glClearColor((double)myRandom(255)/255, (double)myRandom(255)/255, (double)myRandom(255)/255, (double)myRandom(255)/255);
            glutPostRedisplay();
            haveFloor = false;
            break;
    }
}

static void on_timer(int value)
{
    
    if (value != TIMER_ID)
        return;
    
    jump();
    throughWall();
    
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
    glFlush();
    glutSwapBuffers();
}
