// $Id: HltDeltaPTool.h,v 1.1 2008-10-01 09:32:53 leandrop Exp $
#ifndef HLTDELTAPTOOL_H 
#define HLTDELTAPTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "MuonDet/IMuonPosTool.h"
#include "Event/Track.h"
#include "MuonDAQ/IMuonRawBuffer.h"
#include "TrackInterfaces/IFunctionTool.h"            // Interface
#include "Event/L0MuonCandidate.h"

/** @class HltDeltaPTool HltDeltaPTool.h
 *  
 *
 *  @author Leandro de Paula
 *  @date   2008-09-25
 */
class HltDeltaPTool : public GaudiTool, 
                      virtual public ITrackFunctionTool {
public: 
  /// Standard constructor
  HltDeltaPTool( const std::string& type, 
                 const std::string& name,
                 const IInterface* parent);

  virtual ~HltDeltaPTool( ); ///< Destructor

  StatusCode initialize();
  
  double function(const LHCb::Track& track);

private:
  float  l0Momentum(LHCb::MuonTileID tileMX);
  float  mSMomentum(LHCb::MuonTileID tileMX);

  IMuonPosTool*      m_iPosTool;
  IMuonRawBuffer*    m_muonBuffer;

private:

// DeltaP parameters
  std::vector<double> m_pCut;

  LHCb::L0MuonCandidates*     m_inputL0Muon;
  LHCb::Tracks*               m_inputMuSeg;

};
#endif // HLTDELTAPTOOL_H
