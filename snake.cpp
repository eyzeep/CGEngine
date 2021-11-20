#include <iostream>
#include <cmath>
#include "MySTL.h"
#include "CGEngine.cpp"
#include <conio.h>
using namespace std;


//checks wasd buttons status and returns which button is pressed
char checkWASD(int MilSec)
{
    for (int begin = clock(); clock() - begin < MilSec;)
    {
        if (GetAsyncKeyState(0x57))
        {
            Sleep(MilSec - (clock() - begin));
            return 'A';
        }
        if (GetAsyncKeyState(0x41))
        {
            Sleep(MilSec - (clock() - begin));
            return 'C';
        }
        if (GetAsyncKeyState(0x53))
        {
            Sleep(MilSec - (clock() - begin));
            return 'B';
        }
        if (GetAsyncKeyState(0x44))
        {
            Sleep(MilSec - (clock() - begin));
            return 'D';
        }
    }
    return 0;
}

int snakeeeeee(unsigned short XWindow = 26, unsigned short YWindow = 26, int dificulty = 1)
{
    system("cls");
    setPixelSize(13, 13);
    int score = 0;
    prepare({ (SHORT)XWindow,(SHORT)YWindow });
    Pen::drawFrame({ 0,0 }, { (SHORT)XWindow,(SHORT)YWindow }, BGGrey);
    PPObject snake;
    snake.addPainter(snakeHeadVert, { (SHORT)((rand() % (XWindow - 5)) + 5),(SHORT)((rand() % (XWindow - 5)) + 5) }, BGDarkGreen);
    
    COORD* snakeBodyCoord = new COORD[1];
    snakeBodyCoord[0] = snake.getPainterCOORD(0);
    char headDir = 'A';//A - up B - down C - left D - right
    int snakeSpeed = dificulty;//1-3
    int snakeSize = 1;
    PPObject food;
    COORD phantomTail;
    char lastDir= headDir, buffLastDir = 'C';
    food.addPainter(dot, { (short)(1 + rand() % (XWindow - 1)) ,(short)(1 + rand() % (YWindow - 1)) }, BGDarkRed);
    for (;;)
    {
        phantomTail = snakeBodyCoord[snakeSize - 1];

        buffLastDir = headDir;
        //checks for keyboard input 
        headDir = checkWASD(160 / snakeSpeed);
        if (headDir == 0) headDir = buffLastDir;
        if (headDir != buffLastDir)lastDir = buffLastDir;
        if ((headDir == 'A' && lastDir == 'B') ||
            (headDir == 'B' && lastDir == 'A') ||
            (headDir == 'C' && lastDir == 'D') ||
            (headDir == 'D' && lastDir == 'C'))headDir = lastDir;



        //redraw

        COORD last2Pos[2] = { snake.getPainterCOORD(0),snake.getPainterCOORD(0) };
        switch (headDir)
        {
        case 'A':
            snake.painterVectorMove(0, 0, -1, false);
            if (headDir != buffLastDir)
            {
                snake.addPainter(snakeHeadVert,snake.getPainterCOORD(0), BGDarkGreen);
                snake.deletePainter(0);
            }
            break;
        case 'B':
            snake.painterVectorMove(0, 0, 1, false);
            if (headDir != buffLastDir)
            {
                snake.addPainter(snakeHeadVert, snake.getPainterCOORD(0), BGDarkGreen);
                snake.deletePainter(0);
            }

            break;
        case 'C':
            snake.painterVectorMove(0, -1, 0, false);
            if (headDir != buffLastDir)
            {
                snake.addPainter(snakeHeadHoriz, snake.getPainterCOORD(0), BGDarkGreen);
                snake.deletePainter(0);
            }
            break;
        case 'D':
             
            snake.painterVectorMove(0, 1, 0, false);
            if (headDir != buffLastDir)
            {
                snake.addPainter(snakeHeadHoriz, snake.getPainterCOORD(0), BGDarkGreen);
                snake.deletePainter(0);
            }
            break;
        default:
            break;
        }
        dot(last2Pos[0], BGDarkGreen);
        dot(phantomTail, BGBlack);

        for (int i = 1; i < snakeSize; i++)
        {
            last2Pos[1] = snakeBodyCoord[i];
            snakeBodyCoord[i] = last2Pos[0];
            swap(last2Pos[0], last2Pos[1]);
        }

        snakeBodyCoord[0] = snake.getPainterCOORD(0);
        //is it crashed?
        if ((snakeBodyCoord[0].X == 0) ||
            (snakeBodyCoord[0].X == XWindow) ||
            (snakeBodyCoord[0].Y == 0) ||
            (snakeBodyCoord[0].Y == YWindow))
        {
            return score;
        }
        for (int i = 1; i < snakeSize; i++)
        {
            if (snakeBodyCoord[0].Y == snakeBodyCoord[i].Y && snakeBodyCoord[0].X == snakeBodyCoord[i].X)
            {
                return score;
            }
        }





        //is apple eaten?
        if ((food.getPainterCOORD(0).X == snakeBodyCoord[0].X && food.getPainterCOORD(0).Y == snakeBodyCoord[0].Y))
        {

            
            snakeSize++;
            COORD* temp = new COORD[snakeSize];
            for (int i = 0; i < snakeSize - 1; i++)
            {
                temp[i] = snakeBodyCoord[i];
            }
            temp[snakeSize - 1] = snakeBodyCoord[snakeSize - 2];
            delete[] snakeBodyCoord;
            snakeBodyCoord = temp;
            score++;
            for (int i = 0; i < snakeSize; i++)
            {
                int xrand = (rand() % (XWindow - 1)) - food.getPainterCOORD(0).X + 1,
                    yrand = (rand() % (YWindow - 1)) - food.getPainterCOORD(0).Y + 1;
                if (food.getPainterCOORD(0).X+xrand != snakeBodyCoord[i].X && food.getPainterCOORD(0).Y + yrand != snakeBodyCoord[i].Y)
                {
                    food.painterVectorMove(0,xrand, yrand, false);
                    break;
                }
            }
        }
    }


}


int main()
{

   
    return  snakeeeeee(50, 50, 2);
}