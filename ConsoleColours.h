#pragma once
//example SetConsoleTextAttribute(CONSOLEHANDLE, BGCoral|TWhite);
//BGCoral|TWhite	is like Coral background + White text
//BGBlue|TBlack		is like Blue  background + Black text


#define CONSOLEHANDLE GetStdHandle(STD_OUTPUT_HANDLE)




#define BGBlack         0
#define BGDarkBlue      1<<4
#define BGDarkGreen     2<<4
#define BGDarkGoluboy   3<<4
#define BGDarkRed       4<<4
#define BGDarkViolet    5<<4
#define BGDarkYellow    6<<4
#define BGGrey          7<<4
#define BGDarkGrey      8<<4
#define BGBlue          9<<4
#define BGGreen         10<<4
#define BGGoluboy       11<<4
#define BGCoral         12<<4
#define BGViolet        13<<4
#define BGYellow        14<<4
#define BGWhite         15<<4
#define BGRandom		(rand()%16)<<4

#define TBlack          0
#define TDarkBlue       1
#define TDarkGreen      2
#define TDarkGoluboy    3
#define TDarkRed        4
#define TDarkViolet     5
#define TDarkYellow     6
#define TGrey           7
#define TDarkGrey       8
#define TBlue           9
#define TGreen          10
#define TGoluboy        11
#define TCoral          12
#define TViolet         13
#define TYellow         14
#define TWhite          15
#define TRandom		(rand()%16)


void ConsoleDefaultColour()
{
	SetConsoleTextAttribute(CONSOLEHANDLE, BGBlack | TWhite);
}