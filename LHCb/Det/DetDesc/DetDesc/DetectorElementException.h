#ifndef       __DETDESC_DetectorElementEXCEPTION_H__
#define       __DETDESC_DetectorElementEXCEPTION_H__ 1 


class DetectorElement;
class DetectorElement;

#include "GaudiKernel/GaudiException.h" 

///  implementation of class DetectorElement::DetectorElementException 

class DetectorElement::DetectorElementException : public GaudiException 
{  
  ///
 public:
  ///
  DetectorElementException( const std::string     & name                                   , 
			    const DetectorElement * DetectorElement = 0                    , 
			    const StatusCode      &  sc             = StatusCode::FAILURE  ) 
    : GaudiException( name , "*DetElemException*" , sc)
    , m_dee_DetectorElement   ( DetectorElement   ) 
    {} ;
  ///
  DetectorElementException( const std::string     & name                                    , 
			    const GaudiException  & Exception                               , 
			    const DetectorElement * DetectorElement   = 0                   , 
			    const StatusCode      & sc                = StatusCode::FAILURE  ) 
    : GaudiException( name , "*DetElemException*" , sc , Exception )
    , m_dee_DetectorElement   ( DetectorElement    ) 
    {} ;
  /// destructor 
  virtual ~DetectorElementException(){};  
  ///
  virtual inline std::ostream& printOut( std::ostream& os = std::cerr ) const 
    {
      os << " \t" << tag() << " \t " << message() ; 
      switch( code() )
        {
        case StatusCode::SUCCESS : os << "\t StatusCode=SUCCESS"    ;  break ; 
        case StatusCode::FAILURE : os << "\t StatusCode=FAILURE"    ;  break ; 
        default                  : os << "\t StatusCode=" << code() ;  break ; 
        }
      if( 0 != m_dee_DetectorElement ) { os << "\t for DetectorElement=" << m_dee_DetectorElement ; }
      else                             { os << "\t for UKNNOWN DetectorElement"           ; }
      /// do we have exceptions in the linked list? 
      if( 0 != previous() ) { os << std::endl ; previous()->printOut( os ) ; }  
      ///
      return os ; 
    } 
  ///
  virtual inline MsgStream&    printOut( MsgStream&    os             ) const 
    {
      os << " \t"                    << message() ; 
      switch( code() )
        {
        case StatusCode::SUCCESS : os << "\t StatusCode=SUCCESS"    ;  break ; 
        case StatusCode::FAILURE : os << "\t StatusCode=FAILURE"    ;  break ; 
        default                  : os << "\t StatusCode=" << code() ;  break ; 
        }
      ///
      if( 0 != m_dee_DetectorElement ) { os << "\t for DetectorElement=" << m_dee_DetectorElement ; }
      else                             { os << "\t for UKNNOWN DetectorElement"           ; }      
      ///
      /// do we have exceptions in the linked list? 
      if( 0 != previous() ) { os << endreq    ; previous()->printOut( os ) ; }  
      return os ; 
    }
  ///
  virtual inline GaudiException* clone() const  { return  new DetectorElementException(*this); };
  ///
 private:
  ///
  const DetectorElement*    m_dee_DetectorElement    ;
  ///
};

///
///
/// 

#endif  //    __DETDESC_DetectorElementEXCEPTION_H__





