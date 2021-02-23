
#include "PuzzleGrid.h"

#include "solver.h"

using namespace System;
using namespace System::Windows;
using namespace System::Windows::Media;
using namespace System::Windows::Media::Imaging;
using namespace System::Windows::Media::Animation;
using namespace System::Resources;
using namespace System::Reflection;
using namespace System::IO;
using namespace System::Windows::Controls;
using namespace System::Windows::Markup;
using namespace System::Windows::Input;
using namespace System::ComponentModel;
using namespace System::Collections::Generic;


PuzzleGrid::PuzzleGrid()
: mouseButtonPressed(false)
, emptyX(3), emptyY(3)
{
	Assembly^ assembly = Assembly::GetExecutingAssembly();

	ResourceManager^ resourceManager = gcnew ResourceManager("Puzzle15.Puzzle15", assembly);

	Stream^ stImage = resourceManager->GetStream("sanko");

	BitmapImage^ bitmap = gcnew BitmapImage();
	bitmap->BeginInit();
	bitmap->StreamSource = stImage;
	bitmap->EndInit();

	masterPuzzleSize = gcnew Size(bitmap->PixelWidth, bitmap->PixelHeight);

	Image^ img = gcnew Image();
	img->Source = bitmap;
	elementForPuzzle = img;

	Stream^ stResources = resourceManager->GetStream("Resources");

	globalResources = (ResourceDictionary^) XamlReader::Load(stResources);

    AddHandler(PreviewMouseDownEvent, gcnew RoutedEventHandler(this, &PuzzleGrid::OnPreviewPuzzleMouseDown));
    AddHandler(PreviewMouseUpEvent, gcnew RoutedEventHandler(this, &PuzzleGrid::OnPreviewPuzzleMouseUp));
    AddHandler(PreviewMouseMoveEvent, gcnew RoutedEventHandler(this, &PuzzleGrid::OnPreviewPuzzleMouseMove));

	Loaded += gcnew RoutedEventHandler(this, &PuzzleGrid::PuzzleGridLoaded);

	KeyDown += gcnew KeyEventHandler(this, &PuzzleGrid::KeyPressed);

	SetupThePuzzleGridStructure();
}

void PuzzleGrid::SetupThePuzzleGridStructure()
{
	// Define rows and columns in the Grid
	for (int row = 0; row < 4; row++)
	{
		RowDefinition^ r = gcnew RowDefinition();
		r->Height = GridLength::Auto;
		RowDefinitions->Add(r);

		ColumnDefinition^ c = gcnew ColumnDefinition();
		c->Width = GridLength::Auto;
		ColumnDefinitions->Add(c);
	}

	System::Windows::Style^ buttonStyle = (System::Windows::Style^) globalResources["PuzzleButtonStyle"];

	// Now add the buttons in
	int i = 1;
	for (int row = 0; row < 4; row++)
	{
		for (int col = 0; col < 4; col++)
		{
			// lower right cell is empty
			if (row == 4 - 1 && col == 4 - 1)
			{
				continue;
			}
			Button^ b = gcnew Button();

		    b->Style = buttonStyle;

			b->SetValue(RowProperty, row);
			b->SetValue(ColumnProperty, col);
			b->Content = i;

            i++;
			Children->Add(b);
		}
	}
}


void PuzzleGrid::PuzzleGridLoaded(Object^ /*sender*/, System::Windows::RoutedEventArgs^ /*e*/)
{
	Width = masterPuzzleSize->Width;
	Height = masterPuzzleSize->Height;

	const float edgeSize = 1.f / 4;
	double pieceRowHeight = masterPuzzleSize->Height / 4;
	double pieceColWidth = masterPuzzleSize->Width / 4;

	// Set up viewbox appropriately for each tile->
	for each (Button^ b in Children)
	{
		Border^ root = (Border^)b->Template->FindName("TheTemplateRoot", b);

		int row = (int)b->GetValue(RowProperty);
		int col = (int)b->GetValue(ColumnProperty);

        VisualBrush^ vb = gcnew VisualBrush(elementForPuzzle);
		vb->Viewbox = Rect(col * edgeSize, row * edgeSize, edgeSize, edgeSize);
		vb->ViewboxUnits = BrushMappingMode::RelativeToBoundingBox;

		root->Background = vb;
		root->Height = pieceRowHeight;
		root->Width = pieceColWidth;

	    root->RenderTransform = gcnew TranslateTransform(0, 0);
	}
}


