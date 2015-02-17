// ====================================================================
//  UPI Dialog menu classes
// --------------------------------------------------------------------
//
//	Author    : Markus Frank
//
// ====================================================================
#ifndef _UPIDIALOGMENU_H
#define _UPIDIALOGMENU_H 1

#include "UPI/upidef.h"
#include "UPI/UpiSensor.h"
#include "GaudiUPI/DialogMenu.h"

class UpiFactory;
class UpiDialogMenu;

typedef UpiDialogMenu UpiDialogSubMenu;

class UpiFactory : public DialogFactory   {
protected:
  UpiFactory ();
public:
  virtual ~UpiFactory();
  static DialogFactory* instance ();
  virtual DialogMenu* createMenu (CPP::Interactor* actor = 0,DialogMenu* parent = 0,ClientData data = 0);
  virtual DialogSubMenu* createSubMenu (CPP::Interactor* actor = 0,DialogMenu* master = 0,ClientData data = 0);
};

class UpiDialogMenu : public DialogMenu   {
public:
  UpiDialogMenu (CPP::Interactor* actor = 0,DialogMenu* parent = 0,ClientData data = 0);
  UpiDialogMenu (int menuId);
  virtual ~UpiDialogMenu();
  virtual int id();
  virtual DialogStatus window();
  virtual DialogMenu* create (const std::string& title = "",const std::string& utit = "",const std::string& ltit = "",int cmd = 0,int father = 0);
  virtual DialogMenu* createDetached (const std::string& title = "",const std::string& utit = "",const std::string& ltit = "",int cmd = 0,int father = 0);
  virtual DialogMenu* createSubMenu (const std::string& title = "",const std::string& utit = "");
  virtual DialogStatus map();
  virtual DialogStatus replaceTitle (const std::string& a = "",const std::string& b = "",const std::string& c = "");
  virtual DialogStatus destroy();
  virtual DialogStatus setCursor (int cmd = (-1));
  virtual DialogStatus guideCursor (int cmd = (-1));
  virtual DialogStatus addCMD (int cmd,const char* fmt, ...);
  virtual DialogStatus replCMD (int cmd,const char* fmt, ...);
  virtual DialogStatus disableCMD (int cmd);
  virtual DialogStatus enableCMD (int cmd);
  DialogStatus insertCMD (int pos_id,int cmd_id,const char* fmt, ...);
  DialogStatus deleteCMD (int line);
  DialogStatus addCOM (int cmd,const char* fmt, ...);
  DialogStatus replCOM (int cmd,const char* fmt, ...);
  DialogStatus insertCOM (int pos_id,int com_id,const char* fmt, ...);
  DialogStatus deleteCOM (int line);
  DisplayState state() const;
  static DialogStatus enableCursor();
  static DialogStatus disableCursor();
  DialogStatus installBackspaceCall();
  DialogStatus addPAR (int cmd,DialogItem* par);
  DialogStatus addPAR (int cmd,DialogItem** par);
  DialogStatus replace (DialogItem* par);
  DialogStatus replace (DialogItem** par);
  DialogStatus addButtonLine (int cmd,DialogItem* par);
  DialogStatus replaceButtonLine (DialogItem* par);
  virtual DialogStatus reOpen();
  virtual DialogMenu* place (int row = (-1),int col = (-1));
protected:
  int m_id;
};
#endif
