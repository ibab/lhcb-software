
//---------------------------------------------------------------------------------
/** @file RichRecoQC.h
 *
 *  Header file for RICH reconstruction monitoring algorithm : RichRecoQC
 *
 *  CVS Log :-
 *  $Id: RichRecoQC.h,v 1.14 2006-08-12 10:53:32 jonrob Exp $
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
#include "GaudiKernel/AlgFactory.h"

// Interfaces
#include "RichRecBase/IRichRecMCTruthTool.h"

// temporary histogramming numbers
#include "RichRecBase/RichDetParams.h"

// rec helpers
#include "RichRecBase/RichTrackSelector.h"

// interfaces
#include "RichKernel/IRichParticleProperties.h"
#include "RichRecBase/IRichCherenkovAngle.h"
#include "RichRecBase/IRichCherenkovResolution.h"

// RichKernel
#include "RichKernel/RichStatDivFunctor.h"

// GaudiPI
//#include "AIDA_Proxy/AIDA_Proxy.h"

// boost
#include <boost/assign/list_of.hpp>

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

private: // data

  // Pointers to tool instances
  const IRichParticleProperties * m_richPartProp; ///< Rich Particle properties
  const IRichCherenkovAngle * m_ckAngle;  ///< Pointer to RichCherenkovAngle tool
  const IRichCherenkovResolution * m_ckRes; ///< Cherenkov angle resolution tool

  /// Pointer to RichRecMCTruthTool interface
  const IRichRecMCTruthTool* m_richRecMCTruth;

  // job options
  double m_minBeta;        ///< minimum beta value for 'saturated' tracks

  std::vector<unsigned int> m_truePhotCount; ///< Total number of true cherenkov photons per radiator
  std::vector<unsigned int> m_nSegs;         ///< Total number of track segments per radiator

  /// Track selector
  RichTrackSelector m_trSelector;

  bool m_useMCInfo;    ///< switch on/off MonteCarlo information

  std::vector<double> m_chThetaRecHistoLimitMax; ///< Max theta limit for histos for each rad
  std::vector<double> m_chThetaRecHistoLimitMin; ///< Min theta limit for histos for each rad

};

#endif // RICHRECQC_RICHRECOQC_H
