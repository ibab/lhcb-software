// $Id: DBDrivenAnalysisTask.cpp,v 1.3 2008-04-30 13:28:54 ggiacomo Exp $
#include "OMAlib/DBDrivenAnalysisTask.h"
#include "OnlineHistDB/OnlineHistDB.h"
#include "OMAlib/OMAlib.h"
#include "OMAlib/OMAalg.h"
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
      if( Task != "any" &&
          Task != (*ih)->task() ) continue;
      debug() << "histogram with analysis found in DB: "<<(*ih)->identifier() <<endmsg;
      TH1* rooth = NULL;
      if ( (*ih)->isAnaHist() ) { //load sources and produce ROOT histograms
        rooth = onTheFlyHistogram( (*ih), f);
      }
      else {
        rooth = (TH1*) f->Get(((*ih)->hname()).c_str());
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
              OMAalg* thisalg=m_omalib->getAlg(anaAlgs[iana]);
              if (thisalg) {
                if (OMACheckAlg* cka = dynamic_cast<OMACheckAlg*>(thisalg) )
                  cka->exec(*(rooth),
                            warningThr,
                            alarmThr,
                            inputs);
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
      } // end check on root object existence
      else {
        err() << " histogram NOT FOUND in source"<<endmsg;
      }
    } // end loop on histograms to be analyzed
    f->Close();
  } // end check on file
  delete f;
  
  return StatusCode::SUCCESS;
}

TH1* DBDrivenAnalysisTask::onTheFlyHistogram(OnlineHistogram* h,
					     TFile* f)
{
  std::string Algorithm;
  std::vector<std::string> sourcenames;
  std::vector<std::string>::iterator is;
  std::vector<TH1*> sources;
  std::vector<float> parameters;
  TH1* out=m_ownedHisto[h->identifier()];
  
  h->getCreationDirections(Algorithm,sourcenames,parameters);
  verbose() << "  Got analysis histo made with algo "<<Algorithm <<
    " ,"<<sourcenames.size() << " sources and "<<parameters.size()
            << " parameters"<<endmsg;
  bool loadok=true;
  for (is= sourcenames.begin(); is != sourcenames.end() ; is++) {
    verbose() << "   trying to load " << (*is) << endmsg;
    TH1* hh=NULL;
    OnlineHistogram* dbhh=m_histDB->getHistogram(is->c_str());
    if(!dbhh) {
      err() <<"histogram "<<(*is)<<" unknown to HistDB "<<endmsg;
      loadok=false;
      break;
    }
    else {
      if( dbhh->isAnaHist() )
        hh = onTheFlyHistogram(m_histDB->getHistogram(is->c_str()), f);
      else
        hh =(TH1*) f->Get(is->c_str());
    }
    if (hh) {
      verbose() <<"OK: histogram "<<(*is)<<" loaded"<<endmsg;
      sources.push_back(hh);
    }
    else {
      err() <<"histogram "<<(*is)<<" NOT FOUND"<<endmsg;
      loadok=false;
      break;
    }
  }
  if(!loadok) {
    err()<<" ERROR loading sources"<<endmsg;
  }
  else {
    verbose() << "   calling creation algorithm ..."<<endmsg;
    
    OMAalg* thisalg=m_omalib->getAlg(Algorithm);
    
    if (thisalg) {
      if (OMAHcreatorAlg* hca = dynamic_cast<OMAHcreatorAlg*>(thisalg) ) {
        out = hca->exec(&sources, 
                        &parameters,
                        h->identifier(), 
                        h->hname(),
                        out);
        if (out) {
          m_ownedHisto[h->identifier()]=out;
        }
        else {
          err() <<" SOMETHING WRONG calling creator algorithm "<< Algorithm<< endmsg;
        }
      }
      else {
        err() <<"ERROR: "<<Algorithm<< " is not an Hcreator algorithm"<<endmsg;
      }
    }
    else {
      verbose()<<"ERROR: alg "<<Algorithm<< " apparently not implemented in OMAlib"<<endmsg;
    }
  }
  return out;
}
