// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Sim/GiGaCnv/src/component/GiGaMCVertexCnv.h,v 1.1.1.1 2001-04-23 08:34:15 ibelyaev Exp $ 
#ifndef         GIGA_GIGAMCVERTEXCNV_H__
#define         GIGA_GIGAMCVERTEXCNV_H__  1 
/// STL 
#include <set>
/// base class from GiGa
#include "GiGaCnv/GiGaCnvBase.h" 
/// forward declarations:
template <class TYPE> 
class   CnvFactory;
///
class   G4PrimaryVertex;
class   G4PrimaryParticle; 
class   G4ParticleDefinition; 


/** @class GiGaMCVertexCnv GiGaMCVertexCnv.h GiGa/GiGaMCVertexCnv.h
    
    Converter of Geant4 trajectories into Gaudi MCVertices 
    and Gaudi MCVertices into Geant4 G4PrimaryVertices 
    
    @author  Vanya Belyaev
    @date    22/02/2001
*/


class GiGaMCVertexCnv: public GiGaCnvBase
{
  ///
  friend class CnvFactory<GiGaMCVertexCnv>; 
  /// 
protected: 
  /// Standard Constructor
  GiGaMCVertexCnv( ISvcLocator* );
  /// Standard (virtual) destructor 
  virtual ~GiGaMCVertexCnv();
  ///
public:
  /// create Object 
  virtual StatusCode createObj     ( IOpaqueAddress* /* Address */ , DataObject*&     /* Object  */ ) ;
  /// update Object 
  virtual StatusCode updateObj     ( IOpaqueAddress* /* Address */ , DataObject*      /* Object  */ ) ; 
  /// fill the references 
  virtual StatusCode fillObjRefs   ( IOpaqueAddress* /* Address */ , DataObject*      /* Object  */ ) ;
  /// update the references 
  virtual StatusCode updateObjRefs ( IOpaqueAddress* /* Address */ , DataObject*      /* Object  */ ) ;
  /// Create representation 
  virtual StatusCode createRep     ( DataObject*     /* Object  */ , IOpaqueAddress*& /* Address */ ) ;
  /// Update representation 
  virtual StatusCode updateRep     ( DataObject*     /* Object  */ , IOpaqueAddress*  /* Address */ ) ; 
  /// Class ID for created object == class ID for this specific converter
  static const CLID&          classID();
  /// storage Type 
  static const unsigned char storageType() ; 
  ///
private:
  ///
  GiGaMCVertexCnv           ()                       ; /// no default constructor 
  GiGaMCVertexCnv           ( const GiGaMCVertexCnv& ); /// no copy
  GiGaMCVertexCnv& operator=( const GiGaMCVertexCnv& ); /// no assignment  
  ///
protected: 
  /// miscellaneous functions which performs the conversion itself
  G4PrimaryVertex*      VertexFromMCVertex     ( const MCVertex*   vertex   ) ; 
  G4PrimaryParticle*    ParticleFromMCParticle ( const MCParticle* particle ) ;
  G4ParticleDefinition* ParticleDefinition     ( const MCParticle* particle ) ; 
  ///
};


#endif   //     __GIGA_KINECONVERSION_GIGAMCVERTEXCNV_H__














