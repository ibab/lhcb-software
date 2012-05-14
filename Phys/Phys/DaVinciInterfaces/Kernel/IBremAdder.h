// $Id: IBremAdder.h,v 1.2 2009-08-04 09:45:31 jpalac Exp $
#ifndef KERNEL_IBREMADDER_H 
#define KERNEL_IBREMADDER_H 1

// Include files
// from STL
#include <string>
// from Gaudi
#include "GaudiKernel/IAlgTool.h"
// from LHCb
#include "Event/Particle.h"
#include "Event/CaloHypo.h"

/** @class IBremAdder IBremAdder.h Kernel/IBremAdder.h
 *  
 * Interface for BremStrahlung correction to electron particle
 *
 *  @author Olivier Deschamps
 *  @date   2006-10-25
 */
class GAUDI_API IBremAdder : virtual public IAlgTool {
public: 

  DeclareInterfaceID(IBremAdder, 3, 0);

  // Add Brem
  virtual  bool addBrem( LHCb::Particle* particle , bool force=false) const = 0 ;
  // Remove Brem
  virtual  bool removeBrem( LHCb::Particle* particle , bool force=false) const = 0 ;
  // get the list of brem candidates
  virtual const std::vector<const LHCb::CaloHypo*> bremList(const LHCb::Particle* particle)const = 0;

protected:
  virtual ~IBremAdder() ;               // virtual and protected destructors

private:

};
#endif // KERNEL_IBREMADDER_H
