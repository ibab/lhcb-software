#include "GaudiKernel/IInspector.h"

#include "DetDesc/SolidCons.h"
#include "DetDesc/SolidTubs.h"
#include "DetDesc/SolidTrd.h"


#include "DetDesc/SolidTicks.h" 
#include "DetDesc/SolidException.h"

#include "GaudiKernel/StreamBuffer.h"

#include "CLHEP/Units/PhysicalConstants.h"

//
//
//


SolidCons::SolidCons( const std::string & name  ,
		      double ZHalfLength        , 
		      double OuterRadiusMinusZ  ,
		      double OuterRadiusPlusZ   ,
		      double InnerRadiusMinusZ  , 
		      double InnerRadiusPlusZ   , 
		      double StartPhiAngle      , 
		      double DeltaPhiAngle      ,
                      int    CoverModel         )
  : m_cons_name              ( name               )
  , m_cons_zHalfLength       ( ZHalfLength        )
  , m_cons_outerRadiusMinusZ ( OuterRadiusMinusZ  )
  , m_cons_outerRadiusPlusZ  ( OuterRadiusPlusZ   )
  , m_cons_innerRadiusMinusZ ( InnerRadiusMinusZ  )
  , m_cons_innerRadiusPlusZ  ( InnerRadiusPlusZ   )
  , m_cons_startPhiAngle     ( StartPhiAngle      )
  , m_cons_deltaPhiAngle     ( DeltaPhiAngle      )
  //
  , m_cons_coverModel        ( CoverModel         )        
  , m_cons_cover             (         0          )
  //
{
  ///
  if( 0 >= zHalfLength()                                ) { throw SolidException("SolidCons ::ZHalfLength       is not positive!"       );}
  if( 0 >= outerRadiusAtMinusZ()                        ) { throw SolidException("SolidCons ::OuterRadiusMinusZ is not positive!"       );}
  if( 0 >= outerRadiusAtPlusZ()                         ) { throw SolidException("SolidCons ::OuterRadiusPlusZ  is not positive!"       );}
  if( 0 >  innerRadiusAtMinusZ()                        ) { throw SolidException("SolidCons ::InnerRadiusMinusZ is negative !   "       );}
  if( innerRadiusAtMinusZ() >= outerRadiusAtMinusZ()    ) { throw SolidException("SolidCons ::InnerRadiusMinusZ>=OuterRadiusMinusZ!"    );}
  if( 0 >  innerRadiusAtPlusZ()                         ) { throw SolidException("SolidCons ::InnerRadiusPlusZ  is negative !   "       );}
  if( innerRadiusAtPlusZ() >= outerRadiusAtPlusZ()      ) { throw SolidException("SolidCons ::InnerRadiusPlusZ>=OuterRadiusPlusZ!"      );}
  if( -180.0 * degree > startPhiAngle()                 ) { throw SolidException("SolidCons ::StartPhiAngle  is less than -180  degree!");}
  if(  360.0 * degree < startPhiAngle()                 ) { throw SolidException("SolidCons ::StartPhiAngle  is larger than 360 degree!");}
  if(    0.0 * degree > deltaPhiAngle()                 ) { throw SolidException("SolidCons ::DeltaPhiAngle  is less than 0 degree!"    );}
  if(  360.0 * degree < startPhiAngle()+deltaPhiAngle() ) { throw SolidException("SolidCons ::StartPhiAngle+DeltaPhiAngle > 360 degree!");}
  ///
};

//
//
//

SolidCons::~SolidCons()
{
  if( this == m_cons_cover ) {                       m_cons_cover = 0 ;} 
  if( 0    != m_cons_cover ) { delete m_cons_cover ; m_cons_cover = 0 ;}
};


