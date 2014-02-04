#ifndef ICHALKIA_IDIRECTORYSCANNER_H 
#define ICHALKIA_IDIRECTORYSCANNER_H 1

// Include files
// from STL
#include <string>
// from Gaudi
#include "GaudiKernel/IInterface.h"


using namespace std;

static const InterfaceID IID_IDirectoryScanner ( "IDirectoryScanner", 1, 0 );

/** @class IDirectoryScanner IDirectoryScanner.h ichalkia/IDirectoryScanner.h
 *  
 *
 *  @author Ioannis Chalkiadakis
 *  @date   2014-01-24
 */
class IDirectoryScanner : virtual public IInterface {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IDirectoryScanner; }

  virtual StatusCode poller(string scan_path) = 0; ///GET SCAN DIRECTORY AS ARGUMENT
  virtual StatusCode addListener(ISvcLocator* Listener) = 0;
  virtual StatusCode remListener() = 0;
  virtual const StatusCode printListeners() = 0;
  
};
#endif // ICHALKIA_IDIRECTORYSCANNER_H
