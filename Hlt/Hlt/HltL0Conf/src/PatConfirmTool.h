// $Id: PatConfirmTool.h,v 1.1 2007-11-16 11:44:02 albrecht Exp $
#ifndef PATCONFIRMTOOL_H 
#define PATCONFIRMTOOL_H 1

// Include files
#include <string>
#include <vector>

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "HltBase/ITrackConfirmTool.h"            // Interface
#include "TrackInterfaces/IPatSeedingTool.h"

#include "TfKernel/TStationHitManager.h"
#include "PatKernel/PatForwardHit.h"


#include "L0ConfDataStore.h"

namespace LHCb{
  class Track;
}
/** @class PatConfirmTool PatConfirmTool.h
 *  
 * 
 *  @author Johannes Albrecht
 *  @date   2007-10-25
 *
 *  - Tool which gets a state as seed (assumed to be at T3)
 *  - and decodes Ot/IT hits in a region defined by a parabola 
 *    parameterized from the seed state
 *  - Calls PatSeedingTool to search for tracks in this region
 *
 */
class PatConfirmTool : public GaudiTool, virtual public ITrackConfirmTool {
public: 
  /// Standard constructor
  PatConfirmTool( const std::string& type, 
                  const std::string& name,
                  const IInterface* parent);
  
  virtual ~PatConfirmTool( ); ///< Destructor

  // Initialize method
  virtual StatusCode initialize();

  StatusCode tracks(const LHCb::State& seedState, std::vector<LHCb::Track*>& outputTracks );
  
protected:
  
private:
  //variables declared in job options
  int m_nsigma;
  bool m_debugMode;

  Tf::TStationHitManager <PatForwardHit> *  m_tHitManager;
  
  IPatSeedingTool*  m_patSeedingTool;
  
  //debug information
  L0ConfDataStore* m_DataStore;


};
#endif // PATCONFIRMTOOL_H
