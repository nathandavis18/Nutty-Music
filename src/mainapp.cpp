#include "../Headers/mainapp.hpp"
#include "../Headers/mainwindow.hpp"

bool MyApp::OnInit() { //OnInit
	MyWindow* myWindow = new MyWindow();
	return true;
}

MyFrame::MyFrame() : wxFrame(NULL, wxID_ANY, "Nutty Music", wxDefaultPosition, wxSize(1280, 720)) { //App Title is Nutty Music
	this->SetWindowStyle(wxDEFAULT_FRAME_STYLE & ~(wxMAXIMIZE_BOX) & ~(wxRESIZE_BORDER));
	wxMenu* menuFile = new wxMenu;
	menuFile->Append(ID_Hello, "&Hello...\tCtrl-H", "Help string shown in status bar for this menu item");

	wxMenu* menuHelp = new wxMenu;
	menuHelp->Append(wxID_ABOUT);

	wxMenuBar* menuBar = new wxMenuBar;
	menuBar->Append(menuFile, "&File");
	menuBar->Append(menuHelp, "&Help");

	SetMenuBar(menuBar);

	CreateStatusBar();
	SetStatusText("Welcome to wxWidgets!");

	Bind(wxEVT_MENU, [=](wxCommandEvent&) { wxMessageBox("Test", "Testing Test", wxOK | wxICON_INFORMATION);  }, wxID_ABOUT);
	Bind(wxEVT_MENU, [=](wxCommandEvent&) { wxLogMessage("Hello");  }, ID_Hello);
}