#pragma once
#include <windows.h>
#include <cmath>
#include "ConsoleColours.h"
using namespace std;
//Console Graphic Engine


void setPixelSize(int x, int y)
{

    CONSOLE_FONT_INFOEX lpConsoleCurrentFontEx;
    lpConsoleCurrentFontEx.cbSize = sizeof(CONSOLE_FONT_INFOEX);
    lpConsoleCurrentFontEx.dwFontSize.X = x;
    lpConsoleCurrentFontEx.dwFontSize.Y = y;
    lpConsoleCurrentFontEx.FontFamily = FF_DONTCARE;
    SetCurrentConsoleFontEx(CONSOLEHANDLE, false, &lpConsoleCurrentFontEx);
}




class Pen
{
public:
    static int dot(COORD pos,DWORD colour)
    {
        Pen dot(pos,colour,true);
        return 1;
    }
    static int drawFrame(COORD LUcorner, COORD RBcorner,DWORD colour)
    {
        Pen p(LUcorner,colour,true);
        p.vectorMove((RBcorner.X - LUcorner.X), 0);
        p.vectorMove(0, (RBcorner.Y - LUcorner.Y));
        p.vectorMove(-(RBcorner.X - LUcorner.X), 0);
        p.vectorMove(0, -(RBcorner.Y - LUcorner.Y));
        return 1;
    }
    static int drawRectangle(COORD LUcorner, COORD RBcorner, DWORD colour)
    {
        for (;(RBcorner.X-LUcorner.X)>=0&&(RBcorner.Y-LUcorner.Y)>=0;)
        {
            drawFrame({LUcorner.X++,LUcorner.Y++ }, { RBcorner.X--,RBcorner.Y--}, colour);
        }
        return 1;
    }

    //only convex figure
    //points in array must be in counterclockwise order
    static int drawFigure(COORD points[], DWORD colour, int numOfPoints)
    {
        for (int Q = 0; Q < numOfPoints; Q++)//цикл фигур
        {
            Pen penDrawer(points[Q], colour, true);
            Pen penIdk(points[(Q+1)% numOfPoints], NULL, false);
            for (int i = 2; i < numOfPoints; i++)//цикл фигуры
            {
                int numOfPixels = sqrt(pow(points[(Q+i)% numOfPoints].X - points[(Q+i - 1)%numOfPoints].X, 2) + pow(points[(Q + i) % numOfPoints].Y - points[(Q + i - 1) % numOfPoints].Y, 2));
                for (double k = 0; k <= numOfPixels; k += 1)//цикл линии
                {
                    penIdk.vectorMove(k * (points[(Q + i) % numOfPoints].X - points[(Q  -1 +i) % numOfPoints].X) / (numOfPixels), k * (points[(Q + i) % numOfPoints].Y - points[(Q - 1 + i) % numOfPoints].Y) / (numOfPixels));
                    penDrawer.moveToPoint(penIdk);
                    penDrawer.inverseStatus();
                    penDrawer.moveToPoint(points[Q]);
                    penIdk.moveToPoint(points[(Q - 1 + i) % numOfPoints]);
                    penDrawer.inverseStatus();
                }
            }

        }
        return 1;
    }

    int moveToPoint(Pen pnt)
    {
        vectorMove(pnt.getPos().X - getPos().X, pnt.getPos().Y - getPos().Y);
        return 1;
    }
    int moveToPoint(COORD pnt)
    {
        vectorMove(pnt.X - getPos().X, pnt.Y - getPos().Y);
        return 1;
    }

    Pen(const COORD pos, const DWORD colour, const bool status,char symb = ' ')
    {
        this->pos = pos;
        this->colour = colour;
        this->status = status;
        this->symb = symb;
        SetConsoleTextAttribute(CONSOLEHANDLE, colour);
        if (status)
        {
            SetConsoleCursorPosition(CONSOLEHANDLE, pos);
            cout << symb;

        }
        ConsoleDefaultColour();
    }
    Pen(const Pen &copyFrom):Pen(copyFrom.pos,copyFrom.colour,copyFrom.status,copyFrom.symb){}


