// $Id: ExampleAnalysisTask.cpp,v 1.9 2010-10-21 10:17:33 ggiacomo Exp $
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "OMAlib/ExampleAnalysisTask.h"

//for using Histogram DB
#include "OnlineHistDB/OnlineHistDB.h"
#include "OnlineHistDB/OnlineHistogram.h"

// ROOT includes
#include <TFile.h>
#include <TH1.h>
#include <TKey.h>

DECLARE_ALGORITHM_FACTORY( ExampleAnalysisTask )

ExampleAnalysisTask::~ExampleAnalysisTask()
{
}

StatusCode ExampleAnalysisTask::initialize() 
{
  // mandatory: call default initialize method 
  StatusCode sc = AnalysisTask::initialize(); 
  if ( sc.isFailure() ) return sc;  
  
  return StatusCode::SUCCESS;
}



StatusCode ExampleAnalysisTask::analyze(std::string& SaveSet,
                                        std::string Task)
{
  // mandatory: call default analyze method 
  StatusCode sc = AnalysisTask::analyze(SaveSet, Task);
  if ( sc.isFailure() ) return sc;  

  // open saveset (a ROOT file)
  TFile* f = new TFile(SaveSet.c_str(),"READ");

  if (false == f->IsZombie()) {
    // do what you want here

    // list histograms found on files
    info() << "Analyzing saveset "<<SaveSet<<endmsg;
    info() << "Histogram objects in saveset "<<SaveSet<<endmsg;
    TIter next1(f->GetListOfKeys());
    TKey* key;    
    while ((key = (TKey*)next1())) {
      info() << key->GetName() << "  class  "<<key->GetClassName()<<endmsg;
    }


    // the following example gets the list of monitoring histograms from HistDB and checks 
    // their presence in the input file
    if (m_histDB) {
      std::vector<OnlineHistogram*> hlist;
      m_histDB->getHistogramsByTask( Task, &hlist );
      std::vector<OnlineHistogram*>::iterator ih;
      for( ih = hlist.begin() ; ih != hlist.end() ; ih++) {
        TH1* rooth = findRootHistogram(*ih, f);
        if (! rooth) {
          warning() << "Histogram "<< (*ih)->hname() <<" missing in input file"
                    <<endmsg;
        }
        else {
           debug() <<"   histogram "<< (*ih)->hname() <<"found in input file"<<endmsg;
           delete rooth;
        }
      }
    }




    f->Close();
  } 
  delete f;
  
  return StatusCode::SUCCESS;
}

