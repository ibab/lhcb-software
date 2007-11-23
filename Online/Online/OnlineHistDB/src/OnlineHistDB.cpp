// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/OnlineHistDB/src/OnlineHistDB.cpp,v 1.21 2007-11-23 17:58:56 ggiacomo Exp $
/*
   C++ interface to the Online Monitoring Histogram DB
   G. Graziani (INFN Firenze)

*/

#include "OnlineHistDB/OnlineHistDB.h"
using namespace OnlineHistDBEnv_constants;
OnlineHistDB::OnlineHistDB(std::string passwd,
			   std::string user,
			   std::string db) : 
  OnlineHistDBEnv(user),
  OnlineTaskStorage(), 
  OnlineHistogramStorage(),
  OnlinePageStorage(),
  m_AlgListID(-1), m_nit(0)
{
  setTaskEnv((OnlineHistDBEnv*) this);
  setHistEnv((OnlineHistDBEnv*) this);
  setPageEnv((OnlineHistDBEnv*) this);

  // initialize Oracle session and log in
  m_StmtMethod = "OnlineHistDB::OnlineHistDB";
  checkerr( OCIEnvCreate ((OCIEnv **)&m_envhp, (ub4)OCI_OBJECT , (dvoid *)0,
			  (dvoid * (*) (dvoid *, size_t))0, (dvoid * (*)(dvoid *, dvoid *, size_t))0,
			  (void (*)(dvoid *, dvoid *))0, (size_t)0, (dvoid **)0 ),
	    SEVERE);

  checkerr( OCIHandleAlloc((dvoid *) m_envhp, (dvoid **) &m_errhp, OCI_HTYPE_ERROR,
			   (size_t) 0, (dvoid **) 0),
	    SEVERE);

  checkerr( OCILogon2(m_envhp, 
		      m_errhp, 
		      &m_svchp, 
		      (text *) user.data(), 
		      (ub4) user.length(), 
		      (text *) passwd.data(),
		      (ub4) passwd.length(), 
		      (text *) db.data(),
		      (ub4) db.length(),
		      OCI_LOGON2_STMTCACHE),
	    SEVERE);

  // needed initialization for OCI interface
  getOCITypes();
  m_TaggedStatement = new std::set<std::string>;

  // set up object cache to avoid object duplication
  m_TStorage = (OnlineTaskStorage*) this;
  m_HStorage = (OnlineHistogramStorage*) this;
  m_PStorage = (OnlinePageStorage*) this;

  // check that this API version is in sync with DB current schema
  OCIStmt *stmt=NULL;
  int schema = DBschema;
  m_StmtMethod = "OnlineHistDB::OnlineHistDB";
  if ( OCI_SUCCESS == prepareOCIStatement(stmt, "BEGIN ONLINEHISTDB.CHECKSCHEMA(:x1,:x2, :x3); END;") ) {
    if ( OCI_SUCCESS == myOCIBindInt(stmt, ":x1", schema) &&
	 OCI_SUCCESS == myOCIBindInt(stmt, ":x2", m_AlgListID) && 
	 OCI_SUCCESS == myOCIBindInt(stmt, ":x3", m_canwrite)) {
      myOCIStmtExecute(stmt, SEVERE);
    }
  } 

  // get total entry counts
  m_nHistograms = m_nPages = m_nPageFolders = 0;
  OCIStmt *stmt2=NULL;
  if ( OCI_SUCCESS == prepareOCIStatement(stmt2, "BEGIN ONLINEHISTDB.GETTOTALCOUNTS(:x1,:x2,:x3); END;") ) {
    if ( OCI_SUCCESS == myOCIBindInt(stmt2, ":x1", m_nHistograms) &&
	 OCI_SUCCESS == myOCIBindInt(stmt2, ":x2", m_nPages) &&
	 OCI_SUCCESS == myOCIBindInt(stmt2, ":x3", m_nPageFolders)) {
      myOCIStmtExecute(stmt2);
    }
  } 
}

