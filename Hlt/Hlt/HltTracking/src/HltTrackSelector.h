// $Id: HltTrackSelector.h,v 1.1 2006-01-27 21:50:24 hernando Exp $
#ifndef HLTTRACKSELECTOR_H 
#define HLTTRACKSELECTOR_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "TrackInterfaces/ITrackSelector.h"            // Interface


#include "Event/Track.h"

/** @class TrackSelector TrackSelector.h
 *  
 *
 *  @author Jose Angel Hernando Morata
 *  @date   2006-01-26
 */
class HltTrackSelector : public GaudiTool, virtual public ITrackSelector {
public: 
  /// Standard constructor
  HltTrackSelector( const std::string& type, 
                 const std::string& name,
                 const IInterface* parent);

  virtual ~HltTrackSelector( ); ///< Destructor

  virtual StatusCode initialize();

  virtual bool accept(const Track& track) const;

protected:

  std::string m_mode;
  
  std::vector<Track::Flags> m_accept;
  std::vector<Track::Flags> m_noAccept;

  bool m_ok;

private:

};
#endif // HLTTRACKSELECTOR_H
