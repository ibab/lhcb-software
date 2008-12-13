// $Id: HltAntiEleConf.h,v 1.1 2008-12-13 13:01:58 witekma Exp $
#ifndef HLTANTIELECONF_H 
#define HLTANTIELECONF_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Event/Track.h"
#include "TrackInterfaces/ITracksFromTrack.h"
#include "TrackInterfaces/IFunctionTool.h"            // Interface
#include <memory>

/** @class HltAntiEleConf HltAntiEleConf.h
 *  
 *
 *  @author Mariusz Witek
 *  @date   2008-07-05
 */
class HltAntiEleConf : public GaudiTool, 
                        virtual public ITrackFunctionTool {
public: 
  /// Standard constructor
  HltAntiEleConf( const std::string& type, 
                  const std::string& name,
                  const IInterface* parent);

  virtual ~HltAntiEleConf( ); ///< Destructor

  StatusCode initialize();
  StatusCode finalize();
  
  double function(const LHCb::Track& track);

private:
  HltAntiEleConf( const HltAntiEleConf& ) ;            // do not allow!
  HltAntiEleConf& operator=( const HltAntiEleConf& ) ; // do not allow!

  ITracksFromTrack* m_tool;  

};
#endif // HLTANTIELECONF_H
