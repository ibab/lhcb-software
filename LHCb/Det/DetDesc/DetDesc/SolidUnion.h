#ifndef       __DETDESC_SOLID_SOLIDUNION_H__
#define       __DETDESC_SOLID_SOLIDUNION_H__

//
//
//


#include <deque> 
#include <algorithm>
#include <functional> 

class StatusCode;

#include "DetDesc/ISolid.h"
#include "DetDesc/SolidBoolean.h" 
#include "DetDesc/SolidChild.h" 

#include "CLHEP/Vector/Rotation.h" 

class HepTransform3D;
class HepRotation;
class HepPoint3D; 
class ISolidFromStream;

class SolidUnion: public SolidBoolean
{
  ///
  friend class ISolidFromStream;
  ///  
 public:
  //
  SolidUnion( const std::string& name , ISolid* first ); 
  //  
  virtual ~SolidUnion();
  //  
 public:
  
  // from ISolid 
  virtual        std::string   typeName (                      ) const { return "SolidUnion" ; }; 
  inline         bool          isInside ( const HepPoint3D   & ) const ;
  
  // specific 
  inline         StatusCode    unite   ( ISolid*               solid                    , 
                                         const HepTransform3D* mtrx                     ); 
  inline         StatusCode    unite   ( ISolid*               child                    , 
                                         const HepPoint3D&     position                 , 
                                         const HepRotation&    rotation = HepRotation() );
  ///
 protected:
  ///
  SolidUnion();
  ///
 private:
  ///
  SolidUnion           ( const SolidUnion& ) ;
  SolidUnion& operator=( const SolidUnion& ) ;
  //
};

//
//
//

inline StatusCode  SolidUnion::unite( ISolid*                solid    , 
                                      const HepTransform3D*  mtrx     )
{  return addChild( solid , mtrx ); };

//
//
//

inline StatusCode  SolidUnion::unite ( ISolid*               solid    , 
                                       const HepPoint3D&     position , 
                                       const HepRotation&    rotation )
{ return addChild( solid , position , rotation ) ; }; 

//
//
//

inline bool SolidUnion::isInside     ( const HepPoint3D   & point ) const 
{ 
  //  is point inside the "main" volume?  
  if ( first()->isInside( point ) ) { return true; } 
  
  // find the first daughter in which the given point is placed   
  SolidUnion::SolidChildrens::const_iterator ci = 
    std::find_if( childBegin() , childEnd() , Solid_isInside( point ) );
  
  return ( childEnd() == ci ? false : true );   
};

//
//
//


#endif  //    __DETDESC_SOLID_SOLIDUNION_H__


