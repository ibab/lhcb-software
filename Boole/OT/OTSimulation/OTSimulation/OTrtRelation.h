// $Id: OTrtRelation.h,v 1.1.1.1 2004-09-03 13:35:47 jnardull Exp $
#ifndef OTSIMULATION_OTRTRELATION_H
#define OTSIMULATION_OTRTRELATION_H 1

// Gaudi
#include "GaudiKernel/AlgTool.h"

// OTEvent
#include "Event/MCOTDeposit.h"

// OTDet
#include "OTDet/DeOTDetector.h"

// OTSimulation
#include "OTSimulation/IOTrtRelation.h"

class IMagneticFieldSvc;
class MCOTDeposit;

/** @class OTrtRelation OTrtRelation.h "OTSimulation/OTrtRelation.h"
 *
 *  Tool for applying single cell efficiency for Outer Tracker.
 *  This tool is used to covert the drift distance to drift tim.e
 *   @li The parameterization of t_max as a function of B/Tesla comes from \
 *       test-beam data.
 *   @li For the moment assume a simple r-t linear relation.
 *   @li This treatment is only valid for 5 mm straws.
 *
 *  @author M.Needham
 *  @date   21/10/2000
 */

class OTrtRelation : public AlgTool, virtual public IOTrtRelation {

public:

  ///  Constructor
  OTrtRelation(const std::string& type, 
               const std::string& name, 
               const IInterface* parent);

  /// destructor
  virtual ~OTrtRelation();

  /// initialize method
  virtual StatusCode initialize();

  /// finalize method
  virtual StatusCode finalize();

  /// tool 'operation'. Fills the deposit time entry.
  StatusCode convertRtoT(MCOTDeposit* aDeposit);
  
  /// r-t relation with correction for the magnetic field
  double driftTime(const double driftDist, const HepPoint3D& aPoint);
  
  /// inverse r-t relation with correction for the magnetic field
  double driftDistance( const double driftTime, const HepPoint3D& aPoint );

private:
 
  /// returns the drift delay without magnetic field correction
  double driftDelay();

  /// pointer to mag field service
  IMagneticFieldSvc* m_magFieldSvc;

  /// pointer to XML geometry:
  DeOTDetector* m_tracker;

};

#endif // OTSIMULATION_OTRTRELATION_H
