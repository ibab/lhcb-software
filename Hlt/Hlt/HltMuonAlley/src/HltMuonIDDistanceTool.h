// $Id: HltMuonIDDistanceTool.h,v 1.3 2008-05-22 13:39:55 graven Exp $
#ifndef HLTMUONIDDISTANCETOOL_H 
#define HLTMUONIDDISTANCETOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "MuonDet/IMuonPosTool.h"
#include "Event/Track.h"
#include "TrackInterfaces/IFunctionTool.h"            // Interface

/** @class HltMuonIDDistanceTool HltMuonIDDistanceTool.h
 *  
 *
 *  @author Jose Angel Hernando Morata
 *  @date   2007-12-06
 */
class HltMuonIDDistanceTool : public GaudiTool, 
                              virtual public ITrackFunctionTool {
public: 
  /// Standard constructor
  HltMuonIDDistanceTool( const std::string& type, 
                         const std::string& name,
                         const IInterface* parent);

  virtual ~HltMuonIDDistanceTool( ); ///< Destructor

  StatusCode initialize();
  
  double function(const LHCb::Track& track);

private:

  IMuonPosTool*      m_iPosTool;
  double m_maxdist;



};
#endif // HLTMUONIDDISTANCETOOL_H
