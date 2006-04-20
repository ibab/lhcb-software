// $Id: GeometryInfoException.h,v 1.3 2006-04-20 14:39:56 ranjard Exp $
#ifndef     DETDESC_GEOMETRYINFOEXCEPTION_H
#define     DETDESC_GEOMETRYINFOEXCEPTION_H 1 
/// GaudiKernel
#include "GaudiKernel/GaudiException.h" 
///
class MsgStream;
class IGeometryInfo;
///

/** @class GeometryInfoException GeometryInfoException.h DetDesc/GeometryInfoException.h

    Exception class used in GeometryInfo objects 
    
    @author Vanya Belyaev 


    Change to deal with IGeometryInfo

    @author Juan Palacios

*/

class GeometryInfoException : public GaudiException 
{  
public:
  /// constructor 
  GeometryInfoException( const std::string  & name     , 
                         const IGeometryInfo * gi   = 0 ,
                         const StatusCode   & sc = StatusCode::FAILURE );
  ///
  GeometryInfoException( const std::string    & name      , 
                         const GaudiException & ge        , 
                         const IGeometryInfo   * gi   =  0 ,
                         const StatusCode     & sc = StatusCode::FAILURE ) ;
  /// destructor 
  virtual ~GeometryInfoException() throw();
  ///
  virtual std::ostream& printOut( std::ostream& os = std::cerr ) const ; 
  virtual MsgStream&    printOut( MsgStream&    os             ) const ;
  ///
  virtual GaudiException* clone() const;  
  ///
private:
  ///
  const IGeometryInfo*   m_gie_geometryInfo  ; 
  ///
};
///

#endif  //  DETDESC_GEOMETRYINFOEXCEPTION_H

