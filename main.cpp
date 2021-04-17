/*
  Name: Minesweeper
  Author: SG57
  Description:
    Clone of Windows Minesweeper for my own pleasure
*/


#include <vector>
#include <oslib/oslib.h>

PSP_MODULE_INFO("Minesweeper", 1, 1, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);

using namespace std;

const int SCREEN_WIDTH = 480;
const int SCREEN_HEIGHT = 272;
const int ANALOG_SENSITIVITY = 80;

const int EASY = 0;
const int INTERMEDIATE = 1;
const int EXPERT = 2;
const int CUSTOM = 3;

const int cWIDTH = 0;
const int cHEIGHT = 1;
const int MINE = 2;

int DIMENSIONS[4][2] = {
                              {9,9}, // easy
                              {16,16}, // intermediate
                              {30,16},  // expert
                              {1, 1}  // custom
};

int MINES[4] = {
                      10, // easy
                      40, // intermediate
                      99, // expert
                      1   // custom
};

const int X = 0;
const int Y = 1;

int iDifficulty;
int iCursorX;
int iCursorY;
int iFlagged;
int iGridX;
int iGridY;

bool bLost;
bool bWon;

void Reset();

bool Custom();

class cPlace
{
  private:
    int iElementX;
    int iElementY;
    int iMinesAround;
    bool bMine;
    bool bActivated;
    bool bFlagged;
          
  public:    
    cPlace() : iElementX(0), iElementY(0), iMinesAround(0), bMine(false), bActivated(false), bFlagged(false)
    {}
    ~cPlace()
    {}
    
    const int getElementX()
    {
      return iElementX;      
    }
    const int getElementY()
    {
      return iElementY;
    }
    const bool isMine()
    {
      return bMine;      
    }
    const bool isFlagged()
    {
      return bFlagged;      
    }
    const bool isActivated()
    {
      return bActivated;      
    }
    
    void setFlagged(bool flag)
    {
      bFlagged = flag;     
    }
    
    void Initialize(int x, int y, bool mine)
    {
      iElementX = x;
      iElementY = y;
      bMine = mine;
      bActivated = false;
      iMinesAround = 0;
      bFlagged = false;
    }
    
    void getMinesAround();
    void Activate();
    
    void Draw()
    {
      int x, y;

      x = iGridX - ((DIMENSIONS[iDifficulty][X]/2 - iElementX) * 10);
      y = iGridY - ((DIMENSIONS[iDifficulty][Y]/2 - iElementY) * 10);

      if (bActivated || (bLost && bMine))
      {
        oslDrawRect(x, y, x+10, y+10, (bMine) ? RGB(255,0,0) : RGB(64,64,64));
         
        char symbol = ' ';
        
        if (bMine)
        {
          symbol = '*';   
        }
        else if (iMinesAround == 1)
        {
          oslSetTextColor(RGB(0,0,255));
          symbol = '1';     
        }
        else if (iMinesAround == 2)
        {
          oslSetTextColor(RGB(0,210,0));
          symbol = '2';     
        }
        else if (iMinesAround == 3)
        {
          oslSetTextColor(RGB(255,0,0));
          symbol = '3';     
        }
        else if (iMinesAround == 4)
        {
          oslSetTextColor(RGB(0,0,126));
          symbol = '4';     
        }
        else if (iMinesAround == 5)
        {
          oslSetTextColor(RGB(140,0,0));
          symbol = '5';     
        }
        else if (iMinesAround == 6)
        {
          oslSetTextColor(RGB(0,128,255));
          symbol = '6';     
        }
        else if (iMinesAround == 7)
        {
          oslSetTextColor(RGB(0,0,0));
          symbol = '7';     
        }
        else if (iMinesAround == 8)
        {
          oslSetTextColor(RGB(100,100,100));
          symbol = '8';     
        }
      
        oslDrawChar(x+3,y+2,symbol);
        
        oslSetTextColor(RGB(0,0,0));
      }
      else
      {
        oslDrawFillRect(x+1, y+1, x+9, y+9, RGB(127,127,127));
        oslDrawLine(x,y+10,x+10,y+10, RGB(64,64,64));
        oslDrawLine(x+10,y,x+10,y+10, RGB(64,64,64));
        oslDrawLine(x,y,x+10,y, RGB(190,190,190));
        oslDrawLine(x,y,x,y+10, RGB(190,190,190));
      }
      
      if (!bActivated || bLost)
      {
        if (bFlagged)
        {
          oslDrawChar(x+3,y+2,'F');
        }   
      }
    }
};

