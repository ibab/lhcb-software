// $Id: K0S_Creator.h,v 1.1 2004-08-05 09:54:15 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef LOKI_K0S_CREATOR_H 
#define LOKI_K0S_CREATOR_H 1
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/LoKi.h"
// ============================================================================

/** @class K0S_Creator K0S_Creator.h
 *
 *  "LoKi"-version of original Ks2PiPi algorithm by Yuehond XIE 
 *                                     from Phys/Ks2PiPiSel package 
 *     
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2004-07-24
 */
class K0S_Creator : public LoKi::Algo
{
  friend class AlgFactory<K0S_Creator> ;
  
public: 
  
  enum KsType 
    {
      Unknown = 0 ,
      LLKs        ,
      LVKs        , 
      DDKs        , 
      LDKs        
    };
  
public:
  /** standard initializationfo algorihtm 
   *  @return status code 
   */
  virtual StatusCode initialize () ;
  
  /** analyse one event 
   *  @return status code 
   */
  virtual StatusCode analyse    () ;
  
protected:
  
  /** Standard constructor
   *  @see LoKi::Algo
   *  @param name algorithm instance name 
   *  @param pSvc pointer to Service Locator 
   */
  K0S_Creator ( const std::string& name , 
                ISvcLocator*       pSvc ) ;
  
  /// destructor (virtual and protected)
  virtual ~K0S_Creator() {}; 
  
protected:
  
  /** check the track quality,
   *  indeed it is  LoKi-version of Ks2PiPi::trackQlt 
   */
  bool goodTrack ( const Particle* p    ) const ;
  /// helper function for K0S_Creator::goodTrack implementation 
  int  detFlag   ( const Particle* p    ) const ;
  /// check the consistensy the track types with Ks type 
  bool rightPair ( const Particle* pi1  , 
                   const Particle* pi2  , 
                   const KsType    k0s  ) const ;
  bool rightPair ( const Particle* pi1  , 
                   const Particle* pi2  ) const 
  { return rightPair ( pi1 , pi2 , m_KsType ) ; }
  
private:
  
  std::string m_KsTypeName;     ///< type of ks to select  

  double    m_ksmasswinti;      ///< Mass window for the two Pi - loose  
  double    m_ksmasswinloo;     ///< Mass window for the two Pi - tight
  double    m_ksZmin;           ///< Minimum z of Ksx Vtx
  double    m_ksZmax;           ///< Maximum z of Ksx Vtx
  double    m_chi2UncVtxCut;    ///< Maximum chi2 of Unconstrained Vtx fit
  double    m_ptCut;            ///< Minimum pt of Ks
  double    m_impParamPiCutL;   ///< Minimum IP significance of pions
  double    m_impParamPiCutV;   ///< Minimum IP significance of pions
  double    m_impParamPiCutD;   ///< Minimum IP significance of pions

  double    m_chi2Forward ; ///< chi2 cut for foward tracks
  double    m_chi2Match   ; ///< chi2 cut for match tracks
  double    m_deForward   ; ///< de cut for foward tracks without Rich
  double    m_pForward    ; ///< p cut for foward tracks without Rich and calo
  
  /// SdtHep mass for Ks
  double m_KsMass;

  // type of Ks 
  KsType    m_KsType ;
  
} ;

// ============================================================================
// The END
// ============================================================================
#endif // LOKI_K0S_CREATOR_H
// ============================================================================
