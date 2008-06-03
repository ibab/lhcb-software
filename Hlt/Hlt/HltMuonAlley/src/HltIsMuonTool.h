// $Id: HltIsMuonTool.h,v 1.6 2008-06-03 17:10:51 sandra Exp $
#ifndef HLTISMUONTOOL_H 
#define HLTISMUONTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "MuonDet/IMuonPosTool.h"
#include "Event/Track.h"
#include "MuonDAQ/IMuonRawBuffer.h"
#include "TrackInterfaces/IFunctionTool.h"            // Interface
#include "Event/L0MuonCandidate.h"

/** @class HltIsMuonTool HltIsMuonTool.h
 *  
 *
 *  @author Leandro de Paula
 *  @date   2008-01-25
 */
class HltIsMuonTool : public GaudiTool, 
                      virtual public ITrackFunctionTool {
public: 
  /// Standard constructor
  HltIsMuonTool( const std::string& type, 
                 const std::string& name,
                 const IInterface* parent);

  virtual ~HltIsMuonTool( ); ///< Destructor

  StatusCode initialize();
  
  double function(const LHCb::Track& track);

private:
  bool isInFOI(LHCb::Track* track, LHCb::MuonTileID tile);
  float  l0Momentum(LHCb::MuonTileID tileMX);

  IMuonPosTool*      m_iPosTool;
  IMuonRawBuffer*    m_muonBuffer;

private:

// FOI parameters
  std::vector<double> m_p1x;
  std::vector<double> m_p2x;
  std::vector<double> m_p3x;
  std::vector<double> m_p1y;
  std::vector<double> m_p2y;
  std::vector<double> m_p3y;
  LHCb::L0MuonCandidates* m_inputL0Muon;
  bool m_useFastDecoding; 

};
#endif // HLTISMUONTOOL_H
