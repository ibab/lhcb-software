#include "UPI/UpiSensor.h"
#include "CPP/Interactor.h"
#include "CPP/Event.h"
#include "UPI/upidef.h"
#include "WT/wtdef.h"
#include "WT/wt_facilities.h"

static int s_menuID = 1234;

//----------------------------------------------------------------------------
UpiSensor& UpiSensor::instance()  {
  static UpiSensor inst;
  return inst;
}
//----------------------------------------------------------------------------
int UpiSensor::newID() {
  return ++s_menuID;
}
//----------------------------------------------------------------------------
UpiSensor::UpiSensor() : Sensor(WT_FACILITY_UPI,"UpiSensor",true) {
  upic_attach_terminal();
}

//----------------------------------------------------------------------------
UpiSensor::~UpiSensor()   {
  m_table.clear();
}

//----------------------------------------------------------------------------
void UpiSensor::add(Interactor* interactor, int menu) {
  Table::iterator i=m_table.find(menu);
  if ( i != m_table.end() ) m_table.erase(i);
  m_table.insert(std::make_pair(menu,interactor));
}

//----------------------------------------------------------------------------
void UpiSensor::remove( Interactor* /* interactor */, int menu )   {
  Table::iterator i=m_table.find(menu);
  if ( i != m_table.end() ) m_table.erase(i);
}

//----------------------------------------------------------------------------
void UpiSensor::dispatch(void* /* param */)    {
  int menu, command, param, index;
  int status = upic_get_input_with_index( &menu, &command, &param, &index );
  if ( status == UPI_NORMAL )  {
    Table::iterator i=m_table.find(menu);
    if ( i != m_table.end() )  {
      Event       ev;
      ev.target     = (*i).second;
      ev.eventtype  = UpiEvent;
      ev.menu_id    = menu;
      ev.command_id = command;
      ev.param_id   = param;
      ev.index_id   = index;
      ev.target->handle(ev);       
    }
    else  {  
      upic_write_message("Got UPI Input not associated with any Source","");
    }
  }
}
