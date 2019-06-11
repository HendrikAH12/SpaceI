#include <windows.h>
#include <gl/gl.h>

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);

int setaUp = 0, setaDown = 0, setaLeft = 0, setaRight = 0; //Variáveis para o reconhecimento de tecla
float posX = -0.5, posY = 0.5, border = 0.8, square_size = 0.05, move_speed = 0.01; //Setup do personagem e do mapa jogável

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
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
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = "GLSample";
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);;


    if (!RegisterClassEx(&wcex))
        return 0;

    /* create main window */
    hwnd = CreateWindowEx(0,
                          "GLSample",
                          "OpenGL Sample",
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT,
                          CW_USEDEFAULT,
                          800,
                          800,
                          NULL,
                          NULL,
                          hInstance,
                          NULL);

    ShowWindow(hwnd, nCmdShow);

    /* enable OpenGL for the window */
    EnableOpenGL(hwnd, &hDC, &hRC);

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
            /* OpenGL animation code goes here */

            glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            /*
            Controle da posição:
            - Se a tecla for pressionada, checar se o personagem está dentro da borda (quadrado de border x border de tamanho)
            - Se sim modificar a posX e/ou posY adicionando ou subtraindo move_speed, dependendo do necessário
            */

            if(setaRight && (posX >= -border && posX <= border))
                posX += move_speed;
            if(setaLeft && (posX >= -border && posX <= border))
                posX -= move_speed;
            if(setaUp && (posY >= -border && posY <= border))
                posY += move_speed;
            if(setaDown && (posY >= -border && posY <= border))
                posY -= move_speed;

            // Manter o personagem dentro das borders
            if(posX < -border)
                posX = -border;
            if(posX > border)
                posX = border;
            if(posY < -border)
                posY = -border;
            if(posY > border)
                posY = border;

            glPushMatrix();

            //Desenha o quadrado da área jogável
            glBegin(GL_QUADS);

                glColor3f(0.0f, 0.0f, 0.0f); glVertex2f(border + square_size, border + square_size);
                glColor3f(0.0f, 0.0f, 0.0f); glVertex2f(border + square_size, -border - square_size);
                glColor3f(0.0f, 0.0f, 0.0f); glVertex2f(-border - square_size, -border - square_size);
                glColor3f(0.0f, 0.0f, 0.0f); glVertex2f(-border - square_size, border + square_size);

            glEnd();

            //Desenha o personagem
            glBegin(GL_QUADS);

                glColor3f(1, 0.0f, 0.0f);   glVertex2f(posX + square_size, posY + square_size);
                glColor3f(0.0f, 1, 0.0f);   glVertex2f(posX + square_size, posY - square_size);
                glColor3f(0.0f, 0.0f, 1);   glVertex2f(posX - square_size, posY - square_size);
                glColor3f(1, 0.0f, 1);   glVertex2f(posX - square_size, posY + square_size);

            glEnd();

            glPushMatrix();

            glPopMatrix();

            SwapBuffers(hDC);
            Sleep (1);
        }
    }

    /* shutdown OpenGL */
    DisableOpenGL(hwnd, hDC, hRC);

    /* destroy the window explicitly */
    DestroyWindow(hwnd);

    return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_CLOSE:
            PostQuitMessage(0);
        break;

        case WM_DESTROY:
            return 0;

        case WM_KEYDOWN:
        {
            switch (wParam)
            {
                case VK_UP:
                    setaUp = 1;
                    break;

                case VK_DOWN:
                    setaDown = 1;
                    break;

                case VK_LEFT:
                    setaLeft = 1;
                    break;

                case VK_RIGHT:
                    setaRight = 1;
                    break;


                case VK_ESCAPE:
                    PostQuitMessage(0);
                    break;
            }
        }
        break;

        case WM_KEYUP:
        {
            switch(wParam)
            {
                case VK_UP:
                    setaUp = 0;
                    break;

                case VK_DOWN:
                    setaDown = 0;
                    break;

                case VK_LEFT:
                    setaLeft = 0;
                    break;

                case VK_RIGHT:
                    setaRight = 0;
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

void DisableOpenGL (HWND hwnd, HDC hDC, HGLRC hRC)
{
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hRC);
    ReleaseDC(hwnd, hDC);
}

