//
//  World navigation game
//
//  Created by Reid Golnik and Zhen Guo on 11/11/19.
//

/* Interactions
 * Click on the window and drag to change the view direction.
 *
 * Right click on the window for menu to stop, start, or exit the animation.
 *
 * Use the upper arrow key to move forward.
 *
 * Press q to quit the program.
 * Press w to reset the position of obstacles.
 */

#include <GLUT/glut.h>
#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <cstdlib>
#include "tgaClass.h"

using namespace std;


// Constants
const GLfloat PI = 3.1415926f;
const GLfloat RAD_TO_DEG = 180.0 / PI;

//globals for menu
static int window;
static int menu_id;
static int value = 0;

// Global variables.

GLfloat zoom = 50.0;
GLfloat mouse_x = 0.0;            // Mouse coordinates
GLfloat mouse_y = 0.0;
GLint win_width = 400;            // Window dimensions
GLint win_height = 300;


// Drag Function value
static GLfloat angle1 = 0;   /* in degrees */
static GLfloat angle2 = 0;   /* in degrees */
static int moving = 0, startx=0, starty=0;

// x and y and z value for translate, normalized to [0,1].
double xm = 0.0;
double ym = 0.0;
double zm = 0.0;

float xbirdOrigin = 0.0;
float zbirdOrigin = 0.0;

//Original postion of the camera
float xcamera = 0.0;
float ycamera = 3.0;
float zcamera = -7.0;

//Global boundaries
float boundarymax = 26.0;
float boundarymin = -26.0;

// Global flags for animation
bool ybodyTransmax = false;
float speed = 0.025;
bool innerRotateDown = false;
bool jointRotateDown = false;
bool headRotateMax = false;
bool tailRotateMax = false;
bool obstaclesPlaced = false;


//global vairbles for animation
float xbody, ybody, zbody = 0;
float inner_wing_angle = 0;
float joint_wing_angle = 0;
float outter_wing_angle = 0;
float head_angle = 0;
float tail_angle = 0;

float angle = 0.0; //tracks the rotate angle in degree
float new_z,new_x = 0.0; //tracks the bird position
float obstacleLocations[24]; //stores the position of obstacles


//sets up texture using TGA object - several options provided
void SetTextureParameters(int id, int texturenum)
{
    char buffer[30];    //filename holder
    int status = 0;        //error codes for file read
    TGA myTGAImage;
    
    if(texturenum == 1)
    {
        sprintf (buffer, "sky.tga");        //load buffer with filename
    }
    if(texturenum == 2)
    {
        sprintf (buffer, "woosterView.tga");        //load buffer with filename
    }
    
    if(texturenum == 3)
    {
        sprintf (buffer, "brick.tga");        //load buffer with filename
    }
    
    if(texturenum == 4)
    {
        sprintf (buffer, "thatch.tga");        //load buffer with filename
    }
    if(texturenum == 5)
    {
        sprintf (buffer, "cloud.tga");        //load buffer with filename
    }
    
    status = myTGAImage.readTGA(buffer);// Load ""Image.tga"" into a TGA object
    if(!status)
        printf("Error occurred = %d\n",status);
    
    glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
    glBindTexture (GL_TEXTURE_2D, id);
    
    // Build Mipmaps (builds different versions of the picture for distances - looks better)
    gluBuild2DMipmaps(GL_TEXTURE_2D, myTGAImage.format, myTGAImage.width,
                      myTGAImage.height, myTGAImage.format, GL_UNSIGNED_BYTE, myTGAImage.data);
    
    // Lastly, we need to tell OpenGL the quality of our texture map.  GL_LINEAR is the smoothest.
    // GL_NEAREST is faster than GL_LINEAR, but looks blochy and pixilated.  Good for slower computers though.
    
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    
    //other possible settings to play with
    // OpenGL texture variables which change for various apps

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    
    
}

