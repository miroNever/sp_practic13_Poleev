// zadanie13.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "zadanie13.h"
#include <windows.h>
#include <iostream>

#define MAX_LOADSTRING 100

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Разместите код здесь.

    // Инициализация глобальных строк
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_ZADANIE13, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_ZADANIE13));

    MSG msg;

    // Цикл основного сообщения:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  ФУНКЦИЯ: MyRegisterClass()
//
//  ЦЕЛЬ: Регистрирует класс окна.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ZADANIE13));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_ZADANIE13);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   ЦЕЛЬ: Сохраняет маркер экземпляра и создает главное окно
//
//   КОММЕНТАРИИ:
//
//        В этой функции маркер экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится главное окно программы.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  ФУНКЦИЯ: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ЦЕЛЬ: Обрабатывает сообщения в главном окне.
//
//  WM_COMMAND  - обработать меню приложения
//  WM_PAINT    - Отрисовка главного окна
//  WM_DESTROY  - отправить сообщение о выходе и вернуться
//
//
void ListPrograms(HKEY hKey, const std::wstring& subkey, HWND hwndListBox) {
    HKEY hSubKey;
    LONG result = RegOpenKeyExW(hKey, subkey.c_str(), 0, KEY_READ, &hSubKey);
    if (result != ERROR_SUCCESS) {
        std::wcout << L"Ошибка при открытии ключа реестра: " << subkey << std::endl;
        return;
    }

    DWORD index = 0;
    WCHAR szDisplayName[MAX_PATH];
    DWORD dwDisplayNameSize = ARRAYSIZE(szDisplayName);
    while (RegEnumKeyExW(hSubKey, index, szDisplayName, &dwDisplayNameSize, nullptr, nullptr, nullptr, nullptr) == ERROR_SUCCESS) {
        HKEY hProgramKey;
        // Открываем подключь каждой программы
        LONG programResult = RegOpenKeyExW(hSubKey, szDisplayName, 0, KEY_READ, &hProgramKey);
        if (programResult == ERROR_SUCCESS) {
            // Получаем значение DisplayName
            WCHAR szProgramName[MAX_PATH];
            DWORD dwProgramNameSize = ARRAYSIZE(szProgramName);
            DWORD dwType;
            programResult = RegQueryValueExW(hProgramKey, L"DisplayName", nullptr, &dwType, reinterpret_cast<LPBYTE>(szProgramName), &dwProgramNameSize);
            if (programResult == ERROR_SUCCESS && dwType == REG_SZ) {
                // Добавляем название программы в ListBox
                SendMessage(hwndListBox, LB_ADDSTRING, 0, reinterpret_cast<LPARAM>(szProgramName));
            }
            RegCloseKey(hProgramKey);
        }
        dwDisplayNameSize = ARRAYSIZE(szDisplayName);
        index++;
    }

    RegCloseKey(hSubKey);
}




void AutoRun(HKEY hKey, const std::wstring& subkey, HWND hwndListBox) {
    HKEY hSubKey;
    LONG result = RegOpenKeyExW(hKey, subkey.c_str(), 0, KEY_READ, &hSubKey);
    if (result != ERROR_SUCCESS) {
        std::wcout << L"Ошибка при открытии ключа реестра: " << subkey << std::endl;
        return;
    }

    DWORD index = 0;
    WCHAR szValueName[1024];
    DWORD dwValueNameSize = ARRAYSIZE(szValueName);
    DWORD dwType;
    BYTE lpData[1024];
    DWORD dwDataSize = sizeof(lpData);

    while (RegEnumValueW(hSubKey, index, szValueName, &dwValueNameSize, nullptr, &dwType, lpData, &dwDataSize) == ERROR_SUCCESS) {
        // Проверяем, что значение - строка
        if (dwType == REG_SZ) {
            // Добавляем строку в ListBox
            SendMessage(hwndListBox, LB_ADDSTRING, 0, (LPARAM)szValueName);
        }

        // Сбрасываем размеры для следующей итерации
        dwValueNameSize = ARRAYSIZE(szValueName);
        dwDataSize = sizeof(lpData);
        index++;
    }

    RegCloseKey(hSubKey);
}
#define ID_BUTTON 1
#define ID_EDIT1 2
#define ID_EDIT2 3

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static HWND hwndEdit1, hwndEdit2;
    switch (message)
    {
    case WM_CREATE:
    {
        CreateWindowW(L"static", L"Из:", WS_CHILD | WS_VISIBLE | SS_CENTER, 10, 10, 100, 20, hWnd, NULL, hInst, NULL);
        hwndEdit1 = CreateWindowW(L"edit", L"", WS_CHILD | WS_VISIBLE | WS_BORDER, 120, 10, 500, 20, hWnd, (HMENU)ID_EDIT1, hInst, NULL);

        CreateWindowW(L"static", L"В:", WS_CHILD | WS_VISIBLE | SS_CENTER, 10, 40, 100, 20, hWnd, NULL, hInst, NULL);
        hwndEdit2 = CreateWindowW(L"edit", L"", WS_CHILD | WS_VISIBLE | WS_BORDER, 120, 40, 500, 20, hWnd, (HMENU)ID_EDIT2, hInst, NULL);
        
        CreateWindowW(L"button", L"Переместить", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 120, 70, 100, 30, hWnd, (HMENU)ID_BUTTON, hInst, NULL);

        // Создаем ListBox'ы
        HWND hwndListBox1 = CreateWindowW(L"listbox", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | LBS_STANDARD | LBS_DISABLENOSCROLL,
            10, 110, 300, 200, hWnd, (HMENU)1001, hInst, NULL);

        HWND hwndListBox2 = CreateWindowW(L"listbox", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | LBS_STANDARD | LBS_DISABLENOSCROLL,
            320, 110, 300, 200, hWnd, (HMENU)1002, hInst, NULL);

        HWND hwndListBox3 = CreateWindowW(L"listbox", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | LBS_STANDARD | LBS_DISABLENOSCROLL,
            630, 110, 300, 200, hWnd, (HMENU)1003, hInst, NULL);

        // Заполняем ListBox'ы информацией из реестра
        ListPrograms(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall", hwndListBox1);
        AutoRun(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", hwndListBox2);
        AutoRun(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", hwndListBox3);
        break;
    }

    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Разобрать выбор в меню:
            switch (wmId)
            {
            case ID_BUTTON:
            {
                wchar_t oldPath[MAX_PATH];
                wchar_t newPath[MAX_PATH];

                // Получаем текст из полей ввода
                GetWindowText(hwndEdit1, oldPath, MAX_PATH);
                GetWindowText(hwndEdit2, newPath, MAX_PATH);

                // Проверяем, что пути не пустые
                if (wcslen(oldPath) == 0 || wcslen(newPath) == 0) {
                    MessageBox(hWnd, L"Введите пути к файлам.", L"Ошибка", MB_OK | MB_ICONERROR);
                    break;
                }
                // Перемещаем файл
                MoveFileEx(oldPath, newPath, MOVEFILE_WRITE_THROUGH);
                GetLastError();

                break;
            }

            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Добавьте сюда любой код прорисовки, использующий HDC...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Обработчик сообщений для окна "О программе".
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
