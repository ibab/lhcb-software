// $Id: SolidUnion.h,v 1.11 2005-12-08 19:20:01 jpalac Exp $ 
// ===========================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ===========================================================================
#ifndef       DETDESC_SOLIDUNION_H
#define       DETDESC_SOLIDUNION_H 1 

/** STD & STL */  
#include <algorithm>
#include <functional>

/** DetDesc */
#include "DetDesc/ISolid.h"
#include "DetDesc/SolidBoolean.h" 

/** forward declarations from GaudiKernel and DetDesc  */
class StatusCode;
template <class TYPE> 
class SolidFactory;

/** @class SolidUnion SolidUnion.h "DetDesc/SolidUnion.h"
 *
 *  implementation of simple boolean solid - UNION 
 * 
 *  @bug wrong cover method! 
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date xx/xx/xxx
 */

class SolidUnion: public virtual SolidBoolean
{
  /// frined factory for instantiation 
  friend class SolidFactory<SolidUnion>;
  
public:
  
  /** constructor 
   *  @param name name of the intersection
   *  @param first pointer to first/main solid 
   */
  SolidUnion( const std::string& name , ISolid* first ); 
  
  /// destruictor   
  virtual ~SolidUnion();
  
public:
  
  /** - retrieve the specific type of the solid
   *  - implementation of ISolid interface 
   *  @see ISolid 
   *  @return specific type of the solid
   */
  virtual std::string typeName () const { return "SolidUnion" ; };

  /** - check for the given 3D-point. 
   *    Point coordinates are in the local reference 
   *    frame of the solid.   
   *  - implementation of ISolid absstract interface  
   *  @see ISolid 
   *  @param point point (in local reference system of the solid)
   *  @return true if the point is inside the solid
   */
  bool isInside ( const Gaudi::XYZPoint   & point ) const ;
  bool isInside ( const Gaudi::Polar3DPoint& point ) const ;
  bool isInside ( const Gaudi::RhoZPhiPoint& point ) const ;

  /** retrieve the pointer to "the most simplified cover", 
   *  ideally to something like "the bounding box"
   *  @see ISolid::cover()
   *  @return pointer to the most simplified cover
   */
  virtual const ISolid* coverTop () const ;

  /** add child solid to the solid union
   *  @param solid pointer to child solid 
   *  @param mtrx  pointer to transformation 
   *  @return status code 
   */
  StatusCode unite ( ISolid*               solid , 
                     const Gaudi::Transform3D* mtrx  ); 

  /** add child solid to the solid union
   *  @param child    pointer to child solid 
   *  @param position position
   *  @param rotation rotation
   *  @return status code 
   */
  StatusCode unite ( ISolid*               child                    , 
                     const Gaudi::XYZPoint&     position                 , 
                     const Gaudi::Rotation3D&    rotation = Gaudi::Rotation3D() );
  
 protected:

  /** constructor 
   *  @param Name name of the solid union 
   */
  SolidUnion( const std::string& Name = "Anonymous Union");

  /** update bounding parameters
   *  @return StatusCode 
   */
  StatusCode updateBP();
  
 private:
  
  SolidUnion           ( const SolidUnion& ) ; ///< no copy 
  SolidUnion& operator=( const SolidUnion& ) ; ///< no =

private:
  
  mutable ISolid* m_coverTop ;

  template <class aPoint>
  bool isInsideImpl(const aPoint& point) const;
  
};

// ===========================================================================
#endif  ///<  DETDESC_SOLIDUNION_H
// ===========================================================================


