// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/OnlineHistDB/src/OnlineHistDB.cpp,v 1.4 2007-05-02 09:34:41 ggiacomo Exp $
/*
   C++ interface to the Online Monitoring Histogram DB
   G. Graziani (INFN Firenze)

*/

#include "OnlineHistDB/OnlineHistDB.h"



OnlineHistDB::OnlineHistDB(std::string passwd, 
			   std::string user, 
			   std::string db) : 
  OnlineHistDBEnv(user), m_DBschema(3),
  m_stmt(0), m_nit(0), m_maxNit(1000)
{
  m_env = Environment::createEnvironment (Environment::OBJECT);
  m_conn = m_env->createConnection (user, passwd, db);
}

OnlineHistDB::~OnlineHistDB () {  
  std::vector<OnlineHistPage*>::iterator ip;
  for (ip = m_myPage.begin();ip != m_myPage.end(); ++ip) 
    delete *ip; 
  std::vector<OnlineHistogram*>::iterator ih;
  for (ih = m_myHist.begin();ih != m_myHist.end(); ++ih) 
    delete *ih; 
  // a rollback is needed, otherwise all changes are committed at termination
  m_conn->rollback();
  m_env->terminateConnection (m_conn);
  Environment::terminateEnvironment (m_env);}


bool OnlineHistDB::commit() {
  bool out=true;
  // send to DB the cached histogram creation statements 
  sendHistBuffer();
  // send the current page configurations
  //std::vector<OnlineHistPage*>::iterator ip;
  //for (ip = m_myPage.begin();ip != m_myPage.end(); ++ip) 
  //  (*ip)->save(); 
  Statement *fstmt=m_conn->createStatement ("begin OnlineHistDB.mycommit(:x1); end;");
  try{
    fstmt->setInt(1,m_DBschema);
    fstmt->executeUpdate ();
    if (debug()>2) cout << "session terminated"<<endl; 
  }catch(SQLException ex) {
    dumpError(ex,"OnlineHistDB::commit");
    out=false;
  }
  m_conn->terminateStatement (fstmt);  

  // update stored histogram if needed
  std::vector<OnlineHistogram*>::iterator ih;
  for (ih = m_myHist.begin();ih != m_myHist.end(); ++ih) 
    (*ih)->update();
  if(debug()>0) cout << "Transaction succesfully committed"<<endl;
  return out;
}


void OnlineHistDB::setHistogramBufferDepth(int N) {
  m_maxNit=N;
}



bool OnlineHistDB::declareTask(std::string Name, 
			       std::string SubDet1, 
			       std::string SubDet2, 
			       std::string SubDet3,
			       bool RunsOnPhysics, 
			       bool RunsOnCalib, 
			       bool RunsOnEmpty)
{
  bool out=true;
  Statement *tstmt=m_conn->createStatement 
    ("begin OnlineHistDB.DeclareTask(:x1,:x2,:x3,:x4,:x5,:x6,:x7); end;");
  try{
    tstmt->setString(1, Name);
    tstmt->setString(2, SubDet1);
    tstmt->setString(3, SubDet2);
    tstmt->setString(4, SubDet3);
    tstmt->setInt(5, (int)RunsOnPhysics);
    tstmt->setInt(6, (int)RunsOnCalib);
    tstmt->setInt(7, (int)RunsOnEmpty);
    tstmt->executeUpdate ();
  }catch(SQLException ex)
    {
      dumpError(ex,"OnlineHistDB::declareTask for Task="+Name);
      out=false;
    }
  m_conn->terminateStatement (tstmt);  
  return out;
}

