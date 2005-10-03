// $Id: IBeamTool.h,v 1.1 2005-10-03 09:47:18 robbep Exp $
#ifndef GENERATORS_IBEAMTOOL_H 
#define GENERATORS_IBEAMTOOL_H 1

// Include files
// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "CLHEP/Vector/ThreeVector.h"

/** @class IBeamTool IBeamTool.h Generators/IBeamTool.h
 *  
 *  Abstract interface to beam tool
 * 
 *  @author Patrick Robbe
 *  @date   2005-08-17
 */

static const InterfaceID IID_IBeamTool( "IBeamTool" , 1 , 0 ) ;

class IBeamTool : virtual public IAlgTool {
public:
  static const InterfaceID& interfaceID() { return IID_IBeamTool ; }

  virtual void getMeanBeams( Hep3Vector & pBeam1, Hep3Vector & pBeam2 ) 
    const = 0 ;
  
  virtual void getBeams( Hep3Vector & pBeam1, Hep3Vector & pBeam2 ) = 0 ;
};
#endif // GENERATORS_IBEAMTOOL_H
