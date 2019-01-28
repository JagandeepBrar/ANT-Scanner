#include "analyzer.h"

static Initialize * ant = new Initialize();
static HWND hButtonStart, hButtonRefresh, hList;

int WINAPI WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	//Initializing WNDCLASSEX
	//Assigning all of the struct's values
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON));

	//Setting the global variable hInst for easy recall in future
	hInst = hInstance;
	//Registering WNDCLASSEX with Windows
	//Returns if registration fails
	if (!RegisterClassEx(&wcex)) {
		return FALSE;
	}

	//Creating the actual window and setting it's parameters
	//Set most to the default values
	HWND hWnd = CreateWindowEx(WS_EX_CLIENTEDGE, szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 1200, 1000, NULL, NULL, hInstance, NULL);
	//Ensures that CreateWindow ran successfully
	//If not, return
	if (!hWnd) {
		return FALSE;
	}
	//Now it displays the window we just created
	//Updates the window
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	//Message handler
	//Loops and checks for messages
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(
	HWND	hwnd,
	UINT	uMsg,
	WPARAM	wParam,
	LPARAM	lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	//Switch to respond to messages that arrive
	switch (uMsg)
	{
		case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
				//The case for when the initialize button is clicked.
				case BUTTON_INITIALIZE:
				{
					//Resets the viewfinder list.
					SendMessage(hList, LB_RESETCONTENT, 0, 0);
					//Cleans the device list.
					ant->cleanDevices();
					//Flip the status of the switch.
					SendMessage(hButtonStart, WM_SETTEXT, 0, (LPARAM)(ant->flipStatus()));
					
					break;
				}
				case BUTTON_REFRESH:
				{
					refresh();
					break;
				}
			}
			break;
		}
		//Create menu items on the window.
		case WM_CREATE:
		{
			//Creates the button for initialization.
			hButtonStart = CreateWindowEx(WS_EX_CLIENTEDGE,
				"BUTTON",
				"Initialize ANT+ Slave Device",
				WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
				50, 200, 500, 250,
				hwnd, (HMENU)BUTTON_INITIALIZE,
				hInst, NULL);
			//Creates the butto for refreshing.
			hButtonRefresh = CreateWindowEx(WS_EX_CLIENTEDGE,
				"BUTTON",
				"Find/Refresh Nearby Devices",
				WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
				50, 475, 500, 250,
				hwnd, (HMENU)BUTTON_REFRESH,
				hInst, NULL);
			//Creates the listbox for the list of devices.
			hList = CreateWindowEx(WS_EX_CLIENTEDGE,
				"LISTBOX",
				NULL,
				WS_CHILD | WS_VISIBLE,
				600, 10, 550, 925,
				hwnd, (HMENU)MENU_LIST,
				hInst, NULL);
		}

		//Paints the viewfinder
		case WM_PAINT:
		{
			hdc = BeginPaint(hwnd, &ps);
			//Displays the title, and who made it.
			TextOut(hdc, 250, 50, "ANT+ Scanning Tool", 18);
			TextOut(hdc, 261, 80, "Jagandeep Brar", 15);
			EndPaint(hwnd, &ps);
			break;
		}

		//Clicking on the close button
		case WM_CLOSE:
		{
			DestroyWindow(hwnd);
			break;
		}

		//Destroys the viewfinder
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			break;
		}

		//Default response
		default: 
		{
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
			break;
		}
	}
}

void refresh() {
	//Makes sure that the ANT+ slave device is running.
	if (ant->getStatus() != 0) {
		SendMessage(hButtonRefresh, WM_SETTEXT, 0, (LPARAM)("Please Start the USB ANT+ Device"));
	}
	else {
		SendMessage(hButtonRefresh, WM_SETTEXT, 0, (LPARAM)("Find/Refresh Nearby Devices"));
		//Reset the list on refresh.
		SendMessage(hList, LB_RESETCONTENT, 0, 0);
		//Checks to see if there are any devices found yet.
		//If not, display a message about how none are found.
		if (ant->getIndex() == 0) {
			SendMessage(hList, LB_INSERTSTRING, 0, (LPARAM)("No ANT+ devices found!"));
		}
		else {
			//If found any, go through them all and print the data.
			for (int i = 0; i<ant->getIndex(); i++) {
				char * result = new char[100];
				strcpy(result, ant->printDevices(i));
				SendMessage(hList, LB_INSERTSTRING, 0, (LPARAM)(result));
			}
		}
	}
}