bool OnlineHistDB::declareSubSystem(std::string SubSys)
{
  bool out=true;
   Statement *tstmt=m_conn->createStatement 
     ("begin OnlineHistDB.DeclareSubSystem(:x1); end;");
  try{
    tstmt->setString(1, SubSys);
    tstmt->executeUpdate();
  }catch(SQLException ex)
    {
      dumpError(ex,"OnlineHistDB::declareSubSystem for Subsystem="+SubSys);
      out=false;
    }
  m_conn->terminateStatement (tstmt);  
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
  if(m_stmt) {
    m_stmt->addIteration();
    if (debug() >2) cout << "added iteration "<<m_nit <<"/"<<m_maxNit;
  } else {
    m_stmt=m_conn->createStatement 
      ("begin OnlineHistDB.DeclareHistogram(:x1,:x2,:x3,:x4); end;");
    m_stmt->setMaxIterations(m_maxNit);
    m_stmt->setMaxParamSize(1, OnlineHistDBEnv_constants::maxSNsize);
    m_stmt->setMaxParamSize(2, OnlineHistDBEnv_constants::maxANsize);
    m_stmt->setMaxParamSize(3, OnlineHistDBEnv_constants::maxTNsize);
    m_stmt->setMaxParamSize(4, OnlineHistDBEnv_constants::maxHTsize);
    if (debug() >2) cout << "created first iteration "<<m_nit <<"/"<<m_maxNit;
  }
  m_stmt->setString(1, TaskName);
  m_stmt->setString(2, AlgorithmName);
  m_stmt->setString(3, Title);
  m_stmt->setString(4, OnlineHistDBEnv_constants::HistTypeName[(int)Type]);
  m_nit++;
  if (debug() >2) cout <<"   : " << TaskName <<"/"<<AlgorithmName<<"/"<< Title<<endl;
  if (m_nit == m_maxNit) sendHistBuffer();
}


bool OnlineHistDB::sendHistBuffer() {
  bool out=true;
  if (m_nit == 0) return out;
  if (debug() >2) cout << "executing statement" <<endl;
  try{
    m_stmt->executeUpdate ();
    }catch(SQLException ex)
    {
      stringstream error;
      error<<"OnlineHistDB::sendHistBuffer while declaring last " <<
	m_nit << " histograms" <<endl;
      error << "All histograms declared after the one generating the error are not declared to the DB";
      dumpError(ex,error.str());
      out=false;
    }
  m_conn->terminateStatement (m_stmt);  
  m_stmt=0;
  m_nit=0;
  return out;
}

bool OnlineHistDB::declareCheckAlgorithm(std::string Name, 
					 int Npars, 
					 std::string* pars, 
					 std::string doc) {
  bool out=true;
  stringstream statement;
  statement << "begin OnlineHistDB.DeclareCheckAlgorithm(:x1,parameters(";
  int ipar=0;
  while (ipar++<Npars) {
    statement << ":p" << ipar;
    if(ipar<Npars) statement << ",";
  } 
  statement << ")";
  if (doc != "NONE")
    statement << ",:d";
  statement << "); end;";
  
  Statement *astmt=m_conn->createStatement (statement.str());
  try{
    astmt->setString(1, Name);
    ipar=0;
    while (ipar++<Npars) {
      astmt->setString(1+ipar, *pars);
      pars++;
    }
    if (doc != "NONE") {
      astmt->setString(2+Npars,doc);
    }
    astmt->executeUpdate ();
  }catch(SQLException ex)
    {
      dumpError(ex,"OnlineHistDB::declareCheckAlgorithm for "+Name);
      out=false;
    }
  m_conn->terminateStatement (astmt);  
  return out;

}

bool OnlineHistDB::declareCreatorAlgorithm(std::string Name, 
					   int Ninput, 
					   OnlineHistDBEnv::HistType OutputType,
					   int Npars, 
					   std::string* pars, 
					   std::string doc) {
  bool out=true;
  stringstream statement;
  statement << "begin OnlineHistDB.DeclareCreatorAlgorithm(:x1,:x2,parameters(";
  int ipar=0;
  while (ipar++<Npars) {
    statement << ":p" << ipar;
    if(ipar<Npars) statement << ",";
  } 
  statement << "),:type";
  if (doc != "NONE")
    statement << ",:d";
  statement << "); end;";
  Statement *astmt=m_conn->createStatement (statement.str());
  try{
    astmt->setString(1, Name);
    astmt->setInt(2, Ninput);
    ipar=0;
    while (ipar++<Npars) {
      astmt->setString(2+ipar, *pars);
      pars++;
    }
    astmt->setString(3+Npars, OnlineHistDBEnv_constants::HistTypeName[(int)OutputType]);
    if (doc != "NONE")
      astmt->setString(4+Npars, doc);
    astmt->execute();
  }catch(SQLException ex)
    {
      dumpError(ex,"OnlineHistDB::declareCreatorAlgorithm for "+Name);
      out=false;
    }
  m_conn->terminateStatement (astmt);  
  return out;
}

