// $Id: TrackProjectorSelector.h,v 1.1 2006-12-15 19:11:38 graven Exp $
#ifndef TRACKPROJECTORS_TRACKPROJECTORSELECTOR_H 
#define TRACKPROJECTORS_TRACKPROJECTORSELECTOR_H 1

// Include files
#include <map>

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/VectorMap.h"

// from TrackInterfaces
#include "TrackInterfaces/ITrackProjectorSelector.h"


/** @class TrackProjectorSelector TrackProjectorSelector.h TrackProjectors/TrackProjectorSelector.h
 *
 *  TrackProjectorSelector decides which projection to use for
 *  a given (type of) measurement
 *
 *  @author Gerhard Raven
 *  @date   2006-06-22
 */
class TrackProjectorSelector : public GaudiTool,
                               virtual public ITrackProjectorSelector {

public:

  /// Standard constructor
  TrackProjectorSelector( const std::string& type, 
                          const std::string& name,
                          const IInterface* parent );

  /// Destructor
  virtual ~TrackProjectorSelector( );

  StatusCode initialize();

  ITrackProjector* projector(const LHCb::Measurement&) const;

private:

  typedef std::map<LHCb::Measurement::Type,std::string> ProjectorNames;
  ProjectorNames m_projNames;

  typedef GaudiUtils::VectorMap<LHCb::Measurement::Type,ITrackProjector*> Projectors;
  Projectors m_projectors;

};
#endif // TRACKPROJECTORS_TRACKPROJECTORSELECTOR_H
