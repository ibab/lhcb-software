// $Id: GenMonitorAlg.h,v 1.1 2005-04-12 13:00:21 gcorti Exp $
#ifndef GAUSSMONITOR_GENMONITORALG_H 
#define GAUSSMONITOR_GENMONITORALG_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// AIDA
#include "AIDA/IHistogram1D.h"


/** @class GenMonitorAlg GenMonitorAlg.h Algorithms/GenMonitorAlg.h
 *  
 *  Monitoring algorithms for the generator sequences
 *  
 *  @author Patrick Robbe (modified G.Corti)
 *  @date   2005-04-11
 */
class GenMonitorAlg : public GaudiAlgorithm {
public:
  /// Standard constructor
  GenMonitorAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~GenMonitorAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  bool           m_doHistos;            // fill monitor histograms
  std::string    m_dataPath;            // location of input data
  double         m_minEta;              // Min pseudo rapidity acceptance
  double         m_maxEta;              // Max pseudo rapidity acceptance
  IHistogram1D*  m_multiplicity;        // Multiplicity all particles
  IHistogram1D*  m_multiplicitystable;  // Multiplicity particles stable for Pythia
  IHistogram1D*  m_multiplicityCharged; // Multiplicity charged stable particles 
  IHistogram1D*  m_multChEtaAcc;        // Multiplicity charged stable particles in eta
  IHistogram1D*  m_momentum;            // Momentum all particles
  IHistogram1D*  m_momentumstable;      // Momentum particles stable for Pythia
  IHistogram1D*  m_pdgId;               // PDG Id all particles
  IHistogram1D*  m_pdgIdstable;         // PDG Id particles stable for Pythia
  IHistogram1D*  m_lifetimestable;      // lifetime particles stable for Pythia
  IHistogram1D*  m_pseudoRapCharged;    // Pesudo rapidity charged stable particles
  IHistogram1D*  m_ptCharged;           // Pt  charged stable particles 

  int            m_counter       , m_counterstable;
  int            m_counterCharged, m_counterChInEta;
  int            m_nEvents;

  double lifetime ( HepMC::GenParticle * ) ;
};
#endif // GAUSSMONITOR_GENMONITORALG_H
