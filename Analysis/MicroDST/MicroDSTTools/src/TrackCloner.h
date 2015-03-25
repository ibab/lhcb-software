// $Id: TrackCloner.h,v 1.5 2010-08-13 13:22:34 jpalac Exp $
#ifndef MICRODST_TRACKCLONER_H
#define MICRODST_TRACKCLONER_H 1

// base class
#include "ObjectClonerBase.h"

// From MicroDST
#include "MicroDST/ICloneTrack.h"
#include "MicroDST/ICloneMCParticle.h"

// linker stuff
#include "Linker/LinkerTool.h"
#include "Linker/LinkerWithKey.h"

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"

// from LHCb
#include "Event/Track.h"
#include "Event/MCParticle.h"

/** @class TrackCloner TrackCloner.h src/TrackCloner.h
 *
 *  Clone an LHCb::Track. At the moment this does nothing other than
 *  performing a simple clone and storing the clone in the appropriate
 *  TES location. It is a plalce holder for future, more complex cloning.
 *
 *  @author Juan PALACIOS
 *  @date   2008-04-01
 */
class TrackCloner : public extends1<ObjectClonerBase,ICloneTrack>,
                    virtual public IIncidentListener
{

public:

  /// Standard constructor
  TrackCloner( const std::string& type,
               const std::string& name,
               const IInterface* parent);

  virtual ~TrackCloner( ); ///< Destructor

  virtual StatusCode initialize();

  /** Implement the handle method for the Incident service.
   *  This is used to inform the tool of software incidents.
   *
   *  @param incident The incident identifier
   */
  void handle( const Incident& incident );

public:

  virtual LHCb::Track* operator() (const LHCb::Track* track);

private:

  typedef MicroDST::BasicItemCloner<LHCb::Track> BasicTrackCloner;
  typedef std::vector<const LHCb::Track*> TrackList;

private:

  /// Clone a track
  LHCb::Track * clone( const LHCb::Track* track );

  /// Clone MC Links for the given track and its clone
  void cloneMCLinks( const LHCb::Track* track,
                     const LHCb::Track* cloneTrack );

  /// Static list of cloned tracks
  TrackList & clonedTrackList() { static TrackList list; return list; }

private:

  /// Type of MCParticle cloner
  std::string m_mcpClonerName;
  /// MCParticle Cloner
  ICloneMCParticle * m_mcPcloner;

  bool m_cloneAncestors; ///< Flag to turn on cloning of ancestors
  bool m_cloneMCLinks;   ///< Flag to turn on cloning of links to MCParticles

};

#endif // MICRODST_TRACKCLONER_H
