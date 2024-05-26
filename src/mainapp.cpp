#include "../Headers/mainapp.hpp"
#include "../Headers/mainwindow.hpp"

bool MyApp::OnInit() { //OnInit
	MyWindow* myWindow = new MyWindow();
	return true;
}

/// <summary>
/// Initializes the window.
/// </summary>
MyFrame::MyFrame() : wxFrame(NULL, wxID_ANY, "Nutty Music", wxDefaultPosition, wxSize(1280, 720)) { //App Title is Nutty Music
	this->SetWindowStyle(wxDEFAULT_FRAME_STYLE & ~(wxMAXIMIZE_BOX) & ~(wxRESIZE_BORDER));

	CreateStatusBar();
	SetStatusText("Welcome to Nutty Music!");
}