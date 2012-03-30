// $Id: $
#ifndef PRESENTERDOC_IPRESENTERDOC_H 
#define PRESENTERDOC_IPRESENTERDOC_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_IPresenterDoc ( "IPresenterDoc", 1, 0 );

/** @class IPresenterDoc IPresenterDoc.h PresenterDoc/IPresenterDoc.h
 *  Interface for documentation of bin properties in the Presenter
 *
 *  @author Olivier Callot
 *  @date   2012-03-30
 */
class IPresenterDoc : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IPresenterDoc; }


  virtual std::string document( std::string& histoName, int binX, int binY ) = 0;
  
};
#endif // PRESENTERDOC_IPRESENTERDOC_H
