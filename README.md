# bird_navigation_game



This project creates a game where the user navigates a world as a bird. Be careful, you might collide onto randomly allocated obstacles (houses, clouds, or other birds) and can't move anymore!

## Prerequisites

* glut
* OpenGL

## Compile

g++ -Wno-deprecated -framework OpenGL -framework GLUT -framework Foundation -o bird SkyBoxBird.cpp tgaClass.cpp

## Run

If you are on Mac, cd to the folder where bird executable is saved, and just do ./bird. A window will pop up and allows you to interact with the word.

## Interactions

 * Click on the window and drag to change the view direction.
 
 * Right click on the window for menu to stop, start, or exit the animation.
 
 * Use the upper arrow key to move forward.
 
 * Press q to quit the program.
 * Press w to reset the position of obstacles.