void PuzzleGrid::OnPreviewPuzzleMouseDown(Object^ /*sender*/, System::Windows::RoutedEventArgs^ e)
{
	solution = nullptr;
    Button^ b = (Button^) e->Source;
    if (b != nullptr)
    {
        mouseButtonPressed = true;
        buttonBeingMoved = b;

        Children->Remove(b);
        Children->Add(b);

		moveStartPos = Mouse::GetPosition(nullptr);
    }
}

void PuzzleGrid::OnPreviewPuzzleMouseUp(Object^ /*sender*/, System::Windows::RoutedEventArgs^ /*e*/)
{
    if (!mouseButtonPressed)
        return;

    mouseButtonPressed = false;

    Border^ root = (Border^)buttonBeingMoved->Template->FindName("TheTemplateRoot", buttonBeingMoved);

    ((TranslateTransform^)root->RenderTransform)->X = 0;
    ((TranslateTransform^)root->RenderTransform)->Y = 0;

    int row = (int)buttonBeingMoved->GetValue(RowProperty);
    int col = (int)buttonBeingMoved->GetValue(ColumnProperty);

    double pieceRowHeight = masterPuzzleSize->Height / 4;
    double pieceColWidth = masterPuzzleSize->Width / 4;

	Point^ movePos = Mouse::GetPosition(nullptr);
    int offsetX;
    if (movePos->X > moveStartPos->X)
        offsetX = (int)((movePos->X - moveStartPos->X + pieceColWidth / 2) / pieceColWidth);
    else
        offsetX = (int)((movePos->X - moveStartPos->X - pieceColWidth / 2) / pieceColWidth);

    int offsetY;
    if (movePos->Y > moveStartPos->Y)
        offsetY = (int)((movePos->Y - moveStartPos->Y + pieceRowHeight / 2) / pieceRowHeight);
    else
        offsetY = (int)((movePos->Y - moveStartPos->Y - pieceRowHeight / 2) / pieceRowHeight);

    int newCol, newRow;
    if (offsetX != 0 || offsetY != 0)
    {
        newCol = col + offsetX;
        newRow = row + offsetY;

        if (newCol < 0)
            newCol = 0;
        else if (newCol > 3)
            newCol = 3;

        if (newRow < 0)
            newRow = 0;
        else if (newRow > 3)
            newRow = 3;

		if (emptyX == newCol && emptyY == newRow)
		{

			if (offsetX != 0 || offsetY != 0)
			{
				emptyX = col;
				emptyY = row;
				buttonBeingMoved->SetValue(ColumnProperty, newCol);
				buttonBeingMoved->SetValue(RowProperty, newRow);
			}
		}
		else
		{
			for each (Button^ b in Children)
			{
				int r = (int)b->GetValue(RowProperty);
				int c = (int)b->GetValue(ColumnProperty);
				if (r == newRow && c == newCol)
				{
					b->SetValue(ColumnProperty, col);
					b->SetValue(RowProperty, row);

					buttonBeingMoved->SetValue(ColumnProperty, newCol);
					buttonBeingMoved->SetValue(RowProperty, newRow);

					break;
				}
			}
		}
    }
    else
    {
        newCol = emptyX;
        newRow = emptyY;

        if (newCol == col)
        {
            if (newRow == row)
                return;
            if (newRow == row + 1)
            {
                emptyX = col;
                emptyY = row;
				AnimatePiece(buttonBeingMoved, newRow, newCol, Down);
                return;
            }
            if (newRow == row - 1)
            {
                emptyX = col;
                emptyY = row;
				AnimatePiece(buttonBeingMoved, newRow, newCol, Up);
                return;
            }
        }

        if (newRow == row)
        {
            if (newCol == col + 1)
            {
                emptyX = col;
                emptyY = row;
				AnimatePiece(buttonBeingMoved, newRow, newCol, Right);
                return;
            }
            if (newCol == col - 1)
            {
                emptyX = col;
                emptyY = row;
				AnimatePiece(buttonBeingMoved, newRow, newCol, Left);
                return;
            }
        }
    }
}

void PuzzleGrid::OnPreviewPuzzleMouseMove(Object^ /*sender*/, System::Windows::RoutedEventArgs^ /*e*/)
{
    if (mouseButtonPressed)
    {
		Point^ movePos = Mouse::GetPosition(nullptr);

        Border^ root = (Border^)buttonBeingMoved->Template->FindName("TheTemplateRoot", buttonBeingMoved);
        ((TranslateTransform^)root->RenderTransform)->X = movePos->X - moveStartPos->X;
        ((TranslateTransform^)root->RenderTransform)->Y = movePos->Y - moveStartPos->Y;
    }
}


