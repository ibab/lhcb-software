#ifndef CHEATEDPRIMARYVERTICES_H 
#define CHEATEDPRIMARYVERTICES_H 1
// Include files:
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
// From PhysEvent
#include "Event/Particle.h"
#include "Event/ProtoParticle.h"
#include "Event/MCParticle.h"
#include "MCInterfaces/IForcedBDecayTool.h"
#include "GaudiKernel/ParticleProperty.h"
#include "GaudiKernel/IParticlePropertySvc.h"

/** @class PVEff CheatedPrimaryVertices.h
 *
 *
 *  @author Marcin Kucharczyk
 *  @date   2008-12-07
 */

class CheatedPrimaryVertices : public GaudiAlgorithm {
public:
  // Standard constructor
  CheatedPrimaryVertices(const std::string& name, ISvcLocator* pSvcLocator);
  // Destructor
  ~CheatedPrimaryVertices();
  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();
private:
  LHCb::Tracks* m_inputTracks;
  LHCb::RecVertices* m_outputVertices;
  std::string m_inputTracksName;
  std::string m_outputVerticesName;
  int fromMCVertex(const LHCb::MCParticle* mcParticle,
                   const LHCb::MCVertex* mcVertex);
  double chi2Tr(const LHCb::Track* pvTr, const LHCb::RecVertex* invt);
  

};
#endif // CHEATEDPRIMARYVERTICES_H
