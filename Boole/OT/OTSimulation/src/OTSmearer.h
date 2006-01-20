// $Id: OTSmearer.h,v 1.3 2006-01-20 12:57:05 janos Exp $
#ifndef OTSIMULATION_OTSMEARER_H
#define OTSIMULATION_OTSMEARER_H 1

// Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/SmartIF.h"

// OTEvent
#include "Event/MCOTDeposit.h"

// OTDet
#include "OTDet/DeOTDetector.h"

// OTSimulation
#include "OTSimulation/IOTSmearer.h"

// Forward declaration
class IMagneticFieldSvc;

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

  /// finalize method
  virtual StatusCode finalize();

  /// tool 'operation' - returns smeared distance + sigma used
  StatusCode smear(LHCb::MCOTDeposit* aDeposit);

  /// Get the resolution (without magnetic field correction)
  double resolution();

  /// Get the resolution (with magnetic field correction)
  double resolution( Gaudi::XYZPoint& aPoint );
  
private:

  /// parameterization of resolution as function of field
  double sigmaParamFunc(const double By);
  /// pointer to magnetic field service
  IMagneticFieldSvc* m_magFieldSvc;
  /// pointer to XML geometry:
  DeOTDetector* m_tracker;

  /// jobOptions parameters
  SmartIF<IRndmGen> m_genDist;      ///< smart interface to generator

};

#endif // OTSIMULATION_OTSMEARER_H