//Functions to draw a house with brick walls and a thatch roof
void draw_house (GLfloat x, GLfloat z, GLfloat size) {
    // Draw a house.  Save current transformation.
    glEnable(GL_TEXTURE_2D);
    glPushMatrix();
    
    glTranslatef(x, 0.0, z);
    
    glRotatef(-90, 1,0,0);
    glScalef(size, size, size);
    
    SetTextureParameters(0, 3);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);    glVertex3f(1.0, -1.0, 0.0);        // Wall 1
    glTexCoord2f(0, 1);    glVertex3f(1.0, -1.0, 1.0);
    glTexCoord2f(1, 1);    glVertex3f(-1.0, -1.0, 1.0);
    glTexCoord2f(1, 0);    glVertex3f(-1.0, -1.0, 0.0);
    
    glTexCoord2f(1, 0);    glVertex3f(1.0, -1.0, 0.0);        // Wall 2
    glTexCoord2f(0, 0);    glVertex3f(1.0, 1.0, 0.0);
    glTexCoord2f(0, 1);    glVertex3f(1.0, 1.0, 1.0);
    glTexCoord2f(1, 1);    glVertex3f(1.0, -1.0, 1.0);
    
    glTexCoord2f(1, 1);    glVertex3f(-1.0, 1.0, 0.0);        // Wall 3
    glTexCoord2f(1, 0);    glVertex3f(-1.0, 1.0, 1.0);
    glTexCoord2f(0, 0);    glVertex3f(1.0, 1.0, 1.0);
    glTexCoord2f(0, 1);    glVertex3f(1.0, 1.0, 0.0);
    
    glTexCoord2f(0, 1);    glVertex3f(-1.0, -1.0, 1.0);    // Wall 4
    glTexCoord2f(1, 1);    glVertex3f(-1.0, 1.0, 1.0);
    glTexCoord2f(1, 0);    glVertex3f(-1.0, 1.0, 0.0);
    glTexCoord2f(0, 0);    glVertex3f(-1.0, -1.0, 0.0);
    glEnd();
    
    SetTextureParameters(0, 4);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);    glVertex3f(1.0, -1.0, 1.0);     // Roof side 1
    glTexCoord2f(0, 1);    glVertex3f(0.0, -1.0, 1.5);
    glTexCoord2f(1, 1);    glVertex3f(0.0, 1.0, 1.5);
    glTexCoord2f(1, 0);    glVertex3f(1.0, 1.0, 1.0);
    
    glTexCoord2f(0, 0);    glVertex3f(-1.0, -1.0, 1.0);    // Roof side 2
    glTexCoord2f(0, 1);    glVertex3f(0.0, -1.0, 1.5);
    glTexCoord2f(1, 1);    glVertex3f(0.0, 1.0, 1.5);
    glTexCoord2f(1, 0);    glVertex3f(-1.0, 1.0, 1.0);
    glEnd();
    
    
    glBegin(GL_TRIANGLES);
    glTexCoord2f(0, 0);    glVertex3f(-1.0, -1.0, 1.0);    // Roof end 1
    glTexCoord2f(.5, .5);    glVertex3f(0.0, -1.0, 1.5);
    glTexCoord2f(1, 0);    glVertex3f(1.0, -1.0, 1.0);
    
    glTexCoord2f(0, 0);    glVertex3f(-1.0, 1.0, 1.0);        // Roof end 2
    glTexCoord2f(.5, .5);    glVertex3f(0.0, 1.0, 1.5);
    glTexCoord2f(1, 0);    glVertex3f(1.0, 1.0, 1.0);
    
    glEnd();
    
    
    glPushMatrix();    //Cloud Support 1
    SetTextureParameters(0, 5);
    glTranslatef(1, 1, 0);
    glutSolidSphere(.25, 30, 30);
    
    glPopMatrix();
    
    glPushMatrix();    //Cloud Support 2
    glTranslatef(1, -1, 0);
    glutSolidSphere(.25, 30, 30);
    
    glPopMatrix();
    
    glPushMatrix();    //Cloud Support 3
    glTranslatef(-1, 1, 0);
    glutSolidSphere(.25, 30, 30);
    
    glPopMatrix();
    
    glPushMatrix();    //Cloud Support 4
    glTranslatef(-1, -1, 0);
    glutSolidSphere(.25, 30, 30);
    
    glPopMatrix();
    
    
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

