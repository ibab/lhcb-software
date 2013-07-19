// $Id: $
#ifndef CHERENKOVANALYSIS_CHERENKOVG4HISTODEFINESET5_H 
#define CHERENKOVANALYSIS_CHERENKOVG4HISTODEFINESET5_H 1

// Include files
#include <string>
#include <cmath>
#include <vector>
#include "GaudiKernel/ISvcLocator.h"

// Forward declarations
class IHistogram1D;
class IHistogram2D;

/** @class CherenkovG4HistoDefineSet5 CherenkovG4HistoDefineSet5.h CherenkovAnalysis/CherenkovG4HistoDefineSet5.h
 *  
 *
 *  @author Sajan Easo
 *  @date   2011-04-15
 */
class CherenkovG4HistoDefineSet5 {
public: 
  /// Standard constructor
  CherenkovG4HistoDefineSet5( ); 

  virtual ~CherenkovG4HistoDefineSet5( ); ///< Destructor

  
protected:

private:

  void bookCherenkovG4HistogramsSet5();
  
  IHistogram1D*        m_NumTotHitRich1All;
  IHistogram1D*        m_NumTotHitRich2All;
  IHistogram1D*        m_NumTotHitRich2NoScintAll;
  
  IHistogram2D*        m_hXYHitLocationRich1Gas;
  IHistogram2D*        m_hXYHitLocationLensRich1Gas;
  IHistogram2D*        m_hXYHitLocationRich2Gas;
  IHistogram2D*        m_hXYHitLocationNoScintRich2Gas;

  IHistogram2D*        m_hXYHitLocationRich1GasNoDuplicate;
  IHistogram2D*        m_hXYHitLocationRich2GasNoDuplicate;

  IHistogram1D*   m_hPmtOccpRich1;
  IHistogram1D*   m_hPmtOccpRich2;
  IHistogram1D*   m_hPmtOccpNoScintRich2;

  IHistogram1D*   m_hPmtOccpRich1NoDuplicate;
  IHistogram1D*   m_hPmtOccpRich2NoDuplicate;

  IHistogram2D*       m_XYHitLocationRich1GasMirror1 ;
  IHistogram2D*       m_XYHitLocationRich1GasMirror2 ;

  IHistogram2D*      m_hXYRawHitLocationRich1Gas;
  IHistogram2D*      m_hXYHitLocationPMTQWRich1Gas;
  IHistogram2D*      m_hXYHitLocationPMTQWRich2Gas;
  IHistogram2D*      m_hXYHitLocationCF4Rich2Gas;
  
  IHistogram1D*      m_hPmtOccpGasQWRich1;
  IHistogram1D*      m_hPmtOccpPmtQWRich1;
  IHistogram1D*      m_hPmtOccpGasQWRich2;
  IHistogram1D*      m_hPmtOccpPmtQWRich2;
  


  std::string m_RichG4HistoPathSet5; ///< Histo path
  //  ISvcLocator* m_svcLocSet5;

};
#endif // CHERENKOVANALYSIS_CHERENKOVG4HISTODEFINESET5_H