    int vectorMove(const signed int X, const signed int Y)
    {
        //dont even ask me how it works 

        
        COORD posStart = pos;
        if (status &&(X!=0||Y!=0))
        {
            double a = 1;
            double b = 1;
            if (Y == 0)
            {
                a *= (X / abs(X));
                b = 0;
            }
            else if(X==0)
            {
                b *= (Y / abs(Y));
                a = 0;
            }
            else
            {
                a *= abs((double(X) / Y));//x
                b *= abs(1./a);//y
                if (a > 1)
                    a = 1;
                else if (b > 1)
                    b = 1;
                b *= (Y / abs(Y));
                a *= (X / abs(X));
            }
            double qw = pos.Y;//y
            double wq = pos.X;//x
            for (;pos.X != posStart.X + X || pos.Y != posStart.Y + Y;)
            {
                if (pos.X != posStart.X + X)
                    pos.X = wq;
                if (pos.Y != posStart.Y + Y)
                    pos.Y = qw;
                dot(pos, colour);
                wq += a;
                qw += b;
            }
        }
        pos = { SHORT(posStart.X + X),SHORT(posStart.Y + Y) };
        return 1;
    }


    bool getStatus()
    {
        return status;
    }
    DWORD getColour()
    {
        return colour;
    }
    COORD getPos()
    {
        return pos;
    }
    int changeColour(const DWORD colour)
    {
        this->colour = colour;
        return 1;
    }
    int inverseStatus()
    {
        status = !status;
        if (status)
        {
            dot(getPos(), colour);
        }
        ConsoleDefaultColour();
        return 1;
    }


private:
    COORD pos;
    DWORD colour;
    bool status;
    char symb;
    //1 - onscreen
    //0 - offscreen
};

typedef int(*func_ptr)(COORD,DWORD);
class PPObject
{
public:
    PPObject()
    {
        argsCOLOUR = nullptr;
        argsCOORD = nullptr;
        painters = nullptr;
        visible = true;
        size = 0;
    }
    ~PPObject()
    {
        this->hideSprite();
        delete[] argsCOLOUR;
        delete[] argsCOORD;
        delete[] painters;
        argsCOLOUR = nullptr;
        argsCOORD = nullptr;
        painters = nullptr;
    }
    //it will spawn hid
    PPObject(const PPObject& copyFrom)
    {
        this->size = copyFrom.size;
        this->visible = false;
        painters = new func_ptr[size];
        argsCOLOUR = new DWORD[size];
        argsCOORD = new COORD[size];
        for (int i = 0;i<size;i++)
        {
            this->argsCOORD[i] = copyFrom.argsCOORD[i];
            this->argsCOLOUR[i] = copyFrom.argsCOLOUR[i];
            this->painters[i] = copyFrom.painters[i];
            this->painterVisebility[i] = copyFrom.painterVisebility[i];
        }
    }

    //it will spawn hid
    void operator = (const PPObject& copyFrom)
    {
        this->hideSprite();
        if (argsCOLOUR != nullptr || argsCOORD != nullptr || painters != nullptr)
        {
            delete[] argsCOLOUR;
            delete[] argsCOORD;
            delete[] painters;
            delete[] painterVisebility;
            argsCOLOUR = nullptr;
            argsCOORD = nullptr;
            painters = nullptr;
            painterVisebility = nullptr;
        }
        this->size = copyFrom.size;
        this->visible = false;
        painters = new func_ptr[size];
        argsCOLOUR = new DWORD[size];
        argsCOORD = new COORD[size];
        painterVisebility = new bool[size];
        for (int i = 0; i < size; i++)
        {
            this->argsCOORD[i] = copyFrom.argsCOORD[i];
            this->argsCOLOUR[i] = copyFrom.argsCOLOUR[i];
            this->painters[i] = copyFrom.painters[i];
            this->painterVisebility[i] = copyFrom.painterVisebility[i];
        }

    }

    bool operator == (const PPObject& compare)
    {
        if (this->size != compare.size)
        {
            return false;
        }
        for (int i = 0;i<size;i++)
        {
            if (this->argsCOORD[i].X != compare.argsCOORD[i].X ||
                this->argsCOORD[i].Y != compare.argsCOORD[i].Y ||
                this->argsCOLOUR[i]  != compare.argsCOLOUR[i]  ||
                this->painters[i]    != compare.painters[i])
            {
                return false;
            }
        }
        return true;
    }
    bool operator != (const PPObject& compare)
    {
        return !(*this==compare);
    }

