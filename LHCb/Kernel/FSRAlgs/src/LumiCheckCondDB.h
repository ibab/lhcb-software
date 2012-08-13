// $Id: LumiCheckCondDB.h,v 1.5 2010-05-12 08:11:08 panmanj Exp $
#ifndef LUMICHECKCONDDB_H 
#define LUMICHECKCONDDB_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// for incidents listener
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"

// for DB
#include "GaudiKernel/IDetDataSvc.h"
#include "GetLumiParameters.h"

// for TCK
#include "Kernel/IPropertyConfigSvc.h"

/** @class LumiCheckCondDB LumiCheckCondDB.h
 *   
 *  @author Jaap Panman
 *  @date   2010-10-20
 */

class LumiCheckCondDB : public GaudiAlgorithm  
  , public virtual IIncidentListener {

public: 
  /// Standard constructor
  LumiCheckCondDB( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~LumiCheckCondDB( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode stop      ();    ///< Algorithm stopping
  virtual StatusCode finalize  ();    ///< Algorithm finalization

  // ==========================================================================
  // IIncindentListener interface
  // ==========================================================================
  virtual void handle ( const Incident& ) ;
  // ==========================================================================

protected:
  virtual StatusCode checkDB(std::string s);    ///< DB checking code - event loop

protected:
  /// Reference to run records data service
  SmartIF<IDetDataSvc> m_dds;                   ///< DetectorDataSvc

private:
  unsigned long long m_startTime;               ///< start probing database here
  unsigned long long m_numberSteps;             ///< number of steps checked
  unsigned long long m_stepHours;               ///< number of hours per step

  // database conditions and calibration factors
  IGetLumiParameters *m_databaseTool;           ///< tool to query luminosity database
  mutable IIncidentSvc* m_incSvc ;              ///< the incident service 

};
#endif // LUMICHECKCONDDB_H
