#ifndef        __DETDESC_SOLID_SOLIDEXCEPTION_H__
#define        __DETDESC_SOLID_SOLIDEXCEPTION_H__

#include <strstream> 


#include "GaudiKernel/ISolid.h" 

#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/GaudiException.h" 



class SolidException : public GaudiException 
{  
  ///
  ///
  ///
 
 public:

  ///
  ///
  ///
  
  SolidException( const std::string& message      , 
		  const ISolid*      solid  =  0  ) 
    : GaudiException( message , "*SolidException*" , StatusCode::FAILURE ) 
    , m_se_solid    ( solid      ) 
    { } ;

  ///
  ///
  ///
  
  SolidException( const std::string   &  message      ,
                  const GaudiException&  Exception    ,  
		  const ISolid*          solid  =  0  ) 
    : GaudiException( message , "*SolidException*" , StatusCode::FAILURE , Exception ) 
    , m_se_solid    ( solid      ) 
    { } ;
  
  ///
  ///
  ///

  virtual ~SolidException(){};

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
      
      /// if( 0 != m_se_solid ) { os << "\t for Solid=" << m_se_solid ; }
      /// else                  { os << "\t for UKNNOWN Solid"        ; }
      
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
      
      os  << " \t " << message() ; 
      
      ///
      
      switch( code() )
        {
        case StatusCode::SUCCESS : os << "\t StatusCode=SUCCESS"    ;  break ; 
        case StatusCode::FAILURE : os << "\t StatusCode=FAILURE"    ;  break ; 
        default                  : os << "\t StatusCode=" << code() ;  break ; 
        }
      
      ///
      
      /// if( 0 != m_se_solid ) 
      ///	{ os << "\t for Solid(Name/Type)=(" << m_se_solid->name() << "/" << m_se_solid->typeName() << ")" ; }
      /// else                  
      /// { os << "\t for UKNNOWN Solid"        ; }
      
      ///
      /// do we have exceptions in the linked list? 
      /// 

      if( 0 != previous() ) { os << endreq ; previous()->printOut( os ) ; }  

      ///

      return os ; 

      ///

    } 
  
  ///
  ///
  ///

  virtual inline GaudiException* clone() const  { return  new SolidException(*this); };

 private:

  ///

  const ISolid*     m_se_solid         ;       // the "author" of the exception 

  ///
  
};


#endif    //   __DETDESC_SOLID_SOLIDEXCEPTION_H__

