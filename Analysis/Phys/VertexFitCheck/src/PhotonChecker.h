// $Id: PhotonChecker.h,v 1.2 2007-10-17 07:27:55 jpalac Exp $
#ifndef PHOTONCHECKER_H
#define PHOTONCHECKER_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "Kernel/DaVinciTupleAlgorithm.h"

#include "Kernel/Particle2MCLinker.h"

using namespace LHCb;

/** @class PhotonChecker PhotonChecker.h
 *
 *  PhotonChecker is an algorithm to check
 *  the resolution and pull distributions
 *  of (x, y, E) of calorimeter clusters
 *  assocaited with photon particles
 *  using MC truth.
 *
 *  @author Yuehong Xie
 *  @date   2006-5-25
 */
class PhotonChecker : public DaVinciTupleAlgorithm
{

public:

  /// Standard constructor
  PhotonChecker( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~PhotonChecker( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private:

  class IsHypo : public std::unary_function<const CaloHypo*,bool>
  {
  public:
    /// constructor
    IsHypo( const CaloHypo::Hypothesis& hypo ): m_hypo ( hypo ) {};
      /// functor interface
      bool operator() ( const CaloHypo* hypo ) const
      { return 0 != hypo && m_hypo == hypo->hypothesis() ; }
  private:
      IsHypo();
  private:
      CaloHypo::Hypothesis m_hypo ;
  };

private:

  Particle2MCLinker* m_pLinker;
  std::string m_particlePath;

  StatusCode getPhotonParameter(const LHCb::Particle& part,
                                double& zg,
                                Gaudi::Vector3& para ,
                                Gaudi::SymMatrix3x3& cov ) const;


};
#endif
