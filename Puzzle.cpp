
#include "Puzzle.h"

#include "PuzzleGrid.h"

using namespace System;
using namespace System::Windows;
using namespace System::Windows::Media;
using namespace System::Windows::Media::Imaging;
using namespace System::Resources;
using namespace System::Reflection;
using namespace System::IO;
using namespace System::Windows::Controls;



Puzzle::Puzzle()
{
	Title = "Puzzle 15";
    Width = 800;
    Height = 600;

	Background = gcnew LinearGradientBrush(Colors::Black, 
										Colors::White, 
										Point(0.5,0), 
										Point(0.5,1));

	puzzleHostingPanel = gcnew DockPanel();
	AddChild(puzzleHostingPanel);

	Label^ label = gcnew Label();
	label->Content = "Hit F11 to start computation ...";
	puzzleHostingPanel->SetDock(label, Dock::Bottom);
	puzzleHostingPanel->Children->Add(label);

	puzzleHostingPanel->Children->Add(gcnew PuzzleGrid());
}
