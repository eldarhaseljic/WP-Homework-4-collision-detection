#if defined(UNICODE) && !defined(_UNICODE)
#define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
#define UNICODE
#endif

#define ID_TIMER 1
#include <tchar.h>
#include <windows.h>
#include <stdio.h>
#include <list>

void draw (HDC hdc,RECT * rect);
void movestuff (RECT * rect);
void fire (int num);
void make_snowflake (RECT * rect);

typedef struct ObjectInfo
{
    int x,y;
    int width,height;
    int dx,dy;
} Object;

std::list<Object *> snowflakes, bullets;

int snowflakespeed[] = {7,12,17,22,30};

Object background, man, snowflake, bullet;

HBITMAP hbmbackground;
HBITMAP hbmman, hbmmanblack;
HBITMAP hbmsnowflake,hbmsnowflakeblack;
HBITMAP hbmbullet,hbmbulletblack;

/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);

/*  Make the class name into a global variable  */
TCHAR szClassName[ ] = _T("CodeBlocksWindowsApp");

int WINAPI WinMain (HINSTANCE hThisInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpszArgument,
                    int nCmdShow)
{
    HWND hwnd;               /* This is the handle for our window */
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */

    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
    wincl.cbSize = sizeof (WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;                 /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use Windows's default colour as the background of the window */
    wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl))
        return 0;

    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx (
               0,                   /* Extended possibilites for variation */
               szClassName,         /* Classname */
               _T("eldar_haseljic_zadaca4"),       /* Title Text */
               WS_OVERLAPPEDWINDOW, /* default window */
               CW_USEDEFAULT,       /* Windows decides the position */
               CW_USEDEFAULT,       /* where the window ends up on the screen */
               544,                 /* The programs width */
               375,                 /* and height in pixels */
               HWND_DESKTOP,        /* The window is a child-window to desktop */
               NULL,                /* No menu */
               hThisInstance,       /* Program Instance handler */
               NULL                 /* No Window Creation data */
           );

    /* Make the window visible on the screen */
    ShowWindow (hwnd, nCmdShow);

    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage (&messages, NULL, 0, 0))
    {
        /* Translate virtual-key messages into character messages */
        TranslateMessage(&messages);
        /* Send message to WindowProcedure */
        DispatchMessage(&messages);
    }

    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return messages.wParam;
}


