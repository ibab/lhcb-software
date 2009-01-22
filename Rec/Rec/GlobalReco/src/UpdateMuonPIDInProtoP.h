// $Id: UpdateMuonPIDInProtoP.h,v 1.1 2009-01-22 14:19:05 jonrob Exp $
#ifndef UPDATEMUONPIDINPROTOP_H
#define UPDATEMUONPIDINPROTOP_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

#include "Event/MuonPID.h"
#include "Event/ProtoParticle.h"

/** @class UpdateMuonPIDInProtoP UpdateMuonPIDInProtoP.h
 *
 *  Updates the MuonPID information stored in the ProtoParticles
 *
 *  @author Tomasz Skwarnicki
 *  @date   2007-06-28
 */
class UpdateMuonPIDInProtoP : public GaudiAlgorithm 
{
public:

  /// Standard constructor
  UpdateMuonPIDInProtoP( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~UpdateMuonPIDInProtoP( ); ///< Destructor

  virtual StatusCode execute();    ///< Algorithm execution

private:

  bool getMuonData();

  bool replaceMuon( LHCb::ProtoParticle * proto ) const;

private:

  std::string m_muonPath;
  std::string m_protoPath;

  LHCb::ProtoParticles * m_protos;

  typedef std::map<const LHCb::Track *, const LHCb::MuonPID *> TrackToMuonPID;
  TrackToMuonPID m_muonMap;

};

#endif // UPDATEMUONPIDINPROTOP_H
