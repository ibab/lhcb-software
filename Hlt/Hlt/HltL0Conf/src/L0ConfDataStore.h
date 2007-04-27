// $Id: L0ConfDataStore.h,v 1.1 2007-04-27 09:47:51 albrecht Exp $
#ifndef L0CONFDATASTORE_H 
#define L0CONFDATASTORE_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/IIncidentListener.h"

/** @class L0ConfDataStore L0ConfDataStore.h
 *  
 *   This is a trivial tool which gets called by the
 *   L0Conf algorithms to extract debug information.
 *
 *
 *  @author Johannes Albrecht
 *  @date   2007-04-10
 */

static const InterfaceID IID_L0ConfDataStore ( "L0ConfDataStore", 1, 0 );

class L0ConfDataStore : 
  public GaudiTool, 
  virtual public IIncidentListener {
public: 

  //no separateI
  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_L0ConfDataStore; }

  /// Standard constructor
  L0ConfDataStore( const std::string& type, 
             const std::string& name,
             const IInterface* parent);

  virtual ~L0ConfDataStore( ); ///< Destructor

  StatusCode initialize();
  
  void clearStore();  

  double test;
  int region;
  double decodingTime;
  

  int nModulesToDecode;
  

protected:

private:
  void handle ( const Incident& incident );


};
#endif // L0CONFDATASTORE_H