OnlineHistDB::~OnlineHistDB () {  
  // a rollback is needed, otherwise all changes are committed at termination
  //m_conn->rollback();
  OCICacheFree ( m_envhp, m_errhp, m_svchp);
  if (m_envhp)
    OCIHandleFree((dvoid *) m_envhp, OCI_HTYPE_ENV);

}

bool OnlineHistDB::commit() {
  bool out=false;
  
  OCIStmt *stmt=NULL;
  int schema = DBschema;
  if ( OCI_SUCCESS == prepareOCIStatement(stmt, "BEGIN ONLINEHISTDB.MYCOMMIT(:x1); END;") ) {
    if ( OCI_SUCCESS == myOCIBindInt(stmt, ":x1", schema) ) {
      m_StmtMethod = "OnlineHistDB::commit";
      if (out = (OCI_SUCCESS == myOCIStmtExecute(stmt, SEVERE))) {
	if (debug()>2) cout << "session terminated"<<endl; 
      }
    }
  }
  return out;
}


bool OnlineHistDB::declareSubSystem(std::string SubSys)
{
  bool out=true;
  OCIStmt *stmt=NULL;
  m_StmtMethod = "OnlineHistDB::declareSubSystem";
  if( OCI_SUCCESS == prepareOCIStatement(stmt, "BEGIN ONLINEHISTDB.DECLARESUBSYSTEM(:x1); END;") ) {
    if ( OCI_SUCCESS == myOCIBindString(stmt, ":x1", SubSys) ) {
      out = (OCI_SUCCESS == myOCIStmtExecute(stmt));
    }
  }
  return out;
}

void OnlineHistDB::declareHistByServiceName(const std::string &ServiceName)
{
  declareHistogram(ServiceName,"","_BYSN_",H1D);
}

void OnlineHistDB::declareHistogram(std::string TaskName,
				    std::string AlgorithmName,
				    std::string Title,
				    OnlineHistDBEnv::HistType Type)
{
  OCIStmt *stmt;
  m_StmtMethod = "OnlineHistDB::declareHistogram";

  std::string command = "BEGIN ONLINEHISTDB.DECLAREHISTOGRAM(:x1,:x2,:x3,:x4); END;";
  if (OCI_SUCCESS == prepareOCITaggedStatement(stmt, command.c_str(), "HDEC") ) {
    std::string myType( HistTypeName[(int)Type] );
    myOCIBindString(stmt, ":x1", TaskName);
    myOCIBindString(stmt, ":x2", AlgorithmName);
    myOCIBindString(stmt, ":x3", Title);
    myOCIBindString(stmt, ":x4", myType);
  
    if (OCI_SUCCESS == myOCIStmtExecute(stmt)) {
      if (debug() >2) cout <<" Declared Histo: " << TaskName <<"/"<<AlgorithmName<<"/"<< Title<<endl;
    }
    releaseOCITaggedStatement(stmt, "HDEC");
  }
}


