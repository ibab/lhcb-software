#ifndef     __DETDESC_SOLID_SOLIDCONS_H__
#define     __DETDESC_SOLID_SOLIDCONS_H__  
/// STD and STL 
#include <cmath> 
#include <iostream>
/// CLHEP 
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Vector3D.h"
#include "CLHEP/Units/PhysicalConstants.h"
/// GaudiKernel
#include "GaudiKernel/ISolid.h" 
///
class StreamBuffer;
///

/** @class SolidCons SolidCons.h DetDesc/SolidCons.h

    A simple implementation of CONS 
    
    @author Vanya Belyaev 
*/

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
  virtual std::ostream&  printOut ( std::ostream& os = std::cerr )    const; 
  // overloaded printout 
  virtual MsgStream&     printOut ( MsgStream&                   )    const; 
  /// reset to the initial state 
  inline const ISolid*          reset        ()                       const; 
  //`/
  /** calculate the intersection points("ticks") with a given line. 
      Input - line, paramterised by (Point + Vector * Tick) 
      "Tick" is just a value of parameter, at which the intersection occurs 
      Return the number of intersection points (=size of Ticks container)   
  */
  virtual inline  unsigned int intersectionTicks ( const HepPoint3D & Point  ,          /// initial point for teh line 
						   const HepVector3D& Vector ,          /// vector along the line 
						   ISolid::Ticks    & ticks  ) const ;  /// output container of "Ticks"
  /** calculate the intersection points("ticks") with a given line. 
      Input - line, paramterised by (Point + Vector * Tick) 
      "Tick" is just a value of parameter, at which the intersection occurs 
      Return the number of intersection points (=size of Ticks container)   
  */
  virtual inline  unsigned int intersectionTicks ( const HepPoint3D  & Point   ,          /// initial point for teh line 
						   const HepVector3D & Vector  ,          /// vector along the line
						   const ISolid::Tick& tickMin ,          /// minimal value of tick 
						   const ISolid::Tick& tickMax ,          /// maximal value of tick 
						   ISolid::Ticks     & ticks   ) const ;  /// output container of "Ticks"
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
  ///
  /// serialization for reading 
  StreamBuffer& serialize( StreamBuffer& s )       ; 
  /// serialization for writing 
  StreamBuffer& serialize( StreamBuffer& s ) const ; 
  ///
  virtual bool acceptInspector( IInspector* )       ; 
  virtual bool acceptInspector( IInspector* ) const ; 
  ///
private:
  //
  inline double iR_z( double z ) const;
  inline double oR_z( double z ) const;
  //
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
///
#include "DetDesc/SolidCons.icpp"
///


#endif //   __DETDESC_SOLID_SOLIDCONS_H__















