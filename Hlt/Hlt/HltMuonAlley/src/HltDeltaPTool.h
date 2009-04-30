// $Id: HltDeltaPTool.h,v 1.3 2009-04-30 09:25:42 asatta Exp $
#ifndef HLTDELTAPTOOL_H 
#define HLTDELTAPTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
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
// DeltaP parameters
  std::vector<double> m_pCut;

};
#endif // HLTDELTAPTOOL_H
