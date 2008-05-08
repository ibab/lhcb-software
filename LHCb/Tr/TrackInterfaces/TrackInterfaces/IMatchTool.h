// $Id: IMatchTool.h,v 1.1 2008-05-08 12:44:51 albrecht Exp $
#ifndef IMATCHTOOL_H 
#define IMATCHTOOL_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

#include "Event/Track.h"




static const InterfaceID IID_IMatchTool ( "IMatchTool", 1, 0 );

/** @class IMatchTool IMatchTool.h
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
