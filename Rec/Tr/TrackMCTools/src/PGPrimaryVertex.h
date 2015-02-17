#ifndef PGPRIMARYVERTEX_H
#define PGPRIMARYVERTEX_H 1
// Include files:
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/RndmGenerators.h"
// From MCEvent
#include "Event/MCParticle.h"
#include "Event/MCVertex.h"
// From ROOT
#include "TFile.h"
#include "TH3.h"

//#include "MCInterfaces/IForcedBDecayTool.h"

/** @class PVEff PGPrimaryVertex.h
 *
 *
 *  @author Marcin Kucharczyk
 *  @date   2008-12-07
 */

class PGPrimaryVertex : public GaudiAlgorithm {
 public:
  // Standard constructor
  PGPrimaryVertex(const std::string& name, ISvcLocator* pSvcLocator);
  // Destructor
  ~PGPrimaryVertex();
  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

  int fromMCVertex(const LHCb::MCParticle* mcParticle,const LHCb::MCVertex* mcVertex);

 private:
  LHCb::RecVertices* m_outputVertices;
  std::string m_outputVerticesName;
  Rndm::Numbers m_gaus;
  Rndm::Numbers m_flat;
  TFile *m_file;
  TH3* m_hist;
  bool m_usePVResFile;
  std::string m_inputFileName;
  std::string m_histoPath;
};
#endif // PGPRIMARYVERTEX_H
