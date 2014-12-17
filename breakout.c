//
// breakout.c
//
// Computer Science 50
// Problem Set 4
//

// standard libraries
#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Stanford Portable Library
#include "gevents.h"
#include "gobjects.h"
#include "gwindow.h"

// height and width of game's window in pixels
#define HEIGHT 600
#define WIDTH 400

// number of rows of bricks
#define ROWS 5

// number of columns of bricks
#define COLS 10

// radius of ball in pixels
#define RADIUS 10

// lives
#define LIVES 3

// prototypes
void initBricks(GWindow window);
GOval initBall(GWindow window);
GRect initPaddle(GWindow window);
GLabel initScoreboard(GWindow window);
GLabel youWin(GWindow window);
void updateScoreboard(GWindow window, GLabel label, int points);
GObject detectCollision(GWindow window, GOval ball);

int main(void)
{
    // seed pseudorandom number generator
    srand48(time(NULL));

    // instantiate window
    GWindow window = newGWindow(WIDTH, HEIGHT);

    // instantiate bricks
    initBricks(window);

    // instantiate ball, centered in middle of window with a velocity
    GOval ball = initBall(window);

    // instantiate paddle, centered at bottom of window
    GRect paddle = initPaddle(window);

    // instantiate scoreboard, centered in middle of window, just above ball
    GLabel label = initScoreboard(window);

    // number of bricks initially
    int bricks = COLS * ROWS;

    // number of lives initially
    int lives = LIVES;

    // number of points initially
    int points = 0;
    
    // initial velocities
    double velocityofx = 2.0 + drand48();
    double velocityofy = -2.0 - drand48();
       
    // keep playing until game over
    while (lives > 0 && bricks > 0)
    {
         /* MOVE THE PADDLE */
         
            // check for mouse event
            GEvent event = getNextEvent(MOUSE_EVENT);

            // if we heard one
            if (event != NULL)
            {
                // if the event was movement
                if (getEventType(event) == MOUSE_MOVED)
                {
                    // ensure paddle follows middle cursor
                    double x = getX(event) - getWidth(paddle) / 2;
                    setLocation(paddle, x, getY(paddle));
                }
            }
         
         /* BOUNCE THE BALL */ 
         
            // move circle along x-axis
            move(ball, velocityofx, velocityofy);
            
            // call collision detector function
            GObject collidedobject = detectCollision(window, ball);
            
            // bouncing conditions
            if (collidedobject != NULL && collidedobject != label && collidedobject != window )
            {
                // bounce off right edge of window
                if (getX(ball) + getWidth(ball) >= getWidth(window))
                {
                    velocityofx = -velocityofx;
                }

                // bounce off left edge of window
                else if (getX(ball) <= 0)
                {
                    velocityofx = -velocityofx;
                }
                
                // remove and replace ball in center and if hit bottom of window
                else if (getY(ball) + getHeight(ball) >= getHeight(window))
                {
                    // moves the ball, then instantiate another ball with a velocity
                    setLocation(ball, WIDTH / 2 - (getWidth(ball) / 2), HEIGHT / 2);
                    
                    // remove the a life
                    lives--;
                    
                    // wait for a click, then move it again
                    waitForClick();
                    velocityofx = 2.0 + drand48();
                    velocityofy = -2.0 - drand48();
                    
                    move(ball, velocityofx, velocityofy);                    
                }
                
                // bounce off top of window
                else if (getY(ball) <= 0)       
                {
                    velocityofy = -velocityofy;
                }
                
                // bounce off top of paddle or the brick
                else if (strcmp(getType(collidedobject), "GRect") == 0)
                {
                    // bounce off a brick
                    if (collidedobject != paddle && collidedobject != window && collidedobject != label)
                    {
                        points = points + 1;
                        updateScoreboard(window, label, points);
                        removeGWindow(window, collidedobject);
                        if (points == 50)
                        {
                            removeGWindow(window, label);
                            youWin(window);
                            pause(10000);
                            closeGWindow(window);
                            return 0;
                        }
                        velocityofy = -velocityofy;
                    }
                    
                    // bounce off the paddle
                    else
                    {
                        velocityofy = -velocityofy;
                    }
                }
            }
            // linger before moving again
            pause(5);
            
        
    } 

    // wait for click before exiting
    waitForClick();

    // game over
    closeGWindow(window);
    return 0;
}

