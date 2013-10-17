// $Id: $
#ifndef TRACKTYPEFILTERVTXTOPOTRACKSTOOL_H
#define TRACKTYPEFILTERVTXTOPOTRACKSTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IDistanceCalculator.h"
#include "IncTopoVert/IFilterVtxTopoTracksTool.h"            // Interface
#include "GaudiAlg/GaudiHistoTool.h"
//#include "GaudiKernel/NTuple.h"
#include "GaudiAlg/Tuple.h"
#include "GaudiAlg/TupleObj.h"
#include "Event/Particle.h"

using namespace std;

/** @class TrackTypeFilterVtxTopoTracksTool TrackTypeFilterVtxTopoTracksTool.h
 *  This class implements IFilterVtxTopoTracksTool. The selection is performed
 *  on the basis of the track type (Long, Upstream...)
 *
 *  @author Julien Cogan and Mathieu Perrin-Terrin
 *  @date   2012-11-27
 */
class TrackTypeFilterVtxTopoTracksTool : public GaudiHistoTool, virtual public IFilterVtxTopoTracksTool 
{

public:

  /// Standard constructor
  TrackTypeFilterVtxTopoTracksTool( const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent);

public:

  std::vector<const LHCb::Track*> & filteredTracks(Tuples::Tuple* tuple = NULL);
  std::vector<const LHCb::Track*> & filteredTracks(std::vector<const LHCb::Track*> input_tracks, Tuples::Tuple* tuple = NULL);
  void SetParam(double){
  };

  virtual ~TrackTypeFilterVtxTopoTracksTool( ); ///< Destructor

private:


  //track type configurable and enum
  vector<string>                   m_TrackTypeAllowed;
  map<string, LHCb::Track::Types>  m_TrackTypeMap;
  map<LHCb::Track::Types, string>  m_TrackTypeMapRev;


  //container for tracks
  vector<const LHCb::Track*>  m_tracks;

};

#endif // TRACKTYPEFILTERVTXTOPOTRACKSTOOL_H
