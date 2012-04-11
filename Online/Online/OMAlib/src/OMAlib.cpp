// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/OMAlib/src/OMAlib.cpp,v 1.27 2010-10-19 13:30:54 ggiacomo Exp $
/*
  Online Monitoring Analysis library
  G. Graziani (INFN Firenze)

*/
#include "OMAlib/OMAlib.h"
#include "OMAlib/OMAalg.h"
#include "OMAlib/OMAAlgorithms.h"
#include "OnlineHistDB/OnlineHistDB.h"
#include <TH1.h>
#include <TFile.h>
#include <cstdlib>


using namespace std;

// constructor to be used if already connected to HistDB (use NULL for not using HistDB)
OMAlib::OMAlib(OnlineHistDB* HistDB, 
               std::string Name) : 
  OMAMsgInterface(HistDB, Name), m_listSynced(false)
{ 
  m_localDBsession=false;
  checkWritePermissions();
  doFitFuncList();
  doAlgList(); 
}

// constructor opening a new DB session
OMAlib::OMAlib(std::string DBpasswd, 
               std::string DBuser , 
               std::string DB,
               std::string Name) : 
  OMAMsgInterface(new OnlineHistDB(DBpasswd , DBuser, DB), Name), m_listSynced(false)
{
  if(m_histDB) 
    m_localDBsession=true;
  doFitFuncList();
  doAlgList();   
  
}

// constructor with a new default (read-only) DB session
OMAlib::OMAlib(std::string Name) : 
  OMAMsgInterface(new OnlineHistDB(), Name), m_listSynced(false)
{
  if(m_histDB) m_localDBsession=true;
  doFitFuncList();
  doAlgList(); 
}

void OMAlib::openDBSession(std::string DBpasswd, 
                      std::string DBuser , 
                      std::string DB) {
  if (m_localDBsession) {
    delete m_histDB;
    m_histDB = NULL;
    m_localDBsession = false;
  }
  m_histDB = new OnlineHistDB(DBpasswd , DBuser, DB);
  if(m_histDB) {
    m_localDBsession=true;
    checkWritePermissions();
    if (false == m_msgInit) {
      loadMessages();
    }
    else {
      updateMessages();
    }
    if (m_histDB->canwrite() && false == m_listSynced)
      syncList();
  }
}

void OMAlib::closeDBSession(bool commit) {
  if(m_histDB) {
    if (commit)
      m_histDB->commit();
    delete m_histDB;
    m_histDB = NULL;
    m_localDBsession = false;
  }
}


OMAlib::~OMAlib() {
  std::map<std::string, OMAalg*>::iterator ia;
  for (ia=m_algorithms.begin(); ia != m_algorithms.end(); ++ia)
    delete (*ia).second ;
  std::map<std::string, OMAFitFunction*>::iterator i;
  for (i=m_fitfunctions.begin(); i != m_fitfunctions.end(); ++i)
    delete (*i).second ;
}


TH1* OMAlib::findRootHistogram(OnlineHistogram* h,
                               TFile* f,
                               TH1* existingHisto)
{
  TH1* out=NULL;
  std::string rootHname = h->rootName();
  if(false == h->isAnaHist()) {
    out =  (TH1*) f->Get(rootHname.c_str());
    if(!out) { // try w/o algorithm name (bkw compatibility)
      out= (TH1*) f->Get(h->hname().c_str());
    }
    if (out) {
      out->AddDirectory(kFALSE); // be sure the object stays in memory when closing the file
      out->SetName(rootHname.c_str()); // be sure the algorithm name is inside
    }
  }
  else { // analysis (virtual) histogram
    std::string Algorithm;
    std::vector<std::string> sourcenames;
    std::vector<std::string>::iterator is;
    std::vector<TH1*> sources;
    std::vector<float> parameters;
    out=existingHisto;
  
    h->getCreationDirections(Algorithm,sourcenames,parameters);
    bool loadok=true;
    OnlineHistogram* firsth=NULL;
    for (is= sourcenames.begin(); is != sourcenames.end() ; is++) {
      TH1* hh=NULL;
      OnlineHistogram* dbhh=m_histDB->getHistogram(is->c_str());
      if(!firsth) firsth=dbhh;
      if(!dbhh) {
        loadok=false;
        break;
      }
      else {
        hh = findRootHistogram(dbhh, f, NULL);
      }
      if (hh) {
        sources.push_back(hh);
      }
      else {
        loadok=false;
        break;
      }
    }
    if(loadok) {
      OMAalg* thisalg=getAlg(Algorithm);
      
      if (thisalg) {
        if (OMAHcreatorAlg* hca = dynamic_cast<OMAHcreatorAlg*>(thisalg) ) {
          TH1* ref=NULL;
          if (hca->needRef() && sources.size()>0) ref = getReference(firsth,1,"default",NULL);
          std::string htitle(h->htitle());
          out = hca->exec(&sources, 
                          &parameters,
                          rootHname, 
                          htitle,
                          out,
                          ref);
          // delete sources (and reference)
          for (std::vector<TH1*>::iterator ih= sources.begin() ; 
               ih != sources.end() ; ih++) {
            delete (*ih);
          }
          if (ref) delete ref;
        }
      }
    }
  }
  // set bin labels if required
  if (out) {
    std::string sopt;
    if (0 != out->GetXaxis()) {
      if (h->nXbinlabels() > 0) {
        for (unsigned int il = 0; il < h->nXbinlabels(); il++) {
          sopt = h->binlabel(il,0);
          out->GetXaxis()->SetBinLabel(il+1, sopt.c_str());
        }
      }
    }
    if (0 != out->GetYaxis()) {
      if (h->nYbinlabels() > 0) {
        for (unsigned int il = 0; il < h->nYbinlabels(); il++) {
          sopt = h->binlabel(il,1);
          out->GetYaxis()->SetBinLabel(il+1, sopt.c_str());
        }
      }
    }
  }
  return out;
}



