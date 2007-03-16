// $Id: SolidSubtraction.h,v 1.15 2007-03-16 15:57:09 cattanem Exp $
/// ===========================================================================
#ifndef       DETDESC_SOLIDSUBTRACTION_H
#define       DETDESC_SOLIDSUBTRACTION_H 1 

/** STD & STL */
#include <algorithm>
#include <functional> 

/** DetDesc package */
#include "DetDesc/SolidBoolean.h" 

/** forward declarations from GaudiKernel and DetDesc */
class StatusCode;
template <class TYPE>
class SolidFactory;

/** @class SolidSubtraction SolidSubtraction.h "DetDesc/SolidSubtraction.h"
 * 
 *  Simple implementatioin of simple boolean solid - SUBTRACTION 
 * 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru 
 *  @date xx/xx/xxxx
 */

class SolidSubtraction: public SolidBoolean
{
  /// friend factory for instantiation 
  friend class SolidFactory<SolidSubtraction>;
  
public:
  
  /** constructor 
   *  @param name name of the intersection
   *  @param first pointer to first/main solid 
   */
  SolidSubtraction( const std::string& name , ISolid* first );
  
  /// destructor   
  virtual ~SolidSubtraction();
  
public:
  
  /** - retrieve the specific type of the solid
   *  - implementation of ISolid interface 
   *  @see ISolid 
   *  @return specific type of the solid
   */
  virtual std::string typeName ( ) const { return "SolidSubtraction" ; };
  
  /** - check for the given 3D-point. 
   *    Point coordinates are in the local reference 
   *    frame of the solid.   
   *  - implementation of ISolid absstract interface  
   *  @see ISolid 
   *  @param point point (in local reference system of the solid)
   *  @return true if the point is inside the solid
   */

  bool isInside ( const Gaudi::XYZPoint& point ) const ;
  bool isInside ( const Gaudi::Polar3DPoint& point ) const ;
  bool isInside ( const Gaudi::RhoZPhiPoint& point ) const ;
  
  /** subtract child solid to the solid
   *  @param solid pointer to child solid 
   *  @param mtrx  pointer to transformation 
   *  @return status code 
   */
  StatusCode subtract 
  ( ISolid*               solid , 
    const Gaudi::Transform3D* mtrx  );
  
  /** subtract child solid from  the solid 
   *  @param child    pointer to child solid 
   *  @param position position  
   *  @param rotation rotation
   *  @return status code 
   */
  StatusCode subtract 
  ( ISolid*               child                    , 
    const Gaudi::XYZPoint&     position                 , 
    const Gaudi::Rotation3D&    rotation = Gaudi::Rotation3D() );
  
protected:
  
  /** constructor 
   *  @param Name name of the solid subtraction  
   */
  SolidSubtraction( const std::string& Name = "Anonymous Subtraction");

private:

  SolidSubtraction           ( const SolidSubtraction& ) ; ///< no copy 
  SolidSubtraction& operator=( const SolidSubtraction& ) ; ///<no = 

  /**
   * implementation of isInside
   * @param  point reference to any kind of point with x(), y(), z()
   * @return bool
   */
  template <class aPoint>
  bool isInsideImpl(const aPoint& point) const;

};

/// ===========================================================================
#endif  ///<   DETDESC_SOLIDSUBTRACTION_H
/// ===========================================================================
