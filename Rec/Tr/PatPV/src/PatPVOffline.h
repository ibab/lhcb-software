// $Id: PatPVOffline.h,v 1.2 2007-12-04 08:46:52 witekma Exp $
#ifndef PATPVOFFLINE_H
#define PATPVOFFLINE_H 1
// Include files:
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
// Interfaces
#include "TrackInterfaces/IPVOfflineTool.h"

/** @class PatPVOffline PatPVOffline.h
 *
 *
 *  @author Mariusz Witek
 *  @date   2010-10-05
 */
class PatPVOffline : public GaudiAlgorithm {
public:
  // Standard constructor
  PatPVOffline(const std::string& name, ISvcLocator* pSvcLocator);
  // Destructor
  ~PatPVOffline();
  virtual StatusCode initialize();    // Algorithm initialization
  virtual StatusCode execute();       // Algorithm execution
private:
  std::string m_outputVertices;
  // Tools
  IPVOfflineTool* m_pvsfit;    // PV fitting tool
};
#endif // PATPVOFFLINE_H
