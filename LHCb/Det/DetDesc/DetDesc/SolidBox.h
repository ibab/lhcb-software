#ifndef     __DETDESC_SOLID_SOLIDBOX_H__
#define     __DETDESC_SOLID_SOLIDBOX_H__ 1 

#include <cmath> 
#include <iostream> 

#include "GaudiKernel/ISolid.h" 


#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Vector3D.h"

#include "CLHEP/Units/SystemOfUnits.h"

///
///
/// class SolidBox: a simple implementation of BOX 
///
/// Author: Vanya Belyaev 
///
///

class SolidBox: public ISolid
{
  //
 public:
  //
  // constructor & destructor 
  //
  // constructor! NB - no "default values"! Units from CLHEP/Units/SystemOfUnits (mm) 
  SolidBox( const std::string& name        ,  // name of this box                     
	    const double       XHalfLength ,  // half-size in x-direction of this box 
            const double       YHalfLength ,  // half-size in y-direction of this box 
            const double       ZHalfLength ); // half-size in z-direction of this box 
  //
  virtual ~SolidBox(){};
  
  //
  //
  // functions from ISolid:
  inline  const  std::string&      name      ()                    const { return m_box_name   ; };
  inline         std::string       typeName  ()                    const { return "SolidBox"   ; };
  inline         bool              isInside  ( const HepPoint3D& ) const; 
  inline  const  ISolid*           cover     ()                    const { return this         ; };
  inline  const  ISolid*           coverTop  ()                    const { return this         ; };
  
  virtual        std::ostream&     printOut  ( std::ostream&     ) const;

  /// reset to the initial state 
  inline  const  ISolid*           reset     ()                    const { return this         ; };
  
  // calculate the intersection points("ticks") with a given line. 
  // Input - line, paramterised by (Point + Vector * Tick) 
  // "Tick" is just a value of parameter, at which the intercestion occurs 
  // Return the number of intersection points (=size of Ticks container)  
  virtual inline  unsigned int intersectionTicks ( const HepPoint3D&  Point  ,          // initial point for teh line 
						   const HepVector3D& Vector ,          // vector along the line 
						   ISolid::Ticks   &  ticks  ) const ;  // output container of "Ticks"
  // calculate the intersection points("ticks") with a given line. 
  // Input - line, paramterised by (Point + Vector * Tick) 
  // "Tick" is just a value of parameter, at which the intercestion occurs 
  // Return the number of intersection points (=size of Ticks container)   between tickMin and tickMax 
  virtual inline  unsigned int intersectionTicks ( const HepPoint3D  & Point   ,          // initial point for teh line 
						   const HepVector3D & Vector  ,          // vector along the line 
                                                   const ISolid::Tick& tickMin , 
                                                   const ISolid::Tick& tickMax , 
						   ISolid::Ticks     & ticks   ) const ;  // output container of "Ticks"

  //
  //
  // function specific for SolidBox
  //

  // return the full x-size of the box 
  inline         double  xsize      ()  const { return m_box_xHalfLength*2 ; };
  // return the full y-size of the box 
  inline         double  ysize      ()  const { return m_box_yHalfLength*2 ; };
  // return the full z-size of the box 
  inline         double  zsize      ()  const { return m_box_zHalfLength*2 ; };

  // return the half x-size of the box 
  inline         double  xHalfLength()  const { return m_box_xHalfLength   ; };
  // return the half y-size of the box 
  inline         double  yHalfLength()  const { return m_box_yHalfLength   ; };
  // return the half z-size of the box 
  inline         double  zHalfLength()  const { return m_box_zHalfLength   ; };

  
  //
 private:
  //
  SolidBox           ( const SolidBox & );  // no copy-constructor 
  SolidBox& operator=( const SolidBox & );  // no assignment 
  //
  // members
  //
  std::string          m_box_name  ;
  double               m_box_xHalfLength ;
  double               m_box_yHalfLength ;
  double               m_box_zHalfLength ;
  //
};

//
//
//

inline bool SolidBox::isInside( const HepPoint3D& point ) const
{ 
  if ( abs( point.x()) > xHalfLength() || 
       abs( point.y()) > yHalfLength() ||
       abs( point.z()) > zHalfLength() ) { return false; }
  return true; 
};

//
//
//

inline std::ostream& SolidBox::printOut  ( std::ostream&  os ) const
{
  return os << typeName() << " name="             << name()
	    << " (xsize=" << xsize() / millimeter << "[mm],"  
	    <<   "ysize=" << ysize() / millimeter << "[mm],"  
	    <<   "zsize=" << zsize() / millimeter << "[mm])";  
};

//
//
//

#endif //   __DETDESC_SOLID_SOLIDBOX_H__ 