OnlineHistPage* OnlineHistDB::getPage(std::string Name, 
				       std::string Folder) {
  OnlineHistPage* page = 0;
  // see if page object exists already
  std::vector<OnlineHistPage*>::iterator ip;
  for (ip = m_myPage.begin();ip != m_myPage.end(); ++ip) {
    if ((*ip)->name() == Name) {
      page = *ip;
      break;
    }
  }
  // otherwise create new one
  if ( 0 == page) {
    page = new OnlineHistPage(Name,Folder,m_conn,m_user);
    page->setDebug(debug());
    page->setExcLevel(excLevel());
    m_myPage.push_back(page);
  }
  return page;
}

OnlineHistogram* OnlineHistDB::getHistogram(std::string Identifier, 
					    std::string Page,
					    int Instance) {
  OnlineHistogram* h=0;
  // see if the histogram object exists already
  std::vector<OnlineHistogram*>::iterator ih;
  for (ih = m_myHist.begin(); ih != m_myHist.end(); ++ih) {
    if ((*ih)->identifier() == Identifier && (*ih)->page() == Page && 
	(*ih)->instance() == Instance ) {
      h = *ih;
      break;
    }
  }
  if (!h) {
    Statement *astmt=m_conn->createStatement 
      ("begin :x1 := OnlineHistDB.GetHID(:x2,:x3); end;");
    try{
      astmt->registerOutParam(1, OCCIINT);
      astmt->setString(2, Identifier);
      astmt->registerOutParam(3, OCCIINT);
      astmt->execute();
    }catch(SQLException ex) {
      dumpError(ex,"OnlineHistDB::getHistogram for Histogram "+Identifier);
    }
    if(astmt->getInt(1)) {
      h= new OnlineHistogram(Identifier,m_conn,m_user,Page,Instance);
      h->setDebug(debug());
      h->setExcLevel(excLevel());
      if (h->isAbort()) {
	cout<<"Error from OnlineHistDB::getHistogram : cannot create histogram object " 
	    << Identifier <<endl;
	delete h;
	h=0;
      }
      else 
	m_myHist.push_back(h);
    }
    else {
      	cout << "Warning from OnlineHistDB::getHistogram : histogram " << Identifier <<
	  " not found in the DB" << endl;
    }
    m_conn->terminateStatement (astmt);  
  }
  return h;
}

int OnlineHistDB::getAlgorithmNpar(std::string AlgName, int* Ninput) const
{
  int out=-1;
  Statement *astmt=m_conn->createStatement
    ("begin OnlineHistDB.GetAlgoNpar(:x1,:x2, :x3); end;");
  try{
    astmt->registerOutParam(2, OCCIINT);
    astmt->registerOutParam(3, OCCIINT);
    astmt->setString(1, AlgName);
    astmt->execute();
    out= astmt->getInt(2); 
    if(Ninput) *Ninput = astmt->getInt(3);
  }catch(SQLException ex) {
    dumpError(ex,"OnlineHistDB::getAlgorithmNpar");
  }
  m_conn->terminateStatement (astmt);  
  return out;
}

