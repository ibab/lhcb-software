// $Id: TransportSvcIntersections.h,v 1.5 2007-09-13 14:19:16 wouter Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
#ifndef     __DETDESC_TRANSPORTSVC_TRANSPORTSVCINTERSECTIONS_H__ 
#define     __DETDESC_TRANSPORTSVC_TRANSPORTSVCINTERSECTIONS_H__  1

#include "TransportSvc.h" 

// ============================================================================
/** @file TransportSvcIntersections.h
 * 
 *  a simple implementation of TransportSvc::intersection method 
 *  @see TransportSvc 
 *  @see ITransportSvc 
 *  @see ILVolume  
 *  @see IPVolume  
 *  @see ISolid 
 *
 *  @author: Vanya Belyaev 
 */
// ============================================================================

// ============================================================================
/** general method ( returns the "full history" of the volume 
 *  boundary intersections 
 * with different material properties between 2 points )
 *  @see ITransportSvc
 *  @see IGeometryInfo
 *  @see ILVolume
 *  @param Point               initial point on the line  
 *  @param Vector              direction vector of the line 
 *  @param TickMin             minimal value of line paramater
 *  @param TickMax             maximal value of line parameter 
 *  @param Intersept           (output) container of intersections
 *  @param Threshold           threshold value 
 *  @param AlternativeGeometry source of alternative geometry information 
 *  @param GeometryGuess       a guess for navigation 
 */
// ============================================================================
unsigned long TransportSvc::intersections
( const Gaudi::XYZPoint&   point               , 
  const Gaudi::XYZVector&  vect                , 
  const ISolid::Tick&      tickMin             , 
  const ISolid::Tick&      tickMax             , 
  ILVolume::Intersections& intersept           , 
  double                   threshold           , 
  IGeometryInfo*           alternativeGeometry , 
  IGeometryInfo*           guessGeometry       ) const
{

  try { 
    
    intersept.clear();
    
    /// check the input parameters of the line 
    if( tickMin >= tickMax && vect.mag2() <= 0 ) { return 0;}
    
    // Set the top level geometry, because this is what is in the cache
    IGeometryInfo* topGeometry = alternativeGeometry ? alternativeGeometry : standardGeometry() ;
 
    ///
    Gaudi::XYZPoint point1( point + vect * tickMin ) ; 
    Gaudi::XYZPoint point2( point + vect * tickMax ) ; 
    /// check - if the previous paramaters are the same
    if( point1              == m_prevPoint1          && 
        point2              == m_prevPoint2          &&
        threshold           == m_previousThreshold   &&
        topGeometry         == m_previousTopGeometry && 
        guessGeometry       == m_previousGuess         ) 
      {
        /// use cached container!!!
        intersept.reserve( m_localIntersections.size() ); 
        intersept.insert(intersept.begin(),
                         m_localIntersections.begin() , 
                         m_localIntersections.end()); 
        ///
        return intersept.size();  
      }
    
    /** locate the both points inside one "good" 
     *  DetectorElement/GeometryInfo objects
     * 
     * "Good" in this context means that 
     * 
     *  - 1) both points should be "inside"
     *  - 2) it has the associated Logical Volume (it is not "ghost")
     *  - 3) Logical Volume is not an assembly! 
     *  - 4) the line between tickMin and tickMax do not cross 
     *    the Logical Volume ("no holes between") requirement
     */
    
    IGeometryInfo* giLocal = 0 ; 
    IGeometryInfo* gi      = 
      // try the guess geometry
     ( 0 != guessGeometry       && 
        0 != ( giLocal = findLocalGI( point1 , 
                                      point2 , 
                                      guessGeometry ,
                                      alternativeGeometry ) ) ) ? 
      guessGeometry       : 
      // try the previous geometry (only if top-geometry matches)
      ( previousGeometry() && topGeometry == m_previousTopGeometry &&
	0 != ( giLocal = findLocalGI( point1 , 
                                      point2 , 
                                      previousGeometry() ,
                                      topGeometry ) ) ) ? 
      // just take the top geometry
      previousGeometry() :
      ( 0 != ( giLocal = findLocalGI( point1 , 
                                      point2 , 
                                      topGeometry,
                                      topGeometry ) ) ) ? 
      
      topGeometry : 0 ;
    
    ///
    if( 0 == giLocal )
      throw TransportSvcException( "TransportSvc::(1) unable to locate points",
                                   StatusCode::FAILURE );
    if( 0 == gi )
      throw TransportSvcException( "TransportSvc::(2) unable to locate points",
                                   StatusCode::FAILURE );

    /// redefine previous geometry
    setPreviousGeometry( giLocal ); 
    
    /// delegate the calculation to the logical volume 
    const ILVolume* lv = giLocal->lvolume();   
    lv->intersectLine
      ( giLocal->toLocalMatrix() * point , 
        giLocal->toLocalMatrix() * vect  , 
        intersept                 , 
        tickMin                   , 
        tickMax                   , 
        threshold                 ); 
    
    /// make local copy of all parameters:    
    m_prevPoint1           = point1              ;
    m_prevPoint2           = point2              ; 
    m_previousThreshold    = threshold           ;
    m_previousGuess        = guessGeometry       ;
    m_previousTopGeometry  = topGeometry ;
    
    /// intersections 
    m_localIntersections.clear();
    m_localIntersections.reserve( intersept.size()); 
    m_localIntersections.insert(m_localIntersections.begin(),
                                intersept.begin() , 
                                intersept.end());
  }
  catch ( const GaudiException& Exception ) 
    {
      /// 1) reset cache: 
      m_prevPoint1           = Gaudi::XYZPoint() ;
      m_prevPoint2           = Gaudi::XYZPoint() ; 
      m_previousThreshold    = -1000.0      ; 
      m_previousGuess        = 0            ; 
      m_previousTopGeometry  = 0            ;
      m_localIntersections.clear()          ; 
      
      ///  2) throw new exception:
      std::string message
        ("TransportSvc::intersection(...), exception caught; Params: ");
      {
        std::ostringstream ost;
        ost << "Point=" << point 
            << ",Vect=" << vect
            << ",Tick=" << tickMin << "/" << tickMax  
            << "Thrsh=" << threshold; 
        if( 0 != alternativeGeometry ) { ost << "A.Geo!=NULL" ; }  
        if( 0 != guessGeometry ) { ost << "G.Geo!=NULL" ; }  
        message += ost.str();             
        Assert( false , message , Exception );
      }
    }
  catch( ... ) 
    {
      /// 1) reset cache: 
      m_prevPoint1           = Gaudi::XYZPoint() ;
      m_prevPoint2           = Gaudi::XYZPoint() ; 
      m_previousThreshold    = -1000.0      ; 
      m_previousGuess        = 0            ; 
      m_previousTopGeometry  = 0            ;
      m_localIntersections.clear()          ; 
      
      /// 2) throw new exception:
      std::string message
        ("TransportSvc::intersection(...), unknown exception caught; Params: ");
      {
        std::ostringstream ost;
        ost << "Point=" << point 
            << ",Vect=" << vect
            << ",Tick=" << tickMin << "/" << tickMax  
            << "Thrsh=" << threshold; 
        if( 0 != alternativeGeometry ) { ost << "A.Geo!=NULL" ; }  
        if( 0 != guessGeometry ) { ost << "G.Geo!=NULL" ; }  
        message += ost.str();             
        Assert( false , message );
      }
    }

  return intersept.size() ;
  ///
}

// ============================================================================
// The End 
// ============================================================================
#endif  //  __DETDESC_TRANSPORTSVC_TRANSPORTSVCINTERSECTIONS_H__ 
// ============================================================================