//This function drawas a geometry bird
void draw_bird(float x, float z, bool isObstacle)
{
    glPushMatrix();
    
    if(isObstacle)
    {
        glTranslatef(x, 0, z);
    }
    
    //Animate body movement
    glTranslatef(xbody, ybody, zbody);
    
    
    //Drawing Head
    glPushMatrix();
    //Animating the head
    glRotatef(head_angle, 0.0, 1.0, 0.0);
    glTranslatef(0.0, 0.0, 2.5);
    glRotatef(-90.0, 0.0, 1.0, 0.0);
    glColor3f(0.8f, 0.7f, 0.2f);
    glutSolidTeapot(0.5);
    glPopMatrix();
    
    //Drawing Tail
    glPushMatrix();
    //Animating the tail
    glRotatef(tail_angle, 1.0, 0.0, 0.0);
    glTranslatef(0.0, -0.3, -3.0);
    glRotatef(-10.0, 1.0, 0.0, 0.0);
    glScalef(1.0,0.2,1.0);
    glColor3f(0.8f, 0.7f, 0.2f);
    glutSolidCone(0.5, 1.5, 8, 8);
    glPopMatrix();
    
    
    //Draw Right Wing
    glPushMatrix();
    //Animate the whole wing
    glRotatef(inner_wing_angle, 0.0, 0.0, 1.0);
    glPushMatrix();
    //Animate the joint and the outer wing
    glTranslatef(-2.55, 0.0, 1.05);
    glRotatef(joint_wing_angle, 0.0, 0.0, 1.0);
    //Draw out wing
    glPushMatrix();
    glTranslatef(-.8, 0.0, -.85);
    glRotatef(160.0, 0.0, 1.0, 0.0);
    //glRotatef(outter_wing_angle, 0.0, 0.0, 1.0);
    glScalef(2.0,0.1,1.5);
    glColor3f(0.8f, 0.7f, 0.2f);
    glutSolidCube(0.8);
    glPopMatrix();
    //Draw joint
    glRotatef(169.0, 0.0, 1.0, 0.0);
    glScalef(1.0,0.2,1.0);
    glColor3f(0.0, 0.2f, 1.0);
    glutSolidCone(0.25, 1.16, 8, 8);
    glPopMatrix();
    //Draw Inner wing
    glTranslatef(-1.5, 0.0, 0.5);
    //glRotatef(-angle, 1.0, 1.0, 0.0);
    glScalef(2.0,0.1,1.5);
    glColor3f(0.8f, 0.7f, 0.2f);
    glutSolidCube(0.8);
    glPopMatrix();
    
    
    //Draw Left Wing
    glPushMatrix();
    glRotatef(inner_wing_angle, 0.0, 0.0, -1.0);
    glPushMatrix();
    glTranslatef(2.55, 0.0, 1.05);
    glRotatef(joint_wing_angle, 0.0, 0.0, -1.0);
    //Draw out wing
    glPushMatrix();
    glTranslatef(.8, 0.0, -.85);
    glRotatef(-160.0, 0.0, 1.0, 0.0);
    glScalef(2.0,0.1,1.5);
    glColor3f(0.8f, 0.7f, 0.2f);
    glutSolidCube(0.8);
    glPopMatrix();
    //Draw joint
    glRotatef(-169.0, 0.0, 1.0, 0.0);
    glScalef(1.0,0.2,1.0);
    glColor3f(0.0, 0.2f, 1.0);
    glutSolidCone(0.25, 1.16, 8, 8);
    glPopMatrix();
    //Draw Inner wing
    glTranslatef(1.5, 0.0, 0.5);
    //glRotatef(-angle, 1.0, 1.0, 0.0);
    glScalef(2.0,0.1,1.5);
    glColor3f(0.8f, 0.7f, 0.2f);
    glutSolidCube(0.8);
    glPopMatrix();
    
    //Draw Bird Body
    glTranslatef(0.0, 0.0, 0.0);
    glColor3f(0.0, 0.2f, 1.0);
    glScalef(1.0,.8,2.0);
    glutSolidSphere(1.0,30.0,30.0);
    glPopMatrix();
}

