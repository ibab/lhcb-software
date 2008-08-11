// $Id: ExampleAnalysisTask.cpp,v 1.3 2008-08-11 08:05:16 ggiacomo Exp $
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "OMAlib/ExampleAnalysisTask.h"

//for using Histogram DB
#include "OnlineHistDB/OnlineHistDB.h"
#include "OnlineHistDB/OnlineHistogram.h"

// ROOT includes
#include <TFile.h>
#include <TH1.h>

DECLARE_ALGORITHM_FACTORY( ExampleAnalysisTask );

ExampleAnalysisTask::~ExampleAnalysisTask()
{
}

StatusCode ExampleAnalysisTask::initialize() 
{
  StatusCode sc = AnalysisTask::initialize(); 
  if ( sc.isFailure() ) return sc;  
  
  return StatusCode::SUCCESS;
}



StatusCode ExampleAnalysisTask::analyze(std::string& SaveSet,
                                        std::string Task)
{
  // open saveset (a ROOT file)
  TFile* f = new TFile(SaveSet.c_str(),"READ");

  if (false == f->IsZombie()) {
    // do what you want here

    // the following example gets the list of monitoring histograms from HistDB and checks 
    // their presence in the input file
    if (m_histDB) {
      std::vector<OnlineHistogram*> hlist;
      m_histDB->getHistogramsByTask( Task, &hlist );
      std::vector<OnlineHistogram*>::iterator ih;
      for( ih = hlist.begin() ; ih != hlist.end() ; ih++) {
        TH1* rooth =(TH1*) f->Get( ((*ih)->hname()).c_str() );
        if (! rooth) {
          warning() << "Histogram "<< (*ih)->hname() <<" missing in input file"
                    <<endmsg;
        }
      }
    }

    f->Close();
  } 
  delete f;
  
  return StatusCode::SUCCESS;
}

