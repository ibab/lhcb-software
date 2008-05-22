// $Id: IFunctionTool.h,v 1.1 2008-05-22 07:43:41 cattanem Exp $
#ifndef TRACKINTERFACES_IFUNCTIONTOOL_H 
#define TRACKINTERFACES_IFUNCTIONTOOL_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

//forward declarations
namespace LHCb {
  class Track;
  class RecVertex;
};

static const InterfaceID IID_IFunctionTool ( "IFunctionTool", 1, 0 );

/** @class IFunctionTool IFunctionTool.h TrackInterfaces/IFunctionTool.h
 *  
 *
 *  @author Jose Angel Hernando Morata
 *  @date   2007-12-08
 */
template <class T>
class IFunctionTool : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IFunctionTool; }


  virtual double function(const T& t ) = 0;
  
protected:

private:

};

class ITrackFunctionTool : public IFunctionTool<LHCb::Track> {
};

class IRecVertexFunctionTool : public IFunctionTool<LHCb::RecVertex> {
};



#endif // TRACKINTERFACES_IFUNCTIONTOOL_H
