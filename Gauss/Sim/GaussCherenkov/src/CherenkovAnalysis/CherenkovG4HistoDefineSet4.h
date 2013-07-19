// $Id: CherenkovG4HistoDefineSet4.h,v 1.1 2004-02-04 13:53:00 seaso Exp $
#ifndef RICHANALYSIS_CHERENKOVG4HISTODEFINESET4_H 
#define RICHANALYSIS_CHERENKOVG4HISTODEFINESET4_H 1

// Include files
#include <string>
#include <cmath>
#include <vector>
#include "GaudiKernel/ISvcLocator.h"

// Forward declarations
class IHistogram1D;
class IHistogram2D;
// class IProfile;


/** @class CherenkovG4HistoDefineSet4 CherenkovG4HistoDefineSet4.h RichAnalysis/CherenkovG4HistoDefineSet4.h
 *  
 *
 *
 *  @author Sajan EASO
 *  @date   2003-09-22
 */
class CherenkovG4HistoDefineSet4 {
public:
  /// Standard constructor
  CherenkovG4HistoDefineSet4( ); 

  virtual ~CherenkovG4HistoDefineSet4( ); ///< Destructor

protected:

private:
  // These data members are used in the execution of this algorithm
  // They are set in the initialisation phase by the job options service
  // for the RichG4RunAction.
  void bookRichG4HistogramsSet4();
  // Histograms ( used if m_produceHistogramSet4 = 1 (true) )

  std::string m_RichG4HistoPathSet4; ///< Histo path
  ISvcLocator* m_svcLocSet3;
  IHistogram1D* m_hCkvRich1GasD3E1;
  IHistogram1D* m_hCkvRich1GasD3E1A;
  IHistogram1D* m_hCkvRich1GasD4E1;
  IHistogram2D*  m_hCkvRich1GasD4E1VsPhi;
  

  IHistogram1D* m_hCkvRich1GasD1E4;
  IHistogram1D* m_hCkvRich1GasD1E4Rst;
  IHistogram1D*  m_hCkvRich1GasQwPh;
  IHistogram1D* m_hCkvRich2GasD8E4;
  IHistogram1D* m_hCkvRich1GasD8E4;


  IHistogram1D* m_hCkvRich1GasGen;
  IHistogram1D* m_hCkvRich1AgelD3E3;
  IHistogram1D* m_hCkvRich1AgelD1E4;
  IHistogram1D* m_hCkvRich1AgelGen;
  IHistogram1D* m_hCkvRich2GasD3E1;
  IHistogram1D* m_hCkvRich2GasD3E1A;

  IHistogram1D* m_hCkvRich2GasD4E1;
  IHistogram2D* m_hCkvRich2GasD4E1VsPhi;

  IHistogram1D* m_hCkvRich2GasD1E4;
  IHistogram1D* m_hCkvRich2GasD1E4Rst;

  IHistogram1D*  m_hCkvRich2GasQwPh;

  IHistogram1D* m_hCkvRich2GasGen;
  IHistogram1D* m_hCkvRich1GasRes;
  IHistogram1D* m_hCkvRich1GasResConst;
  IHistogram1D* m_hCkvRich1AgelRes;
  IHistogram1D* m_hCkvRich1AgelResConst;
  IHistogram1D* m_hCkvRich2GasRes;
  IHistogram1D* m_hCkvRich2GasResConst;
  IHistogram2D*  m_hCkvRich1GasCkvPhi;
  IHistogram2D*  m_hCkvRich1AgelCkvPhi;
  IHistogram2D*  m_hCkvRich2GasCkvPhi ;
  //  IProfile*  m_hCkvRich1GasCkvResPhiProf;
  // IProfile*  m_hCkvRich1AgelCkvResPhiProf;
  // IProfile*  m_hCkvRich2GasCkvResPhiProf ;
  IHistogram1D*  m_hCkvRich1GasResEmis;
  IHistogram1D*  m_hCkvRich1AgelResEmis;
  IHistogram1D*  m_hCkvRich2GasResEmis;
  IHistogram1D*  m_hCkvRich1GasResPixel;
  IHistogram1D*  m_hCkvRich1AgelResPixel;
  IHistogram1D*  m_hCkvRich2GasResPixel;
  IHistogram1D*   m_hCkvRich1GasResPsf;
  IHistogram1D*   m_hCkvRich1AgelResPsf;
  IHistogram1D*   m_hCkvRich2GasResPsf;
  IHistogram1D*   m_hCkvRich1GasResTotal;
  IHistogram1D*  m_hCkvRich1GasResChr;
  
  IHistogram1D*   m_hCkvRich1AgelResTotal;
  IHistogram1D*   m_hCkvRich2GasResTotal;
  IHistogram1D*   m_hCkvRich2GasResChr;
  
  IHistogram1D*  m_hCkvRich1AgelExitResRefraction;

  IHistogram1D* m_hFocalDistR1;
  IHistogram1D* m_hFocalDistR2;

  IHistogram1D* m_hCkvRich1GasD4E1Prim;
  IHistogram1D* m_hCkvRich1GasD4E1MinusGen;

  IHistogram1D* m_hCkvRich1GasD9E1MinusGen;

  IHistogram1D* m_hCkvRich1GasD4E4MinusGen;
  IHistogram1D* m_hCkvRich1GasD4E4MinusGenPrim;
  IHistogram1D*  m_hCkvRich1GasD4E4;

  IHistogram1D*  m_hCkvRich1GasD9E1MinusGenTFTS;


  IHistogram1D* m_hCkvRich1GasResEmisD4 ;
  IHistogram1D* m_hCkvRich1GasResEmisD4Prim;
  IHistogram1D* m_hCkvRich1GasResEmisD4Rst ;




  


};
#endif // RICHANALYSIS_RICHG4HISTODEFINESET4_H
