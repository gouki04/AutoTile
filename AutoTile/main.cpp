/*
** 本代码展示一种简单的自动地图元件的绘制原理
** 基本原理可以参考原文：http://www.codeproject.com/KB/game/Autotiles_Algorithm.aspx#_comments
** 实现了一个简单的地图编辑器，左键绘制，右键清除
** 如需更换地图元件的图片，更改 宏 TILESET_TEX_FILE 即可
**
** author : gouki04 2011-12-30
*/


#include "..\hge\hge.h"
#include "..\hge\hgesprite.h"

#define SAFE_DELETE(T) { if (T) { delete T; T = 0; } }

#define TILEWIDTH 32.f      // 地图元件宽
#define TILEHEIGHT 32.f     // 地图元件高

#define MAPROW 16   // 地图行数
#define MAPCOL 16   // 地图列数

#define TILESET_TEX_FILE "easyTile.png"     // 地图元件图片
#define HIGHLIGHT_TEX_FILE "highlight.png"  // 高亮框图片

#define MAP_LT_X 0  // 地图左上角x坐标
#define MAP_LT_Y 0  // 地图左上角y坐标

// HGE引擎
HGE *hge = 0;   

// 窗口宽度和高度
int screenWidth = static_cast<int>(MAP_LT_X + TILEWIDTH * MAPCOL);
int screenHeight = static_cast<int>(MAP_LT_Y + TILEHEIGHT * MAPROW);

// 高亮位置
int highlight_row = -1, highlight_col = -1;

// 高亮框
hgeSprite* highlight = 0;

// 16个地图元件
hgeSprite* easyTiles[16];

// 地图数据
typedef unsigned char TileType;
TileType easyMap[MAPROW][MAPCOL];

void drawLines()
{
    DWORD color = 0xFF808080;

    for (int i = 0; i < MAPROW; ++i)
    {
        hge->Gfx_RenderLine(MAP_LT_X, TILEHEIGHT * i, TILEWIDTH * MAPCOL, TILEHEIGHT * i, color);
    }

    for (int i = 0; i < MAPCOL; ++i)
    {
        hge->Gfx_RenderLine(TILEWIDTH * i, MAP_LT_Y, TILEWIDTH * i, TILEHEIGHT * MAPROW, color);
    }
}

void drawHighlight()
{
    if (highlight_row != -1 && highlight_col != -1)
    {
        int r = highlight_row;
        int c = highlight_col;

        highlight->Render(c * TILEWIDTH, r * TILEHEIGHT);
    }
}

void drawEasyMap()
{
    for (int i = 0; i < MAPROW; ++i)
    {
        for (int j = 0; j < MAPCOL; ++j)
        {
            TileType tile = easyMap[i][j];
            easyTiles[tile]->Render(MAP_LT_X + j * TILEWIDTH, MAP_LT_Y + i * TILEHEIGHT);
        }
    }
}

