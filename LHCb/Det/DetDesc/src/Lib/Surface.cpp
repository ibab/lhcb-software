// $Id: Surface.cpp,v 1.9 2009-04-17 08:54:24 cattanem Exp $

// GaudiKernel
#include "GaudiKernel/System.h"
// DetDecs 
#include "DetDesc/Surface.h"
#include "DetDesc/TabulatedProperty.h"


////////////////////////////////////////////////////////////////////////////////
Surface::Surface ( const std::string& /*name*/ ) 
  : DataObject   (        )
  , m_model      ()
  , m_finish     ()
  , m_type       () 
  , m_value      ()
  , m_firstVol   () 
  , m_secondVol  ()
  , m_props      ()
{}
////////////////////////////////////////////////////////////////////////////////
Surface::~Surface() { m_props.clear() ;}
////////////////////////////////////////////////////////////////////////////////
std::ostream&     Surface::fillStream ( std::ostream& s ) const 
{
  s << " Surface " 
    << "  model="      << std::setw(1)   << model  () 
    << "  finish="     << std::setw(1)   << finish () 
    << "  type="       << std::setw(1)   << type   () 
    << "  value="      << std::setw(10)  << value  ()
    << "\tfirstVol='"  << firstVol ()    << "'"
    << "\tsecondVol='" << secondVol()    << "'" 
    << "\t#props="     << std::setw(2)   << m_props.size() 
    << std::endl;
  for (auto it = m_props.begin() ; m_props.end() != it ; ++it)
    { s << "\t" << (*it) ; } 
  return s;
}
////////////////////////////////////////////////////////////////////////////////
MsgStream&        Surface::fillStream ( MsgStream&    s ) const 
{
  s << " Surface " 
    << "  model="      << std::setw(1)  << model  () 
    << "  finish="     << std::setw(1)  << finish () 
    << "  type="       << std::setw(1)  << type   () 
    << "  value="      << std::setw(10) << value  ()
    << "\tfirstVol='"  << firstVol ()   <<"'"
    << "\tsecondVol='" << secondVol()   <<"'" 
    << "\t#props="     << std::setw(2)  << m_props.size() 
    << endmsg ;
  for (auto it = m_props.begin() ; m_props.end() != it ; ++it)
    { s << "\t" << (*it) ; } 
  return s;
}    
////////////////////////////////////////////////////////////////////////////////








