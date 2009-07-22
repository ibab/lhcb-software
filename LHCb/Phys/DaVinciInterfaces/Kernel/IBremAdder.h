// $Id: IBremAdder.h,v 1.1.1.1 2009-07-22 20:54:51 jpalac Exp $
#ifndef KERNEL_IBREMADDER_H 
#define KERNEL_IBREMADDER_H 1

// Include files
// from STL
#include <string>
// from Gaudi
#include "GaudiKernel/IAlgTool.h"
// from LHCb
#include "Event/Particle.h"

static const InterfaceID IID_IBremAdder ( "IBremAdder", 1, 0 );

/** @class IBremAdder IBremAdder.h Kernel/IBremAdder.h
 *  
 * Interface for BremStrahlung correction to electron particle
 *
 *  @author Olivier Deschamps
 *  @date   2006-10-25
 */
class IBremAdder : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IBremAdder; }
  // Add Brem
  virtual  bool addBrem( LHCb::Particle* particle ) const = 0 ;
  // Remove Brem
  virtual  bool removeBrem( LHCb::Particle* particle ) const = 0 ;


protected:

private:

};
#endif // KERNEL_IBREMADDER_H
