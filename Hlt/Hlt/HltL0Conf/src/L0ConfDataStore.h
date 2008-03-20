// $Id: L0ConfDataStore.h,v 1.4 2008-03-20 11:38:07 albrecht Exp $
#ifndef L0CONFDATASTORE_H 
#define L0CONFDATASTORE_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/IIncidentListener.h"
#include "Event/State.h"
#include "TfKernel/TStationHitManager.h"

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
  bool checkConsistency();

  int nL0Candidates;
  std::vector<double> seedingTime;
  std::vector<double> decodingTime;
  std::vector<double> trackingTime;
  std::vector<double> nTHits;
  std::vector<double> region;
  std::vector<Tf::IStationSelector*> trackHypo;
 
protected:

private:
  void handle ( const Incident& incident );
  
  

};
#endif // L0CONFDATASTORE_H
