// ============================================================================
/// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.3  2001/07/24 11:13:56  ibelyaev
/// package restructurization(III) and update for newer GiGa
///
/// Revision 1.2  2001/07/15 20:45:12  ibelyaev
/// the package restructurisation
/// 
// ============================================================================
#ifndef GIGACNV_GIGAMCVERTEXCNV_H
#define GIGACNV_GIGAMCVERTEXCNV_H 1 
// ============================================================================
/// STL 
#include <set>
/// base class from GiGaCnv
#include "GiGaCnv/GiGaCnvBase.h" 
/// forward declarations:
template <class TYPE> 
class   CnvFactory;
///
class   G4PrimaryVertex;
class   G4PrimaryParticle; 
class   G4ParticleDefinition; 


/** @class GiGaMCVertexCnv GiGaMCVertexCnv.h
 *    
 *  Converter of Geant4 trajectories into Gaudi MCVertices 
 *  and Gaudi MCVertices into Geant4 G4PrimaryVertices 
 *  
 *  @author  Vanya Belyaev
 *  @date    22/02/2001
 */

class GiGaMCVertexCnv: public GiGaCnvBase
{
  ///
  friend class CnvFactory<GiGaMCVertexCnv>; 
  /// 
protected:
  
  /** standard constructor
   *  @param Loc pointer to service locator 
   */ 
  GiGaMCVertexCnv( ISvcLocator* Loc );

  /// Standard (virtual) destructor 
  virtual ~GiGaMCVertexCnv();
  ///
public:

  /** create the Object
   *  @param  Address address of the object 
   *  @param  Object  reference to created object 
   *  @return status code
   */ 
  virtual StatusCode createObj     ( IOpaqueAddress*     Address    , 
                                     DataObject*&        Object     ) ;

  /** update the Object
   *  @param  Address address of the object 
   *  @param  Object  reference to updated object 
   *  @return status code
   */ 
  virtual StatusCode updateObj     ( IOpaqueAddress*     Address    , 
                                     DataObject*         Object     ) ; 

  /** fill the object references 
   *  @param  Address address of the object 
   *  @param  Object  reference to the object 
   *  @return status code
   */ 
  virtual StatusCode fillObjRefs   ( IOpaqueAddress*     Address    , 
                                     DataObject*         Object     ) ;

  /** update the object references 
   *  @param  Address address of the object 
   *  @param  Object  reference to the object 
   *  @return status code
   */ 
  virtual StatusCode updateObjRefs ( IOpaqueAddress*     Address    , 
                                     DataObject*         Object     ) ;

  /** create the representation of the object 
   *  @param  Object  reference to the object 
   *  @param  Address address of the object 
   *  @return status code
   */ 
  virtual StatusCode createRep     ( DataObject*         Object     , 
                                     IOpaqueAddress*&    Address    ) ;

  /** update the representation of the object 
   *  @param  Object  reference to the object 
   *  @param  Address address of the object 
   *  @return status code
   */ 
  virtual StatusCode updateRep     ( DataObject*         Object     , 
                                     IOpaqueAddress*     Address    ) ; 
  
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
  ///
private:
  ///
  GiGaMCVertexCnv () ; ///< no default constructor 
  GiGaMCVertexCnv ( const GiGaMCVertexCnv& ); ///< no copy
  GiGaMCVertexCnv& operator=( const GiGaMCVertexCnv& ); ///< no assignment  
  ///
};


#endif   //     __GIGA_KINECONVERSION_GIGAMCVERTEXCNV_H__














