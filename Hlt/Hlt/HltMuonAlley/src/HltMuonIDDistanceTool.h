// $Id: HltMuonIDDistanceTool.h,v 1.2 2008-01-22 10:00:48 hernando Exp $
#ifndef HLTMUONIDDISTANCETOOL_H 
#define HLTMUONIDDISTANCETOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "MuonDet/IMuonPosTool.h"
#include "Event/Track.h"
#include "HltBase/IFunctionTool.h"            // Interface

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

protected:

  IMuonPosTool*      m_iPosTool;
  float m_maxdist;


private:

};
#endif // HLTMUONIDDISTANCETOOL_H
