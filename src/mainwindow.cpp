/* Phaethon - A FLOSS resource explorer for BioWare's Aurora engine games
 *
 * Phaethon is the legal property of its developers, whose names
 * can be found in the AUTHORS file distributed with this source
 * distribution.
 *
 * Phaethon is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * Phaethon is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Phaethon. If not, see <http://www.gnu.org/licenses/>.
 */

/** @file mainwindow.cpp
 *  Phaethon's main window.
 */

#include <wx/menu.h>
#include <wx/splitter.h>
#include <wx/artprov.h>

#include <wx/generic/stattextg.h>

#include "common/ustring.h"
#include "common/version.h"
#include "common/util.h"
#include "common/error.h"
#include "common/filepath.h"

#include "cline.h"
#include "eventid.h"
#include "about.h"
#include "mainwindow.h"

wxBEGIN_EVENT_TABLE(MainWindow, wxFrame)
	EVT_MENU(kEventFileOpenDir , MainWindow::onOpenDir)
	EVT_MENU(kEventFileOpenFile, MainWindow::onOpenFile)
	EVT_MENU(kEventFileClose   , MainWindow::onClose)
	EVT_MENU(kEventFileQuit    , MainWindow::onQuit)
	EVT_MENU(kEventHelpAbout   , MainWindow::onAbout)
wxEND_EVENT_TABLE()


MainWindow::MainWindow(const wxString &title, const wxPoint &pos, const wxSize &size) :
	wxFrame(NULL, wxID_ANY, title, pos, size) {

	CreateStatusBar();
	GetStatusBar()->SetStatusText(wxT("Idle..."));

	wxMenu *menuFile = new wxMenu;

	wxMenuItem *menuFileOpenDir =
		new wxMenuItem(0, kEventFileOpenDir, wxT("Open &directory\tCtrl-D"), Common::UString("Open directory"));
	wxMenuItem *menuFileOpenFile =
		new wxMenuItem(0, kEventFileOpenFile, wxT("Open &file\tCtrl-D"), Common::UString("Open file"));
	wxMenuItem *menuFileClose =
		new wxMenuItem(0, kEventFileClose, wxT("&Close\tCtrl-W"), Common::UString("Close"));

	menuFileOpenDir->SetBitmap(wxArtProvider::GetBitmap(wxART_FOLDER_OPEN));
	menuFileOpenFile->SetBitmap(wxArtProvider::GetBitmap(wxART_FILE_OPEN));
	menuFileClose->SetBitmap(wxArtProvider::GetBitmap(wxART_CLOSE));

	menuFile->Append(menuFileOpenDir);
	menuFile->Append(menuFileOpenFile);
	menuFile->AppendSeparator();
	menuFile->Append(menuFileClose);
	menuFile->AppendSeparator();
	menuFile->Append(kEventFileQuit, wxT("&Quit\tCtrl-Q"), Common::UString("Quit ") + PHAETHON_NAME);


	wxMenu *menuHelp = new wxMenu;
	menuHelp->Append(kEventHelpAbout, wxT("&About\tF1"), Common::UString("About ") + PHAETHON_NAME);

	wxMenuBar *menuBar = new wxMenuBar;
	menuBar->Append(menuFile, wxT("&File"));
	menuBar->Append(menuHelp, wxT("&Help"));

	SetMenuBar(menuBar);


	wxSplitterWindow *splitterMainLog     = new wxSplitterWindow(this, wxID_ANY);
	wxSplitterWindow *splitterTreeRes     = new wxSplitterWindow(splitterMainLog, wxID_ANY);
	wxSplitterWindow *splitterInfoPreview = new wxSplitterWindow(splitterTreeRes, wxID_ANY);

	wxPanel *panelLog     = new wxPanel(splitterMainLog    , wxID_ANY);
	wxPanel *panelPreview = new wxPanel(splitterInfoPreview, wxID_ANY);
	wxPanel *panelInfo    = new wxPanel(splitterInfoPreview, wxID_ANY);
	wxPanel *panelTree    = new wxPanel(splitterTreeRes    , wxID_ANY);

	_resourceTree = new wxTreeCtrl(panelTree, wxID_ANY, wxDefaultPosition, wxDefaultSize,
	                               wxTR_HAS_BUTTONS | wxTR_SINGLE);

	wxGenericStaticText *info    = new wxGenericStaticText(panelInfo   , wxID_ANY, wxT("Info..."));
	wxGenericStaticText *preview = new wxGenericStaticText(panelPreview, wxID_ANY, wxT("Preview..."));

	wxTextCtrl *log = new wxTextCtrl(panelLog, wxID_ANY, wxEmptyString, wxDefaultPosition,
	                                 wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY);

	wxBoxSizer *sizerWindow = new wxBoxSizer(wxVERTICAL);

	wxStaticBoxSizer *sizerLog     = new wxStaticBoxSizer(wxHORIZONTAL, panelLog    , wxT("Log"));
	wxStaticBoxSizer *sizerPreview = new wxStaticBoxSizer(wxHORIZONTAL, panelPreview, wxT("Preview"));
	wxStaticBoxSizer *sizerInfo    = new wxStaticBoxSizer(wxHORIZONTAL, panelInfo   , wxT("Resource info"));
	wxStaticBoxSizer *sizerTree    = new wxStaticBoxSizer(wxHORIZONTAL, panelTree   , wxT("Resources"));

	sizerTree->Add(_resourceTree, 1, wxEXPAND, 0);
	panelTree->SetSizer(sizerTree);

	sizerInfo->Add(info, 0, 0, 0);
	panelInfo->SetSizer(sizerInfo);

	sizerPreview->Add(preview, 0, 0, 0);
	panelPreview->SetSizer(sizerPreview);

	sizerLog->Add(log, 1, wxEXPAND, 0);
	panelLog->SetSizer(sizerLog);

	splitterInfoPreview->SetMinimumPaneSize(20);
	splitterTreeRes->SetMinimumPaneSize(20);
	splitterMainLog->SetMinimumPaneSize(20);

	splitterMainLog->SetSashGravity(1.0);

	splitterInfoPreview->SplitHorizontally(panelInfo, panelPreview);
	splitterTreeRes->SplitVertically(panelTree, splitterInfoPreview);
	splitterMainLog->SplitHorizontally(splitterTreeRes, panelLog);

	sizerWindow->Add(splitterMainLog, 1, wxEXPAND, 0);
	SetSizer(sizerWindow);

	Layout();

	splitterInfoPreview->SetSashPosition(150);
	splitterTreeRes->SetSashPosition(200);
	splitterMainLog->SetSashPosition(480);
}

