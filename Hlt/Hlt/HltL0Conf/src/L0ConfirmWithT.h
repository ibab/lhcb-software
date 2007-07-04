// $Id: L0ConfirmWithT.h,v 1.1 2007-07-04 12:33:22 albrecht Exp $
#ifndef L0CONFIRMWITHT_H 
#define L0CONFIRMWITHT_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "TrackInterfaces/ITracksFromTrack.h"            // Interface

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
class L0ConfirmWithT : public GaudiTool, virtual public ITracksFromTrack {
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
  
protected:

private:

  StatusCode extrapolateToT3( LHCb::State& statePos, LHCb::State& stateNeg );
  
  ITrackConfirmTool* m_TsaConfirmTool;

};
#endif // L0CONFIRMWITHT_H