std::string OnlineHistDB::getAlgParName(std::string AlgName,
					int Ipar) const
{
  std::string out= "";
  bool exc = false;
  Statement *astmt=m_conn->createStatement
    ("begin OnlineHistDB.GetAlgoParName(:x1,:x2,:x3); end;");
  try{
    astmt->setString(1, AlgName);
    astmt->setInt(2, Ipar);
    astmt->registerOutParam(3,OCCISTRING,15);
    astmt->execute();
  }catch(SQLException ex) {
    dumpError(ex,"OnlineHistDB::getAlgParName for Algorithm "+AlgName);
    exc = true;
  }
  if (false == exc) out= astmt->getString(3); 
  m_conn->terminateStatement (astmt);  
  return out;

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
    stringstream command;
    command << "begin OnlineHistDB.DeclareAnalysisHistogram('"
	    << Algorithm << "','" 
	    << Title << "',";
    if (nin == 0)  // input is a set
      command << Sources[0]->hsid() << ",OnlineHistDB.sourceh()";
    else { // input is a list of histograms
      command <<"0,OnlineHistDB.sourceh(";
      for (int ish=0 ; ish < nin ; ish++) {
	command << "'" << Sources[ish]->hid() << "'";
	if (ish < nin-1) command << ",";
      }
      command << ")";
    }
    if (np > 0) {
      command << ",thresholds(";
      for (int ipp=0; ipp<np ;ipp++) {
	command << Parameters->at(ipp);
	if (ipp < np-1) command << ",";   
      }
      command << ")";
    }
    command <<"); end;";
    
    Statement *astmt=m_conn->createStatement (command.str());
    try{
      astmt->execute();
    }catch(SQLException ex)
      {
	dumpError(ex,"OnlineHistDB::declareAnalysisHistogram for Title "+Title);
	ok=false;
      }
  }
  if (ok) {
    std::string Name="ANALYSIS/"+Algorithm+"/"+Title;
    outh=new OnlineHistogram(Name,m_conn,m_user);
    outh->setDebug(debug());
    outh->setExcLevel(excLevel());
  }
  return outh;
}

bool OnlineHistDB::removeHistogram(OnlineHistogram* h,
				   bool RemoveWholeSet) {
  bool out=true;
  string command;
  if (RemoveWholeSet)
    command = "begin :out := OnlineHistDB.DeleteHistogramSet(:1); end;";
  else
    command = "begin :out := OnlineHistDB.DeleteHistogram(:1); end;";
  Statement *dst=m_conn->createStatement(command);
  try{
    dst->registerOutParam(1, OCCIINT);
    if (RemoveWholeSet)
      dst->setInt(2,h->hsid());
    else
      dst->setString(2,h->hid());
    dst->execute();
    out = (dst->getInt(1) == 1);
  }catch(SQLException ex) {
    dumpError(ex,"OnlineHistDB::removeHistogram for Histogram "+h->identifier());
    cout << "Histogram is probably on a page, remove it from all pages first"<<endl;
    out=false;
  }
  if(out) {
    std::vector<OnlineHistogram*>::iterator ih = m_myHist.begin();
    while (ih != m_myHist.end()) {
      if((RemoveWholeSet && (*ih)->hsid() == h->hsid()) ||
	 (*ih)->hid() == h->hid() ) {
	if (h != (*ih)) delete *ih;
	ih=m_myHist.erase(ih);
      }
      else {
	ih++;
      }
    }
    delete h;
  }
  return out;  
}

int OnlineHistDB::genericStringQuery(std::string command, std::vector<string>& list) {
  int nout=0;
  Statement *qst=m_conn->createStatement(command);
  ResultSet *rset = qst->executeQuery ();
  while(rset->next ()) {
    list.push_back(rset->getString(1));
    nout++;
  }
  return nout;
}

int OnlineHistDB::getHistograms(std::string query,
				std::vector<OnlineHistogram*>* list, 
				std::vector<string>* names)
{
  int nout=0;
  string command="select NAME from VIEWHISTOGRAM VH "+query;
  Statement *qst=m_conn->createStatement(command);
  ResultSet *rset = qst->executeQuery ();
  while(rset->next ()) {
    if(list) list->push_back(getHistogram(rset->getString(1)));
    if(names) names->push_back(rset->getString(1));
    nout++;
  }
  return nout;
}

int OnlineHistDB::getAllHistograms(std::vector<OnlineHistogram*>* list ,
				   std::vector<string>* ids) {
  return getHistograms("",list,ids);
}


