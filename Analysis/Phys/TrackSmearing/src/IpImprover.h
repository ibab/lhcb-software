#ifndef IPIMPROVER_H 
#define IPIMPROVER_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "Kernel/DaVinciAlgorithm.h"
#include <string>
#include <vector>
#include <map>
#include "Event/Track.h"
#include "Event/MCParticle.h"
#include <TF1.h>

//class ITrackManipulator;
//class LHCb::Track;
//class LHCb::MCParticle;


/** @class IpImprover IpImprover.h
 *  
 *
 *  @author Sascha Stahl
 *  @date   2012-11-26
 */
class IpImprover : public DaVinciAlgorithm 
{

public: 

  /// Standard constructor
  IpImprover( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~IpImprover( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private:

  std::vector<unsigned int> m_types;
  std::vector<std::string> m_trackLocations;
  double m_improvement;
  double m_pImprovement;
  
  TF1* m_funcx_upgrade;
  TF1* m_funcx_mc;
  TF1* m_funcy_upgrade;
  TF1* m_funcy_mc;

  double m_xpar1_mc;
  double m_xpar2_mc;
  double m_xpar1_upgrade;
  double m_xpar2_upgrade;
  double m_ypar1_mc;
  double m_ypar2_mc;
  double m_ypar1_upgrade;
  double m_ypar2_upgrade;

private:
  
  StatusCode improveTrack(LHCb::Track& aTrack, const LHCb::MCParticle* mcp);
  
  double getImprovement(double pt, bool isX);

};

#endif // IPIMPROVER_H