vector< vector<cPlace> > Place;

void cPlace::getMinesAround()
{
  iMinesAround = 0;
  
  if (iElementX > 0)
  {
    if (Place[iElementY][iElementX-1].isMine())
    {
      iMinesAround++;   
    }

    if (iElementY > 0)
    {
      if (Place[iElementY-1][iElementX-1].isMine())
      {
        iMinesAround++;   
      }
    }
    if (iElementY < Place.size()-1)
    {
      if (Place[iElementY+1][iElementX-1].isMine())
      {
        iMinesAround++;
      }
    }
  }
  if (iElementX < Place[0].size()-1)
  {
    if (Place[iElementY][iElementX+1].isMine())
    {
      iMinesAround++;   
    }

    if (iElementY > 0)
    {
      if (Place[iElementY-1][iElementX+1].isMine())
      {
        iMinesAround++;   
      }
    }
    if (iElementY < Place.size()-1)
    {
      if (Place[iElementY+1][iElementX+1].isMine())
      {
        iMinesAround++;
      }
    }
  }
  if (iElementY > 0)
  {
    if (Place[iElementY-1][iElementX].isMine())
    {
      iMinesAround++;   
    }
  }
  if (iElementY < Place.size()-1)
  {
    if (Place[iElementY+1][iElementX].isMine())
    {
      iMinesAround++;
    }
  }
    
  if (iMinesAround == 0)
  {
    if (iElementX > 0)
    {
      Place[iElementY][iElementX-1].Activate();
      
      if (iElementY > 0)
      {
        Place[iElementY-1][iElementX-1].Activate();
      }
      if (iElementY < Place.size()-1)
      {
        Place[iElementY+1][iElementX-1].Activate();
      }
    }
    if (iElementX < Place[0].size()-1)
    {
      Place[iElementY][iElementX+1].Activate();
  
      if (iElementY > 0)
      {
        Place[iElementY-1][iElementX+1].Activate();
      }
      if (iElementY < Place.size()-1)
      {
        Place[iElementY+1][iElementX+1].Activate();  
      }
    }
    if (iElementY > 0)
    {
      Place[iElementY-1][iElementX].Activate();
    }
    if (iElementY < Place.size()-1)
    {
      Place[iElementY+1][iElementX].Activate();
    }
  }
}

void cPlace::Activate()
{
  if (bActivated || bFlagged)
    return;

  bActivated = true;
  
  if (bMine)
  {
    bLost = true;
  }
  else
  { 
    getMinesAround();
  }
}

void Reset()
{
  if(oslMessageBox("Would you like to play on Easy?",
                   "Easy",
                   oslMake3Buttons(OSL_KEY_CROSS, OSL_MB_YES,
                                   OSL_KEY_CIRCLE, OSL_MB_NO, 0, 0)) == OSL_MB_YES)
  {
    iDifficulty = EASY;
  }
  else if(oslMessageBox("How about Intermediate?",
                   "Intermediate",
                   oslMake3Buttons(OSL_KEY_CROSS, OSL_MB_YES,
                                   OSL_KEY_CIRCLE, OSL_MB_NO, 0, 0)) == OSL_MB_YES)
  {
    iDifficulty = INTERMEDIATE;
  }
  else if(oslMessageBox("Oh really...\n\nAre you man enough for Expert?",
                   "Expert",
                   oslMake3Buttons(OSL_KEY_CROSS, OSL_MB_YES,
                                   OSL_KEY_CIRCLE, OSL_MB_NO, 0, 0)) == OSL_MB_YES)
  {
    iDifficulty = EXPERT;
  }
  else if(Custom())
  {
    iDifficulty = CUSTOM;
  }
  else
  {
    Reset();
    return;  
  }
  
  int mine[MINES[iDifficulty]][2];
  for (int i = 0; i < MINES[iDifficulty]; i++)
  {
    mine[i][X] = rand() % DIMENSIONS[iDifficulty][X];
    mine[i][Y] = rand() % DIMENSIONS[iDifficulty][Y];
    
    for (int j = 0; j < MINES[iDifficulty]; j++)
    {
      if (i != j)
      {
        if (mine[i][X] == mine[j][X] && mine[i][Y] == mine[j][Y])
        {
          mine[i][X] = rand() % DIMENSIONS[iDifficulty][X];
          mine[i][Y] = rand() % DIMENSIONS[iDifficulty][Y];
           
          j = -1;
        }   
      }   
    }
  }
  
  if (Place.size() > 0)
  {
    for (int y = 0; y < (int) Place.size(); y++)
      if (Place[y].size() > 0)
        Place[y].clear();

    Place.clear();
  }
  
  for (int y = 0; y < DIMENSIONS[iDifficulty][Y]; y++)
  {
    Place.push_back(vector<cPlace>());
    
    for (int x = 0; x < DIMENSIONS[iDifficulty][X]; x++)
    {
      bool isMine = false;
      for (int i = 0; i < MINES[iDifficulty]; i++)
      {
        if (mine[i][X] == x && mine[i][Y] == y)
        {
          isMine = true;
          break;   
        }
      }
      
      Place[y].push_back(cPlace());
      
      Place[y][x].Initialize(x, y, isMine);
    }
  }
    
  bLost = false;
  bWon = false;
  iFlagged = 0;
  iGridX = SCREEN_WIDTH/2;
  iGridY = SCREEN_HEIGHT/2;
}

