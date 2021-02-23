#include <windows.h>

#include "Puzzle.h"

using namespace System;
using namespace System::Windows;

[STAThread]
int WINAPI WinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/,
				   LPSTR /*lpCmd*/, int /*nCmd*/)
{
	Window^ win = gcnew Puzzle();

	Application^ app = gcnew Application();
	app->Run(win);
}
