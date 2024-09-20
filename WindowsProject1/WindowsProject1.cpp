#include "framework.h"
#include "resource.h"
#include <fstream>
#include <sstream>

#define MAX_LOADSTRING 100
#define IDC_OUTPUT_BOX 101 // Edit control ID
#define IDC_RUN_BUTTON 102 // Run simulation button ID
#define IDC_SAVE_BUTTON 103 // Save to file button ID

// 전역 변수:
HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];
HWND hOutputBox; // Edit control 핸들

// 함수 선언:
ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);

// 데이터를 저장할 전역 변수
std::string warehouseData;

// 데이터 생성, 분류, 저장 클래스 정의 (이전과 동일)
struct Data {
    std::string header_info;
    std::string actual_data;
    std::string dummy_data; // 필요에 따라 사용될 수 있는 더미 데이터
};

class DataGenerator {
public:
    DataGenerator() {
        srand((unsigned int)time(nullptr)); // 시드 초기화
    }

    Data generateData() {
        Data newData;
        newData.header_info = generateHeaderInfo();
        newData.actual_data = generateActualData();

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
        return "Header_" + std::to_string(rand() % 1000);
    }

    std::string generateActualData() {
        return "Data_" + std::to_string(rand() % 10000);
    }

    std::string generateDummyData() {
        return "Dummy_" + std::to_string(rand() % 10000);
    }
};

class DataClassifier {
public:
    void addPipeline(const std::string& header, const std::string& pipeline) {
        pipelines[header] = pipeline;
    }

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

// GUI 요소 생성
void CreateSimulationControls(HWND hWnd) {
    // 결과 출력 박스 (Edit Control)
    hOutputBox = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT(""),
        WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY,
        10, 10, 400, 300, hWnd, (HMENU)IDC_OUTPUT_BOX, hInst, NULL);

    // Run Simulation 버튼
    CreateWindow(TEXT("button"), TEXT("Run Simulation"),
        WS_VISIBLE | WS_CHILD,
        420, 10, 150, 30,
        hWnd, (HMENU)IDC_RUN_BUTTON, hInst, NULL);

    // Save to File 버튼
    CreateWindow(TEXT("button"), TEXT("Save to File"),
        WS_VISIBLE | WS_CHILD,
        420, 50, 150, 30,
        hWnd, (HMENU)IDC_SAVE_BUTTON, hInst, NULL);
}

// 시뮬레이션 실행 함수
void RunSimulation() {
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

    // Edit Control에 데이터 출력
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
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINDOWSPROJECT1, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    if (!InitInstance(hInstance, nCmdShow)) {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSPROJECT1));

    MSG msg;

    // 기본 메시지 루프입니다:
    while (GetMessage(&msg, nullptr, 0, 0)) {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
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
    hInst = hInstance;

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd) {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    // GUI 요소 생성
    CreateSimulationControls(hWnd);

    return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        if (wmId == IDC_RUN_BUTTON) {
            RunSimulation(); // 시뮬레이션 실행
        }
        else if (wmId == IDC_SAVE_BUTTON) {
            SaveToFile(); // 파일로 저장
        }
        else {
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_PAINT:
    {
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

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
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
