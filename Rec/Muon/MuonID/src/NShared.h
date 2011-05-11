// $Id: $
#ifndef NSHARED_H 
#define NSHARED_H 1

// Include files

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "INShared.h"            // Interface


#include "Event/Track.h"
#include "Event/MuonPID.h"
#include "Event/MuonCoord.h"

/** @class NShared NShared.h
 *  
 *
 *  @author Xabier Cid Vidal
 *  @date   2011-03-17
 */
class NShared : public GaudiTool, virtual public INShared {
public: 
  /// Standard constructor
  NShared( const std::string& type, 
           const std::string& name,
           const IInterface* parent);

  virtual ~NShared( ); ///< Destructor

  StatusCode getNShared(const LHCb::Track& muTrack, int& nshared);
  void fillNShared();

protected:

  // add info for ttracks?
  bool m_useTtrack;

  std::string m_TracksPath;
  
  /// TES path to output the MuonPIDs to
  std::string m_MuonPIDsPath;

  /// TES path to output the Track PIDs to
  std::string m_MuonTracksPath;

  bool compareHits(const LHCb::Track& muTrack1,const LHCb::Track& muTrack2);
  LHCb::MuonPID* mupidFromTrack(const LHCb::Track& mother);

  //from ChargedProtoParticleAddMuonInfo.h
  StatusCode mapMuonPIDs();

  typedef std::map<const LHCb::Track *, LHCb::MuonPID *> TrackToMuonPID;
  TrackToMuonPID m_muonMap;

private:

};
#endif // NSHARED_H
