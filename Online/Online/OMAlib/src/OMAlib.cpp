// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/OMAlib/src/OMAlib.cpp,v 1.11 2009-02-16 10:38:22 ggiacomo Exp $
/*
  Online Monitoring Analysis library
  G. Graziani (INFN Firenze)

*/
#include "OnlineHistDB/OnlineHistDB.h"
#include "OMAlib/OMAlib.h"
#include "OMAlib/OMAAlgorithms.h"
using namespace std;

// constructor to be used if already connected to HistDB (use NULL for not using HistDB)
OMAlib::OMAlib(OnlineHistDB* HistDB, 
               std::string Name) : 
  OMAcommon(HistDB, Name) { 
  m_localDBsession=false;
  doFitFuncList();
  doAlgList(); 
}

// constructor opening a new DB session
OMAlib::OMAlib(std::string DBpasswd, 
               std::string DBuser , 
               std::string DB,
               std::string Name) : 
  OMAcommon(new OnlineHistDB(DBpasswd , DBuser, DB), Name) {
  if(m_histDB) 
    m_localDBsession=true;
  doFitFuncList();
  doAlgList();   
}

// constructor with a new default (read-only) DB session
OMAlib::OMAlib(std::string Name) : 
  OMAcommon(new OnlineHistDB(), Name) {
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
    loadMessages();
    if (m_histDB->canwrite())
      syncList();
  }
}


OMAlib::~OMAlib() {
  std::map<std::string, OMAalg*>::iterator i;
  for (i=m_algorithms.begin(); i != m_algorithms.end(); ++i)
    delete (*i).second ;
  if (m_localDBsession)
    delete m_histDB;
}

// initialize available algorithms and synchronize with DB algorithm list
void OMAlib::doAlgList() {
  OMAcommon* Env = (OMAcommon*) this;
  m_algorithms["CheckXRange"] = new OMACheckXRange(Env);
  m_algorithms["CheckMeanAndSigma"] = new OMACheckMeanAndSigma(Env);
  m_algorithms["GaussFit"] = new OMAGaussFit(Env);
  m_algorithms["CheckHolesAndSpikes"] = new OMACheckHolesAndSpikes(Env);
  m_algorithms["CheckEmptyBins"] = new OMACheckEmptyBins(Env);
  m_algorithms["CompareToReference"] = new OMACompareToReference(Env);
  m_algorithms["CheckEntriesInRange"] = new OMACheckEntriesInRange(Env);
  m_algorithms["Fit"] = new OMAFit(Env);

  m_algorithms["Efficiency"] = new OMAEfficiency(Env);
  m_algorithms["Divide"] = new OMADivide(Env);
  m_algorithms["HMerge"] = new OMAHMerge(Env);
  m_algorithms["Scale"] = new OMAScale(Env);

  if (m_histDB) {
    if (m_histDB->canwrite())
      syncList();
  }
}

// check that all algorithms are declared to the DB
void OMAlib::syncList() {
  if (m_histDB) {
    if (m_histDB->getAlgListID() >= 0 &&
        m_histDB->getAlgListID() < OMAconstants::AlgListID) {
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
        
        fitok=m_histDB->declareFitFunction
          (ff->name(),ff->np(), &(ff->parNames()), ff->mustInit(), ff->doc());

        if (m_debug>1) cout << (fitok ? " OK!" : " ERROR! ") <<endl;
        ok &= fitok;
      }


      if (ok) {
        m_histDB->setAlgListID(OMAconstants::AlgListID);
        m_histDB->commit();
      }
    }
  }
}


