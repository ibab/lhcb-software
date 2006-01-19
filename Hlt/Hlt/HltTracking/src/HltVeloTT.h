// $Id: HltVeloTT.h,v 1.1.1.1 2006-01-19 10:11:25 hernando Exp $
#ifndef HltVELOTT_H 
#define HltVELOTT_H 1

// Include files
// from Gaudi
#include "PatVeloTT/PatVeloTT.h"

/** @class HltForwardConditional HltForwardConditional.h
 *  Forward pattern recognition. Connect a Velo track to the T stations.
 *
 *  @author Jose A. Hernando
 *  @date   2005-04-01

 */
class HltVeloTT : public PatVeloTT {
public: 
  /// Standard constructor
  HltVeloTT( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltVeloTT( ); ///< Destructor

  virtual bool acceptTrack(const Track& track) const;
  
  virtual StatusCode initialize();    
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

  std::string m_mode;

protected:

  bool m_ok;
  
  std::vector<Track::Flags> m_accept;
  std::vector<Track::Flags> m_noAccept;
};
#endif // HltVeloTT_H
