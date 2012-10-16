#ifndef CHERENKOVANALYSIS_CHERENKOVG4HISTODEFINESET8_H 
#define CHERENKOVANALYSIS_CHERENKOVG4HISTODEFINESET8_H 1

// Include files
#include <string>
#include <cmath>
#include <vector>
#include "GaudiKernel/ISvcLocator.h"

// Forward declarations
class IHistogram1D;
class IHistogram2D;

/** @class CherenkovG4HistoDefineSet8 CherenkovG4HistoDefineSet8.h CherenkovAnalysis/CherenkovG4HistoDefineSet8.h
 *  
 *
 *  @author Sajan Easo
 *  @date   2012-10-08
 */
class CherenkovG4HistoDefineSet8 {
public: 
  /// Standard constructor
  CherenkovG4HistoDefineSet8( ); 

  virtual ~CherenkovG4HistoDefineSet8( ); ///< Destructor

protected:

private:
  void BookCkvStepAnalysisSet8Histo();
  

std::string m_CherenkovG4HistoHPathSet8; ///< Histo path
ISvcLocator* m_svcLocSet8;
  IHistogram2D* m_CkvHisto_XY_Rich1Mirror1;
  IHistogram2D* m_CkvHisto_XY_Rich1Mirror2;
  IHistogram2D* m_CkvHisto_XY_Rich1Mirror2LocalTop;
  IHistogram2D* m_CkvHisto_XY_Rich1Mirror2LocalBot;
  IHistogram2D* m_CkvHisto_XY_Rich1QW;
  IHistogram2D* m_CkvHisto_XY_Rich1QWLocalTop;  
  IHistogram2D* m_CkvHisto_XY_Rich1QWLocalBot;  
  IHistogram2D* m_CkvHisto_XY_Rich1PhSup;
  IHistogram2D* m_CkvHisto_XY_Rich1PhSupLocalTop;
  IHistogram2D* m_CkvHisto_XY_Rich1PhSupLocalBot;
  IHistogram2D*  m_CkvHisto_XY_Rich1ModulePlaneTop;
  IHistogram2D*  m_CkvHisto_XY_Rich1ModulePlaneBot;
 
};
#endif // CHERENKOVANALYSIS_CHERENKOVG4HISTODEFINESET7_H
