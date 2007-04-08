// $Id: OTSmearer.h,v 1.7 2007-04-08 16:54:51 janos Exp $
#ifndef OTSIMULATION_OTSMEARER_H
#define OTSIMULATION_OTSMEARER_H 1

// Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/SmartIF.h"

// OTSimulation
#include "IOTSmearer.h"

// Forward declaration
class IMagneticFieldSvc;
class DeOTDetector;
namespace LHCb
{
  class MCOTDeposit;
}

/** @class OTSmearer OTSmearer.h "OTSimulation/OTSmearer.h"
 *
 *  Tool for smearing the drift distance for Outer Tracker.
 *  This tool is used to smear the drift distance for 5mm straws
 *  of 5 mm straw. The parameterization comes from test-beam data.
 *
 *  @author M.Needham
 *  @date   21/10/2000
 */

class OTSmearer : public GaudiTool, virtual public IOTSmearer {

public:

  ///  Constructor
  OTSmearer(const std::string& type, 
            const std::string& name, 
            const IInterface* parent);

  /// destructor
  virtual ~OTSmearer();

  /// initialize method
  virtual StatusCode initialize();

  /// tool 'operation' - returns smeared distance + sigma used
  void smear(LHCb::MCOTDeposit* aDeposit);

  /// Get the resolution (without magnetic field correction)
  double resolution();

  /// Get the resolution (with magnetic field correction)
  double resolution( Gaudi::XYZPoint& aPoint );
  
private:

  /// pointer to magnetic field service
  IMagneticFieldSvc* m_magFieldSvc;
  /// pointer to XML geometry:
  DeOTDetector* m_tracker;

  /// jobOptions parameters
  SmartIF<IRndmGen> m_genDist;      ///< smart interface to generator

};

#endif // OTSIMULATION_OTSMEARER_H
