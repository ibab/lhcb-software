#ifndef MergedPi0ALG_H 
#define MergedPi0ALG_H 1
// Include files
// from STL
#include <string>
// from Gaudi
#include "DaVinciTools/DVAlgorithm.h"
// forward declarations 
class IPhotonFromMergedParams ;  // DaVinciTools 
class IPhysDesktop;
class IParticleStuffer;
class ProtoParticle ;  // PhysEvent 

/** @class MergedPi0Alg MergedPi0Alg.h
 *  Produces MergedPi0 
 *
 * @author Olivier Deschamps
 * @date 11/03/2003 
*/

class MergedPi0Alg : 
  public DVAlgorithm
{  
  /// friend factory for instantiation 
  friend class AlgFactory<MergedPi0Alg>; 

  
public:

  virtual StatusCode initialize ();  
  virtual StatusCode execute    ();  
  virtual StatusCode finalize   ();
  
  MergedPi0Alg
  ( const std::string&  name , 
    ISvcLocator*        pSvc );
  virtual ~MergedPi0Alg();
  
private:
  
  IPhysDesktop*       m_pDesktop;
  IParticleStuffer*   m_pStuffer;
  double              m_pi0MassWin;
  double              m_gPtCut;
  double              m_pi0PtCut;
  double              m_ggDistCut;
  

  bool                m_produceHistogram;
  IHistogram1D*       m_hMassPi0;
  int  m_nEvents;    
  int  m_pi0Count;   


  typedef std::vector<double> Parameters  ; 
  Parameters m_ParMas;

 
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
#endif  // MergedPi0ALG_H
// ============================================================================
