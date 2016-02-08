// $Id: IPatDebugTTTool.h,v 1.1 2008-12-04 09:07:35 cattanem Exp $
#ifndef PATKERNEL_IPATDEBUGTTTOOL_H 
#define PATKERNEL_IPATDEBUGTTTOOL_H 1

// Include files
#include "PatKernel/PatTTHit.h"
#include "TfKernel/TTStationHitManager.h"
// Forward declarations
class MsgStream;

namespace LHCb{
  class Track;
}

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_IPatDebugTTTool ( "IPatDebugTTTool", 1, 0 );

/** @class IPatDebugTTTool IPatDebugTTTool.h PatKernel/IPatDebugTTTool.h
 *  
 *
 *  @author Olivier Callot
 *  @date   2007-10-22
 */
class IPatDebugTTTool : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IPatDebugTTTool; }

  virtual void debugTTClusterOnTrack( const LHCb::Track* track, 
                                      const PatTTHits::const_iterator beginCoord,
                                      const PatTTHits::const_iterator endCoord   ) = 0;
  
  virtual void debugTTCluster( MsgStream& msg, const PatTTHit* hit ) = 0;

  virtual bool isTrueHit( const LHCb::Track* track, const PatTTHit* hit) = 0;
  
  virtual double fracGoodHits( const LHCb::Track* track, const PatTTHits& hits) = 0;
  
  virtual bool isTrueTrack( const LHCb::Track* track, const PatTTHits& hits) = 0;
  virtual bool isTrueTrack( const LHCb::Track* track, const Tf::TTStationHitManager<PatTTHit>::HitRange& hits) = 0;

  virtual void chi2Tuple( const double p, const double chi2, const unsigned int nHits) = 0;
  
  //added by AD 2/1/16 for efficiency vs step

  virtual void initializeSteps(std::vector<std::string> steps) = 0;//initialize all steps in the process  

  virtual void recordStepInProcess(std::string step, bool result) = 0;//record the result of a step in the process
  
  virtual void resetflags() = 0;//reset all flags

  virtual void ForceMCHits(PatTTHits& hits, LHCb::Track* track) = 0;//Special. Force only MC matched hits in the track.

protected:

private:

};
#endif // PATKERNEL_IPATDEBUGTTTOOL_H
