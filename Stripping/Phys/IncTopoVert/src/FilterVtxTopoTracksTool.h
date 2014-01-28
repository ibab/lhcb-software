// $Id: $
#ifndef FILTERVTXTOPOTRACKSTOOL_H 
#define FILTERVTXTOPOTRACKSTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "IncTopoVert/IFilterVtxTopoTracksTool.h"            // Interface
#include "GaudiAlg/Tuple.h"
#include "GaudiAlg/TupleObj.h"

/** @class FilterVtxTopoTracksTool FilterVtxTopoTracksTool.h
 *  This class is the basic implementation of IFilterVtxTopoTracksTool.
 *
 *  @author Julien Cogan and Mathieu Perrin-Terrin
 *  @date   2012-10-24
 */
class FilterVtxTopoTracksTool : public GaudiTool, virtual public IFilterVtxTopoTracksTool {
public: 
  /// Standard constructor
  FilterVtxTopoTracksTool( const std::string& type, 
                           const std::string& name,
                           const IInterface* parent);

  std::vector<const LHCb::Track*> & filteredTracks(Tuples::Tuple* tuple = NULL);
  std::vector<const LHCb::Track*> & filteredTracks(std::vector<const LHCb::Track*>,Tuples::Tuple* tuple = NULL){
    return filteredTracks(tuple);
    };
  void SetParam(double){
  };



  virtual ~FilterVtxTopoTracksTool( ); ///< Destructor

protected:

private:

  std::vector<const LHCb::Track*> m_tracks;

};
#endif // FILTERVTXTOPOTRACKSTOOL_H
