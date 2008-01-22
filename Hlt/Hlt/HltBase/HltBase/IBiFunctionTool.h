// $Id: IBiFunctionTool.h,v 1.1 2008-01-22 09:35:00 hernando Exp $
#ifndef HLTBASE_IBIFUNCTIONTOOL_H 
#define HLTBASE_IBIFUNCTIONTOOL_H 1

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

static const InterfaceID IID_IBiFunctionTool ( "IBiFunctionTool", 1, 0 );

/** @class IBiFunctionTool IBiFunctionTool.h HltBase/IBiFunctionTool.h
 *  
 *
 *  @author Jose Angel Hernando Morata
 *  @date   2007-12-08
 */
template <class T1, class T2>
class IBiFunctionTool : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IBiFunctionTool; }

  virtual double function(const T1& t1, const T2& t2) = 0;



};

class ITrackBiFunctionTool : 
  public IBiFunctionTool<LHCb::Track, LHCb::Track> {
};

class ITrackVertexBiFunctionTool : 
  public IBiFunctionTool<LHCb::Track, LHCb::RecVertex> {
};

class IVertexBiFunctionTool : 
  public IBiFunctionTool<LHCb::RecVertex, LHCb::RecVertex> {
};


#endif // HLTBASE_IBIFUNCTIONTOOL_H
