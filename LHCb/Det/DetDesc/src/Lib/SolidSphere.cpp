#include "GaudiKernel/IInspector.h" 

#include "DetDesc/SolidSphere.h" 
#include "DetDesc/SolidBox.h" 

#include "DetDesc/SolidTicks.h" 

#include "DetDesc/SolidException.h" 

#include "CLHEP/Units/PhysicalConstants.h" 

//
//
//
SolidSphere::SolidSphere( const std::string & name             ,
			  const double        OuterRadius      ,
			  const double        InsideRadius     , 
			  const double        StartPhiAngle    , 
			  const double        DeltaPhiAngle    ,
			  const double        StartThetaAngle  , 
			  const double        DeltaThetaAngle  ,
                          const int           CoverModel       )
  : m_sphere_name            ( name            ) 
  , m_sphere_outerRadius     ( OuterRadius     )  
  , m_sphere_insideRadius    ( InsideRadius    ) 
  , m_sphere_startPhiAngle   ( StartPhiAngle   ) 
  , m_sphere_deltaPhiAngle   ( DeltaPhiAngle   ) 
  , m_sphere_startThetaAngle ( StartThetaAngle ) 
  , m_sphere_deltaThetaAngle ( DeltaThetaAngle )
  //
  , m_sphere_cover           (       0         )
  , m_sphere_coverModel      (   CoverModel    )  
  //
{
  if( 0 >= OuterRadius ) 
    { throw SolidException("SolidSphere constructor::OuterRadius  is not positive!"); } 
  if( 0 >  InsideRadius ) 
    { throw SolidException("SolidSphere constructor::InsideRadius is negative!    "); }
  if( InsideRadius >= OuterRadius ) 
    { throw SolidException("SolidSphere constructor::InsideRadius>=OuterRadius    "); }
  if( -180.0 * degree > StartPhiAngle )
    { throw SolidException("SolidSphere constructor::StartPhiAngle < -180 degree !"); }
  if(  360.0 * degree < StartPhiAngle )
    { throw SolidException("SolidSphere constructor::StartPhiAngle >  360 degree !"); }
  if(    0.0 * degree > DeltaPhiAngle )
    { throw SolidException("SolidSphere constructor::DeltaPhiAngle <    0 degree !"); }
  if(  360.0 * degree < StartPhiAngle+DeltaPhiAngle )
    { throw SolidException("SolidSphere constructor::StartPhiAngle+DeltaPhiAngle > 360 degree !"); }
  if(    0.0 * degree > StartThetaAngle )
    { throw SolidException("SolidSphere constructor::StartThetaAngle < 0 degree !"); }
  if(  180.0 * degree < StartThetaAngle )
    { throw SolidException("SolidSphere constructor::StartThetaAngle >  180 degree !"); }
  if(    0.0 * degree > DeltaThetaAngle )
    { throw SolidException("SolidSphere constructor::DeltaThetaAngle <    0 degree !"); }
  if(  180.0 * degree < StartThetaAngle+DeltaThetaAngle )
    { throw SolidException("SolidSphere constructor::StartThetaAngle+DeltaThetaAngle > 180 degree !"); }
};  

//
//
//

SolidSphere::~SolidSphere()
{
  if( this == m_sphere_cover ) {                       m_sphere_cover = 0 ; } 
  if( 0    != m_sphere_cover ) { delete m_sphere_cover ; m_sphere_cover = 0 ; }   
};


///
/// fictive default constructor 
///
SolidSphere::SolidSphere()
  : m_sphere_name            ( "unnamed Sphere" ) 
  , m_sphere_outerRadius     ( 1000000.0        )  
  , m_sphere_insideRadius    ( 0.0              ) 
  , m_sphere_startPhiAngle   ( 0.0              ) 
  , m_sphere_deltaPhiAngle   ( 360.0 * degree   ) 
  , m_sphere_startThetaAngle ( 0.0              ) 
  , m_sphere_deltaThetaAngle ( 180.0 * degree   )
  //
  , m_sphere_cover           (       0          )
  , m_sphere_coverModel      (       0          )  
  //
{};


///
/// serialization for reading 
///

