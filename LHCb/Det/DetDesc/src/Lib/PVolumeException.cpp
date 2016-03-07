// $Id: PVolumeException.cpp,v 1.7 2009-04-17 08:54:24 cattanem Exp $

// GaudiKernel
#include "GaudiKernel/MsgStream.h"

#include "DetDesc/PVolume.h"
#include "DetDesc/PVolumeException.h"
 


/////////////////////////////////////////////////////////////////////////////////////////
PVolumeException::PVolumeException( const std::string& name    , 
                                    const PVolume*     pvolume ) 
  : GaudiException( name , "*PVolumeException*" , StatusCode::FAILURE ) 
  , m_pve_pvolume    ( pvolume    ) 
{}
/////////////////////////////////////////////////////////////////////////////////////////
PVolumeException::PVolumeException( const std::string&     name      ,
                                    const GaudiException&  Exception ,  
                                    const PVolume*         pvolume     ) 
  : GaudiException( name , "*PVolumeException*" , StatusCode::FAILURE , Exception ) 
  , m_pve_pvolume    ( pvolume    ) 
{}
/////////////////////////////////////////////////////////////////////////////////////////
PVolumeException::~PVolumeException() throw() {}
/////////////////////////////////////////////////////////////////////////////////////////
std::ostream& PVolumeException::printOut( std::ostream& os             ) const 
{
  ///
  os << " \t" << tag() << " \t " << message() ; 
  ///  
  switch( code() )
    {
    case StatusCode::SUCCESS : os << "\t StatusCode=SUCCESS"    ;  break ; 
    case StatusCode::FAILURE : os << "\t StatusCode=FAILURE"    ;  break ; 
    default                  : os << "\t StatusCode=" << code() ;  break ; 
    }
  ///
  os << " \t" << m_pve_pvolume << std::endl ; 
  ///
  return ( 0 != previous() ) ? previous()->printOut( os ) : os ; 
  ///
}
//////////////////////////////////////////////////////////////////////////////////////////
MsgStream&    PVolumeException::printOut( MsgStream&    os             ) const 
{
  ///
  os << " \t" << tag() << " \t " << message() ; 
  ///  
  switch( code() )
    {
    case StatusCode::SUCCESS : os << "\t StatusCode=SUCCESS"    ;  break ; 
    case StatusCode::FAILURE : os << "\t StatusCode=FAILURE"    ;  break ; 
    default                  : os << "\t StatusCode=" << code() ;  break ; 
    }
  ///
  os << " \t" << m_pve_pvolume << endmsg;
  ///
  return ( 0 != previous() ) ? previous()->printOut( os ) : os ; 
  ///
}
//////////////////////////////////////////////////////////////////////////////////////////
GaudiException* PVolumeException::clone() const  { return  new PVolumeException(*this); }
/////////////////////////////////////////////////////////////////////////////////////////