bool FrameFunc()
{
    if (hge->Input_GetKeyState(HGEK_ESCAPE)) 
        return true;

    // 更新鼠标状态
    float mx, my;
    hge->Input_GetMousePos(&mx, &my);

    // 更新高亮位置
    highlight_col = static_cast<int>((mx - MAP_LT_X) / TILEWIDTH);
    highlight_row = static_cast<int>((my - MAP_LT_Y) / TILEHEIGHT);

    if (highlight_col < 0|| highlight_col >= MAPCOL) highlight_col = -1;
    if (highlight_row < 0 || highlight_row >= MAPROW) highlight_row = -1;

    if (hge->Input_GetKeyState(HGEK_LBUTTON))
    {
        if (highlight_row != -1 && highlight_col != -1)
        {
            int r = highlight_row;
            int c = highlight_col;

            // 将中心点周围的16个小格填为1

            if (r > 0)
            {
                if (c > 0) easyMap[r - 1][c - 1] |= 0x8;    // 1000
                easyMap[r - 1][c] |= 0xC;   // 1100
                if (c < MAPCOL - 1) easyMap[r - 1][c + 1] |= 0x4;   // 0100
            }

            if (c > 0) easyMap[r][c - 1] |= 0xA;    // 1010
            easyMap[r][c] |= 0xF;   // 1111
            if (c < MAPCOL - 1) easyMap[r][c + 1] |= 0x5;   // 0101

            if (r < MAPROW - 1)
            {
                if (c > 0) easyMap[r + 1][c - 1] |= 0x2;    // 0010
                easyMap[r + 1][c] |= 0x3;   // 0011
                if (c < MAPCOL - 1) easyMap[r + 1][c + 1] |= 0x1;   // 0001
            }
        }
    }
    else if (hge->Input_GetKeyState(HGEK_RBUTTON))
    {
        if (highlight_row != -1 && highlight_col != -1)
        {
            int r = highlight_row;
            int c = highlight_col;

            // 将中心点周围的16个小格填为0

            if (r > 0)
            {
                if (c > 0) easyMap[r - 1][c - 1] &= ~0x8;   // 0111
                easyMap[r - 1][c] &= ~0xC;  // 0011
                if (c < MAPCOL - 1) easyMap[r - 1][c + 1] &= ~0x4;  // 1011
            }

            if (c > 0) easyMap[r][c - 1] &= ~0xA;   // 0101
            easyMap[r][c] &= ~0xF;  // 0000
            if (c < MAPCOL - 1) easyMap[r][c + 1] &= ~0x5;   // 1010

            if (r < MAPROW - 1)
            {
                if (c > 0) easyMap[r + 1][c - 1] &= ~0x2;   // 1101
                easyMap[r + 1][c] &= ~0x3;  // 1100
                if (c < MAPCOL - 1) easyMap[r + 1][c + 1] &= ~0x1;   // 1110
            }
        }
    }

    return false;
}

bool RenderFunc()
{
    hge->Gfx_BeginScene();
    hge->Gfx_Clear(0xFFFFFFFF);

    // 绘制地图
    drawEasyMap();

    // 绘制网格
    drawLines();

    // 绘制高亮框
    drawHighlight();

    hge->Gfx_EndScene();

    return false;
}

void loadContent()
{
    // 加载高亮框
    HTEXTURE tex = hge->Texture_Load(HIGHLIGHT_TEX_FILE);
    highlight = new hgeSprite(tex, 0, 0, 32, 32);
    highlight->SetColor(0x77FFFFFF);

    // 加载地图元件
    // 元件规格为512*32
    tex = hge->Texture_Load(TILESET_TEX_FILE);
    for (int i = 0; i < 16; ++i)
    {
        easyTiles[i] = new hgeSprite(tex, i * TILEWIDTH, 0.f, TILEWIDTH, TILEHEIGHT);
    }

    memset(easyMap, 0, sizeof(easyMap));
}

void unLoadContent()
{
    SAFE_DELETE(highlight);
    for (int i = 0; i < 16; ++i)
        SAFE_DELETE(easyTiles[i]);
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    hge = hgeCreate(HGE_VERSION);

    hge->System_SetState(HGE_FRAMEFUNC, FrameFunc);
    hge->System_SetState(HGE_RENDERFUNC, RenderFunc);
    hge->System_SetState(HGE_TITLE, "EasyAutoTile Editor");
    hge->System_SetState(HGE_SCREENWIDTH, screenWidth);
    hge->System_SetState(HGE_SCREENHEIGHT, screenHeight);
    hge->System_SetState(HGE_FPS, 60);
    hge->System_SetState(HGE_HIDEMOUSE, false);
    hge->System_SetState(HGE_WINDOWED, true);
    hge->System_SetState(HGE_USESOUND, false);
    hge->System_SetState(HGE_LOGFILE, "AutoTileEditor.log");
    hge->System_SetState(HGE_SHOWSPLASH, false);

    if(hge->System_Initiate())
    {
        loadContent();
        hge->System_Start();
    }
    else
    {	
        MessageBox(NULL, hge->System_GetErrorMessage(), "Error", MB_OK | MB_ICONERROR | MB_APPLMODAL);
    }

    unLoadContent();
    hge->System_Shutdown();

    hge->Release();

    return 0;
}