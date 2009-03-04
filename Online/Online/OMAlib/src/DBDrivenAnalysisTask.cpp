// $Id: DBDrivenAnalysisTask.cpp,v 1.10 2009-03-04 09:46:15 ggiacomo Exp $
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "OMAlib/DBDrivenAnalysisTask.h"
#include "OnlineHistDB/OnlineHistDB.h"
#include <TH1.h>
#include <TFile.h>

DECLARE_ALGORITHM_FACTORY( DBDrivenAnalysisTask );

DBDrivenAnalysisTask::~DBDrivenAnalysisTask()
{
  std::map<std::string, TH1*>::iterator ih;
  for (ih=m_ownedHisto.begin(); ih != m_ownedHisto.end(); ++ih) {
    delete (*ih).second;
  }
}

StatusCode DBDrivenAnalysisTask::initialize() 
{
  StatusCode sc = AnalysisTask::initialize(); 
  if ( sc.isFailure() ) return sc;  
  
  return StatusCode::SUCCESS;
}



StatusCode DBDrivenAnalysisTask::analyze(std::string& SaveSet,
                                         std::string Task)
{
  AnalysisTask::analyze(SaveSet, Task);

  TFile* f = new TFile(SaveSet.c_str(),"READ");
  std::vector<int> anaIDs;
  std::vector<std::string> anaAlgs;
  std::vector<float> warningThr; 
  std::vector<float> alarmThr;
  std::vector<float> inputs;
  bool mask;
  if (false == f->IsZombie()) {
    std::vector<OnlineHistogram*> hlist;
    m_histDB->getHistogramsWithAnalysis( &hlist);
    std::vector<OnlineHistogram*>::iterator ih;
    for(ih = hlist.begin(); ih != hlist.end(); ih++) {
      if (Task == "any") {
        m_taskname = Task = (*ih)->task();
      }
      if( Task != (*ih)->task() ) continue;
      debug() << "histogram with analysis found in DB: "<<(*ih)->identifier() <<
        "  virtual="<<  (*ih)->isAnaHist() <<endmsg;
      TH1* rooth = findRootHistogram(*ih, f ,m_ownedHisto[(*ih)->identifier()]);
      if ( (*ih)->isAnaHist() ) { // keep track for later use
        m_ownedHisto[(*ih)->identifier()] = rooth;
      }
      
      if (rooth) {
        debug() <<"   histogram found in source"<<endmsg;
        (*ih)->getAnalyses(anaIDs,anaAlgs);
        for (unsigned int iana=0 ; iana<anaIDs.size() ;  iana++) {
          debug() << "performing analysis "<<anaAlgs[iana]<<
            " on histo "<<(*ih)->identifier()<<" ... "<<endmsg;
          warningThr.clear();
          alarmThr.clear();
          inputs.clear();
          if ((*ih)->getAnaSettings(anaIDs[iana], &warningThr, &alarmThr, &inputs, mask) ) {
            if (!mask) {
              OMAalg* thisalg= getAlg(anaAlgs[iana]);
              if (thisalg) {
                if (OMACheckAlg* cka = dynamic_cast<OMACheckAlg*>(thisalg) ) {
                  cka->exec(*(rooth),
                            warningThr,
                            alarmThr,
                            inputs,
                            anaIDs[iana],
                            thisalg->needRef() ? getReference((*ih)) : NULL);
                }
                else {
                  err()<<"algorithm "<<anaAlgs[iana]<< " is not a check algorithm"<<endmsg;
                }
              }
              else {
                err()<<"algorithm "<<anaAlgs[iana]<< " apparently not implemented in OMAlib"<<endmsg;
              }
            }
            else {
              info() << "Analysis "<< anaAlgs[iana] <<
                " on histogram "<<(*ih)->identifier()<<" is masked"<<endmsg;
            }
          }
          else {
            err() << " error loading analysis parameters"<<endmsg;
          }
        } // end loop on analyses to be done on this histogram
        if ( false == (*ih)->isAnaHist() ) delete rooth;
      } // end check on root object existence
      else {
        err() << " histogram "<< (*ih)->hname() << " NOT FOUND in source file"<<endmsg;
      }
    } // end loop on histograms to be analyzed
    f->Close();
  } // end check on file
  delete f;
  
  return StatusCode::SUCCESS;
}

