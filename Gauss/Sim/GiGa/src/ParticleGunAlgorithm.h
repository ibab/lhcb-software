#ifndef                __GIGA_ALGORITHMS_PARTICLEGUNALGORITHM_H__
#define                __GIGA_ALGORITHMS_PARTICLEGUNALGORITHM_H__  1 

#include <string>
#include <vector> 
///
/// from CLHEP
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Vector3D.h"
#include "CLHEP/Units/SystemOfUnits.h"


///
/// base class from Gaudi 
#include "GaudiKernel/Algorithm.h" 

///
/// forward declarations 
template<class ALGORITHM> class AlgFactory; 
class IParticlePropertySvc;
class  ParticleProperty; 

///
///
///  ParticleGunAlgorithm: definition of the simplest "generator" algorithm
///
///  Author: Vanya Belyaev 
///  Date  : 10 Aug 2000 
////

 
class ParticleGunAlgorithm: public Algorithm
{

  friend class AlgFactory<ParticleGunAlgorithm>; 

 protected:

  ///
  /// constructor 
  /// 
  ParticleGunAlgorithm( const std::string&  AlgorithmName  , 
			ISvcLocator*        ServiceLocator ); 

  ///
  /// destructor 
  ///
   
  virtual ~ParticleGunAlgorithm(); 

  ///
  ///
  ///
  
 public:
  
  virtual StatusCode  initialize(); 
  virtual StatusCode  execute   (); 
  virtual StatusCode  finalize  (); 

 protected: 

  IParticlePropertySvc* ppSvc() { return m_ParticlePropertySvc; } 
  
 private: 
  
  ///
  ///
  /// 
  
  std::string              m_NameParticlePropertySvc            ;
  IParticlePropertySvc*    m_ParticlePropertySvc                ;
  
  ///
  ///
  /// 
  
  std::string              m_NameOfOutputMCVertexContainer      ;
  
  ///
  ///
  ///
  
  std::string              m_NameOfOutputMCParticleContainer    ;
  
  ///
  ///
  ///
  std::string              m_ParticleName                       ; 
  ParticleProperty*        m_ParticleProperty                   ; 
  HepPoint3D               m_PrimaryVertexPosition              ; 
  std::vector<double>      m_AuxVertexPosition                  ;      
  HepVector3D              m_ParticleMomentum                   ; 
  std::vector<double>      m_AuxParticleMomentum                ;      
  ///
  ///
  ///

};


#endif         //      __GIGA_ALGORITHMS_PARTICLEGUNALGORITHM_H__