///
/// "fictive" default constructor 
///
SolidCons::SolidCons()
  ///
  : m_cons_name              ( "unnamed SolidCons" )
  , m_cons_zHalfLength       ( 1000000             )
  , m_cons_outerRadiusMinusZ ( 1000000             )
  , m_cons_outerRadiusPlusZ  ( 1000000             )
  , m_cons_innerRadiusMinusZ ( 0                   )
  , m_cons_innerRadiusPlusZ  ( 0                   )
  , m_cons_startPhiAngle     ( 0                   )
  , m_cons_deltaPhiAngle     ( 360 * degree        )
  //
  , m_cons_coverModel        ( 0                   )        
  , m_cons_cover             ( 0                   )
  //
{};

///
/// serialization 
///
  /// 
  /// serialization for reading 
  /// serialization for writing 
  ///   


StreamBuffer& SolidCons::serialize( StreamBuffer& s ) 
{
  ///
  if( 0 != m_cons_cover ) { delete m_cons_cover ; m_cons_cover = 0 ; } 
  ///
  s >>  m_cons_name              
    >>  m_cons_zHalfLength       
    >>  m_cons_outerRadiusMinusZ 
    >>  m_cons_outerRadiusPlusZ  
    >>  m_cons_innerRadiusMinusZ 
    >>  m_cons_innerRadiusPlusZ  
    >>  m_cons_startPhiAngle    
    >>  m_cons_deltaPhiAngle    
    >>  m_cons_coverModel ;   
  ///
  if( 0 >= zHalfLength()                                ) { throw SolidException("SolidCons ::ZHalfLength       is not positive!"       );}
  if( 0 >= outerRadiusAtMinusZ()                        ) { throw SolidException("SolidCons ::OuterRadiusMinusZ is not positive!"       );}
  if( 0 >= outerRadiusAtPlusZ()                         ) { throw SolidException("SolidCons ::OuterRadiusPlusZ  is not positive!"       );}
  if( 0 >  innerRadiusAtMinusZ()                        ) { throw SolidException("SolidCons ::InnerRadiusMinusZ is negative !   "       );}
  if( innerRadiusAtMinusZ() >= outerRadiusAtMinusZ()    ) { throw SolidException("SolidCons ::InnerRadiusMinusZ>=OuterRadiusMinusZ!"    );}
  if( 0 >  innerRadiusAtPlusZ()                         ) { throw SolidException("SolidCons ::InnerRadiusPlusZ  is negative !   "       );}
  if( innerRadiusAtPlusZ() >= outerRadiusAtPlusZ()      ) { throw SolidException("SolidCons ::InnerRadiusPlusZ>=OuterRadiusPlusZ!"      );}
  if( -180.0 * degree > startPhiAngle()                 ) { throw SolidException("SolidCons ::StartPhiAngle  is less than -180  degree!");}
  if(  360.0 * degree < startPhiAngle()                 ) { throw SolidException("SolidCons ::StartPhiAngle  is larger than 360 degree!");}
  if(    0.0 * degree > deltaPhiAngle()                 ) { throw SolidException("SolidCons ::DeltaPhiAngle  is less than 0 degree!"    );}
  if(  360.0 * degree < startPhiAngle()+deltaPhiAngle() ) { throw SolidException("SolidCons ::StartPhiAngle+DeltaPhiAngle > 360 degree!");}
  ///
  return s; 
};
///
/// serialization 
///

StreamBuffer& SolidCons::serialize( StreamBuffer& s ) const 
{
  ///
  return s <<  typeName() 
	   <<  m_cons_name              
	   <<  m_cons_zHalfLength       
	   <<  m_cons_outerRadiusMinusZ 
	   <<  m_cons_outerRadiusPlusZ  
	   <<  m_cons_innerRadiusMinusZ 
	   <<  m_cons_innerRadiusPlusZ  
	   <<  m_cons_startPhiAngle    
	   <<  m_cons_deltaPhiAngle    
	   <<  m_cons_coverModel ;   
  ///
};

