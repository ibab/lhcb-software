// $Id: IMatchTool.h,v 1.2 2008-05-09 07:23:15 cattanem Exp $
#ifndef IMATCHTOOL_H 
#define IMATCHTOOL_H 1

// Include files

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// Event model
#include "Event/Track.h"


static const InterfaceID IID_IMatchTool ( "IMatchTool", 1, 0 );

/** @class IMatchTool IMatchTool.h TrackInterfaces/IMatchTool.h
 *  
 *
 *  @author Johannes Albrecht
 *  @date   2008-04-25
 */
class IMatchTool : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IMatchTool; }


  /// match velo tracks with seed tracks, output in matchs
  virtual StatusCode match(const LHCb::Tracks& velos,
                           const LHCb::Tracks& seeds,
                           LHCb::Tracks& matchs )=0;
  
  /// for trigger: match a single velo with seed track, 
  /// output: output track and match chi2, 
  /// SUCCESS if chi2 below cut, FAILURE if not
  virtual StatusCode matchSingle(const LHCb::Track& velo,
                                 const LHCb::Track& seed , 
                                 LHCb::Track& output,
                                 double& chi2)=0;
  

protected:

private:

};
#endif // IMATCHTOOL_H
