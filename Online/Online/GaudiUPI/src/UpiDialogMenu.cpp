#include "GaudiUPI/UpiDialogMenu.h"
#include "UPI/UpiSensor.h"
#include "RTL/rtl.h"
#include <cstdarg>

using namespace std;
#define __START_PARSER(x)                   \
  char buffer[512];                         \
  {                                         \
  int len;                                  \
  va_list arguments;                        \
  va_start( arguments, x);                  \
  len = vsprintf(buffer,x,arguments);       \
  buffer[len] = 0;                          \
  va_end ( arguments );                     }

static bool __cursor_enabled = true;
static int  __absID = 1;

UpiFactory::UpiFactory() : DialogFactory() {
}

UpiFactory::~UpiFactory() {
}

DialogFactory* UpiFactory::instance()  {
  static DialogFactory *factory = 0;
  if ( factory == NULL ) factory = new UpiFactory();
  return factory;
}

DialogMenu* UpiFactory::createMenu (Interactor* actor,DialogMenu* parent,ClientData data) {
  UpiDialogMenu *menu = new UpiDialogMenu(actor,parent,data);
  return menu;
}

DialogSubMenu* UpiFactory::createSubMenu (Interactor* actor,DialogMenu* master,ClientData data) {
  UpiDialogSubMenu *menu = new UpiDialogSubMenu(actor,master,data);
  return menu;
}


UpiDialogMenu::UpiDialogMenu (Interactor* actor,DialogMenu* parent,ClientData data)
: DialogMenu(actor, parent,data), m_id(0)
{
  m_state = DIALOG_DISP_CLOSED;
}

UpiDialogMenu::UpiDialogMenu (int menuId) : m_id(menuId)
{
  m_state = DIALOG_DISP_CLOSED;
}

UpiDialogMenu::~UpiDialogMenu() {
  UpiDialogMenu::destroy();
}

DialogMenu* UpiDialogMenu::create (const string& title,const string& utit,const string& ltit,int cmd,int father)
{
  m_state=DIALOG_DISP_OPEN;
  upic_open_menu(m_id = __absID++,father,cmd,title.c_str(),utit.c_str(),ltit.c_str());
  UpiSensor::instance().add(this, m_id );
  return this;
}

DialogMenu* UpiDialogMenu::createDetached (const string& title,
                                           const string& utit,
                                           const string& ltit,
                                           int /* cmd */, int /* father */ )
{
  upic_open_detached_menu(m_id = __absID++,0,0,title.c_str(),utit.c_str(),ltit.c_str());
  UpiSensor::instance().add(this, m_id );
  return this;
}

DialogMenu* UpiDialogMenu::createSubMenu (const string& title,const string& utit) {
  return create("", title, utit);
}

DialogStatus UpiDialogMenu::destroy()
{
  int status = DIALOG_SUCCESS;
  if ( m_id != 0 )  {
    UpiSensor::instance().remove(this, m_id);
    status = upic_delete_menu(m_id);
    m_id = 0;
  }
  m_state = DIALOG_DISP_CLOSED;
  return ::lib_rtl_is_success(status) ? DIALOG_SUCCESS : DIALOG_ERROR;
}

DialogStatus UpiDialogMenu::window()  {
  int status = upic_open_window();
  m_state = DIALOG_DISP_OPEN;
  return ::lib_rtl_is_success(status) ? DIALOG_SUCCESS : DIALOG_ERROR;
}

DialogStatus UpiDialogMenu::map() {
  m_state = DIALOG_DISP_OPEN;
  return ::lib_rtl_is_success(upic_close_menu()) ? DIALOG_SUCCESS : DIALOG_ERROR;
}

DialogStatus UpiDialogMenu::reOpen()  {
  int status = 0;
  if ( m_id != 0 ) status = upic_open_old_window(m_id);
  return ::lib_rtl_is_success(status) ? DIALOG_SUCCESS : DIALOG_ERROR;
}

int UpiDialogMenu::id() {
  return m_id;
}

DialogStatus UpiDialogMenu::replaceTitle (const string& a,const string& b,const string& c)  {
  int status = DIALOG_ERROR;
  if ( m_id > 0 ) status = upic_change_titles(m_id,a.c_str(), b.c_str(), c.c_str());
  return ::lib_rtl_is_success(status) ? DIALOG_SUCCESS : DIALOG_ERROR;
}