// construction of covering solid:
//  Model == 0 :
//    cover for conical tube segment is conical tube 
//    cover for conical tube is conical cylinder  
//    cover for conical cylinder is TRD  
// Model != 0 : 
//    cover for conical tube     segment is conical cylinder segment 
//    cover for conical cylinder segment is conical cylinder 
//    cover for conical cylinder is TRD 
const ISolid*           SolidCons::cover         () const 
{
  if( 0 != m_cons_cover ) { return m_cons_cover; }              // cover is calculated already 
  //  
  ISolid* cov = 0 ;
  if( 0 == m_cons_coverModel ) 
    {
      // cover for conical tube segment is conical tube 
      if      ( 0.0*degree  != startPhiAngle  () || 360.0*degree    != deltaPhiAngle         () )  
	{ cov = new SolidCons  ("Cover for " + name     () , zHalfLength       () , 
					      outerRadiusAtMinusZ     () , outerRadiusAtPlusZ() , 
					      innerRadiusAtMinusZ     () , innerRadiusAtPlusZ() ); }
      // cover for conical tube is "conical cylinder"  
      else if ( 0.0 != innerRadiusAtMinusZ() || 0.0 != innerRadiusAtPlusZ() )
	{ cov = new SolidCons  ("Cover for " + name     () , zHalfLength     () , 
					      outerRadiusAtMinusZ     () , outerRadiusAtPlusZ() );}
      // cover for "conical cylinder" is TRD  
      else                                                                        
	{ cov = new SolidTrd  ("Cover for " + name     () , zHalfLength      () ,
					     outerRadiusAtMinusZ     () , outerRadiusAtMinusZ() , 
					     outerRadiusAtPlusZ      () , outerRadiusAtPlusZ () ); } 
    }
  else
    {
      //    cover for conical tube     segment is conical cylinder segment 
      if ( 0.0 != innerRadiusAtMinusZ() || 0.0 != innerRadiusAtPlusZ() )
	{ cov = new SolidCons  ("Cover for " + name     () , zHalfLength       () , 
					      outerRadiusAtMinusZ     () , outerRadiusAtPlusZ() , 
                                              0.0 * mm                   ,  0.0 * mm            , 
                                              startPhiAngle           () , deltaPhiAngle     () , 
                                              m_cons_coverModel                                 );}
      //    cover for conical cylinder segment is conical cylinder 
      else if ( 0.0*degree  != startPhiAngle  () || 360.0*degree    != deltaPhiAngle         () )  
	{ cov = new SolidCons  ("Cover for " + name     () , zHalfLength       () , 
					      outerRadiusAtMinusZ     () , outerRadiusAtPlusZ() , 
					      innerRadiusAtMinusZ     () , innerRadiusAtPlusZ() ,
                                              0.0 * degree               , 360.0 * degree       , 
                                              m_cons_coverModel                                 ); }
      // cover for "conical cylinder" is TRD  
      else                                                                        
	{ cov = new SolidTrd  ("Cover for " + name     () , zHalfLength      () ,
					     outerRadiusAtMinusZ     () , outerRadiusAtMinusZ() , 
					     outerRadiusAtPlusZ      () , outerRadiusAtPlusZ () ); } 
    }
  //
  if( 0 == cov ) { return this; } 
  //
  m_cons_cover = cov ; 
  //
  return m_cons_cover ;
};


///
///
///

