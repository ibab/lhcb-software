// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/OMAlib/src/OMAlib.cpp,v 1.4 2008-04-30 13:28:54 ggiacomo Exp $
/*
  Online Monitoring Analysis library
  G. Graziani (INFN Firenze)

*/

#include "OMAlib/OMAlib.h"
#include "OMAlib/OMAAlgorithms.h"
using namespace std;

OMAlib::OMAlib(OnlineHistDB* HistDB) : m_histDB(HistDB), m_debug(2) { 
  m_localDBsession=false;
  doAlgList(); 
}
  
OMAlib::OMAlib(std::string DBpasswd, 
	       std::string DBuser , 
	       std::string DB) : m_debug(2) {
  m_histDB = new OnlineHistDB(DBpasswd , DBuser, DB);
  m_localDBsession=true;
  doAlgList(); 
}

OMAlib::OMAlib() : m_debug(2) {
  m_histDB = new OnlineHistDB();
  m_localDBsession=true;
}

OMAlib::~OMAlib() {
  std::map<std::string, OMAalg*>::iterator i;
  for (i=m_algorithms.begin(); i != m_algorithms.end(); ++i)
    delete (*i).second ;
  if (m_localDBsession)
    delete m_histDB;
}


void OMAlib::doAlgList() {
  m_algorithms["CheckXRange"] = new OMACheckXRange();
  m_algorithms["CheckMeanAndSigma"] = new OMACheckMeanAndSigma();
  m_algorithms["GaussFit"] = new OMAGaussFit();

  m_algorithms["Efficiency"] = new OMAEfficiency();
  m_algorithms["HMerge"] = new OMAHMerge();
  m_algorithms["Scale"] = new OMAScale();
  syncList();
}


void OMAlib::syncList() {
  if (m_histDB) {
    if (m_histDB->getAlgListID() >= 0 &&
        m_histDB->getAlgListID() < OMAconstants::AlgListID) {
      // update DB
      bool ok = true;
      if (m_debug)
        cout << "Sending new algoritm List #"<<OMAconstants::AlgListID <<" to DB"<<endl;
      std::map<std::string, OMAalg*>::iterator ialg;
      for (ialg=m_algorithms.begin(); ialg != m_algorithms.end(); ++ialg) {
        OMAalg* alg=(*ialg).second;
        bool algok;
        if (m_debug>1) 
          cout << "  sending algorithm "<<alg->name() <<" ... ";
        std::vector<std::string> parnames(alg->npars());
        for (int ip=0 ; ip<alg->npars(); ip++)
          parnames[ip] = alg->parName(ip);
        if ("HCREATOR" == alg->type() ) {
          OMAHcreatorAlg* hcalg = dynamic_cast<OMAHcreatorAlg*> (alg);
          algok=m_histDB->declareCreatorAlgorithm
            ( hcalg->name(),
              hcalg->ninput(),
              hcalg->histSetFlag(),
              hcalg->outHType(),
              hcalg->npars(),
              &parnames,
              hcalg->doc() );
        }
        else {
          OMACheckAlg* calg= dynamic_cast<OMACheckAlg*> (alg);
          std::vector<std::string> inputnames(calg->ninput());
          for (int ip=0 ; ip<calg->ninput(); ip++)
            inputnames[ip] = calg->inputName(ip);
          algok=m_histDB->declareCheckAlgorithm
            (calg->name(),
             calg->npars(),
             &parnames,
             calg->ninput(),
             &inputnames,
             calg->doc() ); 
          
          if (m_debug>1) cout << (algok ? " OK!" : " ERROR! ") <<endl;
          ok &= algok;
        }
      }
      if (ok) {
        m_histDB->setAlgListID(OMAconstants::AlgListID);
        m_histDB->commit();
      }
    }
  }
}
