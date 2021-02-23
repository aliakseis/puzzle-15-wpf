#pragma once

ref class Puzzle :
public System::Windows::Window
{
public:
	Puzzle();

	System::Windows::Controls::DockPanel^ puzzleHostingPanel;
};
