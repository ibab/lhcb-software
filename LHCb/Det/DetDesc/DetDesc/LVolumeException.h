#ifndef       __DETDESC_VOLUMES_LVOLUMELVOLUMEXCEPTION_H__
#define       __DETDESC_VOLUMES_LVOLUMELVOLUMEXCEPTION_H__ 1 


class LVolume;

#include "GaudiKernel/GaudiException.h" 

///
///  implementation of class LVolume::LVolumeException 
///
///


///
///
///

class LVolume::LVolumeException : public GaudiException 
{  

  ///
  ///
  /// 

 public:

  ///
  ///
  ///

  LVolumeException( const std::string& name                           , 
		    const LVolume*     lvolume = 0                    , 
		    const StatusCode&  sc      = StatusCode::FAILURE  ) 
    : GaudiException( name , "*LVolumeException*" , sc)
    , m_lve_lvolume   ( lvolume    ) 
    {} ;

  ///
  ///
  ///
  
  LVolumeException( const std::string   & name                             , 
                    const GaudiException& Exception                        , 
		    const LVolume*        lvolume   = 0                    , 
		    const StatusCode&     sc        = StatusCode::FAILURE  ) 
    : GaudiException( name , "*LVolumeException*" , sc , Exception )
    , m_lve_lvolume   ( lvolume    ) 
    {} ;

  ///
  /// destructor 
  ///

  virtual ~LVolumeException(){};
  
  ///
  /// 
  ///
  
  virtual inline std::ostream& printOut( std::ostream& os = std::cerr ) const 
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
      
      /// if( 0 != m_lve_lvolume ) { os << "\t for LVolume=" << m_lve_lvolume ; }
      /// else                     { os << "\t for UKNNOWN LVolume"           ; }
      
      ///
      /// do we have exceptions in the linked list? 
      /// 

      if( 0 != previous() ) { os << std::endl ; previous()->printOut( os ) ; }  

      ///

      return os ; 

      ///

    }
 
  ///
  ///
  ///
  
  virtual inline MsgStream&    printOut( MsgStream&    os             ) const 
    {
      /// 
      
      os << " \t"                    << message() ; 
      
      ///
      
      switch( code() )
        {
        case StatusCode::SUCCESS : os << "\t StatusCode=SUCCESS"    ;  break ; 
        case StatusCode::FAILURE : os << "\t StatusCode=FAILURE"    ;  break ; 
        default                  : os << "\t StatusCode=" << code() ;  break ; 
        }
      
      ///
      
      /// if( 0 != m_lve_lvolume ) { os << "\t for LVolume=" << m_lve_lvolume ; }
      /// else                     { os << "\t for UKNNOWN LVolume"           ; }
      
      ///
      /// do we have exceptions in the linked list? 
      /// 

      if( 0 != previous() ) { os << endreq    ; previous()->printOut( os ) ; }  

      ///

      return os ; 

      ///
    }
 
  ///
  ///
  ///
  
  virtual inline GaudiException* clone() const  { return  new LVolumeException(*this); };
  
  ///
  ///
  ///
  
 private:
  
  ///
  /// "author of exeption"
  ///
  
  const LVolume*    m_lve_lvolume    ;

  ///
  ///
  ///

};

///
///
/// 

#endif  //    __DETDESC_VOLUMES_LVOLUMELVOLUMEXCEPTION_H__
