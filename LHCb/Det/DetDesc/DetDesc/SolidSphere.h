#ifndef     __DETDESC_SOLID_SOLIDSPHERE_H__
#define     __DETDESC_SOLID_SOLIDSPHERE_H__ 1 

#include <cmath>
#include <algorithm>

#include "GaudiKernel/ISolid.h" 


#include "CLHEP/Units/PhysicalConstants.h"

#include "CLHEP/Geometry/Point3D.h" 
#include "CLHEP/Geometry/Vector3D.h" 

class ISolidFromStream;
class StreamBuffer;

///
///  class SolidSphere : just a simple implementation of ISolid interphase for primitive sphere  
///
///  Author: Vanya Belyaev 
///

class SolidSphere: public ISolid
{
  ///
  friend class ISolidFromStream;
  ///
 public:
  //
  // constructor, all size - in mm, all angles in radians 
  SolidSphere( const std::string& name                             ,
               const double       OuterRadius                      ,
	       const double       InsideRadius    =   0.0          , 
	       const double       StartPhiAngle   =   0.0 * degree , 
               const double       DeltaPhiAngle   = 360.0 * degree ,
	       const double       StartThetaAngle =   0.0 * degree , 
               const double       DeltaThetaAngle = 180.0 * degree , 
               const int          CoverModel      =   0            );
  //
  // destructor 
  virtual ~SolidSphere();
  //
  // functions from ISolid:
  // name of this solid 
  inline  const  std::string&      name          ()                      const { return m_sphere_name; };
  // type of this solid 
  inline         std::string       typeName      ()                      const { return "SolidSphere"; };
  // the notorious "isInside" method
  inline         bool              isInside      ( const HepPoint3D&   ) const;
  // covering solid   
  inline const   ISolid*           cover         ()                      const;
  // the top covering solid (normally SolidBox)   
  inline const   ISolid*           coverTop      ()                      const;
  // overloaded printOut 
  virtual inline std::ostream&     printOut      ( std::ostream&       ) const;  
  /// reset to the initial state 
  inline const   ISolid*           reset         ()                      const; 
  
  // calculate the intersection points("ticks") with a given line. 
  // Input - line, paramterised by (Point + Vector * Tick) 
  // "Tick" is just a value of parameter, at which the intercestion occurs 
  // Return the number of intersection points (=size of Ticks container)   
  virtual inline  unsigned int intersectionTicks ( const HepPoint3D & Point  ,          // initial point for teh line 
						   const HepVector3D& Vector ,          // vector along the line 
						   ISolid::Ticks    & ticks  ) const ;  // output container of "Ticks"
  // calculate the intersection points("ticks") with a given line. 
  // Input - line, paramterised by (Point + Vector * Tick) 
  // "Tick" is just a value of parameter, at which the intercestion occurs 
  // Return the number of intersection points (=size of Ticks container)   
  virtual inline  unsigned int intersectionTicks ( const HepPoint3D&   Point   ,          /// initial point for teh line 
						   const HepVector3D&  Vector  ,          /// vector along the line 
						   const ISolid::Tick& tickMin ,          /// minimal value for tick  
						   const ISolid::Tick& tickMax ,          /// maximal value for tick 
						   ISolid::Ticks   &   ticks   ) const ;  /// output container of "Ticks"
  //
  // function specific for SolidSphere

  // return the inner radius of sphere (in mm)
  inline       double               insideRadius   () const { return m_sphere_insideRadius   ; }; 
  // return the outer radius of sphere (in mm)
  inline       double               outerRadius    () const { return m_sphere_outerRadius    ; }; 
  // return the start of phi angle of sphere segment (in radians)
  inline       double               startPhiAngle  () const { return m_sphere_startPhiAngle  ; }; 
  // return the delta of phi angle of sphere segment (in radians)
  inline       double               deltaPhiAngle  () const { return m_sphere_deltaPhiAngle  ; }; 
  // return the start of theta angle of sphere segment (in radians)
  inline       double               startThetaAngle() const { return m_sphere_startThetaAngle; }; 
  // return the delta of theta angle of sphere segment (in radians)
  inline       double               deltaThetaAngle() const { return m_sphere_deltaThetaAngle; }; 
  //
  // return the inner diameter of sphere (in mm)
  inline       double  insideDiameter () const { return m_sphere_insideRadius    * 2 ; }; 
  // return the outer diameter of sphere (in mm)
  inline       double  outerDiameter  () const { return m_sphere_outerRadius     * 2 ; }; 
  // return the end of phi angle of sphere segment (in radians)
  inline       double  endPhiAngle    () const { return m_sphere_deltaPhiAngle   + m_sphere_deltaPhiAngle   ; }; 
  // return the end of theta angle of sphere segment (in radians)
  inline       double  endThetaAngle  () const { return m_sphere_deltaThetaAngle + m_sphere_deltaThetaAngle ; };