void oslUIDrawButtonFrame(int x0, int y0, int w, int h, int state)
{
	#define x1 (x0+w)
	#define y1 (y0+h)
	//Cadre gris
//	if (state == 1)
//		oslDrawFillRect(x0,y0,x1,y1,RGB(192,192,192));
//	else
		oslDrawGradientRect(x0,y0,x1,y1, RGB(208,208,208), RGB(188,188,188), RGB(188,188,188), RGB(168, 168, 168));
	if (state == 1)		{
		//Gris en haut à gauche
		oslDrawRect(x0,y0,x1,y1,RGB(128,128,128));
		x0++, y0++, w-=2, h-=2;
	}
	//Cadre noir en bas à droite
	oslDrawLine(x1-1,y0,x1-1,y1,RGB(0,0,0));
	oslDrawLine(x0,y1-1,x1,y1-1,RGB(0,0,0));
	//Ombre grise en bas à droite
	oslDrawLine(x1-2,y0+1,x1-2,y1-1,RGB(128,128,128));
	oslDrawLine(x0+1,y1-2,x1-2,y1-2,RGB(128,128,128));
	//Reflet blanc en haut à gauche
	oslDrawLine(x0+1,y0+1,x1-2,y0+1,RGB(255,255,255));
	oslDrawLine(x0+1,y0+1,x0+1,y1-2,RGB(255,255,255));
	#undef x1
	#undef y1
}

