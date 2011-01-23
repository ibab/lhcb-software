#ifndef _ICountVeloTracks_H
#define _ICountVeloTracks_H
 
#include "GaudiKernel/IAlgTool.h"
#include "Event/Track.h"

static const InterfaceID IID_ICountVeloTracks( "ICountVeloTracks", 0, 0 );
 
/** @class ICountVeloTracks ICountVeloTracks.h
 *
 *  Interface for counting the distinct VELO tracks in an event
 *
 *  @author David Hutchcroft
 *  @date   21/1/2011
 */

class ICountVeloTracks: virtual public IAlgTool{
 
public:
        
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_ICountVeloTracks ; }
                                                                               
  /** Returns number of distinct VELO tracks contributing to the container
  *
  *  @param tracks reference to LHCb::Tracks container
  *
  *  @return number of distinct VELO tracks
  */
  virtual unsigned int nVeloTracks ( const LHCb::Tracks& tracks ) const = 0;
                                                                           
  /** Returns number of distinct VELO tracks contributing to the container
  *
  *  @param tracksLocation TES location of the Tracks container
  *
  *  @return number of distinct VELO tracks
  */
  virtual unsigned int nVeloTracks ( const std::string& tracksLocation ) const = 0;
                                                                           
                                                                       
};
                                                                               
#endif //_ICountVeloTracks_H
