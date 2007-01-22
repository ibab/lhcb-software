// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/OnlineHistDB/src/OnlineHistDB.cpp,v 1.2 2007-01-22 17:07:59 ggiacomo Exp $
/*
   C++ interface to the Online Monitoring Histogram DB
   G. Graziani (INFN Firenze)

*/

#include "OnlineHistDB/OnlineHistDB.h"



OnlineHistDB::OnlineHistDB(std::string passwd, 
			   std::string user, 
			   std::string db) : 
  OnlineHistDBEnv(), m_DBschema(2),
  m_stmt(0), m_nit(0), m_maxNit(1000), 
  m_maxSNsize(500) , m_maxANsize(100), m_maxTNsize(250), m_debug(0)
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

  m_env->terminateConnection (m_conn);
  Environment::terminateEnvironment (m_env);}


void OnlineHistDB::commit() {
  // send to DB the cached histogram creation statements 
  HistoStmtExec();
  // send the current page configurations
  //std::vector<OnlineHistPage*>::iterator ip;
  //for (ip = m_myPage.begin();ip != m_myPage.end(); ++ip) 
  //  (*ip)->save(); 
  Statement *fstmt=m_conn->createStatement ("begin OnlineHistDB.mycommit(:x1); end;");
  try{
    fstmt->setInt(1,m_DBschema);
    fstmt->executeUpdate ();
    if (m_debug>2) cout << "session terminated"<<endl; 
  }catch(SQLException ex) {
    dumpError(ex,"OnlineHistDB::commit");
  }
  m_conn->terminateStatement (fstmt);  

  // update stored histogram id needed
  std::vector<OnlineHistogram*>::iterator ih;
  for (ih = m_myHist.begin();ih != m_myHist.end(); ++ih) 
    (*ih)->update();
}


void OnlineHistDB::setHistogramBufferDepth(int N) {
  m_maxNit=N;
}

void OnlineHistDB::setDebug(int dl) {
  m_debug=dl;
}

void OnlineHistDB::declareTask(std::string Name, 
			       std::string SubDet1, 
			       std::string SubDet2, 
			       std::string SubDet3,
			       bool RunsOnPhysics, 
			       bool RunsOnCalib, 
			       bool RunsOnEmpty)
{
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
      dumpError(ex,"OnlineHistDB::declareTask");
    }
  m_conn->terminateStatement (tstmt);  
}

void OnlineHistDB::declareSubSystem(std::string SubSys)
{
   Statement *tstmt=m_conn->createStatement 
     ("begin OnlineHistDB.DeclareSubSystem(:x1); end;");
  try{
    tstmt->setString(1, SubSys);
    tstmt->executeUpdate();
  }catch(SQLException ex)
    {
      dumpError(ex,"OnlineHistDB::declareSubSystem");
    }
  m_conn->terminateStatement (tstmt);  
}



void OnlineHistDB::declareHistByServiceName(const std::string &ServiceName)
{
  declareHistogram(ServiceName,"","",0);
}



void OnlineHistDB::declareHistogram(std::string TaskName,
				    std::string AlgorithmName,
				    std::string Title,
				    int Dimension)
{
  if(m_stmt) {
    m_stmt->addIteration();
    if (m_debug >2) cout << "added iteration "<<m_nit <<"/"<<m_maxNit;
  } else {
    m_stmt=m_conn->createStatement 
      ("begin OnlineHistDB.DeclareHistogram(:x1,:x2,:x3,:x4); end;");
    m_stmt->setMaxIterations(m_maxNit);
    m_stmt->setMaxParamSize(1, m_maxSNsize);
    m_stmt->setMaxParamSize(2, m_maxANsize);
    m_stmt->setMaxParamSize(3, m_maxTNsize);
    if (m_debug >2) cout << "created first iteration "<<m_nit <<"/"<<m_maxNit;
  }
  m_stmt->setString(1, TaskName);
  m_stmt->setString(2, AlgorithmName);
  m_stmt->setString(3, Title);
  m_stmt->setInt(4, Dimension);
  m_nit++;
  if (m_debug >2) cout <<"   : " << TaskName <<"/"<<AlgorithmName<<"/"<< Title<<endl;
  if (m_nit == m_maxNit) HistoStmtExec();
}


