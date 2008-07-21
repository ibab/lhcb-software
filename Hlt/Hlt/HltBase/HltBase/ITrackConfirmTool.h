// $Id: ITrackConfirmTool.h,v 1.3 2008-07-21 12:59:44 albrecht Exp $
#ifndef ITRACKCONFIRMTOOL_H 
#define ITRACKCONFIRMTOOL_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "HltBase/ParabolaHypothesis.h"

class LHCb::State;
class LHCb::Track;

static const InterfaceID IID_ITrackConfirmTool ( "ITrackConfirmTool", 1, 0 );

/** @class ITrackConfirmTool ITrackConfirmTool.h
 *  
 *
 *  @author Johannes Albrecht
 *  @date   2007-01-26
 */
class ITrackConfirmTool : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_ITrackConfirmTool; }

  virtual StatusCode tracks(const LHCb::State& seedState, std::vector<LHCb::Track*>& outputTracks ) =0;
  
  virtual ParabolaHypothesis prepareT( const LHCb::State& seedState ,  std::vector<LHCb::LHCbID>& ids)=0;
  

protected:

private:

};
#endif // ITRACKCONFIRMTOOL_H
