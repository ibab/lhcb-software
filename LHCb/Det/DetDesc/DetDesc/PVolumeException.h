// $Id: PVolumeException.h,v 1.1 2006-04-20 14:39:56 ranjard Exp $
#ifndef    DETDESC_PVOLUMEEXCEPTION_H
#define    DETDESC_PVOLUMEEXCEPTION_H 1 
///
#include "GaudiKernel/GaudiException.h"
///
class PVolume; 

///
class PVolumeException : public GaudiException 
{
  ///
public:
  ///
  PVolumeException( const std::string& name          , 
                    const PVolume*     pvolume =  0  ) ;
  ///
  PVolumeException( const std::string&     name          ,
                    const GaudiException&  Exception     ,  
                    const PVolume*         pvolume =  0  );
  ///
  virtual ~PVolumeException() throw();
  ///
  virtual std::ostream&   printOut ( std::ostream& os = std::cerr ) const ;
  virtual MsgStream&      printOut ( MsgStream&    os             ) const ; 
  virtual GaudiException* clone    ()                               const ;
  ///
private:
  ///
  const PVolume*    m_pve_pvolume    ;   // "author" of exceptiom 
  ///
};
///


#endif  // DETDESC_PVOLUMEEXCEPTION_H











