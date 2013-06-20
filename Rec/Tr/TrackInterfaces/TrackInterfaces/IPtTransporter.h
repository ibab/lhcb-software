// $Id: IPtTransporter.h,v 1.2 2008-05-09 07:23:15 cattanem Exp $
#ifndef TRACKINTERFACES_IPTTRANSPORTER_H 
#define TRACKINTERFACES_IPTTRANSPORTER_H 1

// Include files

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

namespace LHCb {
  class State;
}

static const InterfaceID IID_IPtTransporter ( "IPtTransporter", 1, 0 );

/** @class IPtTransporter IPtTransporter.h TrackInterfaces/IPtTransporter.h
 *  
 *  calculate pt at origin from a given state at T
 *
 *  @author Johannes Albrecht
 *  @date   2008-05-08
 */
class IPtTransporter : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IPtTransporter; }

  virtual double ptAtOrigin(double zref, double xref, double yref,
                            double tx, double ty, double p) const =0;
  
  virtual double ptAtOrigin(const LHCb::State& stateAtT) const =0;
  
protected:

private:

};
#endif // TRACKINTERFACES_IPTTRANSPORTER_H
