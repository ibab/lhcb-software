#ifndef     DETDESC_SOLIDTUBS_H
#define     DETDESC_SOLIDTUBS_H 1  
/// STD and STL 
#include <cmath>
#include <iostream> 
/// CLHEP 
#include "CLHEP/Geometry/Point3D.h" 
#include "CLHEP/Geometry/Vector3D.h"
#include "CLHEP/Units/PhysicalConstants.h" 
/// GaudiKernel
#include "DetDesc/ISolid.h" 

class ISolidFromStream;
class StreamBuffer;
class MsgStream;

/** @class SolidTubs SolidTubs.h DetDesc/SolidTubs.h

    A simple implementation of TUBS 
    
    @author Vanya Belyaev 
*/

class SolidTubs: public ISolid
{
  //
  friend class ISolidFromStream;
  //
 public:
  //
  //  constructor, all sizes in mm, all angles in radians 
  SolidTubs( const std::string& name                             ,
             const double       ZHalfLength                      , 
             const double       OuterRadius                      ,
             const double       InnerRadius   =   0.0            , 
             const double       StartPhiAngle =   0.0 * degree   , 
             const double       DeltaPhiAngle = 360.0 * degree   ,
             const int          CoverModel    =   0              );     
  //
  // destructor 
  virtual ~SolidTubs();
  //
  // functions from ISolid:
  // name of this solid 
  inline const std::string&      name          ()                      const { return m_tubs_name  ; };
  // type of this solid 
  inline       std::string       typeName      ()                      const { return "SolidTubs"; };
  // the notorious "isInside" method 
  inline       bool              isInside      ( const HepPoint3D&   ) const; 
  // covering solid 
  inline const ISolid*           cover         ()                      const;
  // "top covering solid" (normally SolidBox) 
  inline const ISolid*           coverTop      ()                      const;
  // overloaded printout 
  virtual std::ostream&     printOut      ( std::ostream& = std::cerr) const;
  virtual MsgStream&        printOut      ( MsgStream&               ) const;
  /// reset to the initial state  
  inline const ISolid*           reset         ()                      const;  
  ///
  /// calculate the intersection points("ticks") with a given line. 
  /// Input - line, paramterised by (Point + Vector * Tick) 
  /// "Tick" is just a value of parameter, at which the intersection occurs 
  /// Return the number of intersection points (=size of Ticks container)   
  virtual inline  unsigned int intersectionTicks ( const HepPoint3D& Point  ,             /// initial point for teh line 
                                                   const HepVector3D& Vector ,             /// vector along the line 
                                                   ISolid::Ticks   & ticks  ) const ;     /// output container of "Ticks"
  virtual inline  unsigned int intersectionTicks ( const HepPoint3D  & Point   ,          /// initial point for the line 
                                                   const HepVector3D  & Vector  ,          /// vector along the line
                                                   const ISolid::Tick& tickMin ,          /// minimal value of tick
                                                   const ISolid::Tick& tickMax ,          /// maximal value of tick 
                                                   ISolid::Ticks     & ticks   ) const ;  /// output container of "Ticks"
  
  //
  // function specific for SolidTubs
  // return inner radius of the tube (in mm)
  inline       double              innerRadius   () const { return m_tubs_innerRadius   ; };
  // return outer radius of the tube (in mm)
  inline       double              outerRadius   () const { return m_tubs_outerRadius   ; };
  // return half-length of the tube (in mm)
  inline       double              zHalfLength   () const { return m_tubs_zHalfLength   ; };
  // return start phi angle of the tube segment (in radians)
  inline       double              startPhiAngle () const { return m_tubs_startPhiAngle ; }; 
  // return delta phi angle of the tube segment (in radians)
  inline       double              deltaPhiAngle () const { return m_tubs_deltaPhiAngle ; }; 
  
  // return inner diameter (in mm) 
  inline       double              innerDiameter   () const { return m_tubs_innerRadius   * 2 ; };
  // return outer diameter (in mm)
  inline       double              outerDiameter   () const { return m_tubs_outerRadius   * 2 ; };
  // return the full length of the tube (in mm)
  inline       double              zLength         () const { return m_tubs_zHalfLength   * 2 ; };
  // return end phi angle of teh tube segment (in radians)  
  inline       double              endPhiAngle     () const { return m_tubs_startPhiAngle + m_tubs_deltaPhiAngle ; }; 

  /// serialization for reading 
  StreamBuffer& serialize( StreamBuffer& s )       ; 
  /// serialization for writing
  StreamBuffer& serialize( StreamBuffer& s ) const ; 
  ///
  /// Inspectable Interface 
  virtual bool acceptInspector( IInspector* )       ; 
  virtual bool acceptInspector( IInspector* ) const ; 
  ///
 protected:
  ///
  SolidTubs();
  ///
 private:
  //
  SolidTubs           ( const SolidTubs & );  // no copy-constructor 
  SolidTubs& operator=( const SolidTubs & );  // no assignment 
  //
  // members
  //
  std::string             m_tubs_name          ;
  double                  m_tubs_zHalfLength   ;
  double                  m_tubs_outerRadius   ;
  double                  m_tubs_innerRadius   ;
  double                  m_tubs_startPhiAngle ;
  double                  m_tubs_deltaPhiAngle ;
  //
  int                     m_tubs_coverModel    ;  // model for constructing the cover solid 
  mutable ISolid*         m_tubs_cover         ;
  //
};
///
#include "DetDesc/SolidTubs.icpp"
///

#endif //   DETDESC_SOLIDTUBS_H

