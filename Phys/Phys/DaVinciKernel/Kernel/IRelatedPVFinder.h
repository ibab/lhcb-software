// $Id: IRelatedPVFinder.h,v 1.1 2006-10-22 13:15:55 pkoppenb Exp $
#ifndef KERNEL_IRELATEDPVFINDER_H 
#define KERNEL_IRELATEDPVFINDER_H 1

// Include files
// from STL
#include <string>

#include "Event/RecVertex.h"
#include "Event/Particle.h"

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_IRelatedPVFinder ( "IRelatedPVFinder", 1, 0 );

/** @class IRelatedPVFinder IRelatedPVFinder.h Kernel/IRelatedPVFinder.h
 *  
 *  Finds related PV according to some criteria
 *
 *  @todo need also to pass the weight... best is to pass the relations table altogether
 *
 *  @author Patrick Koppenburg
 *  @date   2006-10-22
 */
class IRelatedPVFinder : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IRelatedPVFinder; }

  /// Build relation table and store it in desktop
  virtual StatusCode relatedPVs(const LHCb::Particle* p, 
                                Particle2Vertex::Table* ) const =  0;

protected:

private:

};
#endif // KERNEL_IRELATEDPVFINDER_H