void OnlineHistDB::HistoStmtExec() {
  if (m_nit == 0) return;
  if (m_debug >2) cout << "executing statement" <<endl;
  try{
    m_stmt->executeUpdate ();
    }catch(SQLException ex)
    {
      dumpError(ex,"OnlineHistDB::HistoStmtExec");
    }
  m_conn->terminateStatement (m_stmt);  
  m_stmt=0;
  m_nit=0;
}

void OnlineHistDB::declareCheckAlgorithm(std::string Name, 
					 int Npars, 
					 std::string* pars, 
					 std::string doc) {
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
      dumpError(ex,"OnlineHistDB::declareCheckAlgorithm");
    }
  m_conn->terminateStatement (astmt);  


}

void OnlineHistDB::declareCreatorAlgorithm(std::string Name, 
					   int Ninput, 
					   std::string doc) {
  stringstream statement;
  statement << "begin OnlineHistDB.DeclareCreatorAlgorithm(:x1,:x2";
  if (doc != "NONE")
    statement << ",:d";
  statement << "); end;";
  Statement *astmt=m_conn->createStatement (statement.str());
  try{
    astmt->setString(1, Name);
    astmt->setInt(2, Ninput);
    if (doc != "NONE")
      astmt->setString(3, doc);
    astmt->executeUpdate ();
  }catch(SQLException ex)
    {
      dumpError(ex,"OnlineHistDB::declareCreatorAlgorithm");
    }
  m_conn->terminateStatement (astmt);  
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
    page = new OnlineHistPage(Name,Folder,m_conn);
    m_myPage.push_back(page);
  }
  return page;
}

OnlineHistogram* OnlineHistDB::getHistogram(std::string Name, 
					    std::string Page,
					    int Instance) {
  OnlineHistogram* h=0;
  // see if the histogram object exists already
  std::vector<OnlineHistogram*>::iterator ih;
  for (ih = m_myHist.begin(); ih != m_myHist.end(); ++ih) {
    if ((*ih)->name() == Name && (*ih)->page() == Page && 
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
      astmt->setString(2, Name);
      astmt->registerOutParam(3, OCCIINT);
      astmt->execute();
    }catch(SQLException ex) {
      dumpError(ex,"OnlineHistDB::getHistogram");
    }
    if(astmt->getInt(1)) {
      h= new OnlineHistogram(Name,m_conn,Page,Instance);
      if (h->isAbort()) {
	cout<<"Error from OnlineHistDB::getHistogram : cannot create histogram object " 
	    << Name <<endl;
	delete h;
	h=0;
      }
      else
	m_myHist.push_back(h);
    }
    else {
      	cout << "Warning from OnlineHistDB::getHistogram : histogram " << Name <<
	  " not found in the DB" << endl;
    }
    m_conn->terminateStatement (astmt);  
  }
  return h;
}

int OnlineHistDB::getAlgorithmNpar(std::string AlgName) const
{
  int out=-1;
  Statement *astmt=m_conn->createStatement
    ("begin OnlineHistDB.GetAlgoNpar(:x1,:x2); end;");
  try{
    astmt->registerOutParam(2, OCCIINT);
    astmt->setString(1, AlgName);
    astmt->execute();
    out= astmt->getInt(2); 
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
    dumpError(ex,"OnlineHistDB::getAlgParName");
    exc = true;
  }
  if (false == exc) out= astmt->getString(3); 
  m_conn->terminateStatement (astmt);  
  return out;

}

OnlineHistogram* OnlineHistDB::declareAnalysisHistogram(std::string Algorithm,
				std::string Title,
				std::vector<OnlineHistogram*> &Sources)
{
  bool ok=true;
  int np=getAlgorithmNpar(Algorithm);
  if (np<0 || (np>0 && (int) Sources.size() < np) || (np==0 && Sources.size()<1)) {
    ok=false;
  }
  else {
    stringstream command;
    command << "begin OnlineHistDB.DeclareAnalysisHistogram('"
	    << Algorithm << "','" 
	    << Title << "',";
    if (np == 0)  // input is a set
      command << Sources[0]->hsid() << ",OnlineHistDB.sourceh()); end;";
    else { // input is a list of histograms
      command <<"0,OnlineHistDB.sourceh(";
      for (int ish=0 ; ish < np ; ish++) {
	command << "'" << Sources[ish]->hid() << "'";
	if (ish < np-1) command << ",";
      }
      command << ")); end;";
    }
    Statement *astmt=m_conn->createStatement (command.str());
    try{
      astmt->execute();
    }catch(SQLException ex)
      {
	dumpError(ex,"OnlineHistDB::declareAnalysisHistogram");
	ok=false;
      }
  }
  if (ok) {
    std::string Name="ANALYSIS/"+Algorithm+"/"+Title;
    return (new OnlineHistogram(Name,m_conn));
  }
  else
    return 0;
}