int OnlineHistDB::getHistogramsWithAnalysis(std::vector<OnlineHistogram*>* list ,
				   std::vector<string>* ids)
{
  return getHistograms(",HISTOGRAMSET HS WHERE VH.HSID=HS.HSID AND HS.NANALYSIS>0",list,ids);
}

int OnlineHistDB::getAnalysisHistograms(std::vector<OnlineHistogram*>* list ,
				   std::vector<string>* ids)
{
  return getHistograms(",HISTOGRAM H WHERE VH.HID=H.HID AND H.ISANALYSISHIST=1",list,ids);
}

int OnlineHistDB::getHistogramsBySubsystem(std::string SubSys,
					   std::vector<OnlineHistogram*>* list ,
					   std::vector<string>* ids)
{  
  stringstream ss;
  ss << ", TASK T WHERE VH.TASK=T.TASKNAME AND (T.SUBSYS1 ='" << SubSys <<
    "' OR  T.SUBSYS2 ='"  << SubSys << "' OR  T.SUBSYS3 ='" << SubSys <<
    "')";
  return getHistograms( ss.str() , list, ids);
}

int OnlineHistDB::getHistogramsByTask(std::string Task,
				      std::vector<OnlineHistogram*>* list ,
				      std::vector<string>* ids)
{  
  stringstream ss;
  ss << " WHERE VH.TASK='" << Task << "'";
  return getHistograms( ss.str() , list, ids);
}

int OnlineHistDB::getHistogramsByPage(std::string Page,
				      std::vector<OnlineHistogram*>* list ,
				      std::vector<string>* ids)
{  
  stringstream ss;
  ss << " , SHOWHISTO SH WHERE SH.HISTO = VH.HID AND SH.PAGE='" << Page << "' ORDER BY SH.INSTANCE";
  return getHistograms( ss.str() , list, ids);
}
int OnlineHistDB::getHistogramsBySet(std::string SetName,std::vector<OnlineHistogram*>* list ,
				     std::vector<string>* ids)
{
  stringstream ss;
  ss << " WHERE VH.TASK||'/'||VH.ALGO||'/'||VH.TITLE='" << SetName << "'";
  return getHistograms( ss.str() , list, ids);
}

int OnlineHistDB::getHistogramsBySet(const OnlineHistogram& Set,std::vector<OnlineHistogram*>* list ,
				   std::vector<string>* ids)
{
  stringstream ss;
  ss << " WHERE VH.HSID=" << Set.hsid() ;
  return getHistograms( ss.str() , list, ids);
}

int OnlineHistDB::getPageFolderNames(std::vector<string>& list, std::string Parent )
{
  std::string command = "SELECT PAGEFOLDERNAME from PAGEFOLDER";
  if (Parent == "ROOT") {
    command += " WHERE PARENT is NULL";
  }
  else if (Parent != "_ALL_") {
    command += " WHERE PARENT='"+Parent+"'";    
  }
  return genericStringQuery(command,list);
}

int OnlineHistDB::getPageNames(std::vector<string>& list) {
  return getPageNamesByFolder("_ALL_",list);
}

int OnlineHistDB::getPageNamesByFolder(std::string Folder,
				       std::vector<string>& list) {
  std::string command = "SELECT PAGENAME from PAGE";
  if (Folder != "_ALL_") 
    command += " WHERE FOLDER='" + Folder + "'";
  return genericStringQuery(command,list);
}

int OnlineHistDB::getSubsystems(std::vector<string>& list) {
  return genericStringQuery("SELECT SSName from SUBSYSTEM",
			    list);
}

int OnlineHistDB::getTasks(std::vector<string>& list) {
  return genericStringQuery("SELECT TASKNAME from TASK",
			    list);
}

int OnlineHistDB::getAlgorithms(std::vector<string>& list,std::string type) {
  std::string command = "SELECT ALGNAME from ALGORITHM";
  if (type == "CHECK" || type == "HCREATOR")
    command += " WHERE ALGTYPE='" + type + "'";
  return genericStringQuery(command,list);
}
