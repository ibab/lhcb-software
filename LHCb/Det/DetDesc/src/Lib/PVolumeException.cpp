// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/src/Lib/PVolumeException.cpp,v 1.3 2001-03-13 11:58:08 mato Exp $

// GaudiKernel
#include "GaudiKernel/MsgStream.h"
// local
#include "PVolume.h"
#include "PVolumeException.h"
 


/////////////////////////////////////////////////////////////////////////////////////////
PVolumeException::PVolumeException( const std::string& name    , 
                                    const PVolume*     pvolume ) 
  : GaudiException( name , "*PVolumeException*" , StatusCode::FAILURE ) 
  , m_pve_pvolume    ( pvolume    ) 
{} ;
/////////////////////////////////////////////////////////////////////////////////////////
PVolumeException::PVolumeException( const std::string&     name      ,
                                    const GaudiException&  Exception ,  
                                    const PVolume*         pvolume     ) 
  : GaudiException( name , "*PVolumeException*" , StatusCode::FAILURE , Exception ) 
  , m_pve_pvolume    ( pvolume    ) 
{} ;
/////////////////////////////////////////////////////////////////////////////////////////
PVolumeException::~PVolumeException(){};
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
};
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
  os << " \t" << m_pve_pvolume << endreq;
  ///
  return ( 0 != previous() ) ? previous()->printOut( os ) : os ; 
  ///
};
//////////////////////////////////////////////////////////////////////////////////////////
GaudiException* PVolumeException::clone() const  { return  new PVolumeException(*this); };
/////////////////////////////////////////////////////////////////////////////////////////
