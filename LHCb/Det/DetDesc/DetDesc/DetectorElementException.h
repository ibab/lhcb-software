// $Id: DetectorElementException.h,v 1.5 2003-04-25 08:52:23 sponce Exp $
#ifndef       DETDESC_DetectorElementEXCEPTION_H
#define       DETDESC_DetectorElementEXCEPTION_H 1 
/// GaudiKernel 
#include "GaudiKernel/GaudiException.h" 
//
class DetectorElement;
class MsgStream;          

/** @class DetectorElementException DetectorElementException.h DetDesc/DetectorElementException.h

    Exception used in DetectorElement class 
    
    @author Vanya Belyaev 
*/



class DetectorElementException : public GaudiException 
{  
  ///
public:
  /// constructor 
  DetectorElementException( const std::string     & name                                   , 
                            const DetectorElement * DetectorElement = 0                    , 
                            const StatusCode      &  sc             = StatusCode::FAILURE  ); 
  /// constructor from exception 
  DetectorElementException( const std::string     & name                                    , 
                            const GaudiException  & Exception                               , 
                            const DetectorElement * DetectorElement   = 0                   , 
                            const StatusCode      & sc                = StatusCode::FAILURE  ); 
  /// virtual destructor 
  virtual ~DetectorElementException() throw();
  ///
  virtual std::ostream&    printOut ( std::ostream& os = std::cerr ) const ; 
  ///
  virtual MsgStream&       printOut ( MsgStream&                   ) const ; 
  ///
  virtual GaudiException*  clone    ()                               const ;
  ///
 private:
  ///
  const DetectorElement*    m_dee_DetectorElement    ;
  ///
};
///

#endif  //    DETDESC_DetectorElementEXCEPTION_H












