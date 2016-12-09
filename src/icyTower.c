#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <GL/glut.h>

#define TIMER_ID 0
#define TIMER_INTERVAL 25
#define MOVEMENTSPEED 0.03

struct point{
    int x,y;
};

const static float size = 0.1;  //size of object
static float currentX, currentY;    //current object coordinates
static float vectorSpeedX, vectorSpeedY;   //speed vector coords
static int animation_ongoing;   //animation flag
static bool left = false;
static bool right = false;

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

    //main loop
    glutMainLoop();

    return 0;
}

int myRandom(int m) {
	return rand()%m;

}
void test(){
    currentX = -(1 - size / 2) + (2 - size);
    currentY = -(1 - size / 2) + (2 - size);

    vectorSpeedX = -size / 2 + size * MOVEMENTSPEED;
    vectorSpeedY = -size / 2 + size * MOVEMENTSPEED;
}

void moveLeft(){
    currentX -= MOVEMENTSPEED;
}

void moveRight(){
    currentX += MOVEMENTSPEED;
}

void jump(){
    if (currentX <= -(1 - size / 2)) {
        vectorSpeedX *= -1;
        currentX = -(1 - size / 2);
    }
    if (currentX >= 1 - size / 2) {
        vectorSpeedX *= -1;
        currentX = 1 - size / 2;
    }

    currentY += vectorSpeedY;
    if (currentY <= -(1 - size / 2)) {
        vectorSpeedY *= -1;
        currentY = -(1 - size / 2);
    }

    if (currentY >= 1 - size / 2) {
        vectorSpeedY *= -1;
        currentY = 1 - size / 2;
    }

    vectorSpeedY -= .01;
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
        if (!animation_ongoing) {
            glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
            animation_ongoing = 1;
        }
        break;

    case 'p':
    case 'P':
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

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    glColor3ub(255, 193, 7);
    glBegin(GL_POLYGON);

        glVertex3f(currentX - size , currentY - size , 0);
        glVertex3f(currentX + size , currentY - size , 0);
        glVertex3f(currentX + size , currentY + size , 0);
        glVertex3f(currentX - size , currentY + size , 0);
    glEnd();


    glutSwapBuffers();
}
