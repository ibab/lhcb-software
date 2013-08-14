// $Id: $
#ifndef IFILTERVTXTOPOTRACKSTOOL_H 
#define IFILTERVTXTOPOTRACKSTOOL_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "Event/Track.h"
#include "GaudiAlg/Tuple.h"

 namespace Tuples{ 
   class Tuple; 
 } 

//static const InterfaceID IID_IFilterVtxTopoTracksTool ( "IFilterVtxTopoTracksTool", 1, 0 );

/** @class IFilterVtxTopoTracksTool IFilterVtxTopoTracksTool.h IFilterVtxTopoTracksTool.h
 *  This class is an interface for all the tracks filters. These filters select the tracks
 *  to be used to define a vertex function. A tuple can be pass as argument if some info
 *  on the tracks needs to be stored (mainly for optimisation/debug purpose).
 *
 *  @author Julien Cogan and Mathieu Perrin-Terrin
 *  @date   2012-10-24
 */
class GAUDI_API IFilterVtxTopoTracksTool : virtual public IAlgTool {
public: 

  // Return the interface ID
  //static const InterfaceID& interfaceID() { return IID_IFilterVtxTopoTracksTool; }
  DeclareInterfaceID(IFilterVtxTopoTracksTool, 2, 0);

  ///set the parameters of the filter  
  virtual void SetParam( double p ) = 0;

  ///return the list of tracks filtered out from a default location. A tuple can be pass as argument if some info on the tracks needs to be stored (mainly for optimisation/debug purpose).
  virtual std::vector<const LHCb::Track*> & filteredTracks(Tuples::Tuple* tuple = NULL)=0;

  ///return the list of tracks filtered out from an input list of trakcs. A tuple can be pass as argument if some info on the tracks needs to be stored (mainly for optimisation/debug purpose).
  virtual std::vector<const LHCb::Track*> & filteredTracks(std::vector<const LHCb::Track*> input_tracks, Tuples::Tuple* tuple = NULL)=0;


protected:
  std::string m_inputTrackLocation;

};
#endif // IFILTERVTXTOPOTRACKSTOOL_H
