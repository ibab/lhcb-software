/// GaudiKernel
#include "GaudiKernel/MsgStream.h"
// DetDesc 
#include "DetDesc/LVolume.h"
#include "DetDesc/LVolumeException.h"
///

//////////////////////////////////////////////////////////////////////////////////////////
LVolumeException::LVolumeException( const std::string& name    , 
				    const LVolume*     lvolume , 
				    const StatusCode&  sc      ) 
  : GaudiException( name , "*LVolumeException*" , sc)
  , m_lve_lvolume   ( lvolume    ) 
{} ;
//////////////////////////////////////////////////////////////////////////////////////////
LVolumeException::LVolumeException( const std::string   & name       ,
				    const GaudiException& Exception  , 
				    const LVolume*        lvolume    , 
				    const StatusCode&     sc         ) 
  : GaudiException( name , "*LVolumeException*" , sc , Exception )
  , m_lve_lvolume   ( lvolume    ) 
{} ;
//////////////////////////////////////////////////////////////////////////////////////////
LVolumeException::~LVolumeException(){};
//////////////////////////////////////////////////////////////////////////////////////////
std::ostream& LVolumeException::printOut( std::ostream& os             ) const 
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
  os << " \t" << m_lve_lvolume << std::endl ; 
  ///
  return ( 0 != previous() ) ? previous()->printOut( os ) : os ; 
  ///
};
//////////////////////////////////////////////////////////////////////////////////////////
MsgStream&    LVolumeException::printOut( MsgStream&    os             ) const 
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
  os << " \t" << m_lve_lvolume << endreq;
  ///
  return ( 0 != previous() ) ? previous()->printOut( os ) : os ; 
  ///
};
//////////////////////////////////////////////////////////////////////////////////////////
