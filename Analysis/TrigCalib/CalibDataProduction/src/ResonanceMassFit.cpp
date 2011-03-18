// $Id: $
// Include files 

// local
#include "CalibDataProduction/ResonanceMassFit.h"

#define MIN_EVENTS 100
//-----------------------------------------------------------------------------
// Implementation file for class : ResonanceMassFit
//
// 2010-11-03 : Andrew Powell (LHCb)04
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ResonanceMassFit::ResonanceMassFit(  ) {

}

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ResonanceMassFit::ResonanceMassFit( const char *Name,
                                    const char *title,
                                    RooAddPdf* CombPDF,
                                    RooRealVar* SigYield,
                                    RooRealVar* BkgYield,
                                    TTree* Data,
                                    RooArgSet ParamList,
                                    int ParallelJobs) :
  TNamed(Name,title),
  m_ComPDF( CombPDF ),
  m_SigYield( SigYield ),
  m_BkgYield( BkgYield ),
  m_TTree( Data ),
  m_ParamList( ParamList )
{
  m_CPUCores = ParallelJobs;
  m_DataSet = NULL;
  m_MinEvents = MIN_EVENTS;
  m_SPlot = NULL;

}


//=============================================================================
// Destructor
//=============================================================================
ResonanceMassFit::~ResonanceMassFit() 
{
  cout<<"Entering desctructor"<<endl;
  
  if(m_SPlot)
  {
    cout<<"Deleted "<<m_SPlot->GetName()<<endl;
    delete m_SPlot;
  }
  else if(m_DataSet)
  {
    cout<<"Deleted "<<m_DataSet->GetName()<<endl;
    delete m_DataSet;
  }
} 

//=============================================================================
// MakeDataSet Method
//=============================================================================
void ResonanceMassFit::MakeDataSet()
{
  m_DataSet = new RooDataSet( GetName(), 
                              GetTitle(),
                              m_TTree,
                              m_ParamList );  
};

//=============================================================================
// PerformFit Method
//=============================================================================
void ResonanceMassFit::PerformFit()
{
  if(m_DataSet->numEntries()<m_MinEvents)
  {
    cout<<"Insufficient events to Fit: "<<m_DataSet->numEntries()<<endl;
    RooErrorHandler::softAbort();
  }

  cout << "Performing fit to " << m_DataSet->GetName() <<" data"<<endl;
  m_ComPDF->fitTo( *m_DataSet, 
                   Extended(), 
                   NumCPU(m_CPUCores), 
                   Timer(kTRUE) );

  //m_ComPDF->Print("v");
}

//=============================================================================
// Calculate sWeights Method
//=============================================================================
void ResonanceMassFit::CalcSWeights(const char* NewName)
{

  //----------------------------------------
  // First set fit parameters to be constant
  //----------------------------------------
  RooArgSet* FitParams  = m_ComPDF->getParameters(m_DataSet,kTRUE);
  TIterator* iter = FitParams->createIterator() ;
  RooRealVar* arg ;
  while( (arg = (RooRealVar*)iter->Next()) ) 
  {
    if( (*(arg->GetName()) != *(m_SigYield->GetName())) && 
        (*(arg->GetName()) != *(m_BkgYield->GetName())) )
    {
      cout<<"Setting "<<arg->GetName()<<" constant"<<endl;
      arg->setConstant();
    }
  }
  
  //----------------------------------------
  // Declare instance of RooStat::SPlot and 
  // merge sWeight values into m_DataSet. 
  // m_SPlot now takes ownership of m_DatSet
  // Therefore, only m_SPlot need be deleted
  // if it is non-NULL (see desctructor)
  //
  // NB. NewName only used if DataSet is 
  // cloned.
  //----------------------------------------
  m_SPlot = new RooStats::SPlot("m_sData",
                                "An SPlot",
                                *m_DataSet, 
                                m_ComPDF, 
                                RooArgList( *m_SigYield,
                                            *m_BkgYield ),
                                RooArgSet( ),
                                kTRUE,
                                kFALSE,
                                NewName
                                );

  //----------------------------------------
  // Add verification print out statements
  //----------------------------------------
  cout << "Check SWeights:" << endl;

  cout <<  "Yield of signal is " 
       << m_SigYield->getVal() << ".  From sWeights it is "
       << m_SPlot->GetYieldFromSWeight("peak_yield") << endl;
  
  cout << "Yield of background is " 
       << m_BkgYield->getVal() << ".  From sWeights it is "
       << m_SPlot->GetYieldFromSWeight("bkgd_yield") << endl
       << endl;

  //----------------------------------------
  // Set fit parameters back to non-constant
  //----------------------------------------
  iter = FitParams->createIterator() ;
  while( (arg = (RooRealVar*)iter->Next()) ) 
  {
    if( (*(arg->GetName()) != *(m_SigYield->GetName())) && 
        (*(arg->GetName()) != *(m_BkgYield->GetName())) )
    {
      cout<<"Setting "<<arg->GetName()<<" constant"<<endl;
      arg->setConstant(kFALSE);
    }
  }

  cout<<"Leaving CalcSWeight"<<endl;
  
}

//=============================================================================
// Get DataSet Method
//=============================================================================
RooDataSet* ResonanceMassFit::GetDataSet()
{
  if(m_SPlot)
    return m_SPlot->GetSDataSet();
  else
    return m_DataSet;
  
}

//=============================================================================
// SetMinEvents Method
//=============================================================================
void ResonanceMassFit::SetMinEvents(int minEvents)
{
  m_MinEvents = minEvents;
}

//=============================================================================
// Get TTree Method
//=============================================================================
TTree* ResonanceMassFit::GetTTree()
{
  return m_TTree;
}

//=============================================================================
// Get Params Method
//=============================================================================
RooArgSet* ResonanceMassFit::GetParamList()
{
  return &m_ParamList;
}
