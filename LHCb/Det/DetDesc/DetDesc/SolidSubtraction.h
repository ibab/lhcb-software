/// ===========================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
/// ===========================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.6  2001/08/09 16:47:59  ibelyaev
/// update in interfaces and redesign of solids
/// 
/// ===========================================================================
#ifndef       DETDESC_SOLISSUBTRACTION_H
#define       DETDESC_SOLIDSUBTRACTION_H 1 
///@{
/** STD & STL */
#include <algorithm>
#include <functional> 
///@}
/// CLHEP 
#include "CLHEP/Vector/Rotation.h" 
///@{
/** DetDesc package */
#include "DetDesc/SolidBoolean.h" 
#include "DetDesc/SolidChild.h" 
///@}
///@{
/** forward declarations from GaudiKernel, CLHEP and DetDesc */
class StatusCode;
class HepTransform3D;
class HepRotate;
class HepPoint3D; 
template <class TYPE>
class SolidFactory;
///@}

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
  bool isInside ( const HepPoint3D& point ) const ;
  
  /** subtract child solid to the solid
   *  @param solid pointer to child solid 
   *  @param mtrx  pointer to transformation 
   *  @return status code 
   */
  StatusCode subtract 
  ( ISolid*               solid , 
    const HepTransform3D* mtrx  );
  
  /** subtract child solid from  the solid 
   *  @param solid pointer to child solid 
   *  @param position position  
   *  @return status code 
   */
  StatusCode subtract 
  ( ISolid*               child                    , 
    const HepPoint3D&     position                 , 
    const HepRotation&    rotation = HepRotation() );
  
protected:
  
  /** constructor 
   *  @param name name of the solid subtraction  
   */
  SolidSubtraction( const std::string& Name = "Anonymous Subtraction");

private:

  SolidSubtraction           ( const SolidSubtraction& ) ; ///< no copy 
  SolidSubtraction& operator=( const SolidSubtraction& ) ; ///<no = 

};

/// ===========================================================================
#endif  ///<   DETDESC_SOLIDSUBTRACTION_H
/// ===========================================================================