// Assumed to be a valid move
void PuzzleGrid::AnimatePiece(Control^ b, int row, int col, MoveStatus moveStatus)
{
    b->SetValue(ColumnProperty, col);
    b->SetValue(RowProperty, row);

    FrameworkElement^ root = (FrameworkElement^)b->Template->FindName("TheTemplateRoot", b);

	double distance;
    bool isMoveHorizontal;

    if (moveStatus == Left || moveStatus == Right)
	{
        isMoveHorizontal = true;
		distance = (moveStatus == Left ? -1 : 1) * root->Width;
	}
	else
	{
        isMoveHorizontal = false;
        distance = (moveStatus == Up ? -1 : 1) * root->Height;
    }

	// pull the animation after it's complete, because we move change Grid cells->
	DoubleAnimation^ slideAnim = gcnew DoubleAnimation(-distance, 0, TimeSpan::FromSeconds(0.3), FillBehavior::Stop);
    DependencyProperty^ directionProperty = 
		isMoveHorizontal ? TranslateTransform::XProperty : TranslateTransform::YProperty;

	if (solution != nullptr)
		slideAnim->CurrentStateInvalidated += gcnew EventHandler(this, &PuzzleGrid::AnimationStep);

    root->RenderTransform->BeginAnimation(directionProperty, slideAnim);
}

void PuzzleGrid::KeyPressed(Object^ /*sender*/, System::Windows::Input::KeyEventArgs^ e)
{
	if (e->Key != Key::F11)
		return;

	Mouse::OverrideCursor = Cursors::Wait;
	IsEnabled = false;

	array<unsigned char>^ data = gcnew array<unsigned char>(16);

    for each (Button^ b in Children)
    {
        int r = (int)b->GetValue(RowProperty);
        int c = (int)b->GetValue(ColumnProperty);

		int cell = (int) b->Content;

		data[r * 4 + c] = (unsigned char) cell;
	}

	BackgroundWorker^ backgroundWorker = gcnew BackgroundWorker();
	backgroundWorker->DoWork += gcnew DoWorkEventHandler(this, &PuzzleGrid::DoWork);
	backgroundWorker->RunWorkerCompleted += gcnew RunWorkerCompletedEventHandler(this, &PuzzleGrid::RunWorkerCompleted);

	backgroundWorker->RunWorkerAsync(data);
}

void PuzzleGrid::DoWork(Object^ /*sender*/, DoWorkEventArgs^ e)
{
    // Do something
	array<unsigned char>^ data = (array<unsigned char>^) e->Argument;
	array<unsigned char>^ strategy = gcnew array<unsigned char>(80);

	pin_ptr<unsigned char> pInput = &data[0];
	pin_ptr<unsigned char> pResult = &strategy[0];

	int result = Solve(pInput, pResult);
    List<unsigned char>^ list = nullptr;
    if (result > 0)
    {
		list = gcnew List<unsigned char>((IEnumerable<unsigned char>^)strategy);//?
        list->RemoveRange(result, list->Count - result);
    }
	e->Result = list;
}

// Completed Method
void PuzzleGrid::RunWorkerCompleted(Object^ /*sender*/, RunWorkerCompletedEventArgs^ e)
{
	IsEnabled = true;
	Mouse::OverrideCursor = nullptr;

	List<unsigned char>^ result = (List<unsigned char>^) e->Result;
	if (result != nullptr)
	{
		solution = result->GetEnumerator();
		AnimationStep(nullptr, nullptr);
	}
}

void PuzzleGrid::AnimationStep(Object^ sender, EventArgs^ /*e*/)
{
	// Anonymous delegate -- invoke when done.
	Clock^ clock = (Clock^)sender;
	if (clock != nullptr && clock->CurrentState == ClockState::Active)
		return;

	if (solution == nullptr)
		return;

    if (!solution->MoveNext())
    {
        solution = nullptr;
        return;
    }

    MoveStatus nextMove = (MoveStatus) solution->Current;

	int newCol = emptyX;
    int newRow = emptyY;

	switch (nextMove)
	{
	case Left: emptyX++; break;
	case Right: emptyX--; break;
	case Up: emptyY++; break;
	case Down: emptyY--; break;
	}

	Button^ buttonToMove = nullptr;
    for each (Button^ b in Children)
    {
        int r = (int)b->GetValue(RowProperty);
        int c = (int)b->GetValue(ColumnProperty);
		if (c == emptyX && r == emptyY)
		{
			buttonToMove = b;
			break;
		}
	}

	AnimatePiece(buttonToMove, newRow, newCol, nextMove);
}
