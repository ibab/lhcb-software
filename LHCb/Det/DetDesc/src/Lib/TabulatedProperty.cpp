/// STL
#include <string>
#include <vector>
/// GaudiKernel
#include "GaudiKernel/IAxis.h"
#include "GaudiKernel/IHistogram1D.h"
/// DetDesc 
#include "DetDesc/TabulatedProperty.h"


/////////////////////////////////////////////////////////////////////////////////
TabulatedProperty::TabulatedProperty( const std::string& Name )
  : DataObject( Name        )
  , m_type    ( "(Unknown)" ) 
  , m_xAxis   ( "(empty)"   ) 
  , m_yAxis   ( "(empty)"   ) 
  , m_table   (             ) 
{};
/////////////////////////////////////////////////////////////////////////////////
TabulatedProperty::~TabulatedProperty(){ table().clear(); }
/////////////////////////////////////////////////////////////////////////////////
StreamBuffer& TabulatedProperty::serialize  ( StreamBuffer& s ) const 
{ 
  DataObject::serialize( s );
  s << type ()  <<  xAxis()  << yAxis() ;
  s << table().size() ; 
  for( Table::const_iterator it = table().begin() ; table().end() !=it ; ++it )
    { s << it->first << it->second ; }
  return s;
};
/////////////////////////////////////////////////////////////////////////////////
StreamBuffer& TabulatedProperty::serialize  ( StreamBuffer& s )       
{
  DataObject::serialize( s ) ;
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
/////////////////////////////////////////////////////////////////////////////////
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
/////////////////////////////////////////////////////////////////////////////////
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
/////////////////////////////////////////////////////////////////////////////////












