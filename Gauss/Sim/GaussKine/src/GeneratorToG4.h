// $Id: GeneratorToG4.h,v 1.2 2008-06-12 18:45:16 robbep Exp $
#ifndef GENERATORTOG4_H 
#define GENERATORTOG4_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/RndmGenerators.h"

// Forward declarations
class IGiGaSvc;
class G4PrimaryParticle;
class IParticlePropertySvc ;

/** @class GeneratorToG4 GeneratorToG4.h
 *  Algorithm to fill Geant4 Primary from HepMCEvents as filled by
 *  the generator phase of Gauss
 *
 *  @author Gloria CORTI
 *  @date   2005-10-06
 */
class GeneratorToG4 : public GaudiAlgorithm {
public: 
  /// Standard constructor
  GeneratorToG4( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~GeneratorToG4( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

  /** accessor to GiGa Service
   *  @return pointer to GiGa Service
   */
  IGiGaSvc* gigaSvc() const { 
    return m_gigaSvc; 
  }

private:

  // Method to make a Geant4 particle from a generator one
  G4PrimaryParticle* makeG4Particle(HepMC::GenParticle*, LHCb::HepMCEvent*); 
  
  std::string        m_gigaSvcName;      ///< Name of GiGa Service
  IGiGaSvc*          m_gigaSvc;          ///< Pointer to GiGa Service

  std::string        m_location;         ///< Location in TES of input HepMC events

  Rndm::Numbers m_rand ;  
  IParticlePropertySvc * m_pSvc ;
};
#endif // GENERATORTOG4_H
