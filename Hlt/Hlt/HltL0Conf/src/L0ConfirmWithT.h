// $Id: L0ConfirmWithT.h,v 1.5 2008-07-09 08:28:51 hernando Exp $
#ifndef L0CONFIRMWITHT_H 
#define L0CONFIRMWITHT_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "TrackInterfaces/ITracksFromTrack.h"            // Interface

#include "L0ConfDataStore.h"
#include "HltBase/IL0ConfExtrapolator.h"

#include "HltBase/ITrackView.h"

class ITrackConfirmTool;

/** @class L0ConfirmWithT L0ConfirmWithT.h
 *  
 *   Tool to create tracks for L0 confirmation. 
 *   Input: seed track prepared at its subdetector
 *   Output: vector of tracks reconstructed in T-stations
 *
 *
 *  @author Johannes Albrecht
 *  @date   2007-07-04
 */
class L0ConfirmWithT : public GaudiTool, 
                       virtual public ITracksFromTrack,
                       virtual public ITrackView 
{
public: 
  /// Standard constructor
  L0ConfirmWithT( const std::string& type, 
                  const std::string& name,
                  const IInterface* parent);

  virtual ~L0ConfirmWithT( ); ///< Destructor

  /// Initialize method
  virtual StatusCode initialize();

  virtual StatusCode tracksFromTrack( const LHCb::Track& seed,
                                      std::vector<LHCb::Track*>& tracks );

  virtual std::vector<Tf::IStationSelector*>  view(const LHCb::Track& seed);
  virtual std::vector<LHCb::LHCbID> lhcbIDsInView(const LHCb::Track& seed);

  
protected:

private:

  StatusCode prepareStates( const LHCb::Track& seed, LHCb::State* seedStates, int& nStates );
  

  bool m_debugMode;
  
  IL0ConfExtrapolator* m_l0ConfExtrapolator;
  
  ITrackConfirmTool* m_TrackConfirmTool;
  std::string m_trackingTool;

  //debug information
  L0ConfDataStore* m_DataStore;
  // select particle type:
  //	0 = muon
  //	1 = hadron
  //	2 = electron
  typedef enum { Muon = 0, Hadron, Electron, Last } ParticleType;
  ParticleType m_particleType;
  // temporary particle type - verified to be valid in initialize
  int m_particleTypeTMP;

};
#endif // L0CONFIRMWITHT_H
