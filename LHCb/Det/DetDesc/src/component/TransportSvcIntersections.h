#ifndef     __DETDESC_TRANSPORTSVC_TRANSPORTSVCINTERSECTIONS_H__ 
#define     __DETDESC_TRANSPORTSVC_TRANSPORTSVCINTERSECTIONS_H__  1 


#include "DetDesc/TransportSvc.h" 

///
///
///  a simple implementation of TransportSvc::intersection method 
///
///  Author: Vanya Belyaev 
///

///
///
///     general method ( returns the "full history" of the volume boundary intersections 
///     with different material properties between 2 points ) 
///
unsigned long TransportSvc::intersections( const HepPoint3D&        point               ,    /// initial point on the line  
					   const HepVector3D&       vect                ,    /// direction vector of the line 
					   const ISolid::Tick&      tickMin             ,    /// minimal value of line paramater
					   const ISolid::Tick&      tickMax             ,    /// maximal value of line parameter 
					   ILVolume::Intersections& intersept           ,    /// (output) container of intersections
					   double                   threshold           ,    /// threshold value 
					   IGeometryInfo*           alternativeGeometry ,    /// source of alternative geometry information 
					   IGeometryInfo*           guessGeometry       )    /// a guess for navigation 
{
  
  try { 
    ///
    ///
    ///
    
    intersept.clear();
    
    ///
    /// check the input parameters of the line 
    ///
    
    if( tickMin >= tickMax && vect.mag2() <= 0 ) { return 0;}
    
    
    ///
    ///
    ///
    
    HepPoint3D point1( point + vect * tickMin ) ; 
    HepPoint3D point2( point + vect * tickMax ) ; 
    
    
    ///
    /// check - if the previous paramaters are the same
    ///
    if( point1              == m_prevPoint1          && 
	point2              == m_prevPoint2          &&
	threshold           == m_previousThreshold   &&
	alternativeGeometry == m_previousAlternative && 
	guessGeometry       == m_previousGuess         ) 
      {
	///
	/// use cached container!!!
	/// 
	intersept.resize( m_localIntersections.size() ); 
	std::copy( m_localIntersections.begin() , 
		   m_localIntersections.end  () , 
		   intersept.begin()            ); 
	///
	///
	///
	return intersept.size();  
      }
    
    
    ///
    ///
    /// locate the both points inside one "good" DetectorElement/GeometryInfo objects
    /// "Good" in this context means that 
    ///   1) both points should be "inside"
    ///   2) it has the associated Logical Volume (it is not "ghost")
    ///   3) the line between tickMin and tickMax do not cross the Logical Volume
    ///      ("no holes between") requirement
    ///
    
    
    ///
    /// 
    ///
    
    IGeometryInfo* giLocal = 0 ; 
    IGeometryInfo* gi      = 
      ( 0 != guessGeometry       && 0 != ( giLocal = findLocalGI( point1 , point2 , guessGeometry       ) ) ) ? guessGeometry       : 
      ( 0 != alternativeGeometry && 0 != ( giLocal = findLocalGI( point1 , point2 , alternativeGeometry ) ) ) ? alternativeGeometry : 
      ( 0 != previousGeometry()  && 0 != ( giLocal = findLocalGI( point1 , point2 , previousGeometry()  ) ) ) ? previousGeometry()  : 
      ( 0 != standardGeometry()  && 0 != ( giLocal = findLocalGI( point1 , point2 , standardGeometry()  ) ) ) ? standardGeometry()  : 0 ;  
    
    ///
    ///
    ///
    
    Assert( 0 != giLocal , "TransportSvc::(1) unable to locate points" ) ; 
    Assert( 0 != gi      , "TransportSvc::(2) unable to locate points" ) ; 
    ///
    /// redefine previous geometry
    /// 
    
    setPreviousGeometry( giLocal ); 
    
    ///
    ///
    ///

    ///
    /// delegate the calculation to the logical volume 
    ///
    ILVolume* lv = giLocal->lvolume();   
    lv->intersectLine( giLocal->matrix() * point , 
		       giLocal->matrix() * vect  , 
		       intersept                 , 
		       tickMin                   , 
		       tickMax                   , 
		       threshold                 ); 
    
    ///
    /// make local copy of all parameters:
    /// 
    
    m_prevPoint1           = point1              ;
    m_prevPoint2           = point2              ; 
    m_previousThreshold    = threshold           ;
    m_previousGuess        = guessGeometry       ;
    m_previousAlternative  = alternativeGeometry ;
    
    ///  
    /// intersections 
    /// 
    
    m_localIntersections.resize( intersept.size() ); 
    std::copy( intersept.begin           () , 
	       intersept.end             () , 
	       m_localIntersections.begin() );
    
    ///
    ///
    ///
  }
  catch ( const GaudiException& Exception ) 
    {
      ///
      /// 1) reset cache: 
      ///
      m_prevPoint1           = HepPoint3D() ;
      m_prevPoint2           = HepPoint3D() ; 
      m_previousThreshold    = -1000.0      ; 
      m_previousGuess        = 0            ; 
      m_previousAlternative  = 0            ;
      m_localIntersections.clear()          ; 
      
      ///
      ///  2) throw new exception:
      ///
      std::string message("TransportSvc::intersection(...), exception caught; Params: ");
      {
	const unsigned int buflen = 1024;
        char buffer[buflen] = {0,0}; 
	std::ostrstream ost( buffer , buflen ); 
        ost << "Point=" << point 
	    << ",Vect=" << vect
            << ",Tick=" << tickMin << "/" << tickMax  
	    << "Thrsh=" << threshold; 
	if( 0 != alternativeGeometry ) { ost << "A.Geo!=NULL" ; }  
	if( 0 != alternativeGeometry ) { ost << "G.Geo!=NULL" ; }  
	message += ost.str(); 	    
	Assert( false , message , Exception );
      }
    }
  catch( ... ) 
    {
      ///
      /// 1) reset cache: 
      ///
      m_prevPoint1           = HepPoint3D() ;
      m_prevPoint2           = HepPoint3D() ; 
      m_previousThreshold    = -1000.0      ; 
      m_previousGuess        = 0            ; 
      m_previousAlternative  = 0            ;
      m_localIntersections.clear()          ; 
      
      ///
      /// 2) throw new exception:
      ///
      std::string message("TransportSvc::intersection(...), unknown exception caught; Params: ");
      {
	const unsigned int buflen = 1024;
        char buffer[buflen] = {0,0}; 
	std::ostrstream ost( buffer , buflen ); 
        ost << "Point=" << point 
	    << ",Vect=" << vect
            << ",Tick=" << tickMin << "/" << tickMax  
	    << "Thrsh=" << threshold; 
	if( 0 != alternativeGeometry ) { ost << "A.Geo!=NULL" ; }  
	if( 0 != alternativeGeometry ) { ost << "G.Geo!=NULL" ; }  
	message += ost.str(); 	    
	Assert( false , message );
      }
    }


  return intersept.size() ; 

  ///
};

///
///
/// 

#endif  //  __DETDESC_TRANSPORTSVC_TRANSPORTSVCINTERSECTIONS_H__ 
