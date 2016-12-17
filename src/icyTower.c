#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <GL/glut.h>

#define TIMER_ID 0
#define TIMER_INTERVAL 25
#define MOVEMENTSPEED 0.3


struct point{
    int x,y;
};


static float currentX, currentY;    //current object coordinates
static float vectorSpeedX, vectorSpeedY;   //speed vector coords
static int animation_ongoing;   //animation flag
static bool left = false;
static bool right = false;
static float border = 8.4;
static float borderTrough = 8.39;

static void onKeyboard(unsigned char key, int x, int y);
static void onKeyboard2(unsigned char key, int x, int y);

static void on_timer(int value);
static void on_display(void);


int main(int argc, char **argv)
{
    //glut init
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);

    //window create
    glutInitWindowSize(360, 640);
    glutInitWindowPosition(100, 100);
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
    glutMainLoop();

    return 0;
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
    printf("\ta - move left\n");
}

void moveRight(){
    currentX += MOVEMENTSPEED;
    printf("\td - move right\n");
}

void jump(){
    currentY += vectorSpeedY;
    if (currentY <= -(6)) {
        currentY = -6;
        vectorSpeedY *= -1;
    }
    else{
        vectorSpeedY -= 0.06;
    }
}

void throughWall(){
  if(currentX < -borderTrough){
    currentX = borderTrough;
  }

  if(currentX > borderTrough){
    currentX = -borderTrough;
  }
}

void drawPlatform(){
    #define glutCube glutWireCube
    glPushMatrix();
    glScalef(0.2, 1, 1);
    glutCube(1);
    glPopMatrix();
    #undef glutCube
}

static void onKeyboard2(unsigned char key, int x, int y){
    if(key=='d' || key=='D'){
        right=false;
    } else if(key=='a' || key=='A'){
        left=false;
    }
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


    case 'l':
    case 'L':
        //change background color
        glClearColor((double)myRandom(255)/255, (double)myRandom(255)/255, (double)myRandom(255)/255, (double)myRandom(255)/255);
        glutPostRedisplay();
    	break;
    }
}

static void on_timer(int value)
{

    if (value != TIMER_ID)
        return;

    jump();
    throughWall();


    //printf("x=%f  y=%f",currentX, currentY);
    if(left){
        moveLeft();
    }

    if(right){
        moveRight();
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
    gluLookAt(15,0,0,0,0,0,0,1,0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, 1, 1, 100);

    glPushMatrix();
    glTranslatef(0, -1, 0);
    glTranslatef(currentX,currentY,0);
    glColor3ub(255, 193, 7);
    glScalef(1,1,1);

    glutWireCube(1);
    glPopMatrix();
    //end for jumping cube

    //start for floor
    glPushMatrix();
    glTranslatef(0, -8, 0);
    glColor3ub(11, 11, 11);
    glScalef(20,1,1);

    glutWireCube(1);
    glPopMatrix();
    //end for floor


    glutSwapBuffers();
}