OnlineHistogram* OnlineHistDB::declareAnalysisHistogram(std::string Algorithm,
							std::string Title,
							std::vector<OnlineHistogram*> &Sources,
							std::vector<float>* Parameters)
{
  bool ok=true;
  OnlineHistogram* outh=0;
  int nin;
  int np=getAlgorithmNpar(Algorithm,&nin);
  if (np<0 || (nin>0 && (int) Sources.size() < nin) || (nin==0 && Sources.size()<1)) ok=false;
  if (np>0) {
    if (Parameters == NULL) 
      ok=false;
    else
      if(Parameters->size() != (unsigned int)np) ok=false;
  }
  if (!ok) {
    cout << "something wrong in your call to OnlineHistDB::declareAnalysisHistogram("<<
      Algorithm <<"," << Title <<"), aborting..." <<endl;
  }
  else {
    std::stringstream command;
    command << "BEGIN ONLINEHISTDB.DECLAREANALYSISHISTOGRAM('"
	    << Algorithm << "','" 
	    << Title << "',";
    if (nin == 0) { // input is a set
      command << Sources[0]->hsid() << ",ONLINEHISTDB.SOURCEH()";
    }
    else { // input is a list of histograms
      command <<"0,ONLINEHISTDB.SOURCEH(";
      for (int ish=0 ; ish < nin ; ish++) {
	command << "'" << Sources[ish]->hid() << "'";
	if (ish < nin-1) command << ",";
      }
      command << ")";
    }
    command << ",THRESHOLDS(";
    if (np > 0) {
      for (int ipp=0; ipp<np ;ipp++) {
	command << Parameters->at(ipp);
	if (ipp < np-1) command << ",";   
      }
    }
    command << ")";
    command <<",:name); END;";
    

    m_StmtMethod = "OnlineHistDB::declareAnalysisHistogram";
    OCIStmt *stmt=NULL;
    if ( OCI_SUCCESS == prepareOCIStatement(stmt, command.str().c_str()) ) {
      text name[VSIZE_SN]="";
      myOCIBindString(stmt,":name", name, VSIZE_SN);
      if ( OCI_SUCCESS == myOCIStmtExecute(stmt) ) {
	std::string Name= std::string((const char *) name);
	outh=getHistogram(Name);
      }
    }
  }
  return outh;
}


bool OnlineHistDB::declareCheckAlgorithm(std::string Name, 
					 int Npars, 
					 std::vector<std::string> *pars,
					 std::string doc) {
  bool out=true;
  if(Npars>0) {
    if (!pars) { out=false; }
    else {
      if ((int) pars->size() != Npars)
	out=false;
    }
  }
  if (out) {
    out=false;
    std::stringstream statement;
    statement << "BEGIN ONLINEHISTDB.DECLARECHECKALGORITHM(:x1,:par";
    if (doc != "NONE")
      statement << ",:d";
    statement << "); END;";
    
    m_StmtMethod = "OnlineHistDB::declareCheckAlgorithm";
    OCIStmt *stmt=NULL;
    
    if ( OCI_SUCCESS == prepareOCIStatement(stmt, statement.str().c_str()) ) {
      myOCIBindString(stmt, ":x1", Name);
      OCITable *parameters;
      checkerr( OCIObjectNew ( m_envhp, m_errhp, m_svchp, OCI_TYPECODE_TABLE,
			       OCIparameters, (dvoid *) 0, OCI_DURATION_SESSION, TRUE,
			       (dvoid **) &parameters));
      if (Npars>0)
	stringVectorToVarray(*pars, parameters);
      myOCIBindObject(stmt, ":par",(void **) &parameters , OCIparameters);
      if (doc != "NONE") 
	myOCIBindString(stmt, ":d", doc);
      out = (OCI_SUCCESS == myOCIStmtExecute(stmt));
      checkerr(OCIObjectFree ( m_envhp, m_errhp, parameters, OCI_OBJECTFREE_FORCE) );
    }
  }
  return out;
}
bool OnlineHistDB::declareCreatorAlgorithm(std::string Name, 
					   int Ninput, 
					   OnlineHistDBEnv::HistType OutputType,
					   int Npars, 
					   std::vector<std::string> *pars, 
					   std::string doc) {
  bool out=false;
  std::stringstream statement;
  statement << "BEGIN ONLINEHISTDB.DECLARECREATORALGORITHM(:x1,:x2,PARAMETERS(";
  int ipar=0;
  while (ipar++<Npars) {
    statement << ":p" << ipar;
    if(ipar<Npars) statement << ",";
  } 
  statement << "),:type";
  if (doc != "NONE")
    statement << ",:d";
  statement << "); END;";

  m_StmtMethod = "OnlineHistDB::declareCreatorAlgorithm";
  OCIStmt *stmt=NULL;
  if ( OCI_SUCCESS == prepareOCIStatement(stmt, statement.str().c_str()) ) {
    myOCIBindString(stmt, ":x1", Name);
    myOCIBindInt   (stmt, ":x2", Ninput);
    ipar=0;
    while (ipar++<Npars) {
      std::stringstream pname;
      pname << ":p" << ipar;
      myOCIBindString(stmt, pname.str().c_str(), pars->at(ipar) );
    }
    std::string myType( HistTypeName[(int)OutputType] );
    myOCIBindString(stmt,":type",myType);
    if (doc != "NONE") {
      myOCIBindString(stmt, ":d", doc);
    }
    out = (OCI_SUCCESS == myOCIStmtExecute(stmt));
  }
  return out;
}

