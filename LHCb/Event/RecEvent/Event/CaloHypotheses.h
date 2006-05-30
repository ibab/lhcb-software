// $Id: CaloHypotheses.h,v 1.3 2006-05-30 08:43:03 cattanem Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
#ifndef RECEVENT_CALOHYPOTHESES_H 
#define RECEVENT_CALOHYPOTHESES_H 1

// Include files

/** @namespace CaloHypotheses 
 *  
 *  Namespace for definition of hypotheses for 
 *  calorimetry reconstruction/analysis 
 *
 *  @author Vany Belyaev Ivan.Belyaev@itep.ru
 *  @date   28/10/2001
 */
namespace LHCb
{
namespace CaloHypotheses 
{
  /// hypotheses
  enum Hypothesis
  {
    Undefined = 0          , ///< default value for hypothesis  
    Mip                    , ///< m.i.p. particle, charge undefined 
    MipPositive            , ///< positively charged m.i.p. particle
    MipNegative            , ///< negatively chargen m.i.p. particle
    Photon                 , ///< regular photon 
    PhotonFromMergedPi0    , ///< "photon" from merged pi0 
    BremmstrahlungPhoton   , ///< photon from e+/e- bremmstrahlung 
    Pi0Resolved            , ///< resolved pi0   (2 isolated photons)
    Pi0Overlapped          , ///< overlapped pi0 (2 partially overlaped)
    Pi0Merged              , ///< merged pi0     (1 cluster) 
    EmCharged              , ///< charged em particle with undefined charge 
    Positron               , ///< positively charged em particle 
    Electron               , ///< negatively charged em particle 
    EmChargedSeed          , ///< charged seed for tracking 
    PositronSeed           , ///< positively hcraged seed for tracking
    ElectronSeed           , ///< negatively charged seed tor tracking
    NeutralHadron          , ///< neutral hadron 
    ChargedHadron          , ///< charged hadron 
    PositiveHadron         , ///< positively charged hadron 
    NegativeHadron         , ///< negatively charged hadron 
    Jet                    , ///< the jet 
    Other                    ///< should be the last! 
  };
};
};

// ============================================================================
#endif ///< RECEVENT_CALOHYPOTHESES_H
// ============================================================================
