#ifndef     __DETDESC_SOLID_SOLIDCONS_H__
#define     __DETDESC_SOLID_SOLIDCONS_H__  

#include <cmath> 
#include <iostream>

#include "GaudiKernel/ISolid.h" 

#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Vector3D.h"

#include "CLHEP/Units/PhysicalConstants.h"

class StreamBuffer;


///
///
/// class SolidCons - the native implementation of the segment of the conical tube 
///
/// Author: Vanya Belyaev 
///

class SolidCons: public ISolid
{
  ///
  friend class ISolidFromStream;
  ///
 public:
  //
  // constructorm, all sizes in mm, all angles in radians 
  SolidCons( const std::string& name                                  ,
	     const double       ZHalfLength                           , 
	     const double       OuterRadiusAtMinusZ                   ,
	     const double       OuterRadiusAtPlusZ                    ,
	     const double       InnerRadiusAtMinusZ  =   0.0          , 
	     const double       InnerRadiusAtPlusZ   =   0.0          , 
	     const double       StartPhiAngle        =   0.0 * degree , 
	     const double       DeltaPhiAngle        = 360.0 * degree ,
             const int          CoverModel           =   0            );
  //
  // destructor 
  virtual ~SolidCons();
  //
  // functions from ISolid:
  //
  // return the name of this solid 
  inline const std::string&     name         ()                       const { return m_cons_name  ; };
  // return the type of this solid 
  inline       std::string      typeName     ()                       const { return "SolidCons"; };
  // the notorious "isInside" method 
  inline       bool             isInside     (  const HepPoint3D&   ) const ;
  // covering solid 
  inline const ISolid*          cover        ()                       const ;
  // "the top covering" solid  (normally SolidBox)
  inline const ISolid*          coverTop     ()                       const ;
  // overloaded printout 
  virtual inline std::ostream&  printOut     ( std::ostream&        ) const; 
  /// reset to the initial state 
  inline const ISolid*          reset        ()                       const; 
  ///
  /// calculate the intersection points("ticks") with a given line. 
  /// Input - line, paramterised by (Point + Vector * Tick) 
  /// "Tick" is just a value of parameter, at which the intersection occurs 
  /// Return the number of intersection points (=size of Ticks container)   
  virtual inline  unsigned int intersectionTicks ( const HepPoint3D & Point  ,          /// initial point for teh line 
						   const HepVector3D& Vector ,          /// vector along the line 
						   ISolid::Ticks    & ticks  ) const ;  /// output container of "Ticks"
  /// calculate the intersection points("ticks") with a given line. 
  /// Input - line, paramterised by (Point + Vector * Tick) 
  /// "Tick" is just a value of parameter, at which the intersection occurs 
  /// Return the number of intersection points (=size of Ticks container)   
  virtual inline  unsigned int intersectionTicks ( const HepPoint3D  & Point   ,          /// initial point for teh line 
						   const HepVector3D & Vector  ,          /// vector along the line
						   const ISolid::Tick& tickMin ,          /// minimal value of tick 
						   const ISolid::Tick& tickMax ,          /// maximal value of tick 
						   ISolid::Ticks     & ticks   ) const ;  /// output container of "Ticks"
  
  //
  //
  // function specific for SolidCons
  //
  // inner radius at minus Z  
  inline double  innerRadiusAtMinusZ () const { return m_cons_innerRadiusMinusZ ; };
  // outer radius at minus Z  
  inline double  outerRadiusAtMinusZ () const { return m_cons_outerRadiusMinusZ ; };
  // inner radius at plus Z  
  inline double  innerRadiusAtPlusZ  () const { return m_cons_innerRadiusPlusZ  ; };
  // outer radius at plus Z  
  inline double  outerRadiusAtPlusZ  () const { return m_cons_outerRadiusPlusZ  ; };
  // half length 
  inline double  zHalfLength         () const { return m_cons_zHalfLength       ; };
  // start phi angle   
  inline double  startPhiAngle       () const { return m_cons_startPhiAngle     ; }; 
  // delta phi   
  inline double  deltaPhiAngle       () const { return m_cons_deltaPhiAngle     ; }; 
  //
  // additional functions:
  //
  // inner diameter at minus Z  
  inline double  innerDiameterAtMinusZ () const { return m_cons_innerRadiusMinusZ * 2 ; };
  // outer radius at minus Z  
  inline double  outerDiameterAtMinusZ () const { return m_cons_outerRadiusMinusZ * 2 ; };
  // inner radius at plus Z  
  inline double  innerDiameterAtPlusZ  () const { return m_cons_innerRadiusPlusZ  * 2 ; };
  // outer radius at plus Z  
  inline double  outerDiameterAtPlusZ  () const { return m_cons_outerRadiusPlusZ  * 2 ; };
  // full length 
  inline double  zLength               () const { return m_cons_zHalfLength       * 2 ; };
  // end phi angle   
  inline double  endPhiAngle           () const { return m_cons_startPhiAngle + m_cons_deltaPhiAngle ; }; 
  
