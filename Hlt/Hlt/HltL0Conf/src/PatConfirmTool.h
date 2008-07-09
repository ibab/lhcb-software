// $Id: PatConfirmTool.h,v 1.5 2008-07-09 08:28:51 hernando Exp $
#ifndef PATCONFIRMTOOL_H 
#define PATCONFIRMTOOL_H 1

// Include files
#include <string>
#include <vector>

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "HltBase/ITrackConfirmTool.h"            // Interface
#include "TrackInterfaces/IPatSeedingTool.h"

//#include "HltBase/ITrackView.h"

#include "TfKernel/TStationHitManager.h"
#include "PatKernel/PatForwardHit.h"

#include "HltBase/IL0ConfExtrapolator.h"
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
class PatConfirmTool : public GaudiTool, 
                       virtual public ITrackConfirmTool
                     //                     ,virtual public ITrackView 
{
public: 
  /// Standard constructor
  PatConfirmTool( const std::string& type, 
                  const std::string& name,
                  const IInterface* parent);
  
  virtual ~PatConfirmTool( ); ///< Destructor

  // Initialize method
  virtual StatusCode initialize();

  StatusCode tracks(const LHCb::State& seedState, std::vector<LHCb::Track*>& outputTracks);

  
  

  //  Tf::IStationSelector& view(const LHCb::Track& seed);
  //std::vector<LHCb::LHCbID> lhcbIDsInView(const LHCb::Track& seed);
  ParabolaHypothesis prepareT( const LHCb::State& seedState ,  std::vector<LHCb::LHCbID> ids);
protected:
  
private:
  ParabolaHypothesis prepareT( const LHCb::State& seedState );

  bool isClone(const LHCb::Track& t1 , const LHCb::Track& t2);
  
  //variables declared in job options
  double m_nSigmaX, m_nSigmaY, m_nSigmaTx, m_nSigmaTy ;

  bool m_debugMode;
  unsigned m_minHitsInOT;

  Tf::TStationHitManager <PatForwardHit> *  m_tHitManager;

  IPatSeedingTool*  m_patSeedingTool;
  IL0ConfExtrapolator* m_l0ConfExtrapolator;
 
  //debug information
  L0ConfDataStore* m_DataStore;
};
#endif // PATCONFIRMTOOL_H
