// $Id: TrackCloner.h,v 1.1 2008-04-01 12:34:34 jpalac Exp $
#ifndef MICRODST_TRACKPARTICLECLONER_H 
#define MICRODST_TRACKCLONER_H 1

// Include files
// from Gaudi
#include <MicroDST/MicroDSTTool.h>
#include <MicroDST/ICloneTrack.h>            // Interface
#include <MicroDST/Functors.hpp>

/** @class TrackCloner TrackCloner.h src/TrackCloner.h
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2008-04-01
 */
class TrackCloner : public MicroDSTTool, 
                    virtual public ICloneTrack {
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

  LHCb::Track* clone(const LHCb::Track* protoParticle);

private:

  typedef MicroDST::BasicItemCloner<LHCb::Track> BasicTrackCloner;

};
#endif // MICRODST_TRACKCLONER_H