MainWindow::~MainWindow() {
}

void MainWindow::onQuit(wxCommandEvent &event) {
	Close(true);
}

void MainWindow::onAbout(wxCommandEvent &event) {
	AboutDialog *about = new AboutDialog(this);
	about->show();
}

void MainWindow::onOpenDir(wxCommandEvent &event) {
	wxDirDialog dialog(this, wxT("Open Aurora game directory"), wxEmptyString,
	                   wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);

	if (dialog.ShowModal() != wxID_OK)
		return;

	open(dialog.GetPath());
}

void MainWindow::onOpenFile(wxCommandEvent &event) {
	wxFileDialog dialog(this, wxT("Open Aurora game resource file"), wxEmptyString, wxEmptyString,
	                    wxT("Aurora game resource (*.*)|*.*"), wxFD_DEFAULT_STYLE | wxFD_FILE_MUST_EXIST);

	if (dialog.ShowModal() != wxID_OK)
		return;

	open(dialog.GetPath());
}

void MainWindow::onClose(wxCommandEvent &event) {
	close();
}

void MainWindow::forceRedraw() {
	Refresh();
	Update();
}

bool MainWindow::open(Common::UString path) {
	close();

	if (!Common::FilePath::isDirectory(path) && !Common::FilePath::isRegularFile(path)) {
		warning("Path \"%s\" is neither a directory nor a regular file", path.c_str());
		return false;
	}

	path = Common::FilePath::normalize(path);

	if (Common::FilePath::isDirectory(path))
		GetStatusBar()->PushStatusText(Common::UString("Recursively adding all files in ") + path + "...");
	else
		GetStatusBar()->PushStatusText(Common::UString("Adding file ") + path + "...");

	forceRedraw();

	try {
		_files.readPath(path, -1);
	} catch (Common::Exception &e) {
		GetStatusBar()->PopStatusText();

		Common::printException(e, "WARNING: ");
		return false;
	}

	GetStatusBar()->PopStatusText();

	_path = path;

	populateTree();

	return true;
}

void MainWindow::close() {
	_resourceTree->DeleteAllItems();

	_files.clear();
	_path.clear();
}

void MainWindow::populateTree(const Common::FileTree::Entry &e, wxTreeItemId t) {
	for (std::list<Common::FileTree::Entry>::const_iterator c = e.children.begin();
	     c != e.children.end(); ++c) {

		wxTreeItemId cT = _resourceTree->AppendItem(t, c->name);
		populateTree(*c, cT);
	}
}

void MainWindow::populateTree() {
	const Common::FileTree::Entry &fileRoot = _files.getRoot();

	wxTreeItemId treeRoot = _resourceTree->AddRoot(fileRoot.name);

	populateTree(fileRoot, treeRoot);
}