StreamBuffer& SolidSphere::serialize( StreamBuffer& s ) 
{
  ///
  if( 0 != m_sphere_cover ) { delete m_sphere_cover ; m_sphere_cover = 0 ; }  
  ///
  s >>  m_sphere_name           
    >>  m_sphere_outerRadius    
    >>  m_sphere_insideRadius    
    >>  m_sphere_startPhiAngle  
    >>  m_sphere_deltaPhiAngle   
    >>  m_sphere_startThetaAngle 
    >>  m_sphere_deltaThetaAngle 
    >>  m_sphere_coverModel  ;    
  ///
  if( 0 >= outerRadius() ) 
    { throw SolidException("SolidSphere constructor::OuterRadius  is not positive!"); } 
  if( 0 >  insideRadius() ) 
    { throw SolidException("SolidSphere constructor::InsideRadius is negative!    "); }
  if( insideRadius() >= outerRadius() ) 
    { throw SolidException("SolidSphere constructor::InsideRadius>=OuterRadius    "); }
  if( -180.0 * degree > startPhiAngle() )
    { throw SolidException("SolidSphere constructor::StartPhiAngle < -180 degree !"); }
  if(  360.0 * degree < startPhiAngle() )
    { throw SolidException("SolidSphere constructor::StartPhiAngle >  360 degree !"); }
  if(    0.0 * degree > deltaPhiAngle() )
    { throw SolidException("SolidSphere constructor::DeltaPhiAngle <    0 degree !"); }
  if(  360.0 * degree < startPhiAngle()+deltaPhiAngle() )
    { throw SolidException("SolidSphere constructor::StartPhiAngle+DeltaPhiAngle > 360 degree !"); }
  if(    0.0 * degree > startThetaAngle() )
    { throw SolidException("SolidSphere constructor::StartThetaAngle < 0 degree !"); }
  if(  180.0 * degree < startThetaAngle() )
    { throw SolidException("SolidSphere constructor::StartThetaAngle >  180 degree !"); }
  if(    0.0 * degree > deltaThetaAngle() )
    { throw SolidException("SolidSphere constructor::DeltaThetaAngle <    0 degree !"); }
  if(  180.0 * degree < startThetaAngle()+deltaThetaAngle() )
    { throw SolidException("SolidSphere constructor::StartThetaAngle+DeltaThetaAngle > 180 degree !"); }  

  ///
  return s;
  ///
};

///
/// serialization for writing 
///

StreamBuffer& SolidSphere::serialize( StreamBuffer& s ) const
{
  ///
  return s <<  typeName() 
	   <<  m_sphere_name           
	   <<  m_sphere_outerRadius    
	   <<  m_sphere_insideRadius    
	   <<  m_sphere_startPhiAngle  
	   <<  m_sphere_deltaPhiAngle   
	   <<  m_sphere_startThetaAngle 
	   <<  m_sphere_deltaThetaAngle 
	   <<  m_sphere_coverModel    ;   
  ///
};


// construction of covering solid:
//  Model == 0 : 
//    cover for sphere segment is segment with no gap in Theta 
//    cover for sphere segment with no gap in Theta is spehere segment with no gap in Phi 
//    cover for sphere segment with no gap in Phi   is sphere with null inside radius 
//    cover for spehere with null inside radius is box 
//  Model != 0 
//    cover for sphere with not null inside radius is sphere with null inside radius 
//    cover for sphere with null inside radius is sphere segment with no gap in phi 
//    cover for sphere segment with no gap in phi is full sphere
//    cover for full sphere is box 
const ISolid*           SolidSphere::cover         () const 
{
  if( 0 != m_sphere_cover ) { return m_sphere_cover; }              // cover is calculated already 
  //  
  ISolid* cov = 0 ;
  if( 0 == m_sphere_coverModel ) 
    { 
      //    cover for sphere segment is segment with no gap in Theta 
      if      ( 0.0*degree  != startThetaAngle() || 180.0*degree  != deltaThetaAngle()  )           
	{ cov = new SolidSphere("Cover for " + name() , 
					      insideRadius ()       , outerRadius  () , 
					      startPhiAngle()       , deltaPhiAngle() ); }
      //    cover for sphere segment with no gap in Theta is spehere segment with no gap in Phi 
      else if ( 0.0*degree  != startPhiAngle  () || 360.0*degree  != deltaPhiAngle  ()  )           
	{ cov = new SolidSphere("Cover for " + name() , outerRadius() , insideRadius() ); } 
      //    cover for sphere segment with no gap in Phi   is sphere with null inside radius 
      else if ( 0.0 != insideRadius() )                                             
	{ cov = new SolidSphere("Cover for " + name() , outerRadius() ); } 
      //    cover for spehere with null inside radius is box 
      else                                                                        
	{ cov = new SolidBox   ("Cover for " + name() , outerRadius()  , 
					      outerRadius()         , outerRadius() ); }
    }
  else
    {
      //    cover for sphere with not null inside radius is sphere with null inside radius 
      if      ( 0.0 != insideRadius() )                                             
	{ cov = new SolidSphere("Cover for " + name() , outerRadius    () ,  0.0 * mm       , 
					      startPhiAngle      () , deltaPhiAngle  () , 
					      startThetaAngle    () , deltaThetaAngle() , 
					      m_sphere_coverModel                                         ); } 
      //    cover for sphere with null inside radius is sphere segment with no gap in phi 
      else if ( 0.0*degree  != startPhiAngle  () || 360.0*degree  != deltaPhiAngle  ()  )           
	{ cov = new SolidSphere("Cover for " + name() , outerRadius()      , insideRadius() , 
					      0.0 * degree          , 360.0 * degree     , 
					      startThetaAngle()     , deltaThetaAngle () , 
                                              m_sphere_coverModel                                         ); } 
      //    cover for sphere segment with no gap in phi is full sphere
      else if ( 0.0*degree  != startThetaAngle() || 180.0*degree  != deltaThetaAngle()  )           
	{ cov = new SolidSphere("Cover for " + name()  ,  outerRadius  () , insideRadius()  , 
					      startPhiAngle()        , deltaPhiAngle () ,
                                              0.0 * degree           , 180.0 * degree   , 
                                              m_sphere_coverModel                                         ); }
      //    cover for full sphere is box 
      else                                                                        
	{ cov = new SolidBox   ("Cover for " + name() , outerRadius()  , 
					      outerRadius()         , outerRadius() ); }
    }
  
  //
  if( 0 == cov ) { return this; } 
  //
  m_sphere_cover = cov; 
  //
  return m_sphere_cover;
  //
};

