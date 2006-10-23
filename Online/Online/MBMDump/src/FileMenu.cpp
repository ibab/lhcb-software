#include "MBMDump/MBMDump.h"
#include "UPI/upidef.h"
using namespace MBMDump;

FileMenu::FileMenu(FileMainMenu* par,int cmd_id) : BaseMenu(par), m_parentCmd(cmd_id)
{
  m_file = dynamic_cast<FileMainMenu*>(par);
  ::strncpy(m_fileName,par->fileName(),sizeof(m_fileName));
  ::upic_open_detached_menu(id(),parent().id(),cmd_id,"Supply file name","",procName());
  ::upic_set_param(m_fileName,1,"%96s",m_fileName,0,0,0,0,0);
  ::upic_add_command(1,"File Name:^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^","");
  ::upic_enable_action_routine(id(),1,Routine(BaseMenu::dispatch));
  ::upic_close_menu();
}

FileMenu::~FileMenu() {
  ::upic_delete_menu(id());
}

void FileMenu::handleMenu(int cmd_id)   {
  if ( cmd_id == 1 ) m_file->acceptFile(m_fileName);
}