DialogStatus UpiDialogMenu::addCMD (int cmd,const char* fmt, ...) {
  __START_PARSER(fmt);
  int status = DIALOG_SUCCESS;  
  if ( m_id > 0 ) upic_add_command(cmd,buffer,"");
  return ::lib_rtl_is_success(status) ? DIALOG_SUCCESS : DIALOG_ERROR;
}

DialogStatus UpiDialogMenu::replCMD (int cmd,const char* fmt, ...)  {
  __START_PARSER(fmt);
  int status = DIALOG_SUCCESS;  
  if ( m_id > 0 ) upic_replace_comment(m_id,cmd,buffer,"");
  return ::lib_rtl_is_success(status) ? DIALOG_SUCCESS : DIALOG_ERROR;
}

DialogStatus UpiDialogMenu::insertCMD (int pos_id,int cmd_id,const char* fmt, ...)  {
  __START_PARSER(fmt);
  int status = DIALOG_SUCCESS;  
  if ( m_id > 0 ) upic_insert_command(m_id,pos_id,cmd_id,buffer,"");
  return ::lib_rtl_is_success(status) ? DIALOG_SUCCESS : DIALOG_ERROR;
}

DialogStatus UpiDialogMenu::deleteCMD (int line)
{
  int status = DIALOG_SUCCESS;  
  if ( m_id > 0 ) upic_delete_command(m_id,line);
  return ::lib_rtl_is_success(status) ? DIALOG_SUCCESS : DIALOG_ERROR;
}

DialogStatus UpiDialogMenu::addCOM (int cmd,const char* fmt, ...) {
  __START_PARSER(fmt);
  int status = DIALOG_SUCCESS;  
  if ( m_id > 0 ) upic_add_comment(cmd,buffer,"");
  return ::lib_rtl_is_success(status) ? DIALOG_SUCCESS : DIALOG_ERROR;
}

DialogStatus UpiDialogMenu::replCOM (int cmd,const char* fmt, ...)  {
  __START_PARSER(fmt);
  int status = DIALOG_SUCCESS;  
  if ( m_id > 0 ) upic_replace_comment(m_id,cmd,buffer,"");
  return ::lib_rtl_is_success(status) ? DIALOG_SUCCESS : DIALOG_ERROR;
}

DialogStatus UpiDialogMenu::insertCOM (int pos_id,int com_id,const char* fmt, ...)  {
  __START_PARSER(fmt);
  int status = DIALOG_SUCCESS;  
  if ( m_id > 0 ) upic_insert_comment(m_id,pos_id,com_id,buffer,"");
  return ::lib_rtl_is_success(status) ? DIALOG_SUCCESS : DIALOG_ERROR;
}

DialogStatus UpiDialogMenu::deleteCOM (int line)
{
  int status = DIALOG_SUCCESS;  
  if ( m_id > 0 ) upic_delete_comment(m_id,line);
  return ::lib_rtl_is_success(status) ? DIALOG_SUCCESS : DIALOG_ERROR;
}

DialogStatus UpiDialogMenu::enableCursor()
{
  upic_write_message("Cursor is now ENABLED!","");
  __cursor_enabled = 1;
  return DIALOG_SUCCESS;
}

DialogStatus UpiDialogMenu::disableCursor()
{
  upic_write_message("Cursor is now DISABLED!","");
  __cursor_enabled = 0;
  return DIALOG_SUCCESS;
}

DialogStatus UpiDialogMenu::installBackspaceCall()  {
  int status = DIALOG_SUCCESS;  
  Interactor *target = (actor() != 0 ) ? actor() : this;
  if ( m_id > 0 )   {
    status = upic_declare_callback(m_id,CALL_ON_BACK_SPACE,(Routine)backSpaceCallBack,(void*)target);
  }
  return ::lib_rtl_is_success(status) ? DIALOG_SUCCESS : DIALOG_ERROR;
}

DialogStatus UpiDialogMenu::setCursor (int cmd) {
  int status = ( m_id == 0 ) ? DIALOG_SUCCESS : upic_set_cursor(m_id,cmd,0);
  return ::lib_rtl_is_success(status) ? DIALOG_SUCCESS : DIALOG_ERROR;
}