///
/// calculate the intersection points("ticks") with a given line. 
/// Input - line, paramterised by (Point + Vector * Tick) 
/// "Tick" is just a value of parameter, at which the intercestion occurs 
/// Return the number of intersection points (=size of Ticks container)   
inline  unsigned int SolidCons::intersectionTicks ( const HepPoint3D & point  ,          // initial point for teh line 
						    const HepVector3D& vect   ,          // vector along the line 
						    ISolid::Ticks    & ticks  ) const    // output container of "Ticks"
{
  
  // line with numm direction vector is not able to intersect any solid 
  if( vect.mag2() <= 0 )  { return 0 ;}                               // RETURN!!!
  
  // intersect with z-planes 
  SolidTicks::LineIntersectsTheZ( point , vect , -1.0 * zHalfLength() , std::back_inserter( ticks ) ); 
  SolidTicks::LineIntersectsTheZ( point , vect ,        zHalfLength() , std::back_inserter( ticks ) ); 
  
  //intersect with phi 
  if( ( 0 != startPhiAngle() ) || ( 360 * degree != deltaPhiAngle() ) )
    {
      SolidTicks::LineIntersectsThePhi( point , vect , startPhiAngle()                   , std::back_inserter( ticks ) ); 
      SolidTicks::LineIntersectsThePhi( point , vect , startPhiAngle() + deltaPhiAngle() , std::back_inserter( ticks ) ); 
    } 
  
  /// intersect with outer conical surface
  SolidTicks::LineIntersectsTheCone( point , vect , 
				     outerRadiusAtMinusZ() , outerRadiusAtPlusZ () , 
				     -1.0 * zHalfLength () , zHalfLength        () , std::back_inserter( ticks ) ); 
  
  /// intersect with inner conical surface
  if( ( 0 < innerRadiusAtPlusZ() ) || ( 0 < innerRadiusAtMinusZ() )  )
    {
      SolidTicks::LineIntersectsTheCone( point , vect , 
					 innerRadiusAtMinusZ() , innerRadiusAtPlusZ () , 
					 -1.0 * zHalfLength () , zHalfLength        () , std::back_inserter( ticks ) );     
    }
  
  /// sort and remove adjancent and some EXTRA ticks and return 
  return SolidTicks::RemoveAdjancentTicks( ticks , point , vect , *this );  
  
};

///
///
///

///
/// calculate the intersection points("ticks") with a given line. 
/// Input - line, paramterised by (Point + Vector * Tick) 
/// "Tick" is just a value of parameter, at which the intercestion occurs 
/// Return the number of intersection points (=size of Ticks container)   
inline  unsigned int SolidCons::intersectionTicks ( const HepPoint3D &   point   ,          // initial point for teh line 
						    const HepVector3D&   vect    ,          // vector along the line 
                                                    const ISolid::Tick& tickMin ,
                                                    const ISolid::Tick& tickMax ,
						    ISolid::Ticks&      ticks   ) const    // output container of "Ticks"
{
  ///
  intersectionTicks( point , vect , ticks ); 
  /// sort and remove adjancent and some EXTRA ticks and return 
  return SolidTicks::RemoveAdjancentTicks( ticks , point , vect , tickMin , tickMax , *this );  
  ///
}

///
/// IInspectable interface 
///

bool SolidCons::acceptInspector( IInspector* pInspector ) 
{
  ///
  const ISolid* s = this;
  return s->acceptInspector( pInspector ); 
};

///
///
///

bool SolidCons::acceptInspector( IInspector* pInspector ) const
{
  ///
  if( 0 == pInspector ) { return false; } 
  ///
  pInspector->inspectByRef( &m_cons_name              , this , "ConsName"     ) ; 
  pInspector->inspectByRef( &m_cons_outerRadiusMinusZ , this , "OuterRadAt-Z" ) ; 
  pInspector->inspectByRef( &m_cons_outerRadiusPlusZ  , this , "OuterRadAt+Z" ) ; 
  pInspector->inspectByRef( &m_cons_innerRadiusMinusZ , this , "InnerRadAt-Z" ) ; 
  pInspector->inspectByRef( &m_cons_innerRadiusPlusZ  , this , "InnerRadAt+Z" ) ; 
  pInspector->inspectByRef( &m_cons_startPhiAngle     , this , "startPhi"     ) ; 
  pInspector->inspectByRef( &m_cons_deltaPhiAngle     , this , "deltaPhi"     ) ; 
  pInspector->inspectByRef( &m_cons_coverModel        , this , "CoverModel"   ) ; 
  ///
  return true;
};