bool Custom()
{
	const int x0=240-150, y0=136-60, x1=240+150, y1=136+60;
	int btX, variable[3], option;
  OSL_IMAGE *curBuf = oslGetDrawBuffer();

  variable[cWIDTH] = DIMENSIONS[CUSTOM][X];
  variable[cHEIGHT] = DIMENSIONS[CUSTOM][Y];
  variable[MINE] = MINES[CUSTOM];
  
  option = cWIDTH;

	oslSetDrawBuffer(OSL_SECONDARY_BUFFER);

	while(!osl_quit)
  {
    oslStartDrawing();

  	oslUIDrawButtonFrame(x0,y0,x1-x0,y1-y0,0);

  	oslDrawGradientRect(x0+3, y0+3, x1-3, y0+3+13, RGB(0,0,128), RGB(16,132,208), RGB(0,0,128), RGB(16,132,208));

  	oslSetBkColor(RGBA(0,0,0,0));
  	oslSetTextColor(RGB(255,255,255));
  	oslDrawString(x0+8, y0+6, "Custom");

    char text[30];

    if (option == cWIDTH)
      oslSetTextColor(RGB(0,0,255));
    else
      oslSetTextColor(RGB(0,0,0));
  	
  	snprintf(text,30,"Width:   %i", variable[cWIDTH]);
  	oslDrawTextBox(x0+8, y0+22, x1-4, y1-30, text, 0);

    if (option == cHEIGHT)
      oslSetTextColor(RGB(0,0,255));
    else
      oslSetTextColor(RGB(0,0,0));

  	snprintf(text,30,"Height:  %i", variable[cHEIGHT]);
  	oslDrawTextBox(x0+8, y0+32, x1-4, y1-20, text, 0);

    if (option == MINE)
      oslSetTextColor(RGB(0,0,255));
    else
      oslSetTextColor(RGB(0,0,0));

  	snprintf(text,30,"Mines:   %i", variable[MINE]);
  	oslDrawTextBox(x0+8, y0+42, x1-4, y1-10, text, 0);
  	
  	oslSetTextColor(RGB(0,0,0));

  	btX = ((x0+x1)/2)-95;

  	oslUIDrawButtonFrame(btX, y1-30, 80, 21, 1);
  	oslDrawString(btX+10,y1-23,"X Yes");

  	oslUIDrawButtonFrame(btX+90, y1-30, 80, 21, 1);
  	oslDrawString(btX+100,y1-23,"O No");

    oslEndDrawing();
    oslSyncFrameEx(0,0,0);

		oslReadKeys();

		if (osl_keys->pressed.cross)
    {
      if (variable[MINE] > variable[cWIDTH]*variable[cHEIGHT])
      {
        oslMessageBox("Too many mines for the desired dimensions of the minefield!","Error",oslMake3Buttons(OSL_KEY_CROSS, OSL_MB_OK, 0, 0, 0, 0));
      }
      else
      {
        DIMENSIONS[CUSTOM][X] = variable[cWIDTH];
        DIMENSIONS[CUSTOM][Y] = variable[cHEIGHT];
        MINES[CUSTOM] = variable[MINE];
        
		    oslSetDrawBuffer(curBuf);
		    oslReadKeys();
        return true;
      }
		}
		else if (osl_keys->pressed.circle)
		{
		  oslSetDrawBuffer(curBuf);
		  oslReadKeys();
      return false;
    }
    
    if (osl_keys->pressed.up)
    {
      option--;
      if (option < cWIDTH)
        option = MINE;
    }
    else if (osl_keys->pressed.down)
    {
      option++;
      if (option > MINE)
        option = cWIDTH;
    }
    
    if (osl_keys->held.right)
    {
      variable[option]++;
    }
    else if (osl_keys->held.left)
    {
      variable[option]--;
      if (variable[option] < 1)
        variable[option] = 1;
    }
	}
	
	return false;
}


void Draw()
{
  oslStartDrawing();
  oslDrawGradientRect(0, 0, 480, 272,
                      RGB(200,200,200), RGB(126,126,126), RGB(126,126,126), RGB(64,64,64));

  for (int y = 0; y < (int) Place.size(); y++)
  {
    for (int x = 0; x < (int) Place[y].size(); x++)
    {
      Place[y][x].Draw();
    }
  }
  
  oslDrawLine(iCursorX+1, iCursorY+1, iCursorX+6, iCursorY+6, RGB(0,0,0));
  oslDrawLine(iCursorX+1, iCursorY+1, iCursorX+6, iCursorY+1, RGB(0,0,0));
  oslDrawLine(iCursorX+1, iCursorY+1, iCursorX+1, iCursorY+6, RGB(0,0,0));
  
  oslDrawLine(iCursorX, iCursorY, iCursorX+5, iCursorY+5, RGB(255,255,255));
  oslDrawLine(iCursorX, iCursorY, iCursorX+5, iCursorY, RGB(255,255,255));
  oslDrawLine(iCursorX, iCursorY, iCursorX, iCursorY+5, RGB(255,255,255));
  
  char str[30];
  snprintf(str,30,"Mines: %i", MINES[iDifficulty] - iFlagged);
  oslDrawString(2,2,str);

  oslDrawString(212,4,"Minesweeper");
  oslDrawString(160,260,"Reset --------> START");
  
  oslEndDrawing();
  oslSyncFrameEx(0,0,0);
}

