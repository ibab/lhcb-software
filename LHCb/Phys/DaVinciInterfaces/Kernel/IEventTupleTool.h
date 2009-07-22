// $Id: IEventTupleTool.h,v 1.1.1.1 2009-07-22 20:54:51 jpalac Exp $
#ifndef DECAYTUPLE_IEVENTTUPLETOOL_H
#define DECAYTUPLE_IEVENTTUPLETOOL_H  1

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_IEventTupleTool ( "IEventTupleTool", 1, 0 );

namespace Tuples{
  class Tuple;
};

// ==================================================
// ==================================================

/** @class IEventTupleTool
 * 
 * Fill some variables into a tuple.
 *
 * \sa IParticleTupleTool
 * 
 * @author Jeremie Borel
 * @date November 2007
 */
class IEventTupleTool : virtual public IAlgTool {
 public:
  virtual ~IEventTupleTool(){};
  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IEventTupleTool; }

  //! Fill the tuple. 
  virtual StatusCode fill( Tuples::Tuple& )=0;
};



#endif // DECAYTUPLE_IEVENTTUPLETOOL_H
