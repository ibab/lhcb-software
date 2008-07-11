// $Id: IDaVinciAssociatorsWrapper.h,v 1.3 2008-07-11 13:30:09 gligorov Exp $
#ifndef IDAVINCIASSOCIATORSWRAPPER_H 
#define IDAVINCIASSOCIATORSWRAPPER_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

#include "Kernel/Particle2MCLinker.h"
#include "Kernel/Particle2MCMethod.h"

static const InterfaceID IID_IDaVinciAssociatorsWrapper ( "IDaVinciAssociatorsWrapper", 1, 0 );

/** @class IDaVinciAssociatorsWrapper IDaVinciAssociatorsWrapper.h
 *  
 *  Wrapper around  Particle2MCLinker class
 *
 *  To use:
 *  
 *  Particle2MCLinker mylinker = m_myLinkerTool->linker(linkerMethod,container);
 *
 *  where linker Method corresponds to one of the Methods declared in Kernel/Particle2MCMethod.h,
 *  for example Link, Composite, Chi2. Container allows you to specify a specifi location for the
 *  particles to be associated, It defaults to 0 if omitted:
 * 
 *  Particle2MCLinker mylinker = m_myLinkerTool->linker(linkerMethod);
 *
 *  Note that you will need to declare a different instance of this tool
 *  for each linker which you wish to use. Declare an instance in your
 *  algorithm's initialize:
 *
 *  m_myLinkerTool = tool<IDaVinciAssociatorsWrapper>("DaVinciAssociatorsWrapper","exampleTool",this);
 *
 *  @author V. Gligorov (based on code by P. Koppenburg)
 *  @date   2008-06-26
 */
class IDaVinciAssociatorsWrapper : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IDaVinciAssociatorsWrapper; }

  virtual Particle2MCLinker* linker(Particle2MCMethod::AssociationMethod myMethod = Particle2MCMethod::Links, 
                                    std::vector< std::string > myLocations
                                      = std::vector<std::string>(1,"")
                                   ) = 0 ; ///< returns a linker
protected:

private:

};
#endif // IDAVINCIASSOCIATORSWRAPPER_H