 private:
  //
  inline double iR_z( double z ) const
    {
      double a = ( innerRadiusAtPlusZ()-innerRadiusAtMinusZ() ) / zHalfLength();
      double b = ( innerRadiusAtPlusZ()+innerRadiusAtMinusZ() )  ; 
      //
      return 0.5*(a*z+b);
    } 
  //
  inline double oR_z( double z ) const
    {
      double a = ( outerRadiusAtPlusZ()-outerRadiusAtMinusZ() ) / zHalfLength();
      double b = ( outerRadiusAtPlusZ()+outerRadiusAtMinusZ() )  ; 
      //
      return 0.5*(a*z+b);
    } 
  /// 
  /// serialization for reading 
  StreamBuffer& serialize( StreamBuffer& s )       ; 
  /// serialization for writing 
  StreamBuffer& serialize( StreamBuffer& s ) const ; 
  ///   
 protected:
  ///
  SolidCons();
  ///
 private:
  //
  SolidCons           ( const SolidCons & );  // no copy-constructor 
  SolidCons& operator=( const SolidCons & );  // no assignment 
  //
  // members
  //
  std::string             m_cons_name              ;
  double                  m_cons_zHalfLength       ;
  double                  m_cons_outerRadiusMinusZ ;
  double                  m_cons_outerRadiusPlusZ  ;
  double                  m_cons_innerRadiusMinusZ ;
  double                  m_cons_innerRadiusPlusZ  ;
  double                  m_cons_startPhiAngle     ;
  double                  m_cons_deltaPhiAngle     ;
  //
  //
  int                     m_cons_coverModel;
  mutable ISolid*         m_cons_cover; 
  //

};

//
//
//


inline   bool  SolidCons::isInside( const HepPoint3D & point ) const
{
  //
  if( abs( point.z() ) > zHalfLength() ) { return false; }
  //
  double rho = point.perp(); 
  //
  if( rho < iR_z( point.z() ) ) { return false; }
  if( rho > oR_z( point.z() ) ) { return false; }
  //
  double phi = point.phi();
  if( phi < 0 ) { phi += 360.0*degree; };
  //
  if( phi < startPhiAngle()                   ) { return false; } 
  if( phi > startPhiAngle() + deltaPhiAngle() ) { return false; }
  //
  return true;
}
//
//
//
//


inline const ISolid* SolidCons::coverTop      ()  const 
{
  const ISolid* cov = this; 
  while( cov != cov->cover() ){ cov = cov->cover(); } 
  return cov; 
};

//
//
//

inline std::ostream&  SolidCons::printOut      ( std::ostream&  os ) const
{
  os << typeName()                     << " name="                             << name()
     << " (zLength="                   << zLength              () / millimeter << "[mm]"
     <<   ",outerRadiusAtMinusZ"       << outerRadiusAtMinusZ  () / millimeter << "[mm]"
     <<   ",outerRadiusAtPlusZ"        << outerRadiusAtPlusZ   () / millimeter << "[mm]";
  if( innerRadiusAtMinusZ() > 0 ) 
    { os <<   ",innerRadiusAtMinusZ"   << innerRadiusAtMinusZ  () / millimeter << "[mm]"; } 
  if( innerRadiusAtPlusZ () > 0 ) 
    { os <<   ",innerRadiusAtPlusZ"    << innerRadiusAtPlusZ   () / millimeter << "[mm]"; }
  if( startPhiAngle() != 0 * degree || deltaPhiAngle() != 360 * degree ) 
    { 
      os <<   ",startPhiAngle="        << startPhiAngle        () / degree     << "[degree]"
	 <<   ",deltaPhiAngle="        << deltaPhiAngle        () / degree     << "[degree]";
    }
  return os << ")";
};

///
///
/// reset to the initial state 
inline const ISolid* SolidCons::reset() const
{
  if( 0 != m_cons_cover ) { delete m_cons_cover; m_cons_cover = 0 ; } 
  return this; 
}; 

//
//
//

#endif //   __DETDESC_SOLID_SOLIDCONS_H__



