// $Id: IRelatedPV.h,v 1.2 2009-08-04 09:45:31 jpalac Exp $
#ifndef KERNEL_IRELATEDPV_H 
#define KERNEL_IRELATEDPV_H 1

// Include files
// from STL
#include <string>

#include "Event/VertexBase.h"
#include "Event/Particle.h"

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

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
class GAUDI_API IRelatedPV : virtual public IAlgTool {
public: 

  DeclareInterfaceID(IRelatedPV, 2, 0);
  
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
