#ifndef         __GIGA_KINECONVERSION_GIGAMCVERTEXCNV_H__
#define         __GIGA_KINECONVERSION_GIGAMCVERTEXCNV_H__  1 


///
/// from STL 
#include <set>

///
/// base class from GiGa
#include "GiGa/GiGaCnv.h" 
template <class TYPE> class CnvFactory;


class G4PrimaryVertex;
class G4PrimaryParticle; 
class G4ParticleDefinition; 


///
///
///


class GiGaMCVertexCnv: public GiGaCnv
{
  ///
  friend class CnvFactory<GiGaMCVertexCnv>; 
  ///
 
 protected: 

  /// Standard Constructor
  GiGaMCVertexCnv( ISvcLocator* Locator );
  /// Standard (virtual) destructor 
  virtual ~GiGaMCVertexCnv();
  ///

 public:

  /// Create representation 
  virtual StatusCode createRep(DataObject*     Object  , IOpaqueAddress*& Address ) ;
  /// Update representation 
  virtual StatusCode updateRep(DataObject*     Object  , IOpaqueAddress*  Address ) ; 
  /// Class ID for created object == class ID for this specific converter
  static const CLID&          classID();
  /// storage Type 
  static const unsigned char storageType() ; 
  ///

 protected: 

  ///
  /// miscellaneous functions which performs the conversion itself
  ///  
  G4PrimaryVertex*      VertexFromMCVertex     ( const MCVertex*   vertex   ) ; 
  G4PrimaryParticle*    ParticleFromMCParticle ( const MCParticle* particle ) ;
  G4ParticleDefinition* ParticleDefinition     ( const MCParticle* particle ) ; 

 private: 
 
};


#endif   //     __GIGA_KINECONVERSION_GIGAMCVERTEXCNV_H__














