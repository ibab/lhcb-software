#ifndef MOMENTUM2FLIGHTANGLEFILTERCRITERION_H 
#define MOMENTUM2FLIGHTANGLEFILTERCRITERION_H 1

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

/** @class Momentum2FlightAngleFilterCriterion 
 *         Momentum2FlightAngleFilterCriterion.h
 *
 *  Returns a yes/no depending on the cosine of the angle between the particle 
 *  momentum and the flight distance w.r.t either
 *  -> all PVs
 *  -> Best PV (default), chosen as the one for which the IPS of the composite
 *     particle is the smallest
 * 
 *  @author Luis Fernandez
 *  @date   27/07/2004
 */
class Momentum2FlightAngleFilterCriterion : public GaudiTool, 
                                            virtual public IFilterCriterion {

public:

  /// Standard constructor
  Momentum2FlightAngleFilterCriterion( const std::string& type, 
                                       const std::string& name,
                                       const IInterface* parent);
  
  /// Destructor
  virtual ~Momentum2FlightAngleFilterCriterion( ){ }; 

  /// Initialize: Prints Out the cut values.
  StatusCode initialize( );

  /// Test if filter is satisfied.
  inline bool isSatisfied( const Particle* const & part );

  /// Test if filter is satisfied.
  inline bool operator()( const Particle* const & part );

  StatusCode finalize(){return GaudiTool::finalize () ;};

private:

  bool       m_CutBestPV; ///< Cuts w.r.t Best PV (true). False w.r.t all PVs
  double     m_mincos; ///< Min .cosine between momentum and flight distance
  bool       m_Trg ;         ///< Use L1 PV even for offline particles
  std::string m_PVContainer;
  
  IDataProviderSvc* m_EDS;
  IGeomDispCalculator* m_ipTool;

};

#endif // MOMENTUM2FLIGHTANGLEFILTERCRITERION_H
