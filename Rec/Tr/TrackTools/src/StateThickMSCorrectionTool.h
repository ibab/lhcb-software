// $Id: StateThickMSCorrectionTool.h,v 1.3 2008-03-31 07:13:28 mneedham Exp $
#ifndef TRACKTOOLS_STATETHICKMSCORRECTIONTOOL_H 
#define TRACKTOOLS_STATETHICKMSCORRECTIONTOOL_H 1

// Include files
// -------------
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// from TrackInterfaces
#include "TrackInterfaces/IStateCorrectionTool.h"

// from TrackEvent
#include "Event/State.h"

/** @class StateThickMSCorrectionTool StateThickMSCorrectionTool.h
 *  
 *  This state correction tool applies a multiple scattering correction
 *  for a thick scatter.
 *  As scattering formula is not linear, add possibility to omit log term
 *  using 'RossiAndGreisen', which should lead to better pulls.
 *
 * Parameters:
 *  - MSFudgeFactor2: Scale factor (squared) multiplied with the (13.6MeV)^2 in MS formula
 *  - UseRossiAndGreisen: Don't use the log-term in MS formula (this is recommended from 2015 on, as the log-term breaks the linearity of many scatterers after each other)
 *  - RossiAndGreisenFact2: Constant (squared) in MS formula to replace the (13.6MeV)^2. Only used if larger than 0. Default is '-1e42' and then (13.6MeV)^2 is used.
 *
 *  @author Eduardo Rodrigues
 *  @date   2006-08-21
 *  (Original code taken from the master extrapolator)
 *
 *  @author Miriam Hess, Michel De Cian
 *  @date 2015-07-10
 *
 */
class StateThickMSCorrectionTool : public GaudiTool,
                                   virtual public IStateCorrectionTool {
public:
  /// Correct a State for multiple scattering in the case of a thick scatter
  void correctState( LHCb::State& state,
                     const Material* material,
                     double wallThickness = 0,
                     bool upstream = true,  
                     LHCb::ParticleID pid = LHCb::ParticleID(211) ) const override;
  
  /// Standard constructor
  StateThickMSCorrectionTool( const std::string& type, 
                              const std::string& name,
                              const IInterface* parent);
  
  ~StateThickMSCorrectionTool( ) override = default; ///< Destructor
  
  virtual StatusCode initialize() override;
private:
  // Job options
  double m_msff2;    ///< fudge factor for multiple scattering errors
  double m_msff2MoliereFact2; ///< auxiliary variable, precomputed
  double m_msff2RossiAndGreisenFact2; ///< factor to use when log term is omitted
  bool   m_useRossiAndGreisen; ///< if true, not log term is used
  
  
};
#endif // TRACKTOOLS_STATETHICKMSCORRECTIONTOOL_H
