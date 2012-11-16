#ifndef HLTINSERTTRACKERRPARAM_H 
#define HLTINSERTTRACKERRPARAM_H 1

// Include files
// from Hlt
#include "GaudiAlg/GaudiAlgorithm.h"

/** @class HltInsertTrackErrParam HltInsertTrackErrParam.h
 *
 *  Reimplementation of Hugo's TrgInsertTrackErrParam.
 *  Hacks error matrix of tracks according to 1/Pt. 
 *  See note <A NAME="http://cdsweb.cern.ch/record-restricted/832444/">LHCb-2005-012</a> (internal).
 *
 *  @tod: Should this be an HltAlgorithm? 
 * 
 *  @author Patrick Koppenburg, Hugo Ruiz
 *  @date   2007-07-16
 */
class HltInsertTrackErrParam : public GaudiAlgorithm {
public: 
  /// Standard constructor
  HltInsertTrackErrParam( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltInsertTrackErrParam( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

protected:

  StatusCode insertParamInTrack( LHCb::Track* tr );
  StatusCode insertParamInState( LHCb::State* s );
  void printParams( std::vector<double>& , std::string );
  
private:

  std::vector<double> m_xParams ;   ///< x parameters
  std::vector<double> m_yParams ;   ///< y parameters 
  std::string m_inputLocation ;     ///< Location for input
  /// Location for output. If empty, tracks will be stored in iputlocation as well
  std::string m_outputLocation ;    
  bool m_newLocation ; ///< = m_outputLocation!="" 
  bool m_keepMomentum ; ///< keep momentum covariance terms
};
#endif // HLTINSERTTRACKERRPARAM_H
