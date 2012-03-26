// $Id: $
#ifndef RICHANALYSIS_RICHG4HISTODEFINESET5_H 
#define RICHANALYSIS_RICHG4HISTODEFINESET5_H 1

// Include files
#include <string>
#include <cmath>
#include <vector>
#include "GaudiKernel/ISvcLocator.h"

/** @class RichG4HistoDefineSet5 RichG4HistoDefineSet5.h RichAnalysis/RichG4HistoDefineSet5.h
 *  
 *
 *  @author Sajan Easo
 *  @date   2012-03-18
 */

// Forward declarations
class IHistogram1D;
class IHistogram2D;

class RichG4HistoDefineSet5 {
public: 
  /// Standard constructor
  RichG4HistoDefineSet5( ); 

  virtual ~RichG4HistoDefineSet5( ); ///< Destructor


protected:

private:
  void bookRichG4HistogramsSet5();
  
  IHistogram1D*        m_NumTotHitHRich1All;
  IHistogram1D*        m_NumTotHitHRich2All;
  IHistogram1D*        m_NumTotHitHRich2NoScintAll;
  IHistogram1D*        m_NumTotHitHRich1AllA;
  IHistogram1D*        m_NumTotHitHRich2AllA;
  IHistogram1D*        m_NumTotHitHRich2NoScintAllA;

  IHistogram1D*        m_NumTotHitHRich1AllBinary;
  IHistogram1D*        m_NumTotHitHRich2AllBinary;
  IHistogram1D*        m_NumTotHitHRich2NoScintAllBinary;
  
  IHistogram2D*        m_hXYHitLocationHRich1Gas;
  IHistogram2D*        m_hXYHitLocationHRich2Gas;
  IHistogram2D*        m_hXYHitLocationHNoScintRich2Gas;

  IHistogram1D*   m_hPmtOccpHRich1;
  IHistogram1D*   m_hPmtOccpHRich2;
  IHistogram1D*   m_hPmtOccpHNoScintRich2;

  IHistogram2D*        m_hXYHitLocationHRich1GasBinary;
  IHistogram2D*        m_hXYHitLocationHRich2GasBinary;
  IHistogram2D*        m_hXYHitLocationHNoScintRich2GasBinary;

  IHistogram1D*   m_hPmtOccpHRich1Binary;
  IHistogram1D*   m_hPmtOccpHRich2Binary;
  IHistogram1D*   m_hPmtOccpHNoScintRich2Binary;
  
  std::string m_RichG4HistoHPathSet5; ///< Histo path
  
  
   

};
#endif // RICHANALYSIS_RICHG4HISTODEFINESET5_H