DialogStatus UpiDialogMenu::guideCursor (int cmd) {
  int status = DIALOG_SUCCESS;  
  if (__cursor_enabled && m_id != 0 ) status  = upic_set_cursor(m_id,cmd,0);
  return ::lib_rtl_is_success(status) ? DIALOG_SUCCESS : DIALOG_ERROR;
}

DialogStatus UpiDialogMenu::disableCMD (int cmd)  {
  int status = DIALOG_SUCCESS;  
  if ( m_id > 0 ) status = upic_disable_command(m_id,cmd);
  return ::lib_rtl_is_success(status) ? DIALOG_SUCCESS : DIALOG_ERROR;
}

DialogStatus UpiDialogMenu::enableCMD (int cmd)
{
  int status = DIALOG_SUCCESS;  
  if ( m_id != 0 ) status = upic_enable_command(m_id,cmd);
  return ::lib_rtl_is_success(status) ? DIALOG_SUCCESS : DIALOG_ERROR;
}

DisplayState UpiDialogMenu::state()  const  {
  return m_state;
}

DialogStatus UpiDialogMenu::addPAR (int cmd,DialogItem* p)    {
  char* c = 0;
  upic_set_param (p->buffer(),
    1,
    p->format().c_str(),
    p->current(),
    p->min(),
    p->max(),
    p->list(),
    p->length(),
    p->useList());
  p->setID(c+cmd);
  if ( m_id != 0 ) upic_add_param_line(cmd,p->text().c_str(),"");
  return DIALOG_SUCCESS;
}

DialogStatus UpiDialogMenu::addPAR (int cmd,DialogItem** p)  {
  char* c = 0;
  string text;
  for ( int i = 0; p[i] != NULL; i++ )  {
    upic_set_param (p[i]->buffer(),
      i+1,
      p[i]->format().c_str(),
      p[i]->current(),
      p[i]->min(),
      p[i]->max(),
      p[i]->list(),
      p[i]->length(),
      p[i]->useList());
    text += p[i]->text();
    p[i]->setID(c+cmd);
  }
  if ( m_id != 0 ) upic_add_param_line(cmd,text.c_str(),"");
  return DIALOG_SUCCESS;
}

DialogStatus UpiDialogMenu::replace (DialogItem* p)  {
  upic_set_param (p->buffer(),
    1,
    p->format().c_str(),
    p->current(),
    p->min(),
    p->max(),
    p->list(),
    p->length(),
    p->useList());
  if ( m_id != 0 ) 
    upic_replace_param_line(m_id,(long)p->id(),p->text().c_str(),"");
  return DIALOG_SUCCESS;
}

DialogStatus UpiDialogMenu::replace (DialogItem** p)  {
  string text;
  for ( int i = 0; p[i]; i++ )  {
    upic_set_param (p[i]->buffer(),
      i+1,
      p[i]->format().c_str(),
      p[i]->current(),
      p[i]->min(),
      p[i]->max(),
      p[i]->list(),
      p[i]->length(),
      p[i]->useList());
    text += p[i]->text();
  }
  if ( m_id != 0 )
    upic_replace_param_line((int)m_id,(long)p[0]->id(),text.c_str(), "");
  return DIALOG_SUCCESS;
}

DialogStatus UpiDialogMenu::addButtonLine (int cmd,DialogItem* p)   {
  char* c = 0;
  for(int i=0; i < p->length(); i++ )
    upic_set_param (p->buffer(),i+1,p->format().c_str(),p->item(i),0,0,0,0,1);
  p->setID(c+cmd);
  if ( m_id != 0 ) 
    upic_add_param_line(cmd,p->text().c_str(),"");
  return DIALOG_SUCCESS;
}

DialogStatus UpiDialogMenu::replaceButtonLine (DialogItem* p) {
  for(int i=0; i < p->length(); i++ )
    upic_set_param (p->buffer(),i+1,p->format().c_str(),p->item(i),0,0,0,0,1);
  if ( m_id != 0 ) 
    upic_replace_param_line((int)m_id,(long)p->id(),p->text().c_str(),"");
  return DIALOG_SUCCESS;
}

DialogMenu* UpiDialogMenu::place (int row,int col)  {
  if ( m_id != 0 && row >= 0 && col >= 0 ) {
    /* int status = */ ::upic_set_window_position ( m_id, row, col );
  }
  return this;
}
