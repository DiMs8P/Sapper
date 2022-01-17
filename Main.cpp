#include <windows.h>
#include <gl/gl.h>
#include <random>
#include <ctime>

#pragma comment (lib, "opengl32.lib")

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);
wchar_t* convertCharArrayToLPCWSTR(const char*);
void ShowCell();
void GameStart();
bool IsCellAround(int,int);
void ShowMine();
void ShowField();
void Line(float, float,float,float);
void PaintLines(int);
void TransCoord(const HWND& , float , float , float* , float* );
void PaintFlag();
void ShowEnpty(int, int);

typedef struct {
    bool mine;
    bool isopen ;
    bool flag;
    int cmaround;
}Cell;

bool flag = false;
#define WorH 15
#define WorW 15

Cell Field[WorH][WorW];
int mine = 20;

void TransCoord(const HWND& hwnd, float x, float y, float *x0, float *y0) {
    RECT rt;
    GetClientRect(hwnd, &rt);
    *x0 = x / rt.right * WorH;
    *y0 = WorW - y / rt.bottom * WorW;
}

void GameStart() {
    srand(time(0));
    for (int i = 0; i < WorH; i++) {
        for (int j = 0; j < WorW; j++) {
            Field[i][j].cmaround = 0;
            Field[i][j].mine = 0;
            Field[i][j].flag = 0;
            Field[i][j].isopen = 0;
        }
    }
    flag = false;
    for (int i = 0; i < mine; i++) {
        int x = rand() % WorH;
        int y = rand() % WorW;
        if (Field[x][y].mine)
            i--;
        else {
            Field[x][y].mine = true;

            for (int dx = -1; dx < 2; dx++) {
                for (int dy = -1; dy < 2; dy++) {
                    if (IsCellAround(x + dx, y + dy))
                        Field[x + dx][y + dy].cmaround++;
                }
            }
        }
    }
}

void ShowCell() {

    glBegin(GL_TRIANGLE_STRIP);

    glColor3f(0.8,0.8,0.8);
                            glVertex2f(0, 1);
    glColor3f(0.7, 0.7, 0.7);
                            glVertex2f(1, 1);
                            glVertex2f(0, 0);
    glColor3f(0.6, 0.6, 0.6);

                            glVertex2f(1, 0);
    glEnd();
}

void ShowOpenCell() {

    glBegin(GL_TRIANGLE_STRIP);

    glColor3f(0.3, 0.2, 0.3);
    glVertex2f(0, 1);
    glColor3f(0.4, 0.2, 0.7);
    glVertex2f(1, 1);
    glVertex2f(0, 0);
    glColor3f(0.2, 0.6, 0.6);

    glVertex2f(1, 0);
    glEnd();
}

void ShowMine() {
    glBegin(GL_TRIANGLE_STRIP);

    glColor3f(0, 0, 0);
    glVertex2f(0.25, 0.75);
    glVertex2f(0.75, 0.75);
    glVertex2f(0.25, 0.25);

    glVertex2f(0.75, 0.25);
    glEnd();
}

void ShowField() {
    glLoadIdentity();
    glTranslatef(-1, -1, 0);
    glScalef(2.0/WorH, 2.0/WorW, 1);
    for (int i = 0; i < WorH; i++) {
        for (int j = 0; j < WorW; j++) {
            glPushMatrix();
            glTranslatef(i, j, 0);
            if (Field[i][j].isopen) {
                ShowOpenCell();
                if (Field[i][j].mine)
                    ShowMine();
                else {
                    if (Field[i][j].cmaround)
                        PaintLines(Field[i][j].cmaround);
                }
            }
       
            else ShowCell();

            if (Field[i][j].flag && !Field[i][j].isopen) PaintFlag();

            glPopMatrix();
        }
    }
}

bool IsCellAround(int x, int y) {

    return (x >= 0) && (x < WorH) && (y >= 0) && (y < WorW);
}

void ShowEnpty(int x, int y) {
    if (!IsCellAround(x, y) || Field[x][y].isopen) return;
    Field[x][y].isopen = true;
    if (Field[x][y].cmaround == 0) 
        for (int dx = -1; dx < 2; dx++)
            for (int dy = -1; dy < 2; dy++)
                ShowEnpty(x + dx, y + dy);
    if (Field[x][y].mine) {
        flag = true;
        for (int i = 0; i < WorH; i++) {
            for (int j = 0; j < WorW; j++) {
                Field[i][j].isopen = true;
            }
        }
    }
}