TH1* OMAlib::getReference(OnlineHistogram* h,
                          int startrun,
                          std::string DataType,
                          TH1* existingHisto) {
  TH1* out=NULL;
  if(h) {
    std::stringstream refFile;
    std::string task= h->task();
    if(h->isAnaHist()) { // remove _ANALYSIS from task name
      task = task.substr(0,task.find("_ANALYSIS"));
      // note that all sources must be from the same task!!
    }
    refFile << refRoot() << "/"
            << task << "/" << DataType << "_"
            << startrun << ".root";
    TFile* f = new TFile(refFile.str().c_str(),"READ");
    if(f) {
      if (false == f->IsZombie() ) {        
        out = findRootHistogram(h, f, existingHisto);
        f->Close();
      }
      delete f;
    }
  }
  return out;
}


// initialize available algorithms and synchronize with DB algorithm list
void OMAlib::doAlgList() {
  m_algorithms["CheckXRange"] = new OMACheckXRange(this);
  m_algorithms["CheckMeanAndSigma"] = new OMACheckMeanAndSigma(this);
  m_algorithms["GaussFit"] = new OMAGaussFit(this);
  m_algorithms["CheckHolesAndSpikes"] = new OMACheckHolesAndSpikes(this);
  m_algorithms["CheckEmptyBins"] = new OMACheckEmptyBins(this);
  m_algorithms["CompareToReference"] = new OMACompareToReference(this);
  m_algorithms["CheckEntriesInRange"] = new OMACheckEntriesInRange(this);
  m_algorithms["Fit"] = new OMAFit(this);
  m_algorithms["IfbMonitor"] = new OMAIfbMonitor(this);
  m_algorithms["CheckDeadBins"] = new OMACheckDeadBins(this);
  m_algorithms["CheckErrorBins"] = new OMACheckErrorBins(this);
  m_algorithms["CheckMinMax"] = new OMACheckMinMax(this);

  m_algorithms["Efficiency"] = new OMAEfficiency(this);
  m_algorithms["Divide"] = new OMADivide(this);
  m_algorithms["HMerge"] = new OMAHMerge(this);
  m_algorithms["Scale"] = new OMAScale(this);
  m_algorithms["Add"] =  new OMAAdd(this);
  m_algorithms["Asymmetry"] =  new OMAAsymmetry(this);
  m_algorithms["Multiply"] =  new OMAMultiply(this);
  m_algorithms["Project"] =  new OMAProject(this);
  m_algorithms["Rebin"] =  new OMARebin(this);
  m_algorithms["DivideByReference"] = new OMADivideByReference(this);
  m_algorithms["SubtractReference"] = new OMASubtractReference(this);

  if (m_histDB) {
    if (m_histDB->canwrite() && false == m_listSynced)
      syncList();
  }
}