void drawScene(void) {
    glEnable(GL_TEXTURE_2D);
    SetTextureParameters(0, 1);
    glBegin(GL_QUADS);
    
    ////////////////////// BACK WALL ///////////////////////
    
    glTexCoord2f(0, 0);    glVertex3f(-30.0, -1, -30);
    glTexCoord2f(0, 1);    glVertex3f(-30.0, 9, -30);
    glTexCoord2f(1, 1);    glVertex3f(30.0, 9, -30);
    glTexCoord2f(1, 0);    glVertex3f(30.0, -1, -30);
    
    ////////////////////// FRONT WALL ///////////////////////
    
    glTexCoord2f(0, 0);    glVertex3f(-30.0, -1, 30.0);
    glTexCoord2f(0, 1);    glVertex3f(-30.0, 9, 30.0);
    glTexCoord2f(1, 1);    glVertex3f(30.0, 9, 30.0);
    glTexCoord2f(1, 0);    glVertex3f(30.0, -1, 30.0);
    
    ////////////////////// LEFT WALL ///////////////////////
    
    glTexCoord2f(0, 0);    glVertex3f(-30.0, -1, 30.0);
    glTexCoord2f(0, 1);    glVertex3f(-30.0, 9, 30.0);
    glTexCoord2f(1, 1);    glVertex3f(-30.0, 9, -30.0);
    glTexCoord2f(1, 0);    glVertex3f(-30.0, -1, -30.0);
    
    ////////////////////// RIGHT WALL ///////////////////////
    
    glTexCoord2f(0, 0);    glVertex3f(30.0, -1, 30);
    glTexCoord2f(0, 1);    glVertex3f(30.0, 9, 30);
    glTexCoord2f(1, 1);    glVertex3f(30.0, 9, -30);
    glTexCoord2f(1, 0);    glVertex3f(30.0, -1, -30);
    
    // Stop drawing the walls
    glEnd();
    
    ////////////////////// FLOOR ///////////////////////////
    
    SetTextureParameters(0, 2);
    glBegin(GL_QUADS);
    
    glColor3f(0, 0 , 0);
    glTexCoord2f(0, 1);    glVertex3f(-30, -1, 30);
    glTexCoord2f(0, 0);    glVertex3f(-30, -1, -30);
    glTexCoord2f(1, 0);    glVertex3f(30, -1, -30);
    glTexCoord2f(1, 1);    glVertex3f(30, -1, 30);
    
    // Stop drawing the floor
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

//Draw a simple cloud made of three spheres
void drawCloud (float xcoord, float zcoord) {
    glColor3f(1, 1, 1);
    glPushMatrix();
    glTranslatef(xcoord, 0, zcoord);
    glPushMatrix();
    glTranslatef(-.75, 0, 0);
    glutSolidSphere(1, 30, 30);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0, .75, 0);
    glutSolidSphere(1, 30, 30);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0, 0, -.75);
    glutSolidSphere(1, 30, 30);
    glPopMatrix();
    glPopMatrix();
}

