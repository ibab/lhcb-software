#ifndef     DETDESC_SOLIDBOX_H
#define     DETDESC_SOLIDBOX_H 1 
/// STD and STL 
#include <cmath> 
#include <iostream> 
/// CLHEP 
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Vector3D.h"
#include "CLHEP/Units/SystemOfUnits.h"
/// GAUDI Kernel 
#include "GaudiKernel/ISolid.h" 
#include "GaudiKernel/StreamBuffer.h" 

class MsgStream;
class ISolidFromStream;

/** @class SolidBox SolidBox.h DetDesc/SolidBox.h

    A simple implementation of BOX 
    
    @author Vanya Belyaev 
*/


class SolidBox: public ISolid
{
  ///
  friend class ISolidFromStream;
  ///
public:
  //
  SolidBox( const std::string& name        ,  // name of this box                     
	    const double       XHalfLength ,  // half-size in x-direction of this box 
            const double       YHalfLength ,  // half-size in y-direction of this box 
            const double       ZHalfLength ); // half-size in z-direction of this box 
  //
  virtual ~SolidBox();  
  //
  // functions from ISolid:
  inline  const  std::string&      name      ()                    const { return m_box_name   ; };
  inline         std::string       typeName  ()                    const { return "SolidBox"   ; };
  inline         bool              isInside  ( const HepPoint3D& ) const; 
  inline  const  ISolid*           cover     ()                    const { return this         ; };
  inline  const  ISolid*           coverTop  ()                    const { return this         ; };
  
  virtual        std::ostream&     printOut  ( std::ostream&     ) const;
  virtual        MsgStream&        printOut  ( MsgStream&        ) const;
  
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
  /// serialization for reading 
  StreamBuffer& serialize( StreamBuffer& s )       ; 
  /// serialization for writing 
  StreamBuffer& serialize( StreamBuffer& s ) const ; 
  /// from IInspectable interface  
  virtual bool acceptInspector( IInspector* pInspector )       ; 
  virtual bool acceptInspector( IInspector* pInspector ) const ; 
  /// 
protected:
  ///
  SolidBox           (                 ); /// default constructor
  ///
private:
  //
  SolidBox           ( const SolidBox & );  // no copy-constructor 
  SolidBox& operator=( const SolidBox & );  // no assignment 
  //
private:
  //
  std::string          m_box_name  ;
  double               m_box_xHalfLength ;
  double               m_box_yHalfLength ;
  double               m_box_zHalfLength ;
  //
};
///
#include "DetDesc/SolidBox.icpp"
///


#endif //  DETDESC_SOLIDBOX_H