void Input()
{
  oslReadKeys(); 
  
  if (osl_keys->held.start)
  {
    Reset();
  }
  
  if (osl_keys->pressed.L)
  {
    iGridX -= 10;
  }
  else if (osl_keys->pressed.R)
  {
    iGridX += 10;
  }

  if (osl_keys->pressed.triangle)
  {
    iGridY -= 10;
  }
  else if (osl_keys->pressed.circle)
  {
    iGridY += 10;
  }
  
  if (osl_keys->analogX < -ANALOG_SENSITIVITY && iCursorX > 0)
  {
    iCursorX -= (oslAbs(osl_keys->analogX) / 40);
  }
  else if (osl_keys->analogX > ANALOG_SENSITIVITY && iCursorX < SCREEN_WIDTH)
  {
    iCursorX += (oslAbs(osl_keys->analogX) / 40);
  }
  
  if (osl_keys->analogY < -ANALOG_SENSITIVITY && iCursorY > 0)
  {
    iCursorY -= (oslAbs(osl_keys->analogY) / 40);
  }
  else if (osl_keys->analogY > ANALOG_SENSITIVITY && iCursorY < SCREEN_HEIGHT)
  {
    iCursorY += (oslAbs(osl_keys->analogY) / 40);
  }
  
  if (osl_keys->held.left && iCursorX > 0)
  {
    iCursorX--;
  }
  else if (osl_keys->held.right && iCursorX < SCREEN_WIDTH)
  {
    iCursorX++;     
  }
  if (osl_keys->held.up && iCursorY > 0)
  {
    iCursorY--;
  }
  else if (osl_keys->held.down && iCursorY < SCREEN_HEIGHT)
  {
    iCursorY++;     
  }
  
  if (osl_keys->pressed.cross && !bLost && !bWon)
  {
    int x, y;
                      
    x = (iCursorX - (iGridX - ((DIMENSIONS[iDifficulty][X]/2) * 10))) / 10;
    y = (iCursorY - (iGridY - ((DIMENSIONS[iDifficulty][Y]/2) * 10))) / 10;
                      
    if (x >= 0 && x < DIMENSIONS[iDifficulty][X] &&
        y >= 0 && y < DIMENSIONS[iDifficulty][Y])
    {
      if (!Place[y][x].isFlagged())
        Place[y][x].Activate();

      return;
    }
  }
  else if (osl_keys->pressed.square && !bLost && !bWon)
  {
    int x, y;

    x = (iCursorX - (iGridX - ((DIMENSIONS[iDifficulty][X]/2) * 10))) / 10;
    y = (iCursorY - (iGridY - ((DIMENSIONS[iDifficulty][Y]/2) * 10))) / 10;

    if (x >= 0 && x < DIMENSIONS[iDifficulty][X] &&
        y >= 0 && y < DIMENSIONS[iDifficulty][Y])
    {
      if (!Place[y][x].isActivated())
      {
        Place[y][x].setFlagged(!Place[y][x].isFlagged());
        if (Place[y][x].isFlagged())
        {
          iFlagged++;

          if (iFlagged == MINES[iDifficulty])
          {
            int rightFlagged = 0;

            for (int yy = 0; yy < (int) Place.size(); yy++)
            {
              for (int xx = 0; xx < (int) Place[y].size(); xx++)
              {
                if (Place[yy][xx].isMine() && Place[yy][xx].isFlagged())
                {
                  rightFlagged++;
                }
              }
            }

            if (rightFlagged == MINES[iDifficulty])
            {
              for (int yy = 0; yy < (int) Place.size(); yy++)
              {
                for (int xx = 0; xx < (int) Place[yy].size(); xx++)
                {
                  if (!Place[yy][xx].isMine() && !Place[yy][xx].isFlagged())
                  {
                    Place[yy][xx].Activate();
                  }
                }
              }

              oslMessageBox("To think you could've spent all that time doing something a bit more meaningful...","Winner!",oslMake3Buttons(OSL_KEY_CROSS, OSL_MB_OK, 0, 0, 0, 0));

              bWon = true;
            }
          }
        }
        else
        {
          iFlagged--;
        }
      }

      return;
    }
  }
}

int main(int argc, char* argv[])
{
 	oslInit(0);
	oslInitGfx(OSL_PF_8888, 1);
  oslInitConsole();

  srand(time(NULL));
  
  oslShowSplashScreen1();
  
  OSL_IMAGE *background = oslLoadImageFilePNG("neoflash.png", OSL_IN_RAM | OSL_SWIZZLED, OSL_PF_5650);

  while (!osl_quit){
        oslReadKeys();
        if (osl_keys->pressed.cross)
          break;

        oslStartDrawing();

        oslDrawImage(background);

        oslEndDrawing();
        oslSyncFrame();
  }

  if (background != NULL)
    oslDeleteImage(background);

  Reset();

  while(!osl_quit)
  {
    Input();
    Draw();  
  }

  oslDeinitAudio();
  oslEndGfx();
	oslQuit();
	
  return 0;
}
