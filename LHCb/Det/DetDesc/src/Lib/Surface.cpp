// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/src/Lib/Surface.cpp,v 1.6 2002-01-21 14:46:46 sponce Exp $

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
{}; 
////////////////////////////////////////////////////////////////////////////////
Surface::~Surface() { m_props.clear() ;}
////////////////////////////////////////////////////////////////////////////////
StreamBuffer&     Surface::serialize( StreamBuffer& s ) const 
{
  DataObject::serialize( s ) ;
  return s << model     () 
           << finish    ()
           << type      () 
           << value     () 
           << firstVol  () 
           << secondVol () 
           << m_props( this ) ;
};
////////////////////////////////////////////////////////////////////////////////
StreamBuffer&     Surface::serialize( StreamBuffer& s )       
{
  DataObject::serialize( s ) ;
  return s >> m_model      
           >> m_finish    
           >> m_type       
           >> m_value      
           >> m_firstVol   
           >> m_secondVol  
           >> m_props( this ) ;
};
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
  for (Tables::const_iterator it = m_props.begin() ; m_props.end() != it ; ++it)
    { s << "\t" << (*it) ; } 
  return s;
};    
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
    << endreq ;
  for (Tables::const_iterator it = m_props.begin() ; m_props.end() != it ; ++it)
    { s << "\t" << (*it) ; } 
  return s;
};    
////////////////////////////////////////////////////////////////////////////////