void PaintLines(int a) {
    glLineWidth(3);
    glColor3f(1, 1, 0);
    glBegin(GL_LINES);
    if ((a != 5) && (a != 6)) Line(0.65, 0.85, 0.65, 0.5);
    if (a != 2) Line(0.65,0.15,0.65,0.5);
    if ((a != 1) && (a != 4)) Line(0.35,0.85,0.65,0.85);
    if (a != 1 && a != 2 && a != 3 && a != 7) Line(0.35,0.5,0.35,0.85);
    if ((a == 2) || (a == 6) || (a == 8) || (a == 0)) Line(0.35, 0.15, 0.35, 0.5);
    if (a != 1 && a != 4 && a != 7) Line(0.35,0.15, 0.65,0.15);
    if (a != 0 && a != 1 && a != 7) Line(0.35, 0.5, 0.65, 0.5);

    glEnd();
}

void PaintFlag() {

    glLineWidth(3);
    glColor3f(0, 1, 1);

    glBegin(GL_LINES);

    Line(0.35, 0.15, 0.35, 0.85);

    glEnd();
    glBegin(GL_TRIANGLES);

    glVertex2f(0.35, 0.15);
    glVertex2f(0.35, 0.85);
    glVertex2f(0.75, 0.5);

    glEnd();

}

void Line(float x1, float y1, float x2, float y2) {
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
}

int main()
{

    WNDCLASSEX wcex;
    HWND hwnd;
    HDC hDC;
    HGLRC hRC;
    MSG msg;
    BOOL bQuit = FALSE;

    /* register window class */
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_OWNDC;
    wcex.lpfnWndProc = WindowProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = convertCharArrayToLPCWSTR("GLSample");
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);


    if (!RegisterClassEx(&wcex))
        return 0;

    /* create main window */
    hwnd = CreateWindowEx(0,
        convertCharArrayToLPCWSTR("GLSample"),
        convertCharArrayToLPCWSTR("ShishaHEHEHE"),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        500,
        500,
        NULL,
        NULL,
        NULL,
        NULL);

    ShowWindow(hwnd, SW_SHOWNORMAL);

    /* enable OpenGL for the window */
    EnableOpenGL(hwnd, &hDC, &hRC);

    GameStart();

    /* program main loop */
    while (!bQuit)
    {
        /* check for messages */
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            /* handle or dispatch messages */
            if (msg.message == WM_QUIT)
            {
                bQuit = TRUE;
            }
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {
            
            glClearColor(0,0,0,0);
            glClear(GL_COLOR_BUFFER_BIT);


            ShowField();

            SwapBuffers(hDC);
            Sleep(1);
        }
    }

    /* shutdown OpenGL */
    DisableOpenGL(hwnd, hDC, hRC);

    /* destroy the window explicitly */
    DestroyWindow(hwnd);

    return msg.wParam;
}

wchar_t* convertCharArrayToLPCWSTR(const char* charArray)
{
    wchar_t* wString = new wchar_t[4096];
    MultiByteToWideChar(CP_ACP, 0, charArray, -1, wString, 4096);
    return wString;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CLOSE:
        PostQuitMessage(0);
        break;

    case WM_LBUTTONDOWN:
    {
        if (!flag) {
            POINTFLOAT pt;
            TransCoord(hwnd, LOWORD(lParam), HIWORD(lParam), &pt.x, &pt.y);
            int x = (int)pt.x;
            int y = (int)pt.y;
            if (IsCellAround(x, y) && !Field[x][y].flag)
                ShowEnpty(x, y);
        }
        else { GameStart(); }
        break;
    }
    case WM_RBUTTONDOWN: {
        POINTFLOAT pt;
        TransCoord(hwnd, LOWORD(lParam), HIWORD(lParam), &pt.x, &pt.y);
        int x = (int)pt.x;
        int y = (int)pt.y;
        if (IsCellAround(x, y))
            Field[x][y].flag = !Field[x][y].flag;
    }

    case WM_DESTROY:
        return 0;

    case WM_KEYDOWN:
    {
        switch (wParam)
        {
        case VK_ESCAPE:
            PostQuitMessage(0);
            break;
        }
    }
    break;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}

void EnableOpenGL(HWND hwnd, HDC* hDC, HGLRC* hRC)
{
    PIXELFORMATDESCRIPTOR pfd;

    int iFormat;
    /* get the device context (DC) */
    *hDC = GetDC(hwnd);

    /* set the pixel format for the DC */
    ZeroMemory(&pfd, sizeof(pfd));

    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW |
        PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;

    iFormat = ChoosePixelFormat(*hDC, &pfd);

    SetPixelFormat(*hDC, iFormat, &pfd);

    /* create and enable the render context (RC) */
    *hRC = wglCreateContext(*hDC);

    wglMakeCurrent(*hDC, *hRC);
    
}


void DisableOpenGL(HWND hwnd, HDC hDC, HGLRC hRC)
{
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hRC);
    ReleaseDC(hwnd, hDC);
}

