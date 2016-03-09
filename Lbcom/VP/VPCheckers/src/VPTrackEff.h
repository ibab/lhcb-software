#ifndef VPTRACKEFF_H
#define VPTRACKEFF_H 1

// Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"

// LHCb
#include "Event/MCParticle.h"
#include "VPDet/DeVP.h"

/** @class VPTrackEff
 *
 * Class for Velo track monitoring Ntuple
 *  @author T. Bird
 *  @date   2013-10-02
 */

class VPTrackEff : public GaudiTupleAlg {

public:

  /// Standard constructor
  VPTrackEff(const std::string& name, ISvcLocator* pSvcLocator);
  /// Destructor
  virtual ~VPTrackEff();

  /// Algorithm execution
  virtual StatusCode execute();
  /// Algorithm initialization
  virtual StatusCode initialize();

private:
  
  const LHCb::MCVertex* findMCOriginVertex(const LHCb::MCParticle& particle, 
                                           const double decaylengthtolerance);
  int mcMotherType(const LHCb::MCParticle& particle);
  int mcVertexType(const LHCb::MCParticle& particle);

  std::string m_trackLocation;    ///< Input track container location
  std::string m_mchitLocation;    ///< Input hit container location
 
  const DeVP* m_det;

};


#endif

