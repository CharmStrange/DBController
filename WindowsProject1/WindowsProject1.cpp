#include "framework.h"
#include "resource.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <string>
#include "sqlite3.h"

#define MAX_LOADSTRING 100
#define IDC_OUTPUT_BOX 101
#define IDC_RUN_BUTTON 102
#define IDC_SAVE_BUTTON 103

HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];
HWND hOutputBox;

std::string warehouseData;
sqlite3* db; // SQLite DB 핸들

// 함수 선언
ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);

// 윈도우 클래스 등록
ATOM MyRegisterClass(HINSTANCE hInstance) {
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_WINDOWSPROJECT1);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));

    return RegisterClassExW(&wcex);
}





// 테이블 생성 함수
void createTables() {
    const char* createMainTable =
        "CREATE TABLE IF NOT EXISTS Main ("
        "Id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "AccountInfo TEXT NOT NULL, "
        "AccountID TEXT NOT NULL, "
        "AccountPW TEXT NOT NULL, "
        "Platform TEXT NOT NULL);";

    const char* createElseTable =
        "CREATE TABLE IF NOT EXISTS `Else` ("
        "Id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "AccountInfo TEXT NOT NULL, "
        "AccountID TEXT NOT NULL, "
        "AccountPW TEXT NOT NULL, "
        "Platform TEXT NOT NULL);";

    // 다른 테이블도 동일하게 생성
    sqlite3_exec(db, createMainTable, NULL, NULL, NULL);
    sqlite3_exec(db, createElseTable, NULL, NULL, NULL);
}

// GUI 요소 생성
void CreateSimulationControls(HWND hWnd) {
    hOutputBox = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT(""),
        WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY,
        10, 10, 400, 300, hWnd, (HMENU)IDC_OUTPUT_BOX, hInst, NULL);

    CreateWindow(TEXT("button"), TEXT("Run Simulation"),
        WS_VISIBLE | WS_CHILD, 420, 10, 150, 30,
        hWnd, (HMENU)IDC_RUN_BUTTON, hInst, NULL);

    CreateWindow(TEXT("button"), TEXT("Save to File"),
        WS_VISIBLE | WS_CHILD, 420, 50, 150, 30,
        hWnd, (HMENU)IDC_SAVE_BUTTON, hInst, NULL);
}

// 인스턴스 초기화
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) {
    hInst = hInstance; // 글로벌 인스턴스 핸들을 저장

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, 600, 400, nullptr, nullptr, hInstance, nullptr); // 창 크기를 적절히 설정

    if (!hWnd) {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    // 시뮬레이션 컨트롤을 생성 (창이 만들어진 후)
    CreateSimulationControls(hWnd);

    return TRUE;
}

// 데이터베이스에 데이터 삽입
void insertData(const std::string& tableName, const std::string& accountInfo,
    const std::string& accountID, const std::string& accountPW, const std::string& platform) {
    std::string query = "INSERT INTO `" + tableName + "` (AccountInfo, AccountID, AccountPW, Platform) VALUES ('" +
        accountInfo + "', '" + accountID + "', '" + accountPW + "', '" + platform + "');";

    char* errMsg = 0;
    int rc = sqlite3_exec(db, query.c_str(), NULL, NULL, &errMsg);
    if (rc != SQLITE_OK) {
        MessageBox(NULL, TEXT("Error inserting data!"), TEXT("Error"), MB_OK);
        sqlite3_free(errMsg);
    }
}

// 데이터베이스에서 데이터 조회
std::string queryData(const std::string& tableName) {
    std::string result;
    std::string query = "SELECT * FROM `" + tableName + "`;";

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        return "Error querying data!";
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        std::string id = std::to_string(sqlite3_column_int(stmt, 0));
        std::string accountInfo = (const char*)sqlite3_column_text(stmt, 1);
        std::string accountID = (const char*)sqlite3_column_text(stmt, 2);
        std::string accountPW = (const char*)sqlite3_column_text(stmt, 3);
        std::string platform = (const char*)sqlite3_column_text(stmt, 4);
        result += "Id: " + id + ", AccountInfo: " + accountInfo + ", AccountID: " + accountID +
            ", AccountPW: " + accountPW + ", Platform: " + platform + "\n";
    }
    sqlite3_finalize(stmt);
    return result.empty() ? "No data found." : result;
}

// 데이터 삭제
void deleteData(const std::string& tableName, int id) {
    std::string query = "DELETE FROM `" + tableName + "` WHERE Id = " + std::to_string(id) + ";";
    sqlite3_exec(db, query.c_str(), NULL, NULL, NULL);
}

// 시뮬레이션 실행 함수
void RunSimulation() {
    std::string data = queryData("Main");
    warehouseData = data;

    std::wstring ws(warehouseData.begin(), warehouseData.end());
    SetWindowText(hOutputBox, ws.c_str());
}

// 데이터를 파일로 저장하는 함수
void SaveToFile() {
    std::ofstream outfile("simulation_output.txt");
    outfile << warehouseData;
    outfile.close();
    MessageBox(NULL, TEXT("Data saved to simulation_output.txt"), TEXT("Save Successful"), MB_OK);
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINDOWSPROJECT1, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    if (!InitInstance(hInstance, nCmdShow)) {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSPROJECT1));
    MSG msg;

    // SQLite 데이터베이스 초기화
    sqlite3_open("simulation.db", &db);
    createTables();

    // 메시지 루프
    while (GetMessage(&msg, nullptr, 0, 0)) {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    // 데이터베이스 닫기
    sqlite3_close(db);
    return (int)msg.wParam;
}

// 윈도우 프로시저 정의
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_SIZE: {
        int width = LOWORD(lParam);
        int height = HIWORD(lParam);

        // OutputBox의 크기를 창의 크기에 맞게 조정
        MoveWindow(hOutputBox, 10, 10, width - 200, height - 20, TRUE);

        // Run 버튼과 Save 버튼의 위치를 새로 설정
        MoveWindow(GetDlgItem(hWnd, IDC_RUN_BUTTON), width - 170, 10, 150, 30, TRUE);
        MoveWindow(GetDlgItem(hWnd, IDC_SAVE_BUTTON), width - 170, 50, 150, 30, TRUE);
    }
                break;

    case WM_COMMAND: {
        int wmId = LOWORD(wParam);
        switch (wmId) {
        case IDC_RUN_BUTTON:
            RunSimulation(); // 시뮬레이션 실행
            break;
        case IDC_SAVE_BUTTON:
            SaveToFile(); // 파일로 저장
            break;
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

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
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


// "정보" 대화 상자 처리 함수 정의
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    UNREFERENCED_PARAMETER(lParam);
    switch (message) {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
