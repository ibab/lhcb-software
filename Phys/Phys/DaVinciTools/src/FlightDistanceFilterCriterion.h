#ifndef FLIGHTDISTANCEFILTERCRITERION_H 
#define FLIGHTDISTANCEFILTERCRITERION_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// from DaVinciTools
#include "DaVinciTools/IFilterCriterion.h"

// forward declarations
class IGeomDispCalculator;
class IDataProviderSvc;

/** @class FlightDistanceFilterCriterion FlightDistanceFilterCriterion.h
 *  Returns a yes/no depending on flight distance / significance w.r.t either 
 *  -> all PVs (for the min cuts)
 *  -> Best PV (default), chosen as the one for which the IPS of the composite 
 *     particle is the smallest (for the min and max cuts)
 *  Cuts:
 *       - on the minimum signed or unsigned flight distance or significance
 *         w.r.t all PVs or the Best PV
 *       - on the maximum signed or unsigned flight distance or significance
 *         w.r.t the Best PV
 *  @author Luis Fernandez
 *  @date   15/07/2004
 */
class FlightDistanceFilterCriterion : public GaudiTool, 
                           virtual public IFilterCriterion {

public:

  /// Standard constructor
  FlightDistanceFilterCriterion( const std::string& type, 
                      const std::string& name,
                      const IInterface* parent);
  
  /// Destructor
  virtual ~FlightDistanceFilterCriterion( ){ }; 

  /// Initialize: Prints Out the cut values.
  StatusCode initialize( );

  /// Test if flight distance filter is satisfied.
  inline bool isSatisfied( const Particle* const & part );

  /// Test if flight distance filter is satisfied.
  inline bool operator()( const Particle* const & part );

  StatusCode finalize(){return GaudiTool::finalize () ;};

private:

  // Two modes (both at the same time not possible with same instance):
  //           -> compute flight w.r.t Best PV (default)
  //           -> compute flight w.r.t all PVs
  bool       m_CutBestPV; ///< Cuts w.r.t Best PV (true). False w.r.t all PVs

  // Min cuts for flight distance
  double     m_minFPV; ///< Min unsigned Flight distance
  double     m_minFSPV; ///< Min unsigned Flight Significance
  double     m_minSignedFPV; ///< Min signed Flight distance
  double     m_minSignedFSPV; ///< Min signed Flight Significance

  // Max cuts for flight distance w.r.t the Best PV
  double     m_maxFPV; ///< Max unsigned Flight distance w.r.t Best PV
  double     m_maxFSPV; ///< Max unsigned Flight Significance w.r.t Best PV
  double     m_maxSignedFPV; ///< Max signed Flight distance w.r.t Best PV
  double     m_maxSignedFSPV; ///< Max signed Flight Significance w.r.t Best PV
  bool       m_Trg ;         ///< Use L1 PV even for offline particles

  std::string m_PVContainer;
  IDataProviderSvc* m_EDS;
  IGeomDispCalculator* m_vtxDisTool;
  IGeomDispCalculator* m_ipTool;

};

#endif // FLIGHTDISTANCEFILTERCRITERION_H
