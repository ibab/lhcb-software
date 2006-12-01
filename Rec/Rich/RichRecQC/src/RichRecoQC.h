
//---------------------------------------------------------------------------------
/** @file RichRecoQC.h
 *
 *  Header file for RICH reconstruction monitoring algorithm : RichRecoQC
 *
 *  CVS Log :-
 *  $Id: RichRecoQC.h,v 1.18 2006-12-01 16:02:32 cattanem Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   2002-07-02
 */
//---------------------------------------------------------------------------------

#ifndef RICHRECQC_RICHRECOQC_H
#define RICHRECQC_RICHRECOQC_H 1

// base class
#include "RichRecBase/RichRecHistoAlgBase.h"

// from Gaudi
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"

// Interfaces
#include "RichRecBase/IRichRecMCTruthTool.h"

// temporary histogramming numbers
#include "RichRecBase/RichDetParams.h"

// interfaces
#include "RichRecBase/IRichTrackSelector.h"
#include "RichKernel/IRichParticleProperties.h"
#include "RichRecBase/IRichCherenkovAngle.h"
#include "RichRecBase/IRichCherenkovResolution.h"

// RichKernel
#include "RichKernel/RichStatDivFunctor.h"

// GaudiPI
//#include "AIDA_Proxy/AIDA_Proxy.h"

// boost
#include "boost/assign/list_of.hpp"

//---------------------------------------------------------------------------------
/** @class RichRecoQC RichRecoQC.h
 *
 *  Quality control monitor for Rich Reconstruction
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   2002-07-02
 */
//---------------------------------------------------------------------------------

class RichRecoQC : public RichRecHistoAlgBase 
{

public:

  /// Standard constructor
  RichRecoQC( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~RichRecoQC( ); ///< Destructor

  virtual StatusCode initialize();    // Algorithm initialization
  virtual StatusCode execute   ();    // Algorithm execution
  virtual StatusCode finalize  ();    // Algorithm finalization

private: // methods

  /// Fit a Gaussian to the given distribution
  //AIDA::IFitResult * fitHisto( AIDA::IHistogram1D * histo,
  //                             const std::pair<double,double> & range,
  //                             const std::vector<double> & params );

  /// Access RichParticleProperties tool on demand
  const IRichParticleProperties * partPropTool() const
  {
    if ( !m_richPartProp ) { acquireTool( "RichParticleProperties", m_richPartProp ); }
    return m_richPartProp;
  }

  /// access RichCherenkovAngle tool on demand
  const IRichCherenkovAngle * ckAngleTool() const
  {
    if ( !m_ckAngle ) { acquireTool( "RichCherenkovAngle", m_ckAngle ); }
    return m_ckAngle;
  }

  /// access RichRecMCTruthTool tool on demand
  const IRichRecMCTruthTool * richRecMCTool() const
  {
    if ( !m_richRecMCTruth ) { acquireTool( "RichRecMCTruthTool", m_richRecMCTruth ); }
    return m_richRecMCTruth;
  }

  /// access RichCherenkovResolution tool on demand
  const IRichCherenkovResolution * ckResTool() const
  {
    if ( !m_ckRes ) { acquireTool( "RichCherenkovResolution", m_ckRes ); }
    return m_ckRes;
  }

private: // data

  // Pointers to tool instances
  mutable const IRichParticleProperties * m_richPartProp; ///< Rich Particle properties
  mutable const IRichCherenkovAngle * m_ckAngle;  ///< Pointer to RichCherenkovAngle tool
  mutable const IRichCherenkovResolution * m_ckRes; ///< Cherenkov angle resolution tool
  mutable const IRichRecMCTruthTool* m_richRecMCTruth;  ///< Pointer to RichRecMCTruthTool interface
  mutable const Rich::IRichTrackSelector * m_trSelector;  ///< Track selector

  // job options selection cuts
  std::vector<double> m_minBeta;           ///< minimum beta value for 'saturated' tracks
  std::vector<double> m_minP; ///< Min momentum per radiator
  std::vector<double> m_maxP; ///< Max momentum per radiator

  std::vector<unsigned int> m_truePhotCount; ///< Total number of true cherenkov photons per radiator
  std::vector<unsigned int> m_nSegs;         ///< Total number of track segments per radiator

  std::vector<double> m_chThetaRecHistoLimitMax; ///< Max theta limit for histos for each rad
  std::vector<double> m_chThetaRecHistoLimitMin; ///< Min theta limit for histos for each rad

};

#endif // RICHRECQC_RICHRECOQC_H