/**
 * Initializes window with a grid of bricks.
 */
void initBricks(GWindow window)
{
    // variables for the rectangle dimensions
    double width = 23;
    double height = 7;
    
    // init the objects
    GRect brick[ROWS][COLS];
    
    // paint objects via their rows and columns onto the window
    for (int i = 0; i < ROWS; i++) 
    {            
            for (int j = 0; j < COLS; j++)
            {
                // (distance from left + how far to the next rect), (intial distance from top + distance from each row)
                brick[i][j] = newGRect(31 + j*35, 40 + i*20, width, height);
                setFilled(brick[i][j], true);
                
                // set the colors of each row
                if (i == 0)
                {
                setColor(brick[i][j], "dark_gray");
                }
                
                else if (i == 1)
                {
                setColor(brick[i][j], "gray");
                }
                else if (i == 2)
                {
                setColor(brick[i][j], "light_gray");
                }
                else if (i == 3)
                {
                setColor(brick[i][j], "gray");
                }
                else if (i == 0)
                {
                setColor(brick[i][j], "dark_gray");
                }
                add(window, brick[i][j]);             
            }   
    }
}

/**
 * Instantiates ball in center of window.  Returns ball.
 */
GOval initBall(GWindow window)
{
    // set initial variables
    double width = 25, height = 25; 
    
    // init the ball with a color
    GOval ball = newGOval(WIDTH / 2 - (width / 2), HEIGHT / 2, width, height);
    setFilled(ball, true);
    setColor(ball, "orange");
    add(window, ball);
    return ball;
}

/**
 * Instantiates paddle in bottom-middle of window.
 */
GRect initPaddle(GWindow window)
{
    // set initial variables
    double width = 60, height = 10; 
    
    // create paddle with colors and add to screen
    GRect paddle = newGRect(WIDTH / 2 - (width / 2), HEIGHT - (HEIGHT / 8), width, height);
    setFilled(paddle, true);
    setColor(paddle, "black");
    add(window, paddle);
    return paddle;
}

/**
 * Instantiates, configures, and returns label for scoreboard.
 */
GLabel initScoreboard(GWindow window)
{
    // initialize the location variables
    double x, y; 
    
    // set the label
    GLabel label = newGLabel("0");
    setFont(label, "Arial-40");
    x = WIDTH / 2 - (getWidth(label) / 2);
    y = HEIGHT / 2;
    setLocation(label, x, y);
    add(window, label);
    return label;
}

/**
 * Instantiates, configures, and returns a congratulatory label upon winning.
 */
GLabel youWin(GWindow window)
{
    // initialize the location variables
    double x, y; 
    
    // set the label
    GLabel label = newGLabel("YOU WIN");
    setFont(label, "Arial-80");
    x = WIDTH / 2 - (getWidth(label) / 2);
    y = HEIGHT / 2;
    setLocation(label, x, y);
    add(window, label);
    return label;
}

/**
 * Updates scoreboard's label, keeping it centered in window.
 */
void updateScoreboard(GWindow window, GLabel label, int points)
{
    // update label
    char s[12];
    sprintf(s, "%i", points);
    setLabel(label, s);

    // center label in window
    double x = (getWidth(window) - getWidth(label)) / 2;
    double y = (getHeight(window) - getHeight(label)) / 2;
    setLocation(label, x, y);
}

/**
 * Detects whether ball has collided with some object in window
 * by checking the four corners of its bounding box (which are
 * outside the ball's GOval, and so the ball can't collide with
 * itself).  Returns object if so, else NULL.
 */
GObject detectCollision(GWindow window, GOval ball)
{
    // ball's location
    double x = getX(ball);
    double y = getY(ball);

    // for checking for collisions
    GObject object;

    // check for collision at ball's top-left corner
    object = getGObjectAt(window, x, y);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's top-right corner
    object = getGObjectAt(window, x + 2 * RADIUS, y);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's bottom-left corner
    object = getGObjectAt(window, x, y + 2 * RADIUS);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's bottom-right corner
    object = getGObjectAt(window, x + 2 * RADIUS, y + 2 * RADIUS);
    if (object != NULL)
    {
        return object;
    }

    // no collision
    return NULL;
}