//Randomly palces obstacles around the scene
void placeObstacles() {
    if(!obstaclesPlaced)
    {
        for(int j = 0; j < 24; j++)
        {
            obstacleLocations[j] = (rand()%57)-28;
            //ensures obstacles don't spawn on top of the starting position
            while(obstacleLocations[j] > -4 && obstacleLocations[j] < 4)
            {
                obstacleLocations[j] = (rand()%57)-28;
            }
        }
        obstaclesPlaced = true;
    }
    
    int i = 0;
    while(i < 24)
    {
        drawCloud(obstacleLocations[i], obstacleLocations[i + 1]);
        i = i + 2;
        
        draw_bird(obstacleLocations[i], obstacleLocations[i + 1], true);
        i = i + 2;
        
        draw_house(obstacleLocations[i], obstacleLocations[i + 1], 2);
        i = i + 2;
    }
}

//Sets up the camera using gluLookAt(), calls draw_bird(), and drawScene()
void setViewPoint (void) {
    angle = int(RAD_TO_DEG * angle1 * .1) % 360;
    
    
    gluLookAt(
              xcamera, ycamera, zcamera,
              0.0 , 1.0, 0.0,
              0.0, 1.0, 0.0 );
    
    draw_bird(xbirdOrigin, zbirdOrigin, false);
    glRotatef(angle,0.0, 1.0, 0.0);
    glTranslatef(new_x, 0, -new_z);
    
    drawScene();
}


//Display callback
void display (void) {
    // The display callback function.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    setViewPoint();
    
    placeObstacles();
    
    glutSwapBuffers();
}



//Idle function
void idle()
{
    //controls for menu
    if(value == 0 || value == 1)
    {
        //This is for body bumping
        if (!ybodyTransmax)
        {
            ybody = ybody + speed;
            if (ybody > 1) ybodyTransmax = true;
        }
        else{
            ybody = ybody - speed;
            if (ybody < -.2) ybodyTransmax = false;
        }
        //This is for wing animation
        if (!innerRotateDown)
        {
            inner_wing_angle ++;
            joint_wing_angle -= 1.5;
            //outter_wing_angle -= 1.5;
            if (inner_wing_angle > 15) innerRotateDown = true;
            
        }
        else{
            inner_wing_angle --;
            joint_wing_angle += 1.5;
            //outter_wing_angle += 1.5;
            if (inner_wing_angle < -15) innerRotateDown = false;
            
        }
        //This is for head rotation
        if (!headRotateMax)
        {
            head_angle += .4;
            if (head_angle > 5) headRotateMax = true;
        }
        else{
            head_angle -= .4;
            if (head_angle < -5) headRotateMax = false;
        }
        //This is for tail animation
        if (!tailRotateMax)
        {
            tail_angle += .4;
            if (tail_angle > 5) tailRotateMax = true;
        }
        else{
            tail_angle -= .4;
            if (tail_angle < -5) tailRotateMax = false;
        }
    }
    
    
    glutPostRedisplay();
}


//This funciton manages keybord actions.
/*
 Press q to quit the program.
 Press w to reset the obstacles
 */
void keyboard (unsigned char key, int x, int y) {
    
    switch (key) {
            // Press q to terminate the program.
        case 'q':
            exit(0);
            break;
            //Randomizes the locations of obstacles
        case 'w':
            obstaclesPlaced = false;
            break;
    }
}

//This function check if the bird is going to collide an obstacle
bool is_colliding() {
    bool flag = false;
    bool flag1 = false;
    bool flag2 = false;
    int m = 0;
    while(m < 24) //Loop through the array where the location of obstacles are stored
    {
        //If the index is even, check the x value
        
        if(new_x + (.5 * sin(angle*PI/180)) > (obstacleLocations[m] - 2) &&
           new_x + (.5 * sin(angle*PI/180)) < (obstacleLocations[m] + 2))
        {
            flag1 = true;
        }
        
         //If the index is odd, check the z value
        
        if(new_z + (.5 * cos(angle*PI/180)) > (obstacleLocations[m+1] - 2 ) &&
           new_z + (.5 * cos(angle*PI/180)) < (obstacleLocations[m+1] + 2))
        {
            flag2 = true;
        }
        flag = flag1 and flag2;
        
        m+=2;
    }
    return flag;
}

