// $Id: TsaSeedReferenceCreator.h,v 1.2 2006-08-17 08:36:09 mneedham Exp $
#ifndef _TsaSeedReferenceCreator_H
#define _TsaSeedReferenceCreator_H

/** @class TsaSeedReferenceCreator TrackTools/TsaSeedReferenceCreator.h
 *
 * Implementation of TrackCaloMatch tool
 * see interface header for description
 *
 *  @author M.Needham
 *  @date   30/12/2005
 */

#include "GaudiAlg/GaudiTool.h"

#include "TrackInterfaces/ITrackManipulator.h"
#include "GaudiKernel/IIncidentListener.h"

#include <string>

namespace LHCb{
  class Track;
  class Measurement;
}

class ITrajPoca;

#include "SeedTrack.h"

class TsaSeedReferenceCreator: public GaudiTool,
                               virtual public ITrackManipulator, 
                               virtual public IIncidentListener  {

public:

  /** constructer */
  TsaSeedReferenceCreator(const std::string& type,
                            const std::string& name,
                            const IInterface* parent);  

  /** destructer */
  virtual ~TsaSeedReferenceCreator();

  /** intialize */
  StatusCode initialize();

  /** add reference info to the track */  
  StatusCode execute(const LHCb::Track& aTrack) const;

  /** Implement the handle method for the Incident service.
  *  This is used to inform the tool of software incidents.
  *
  *  @param incident The incident identifier
  */
  void handle( const Incident& incident );

private:

  void initEvent() const;

  void addReference(LHCb::Measurement* meas, const SeedTrack* seedTrack,
                    const double qOverP, const int ambiguity) const;

  ITrajPoca*         m_poca;          ///< Pointer to the ITrajPoca interface

  // Job options
  bool m_setLRAmbiguities;            ///< Flag to set the OT LR ambiguities


  mutable SeedTracks* m_seeds;  
  mutable bool m_configured;

};

#endif
