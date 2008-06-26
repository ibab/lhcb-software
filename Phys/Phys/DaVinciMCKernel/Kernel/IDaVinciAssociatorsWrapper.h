// $Id: IDaVinciAssociatorsWrapper.h,v 1.1 2008-06-26 23:48:19 gligorov Exp $
#ifndef IDAVINCIASSOCIATORSWRAPPER_H 
#define IDAVINCIASSOCIATORSWRAPPER_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

#include "Kernel/Particle2MCLinker.h"

static const InterfaceID IID_IDaVinciAssociatorsWrapper ( "IDaVinciAssociatorsWrapper", 1, 0 );

/** @class IDaVinciAssociatorsWrapper IDaVinciAssociatorsWrapper.h
 *  
 *  Wrapper around  Particle2MCLinker class
 *
 *  @author V. Gligorov (based on code by P. Koppenburg)
 *  @date   2008-06-26
 */
class IDaVinciAssociatorsWrapper : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IDaVinciAssociatorsWrapper; }

  virtual Particle2MCLinker* linker(std::string, std::vector< std::string >) = 0 ; ///< returns a linker
protected:

private:

};
#endif // IDAVINCIASSOCIATORSWRAPPER_H
