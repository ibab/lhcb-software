#ifndef    __DETDESC_VOLUMES_PVOLUME_PVOLUMEEXCEPTION_H__
#define    __DETDESC_VOLUMES_PVOLUME_PVOLUMEEXCEPTION_H__ 1 


#include "GaudiKernel/GaudiException.h"

///
///  inplementation of exception class fro PVolume 
///


class PVolume::PVolumeException : public GaudiException 
{
  ///

 public:

  ///

  PVolumeException( const std::string& name          , 
		    const PVolume*     pvolume =  0  ) 
    : GaudiException( name , "*PVolumeException*" , StatusCode::FAILURE ) 
    , m_pve_pvolume    ( pvolume    ) 
    {} ;

  ///
  ///
  ///
  
  PVolumeException( const std::string&     name          ,
                    const GaudiException&  Exception     ,  
		    const PVolume*         pvolume =  0  ) 
    : GaudiException( name , "*PVolumeException*" , StatusCode::FAILURE , Exception ) 
    , m_pve_pvolume    ( pvolume    ) 
    {} ;
  
  ///
  ///
  ///

  virtual ~PVolumeException(){};

  ///
  ///
  ///

  virtual inline std::ostream& printOut( std::ostream& os = std::cerr ) const 
    {
      GaudiException::printOut( os );
      /// if( 0 != m_pve_pvolume ) { os << "\t for PV" << m_pve_pvolume ; }
      /// else                     { os << "\t for UKNNOWN PVolume "    ; }
      return os;  
    }
 
  ///
  ///
  ///

  virtual inline MsgStream&    printOut( MsgStream&    os             ) const 
    {
      GaudiException::printOut( os );
      ///      if( 0 != m_pve_pvolume ) { os << "\t for PV=" << m_pve_pvolume ; }
      ///      else                     { os << "\t for UKNNOWN Pvolume"      ; }
      return os;  
    } 
  
  ///
  ///
  ///

  virtual inline GaudiException* clone() const  { return  new PVolumeException(*this); };

  ///
  ///
  ///

 private:

  ///
  ///
  ///

  const PVolume*    m_pve_pvolume    ;   // "author" of exceptiom 

  ///
  ///
  ///
};



#endif  // __DETDESC_VOLUMES_PVOLUME_PVOLUMEEXCEPTION_H__

