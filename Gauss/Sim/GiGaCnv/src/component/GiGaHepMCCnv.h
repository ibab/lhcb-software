// 
//  Converter from HepMC to G4
//
//  initial coding by W. Pokorski 15.02.2002 (based on GiGaMCVertexCnv by I. Belyaev)
//
//
//
// ============================================================================
#ifndef GIGACNV_GIGAHEPMCCNV_H
#define GIGACNV_GIGAHEPMCCNV_H 1 
// ============================================================================
/// STL 
#include <set>
/// base class from GiGaCnv
#include "GiGaCnv/GiGaCnvBase.h" 
#include "GiGaCnv/GiGaLeaf.h" 
/// forward declarations:
template <class TYPE> 
class   CnvFactory;
///
class   G4PrimaryVertex;
class   G4PrimaryParticle; 
class   G4ParticleDefinition; 




class GiGaHepMCCnv: public GiGaCnvBase
{
  ///
  friend class CnvFactory<GiGaHepMCCnv>; 
  /// 
protected:

  typedef ObjectVector<HepMC::GenVertex>   Vertices  ;
  typedef ObjectVector<HepMC::GenParticle> Particles ;
    


protected:
  
  /** standard constructor
   *  @param Loc pointer to service locator 
   */ 
  GiGaHepMCCnv( ISvcLocator* Loc );

  /// Standard (virtual) destructor 
  virtual ~GiGaHepMCCnv();
  ///
public:
  
  /** create the representation of the object 
   *  @param  Object  reference to the object 
   *  @param  Address address of the object 
   *  @return status code
   */ 
  virtual StatusCode createRep     
  ( DataObject*         Object     , 
    IOpaqueAddress*&    Address    ) ;
  
  /** update the representation of the object 
   *  @param  Object  reference to the object 
   *  @param  Address address of the object 
   *  @return status code
   */ 
  virtual StatusCode updateRep     
  ( DataObject*         Object     , 
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
  GiGaHepMCCnv () ; ///< no default constructor 
  GiGaHepMCCnv ( const GiGaHepMCCnv& ); ///< no copy
  GiGaHepMCCnv& operator=( const GiGaHepMCCnv& ); ///< no assignment  
  ///
private:

  GiGaLeaf m_leaf;
  
};


// ============================================================================
// End 
// ============================================================================
#endif   //     __GIGA_KINECONVERSION_GIGAHEPMCCNV_H__
// ============================================================================