  ///
  /// serialization for reading 
  StreamBuffer& serialize( StreamBuffer& s ) ; 
  /// serialization for writing 
  StreamBuffer& serialize( StreamBuffer& s ) const ; 
  ///

 protected:
  ///
  SolidSphere();
  ///
 private:
  //
  SolidSphere           ( const SolidSphere& );  // no copy-constructor 
  SolidSphere& operator=( const SolidSphere& );  // no assignment 
  //
  // members
  //
  std::string           m_sphere_name            ;
  double                m_sphere_outerRadius     ;  
  double                m_sphere_insideRadius    ; 
  double                m_sphere_startPhiAngle   ;
  double                m_sphere_deltaPhiAngle   ; 
  double                m_sphere_startThetaAngle ; 
  double                m_sphere_deltaThetaAngle ;
  //
  mutable  ISolid*      m_sphere_cover           ;
  int                   m_sphere_coverModel      ;
  //
};

//
//
//

inline   bool  SolidSphere::isInside( const HepPoint3D & point) const
{
  //
  double r2 = point.mag2();
  //
  if ( r2 > outerRadius () * outerRadius ()       ) { return false; }
  if ( r2 < insideRadius() * insideRadius()       ) { return false; }
  //
  double phi = point.phi();
  if( phi < 0 ){ phi+=360.0*degree; }
  //
  if( phi < startPhiAngle()                       ) { return false; } 
  if( phi > startPhiAngle() + deltaPhiAngle()     ) { return false; }
  //
  double theta = point.theta();
  //
  if( theta < startThetaAngle()                   ) { return false; } 
  if( theta > startThetaAngle() + deltaPhiAngle() ) { return false; } 
  //
  return true; 
  //
}

//
//
//

inline const ISolid* SolidSphere::coverTop      () const 
{
  const ISolid* cov = this; 
  while( cov != cov->cover() ){ cov = cov->cover(); } 
  return cov; 
};

///
///
///

inline std::ostream&  SolidSphere::printOut      ( std::ostream&  os ) const
{
  os << typeName()            << "name="                        << name()
     << " (outerRadius="      << outerRadius    () / millimeter << "[mm]" ; 
  if( insideRadius   () > 0 ) 
    { 
      os <<   ",insideRadius="     << insideRadius   () / millimeter << "[mm]";
    }
  if( startPhiAngle  () != 0 * degree || deltaPhiAngle  () != 360 * degree ) 
    {
      os <<   ",startPhiAngle="    << startPhiAngle  () / degree     << "[degree]";
      os <<   ",deltaPhiAngle="    << deltaPhiAngle  () / degree     << "[degree]";
    }
  if( startThetaAngle() != 0 * degree || deltaThetaAngle() != 180 * degree ) 
    { 
      os <<   ",startThetaAngle="  << startThetaAngle() / degree     << "[degree]";
      os <<   ",deltaThetaAngle="  << deltaThetaAngle() / degree     << "[degree]";
    }
  ///
  return os << ")" ;
};

//
//
//

///
///
/// reset to the initial state 
inline const   ISolid* SolidSphere::reset() const
{
  if( 0 != m_sphere_cover ) { delete m_sphere_cover; m_sphere_cover = 0 ;}
  return this;
};

///
///
///

#endif //   __DETDESC_SOLID_SOLIDSPHERE_H__