    PPObject operator + (const PPObject& plus)
    {
        PPObject res(*this);
        for (int i = 0;i<plus.size;i++)
        {
            res.addPainter(plus.painters[i], plus.argsCOORD[i], plus.argsCOLOUR[i]);
        }


        return res;
    }

    int changeAllColour(DWORD colour)
    {
        for (int i = 0;i<size;i++)
        {
            this->argsCOLOUR[i] = colour;
        }
        return 1;
    }
    int changeColour(const int painterId,DWORD colour)
    {
        this->argsCOLOUR[painterId] = colour;
        return 1;
    }



    int allVectorMove(const signed int X, const signed int Y)
    {
        bool buff = this->visible;
        if(visible)this->hideSprite();
        for (int i = 0;i<size;i++)
        {
            argsCOORD[i].X += X;
            argsCOORD[i].Y += Y;
            if(painterVisebility[i])painters[i](argsCOORD[i], argsCOLOUR[i]);
        }
        if(buff)this->showSprite();
        return 1;
    }

    int painterVectorMove(const int painterId,const signed int X, const signed int Y,bool erraseLastPos=true)
    {
        if (erraseLastPos && painterVisebility[painterId]) painters[painterId](argsCOORD[painterId],BGBlack);
        this->argsCOORD[painterId].X += X;
        this->argsCOORD[painterId].Y += Y;
        if (painterVisebility[painterId])painters[painterId](argsCOORD[painterId], argsCOLOUR[painterId]);
        return 1;
    }
    int refreshPainter(int painterId)
    {
        if (painterVisebility[painterId])
        {
            painters[painterId](argsCOORD[painterId], BGBlack);
            painters[painterId](argsCOORD[painterId], argsCOLOUR[painterId]);
        }
        return 1;
    }
    int inverseVisebilityPainter(int painterId)
    {
        if (painterVisebility[painterId])
        {
            painters[painterId](argsCOORD[painterId], BGBlack);
        }
        else
        {
            painters[painterId](argsCOORD[painterId], argsCOLOUR[painterId]);
        }
        painterVisebility[painterId] = !painterVisebility[painterId];
        return 1;
    }

    COORD getPainterCOORD(int painterId)
    {
        return argsCOORD[painterId];
    }


    int refresh()
    {
        if (visible)
        {
            this->hideSprite();
            this->showSprite();
        }
        return 1;
    }

    int swapPainters(int firstPainterID, int secondPainterID)
    {
        COORD buffCOORDArg = argsCOORD[firstPainterID];
        DWORD buffColourArg =  argsCOLOUR[firstPainterID];
        func_ptr buffPainter = painters[firstPainterID];
        bool buffPainterViseb = painterVisebility[firstPainterID];

        argsCOORD[firstPainterID] = argsCOORD[secondPainterID];
        argsCOLOUR[firstPainterID] = argsCOLOUR[secondPainterID];
        painters[firstPainterID] = painters[secondPainterID];
        painterVisebility[firstPainterID] = painterVisebility[secondPainterID];

        argsCOORD[secondPainterID] = buffCOORDArg;
        argsCOLOUR[secondPainterID] = buffColourArg;
        painters[secondPainterID] = buffPainter;
        painterVisebility[secondPainterID] = buffPainterViseb;
        return 1;
    }


