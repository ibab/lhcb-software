#ifndef ResolvedPi0ALG_H 
#define ResolvedPi0ALG_H 1
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
 * @date 24/07/02/2002 
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
  
  ResolvedPi0Alg
  ( const std::string&  name , 
    ISvcLocator*        pSvc );
  virtual ~ResolvedPi0Alg();
  
private:
  IPhysDesktop*       m_pDesktop;
  IParticleStuffer*   m_pStuffer;
  double              m_pi0MassWin;
  double              m_gPtCut;
  
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
#endif  // ResolvedPi0ALG_H
// ============================================================================
