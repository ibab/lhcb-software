#ifndef PRCHEATEDVP_H 
#define PRCHEATEDVP_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "Event/VPCluster.h"
#include "PrFitPolinomial.h"
/** @class PrCheatedVP PrCheatedVP.h
 *  Cheated Pattern for the Velo
 *
 *  @author Olivier Callot
 *  @date   2012-07-26
 */
class PrCheatedVP : public GaudiAlgorithm {
public: 
  /// Standard constructor
  PrCheatedVP( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~PrCheatedVP( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:

  bool m_useMCHits;
  bool m_binaryClusters;
  Gaudi::XYZPoint getXYZ(LHCb::VPCluster* cluster);
  Gaudi::XYZPoint getBinaryXYZ(LHCb::VPCluster* cluster);
};

#endif // PRCHEATEDVP_H