//Move forward in the direction the bird is looking
void controls (int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_UP:
            //Check the boundries of the scene
            if(new_x + (.5 * sin(angle*PI/180)) < boundarymax &&
               new_x + (.5 * sin(angle*PI/180)) > boundarymin &&
               new_z + (.5 * cos(angle*PI/180)) < boundarymax &&
               new_z + (.5 * cos(angle*PI/180)) > boundarymin)
            {
                //Check the obstacle collision
                if(!is_colliding())
                {
                    new_x = new_x + (.5 * sin(angle*PI/180));
                    new_z = new_z + (.5 * cos(angle*PI/180));
                }
            }
            break;
    }
}

//This function manage the projection
void set_projection () {
    // Reset the projection when zoom setting or window shape changes.
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(zoom, GLfloat(win_width) / GLfloat(win_height),
                   1.0, 500.0);
    
    
    
    GLfloat light_position[] = { 0.0, 5, 0.0, 0.0 };
    
    GLfloat light_ambient[] = {1,1,1,1.0};
    
    glClearColor (0.5,0.7,1.0, 0.0);
    
    glShadeModel (GL_SMOOTH); //smooth shaded polygons
    
    glLightfv(GL_LIGHT0, GL_POSITION, light_position); //light position
    
    glLightfv(GL_LIGHT0,GL_AMBIENT, light_ambient); //a slightly red ambient light
    
    GLfloat mat_ambient[]= {1, 1.0, 1, 1.0};
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient); //define mat color
    
    glEnable(GL_LIGHTING); //enable lighting
    
    glEnable(GL_LIGHT0); //enable the first light = light0
    
    glEnable(GL_DEPTH_TEST); //depth test performed
    
    glEnable(GL_NORMALIZE);
}

//This function reshapes the object with the window
void reshape (GLint new_width, GLint new_height) {
    // Window reshaping function.
    win_width = new_width;
    win_height = new_height;
    glViewport(0, 0, win_width, win_height);
    set_projection();
}

//This function manage mouse movement to drag the object
void drag (int x, int y) {
    // Use mouse values to move the scenery around.
    xm = (2.0 * x) / win_width - 1.0;
    ym = (2.0 * y) / win_height - 1.0;
    if (moving) {
        angle1 = (angle1 + (x - startx));
        angle2 = (angle2 + (y - starty));
        startx = x;
        starty = y;
        glutPostRedisplay();
    }
    glutPostRedisplay();
}
//Determines if left mouse is being held
static void
mouse(int button, int state, int x, int y)
{
    /* Rotate the scene with the left mouse button. */
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            moving = 1;
            startx = x;
            starty = y;
        }
        if (state == GLUT_UP) {
            moving = 0;
        }
        
    }
}

//This is the menu
void menu(int num)
{
    if(num == 0)
    {
        glutDestroyWindow(window);
        exit(0);
    }
    else
        value = num;
    
    glutPostRedisplay();
}

//Create a menu attached on right button
//Options are start, stop or quit the animation
void createMenu(void)
{
    menu_id = glutCreateMenu(menu);
    glutAddMenuEntry("Start", 1);
    glutAddMenuEntry("Stop", 2);
    glutAddMenuEntry("Quit", 0);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

int main (int argc, char *argv[]) {
    
    // Initialize GLUT.
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA);
    glutInitWindowSize(win_width, win_height);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("An Ugly Bird");
    
    // Register callbacks.
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutIdleFunc(idle);
    glutMouseFunc(mouse);
    glutMotionFunc(drag);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(controls);
    
    // Select GL options.
    glEnable(GL_DEPTH_TEST);
    
    // Initialize projection.
    set_projection();
    createMenu();
    
    
    
    glutMainLoop();
    return 0;
}







