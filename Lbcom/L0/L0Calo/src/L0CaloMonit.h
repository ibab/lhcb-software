// $Id: L0CaloMonit.h,v 1.4 2008-03-10 19:40:07 robbep Exp $
#ifndef L0CALOMONIT_H 
#define L0CALOMONIT_H 1
// Include files

// from Gaudi 
#include "CaloUtils/Calo2Dview.h"

class IHistogram1D ;
class IHistogram2D ;

class DeCalorimeter ;

/** @class L0CaloMonit L0CaloMonit.h
 *  Monitoring algorithm for the L0 Calorimeter trigger
 *  Scans the L0CaloCandidate and fills histograms.
 *
 *  @author Olivier Callot
 *  @date   31/05/2001
 */
class L0CaloMonit: public Calo2Dview {
public:
  /// Standard constructor
  L0CaloMonit(const std::string& name, ISvcLocator* pSvcLocator );

  /// Standard destructor
  virtual ~L0CaloMonit( ); 

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:

  DeCalorimeter * m_ecal            ; ///< Pointer to Ecal detector element
  DeCalorimeter * m_hcal            ; ///< Pointer to Hcal detector element


  int m_nEvents ; 
  int m_nUsefulEvents ; 

  bool m_fullMonitoring ; 
  std::string              m_inputDataSuffix ;
  IHistogram1D*            m_histElectron      ;
  IHistogram1D*            m_histPhoton        ;
  IHistogram1D*            m_histHadron        ;
  IHistogram1D*            m_histPi0Local      ;
  IHistogram1D*            m_histPi0Global     ;
  IHistogram1D*            m_histSumEt         ;
  IHistogram1D*            m_histSumEtZoom     ;
  IHistogram1D*            m_histSpdMult       ;

  IHistogram2D*            m_histHadron2DOuter ;
  IHistogram2D*            m_histHadron2DInner ;

  IHistogram1D*            m_histHadronCrate22 ;
  IHistogram1D*            m_histHadronCrate23 ;
  IHistogram1D*            m_histHadronCrate24 ;
  IHistogram1D*            m_histHadronCrate25 ;

  IHistogram1D* m_histElectronCrate8 ; 
  IHistogram1D* m_histElectronCrate9 ; 
  IHistogram1D* m_histElectronCrate10 ; 
  IHistogram1D* m_histElectronCrate11 ; 
  IHistogram1D* m_histElectronCrate12 ; 
  IHistogram1D* m_histElectronCrate13 ; 
  IHistogram1D* m_histElectronCrate14 ; 
  IHistogram1D* m_histElectronCrate15 ; 
  IHistogram1D* m_histElectronCrate16 ; 
  IHistogram1D* m_histElectronCrate17 ; 
  IHistogram1D* m_histElectronCrate18 ; 
  IHistogram1D* m_histElectronCrate19 ; 
  IHistogram1D* m_histElectronCrate20 ; 
  IHistogram1D* m_histElectronCrate21 ; 
  IHistogram2D* m_histElectron2DOuter  ; 
  IHistogram2D* m_histElectron2DMiddle ; 
  IHistogram2D* m_histElectron2DInner  ; 

  IHistogram1D* m_histPhotonCrate8 ; 
  IHistogram1D* m_histPhotonCrate9 ; 
  IHistogram1D* m_histPhotonCrate10 ; 
  IHistogram1D* m_histPhotonCrate11 ; 
  IHistogram1D* m_histPhotonCrate12 ; 
  IHistogram1D* m_histPhotonCrate13 ; 
  IHistogram1D* m_histPhotonCrate14 ; 
  IHistogram1D* m_histPhotonCrate15 ; 
  IHistogram1D* m_histPhotonCrate16 ; 
  IHistogram1D* m_histPhotonCrate17 ; 
  IHistogram1D* m_histPhotonCrate18 ; 
  IHistogram1D* m_histPhotonCrate19 ; 
  IHistogram1D* m_histPhotonCrate20 ; 
  IHistogram1D* m_histPhotonCrate21 ; 
  IHistogram2D* m_histPhoton2DOuter  ; 
  IHistogram2D* m_histPhoton2DMiddle ; 
  IHistogram2D* m_histPhoton2DInner  ; 

  IHistogram1D* m_histPi0LocalCrate8 ; 
  IHistogram1D* m_histPi0LocalCrate9 ; 
  IHistogram1D* m_histPi0LocalCrate10 ; 
  IHistogram1D* m_histPi0LocalCrate11 ; 
  IHistogram1D* m_histPi0LocalCrate12 ; 
  IHistogram1D* m_histPi0LocalCrate13 ; 
  IHistogram1D* m_histPi0LocalCrate14 ; 
  IHistogram1D* m_histPi0LocalCrate15 ; 
  IHistogram1D* m_histPi0LocalCrate16 ; 
  IHistogram1D* m_histPi0LocalCrate17 ; 
  IHistogram1D* m_histPi0LocalCrate18 ; 
  IHistogram1D* m_histPi0LocalCrate19 ; 
  IHistogram1D* m_histPi0LocalCrate20 ; 
  IHistogram1D* m_histPi0LocalCrate21 ; 
  IHistogram2D* m_histPi0Local2DOuter  ; 
  IHistogram2D* m_histPi0Local2DMiddle ; 
  IHistogram2D* m_histPi0Local2DInner  ; 

  IHistogram1D* m_histPi0GlobalCrate8 ; 
  IHistogram1D* m_histPi0GlobalCrate9 ; 
  IHistogram1D* m_histPi0GlobalCrate10 ; 
  IHistogram1D* m_histPi0GlobalCrate11 ; 
  IHistogram1D* m_histPi0GlobalCrate12 ; 
  IHistogram1D* m_histPi0GlobalCrate13 ; 
  IHistogram1D* m_histPi0GlobalCrate14 ; 
  IHistogram1D* m_histPi0GlobalCrate15 ; 
  IHistogram1D* m_histPi0GlobalCrate16 ; 
  IHistogram1D* m_histPi0GlobalCrate17 ; 
  IHistogram1D* m_histPi0GlobalCrate18 ; 
  IHistogram1D* m_histPi0GlobalCrate19 ; 
  IHistogram1D* m_histPi0GlobalCrate20 ; 
  IHistogram1D* m_histPi0GlobalCrate21 ; 
  IHistogram2D* m_histPi0Global2DOuter  ; 
  IHistogram2D* m_histPi0Global2DMiddle ; 
  IHistogram2D* m_histPi0Global2DInner  ; 

  IHistogram1D*            m_histElectronFull      ;
  IHistogram1D*            m_histPhotonFull        ;
  IHistogram1D*            m_histHadronFull        ;
  IHistogram1D*            m_histPi0LocalFull      ;
  IHistogram1D*            m_histPi0GlobalFull     ;
  IHistogram1D*            m_histSumEtFull         ;
  IHistogram1D*            m_histSumEtZoomFull     ;
  IHistogram1D*            m_histSpdMultFull       ;
  IHistogram1D*            m_histHadronSlave1OutFull ;
  IHistogram1D*            m_histHadronSlave2OutFull ;
  IHistogram1D*            m_histHadronSlave1InFull ;
  IHistogram1D*            m_histHadronSlave2InFull ;
  IHistogram1D*            m_histSumEtSlave1OutFull ;
  IHistogram1D*            m_histSumEtSlave2OutFull ;
  IHistogram1D*            m_histSumEtSlave1InFull ;
  IHistogram1D*            m_histSumEtSlave2InFull ;

};
#endif // L0CALOMONIT_H
