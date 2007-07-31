// $Id: HltMatchOffTrack2OnCalo.h,v 1.1 2007-07-31 15:59:35 gguerrer Exp $
#ifndef HLTMATCHOFFTRACK2ONCALO_H 
#define HLTMATCHOFFTRACK2ONCALO_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "HltBase/ITrackMatch.h"            // Interface
#include "HltBase/HltUtils.h"
#include "CaloInterfaces/IPart2Calo.h"

/** @class HltMatchOffTrack2OnCalo HltMatchOffTrack2OnCalo.h
 *  
 *
 *  @author Gabriel Guerrer
 *  @date   2007-07-30
 */
class HltMatchOffTrack2OnCalo : public GaudiTool, virtual public ITrackMatch {
public: 
  /// Standard constructor
  HltMatchOffTrack2OnCalo( const std::string& type, 
                           const std::string& name,
                           const IInterface* parent);

  virtual ~HltMatchOffTrack2OnCalo( ); ///< Destructor
  
  StatusCode initialize();
  
  StatusCode match(const LHCb::Track& track0, //calo tracks
                   const LHCb::Track& track1, 
                   double& quality);

protected:

private:

  ITrack2Calo* m_tooltrack2calo;

};
#endif // HLTMATCHOFFTRACK2ONCALO_H
