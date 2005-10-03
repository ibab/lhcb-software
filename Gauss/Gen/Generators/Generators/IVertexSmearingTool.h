// $Id: IVertexSmearingTool.h,v 1.1 2005-10-03 09:53:46 robbep Exp $
#ifndef GENERATORS_IVERTEXSMEARINGTOOL_H 
#define GENERATORS_IVERTEXSMEARINGTOOL_H 1

// Include files
// from Gaudi
#include "GaudiKernel/IAlgTool.h"

class HepMCEvent ;

/** @class IVertexSmearingTool IVertexSmearingTool.h 
 *   Generators/IVertexSmearingTool.h
 *  
 *  Abstract interface to vertex smearing tools
 * 
 *  @author Patrick Robbe
 *  @date   2005-08-17
 */

static const InterfaceID IID_IVertexSmearingTool( "IVertexSmearingTool" , 1 , 
                                                  0 ) ;

class IVertexSmearingTool : virtual public IAlgTool {
public:
  static const InterfaceID& interfaceID() { return IID_IVertexSmearingTool ; }
  
  virtual StatusCode smearVertex( HepMCEvent * theEvent ) = 0 ;
};
#endif // GENERATORS_ISMEARINGTOOL_H