bool OnlineHistDB::removePageFolder(std::string Folder) {
  bool out=false;
  int iout=0;
  // check that there are no pages on folder
  m_StmtMethod = "OnlineHistDB::removePageFolder";
  OCIStmt *stmt=NULL;
  if ( OCI_SUCCESS == prepareOCIStatement(stmt, "begin :out := ONLINEHISTDB.DELETEPAGEFOLDER(:f)") ) {
    myOCIBindInt   (stmt, ":out", iout);
    myOCIBindString(stmt, ":f", Folder);
    if (OCI_SUCCESS == myOCIStmtExecute(stmt)) {
      out = (iout>0);
    }
  }
  return out;
}

void OnlineHistDB::refresh() {
  reloadHistograms();
  reloadPages();
  reloadTasks();
}


bool OnlineHistDB::setAlgListID(int algListID) {
  bool out=false;
  m_StmtMethod = "OnlineHistDB::setAlgListID";
  OCIStmt *stmt=NULL;
  if (OCI_SUCCESS == prepareOCIStatement(stmt, "UPDATE ERGOSUM SET ALGLIST=:1") ) {
    myOCIBindInt(stmt, ":1", algListID);
    if (OCI_SUCCESS == myOCIStmtExecute(stmt)) {
      out = true;
    }
  }
  return out;
}


int OnlineHistDB::getAllHistograms(std::vector<OnlineHistogram*>* list ,
				   std::vector<string>* ids,
				   std::vector<string>* types) {
  m_StmtMethod = "OnlineHistDB::getAllHistograms";
  return getHistograms("",list,ids,types);
}

int OnlineHistDB::getHistogramsWithAnalysis(std::vector<OnlineHistogram*>* list ,
					    std::vector<string>* ids,
					    std::vector<string>* types)
{
  m_StmtMethod = "OnlineHistDB::getHistogramsWithAnalysis";
  return getHistograms(",HISTOGRAMSET HS WHERE VH.HSID=HS.HSID AND HS.NANALYSIS>0",
		       list,ids,types);
}

int OnlineHistDB::getAnalysisHistograms(std::vector<OnlineHistogram*>* list ,
					std::vector<string>* ids,
					std::vector<string>* types)
{
  m_StmtMethod = "OnlineHistDB::getAnalysisHistograms";
  return getHistograms(",HISTOGRAM H WHERE VH.HID=H.HID AND H.ISANALYSISHIST=1",
		       list,ids,types);
}
int OnlineHistDB::getHistogramsBySubsystem(std::string SubSys,
					   std::vector<OnlineHistogram*>* list ,
					   std::vector<string>* ids,
					   std::vector<string>* types)
{  
  m_StmtMethod = "OnlineHistDB::getHistogramsBySubsystem";
  std::stringstream ss;
  ss << ", TASK T WHERE VH.TASK=T.TASKNAME AND (T.SUBSYS1 ='" << SubSys <<
    "' OR  T.SUBSYS2 ='"  << SubSys << "' OR  T.SUBSYS3 ='" << SubSys <<
    "')";
  return getHistograms( ss.str() , list, ids,types);
}

