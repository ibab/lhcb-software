/// ===========================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
/// ===========================================================================
/// $Log: not supported by cvs2svn $ 
/// ===========================================================================
#ifndef       DETDESC_SOLIDUNION_H
#define       DETDESC_SOLIDUNION_H 1 
///@{ 
/** STD & STL */  
#include <algorithm>
#include <functional>
///@} 
/// CLHEP
#include "CLHEP/Vector/Rotation.h" 
///@{
/** DetDesc */
#include "DetDesc/ISolid.h"
#include "DetDesc/SolidBoolean.h" 
#include "DetDesc/SolidChild.h" 
///@}
///@{
/** forward declarations from GaudiKernel and DetDesc  */
class StatusCode;
class HepTransform3D;
class HepRotation;
class HepPoint3D; 
template <class TYPE> 
class SolidFactory;
///@}

/** @class SolidUnion SolidUnion.h "DetDesc/SolidUnion.h"
 *
 *  implementation of simple boolean solid - UNION 
 * 
 *  @bug wrong cover method! 
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date xx/xx/xxx
 */

class SolidUnion: public SolidBoolean
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
  bool isInside ( const HepPoint3D   & point ) const ;

  /** add child solid to the solid union
   *  @param solid pointer to child solid 
   *  @param mtrx  pointer to transformation 
   *  @return status code 
   */
  StatusCode unite ( ISolid*               solid , 
                     const HepTransform3D* mtrx  ); 

  /** add child solid to the solid union
   *  @param solid pointer to child solid 
   *  @param position position  
   *  @return status code 
   */
  StatusCode unite ( ISolid*               child                    , 
                     const HepPoint3D&     position                 , 
                     const HepRotation&    rotation = HepRotation() );
  
 protected:

  /** constructor 
   *  @param name name of the solid union 
   */
  SolidUnion( const std::string& Name = "Anonymous Union");
  
 private:
  
  SolidUnion           ( const SolidUnion& ) ; ///< no copy 
  SolidUnion& operator=( const SolidUnion& ) ; ///< no =
  
};

/// ===========================================================================
#endif  ///<  DETDESC_SOLIDUNION_H
/// ===========================================================================


