// $Id: MCTruthMonitor.h,v 1.2 2009-03-26 21:45:04 robbep Exp $
#ifndef MCTRUTHMONITOR_H 
#define MCTRUTHMONITOR_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"

// from AIDA
#include "AIDA/IHistogram1D.h"

/** @class MCTruthMonitor MCTruthMonitor.h
 * 
 *  Algorithm to fill reference historgam for MCParticles and MCVertices.
 *  A more detailed set is switched on with property "DetailedHistos = true".
 *
 *  @author Gloria CORTI
 *  @date   2005-08-12
 */
class MCTruthMonitor : public GaudiHistoAlg {
public: 
  /// Standard constructor
  MCTruthMonitor( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~MCTruthMonitor( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:
  bool detailedHistos() { 
    return m_detailedHistos; 
  }

private:

  bool m_detailedHistos;             ///< Property to control set of histos
  std::string m_mcParticles ;        ///< Location of the MCParticles
  std::string m_mcVertices ;         ///< Location of the MCVertices
  AIDA::IHistogram1D* m_hNPart;      ///< Histo of MCParticle multiplicity
  AIDA::IHistogram1D* m_hNVert;      ///< Histo of MCVertices multiplicity  
  AIDA::IHistogram1D* m_hPOrigZ;     ///< Z Origin of all particles 
  AIDA::IHistogram1D* m_hPOrigT;     ///< Origin vertex type for all particles
  AIDA::IHistogram1D* m_hPMom;       ///< |P| of all particles
  AIDA::IHistogram1D* m_hPPrimMom;   ///< |P| of particles from primary vertex
  AIDA::IHistogram1D* m_hPProtMom;   ///< |P| of protons
  AIDA::IHistogram1D* m_hNProtons;   ///< p/p_bar multiplicity
  AIDA::IHistogram1D* m_hNNeutrons;  ///< n/n_bar multiplicity
  AIDA::IHistogram1D* m_hNChPions;   ///< pi+/- multiplicity
  AIDA::IHistogram1D* m_hNPiZeros;   ///< pi0 multiplicity
  AIDA::IHistogram1D* m_hNChKaons;   ///< K+/- multiplicity
  AIDA::IHistogram1D* m_hNKs;        ///< Kshort multiplicity
  AIDA::IHistogram1D* m_hNElectrons; ///< e+/- multiplicity
  AIDA::IHistogram1D* m_hNMuons;     ///< mu+/- multiplicity
  AIDA::IHistogram1D* m_hNGammas;    ///< gammas multiplicity 
  AIDA::IHistogram1D* m_hNBeauty;    ///< Particles with b/b_bar quark multip.
  AIDA::IHistogram1D* m_hNCharm;     ///< Particles with c/c_bar quark multip.
  AIDA::IHistogram1D* m_hNNuclei;    ///< nuclei multiplicity 
  AIDA::IHistogram1D* m_hVType;      ///< Type of all vertices
  AIDA::IHistogram1D* m_hVZpos;      ///< Zpos of all vertices (in detector) 
  AIDA::IHistogram1D* m_hVZpos2;     ///< Zpos of all vertices (symm. in IP)
  AIDA::IHistogram1D* m_hVTime;      ///< Time of all vertices

};
#endif // MCTRUTHMONITOR_H
