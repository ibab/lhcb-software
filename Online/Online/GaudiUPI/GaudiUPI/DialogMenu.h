// DECLARATIONS FOR OMT MODULE dialogmenu
#ifndef _dialogmenu_H
#define _dialogmenu_H 1


#include <string>
#include "CPP/Interactor.h"
#include "GaudiUPI/DialogItem.h"


enum DialogMenuCalls
{
  DIALOGMENU_BACK_SPACE = (-1)
};
enum DisplayState
{
  DIALOG_DISP_ERROR = 0,
  DIALOG_DISP_CLOSED,
  DIALOG_DISP_OPEN,
  DIALOG_DISP_TEST
};
enum DialogStatus
{
  DIALOG_ERROR = 0,
  DIALOG_SUCCESS = 1
};


class DialogMenu;
class DialogFactory;
class DialogSubMenuCreator;
class DialogMenuCreator;

typedef long (*BackSpaceCall)(int,int,int,ClientData);
typedef DialogMenu DialogSubMenu;

class DialogMenu : public CPP::Interactor
{
public:
  DialogMenu (CPP::Interactor* actor = 0,DialogMenu* parent = 0,ClientData /* data */ = 0) 
    : m_actor(actor), m_parent(parent) {}
    virtual ~DialogMenu() {}
    virtual int          id() {return (-1);}
    virtual void         handle (const CPP::Event& event);
    virtual DialogStatus installBackspaceCall() {return DIALOG_SUCCESS;}
    virtual Interactor*  actor() {return m_actor;}
    virtual DialogStatus window() {return DIALOG_ERROR;}
    virtual DialogStatus reOpen() {return DIALOG_SUCCESS;}
    virtual DialogStatus replaceTitle (const std::string& = "",const std::string& = "",const std::string& = "") {return DIALOG_SUCCESS;}
    virtual DialogMenu*  create (const std::string& = "",const std::string& = "",const std::string& = "",int = 0,int = 0) {return (DialogMenu*)NULL;}
    virtual DialogMenu*  createDetached (const std::string& = "",const std::string& = "",const std::string& = "",int = 0,int = 0) {
      return (DialogMenu*)NULL;
    }
    virtual DialogMenu*  createSubMenu (const std::string& = "",const std::string& = 0) {return (DialogMenu*)NULL;}
    virtual DialogStatus map() {return DIALOG_SUCCESS;}
    virtual DialogStatus destroy() {return DIALOG_SUCCESS;}
    virtual DialogStatus addCMD (int,const char*, ...) {return DIALOG_SUCCESS;}
    virtual DialogStatus replCMD (int,const char*, ...) {return DIALOG_SUCCESS;}
    virtual DialogStatus addCOM (int,const char*, ...) {return DIALOG_SUCCESS;}
    virtual DialogStatus replCOM (int,const char*, ...) {return DIALOG_SUCCESS;}
    virtual DialogStatus insertCOM (int,int, const char*, ...) {return DIALOG_SUCCESS;}
    virtual DialogStatus deleteCOM (int) {return DIALOG_SUCCESS;}
    virtual DialogStatus deleteCMD (int) {return DIALOG_SUCCESS;}
    virtual DialogStatus insertCMD (int,int,const char*, ...) {return DIALOG_SUCCESS;}
    virtual DialogStatus setCursor (int = (-1)) {return DIALOG_SUCCESS;}
    virtual DialogStatus guideCursor (int = (-1)) {return DIALOG_SUCCESS;}
    static DialogStatus  enableCursor() {return DIALOG_SUCCESS;}
    static DialogStatus  disableCursor() {return DIALOG_SUCCESS;}
    virtual DisplayState state() const {return DIALOG_DISP_ERROR;}
    virtual DialogStatus accumulateComment (int pos_id,int& com_id,int max_lines,char* def_text,std::string& display_text);
    virtual DialogStatus flushComment (int pos_id,int& com_id,int max_lines,char* def_text,std::string& display_text);
    virtual DialogStatus disableCMD (int) {return DIALOG_SUCCESS;}
    virtual DialogStatus enableCMD (int) {return  DIALOG_SUCCESS;}
    static long          backSpaceCallBack (int,int,int,ClientData);
    virtual DialogStatus addPAR (int,DialogItem* ) {return DIALOG_SUCCESS;}
    virtual DialogStatus addPAR (int,DialogItem**) {return DIALOG_SUCCESS;}
    virtual DialogStatus replace (DialogItem* ) {return DIALOG_SUCCESS;}
    virtual DialogStatus replace (DialogItem**) {return DIALOG_SUCCESS;}
    virtual DialogStatus addButtonLine (int,DialogItem*) {return DIALOG_SUCCESS;}
    virtual DialogStatus replaceButtonLine (DialogItem*) {return DIALOG_SUCCESS;}
    virtual DialogMenu*  place (int = (-1),int = (-1)) {return this;}
protected:
  Interactor*  m_actor;
  DisplayState m_state;
  DialogMenu*  m_parent;
};

class DialogFactory   {
protected:
  DialogFactory ();
public:
  virtual ~DialogFactory();
  static DialogFactory* instance ();
  virtual DialogMenu* createMenu (CPP::Interactor* actor = 0,DialogMenu* parent = 0,ClientData data = 0) = 0;
  virtual DialogSubMenu* createSubMenu (CPP::Interactor* actor = 0,DialogMenu* master = 0,ClientData data = 0) = 0;
};

class DialogSubMenuCreator    {
protected:
  DialogMenu* m_menu;
public:
  DialogSubMenuCreator (Interactor* actor = 0,DialogMenu* par = 0,ClientData dat = 0) 
  { m_menu = DialogFactory::instance()->createSubMenu(actor,par,dat);   }
  virtual ~DialogSubMenuCreator()           {                           }
  virtual DialogSubMenu* operator->() const {    return m_menu;         }
};

class DialogMenuCreator   {
protected:
  DialogMenu* m_menu;
public:
  DialogMenuCreator (CPP::Interactor* actor = 0,DialogMenu* parent = 0,ClientData data = 0) 
  { m_menu = DialogFactory::instance()->createMenu(actor,parent,data);  }
  virtual ~DialogMenuCreator()            {                             }
  virtual DialogMenu* operator->() const  {    return m_menu;           }
};
#endif