    int deletePainter(int painterToDelId)
    {
        bool buffer = visible;
        if(visible)this->hideSprite();
        COORD* pNewCoord = new COORD[size - 1];
        DWORD* pNewColour = new DWORD[size - 1];
        func_ptr* pNewFuncs = new func_ptr[size - 1];
        for (int iO = 0, iN = 0;iO< size;iO++)
        {
            if (iO == painterToDelId)
            {
                continue;
            }
            *(pNewCoord + iN) = *(argsCOORD + iO);
            *(pNewColour + iN) = *(argsCOLOUR + iO);
            *(pNewFuncs + iN) = *(painters + iO);
            iN = iO;
        }
        delete[] argsCOORD;
        delete[] argsCOLOUR;
        delete[] painters;
        argsCOORD = pNewCoord;
        argsCOLOUR = pNewColour;
        painters = pNewFuncs;
        size--;
        if (buffer)this->showSprite();
        return 1;


    }
    //returns painterID
    int addPainter(func_ptr painterToAdd, COORD paramCOORD, DWORD paramCOLOUR,bool visebility = true)
    {
        COORD* pNewCoord = new COORD[size + 1];
        DWORD* pNewColour = new DWORD[size + 1];
        func_ptr* pNewFuncs = new func_ptr[size + 1];
        bool* pNewViseb = new bool[size + 1];

        for (int i = 0;i<size;i++)
        {
            pNewCoord[i] = argsCOORD[i];
            pNewColour[i] = argsCOLOUR[i];
            pNewFuncs[i] = painters[i];
            pNewViseb[i] = painterVisebility[i];
        }

        pNewCoord[size]  =  paramCOORD;
        pNewColour[size] =  paramCOLOUR;
        pNewFuncs[size]  =  painterToAdd;
        pNewViseb[size] = visebility;

        delete[] argsCOORD;
        delete[] argsCOLOUR;
        delete[] painters;
        delete[] painterVisebility;
        argsCOORD = pNewCoord;
        argsCOLOUR = pNewColour;
        painters = pNewFuncs;
        painterVisebility = pNewViseb;
        size++;
        if (visible) this->showSprite();
        return size-1;

    }
    int inverseVisebility()
    {
        if (visible)
        {
            hideSprite();
            
        }
        else
        {
            showSprite();
        }
        return 1;
    }
    int getPainterId(func_ptr painter, COORD paramCOORD, DWORD paramCOLOUR)
    {
        int ret = -1;
        for (int iO = 0; iO < size; iO++)
        {
            if (*(argsCOLOUR + iO) == paramCOLOUR &&
                (*(argsCOORD + iO)).X == paramCOORD.X &&
                (*(argsCOORD + iO)).Y == paramCOORD.Y &&
                *(painters + iO) == painter)
            {
                ret = iO;
                break;
            }

        }
        return ret;
    }
private:
    int showSprite()
    {
        for (int i = 0; i < size; i++)
        {
            if(painterVisebility[i])painters[i](argsCOORD[i], argsCOLOUR[i]);
        }
        visible = true;
        ConsoleDefaultColour();
        return 1;
    }
    int hideSprite()
    {
        for (int i = 0; i < size; i++)
        {
            painters[i](argsCOORD[i], BGBlack);
        }
        visible = false;
        ConsoleDefaultColour();
        return 1;
    }
    COORD* argsCOORD;
    DWORD* argsCOLOUR;
    func_ptr* painters;
    bool* painterVisebility;
    int size;
    bool visible;
    //array of pointers to functions contain functions that draw a sprite
    //they must have only 2 parameters 
    //1 COORD of "center" of a sprite part
    //2 colour of a sprite part
};










//resizes window, disbles cursor and generates seed of rand 
void prepare(COORD crd)
{
    srand(time(NULL));
    CONSOLE_CURSOR_INFO structCursorInfo;
    GetConsoleCursorInfo(CONSOLEHANDLE, &structCursorInfo);
    structCursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(CONSOLEHANDLE, &structCursorInfo);
    SMALL_RECT src = { 0, 0, crd.X, crd.Y };
    SetConsoleWindowInfo(CONSOLEHANDLE, true, &src);
    SetConsoleScreenBufferSize(CONSOLEHANDLE, crd);
}

int dot(const COORD pos, const DWORD colour)
{
    Pen p(pos, colour, true);
    return 1;
}
int snakeHeadVert(const COORD pos, const DWORD colour)
{
    Pen p(pos, colour, true, '"');
    return 1;
}
int snakeHeadHoriz(const COORD pos, const DWORD colour)
{
    Pen p(pos, colour, true, ':');
    // : for y movement
    // " for x movement
    return 1;
}
int snakeB(const COORD pos, const DWORD colour)
{
    Pen p(pos, colour, true);
    return 1;
}