//
//
//



/// calculate the intersection points("ticks") with a given line. 
/// Input - line, paramterised by (Point + Vector * Tick) 
/// "Tick" is just a value of parameter, at which the intercestion occurs 
/// Return the number of intersection points (=size of Ticks container)   

inline  unsigned int SolidSphere::intersectionTicks ( const HepPoint3D& point  ,          // initial point for teh line 
						      const HepVector3D& vect   ,          // vector along the line 
						      ISolid::Ticks   & ticks  ) const    // output container of "Ticks"
{
  ticks.clear();
  
  /// line with null direction vector in not able to intersect something
  if( vect.mag2() <= 0 ) { return 0; }   // RETURN!!!

  
  ///
  ///
  ///  try to intersect with sphere outer radius
  if( 0 == SolidTicks::LineIntersectsTheSphere( point , vect , outerRadius() , std::back_inserter( ticks ) ) ) { return 0; } 
  
  // check for intersection with inner radius 
  if( insideRadius() > 0 ) 
    { SolidTicks::LineIntersectsTheSphere( point , vect , outerRadius() , std::back_inserter( ticks ) ) ; }
  
  
  //
  //
  // check for phi angle 
  if( 0 * degree != startPhiAngle() || 360 * degree != deltaPhiAngle() ) 
    {
      SolidTicks::LineIntersectsThePhi( point , vect , startPhiAngle() , std::back_inserter( ticks ) ) ;  
      SolidTicks::LineIntersectsThePhi( point , vect , startPhiAngle() + deltaPhiAngle() , std::back_inserter( ticks ) ) ;  
    }
  
  ///
  ///
  /// check for theta angle 
  if( 0 * degree != startThetaAngle() || 180 * degree != deltaThetaAngle() )
    {
      SolidTicks::LineIntersectsTheTheta( point , vect , startPhiAngle() , std::back_inserter( ticks ) ) ;  
      SolidTicks::LineIntersectsTheTheta( point , vect , startPhiAngle() + deltaPhiAngle() , std::back_inserter( ticks ) ) ;  
    }
  
  ///
  ///
  /// sort and remove adjancent and some EXTRA ticks and return
  return SolidTicks::RemoveAdjancentTicks( ticks , point , vect , *this );  
  
};

///
///
///

/// calculate the intersection points("ticks") with a given line. 
/// Input - line, paramterised by (Point + Vector * Tick) 
/// "Tick" is just a value of parameter, at which the intercestion occurs 
/// Return the number of intersection points (=size of Ticks container)   

inline  unsigned int SolidSphere::intersectionTicks ( const HepPoint3D&   point  ,          // initial point for teh line 
						      const HepVector3D&   vect   ,          // vector along the line 
                                                      const ISolid::Tick& tickMin ,         // minimal value for Tick 
                                                      const ISolid::Tick& tickMax ,         // maximal value for Tick 
						      ISolid::Ticks   &   ticks  ) const    // output container of "Ticks"
{
  ///
  intersectionTicks( point , vect , ticks ) ; 
  /// sort and remove adjancent and some EXTRA ticks and return
  return SolidTicks::RemoveAdjancentTicks( ticks , point , vect , tickMin, tickMax , *this );  
  ///
};
///
///
///


bool SolidSphere::acceptInspector( IInspector* pInspector ) 
{
  ///
  const ISolid* s = this ; 
  return s->acceptInspector( pInspector ); 
};

///
///
///

bool SolidSphere::acceptInspector( IInspector* pInspector )  const 
{
  ///
  if( 0 == pInspector ) { return false; } 
  ///
  pInspector->inspectByRef ( &m_sphere_name            , this , "SphereName"  ) ; 
  pInspector->inspectByRef ( &m_sphere_outerRadius     , this , "OuterRadius" ) ; 
  pInspector->inspectByRef ( &m_sphere_insideRadius    , this , "InnerRadius" ) ; 
  pInspector->inspectByRef ( &m_sphere_startPhiAngle   , this , "StartPhi"    ) ; 
  pInspector->inspectByRef ( &m_sphere_deltaPhiAngle   , this , "DeltaPhi"    ) ; 
  pInspector->inspectByRef ( &m_sphere_startThetaAngle , this , "StartTheta"  ) ; 
  pInspector->inspectByRef ( &m_sphere_deltaThetaAngle , this , "DeltaTheta"  ) ; 
  pInspector->inspectByRef ( &m_sphere_coverModel      , this , "CoverModel"  ) ;  
  ///
  return true;
  ///
};
