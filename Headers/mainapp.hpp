#pragma once
#include <filesystem>
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

class MyApp : public wxApp {
public:
	virtual bool OnInit();
private:
};

class MyFrame : public wxFrame {
public:
	MyFrame();
};

enum { ID_Hello = 1 };