void OMAlib::doFitFuncList() {
  std::vector<std::string> ParNames;
  ParNames.push_back("Constant"); ParNames.push_back("Mean");ParNames.push_back("Sigma");
  m_fitfunctions["gaus"] = new
    OMAFitFunction("gaus",
                   "gaus",
                   ParNames,
                   false,
                   "gaussian function",
                   true);

  ParNames.clear();
  ParNames.push_back("Constant"); ParNames.push_back("Slope");
  m_fitfunctions["expo"] = new
    OMAFitFunction("expo",
                   "expo",
                   ParNames,
                   false,
                   "exponential function",
                   true);
  ParNames.clear();
  ParNames.push_back("Constant"); ParNames.push_back("MPV");ParNames.push_back("Sigma");
  m_fitfunctions["landau"] = new
    OMAFitFunction("landau",
                   "landau",
                   ParNames,
                   false,
                   "landau function",
                   true);

  // polynomials
  ParNames.clear();
  for (unsigned int degree=0 ; degree<10; degree++) {
    std::stringstream pname,fname,fdesc;
    pname << "P" << degree;
    fname << "pol" << degree;
    fdesc << "polynomium of degree "<< degree;
    ParNames.push_back(pname.str());
    m_fitfunctions[fname.str()] = new
      OMAFitFunction(fname.str(),
                     fname.str(),
                     ParNames,
                     false,
                     fdesc.str(),
                     true);
  }

  // special derived classes 
  OMAFitFunction * newff;
  // gauss + poly background
  for (unsigned int degree=0 ; degree<3; degree++) {
    newff = new OMAFitGausPlusBkg(degree);
    m_fitfunctions[newff->name()] = newff;
  }
  // double gaussian
  newff = new OMAFitDoubleGaus();
  m_fitfunctions[newff->name()] = newff;

  // custom functions
  OMAFitFunction * richfit = new OMAFitTH2withSinCosC();
  m_fitfunctions[richfit->name()] = richfit;
  OMAFitHLTjpsi * jpsifit = new OMAFitHLTjpsi();
  m_fitfunctions[jpsifit->name()] = jpsifit;

}



// check that all algorithms are declared to the DB
void OMAlib::syncList() {
  if (m_histDB) {
    int dbAlgListID = m_histDB->getAlgListID();
    if (dbAlgListID >= 0 &&
        dbAlgListID < OMAconstants::AlgListID) {
      // update DB
      bool ok = true;
      if (m_debug)
        cout << "Sending new algoritm List #"<<OMAconstants::AlgListID <<" to DB"<<endl;

      // list of algorithms
      std::map<std::string, OMAalg*>::iterator ialg;
      for (ialg=m_algorithms.begin(); ialg != m_algorithms.end(); ++ialg) {
        OMAalg* alg=(*ialg).second;
        bool algok = false;
        if (m_debug>1) 
          cout << "  sending algorithm "<<alg->name() <<" ... ";
        std::vector<std::string> parnames(alg->npars());
        std::vector<float> pardefval(alg->npars());
        for (int ip=0 ; ip<alg->npars(); ip++) {
          parnames[ip] = alg->parName(ip);
          pardefval[ip] = alg->parDefValue(ip);
        }
        if (OMAalg::HCREATOR == alg->type() ) {
          OMAHcreatorAlg* hcalg = dynamic_cast<OMAHcreatorAlg*> (alg);
          algok=m_histDB->declareCreatorAlgorithm
            ( hcalg->name(),
              hcalg->ninput(),
              hcalg->histSetFlag(),
              hcalg->outHType(),
              hcalg->npars(),
              &parnames,
              &pardefval,
              &(hcalg->doc()) );
        }
        else {
          OMACheckAlg* calg= dynamic_cast<OMACheckAlg*> (alg);
          std::vector<std::string> inputnames(calg->ninput());
          std::vector<float> inputdefv(calg->ninput());
          for (int ip=0 ; ip<calg->ninput(); ip++) {
            inputnames[ip] = calg->inputName(ip);
            inputdefv[ip] = calg->inputDefValue(ip);
          }
          algok=m_histDB->declareCheckAlgorithm
            (calg->name(),
             calg->npars(),
             &parnames,
             &pardefval,
             calg->ninput(),
             &inputnames,
             &inputdefv,
             &(calg->doc()) ); 
        }
        if (m_debug>1) cout << (algok ? " OK!" : " ERROR! ") <<endl;
        ok &= algok;
      }

      // now fit functions
      std::map<std::string, OMAFitFunction*>::iterator iff;
      for (iff=m_fitfunctions.begin(); iff != m_fitfunctions.end(); ++iff) {
        OMAFitFunction* ff=(*iff).second;
        bool fitok = false;
        if (m_debug>1) 
          cout << "  sending fit function "<<ff->name() <<" ... ";
        
        ff->checkDefValues();

        fitok=m_histDB->declareFitFunction
          (ff->name(),
           ff->np(), 
           &(ff->parNames()), 
           ff->mustInit(), 
           ff->doc(),
           &(ff->parDefValues()),
           &(ff->inputNames()),
           &(ff->inputDefValues())     );

        if (m_debug>1) cout << (fitok ? " OK!" : " ERROR! ") <<endl;
        ok &= fitok;
      }


      if (ok) {
        m_histDB->setAlgListID(OMAconstants::AlgListID);
        m_histDB->commit();
        m_listSynced = true;
      }
    }
    else if (dbAlgListID == OMAconstants::AlgListID) {
      m_listSynced = true;
    }
    else if (dbAlgListID > OMAconstants::AlgListID) {
      cout << " -- OMAlib WARNING: HistDB has an algorithm list more recent than your code --"<<endl;
      cout << " -- Please update to the latest OMAlib to profit of all its functionalities --"<<endl;
    }

    
  }
}



