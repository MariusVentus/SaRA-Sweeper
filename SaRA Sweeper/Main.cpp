#include <windows.h>
#include <direct.h>
#include <fstream>
#include <string>
#include <vector>

#define IDC_MAIN_EDIT 101
#define ID_FILE_EXIT 9001
#define ID_ABOUT 9002
#define ID_HELP 9003
#define ID_SWEEP 9004

//Globals
const wchar_t g_szClassName[] = L"SaRASweeperMain";
const wchar_t g_WindowTitle[] = L"SaRASweeper V0.0.1";
HWND hMainWindow, hNote;

//Forward Declarations
bool RegisterMainWindow(HINSTANCE hInstance);
bool CreateMainWindow(HINSTANCE hInstance);
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

void AddMenu(HWND hwnd);
void AddControls(HWND hwnd);
bool SelectFile(HWND hwnd, std::wstring& path);
bool LoadFile(const std::wstring& path, std::wstring& content);
std::wstring ScrubFileContent(const std::wstring& content);


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG Msg;

	if (!RegisterMainWindow(hInstance)) {
		return 0;
	}

	if (!CreateMainWindow(hInstance)) {
		return 0;
	}

	ShowWindow(hMainWindow, nCmdShow);
	UpdateWindow(hMainWindow);

	while (GetMessage(&Msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
	return Msg.wParam;
}


bool RegisterMainWindow(HINSTANCE hInstance)
{
	WNDCLASSEX wc = { 0 };

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = 0;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = CreateSolidBrush(RGB(0, 162, 237));
	wc.lpszMenuName = NULL;
	wc.lpszClassName = g_szClassName;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassEx(&wc))
	{
		MessageBox(NULL, L"Window Registration Failed!", L"Error!",
			MB_ICONEXCLAMATION | MB_OK);
		return false;
	}
	else {
		return true;
	}
}

bool CreateMainWindow(HINSTANCE hInstance)
{
	hMainWindow = CreateWindowEx(WS_EX_CLIENTEDGE, g_szClassName, g_WindowTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 500, 500, NULL, NULL, hInstance, NULL);

	if (hMainWindow == NULL)
	{
		MessageBox(NULL, L"Window Creation Failed!", L"Error!",
			MB_ICONEXCLAMATION | MB_OK);
		return false;
	}
	else {
		return true;
	}
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CREATE:
		AddMenu(hwnd);
		AddControls(hwnd);
		break;
	case WM_COMMAND:
		switch (wParam) {
		case ID_SWEEP:
		{
			std::wstring filepath = L"";
			SelectFile(hwnd, filepath);
			std::wstring content = L"";
			LoadFile(filepath, content);
			SetWindowText(hNote, L"Checking...");
			SetWindowText(hNote, ScrubFileContent(content).c_str());
		}
		break;
		case ID_FILE_EXIT:
			PostQuitMessage(0);
			break;
		case ID_HELP:
			MessageBox(NULL, L"No help, only Zuul.\nOr ping me on Teams \n\n-MariusVentus", L"Halp", MB_OK | MB_ICONINFORMATION);
			break;
		case ID_ABOUT:
			MessageBox(NULL, L"Made over lunch and a breakfast! Quick ItemCount checker.", L"About", MB_OK | MB_ICONINFORMATION);
			break;
		default:
			break;
		}
		break;
	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

void AddMenu(HWND hwnd)
{
	HMENU hMenu, hFileMenu;
	hMenu = CreateMenu();
	//File Menu
	hFileMenu = CreatePopupMenu();
	AppendMenu(hFileMenu, MF_STRING, ID_FILE_EXIT, L"Exit");
	AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hFileMenu, L"File");

	//Remaining Main Menu Items1e1e1e
	AppendMenu(hMenu, MF_STRING, ID_ABOUT, L"About");
	AppendMenu(hMenu, MF_STRING, ID_HELP, L"Help");

	SetMenu(hwnd, hMenu);
}

void AddControls(HWND hwnd)
{
	//Notes
	CreateWindowEx(WS_EX_CLIENTEDGE, L"STATIC", L" Points of Interest: ", WS_CHILD | WS_VISIBLE,
		15, 15, 440, 25, hwnd, NULL, GetModuleHandle(NULL), NULL);
	hNote = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"",
		WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | WS_HSCROLL | ES_AUTOHSCROLL,
		15, 40, 440, 300, hwnd, (HMENU)IDC_MAIN_EDIT, GetModuleHandle(NULL), NULL);

	//Load File and display cleaned items
	CreateWindowEx(WS_EX_CLIENTEDGE, L"Button", L"Load and Clean", WS_CHILD | WS_VISIBLE,
		15, 355, 440, 50, hwnd, (HMENU)ID_SWEEP, GetModuleHandle(NULL), NULL);
}

bool SelectFile(HWND hwnd, std::wstring& path)
{
	wchar_t* buffer = _wgetcwd(NULL, 0);

	OPENFILENAME ofn = { 0 };
	wchar_t file_name[MAX_PATH] = { 0 };

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFile = file_name;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFilter = L"All Files\0*.*\0";
	ofn.nFilterIndex = 1;

	bool fileSelected = GetOpenFileName(&ofn);

	path = ofn.lpstrFile;
	//if (fileSelected != false) {
	//	std::filesystem::create_directories(".\\Templates\\Copies");
	//	std::filesystem::copy(ofn.lpstrFile, ".\\Templates\\Copies", std::filesystem::copy_options::overwrite_existing);
	//}

	//char* buffer2 = _getcwd(NULL, 0);
	_wchdir(buffer);
	//buffer = _getcwd(NULL, 0);
	free(buffer);
	return fileSelected;
}

bool LoadFile(const std::wstring & path, std::wstring & content)
{
	std::wifstream in(path);
	if (in) {
		std::wstring token = L"";
		do {
			token.clear();
			std::getline(in, token);
			//Remove whitespace
			while (token.find(L" ") != std::string::npos) {
				token.erase(token.find(L" "), 1);
			}
			content.append(token);
		} while (!in.eof());
		return true;
	}
	else {
		return false;
	}
}

std::wstring ScrubFileContent(const std::wstring& content)
{
	//This is so ugly, please don't look. Will make it pretty later. Maybe.
	std::vector<std::wstring> pairs;
	std::wstring token = L"";
	std::wstring localContent = content;
	localContent.erase(0, localContent.find(L"{\"text\":\"fullfolderpath="));
	//Fill Vector
	while (localContent.find(L"{\"text\":\"fullfolderpath=") != std::string::npos || localContent.find(L",{\"text\":\"ItemCount=") != std::string::npos) {
		token.clear();
		token = localContent.substr(localContent.find(L"{\"text\":\"fullfolderpath="), localContent.find(L",{\"text\":\"ItemCount="));
		localContent.erase(localContent.find(token), token.size());
		token.append(localContent.substr(localContent.find(L",{\"text\":\"ItemCount="), localContent.find(L"\"}")+2));
		localContent.erase(0, localContent.find(L"{\"text\":\"fullfolderpath="));
		
		if (!token.empty()) {
			pairs.push_back(token);
		}
	}

	//Only choose those with ItemCount greater than 4000
	std::wstring output;
	for (unsigned i = 0; i < pairs.size(); i++) {
		token.clear();
		token = pairs[i];
		token.erase(0, token.find_last_of(L"=") + 1);
		token.erase(token.find(L"\"}"));
		if (std::stoul(token) > 4000) {
			output.append(pairs[i]);
			output.append(L"\r\n\r\n");
		}
	}
	return output;

}
