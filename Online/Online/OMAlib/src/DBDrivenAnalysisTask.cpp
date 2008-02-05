#include "OMAlib/DBDrivenAnalysisTask.h"
#include "OnlineHistDB/OnlineHistDB.h"
#include "OMAlib/OMAlib.h"
#include <TH1.h>
#include <TFile.h>

DBDrivenAnalysisTask::~DBDrivenAnalysisTask()
{
  std::map<std::string, TH1*>::iterator ih;
  for (ih=m_ownedHisto.begin(); ih != m_ownedHisto.end(); ++ih) {
    delete (*ih).second;
  }
}


bool DBDrivenAnalysisTask::exec(std::string SaveSet)
{
  TFile* f = new TFile(SaveSet.c_str(),"READ");
  std::vector<int> anaIDs;
  std::vector<std::string> anaAlgs;
  std::vector<float> warningThr; 
  std::vector<float> alarmThr;
  bool mask;
  if (false == f->IsZombie()) {
    std::vector<OnlineHistogram*> hlist;
    m_histDB->getHistogramsWithAnalysis( &hlist);
    
    for (unsigned int i=0;i<hlist.size();i++) {
      cout << "found histo with analysis: "<<hlist[i]->identifier() <<endl;
      TH1* rooth = NULL;
      if ( hlist[i]->isAnaHist() ) { //load sources and produce ROOT histograms
	rooth = onTheFlyHistogram( hlist[i], f);
      }
      else {
	rooth = (TH1*) f->Get((hlist[i]->hname()).c_str());
      }
      if (rooth) {
	cout <<" OK"<<endl;
	// get analysis options 
	cout << "getting getAnalyses for "<<hlist[i]->identifier()<<endl;
	hlist[i]->getAnalyses(anaIDs,anaAlgs);
	for (unsigned int iana=0 ; iana<anaIDs.size() ;  iana++) {
	  cout << "performing analysis "<<anaAlgs[iana]<<" on histo "<<hlist[i]->identifier()<<" ... ";
	  if (hlist[i]->getAnaSettings(anaIDs[iana], &warningThr, &alarmThr, mask) ) {
	    if (!mask) {
	      OMAalg* thisalg=m_omalib->getAlg(anaAlgs[iana]);
	      if (thisalg) {
		if (OMACheckAlg* cka = dynamic_cast<OMACheckAlg*>(thisalg) )
		  cka->exec(*(rooth),
			    warningThr,
			    alarmThr,
			    cout);
		else {
		  cout<<"ERROR: alg "<<anaAlgs[iana]<< " not a check algorithm"<<endl;
		}
	      }
	      else {
		cout<<"ERROR: alg "<<anaAlgs[iana]<< " apparently not implemented in OMAlib"<<endl;
	      }
	    }
	    else {
	      cout << "Analysis "<< anaAlgs[iana] <<
		" on histogram "<<hlist[i]->identifier()<<" is masked"<<endl;
	    }
	  }
	  else {
	    cout << " ERROR loading parameters"<<endl;
	  }
	} // end loop on analyses to be done on this histogram
      } // end check on root object existence
      else {
	cout <<" NOT FOUND"<<endl;
      }
    } // end loop on histograms to be analyzed
    f->Close();
  } // end check on file
  delete f;
  
  return true;
}

TH1* DBDrivenAnalysisTask::onTheFlyHistogram(OnlineHistogram* h,
					     TFile* f)
{
  std::string Algorithm;
  std::vector<std::string> sourcenames;
  std::vector<TH1*> sources;
  std::vector<float> parameters;
  TH1* out=m_ownedHisto[h->identifier()];
  
  h->getCreationDirections(Algorithm,sourcenames,parameters);
  cout << "  Got ana histo made with algo "<<Algorithm <<
    " ,"<<sourcenames.size() << " sources and "<<parameters.size()
       << " parameters"<<endl;
  bool loadok=true;
  for (unsigned int js=0;js<sourcenames.size();js++) {
    cout << "   trying to load "<<sourcenames[js]<<" ...";
    TH1* hh =(TH1*) f->Get(sourcenames[js].c_str());
    if (hh) {
      cout <<" OK"<<endl;
      sources.push_back(hh);
    }
    else {
      cout <<" NOT FOUND"<<endl;
      loadok=false;
      break;
    }
  }
  if(!loadok) {
    cout<<" ERROR loading sources"<<endl;
  }
  else {
    cout << "   calling creation algorithm ...";
	
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
	  cout <<" OK"<<endl;
	}
	else {
	  cout <<" SOMETHING WRONG"<<endl;
	}
      }
      else {
	cout<<"ERROR: alg "<<Algorithm<< " not an Hcreator algorithm"<<endl;
      }
    }
    else {
      cout<<"ERROR: alg "<<Algorithm<< " apparently not implemented in OMAlib"<<endl;
    }
  }
  return out;
}
