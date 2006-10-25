// DECLARATIONS FOR OMT MODULE upidialog
#ifndef _upidialog_H
#define _upidialog_H 1

#include <cstdio>
#include <stdarg.h>

#include <string>
#include "UPI/upidef.h"
#include "CPP/DialogMenu.h"

typedef long (*BackSpaceCall)(int,int,int,ClientData);

class UpiDialog   {
  bool cursorEnabled() const;
public:
  UpiDialog (int id);
  UpiDialog();
  ~UpiDialog() {}
  //  Standard destructor
  //      Arguments: None
  virtual int window() {upic_open_window();return DIALOG_DISP_OPEN;}
  virtual void Open (int father,int cmd,char* title,char* utit,char* ltit) 
  {m_state=DIALOG_DISP_OPEN;upic_open_menu(m_id,father,cmd,title,utit,ltit);}
  virtual void Open (char* title,char* utit,char* ltit) {Open(0,0,title,utit,ltit);}
  virtual void OpenDetached (char* title,char* utit,char* ltit) 
  {m_state=DIALOG_DISP_OPEN;upic_open_detached_menu(m_id,0,0,title,utit,ltit);}
  virtual void SetCursor (int cmd) {if (cursorEnabled()) upic_set_cursor(m_id,cmd,0);}
  virtual void SetCursor (int id,int cmd) {if (cursorEnabled()) upic_set_cursor(id,cmd,0);}
  virtual void AddCMD (int cmd,const char* fmt, ...);
  virtual void ReplCMD (int cmd,const char* fmt, ...);
  virtual int DisableDialog (int cmd) {return upic_disable_command(m_id,cmd);}
  virtual int EnableDialog (int cmd) {return upic_enable_command(m_id,cmd);}
  virtual void AddCOM (int cmd,const char* fmt, ...);
  virtual void ReplCOM (int cmd,const char* fmt, ...);
  virtual void AddPAR (int cmd,const char* fmt, ...);
  virtual void ReplPAR (int cmd,const char* fmt, ...);
  virtual void setParam (void* a1,int a2,const std::string& a3,int a4,int a5,int a6,void* a7,int a8,int a9);
  virtual void setParam (void* a1,int a2,const std::string& a3,float a4,float a5,float a6,void* a7,int a8,int a9);
  virtual void setParam (void* a1,int a2,const std::string& a3,void* a4,void* a5,void* a6,void* a7,int a8,int a9);
  virtual void ModPAR (int a2,int a3,void* a4,void* a5,void* a6,void* a7,int a8,int a9);
  virtual void Write (char* buffer);
  virtual void close() {upic_close_menu();}
  virtual void Delete() {upic_delete_menu(m_id);}
  virtual void DeleteIfWindowOpen (DisplayState* state);
  virtual DisplayState State() { return m_state; }
  virtual void replaceTitle (char* a,char* b,char* c);
  virtual bool Connected();
  virtual void InstallBackSpaceCall (BackSpaceCall callback,ClientData par);
  virtual void InstallBackSpaceCall (int id,BackSpaceCall callback,ClientData par);
  virtual void EnableCursor();
  virtual void DisableCursor();
  virtual void insertComment (int pos_id,int& com_id,int max_lines,char* def_text,std::string& display_text);
  virtual void FlushComment (int pos_id,int& com_id,int max_lines,char* def_text,std::string& display_text);
  virtual void InsertCOM (int id,int pos_id,int com_id,const char* fmt, ...);
  virtual void InsertCOM (int pos_id,int com_id,const char* fmt, ...);
  virtual void InsertCMD (int id,int pos_id,int cmd_id,const char* fmt, ...);
  virtual void InsertCMD (int pos_id,int cmd_id,const char* fmt, ...);
  virtual void DeleteComment (int line);
  virtual void DeleteCommand (int line);
protected:
  int m_id;
  DisplayState m_state;
};

#endif
