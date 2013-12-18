// $Id: RichG4HistoDefineSet4.h,v 1.1 2004-02-04 13:53:00 seaso Exp $
#ifndef RICHANALYSIS_RICHG4HISTODEFINESET4_H 
#define RICHANALYSIS_RICHG4HISTODEFINESET4_H 1

// Include files
#include <string>
#include <cmath>
#include <vector>
#include "GaudiKernel/ISvcLocator.h"

// Forward declarations
class IHistogram1D;
class IHistogram2D;
// class IProfile;


/** @class RichG4HistoDefineSet4 RichG4HistoDefineSet4.h RichAnalysis/RichG4HistoDefineSet4.h
 *  
 *
 *
 *  @author Sajan EASO
 *  @date   2003-09-22
 */
class RichG4HistoDefineSet4 {
public:
  /// Standard constructor
  RichG4HistoDefineSet4( ); 

  virtual ~RichG4HistoDefineSet4( ); ///< Destructor

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
  IHistogram1D* m_hCkvRich1GasD4E1;
  IHistogram1D* m_hCkvRich1GasD1E4;
  IHistogram1D* m_hCkvRich1GasD5E4;
  IHistogram1D*  m_hCkvRich1GasQw;
  IHistogram1D*  m_hCkvRich1GasQwPh;
   
  IHistogram1D* m_hCkvRich1GasGen;
  IHistogram1D* m_hCkvRich1AgelD3E3;
  IHistogram1D* m_hCkvRich1AgelD4E3;
  IHistogram1D* m_hCkvRich1AgelD1E4;
  IHistogram1D* m_hCkvRich1AgelD5E4;
  IHistogram1D* m_hCkvRich1AgelQw;
  IHistogram1D* m_hCkvRich1AgelQwPh;
  IHistogram1D* m_hCkvRich1AgelGen;
  IHistogram1D* m_hCkvRich2GasD3E1;
  IHistogram1D* m_hCkvRich2GasD4E1;
  IHistogram1D* m_hCkvRich2GasD1E4;
  IHistogram1D* m_hCkvRich2GasD5E4;
  IHistogram1D*  m_hCkvRich2GasQw;
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
  IHistogram1D*  m_hCkvRich2GasResEmisLimited;
  IHistogram1D*  m_hCkvRich1GasResPixel;
  IHistogram1D*  m_hCkvRich1AgelResPixel;
  IHistogram1D*  m_hCkvRich2GasResPixel;
  IHistogram1D*   m_hCkvRich1GasResPsf;
  IHistogram1D*   m_hCkvRich1AgelResPsf;
  IHistogram1D*   m_hCkvRich2GasResPsf;
  IHistogram1D*   m_hCkvRich1GasResTotal;
  IHistogram1D*   m_hCkvRich1AgelResTotal;
  IHistogram1D*   m_hCkvRich2GasResTotal;
  
  IHistogram1D*  m_hCkvRich1AgelExitResRefraction;

  

};
#endif // RICHANALYSIS_RICHG4HISTODEFINESET4_H
