// $Id: TrackCloner.h,v 1.4 2009-07-30 10:03:01 jpalac Exp $
#ifndef MICRODST_TRACKPARTICLECLONER_H 
#define MICRODST_TRACKCLONER_H 1

// Include files
// from Gaudi
#include <MicroDST/MicroDSTTool.h>
#include <MicroDST/ICloneTrack.h>            // Interface
#include <MicroDST/Functors.hpp>

/** @class TrackCloner TrackCloner.h src/TrackCloner.h
 *  
 *  Clone an LHCb::Track. At the moment this does nothing other than 
 *  performing a simple clone and storing the clone in the appropriate
 *  TES location. It is a plalce holder for future, more complex cloning.
 *  
 *  @author Juan PALACIOS
 *  @date   2008-04-01
 */
class TrackCloner : public extends1<MicroDSTTool, ICloneTrack> {
public: 
  /// Standard constructor
  TrackCloner( const std::string& type, 
               const std::string& name,
               const IInterface* parent);

  virtual ~TrackCloner( ); ///< Destructor

  StatusCode initialize();

  virtual LHCb::Track* operator() (const LHCb::Track* track);

protected:

private:

  LHCb::Track* clone(const LHCb::Track* track);

private:

  typedef MicroDST::BasicItemCloner<LHCb::Track> BasicTrackCloner;

};
#endif // MICRODST_TRACKCLONER_H
