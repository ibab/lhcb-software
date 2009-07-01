// $Id: IGetArrival.h,v 1.1 2009-07-01 18:27:11 polye Exp $
#ifndef IGETARRIVAL_H 
#define IGETARRIVAL_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include <vector>
#include "Event/Track.h"

static const InterfaceID IID_IGetArrival ( "IGetArrival", 1, 0 );

/** @class IGetArrival ICLTool.h
 *  
 *
 *  @author Jose Angel Hernando Morata
 *  @author Xabier Cid Vidal
 *  @date   2008-07-02
 */
class IGetArrival : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IGetArrival; }
  
  virtual StatusCode getArrivalFromTrack(const LHCb::Track& mutrack, double& parr)=0;
  virtual StatusCode clArrival(const LHCb::Track& muTrack, double& clarr)=0;
  virtual StatusCode clArrival(const double p,const std::vector<int>& type_st, double& clarr)=0;
  

private:

};
#endif // IGETARRIVAL_H
