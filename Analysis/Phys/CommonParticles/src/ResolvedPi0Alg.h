#ifndef RESOLVEDPI0ALG_H
#define RESOLVEDPI0ALG_H 1
// Include files
// from STL
#include <string>
// from Gaudi
#include "DaVinciTools/DVAlgorithm.h"
// forward declarations
class IPhysDesktop;
class IParticleStuffer;

/** @class ResolvedPi0Alg ResolvedPi0Alg.h
 *  Produces ResolvedPi0
 *
 * @author Frederic Machefert
 * @date 2002-07-24
*/

class ResolvedPi0Alg :
  public DVAlgorithm
{
  /// friend factory for instantiation
  friend class AlgFactory<ResolvedPi0Alg>;

public:

  virtual StatusCode initialize ();
  virtual StatusCode execute    ();
  virtual StatusCode finalize   ();

  bool               goodComb   (SmartRef<Particle>,SmartRef<Particle>);
  StatusCode         makePi0    (SmartRef<Particle>,SmartRef<Particle>);


  ResolvedPi0Alg
  ( const std::string&  name ,
    ISvcLocator*        pSvc );
  virtual ~ResolvedPi0Alg();

private:
  IPhysDesktop*       m_pDesktop;
  IParticleStuffer*   m_pStuffer;

  double              m_photonLhMin;
  bool                m_singlePhotonUse;
  double              m_pi0MassWin;
  double              m_gPtCut;
  double              m_pi0PtCut;

  bool                m_produceHistogram;
  IHistogram1D*       m_hMassPi0;

  int  m_nEvents;
  int  m_pi0Count;

  /// SdtHep ID for pi0
  long   m_pi0ID;
  /// SdtHep mass for pi0
  double m_pi0Mass;

  /// SdtHep ID for gamma
  long   m_gammaID;

 
};

// ============================================================================
// The END 
// ============================================================================
#endif  // RESOLVEDPI0ALG_H
// ============================================================================
