#pragma once

// Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

/** @class MaskParticles MaskParticles.h
 *
 *  Algorithm to prevent certain classes of generated particles from 
 *  being transmitted to the simulation phase by setting their status
 *  to DocumentationParticle.
 *
 */

class MaskParticles : public GaudiAlgorithm {

 public: 
  /// Standard constructor
  MaskParticles(const std::string& name, ISvcLocator* pSvcLocator);
  /// Destructor
  virtual ~MaskParticles();

  virtual StatusCode execute();       ///< Algorithm execution

 private: 
  /// TES location of input HepMC events.
  std::string m_inputLocation; 
  /// PDG ID of particles to be masked.
  int m_pid;
  /// Min. pseudorapidity required for a particle to be masked.
  double m_etaCut;
  /// Min. energy required for a particle to be masked.
  double m_energyCut;

};
