#ifndef       __DETDESC_SOLID_SOLISSUBTRACTION_H__
#define       __DETDESC_SOLID_SOLIDSUBTRACTION_H__

#include <deque> 
#include <algorithm>
#include <functional> 

#include "GaudiKernel/ISolid.h"
#include "DetDesc/SolidBoolean.h" 
#include "DetDesc/SolidChild.h" 


#include "CLHEP/Vector/Rotation.h" 

//
//
//  class SolidSubtraction: represent a "complex" solid. 
//                          
//          usage: 
//  (in this example, box with cavities) 
//
// ISolid* s1 = new SolidBox("box1",10.0*cm,20.0*cm,30.0*cm);  // prepare "main" solid
// 
// ISolid* s  = new SolidSubtraction("subtraction",s1);        // activate subtraction
// 
// ISolid* s2 = new SolidTubs("box2",2.0*cm,1.0*cm,0.5*cm);    // prepare 1st child 
// ISolid* s3 = new SolidTubs("box2",2.0*cm,1.0*cm,0.5*cm);    // prepare 2nd child 
// /* ......*/ 
// ISolid* sn = new SolidTubs("boxn",1.0*cm,1.0*cm,0.5*cm);    // prepare nth child 
// 
// s->subtract(s2);
// s->subtract(s3);
// /* .... */
// s->subtract(sn);
//
// 



class StatusCode;
class HepTransform3D;
class HepRotate;
class HepPoint3D; 

class SolidSubtraction: public SolidBoolean
{
  //
  friend class ISolidFromStream;
  //
 public:
  //
  SolidSubtraction( const std::string& name , ISolid* first ); 
  //  
  virtual ~SolidSubtraction();
  //  
 public:
  
  // from ISolid 
  virtual        std::string   typeName (                      ) const { return "SolidSubtraction" ; }; 
  inline         bool          isInside ( const HepPoint3D   & ) const ;
  
  // specific 
  inline         StatusCode    subtract ( ISolid*               solid , 
                                          const HepTransform3D* mtrx  ); 

  // add child to daughter container 
  inline         StatusCode    subtract ( ISolid*               child                    , 
                                          const HepPoint3D&     position                 , 
                                          const HepRotation&    rotation = HepRotation() );
  
 protected:
  ///
  SolidSubtraction();
  ///
 private:
  ///
  SolidSubtraction           ( const SolidSubtraction& ) ; 
  SolidSubtraction& operator=( const SolidSubtraction& ) ; 
  ///
};

//
//
//

inline StatusCode  SolidSubtraction::subtract( ISolid*                solid    , 
                                               const HepTransform3D*  mtrx     )
{  return addChild( solid , mtrx ); };

//
//
// 

inline StatusCode  SolidSubtraction::subtract ( ISolid*               solid    , 
                                                const HepPoint3D&     position , 
                                                const HepRotation&    rotation )
{ return addChild( solid , position , rotation ) ; }; 

 
//
//
//



//
//
//

inline bool SolidSubtraction::isInside     ( const HepPoint3D   & point ) const 
{ 
  //  is point inside the "main" volume?  
  if ( !first()->isInside( point ) ) { return false; } 
  
  // find the first daughter in which the given point is placed   
  SolidSubtraction::SolidChildrens::const_iterator ci = 
    std::find_if( childBegin() , childEnd() , Solid_isInside( point ) );
  
  return ( childEnd() == ci ? true : false );   
};

//
//
//


#endif  //    __DETDESC_SOLID_SOLIDSUBTRACTION_H__