int OnlineHistDB::getHistogramsByTask(std::string Task,
				      std::vector<OnlineHistogram*>* list ,
				      std::vector<string>* ids,
				      std::vector<string>* types)
{  
  m_StmtMethod = "OnlineHistDB::getHistogramsByTask";
  std::stringstream ss;
  ss << " WHERE VH.TASK='" << Task << "'";
  return getHistograms( ss.str() , list, ids,types);
}

int OnlineHistDB::getHistogramsByPage(std::string Page,
				      std::vector<OnlineHistogram*>* list ,
				      std::vector<string>* ids,
				      std::vector<string>* types)
{  
  std::string folder;
  int nout=0;
  const int Nfetch=100;
  std::stringstream ss;
  Page = PagenameSyntax(Page, folder);
  ss << "select VH.NAME,VH.HSTYPE,SH.INSTANCE from VIEWHISTOGRAM VH, SHOWHISTO SH "
     << "WHERE SH.HISTO = VH.HID AND SH.PAGE='" << Page 
     << "' ORDER BY SH.INSTANCE";
  m_StmtMethod = "OnlineHistDB::getHistogramsByPage";
  OCIStmt *stmt=NULL;
  if (OCI_SUCCESS == 
      prepareOCIStatement(stmt, ss.str().c_str()) ) {
    if (OCI_SUCCESS == myOCISelectExecute(stmt) ) {
      text Name[Nfetch][VSIZE_NAME];
      text HStype[Nfetch][VSIZE_TYPE];
      for (int k=0; k<Nfetch; k++) {
	Name[k][0]= HStype[k][0]= '\0';
      }
      int instance[Nfetch];
      myOCIDefineString(stmt, 1, Name[0]  , VSIZE_NAME);
      myOCIDefineString(stmt, 2, HStype[0], VSIZE_TYPE);
      myOCIDefineInt   (stmt, 3, instance[0]);
      int xf = Nfetch;
      while ( xf == Nfetch) {
	xf = myOCIFetch(stmt, Nfetch);
	if(list) list->reserve(list->size()+xf);
	if(ids) ids->reserve(ids->size()+xf);
	if(types) types->reserve(types->size()+xf);
	for(int j=0; j<xf ; j++) {
	  if(list) list->push_back(getHistogram(std::string((const char*) Name[j]), 
						Page, instance[j]));
	  if(ids) ids->push_back(std::string((const char*) Name[j]));
	  if(types) types->push_back(std::string((const char*) HStype[j]));
	  nout++;     
	}      
      }
    }
  }
  return nout;
}

int OnlineHistDB::getHistogramsBySet(std::string SetName,std::vector<OnlineHistogram*>* list ,
				     std::vector<string>* ids,
				     std::vector<string>* types)
{
  m_StmtMethod = "OnlineHistDB::getHistogramsBySet";
  std::stringstream ss;
  ss << " WHERE VH.TASK||'/'||VH.ALGO||'/'||VH.TITLE='" << SetName << "'";
  return getHistograms( ss.str() , list, ids,types);
}

int OnlineHistDB::getHistogramsBySet(const OnlineHistogram& Set,std::vector<OnlineHistogram*>* list ,
				     std::vector<string>* ids,
				     std::vector<string>* types)
{
  m_StmtMethod = "OnlineHistDB::getHistogramsBySet";
  std::stringstream ss;
  ss << " WHERE VH.HSID=" << Set.hsid() ;
  return getHistograms( ss.str() , list, ids, types);
}



int OnlineHistDB::getPageFolderNames(std::vector<string>& list, std::string Parent )
{
  m_StmtMethod = "OnlineHistDB::getPageFolderNames";
  std::string command = "SELECT PAGEFOLDERNAME FROM PAGEFOLDER";
  if (Parent != "_ALL_") {
    command += " WHERE PARENT='"+Parent+"'";    
  }
  return genericStringQuery(command,list);
}


