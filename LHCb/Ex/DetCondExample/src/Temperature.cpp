//$Header: /afs/cern.ch/project/cvs/reps/lhcb/Ex/DetCondExample/src/Temperature.cpp,v 1.1.1.1 2001-09-14 15:45:03 andreav Exp $
#include <string> 

#include "Temperature.h"

//---------------------------------------------------------------------------

/// Default constructor
Temperature::Temperature( )
  : ConditionData     (   )
  , m_temperature     ( 0 )
{
}; 

//---------------------------------------------------------------------------

/// Constructor
Temperature::Temperature( const double temperature )
  : ConditionData     (             )
  , m_temperature     ( temperature )
{
}; 

//---------------------------------------------------------------------------

/// Destructor
Temperature::~Temperature() 
{
};

//---------------------------------------------------------------------------

void Temperature::setTemperature ( const double temperature ) { 
  m_temperature = temperature; 
};

//---------------------------------------------------------------------------

const double Temperature::temperature () const { 
  return m_temperature; 
};

//---------------------------------------------------------------------------

/// Printout to a message stream
MsgStream& Temperature::printOut( MsgStream& os ) const {
  return os 
    << "This is a Temperature object. Temperature value = " 
    << temperature();
};

//---------------------------------------------------------------------------

