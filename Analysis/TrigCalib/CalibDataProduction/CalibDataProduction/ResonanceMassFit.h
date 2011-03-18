// $Id: $
#ifndef CALIBDATAPRODUCTION_RESONANCEMASSFIT_H 
#define CALIBDATAPRODUCTION_RESONANCEMASSFIT_H 1

// Include files
#include "RooFit.h"
#include "RooGlobalFunc.h"
#include "RooRealVar.h"
#include "RooProdPdf.h"
#include "RooAddPdf.h"
#include "TTree.h"
#include "RooDataSet.h"
#include "RooArgList.h"
#include "TNamed.h"
#include "RooErrorHandler.h"
#include "RooStats/SPlot.h"

//ClassImp(ResonanceMassFit);

/** @class ResonanceMassFit ResonanceMassFit.h CalibDataProduction/ResonanceMassFit.h
 *  
 *
 *  @author Andrew Powell (LHCb)04
 *  @date   2010-10-27
 */

using namespace RooFit;
using namespace RooStats;

class ResonanceMassFit : public TNamed
{
public: 
  /// Standard constructor
  ResonanceMassFit( );
  ResonanceMassFit( const char *Name,
                    const char *title,
                    RooAddPdf* CombPDF,
                    RooRealVar* SigYield,
                    RooRealVar* BkgYield,
                    TTree* Data,
                    RooArgSet ParamList,
                    int ParallelJobs);
                   
  virtual ~ResonanceMassFit( ); ///< Destructor

  void MakeDataSet( );
  void PerformFit( );
  void CalcSWeights( const char *NewName );
  RooDataSet* GetDataSet( );
  void SetMinEvents( int minEvents );
  TTree* GetTTree( );
  RooArgSet* GetParamList( );

protected:

private:

  // Combined PDF
  RooAddPdf* m_ComPDF;
  
  // Yields
  RooRealVar* m_SigYield;
  RooRealVar* m_BkgYield;

  // TTree Input Data
  TTree* m_TTree;

  // RooDataSet (Unweighted)
  RooDataSet* m_DataSet;

  // RooStat::SPlot
  RooStats::SPlot* m_SPlot;

  // RooArgSet
  RooArgSet m_ParamList;

   // Number of parellel fit jobs
  int m_CPUCores;  

  // Min number of events allowed
  int m_MinEvents;
};
#endif // CALIBDATAPRODUCTION_RESONANCEMASSFIT_H
