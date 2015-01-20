#ifndef DISPLAY_H
#define DISPLAY_H
// Mainframe macro generated from application: /usr/local/root/bin/root.exe
// By ROOT version 5.12/00 on 2007-05-29 20:52:48

#if !defined( __CINT__) || defined (__MAKECINT__)

#ifndef ROOT_TGDockableFrame
#include "TGDockableFrame.h"
#endif
#ifndef ROOT_TGMenu
#include "TGMenu.h"
#endif
#ifndef ROOT_TGMdiDecorFrame
#include "TGMdiDecorFrame.h"
#endif
#ifndef ROOT_TG3DLine
#include "TG3DLine.h"
#endif
#ifndef ROOT_TGMdiFrame
#include "TGMdiFrame.h"
#endif
#ifndef ROOT_TGMdiMainFrame
#include "TGMdiMainFrame.h"
#endif
#ifndef ROOT_TGuiBldHintsButton
#include "TGuiBldHintsButton.h"
#endif
#ifndef ROOT_TGMdiMenu
#include "TGMdiMenu.h"
#endif
#ifndef ROOT_TGListBox
#include "TGListBox.h"
#endif
#ifndef ROOT_TGNumberEntry
#include "TGNumberEntry.h"
#endif
#ifndef ROOT_TGScrollBar
#include "TGScrollBar.h"
#endif
#ifndef ROOT_TGuiBldHintsEditor
#include "TGuiBldHintsEditor.h"
#endif
#ifndef ROOT_TGFileDialog
#include "TGFileDialog.h"
#endif
#ifndef ROOT_TGShutter
#include "TGShutter.h"
#endif
#ifndef ROOT_TGButtonGroup
#include "TGButtonGroup.h"
#endif
#ifndef ROOT_TGCanvas
#include "TGCanvas.h"
#endif
#ifndef ROOT_TGFSContainer
#include "TGFSContainer.h"
#endif
#ifndef ROOT_TGButton
#include "TGButton.h"
#endif
#ifndef ROOT_TGuiBldEditor
#include "TGuiBldEditor.h"
#endif
#ifndef ROOT_TGTextEdit
#include "TGTextEdit.h"
#endif
#ifndef ROOT_TGSplitter
#include "TGSplitter.h"
#endif
#ifndef ROOT_TGFSComboBox
#include "TGFSComboBox.h"
#endif
#ifndef ROOT_TGLabel
#include "TGLabel.h"
#endif
#ifndef ROOT_TGView
#include "TGView.h"
#endif
#ifndef ROOT_TGMsgBox
#include "TGMsgBox.h"
#endif
#ifndef ROOT_TRootGuiBuilder
#include "TRootGuiBuilder.h"
#endif
#ifndef ROOT_TGTab
#include "TGTab.h"
#endif
#ifndef ROOT_TGListView
#include "TGListView.h"
#endif
#ifndef ROOT_TGStatusBar
#include "TGStatusBar.h"
#endif
#ifndef ROOT_TGToolTip
#include "TGToolTip.h"
#endif
#ifndef ROOT_TGToolBar
#include "TGToolBar.h"
#endif
#ifndef ROOT_TRootEmbeddedCanvas
#include "TRootEmbeddedCanvas.h"
#endif
#ifndef ROOT_TCanvas
#include "TCanvas.h"
#endif
#ifndef ROOT_TGuiBldDragManager
#include "TGuiBldDragManager.h"
#endif
#include "Riostream.h"
#endif

#include <iostream>
using namespace std;
#include "container_ROOT.h"

class InfoWindow:public TGMainFrame
{
  
  TGMainFrame * fMainFrame892;
  TRootEmbeddedCanvas *fRootEmbeddedCanvas514; 
  TCanvas *c123 ;
  TGStatusBar *fStatusBar528;
  TGTextEdit *fTextEdit532;
  
  int lines;
  int * alive;

  container_ROOT cont;

  int numGraphs, numTexts;
  

public:

  InfoWindow(int *);
  ~InfoWindow();
  
  void ShowCont(const std::string& ins);

  void textclear();
  void appendline(const char * fn);
  void DoClose();
  void DoResize();
  void raise();
  void Layout();
  void display(int x=600,int y=700);
  
  TCanvas * canvas();
  TGTextEdit * textedit(){return fTextEdit532;}
  void print(const char * name = ""){
    if (*alive ==2){
      if (name[0] == '\0') 
        canvas()->Print("canvas.png");
      else
        canvas()->Print(name);
    }
    if (fTextEdit532!=NULL){
      fTextEdit532->SaveFile("details.txt");
    }
  }
  
  
  //  ClassDef(InfoWindow,1)
};
#endif
