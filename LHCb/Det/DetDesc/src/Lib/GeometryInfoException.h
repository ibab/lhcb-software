#ifndef     __DETDESC_GEOMETRYINFO_GEOMETRYINFOEXCEPTION_H__
#define     __DETDESC_GEOMETRYINFO_GEOMETRYINFOEXCEPTION_H__ 1 


#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/GaudiException.h" 

///
///
///  class GeometryInfo::GeometryInfoException   
///          exception class used for GeometryInfo Objects
///
///
///


class GeometryInfoException : public GaudiException 
{  
 public:
  /// constructor 
  GeometryInfoException( const std::string  & name     , 
			 const GeometryInfo * gi   = 0 ,
                         const StatusCode   & sc = StatusCode::FAILURE )
    : GaudiException( name , "*GeometryInfoException*" , sc )
    , m_gie_geometryInfo  ( gi   ) 	
    {};
  GeometryInfoException( const std::string    & name      , 
                         const GaudiException & ge        , 
			 const GeometryInfo   * gi   =  0 ,
                         const StatusCode     & sc = StatusCode::FAILURE )
    : GaudiException( name , "*GeometryInfoException*" , sc , ge )
    , m_gie_geometryInfo  ( gi   ) 	
    {};	
  /// destructor 
  virtual ~GeometryInfoException(){};
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
      if( 0 != m_gie_geometryInfo ) { os << "\t for GI=" << m_gie_geometryInfo        ; }
      else                          { os << "\t for UKNNOWN GeometryInfo object "     ; }
      /// do we have exceptions in the linked list? 
      if( 0 != previous() ) { os << std::endl            ; } 
      if( 0 != previous() ) { previous()->printOut( os ) ; }  
      return os ; 
    }
  ///
  virtual inline MsgStream&    printOut( MsgStream&    os             ) const 
    {
      /// 
      os << " \t" << " \t " << message() ; 
      ///
      switch( code() )
	{
	case StatusCode::SUCCESS : os << "\t StatusCode=SUCCESS"    ;  break ; 
	case StatusCode::FAILURE : os << "\t StatusCode=FAILURE"    ;  break ; 
	default                  : os << "\t StatusCode=" << code() ;  break ; 
	}
      ///      
      if( 0 != m_gie_geometryInfo ) { os << "\t for GI=" << m_gie_geometryInfo        ; }
      else                          { os << "\t for UKNNOWN GeometryInfo object "     ; }
      /// do we have exceptions in the linked list? 
      if( 0 != previous() ) { os << endreq               ; } 
      if( 0 != previous() ) { previous()->printOut( os ) ; }  
      ///
      return os ; 
      ///
    }
  ///
  virtual inline GaudiException* clone() const  { return  new GeometryInfoException(*this); };  
  ///
private:
  ///
  const GeometryInfo*   m_gie_geometryInfo  ; 
  ///
};

///
///
///

#endif  //  __DETDESC_GEOMETRYINFO_GEOMETRYINFOEXCEPTION_H__
