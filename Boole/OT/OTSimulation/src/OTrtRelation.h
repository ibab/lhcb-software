// $Id: OTrtRelation.h,v 1.8 2007-06-27 15:44:55 janos Exp $
#ifndef OTSIMULATION_OTRTRELATION_H
#define OTSIMULATION_OTRTRELATION_H 1

// Gaudi
#include "GaudiAlg/GaudiTool.h"

// OTSimulation
#include "IOTrtRelation.h"

// Forward declarations
class DeOTDetector;
class IMagneticFieldSvc;


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

class OTrtRelation : public GaudiTool, virtual public IOTrtRelation {

public:

  ///  Constructor
  OTrtRelation(const std::string& type, 
               const std::string& name, 
               const IInterface* parent);

  /// destructor
  virtual ~OTrtRelation();

  /// initialize method
  virtual StatusCode initialize();

  /// tool 'operation'. Fills the deposit time entry.
  void convertRtoT(LHCb::MCOTDeposit* aDeposit) const;
  
  /// r-t relation with correction for the magnetic field
  double driftTime(const double driftDist, const Gaudi::XYZPoint& aPoint) const;
  
  /// inverse r-t relation with correction for the magnetic field
  double driftDistance( const double driftTime, const Gaudi::XYZPoint& aPoint ) const;

private:
 
  /// returns the drift delay without magnetic field correction
  double driftDelay();

  /// pointer to mag field service
  IMagneticFieldSvc* m_magFieldSvc;

  /// pointer to XML geometry:
  DeOTDetector* m_tracker;

};

#endif // OTSIMULATION_OTRTRELATION_H