int OnlineHistDB::getHistograms(std::string query,std::vector<OnlineHistogram*>& list)
{
  int nout=0;
  string command="select NAME from VIEWHISTOGRAM VH "+query;
  Statement *qst=m_conn->createStatement(command);
  ResultSet *rset = qst->executeQuery ();
  while(rset->next ()) {
    list.push_back(getHistogram(rset->getString(1)));
    nout++;
  }
  return nout;
}



int OnlineHistDB::getHistogramsWithAnalysis(std::vector<OnlineHistogram*>& list)
{
  return getHistograms(",HISTOGRAMSET HS WHERE VH.HSID=HS.HSID AND HS.NANALYSIS>0",list);
}

int OnlineHistDB::getAnalysisHistograms(std::vector<OnlineHistogram*>& list)
{
  return getHistograms(",HISTOGRAM H WHERE VH.HID=H.HID AND H.ISANALYSISHIST=1",list);
}

int OnlineHistDB::getHistogramsBySubsystem(std::string SubSys,
					    std::vector<OnlineHistogram*>& list)
{  
  stringstream ss;
  ss << ", TASK T WHERE VH.TASK=T.TASKNAME AND (T.SUBSYS1 ='" << SubSys <<
    "' OR  T.SUBSYS2 ='"  << SubSys << "' OR  T.SUBSYS3 ='" << SubSys <<
    "')";
  return getHistograms( ss.str() , list);
}

int OnlineHistDB::getHistogramsByTask(std::string Task,
				       std::vector<OnlineHistogram*>& list)
{  
  stringstream ss;
  ss << " WHERE VH.TASK='" << Task << "'";
  return getHistograms( ss.str() , list);
}

int OnlineHistDB::getHistogramsByPage(std::string Page,
				       std::vector<OnlineHistogram*>& list)
{  
  stringstream ss;
  ss << " , SHOWHISTO SH WHERE SH.HISTO = VH.HID AND SH.PAGE='" << Page << "' ORDER BY SH.INSTANCE";
  return getHistograms( ss.str() , list);
}
int OnlineHistDB::getHistogramsBySet(std::string SetName,std::vector<OnlineHistogram*>& list)
{
  stringstream ss;
  ss << " WHERE VH.TASK||'/'||VH.ALGO||'/'||VH.TITLE='" << SetName << "'";
  return getHistograms( ss.str() , list);
}

int OnlineHistDB::getHistogramsBySet(const OnlineHistogram& Set,std::vector<OnlineHistogram*>& list)
{
  stringstream ss;
  ss << " WHERE VH.HSID=" << Set.hsid() ;
  return getHistograms( ss.str() , list);
}

int OnlineHistDB::getPageFolderNames(std::vector<string>& list)
{
  int nout=0;
  std::string command = "SELECT PAGEFOLDERNAME from PAGEFOLDER";
  Statement *qst=m_conn->createStatement(command);
  ResultSet *rset = qst->executeQuery ();
  while(rset->next ()) {
    list.push_back(rset->getString(1));
    nout++;
  }
  return nout;
}

int OnlineHistDB::getPageNames(std::vector<string>& list)
{
  return getPageNamesByFolder("_ALL_",list);
}

int OnlineHistDB::getPageNamesByFolder(std::string Folder,
				       std::vector<string>& list)
{
  int nout=0;
  std::string command = "SELECT PAGENAME from PAGE";
  if (Folder != "_ALL_") 
    command += " WHERE FOLDER='" + Folder + "'";
  Statement *qst=m_conn->createStatement(command);
  ResultSet *rset = qst->executeQuery ();
  while(rset->next ()) {
    list.push_back(rset->getString(1));
    nout++;
  }
  return nout;
}
