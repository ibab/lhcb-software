// ============================================================================
#ifndef GIGACNV_GIGACOLLISIONCNV_H
#define GIGACNV_GIGACOLLISIONCNV_H 1 
// ============================================================================
/// base class from GiGaCnv
#include "GiGaCnv/GiGaCnvBase.h" 
#include "GiGaCnv/GiGaLeaf.h" 
/// forward declarations:
template <class TYPE> 
class   CnvFactory;
///


/** @class GiGaCollisionCnv GiGaCollisionCnv.h
 *    
 *  Converter of Collisions
 *  
 *  @author  Witek Pokorski Witold.Pokorski@cern.ch
 *  @date    10/07/2003
 */

class GiGaCollisionCnv: public GiGaCnvBase
{
  ///
  friend class CnvFactory<GiGaCollisionCnv>; 
  /// 
protected:
  
  /** standard constructor
   *  @param Loc pointer to service locator 
   */ 
  GiGaCollisionCnv( ISvcLocator* Loc );

  /// Standard (virtual) destructor 
  virtual ~GiGaCollisionCnv();
  ///
public:

  /** create the Object
   *  @param  Address address of the object 
   *  @param  Object  reference to created object 
   *  @return status code
   */ 
  virtual StatusCode createObj     
  ( IOpaqueAddress*     Address    , 
    DataObject*&        Object     ) ;
  
  /** update the Object
   *  @param  Address address of the object 
   *  @param  Object  reference to updated object 
   *  @return status code
   */ 
  virtual StatusCode updateObj     
  ( IOpaqueAddress*     Address    , 
    DataObject*         Object     ) ;  
  
  /** class ID for created object = class ID for this specific converter
   *  @return class id of created objects
   */
  static const CLID&          classID();

  /** storage type 
   *  @return storage type 
   */
  static const unsigned char storageType() ; 

  /** initialize the converter
   *  @return status code 
   */
  virtual StatusCode initialize();
  
  /** finalize  the converter
   *  @return status code 
   */
  virtual StatusCode finalize ();

  /** auxiliary method to assign collisions to particles and vertices
   *  @return status code 
   */
  StatusCode PointToCollision(MCVertex*, Collision*);
  

  ///
private:
  ///
  GiGaCollisionCnv () ; ///< no default constructor 
  GiGaCollisionCnv ( const GiGaCollisionCnv& ); ///< no copy
  GiGaCollisionCnv& operator=( const GiGaCollisionCnv& ); ///< no assignment  
  ///
private:

  GiGaLeaf m_leaf;
  
};


#endif   //     













