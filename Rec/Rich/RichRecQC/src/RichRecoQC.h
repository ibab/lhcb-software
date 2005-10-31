
//---------------------------------------------------------------------------------
/** @file RichRecoQC.h
 *
 *  Header file for RICH reconstruction monitoring algorithm : RichRecoQC
 *
 *  CVS Log :-
 *  $Id: RichRecoQC.h,v 1.10 2005-10-31 13:31:39 jonrob Exp $
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

};

#endif // RICHRECQC_RICHRECOQC_H
