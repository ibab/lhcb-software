// $Id: TrackCloner.h,v 1.5 2010-08-13 13:22:34 jpalac Exp $
#ifndef MICRODST_TRACKCLONER_H
#define MICRODST_TRACKCLONER_H 1

#include "ObjectClonerBase.h"

#include <MicroDST/ICloneTrack.h>

/** @class TrackCloner TrackCloner.h src/TrackCloner.h
 *
 *  Clone an LHCb::Track. At the moment this does nothing other than
 *  performing a simple clone and storing the clone in the appropriate
 *  TES location. It is a plalce holder for future, more complex cloning.
 *
 *  @author Juan PALACIOS
 *  @date   2008-04-01
 */
class TrackCloner : public extends1<ObjectClonerBase,ICloneTrack>
{

public:

  /// Standard constructor
  TrackCloner( const std::string& type,
               const std::string& name,
               const IInterface* parent);

  virtual ~TrackCloner( ); ///< Destructor

  virtual LHCb::Track* operator() (const LHCb::Track* track);

private:

  LHCb::Track* clone(const LHCb::Track* track);

private:

  typedef MicroDST::BasicItemCloner<LHCb::Track> BasicTrackCloner;

private:

  bool m_cloneAncestors;

};

#endif // MICRODST_TRACKCLONER_H
