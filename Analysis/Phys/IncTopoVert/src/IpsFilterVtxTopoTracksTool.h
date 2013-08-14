// $Id: $
#ifndef IPSTRACKTYPEFILTERVTXTOPOTRACKSTOOL_H 
#define IPSTRACKTYPEFILTERVTXTOPOTRACKSTOOL_H 1

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

/** @class IpsFilterVtxTopoTracksTool IpsFilterVtxTopoTracksTool.h
 *  This class implements IFilterVtxTopoTracksTool. The selection is performed 
 *  on the basis of the impact parameter significance of the track with respect
 *  to all primary vertices in the event.
 *
 *  @author Julien Cogan and Mathieu Perrin-Terrin
 *  @date   2012-11-27
 */
class IpsFilterVtxTopoTracksTool : public GaudiHistoTool, virtual public IFilterVtxTopoTracksTool {
public: 
  /// Standard constructor
  IpsFilterVtxTopoTracksTool( const std::string& type, 
                                       const std::string& name,
                                       const IInterface* parent);
  StatusCode initialize();

  std::vector<const LHCb::Track*> & filteredTracks(Tuples::Tuple* tuple = NULL);
  std::vector<const LHCb::Track*> & filteredTracks(std::vector<const LHCb::Track*> input_tracks, Tuples::Tuple* tuple = NULL);
  void SetParam(double p){
    m_ips = p;
  };

  virtual ~IpsFilterVtxTopoTracksTool( ); ///< Destructor
    
protected:

private:

  IDistanceCalculator*  m_Geom;
  std::string m_distanceCalculatorToolType; ///< Type of the distance calculator Tool


  //ips cut
  double                           m_ips;

  
  //container for tracks
  vector<const LHCb::Track*>  m_tracks;

};
#endif // IPSTRACKTYPEFILTERVTXTOPOTRACKSTOOL_H
