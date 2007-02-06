// $Id: IRelatedPV.h,v 1.3 2007-02-06 09:57:56 pkoppenb Exp $
#ifndef KERNEL_IRELATEDPV_H 
#define KERNEL_IRELATEDPV_H 1

// Include files
// from STL
#include <string>

#include "Event/VertexBase.h"
#include "Event/Particle.h"

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_IRelatedPV ( "IRelatedPV", 1, 0 );

/** @class IRelatedPV IRelatedPV.h Kernel/IRelatedPV.h
 *  
 *  Returns the related PV for a Particle.
 *  Actually tries to access the desktop of the 
 *  parent algorithm.
 *
 *
 *  @author Patrick Koppenburg
 *  @date   2006-09-12
 */
class IRelatedPV : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IRelatedPV; }

  /// Returns the best related PV 
  virtual const LHCb::VertexBase* bestPV(const LHCb::Particle*) = 0 ;
  
  /// Returns all related PVs ordered by weight and above a given weight 
  virtual StatusCode allPVs(const LHCb::Particle*p, 
                            LHCb::VertexBase::ConstVector&,
                            double minweight=0) = 0 ;  

protected:

private:

};
#endif // KERNEL_IRELATEDPV_H
