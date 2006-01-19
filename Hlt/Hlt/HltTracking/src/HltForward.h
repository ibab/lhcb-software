// $Id: HltForward.h,v 1.1.1.1 2006-01-19 10:11:25 hernando Exp $
#ifndef HltFORWARD_H 
#define HltFORWARD_H 1

// Include files
// from Gaudi
#include "PatForward/PatForward.h"

/** @class HltForwardConditional HltForwardConditional.h
 *  Forward pattern recognition. Connect a Velo track to the T stations.
 *
 *  @author Jose A. Hernando
 *  @date   2005-04-01

 */
class HltForward : public PatForward {
public: 
  /// Standard constructor
  HltForward( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltForward( ); ///< Destructor

  virtual bool acceptTrack(const Track& track) const;
  
  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

  std::string m_mode;

protected:

  bool m_ok;
  
  std::vector<Track::Flags> m_accept;
  std::vector<Track::Flags> m_noAccept;
};
#endif // HltFORWARD_H
