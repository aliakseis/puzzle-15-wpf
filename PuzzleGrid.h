#pragma once

enum MoveStatus { Left, Up, Right, Down };

ref class PuzzleGrid :
public System::Windows::Controls::Grid
{
public:
	PuzzleGrid(void);

private:
	void PuzzleGridLoaded(Object^ sender, System::Windows::RoutedEventArgs^ e);

	void SetupThePuzzleGridStructure();

	void OnPreviewPuzzleMouseDown(Object^ sender, System::Windows::RoutedEventArgs^ e);
	void OnPreviewPuzzleMouseUp(Object^ sender, System::Windows::RoutedEventArgs^ e);
	void OnPreviewPuzzleMouseMove(Object^ sender, System::Windows::RoutedEventArgs^ e);

	void AnimatePiece(System::Windows::Controls::Control^ b, int row, int col, MoveStatus moveStatus);

	void KeyPressed(Object^ sender, System::Windows::Input::KeyEventArgs^ e);

	void DoWork(Object^ sender, System::ComponentModel::DoWorkEventArgs^ e);
	void RunWorkerCompleted(Object^ sender, System::ComponentModel::RunWorkerCompletedEventArgs^ e);
	void AnimationStep(Object^ sender, System::EventArgs^ e);


	System::Windows::ResourceDictionary^ globalResources;
	System::Windows::Size^ masterPuzzleSize;
	System::Windows::UIElement^ elementForPuzzle;
	bool mouseButtonPressed;
	System::Windows::Controls::Button^ buttonBeingMoved;
	System::Windows::Point^ moveStartPos;
	int emptyX, emptyY;
	System::Collections::Generic::IEnumerator<unsigned char>^ solution;
};
