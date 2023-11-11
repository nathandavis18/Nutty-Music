#pragma once
#include <filesystem>
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

class MyApp : public wxApp {
public:
	virtual bool OnInit();
};

class MyFrame : public wxFrame {
public:
	MyFrame();
private:
	void OnClose(wxCloseEvent& event);
};

enum { ID_Hello = 1 };