/*  This function is called by the Windows function DispatchMessage()  */

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    RECT rect;

    switch (message)                  /* handle the messages */
    {
    case WM_CREATE:
    {
        BITMAP bitmap;
        GetClientRect(hwnd,&rect);

        unsigned int time = SetTimer(hwnd,ID_TIMER, 55 , NULL);
        hbmbackground = (HBITMAP)LoadImage(NULL,"black.bmp",IMAGE_BITMAP,rect.right,rect.bottom,LR_LOADFROMFILE);
        hbmman = (HBITMAP)LoadImage(NULL,"man.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
        hbmmanblack = (HBITMAP)LoadImage(NULL,"manblack.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
        hbmbullet = (HBITMAP)LoadImage(NULL,"bullet.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
        hbmbulletblack = (HBITMAP)LoadImage(NULL,"bulletblack.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
        hbmsnowflake = (HBITMAP)LoadImage(NULL,"snowflake.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
        hbmsnowflakeblack = (HBITMAP)LoadImage(NULL,"snowflakeblack.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE);

        GetObject(hbmbackground,sizeof(BITMAP),&bitmap);
        background.width = bitmap.bmWidth;
        background.height = bitmap.bmHeight;

        GetObject(hbmman,sizeof(BITMAP),&bitmap);
        man.width = bitmap.bmWidth/7;
        man.height = bitmap.bmHeight;
        man.dx = 0;
        man.x = 0;
        man.y = rect.bottom - bitmap.bmHeight;

        GetObject(hbmsnowflake,sizeof(BITMAP),&bitmap);
        snowflake.width = bitmap.bmWidth/11;
        snowflake.height = bitmap.bmHeight;
        snowflake.y = -bitmap.bmHeight;

        GetObject(hbmbullet, sizeof(BITMAP), &bitmap);
        bullet.width = bitmap.bmWidth;
        bullet.height = bitmap.bmHeight;
        break;
    }
    case WM_TIMER:
    {
        HDC hdc = GetDC(hwnd);
        GetClientRect(hwnd,&rect);
        make_snowflake(&rect);
        movestuff(&rect);
        draw (hdc,&rect);
        ReleaseDC(hwnd,hdc);
        break;
    }
    case WM_RBUTTONDOWN:
    {
        fire(2);
        break;
    }
    case WM_LBUTTONDOWN:
    {
        fire(1);
        break;
    }
    case WM_MOUSEMOVE:
    {
        POINT p;
        GetCursorPos(&p);
        ScreenToClient(hwnd,&p);
        man.x = p.x - man.width/2;
        man.y = p.y - man.height/2;
        ShowCursor(FALSE);
        break;
    }
    case WM_KEYDOWN:
    {
        GetClientRect(hwnd,&rect);
        switch(wParam)
        {
        case VK_SPACE:
            if((GetAsyncKeyState(VK_RSHIFT) & 0x8000) || (GetAsyncKeyState(VK_LSHIFT) & 0x8000))
            {
                fire(2);
            }
            else
            {
                fire(1);
            }
                break;
        case VK_RIGHT:
            if(man.x < rect.right - man.width)
                man.x += 4;
            break;
        case VK_LEFT:
            if(man.x > rect.left )
                man.x -= 4;
            ++man.dx;
            if(man.dx == 7)
                man.dx = 0;
            break;
        case VK_UP:
            if(man.y > rect.top )
                man.y -= 5;
            break;
        case VK_DOWN:
            if(man.y < rect.bottom - man.height)
                man.y += 5;
            break;
        }
        break;
    }
    case WM_DESTROY:
        PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
        break;
    default:                      /* for messages that we don't deal with */
        return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}

void make_snowflake(RECT * rect)
{
    Object * new_snowflake;

    new_snowflake = new Object();
    new_snowflake -> dx = rand()%11;
    new_snowflake -> dy = snowflakespeed[rand()%(sizeof(snowflakespeed)/sizeof(int))];
    new_snowflake -> x = rand()%(rect->right);
    new_snowflake -> y = snowflake.y;
    new_snowflake -> width = snowflake.width;
    new_snowflake -> height = snowflake.height;

    snowflakes.push_back(new_snowflake);
}

void fire(int num)
{
    Object * new_bullet;
    if(num == 1)
    {
        new_bullet = new Object();
        new_bullet -> dx = 20;
        new_bullet -> x = man.x + man.width/2 - bullet.width ;
        new_bullet -> y = man.y + man.height/2 + bullet.height ;
        new_bullet -> width = bullet.width;
        new_bullet -> height = bullet.height;

        bullets.push_back(new_bullet);
    }
    else
    {
        new_bullet = new Object();
        new_bullet -> dx = 20;
        new_bullet -> x = man.x + man.width/2 - bullet.width;
        new_bullet -> y = man.y + man.height/2 + bullet.height ;
        new_bullet -> width = bullet.width;
        new_bullet -> height = bullet.height;

        bullets.push_back(new_bullet);

        new_bullet = new Object();
        new_bullet -> dx = 25;
        new_bullet -> x = man.x + man.width/2 - bullet.width;
        new_bullet -> y = man.y + man.height/2 + bullet.height/2;
        new_bullet -> width = bullet.width;
        new_bullet -> height = bullet.height;

        bullets.push_back(new_bullet);
    }
}

void movestuff(RECT * rect)
{
    Object * object_for_delete;
    for(auto temp = bullets.begin(); temp!=bullets.end();)
    {
        if((*temp) -> x < rect -> right)
        {
            (*temp)->x += (*temp)->dx;
            temp++;
        }
        else
        {
            object_for_delete= *temp;
            temp = bullets.erase(temp);
            delete object_for_delete;
        }
    }

    for(auto temp = snowflakes.begin(); temp!=snowflakes.end();)
    {
        if((*temp)->y < rect -> bottom)
        {
            (*temp)->y += (*temp)->dy;
            temp++;
        }
        else
        {
            object_for_delete = *temp;
            temp = snowflakes.erase(temp);
            delete object_for_delete;
        }
    }

}
void draw (HDC hdc, RECT * rect)
{
    HDC first = CreateCompatibleDC(hdc);
    HBITMAP firstbm = (HBITMAP) CreateCompatibleBitmap(hdc, rect -> right, rect -> bottom);
    HBITMAP savefirstbm = (HBITMAP) SelectObject(first,firstbm);

    HDC second = CreateCompatibleDC(hdc);
    HBITMAP secondbm = (HBITMAP) SelectObject( second, hbmbackground);
    BitBlt(first, background.x, background.y, background.width, background.height, second, 0, 0, SRCCOPY);

    for(auto temp = bullets.begin(); temp!=bullets.end(); ++temp)
    {
        bool no_colison = true;
        for(auto temp2 = snowflakes.begin(); temp2!=snowflakes.end(); ++temp2)
        {
            if( ((((*temp) -> x) + ((*temp)->width)) >= ((*temp2) -> x) ) &&
                ((((*temp) -> x) + ((*temp)->width)) <= (((*temp2) -> x) + ((*temp2)->width))) &&
                ((((*temp) -> y) + ((*temp)->height)/2) <= (((*temp2) -> y) + ((*temp2)->height))) &&
                 ((((*temp) -> y) + ((*temp)->height)/2) >= ((*temp2) -> y) ) )
               {
                    auto x = *temp2;
                    temp2 = snowflakes.erase(temp2);
                    delete x;
                    no_colison = false;
                    break;
               }
        }

        if(no_colison)
        {
        SelectObject(second, hbmbullet);
        BitBlt(first, (*temp)->x, (*temp)->y, (*temp)->width, (*temp)->height, second, 0, 0,SRCAND);

        SelectObject(second, hbmbulletblack);
        BitBlt(first, (*temp)->x, (*temp)->y, (*temp)->width, (*temp)->height, second, 0, 0,SRCPAINT);
        }
        else
        {
             auto x = *temp;
             temp = bullets.erase(temp);
             delete x;
        }
    }

    SelectObject(second, hbmman);
    BitBlt(first, man.x, man.y, man.width, man.height, second, man.dx*man.width, 0,SRCAND);

    SelectObject(second, hbmmanblack);
    BitBlt(first, man.x, man.y, man.width, man.height, second, man.dx*man.width, 0,SRCPAINT);

    --man.dx;
    if(man.dx == -1)
        man.dx = 6;

    for(auto temp = snowflakes.begin(); temp!=snowflakes.end(); ++temp)
    {

        SelectObject(second, hbmsnowflake);
        BitBlt(first, (*temp)->x, (*temp)->y, (*temp)->width, (*temp)->height, second, (*temp)->dx*(*temp)->width, 0,SRCAND);

        SelectObject(second, hbmsnowflakeblack);
        BitBlt(first, (*temp)->x, (*temp)->y, (*temp)->width, (*temp)->height, second, (*temp)->dx*(*temp)->width, 0,SRCPAINT);

        ++((*temp)->dx);
        if((*temp)->dx == 11)
            (*temp)->dx = 0;
    }

    SelectObject(second,secondbm);
    DeleteDC(second);
    DeleteObject(secondbm);

    BitBlt(hdc, 0, 0, rect -> right, rect -> bottom, first, 0, 0, SRCCOPY);
    SelectObject(first,savefirstbm);
    DeleteDC(first);
    DeleteObject(firstbm);
    DeleteObject(savefirstbm);
}