int OnlineHistDB::getPageNamesByFolder(std::string Folder,
				       std::vector<string>& list) {
  m_StmtMethod = "OnlineHistDB::getPageNamesByFolder";
  std::string command = "SELECT PAGENAME FROM PAGE";
  if (Folder != "_ALL_") 
    command += " WHERE FOLDER='" + Folder + "'";
  return genericStringQuery(command,list);
}

int OnlineHistDB::getPageNames(std::vector<string>& list) {
  m_StmtMethod = "OnlineHistDB::getPageNames";
  return getPageNamesByFolder("_ALL_",list);
}

int OnlineHistDB::getSubsystems(std::vector<string>& list) {
  m_StmtMethod = "OnlineHistDB::getSubsystems";
  return genericStringQuery("SELECT SSNAME FROM SUBSYSTEM",
			    list);
}

int OnlineHistDB::getTasks(std::vector<string>& list) {
  m_StmtMethod = "OnlineHistDB::getTasks";
  return genericStringQuery("SELECT TASKNAME FROM TASK",
			    list);
}

int OnlineHistDB::getAlgorithms(std::vector<string>& list,std::string type) {
  m_StmtMethod = "OnlineHistDB::getAlgorithms";
  std::string command = "SELECT ALGNAME FROM ALGORITHM";
  if (type == "CHECK" || type == "HCREATOR")
    command += " WHERE ALGTYPE='" + type + "'";
  return genericStringQuery(command,list);
}


int OnlineHistDB::genericStringQuery(std::string command, std::vector<string>& list) {
  int nout=0;
  const int Nfetch=100;
  OCIStmt *stmt=NULL;
  if (OCI_SUCCESS == 
      prepareOCIStatement(stmt, command.c_str()) ) {
    if (OCI_SUCCESS == myOCISelectExecute(stmt) ) {
      text Name[Nfetch][VSIZE_NAME];
      for (int k=0; k<Nfetch; k++)
	Name[k][0]='\0';
      myOCIDefineString(stmt, 1, Name[0]  ,VSIZE_NAME);
      int xf = Nfetch;
      while ( xf == Nfetch) {
	xf = myOCIFetch(stmt, Nfetch);
	list.reserve(list.size()+xf);
	for(int j=0; j<xf ; j++) {
	  list.push_back(std::string((const char*) Name[j] ) );
	  nout++;     
	}      
      }
    }
  }
  return nout;
}



int OnlineHistDB::getHistograms(std::string query,
				std::vector<OnlineHistogram*>* list, 
				std::vector<string>* ids,
				std::vector<string>* types)
{
  const int Nfetch=100;
  int nout=0;
  string command="select VH.NAME,VH.HSTYPE from VIEWHISTOGRAM VH "+query;
  
  OCIStmt *stmt=NULL;
  if (OCI_SUCCESS == 
      prepareOCIStatement(stmt, command.c_str()) ) {
    if (OCI_SUCCESS == myOCISelectExecute(stmt) ) {
      text Name[Nfetch][VSIZE_NAME];
      text HStype[Nfetch][VSIZE_TYPE];
      for (int k=0; k<Nfetch; k++) {
	Name[k][0]='\0';
	HStype[k][0]='\0';
      }
      myOCIDefineString(stmt, 1, Name[0]  ,VSIZE_NAME);
      myOCIDefineString(stmt, 2, HStype[0],VSIZE_TYPE);
      int xf = Nfetch;
      while ( xf == Nfetch) {
	xf = myOCIFetch(stmt, Nfetch);
	if(list) list->reserve(list->size()+xf);
	if(ids) ids->reserve(ids->size()+xf);
	if(types) types->reserve(types->size()+xf);
	for(int j=0; j<xf ; j++) {
	  if(list) list->push_back(getHistogram( std::string((const char*) Name[j]) ));
	  if(ids) ids->push_back(std::string((const char*) Name[j] ) );
	  if(types) types->push_back(std::string((const char*) HStype[j] ) );
	  nout++;     
	}      
      }
    }
  }
  return nout;
}
