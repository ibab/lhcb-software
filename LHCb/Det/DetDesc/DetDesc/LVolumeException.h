#ifndef       DETDESC_LVOLUMEXCEPTION_H
#define       DETDESC_LVOLUMEXCEPTION_H 1 
///   GaudiKernel
#include "GaudiKernel/GaudiException.h" 
///
class LVolume; 
///

class LVolumeException : public GaudiException 
{  
  ///
public:
  ///
  LVolumeException( const std::string&    name                             , 
                    const LVolume*        lvolume   = 0                    , 
                    const StatusCode&     sc        = StatusCode::FAILURE  ); 
  ///
  LVolumeException( const std::string   & name                             , 
                    const GaudiException& Exception                        , 
                    const LVolume*        lvolume   = 0                    , 
                    const StatusCode&     sc        = StatusCode::FAILURE  ); 
  ///
  virtual ~LVolumeException();
  /// 
  virtual inline std::ostream& printOut( std::ostream& os = std::cerr ) const ; 
  virtual inline MsgStream&    printOut( MsgStream&    os             ) const ;
  virtual inline GaudiException* clone() const  { return  new LVolumeException(*this); };  
  ///
 private:
  ///  
  const LVolume*    m_lve_lvolume    ;
  ///
};
/// 

#endif  //    DETDESC_LVOLUMEXCEPTION_H
