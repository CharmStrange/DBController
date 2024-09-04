#include "framework.h"
#include "WindowsProject1.h"

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

// 데이터 구조체 정의
struct Data {
    std::string header_info;
    std::string actual_data;
    std::string dummy_data; // 필요에 따라 사용될 수 있는 더미 데이터
};

class DataGenerator {
public:
    DataGenerator() {
        // 시드 초기화
        srand((unsigned int)time(nullptr));
    }

    Data generateData() {
        Data newData;
        newData.header_info = generateHeaderInfo();
        newData.actual_data = generateActualData();

        // 랜덤으로 더미 데이터를 생성할지 결정
        if (rand() % 2) {
            newData.dummy_data = generateDummyData();
        }
        else {
            newData.dummy_data = "";
        }

        return newData;
    }

private:
    std::string generateHeaderInfo() {
        // 간단한 예로 헤더 정보를 생성
        return "Header_" + std::to_string(rand() % 1000);
    }

    std::string generateActualData() {
        // 간단한 예로 실제 데이터를 생성
        return "Data_" + std::to_string(rand() % 10000);
    }

    std::string generateDummyData() {
        // 간단한 예로 더미 데이터를 생성
        return "Dummy_" + std::to_string(std::rand() % 10000);
    }
};

class DataClassifier {
public:
    // 파이프라인을 설정
    void addPipeline(const std::string& header, const std::string& pipeline) {
        pipelines[header] = pipeline;
    }

    // 데이터 분류
    std::string classify(const Data& data) {
        if (pipelines.find(data.header_info) != pipelines.end()) {
            return pipelines[data.header_info];
        }
        else {
            return "default_pipeline";
        }
    }

private:
    std::unordered_map<std::string, std::string> pipelines;
};

class DataWarehouse {
public:
    void storeData(const std::string& pipeline, const Data& data) {
        warehouse[pipeline].push_back(data);
    }

    std::string displayWarehouse() const {
        std::string result;
        for (const auto& entry : warehouse) {
            result += "\nPipeline: " + entry.first + "\n";
            for (const auto& data : entry.second) {
                result += "  Header: " + data.header_info + ", Data: " + data.actual_data;
                if (!data.dummy_data.empty()) {
                    result += ", Dummy: " + data.dummy_data;
                }
                result += "\n";
            }
        }
        return result;
    }

private:
    std::unordered_map<std::string, std::vector<Data>> warehouse;
};

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINDOWSPROJECT1, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSPROJECT1));

    MSG msg;

    // 기본 메시지 루프입니다:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSPROJECT1));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_WINDOWSPROJECT1);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

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

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static std::string warehouseData;

    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        switch (wmId)
        {
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

        // 데이터 생성, 분류 및 저장
        DataGenerator generator;
        DataClassifier classifier;
        DataWarehouse warehouse;

        // 파이프라인 설정
        classifier.addPipeline("Header_1", "pipeline_1");
        classifier.addPipeline("Header_2", "pipeline_2");
        classifier.addPipeline("Header_3", "pipeline_3");

        // 예제: 10개의 데이터를 생성, 분류 및 저장
        for (int i = 0; i < 10; ++i) {
            Data data = generator.generateData();
            std::string pipeline = classifier.classify(data);
            warehouse.storeData(pipeline, data);
        }

        // 저장된 데이터 출력
        warehouseData = warehouse.displayWarehouse();

        // 줄바꿈 처리를 위한 코드를 추가
        int y = 5;  // 시작 y 좌표
        size_t pos = 0;
        std::string line;
        while ((pos = warehouseData.find('\n')) != std::string::npos) {
            line = warehouseData.substr(0, pos);
            TextOutA(hdc, 5, y, line.c_str(), (int)line.length());
            warehouseData.erase(0, pos + 1);
            y += 20;  // 다음 줄로 이동 (줄 간격)
        }
        // 마지막 라인 처리
        if (!warehouseData.empty()) {
            TextOutA(hdc, 5, y, warehouseData.c_str(), (int)warehouseData.length());
        }

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
