// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/DetDesc/SolidIntersection.h,v 1.5 2001-03-15 12:43:39 ibelyaev Exp $
#ifndef      DETDESC_SOLISINTERSECTION_H
#define      DETDESC_SOLIDINTERSECTION_H 1 

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
class StreamBuffer;


class SolidIntersection: public SolidBoolean
{
  ///
  friend class ISolidFromStream;
  ///  
 public:
  //
  SolidIntersection( const std::string& name , ISolid* first ); 
  //  
  virtual ~SolidIntersection();
  //  
 public:
  
  // from ISolid 
  virtual        std::string   typeName (                      ) const { return "SolidIntersection" ; }; 
  inline         bool          isInside ( const HepPoint3D   & ) const ;
  
  // specific 
  inline         StatusCode    intersect( ISolid*               solid                    , 
                                          const HepTransform3D* mtrx                     );
  
  inline         StatusCode    intersect( ISolid*               child                    , 
                                          const HepPoint3D&     position                 , 
                                          const HepRotation&    rotation = HepRotation() );
 protected:
  ///
  SolidIntersection();
  ///
 private:
  ///
  SolidIntersection           ( const SolidIntersection& );
  SolidIntersection& operator=( const SolidIntersection& );
  ///
};

///
///
///

inline bool SolidIntersection::isInside     ( const HepPoint3D   & point ) const 
{ 
  //  is point inside the "main" volume?  
  if ( !first()->isInside( point ) ) { return false; } 
  
  // find the first daughter in which the given point is NOT placed   
  SolidBoolean::SolidChildrens::const_iterator ci = 
    std::find_if( childBegin() , childEnd() , std::not1(Solid_isInside( point )) );
  
  return ( childEnd() == ci ? true : false );   
};

//
//
//

inline StatusCode  SolidIntersection::intersect( ISolid*               solid     , 
                                                 const HepTransform3D* mtrx      )
{  return addChild( solid , mtrx ); };

//
//
//

inline StatusCode  SolidIntersection::intersect ( ISolid*               solid    , 
                                                  const HepPoint3D&     position , 
                                                  const HepRotation&    rotation )
{ return addChild( solid , position , rotation ) ; }; 

 
//
//
//


#endif  //    DETDESC_SOLIDINTERSECTION_H




