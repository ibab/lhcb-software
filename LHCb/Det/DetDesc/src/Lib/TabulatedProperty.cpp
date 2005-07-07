// $Id: TabulatedProperty.cpp,v 1.7 2005-07-07 12:20:38 marcocle Exp $

// DetDesc 
#include "DetDesc/TabulatedProperty.h"

// STL
#include <string>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
TabulatedProperty::TabulatedProperty( const std::string& /*Name*/ )
  : ValidDataObject( )
  , m_type    ( "(Unknown)" ) 
  , m_xAxis   ( "(empty)"   ) 
  , m_yAxis   ( "(empty)"   ) 
  , m_table   (             ) 
{};
////////////////////////////////////////////////////////////////////////////////
TabulatedProperty::~TabulatedProperty(){ table().clear(); }
////////////////////////////////////////////////////////////////////////////////
StreamBuffer& TabulatedProperty::serialize  ( StreamBuffer& s ) const 
{ 
  ValidDataObject::serialize( s );
  s << type ()  <<  xAxis()  << yAxis() ;
  s << table().size() ; 
  for( Table::const_iterator it = table().begin() ; table().end() !=it ; ++it )
    { s << it->first << it->second ; }
  return s;
};
////////////////////////////////////////////////////////////////////////////////
StreamBuffer& TabulatedProperty::serialize  ( StreamBuffer& s )       
{
  ValidDataObject::serialize( s ) ;
  s >> m_type >> m_xAxis >> m_yAxis ;
  ///
  Table::size_type  size     ; 
  s >> size                  ;
  table() = Table( size , Entry() ) ;
  for( Table::iterator it = table().begin() ; table().end() != it ; ++it )
    { s >> it->first >> it->second ; } 
  ///
  return s;
}
////////////////////////////////////////////////////////////////////////////////
std::ostream& TabulatedProperty::fillStream ( std::ostream& s ) const 
{
  s << " \tTabulatedProperty " 
    << " \ttype="      << type () 
    << " \txAxis="     << xAxis() 
    << " \tyAxis="     << yAxis() 
    << " \t#enties="   << table().size() << std::endl; 
  for( Table::const_iterator it = table().begin() ; table().end() != it ; ++it )
    {
      s << " \t\t\t( entry#"    
        << std::setw(2) << it-table().begin()
        << "   "      
        << std::setw(12) << it->first 
        << "   "      
        << std::setw(12) << it->second << " ) " << std::endl;
    }
  return s ;
};
////////////////////////////////////////////////////////////////////////////////
MsgStream&    TabulatedProperty::fillStream ( MsgStream&    s ) const 
{
  s << " \tTabulatedProperty " 
    << " \ttype="       << type() 
    << " \txAxis="      << xAxis() 
    << " \tyAxis="      << yAxis() 
    << " \t#entries="   << table().size() << endreq; 
  for( Table::const_iterator it = table().begin() ; table().end() != it ; ++it )
    {
      s << " \t\t\t( entry#" 
        << std::setw(2) << it-table().begin()
        << "   "      
        << std::setw(12) << it->first 
        << "   " 
        << std::setw(12) << it->second << " )" << endreq ;
    }
  return s ;
};
////////////////////////////////////////////////////////////////////////////////












