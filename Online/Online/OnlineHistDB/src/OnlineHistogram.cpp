// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/OnlineHistDB/src/OnlineHistogram.cpp,v 1.18 2007-10-26 13:46:43 ggiacomo Exp $
/*
   C++ interface to the Online Monitoring Histogram DB
   G. Graziani (INFN Firenze)

*/

#include <math.h>
#include "OnlineHistDB/OnlineHistogram.h"

OnlineHistogram::OnlineHistogram(OnlineHistDBEnv &env,
				 std::string Identifier,
				 std::string Page,
				 int Instance) :
  OnlineHistDBEnv(env), SourceSet(0),
  m_isAbort(false), m_identifier(Identifier), 
  m_page(Page), m_instance(Instance), 
  m_DOinit(false), m_InitDOmode(NONE), m_domode(NONE), m_hsdisp(0), m_hdisp(0), m_shdisp(0) {
    
    if (m_page  != "_NONE_") {
      if (!verifyPage(m_page,Instance)) {
	m_page = "_NONE_";
	m_instance =1;
      }
    }
    load();
  }

void OnlineHistogram::checkServiceName() {
  // update the object from DB if the NIM service name has changed 
  ResultSet *rset;
  Statement *query = m_conn->createStatement("SELECT SN FROM DIMSERVICENAME WHERE PUBHISTO=:1");
  bool reload=false;
  query->setString(1,m_hid);
  rset =  query->executeQuery ();
  if(rset->next ()) {
    if (rset->getString(1) != m_dimServiceName) 
      reload = true;
  }
  if (reload) {
    if (debug() > 3) cout << "Reloading Histogram " << identifier() << endl;
    load();
  }
  m_conn->terminateStatement(query);
}

bool OnlineHistogram::setPage(std::string Page,
			      int Instance) {
  bool out=true;
  if ( Page != m_page || Instance != m_instance) {
    out=verifyPage(Page,Instance); // Page may change
    if (out && (Page != m_page || Instance != m_instance)) {
      m_page = Page;
      m_instance = Instance;
      load();
      m_InitDOmode = m_domode;
    }
  }
  return out;
}

void OnlineHistogram::unsetPage() {
  if (m_page != "_NONE_") {
    m_page = "_NONE_";
    m_instance = 1;
    load();
  }
}

bool OnlineHistogram::setDimServiceName(std::string DimServiceName) {
  bool out=true;
  Statement *astmt = m_conn->createStatement("begin :out := OnlineHistDB.SetDimServiceName(:2,:3); end;");
  try{
    astmt->registerOutParam(1,OCCIINT);
    astmt->setString(2, m_hid);
    astmt->setString(3, DimServiceName);
    astmt->execute();
    out=astmt->getInt(1);
  }catch(SQLException ex)
    {
      dumpError(ex,"OnlineHistogram::setdimServiceName");
      out=false;
    }
  m_conn->terminateStatement (astmt);  
  return out;
}


bool OnlineHistogram::setDoc(std::string doc) {
  // to be done in v4:add length check 
  bool out=true;
  Statement *astmt = m_conn->createStatement("UPDATE HISTOGRAMSET set DOC=:1 where HSID=:2");
  try{
    astmt->setString(1, doc);
    astmt->setInt   (2, m_hsid);
    astmt->execute();
    m_Doc = doc;
  }catch(SQLException ex)
    {
      dumpError(ex,"OnlineHistogram::setDoc");
      out=false;
    }
  m_conn->terminateStatement (astmt);  
  return out;
}


bool OnlineHistogram::setDescr(std::string descr) {
  // to be done in v4:add length check 
  bool out=true;
  Statement *astmt = m_conn->createStatement("UPDATE HISTOGRAMSET set DESCR=:1 where HSID=:2");
  try{
    astmt->setString(1, descr);
    astmt->setInt   (2, m_hsid);
    astmt->execute();
    m_Descr = descr;
  }catch(SQLException ex)
    {
      dumpError(ex,"OnlineHistogram::setDescr");
      out=false;
    }
  m_conn->terminateStatement (astmt);  
  return out;
}





bool OnlineHistogram::verifyPage(std::string &Page, int Instance) {
  // check that page exists and histogram is on it
  bool out=true;
  std::string fold;
  Page = PagenameSyntax(Page, fold);
  if (m_page != "_NONE_") {
    ResultSet *rset;
    Statement *query = 
      m_conn->createStatement("SELECT INSTANCE FROM SHOWHISTO SH ,VIEWHISTOGRAM VH WHERE VH.NAME=:1 AND SH.PAGE=:2 AND SH.INSTANCE=:3");
    query->setString(1,m_identifier);
    query->setString(2,Page);
    query->setInt(3,Instance);
    try{
      rset =  query->executeQuery();
      if(!rset->next ()) 
	out=false;
    } 
    catch(SQLException ex) {
      dumpError(ex,"OnlineHistogram::verifyPage");
      out=false;
    }
    m_conn->terminateStatement(query);
  }
  return out;
}

void OnlineHistogram::load() {
  std::string com="begin :out := OnlineHistDB.GetHistogramData(:2,:3,:4,:5,:6,:7,:8,:9,:10,:11,:12,:13,:14,:15,:16,:17,:18,:19,:20,:21,:22, :23); end;";
  Statement *query = m_conn->createStatement(com);
  try{
    int ip=1;
    query->registerOutParam(ip++,OCCIINT);
    query->setString(ip++, m_identifier);
    query->setString(ip++, m_page);
    query->setInt(ip++, m_instance);
    query->registerOutParam(ip++,OCCISTRING,12);
    query->registerOutParam(ip++,OCCIINT);
    query->registerOutParam(ip++,OCCIINT);
    query->registerOutParam(ip++,OCCIINT);
    query->registerOutParam(ip++,OCCISTRING,4);
    query->registerOutParam(ip++,OCCISTRING,200);
    query->registerOutParam(ip++,OCCISTRING,50);
    query->registerOutParam(ip++,OCCISTRING,100);
    query->registerOutParam(ip++,OCCISTRING,100);
    query->registerOutParam(ip++,OCCIINT);
    query->registerOutParam(ip++,OCCISTRING,4000);
    query->registerOutParam(ip++,OCCISTRING,200);
    query->registerOutParam(ip++,OCCIINT);
    query->registerOutParam(ip++,OCCIINT);
    query->registerOutParam(ip++,OCCIINT);
    query->registerOutParam(ip++,OCCIINT);
    query->registerOutParam(ip++,OCCIINT);
    query->registerOutParam(ip++,OCCIINT);
    query->registerOutParam(ip++,OCCISTRING,500);
    query->execute();
  }catch(SQLException ex)
    {
      dumpError(ex,"OnlineHistogram:: OnlineHistogram");
      m_isAbort=true;
    }
  if (!(query->getInt(1))) {
    m_isAbort=true;
    m_conn->terminateStatement (query);
  }
  else {
    int ip=5;
    m_hid=query->getString(ip++);
    m_hsid=query->getInt(ip++);
    m_ihs=query->getInt(ip++);
    m_nhs=query->getInt(ip++);
    m_hstype=query->getString(ip++);
    m_hsname=query->getString(ip++);
    m_subname=query->getString(ip++);
    m_task=query->getString(ip++);
    m_algorithm=query->getString(ip++);
    m_nanalysis=query->getInt(ip++);
    m_Descr=query->getString(ip++);
    m_Doc=query->getString(ip++);
    m_isAnaHist=query->getInt(ip++);
    m_creation=query->getInt(ip++);
    m_obsoleteness=query->getInt(ip++);
    m_hdisp=query->getInt(ip++);
    m_hsdisp=query->getInt(ip++);
    m_shdisp=query->getInt(ip++);
    m_dimServiceName=query->getString(ip++);
    m_conn->terminateStatement (query);

    if (m_nanalysis>0) {
      com="begin :out := OnlineHistDB.GetHistoAnalysis(:2, :3, :4); end;";
      query = m_conn->createStatement(com);
      try{
	query->registerOutParam(1,OCCIINT);
	query->setInt(2, m_hsid);
	query->registerOutParam(3, OCCIVECTOR, 0,m_user+".INTLIST");
	query->registerOutParam(4, OCCIVECTOR, 0,m_user+".ANALIST");
	query->execute();
      }catch(SQLException ex)
	{
	  dumpError(ex,"OnlineHistogram::load calling GetHistoAnalysis");
	  m_isAbort=true;
	}
      getVector(query, 3, m_anaId);
      getVector(query, 4, m_anaName);
      m_conn->terminateStatement (query);
    } // end of analysis part
    
    

    // display options
    createDisplayOptions();
  }
}




void OnlineHistogram::createDisplayOptions() {
  if(!m_DOinit) {
    m_do.push_back(new OnlineDisplayOption("LABEL_X",OnlineDisplayOption::STRING));
    m_do.push_back(new OnlineDisplayOption("LABEL_Y",OnlineDisplayOption::STRING));
    m_do.push_back(new OnlineDisplayOption("LABEL_Z",OnlineDisplayOption::STRING));
    m_do.push_back(new OnlineDisplayOption("YMIN",OnlineDisplayOption::FLOAT));
    m_do.push_back(new OnlineDisplayOption("YMAX",OnlineDisplayOption::FLOAT));
    m_do.push_back(new OnlineDisplayOption("STATS",OnlineDisplayOption::INT));
    m_do.push_back(new OnlineDisplayOption("FILLSTYLE",OnlineDisplayOption::INT));
    m_do.push_back(new OnlineDisplayOption("FILLCOLOR",OnlineDisplayOption::INT));
    m_do.push_back(new OnlineDisplayOption("LINESTYLE",OnlineDisplayOption::INT));
    m_do.push_back(new OnlineDisplayOption("LINECOLOR",OnlineDisplayOption::INT));
    m_do.push_back(new OnlineDisplayOption("LINEWIDTH",OnlineDisplayOption::INT));
    m_do.push_back(new OnlineDisplayOption("DRAWOPTS",OnlineDisplayOption::STRING));
  }
  if(m_shdisp) {
    m_domode=HISTPAGE;
    getDisplayOptions(m_shdisp);
  }
  else if(m_hdisp) {
    m_domode=HIST;
    getDisplayOptions(m_hdisp);
  }
  else if(m_hsdisp) {
    m_domode=SET;
    getDisplayOptions(m_hsdisp);
  }
  if(!m_DOinit) m_InitDOmode=m_domode;
  m_DOinit=true;
}


bool OnlineHistogram::loadCreationDirections() {
  bool out = false;
  if(m_isAnaHist) { 
    Statement *query = m_conn->createStatement
      ("begin OnlineHistDB.GetAnaHistDirections(theHCID=> :1,Alg => :2,Sset => :3, Sources => :4, Pars =>:5 ); end;");
    try{
      query->setString(1, m_hid);
      query->registerOutParam(2, OCCISTRING,30);
      query->registerOutParam(3, OCCIINT);
      query->registerOutParam(4, OCCIVECTOR, 0,m_user+".HNALIST");
      query->registerOutParam(5, OCCIVECTOR, 0,m_user+".FLOLIST");
      query->execute();
      CreationAlgorithm=query->getString(2);
      SourceSet=query->getInt(3);
      getVector(query, 4, Sources);
      getVector(query, 5, SourcePar);
    }catch(SQLException ex)
      {
	dumpError(ex,"OnlineHistogram::loadCreationDirections");
      }
    m_conn->terminateStatement (query);
    out = true;    
  }
  return out;
}


OnlineHistogram::~OnlineHistogram()
{
  std::vector<OnlineDisplayOption*>::iterator ip;
  for (ip = m_do.begin();ip != m_do.end(); ++ip) 
    delete *ip; 
}


bool OnlineHistogram::remove(bool RemoveWholeSet) {
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
      dst->setInt(2,hsid());
    else
      dst->setString(2,hid());
    dst->execute();
    out = (dst->getInt(1) == 1);
  }catch(SQLException ex) {
    dumpError(ex,"OnlineHistDB::removeHistogram for Histogram "+identifier());
    cout << "Histogram is probably on a page, remove it from all pages first"<<endl;
    out=false;
  }
  m_conn->terminateStatement (dst);
  if(out) m_isAbort=true;
  return out;  
}


// DISPLAY OPTIONS

void OnlineHistogram::getDisplayOptions(int doid) {
  int out=0;
  Statement *astmt=m_conn->createStatement 
    ("begin :out := OnlineHistDB.GetDisplayOptions(:1,:2,:3,:4,:5,:6,:7,:8,:9,:10,:11,:12,:13); end;");
  try{
    astmt->registerOutParam(1, OCCIINT); 
    astmt->setInt(2, doid);
    astmt->registerOutParam(3,OCCISTRING,50);
    astmt->registerOutParam(4,OCCISTRING,50);
    astmt->registerOutParam(5,OCCISTRING,50);
    astmt->registerOutParam(6,OCCIFLOAT);
    astmt->registerOutParam(7,OCCIFLOAT);
    astmt->registerOutParam(8,OCCIINT);
    astmt->registerOutParam(9,OCCIINT);
    astmt->registerOutParam(10,OCCIINT);
    astmt->registerOutParam(11,OCCIINT);
    astmt->registerOutParam(12,OCCIINT);
    astmt->registerOutParam(13,OCCIINT);
    astmt->registerOutParam(14,OCCISTRING,50);
    astmt->execute();
    out=astmt->getInt(1);
  }catch(SQLException ex) {
    dumpError(ex,"OnlineHistogram::getDisplayOptions");
  }
  if (out) {
    getDO("LABEL_X")->set(astmt->getString(3));
    getDO("LABEL_Y")->set(astmt->getString(4));
    getDO("LABEL_Z")->set(astmt->getString(5));
    getDO("YMIN")->set(astmt->getFloat(6));
    getDO("YMAX")->set(astmt->getFloat(7));
    getDO("STATS")->set(astmt->getInt(8));
    getDO("FILLSTYLE")->set(astmt->getInt(9));
    getDO("FILLCOLOR")->set(astmt->getInt(10));
    getDO("LINESTYLE")->set(astmt->getInt(11));
    getDO("LINECOLOR")->set(astmt->getInt(12));
    getDO("LINEWIDTH")->set(astmt->getInt(13));
    getDO("DRAWOPTS")->set(astmt->getString(14));
  }
  else
    m_domode = NONE;
  m_conn->terminateStatement (astmt);
}


int OnlineHistogram::setDisplayOptions(std::string function) {
  //update the DB with the current display options
  int out;
  stringstream statement;
  statement << "begin :out := OnlineHistDB." << function ;
  std::vector<OnlineDisplayOption*>::iterator ip;
  for (ip = m_do.begin();ip != m_do.end(); ++ip) {
    if((*ip)->isSet()) {
      statement  << ",K" << (*ip)->name() << " => '";
      switch ((*ip)->type()) {
      case OnlineDisplayOption::STRING :
	statement  << *(static_cast<const std::string*>((*ip)->value()));
	break;
      case OnlineDisplayOption::FLOAT :
	statement  << *(static_cast<const float*>((*ip)->value()));
	break;
      case OnlineDisplayOption::INT :
	statement  << *(static_cast<const int*>((*ip)->value()));
	break;
      }
      statement << "'";
    } 
  }
  statement  <<	", reset => 1); end;";
  if(debug() > 3) cout << "Sending statement: "<<statement.str() << endl;
  Statement *fstmt=m_conn->createStatement (statement.str());
  try{
    fstmt->registerOutParam(1, OCCIINT); 
    fstmt->execute();
    out=fstmt->getInt(1);
  } catch(SQLException ex)
    {
      dumpError(ex,"OnlineHistogram::"+function);
      out=0;
    }
  m_conn->terminateStatement (fstmt); 
  return out;
}


bool OnlineHistogram::checkHSDisplayFromDB() {
  bool out=true;
  ResultSet *rset;
  Statement *query = 
    m_conn->createStatement("SELECT HSDISPLAY FROM HISTOGRAMSET WHERE HSID=:1 AND HSDISPLAY IS NOT NULL");
  query->setInt(1,m_hsid);
  try{
    rset =  query->executeQuery();
    if(!rset->next ()) 
      out=false;
  } 
  catch(SQLException ex) {
    dumpError(ex,"OnlineHistogram::checkHSDisplayFromDB");
    out=false;
  }
  return out;
}
bool OnlineHistogram::checkHDisplayFromDB() {
  bool out=true;
  ResultSet *rset;
  Statement *query = 
    m_conn->createStatement("SELECT DISPLAY FROM HISTOGRAM WHERE HID=:1 AND DISPLAY IS NOT NULL");
  query->setString(1,m_hid);
  try{
    rset =  query->executeQuery();
    if(!rset->next ()) 
      out=false;
  } 
  catch(SQLException ex) {
    dumpError(ex,"OnlineHistogram::checkHSDisplayFromDB");
    out=false;
  }
  return out;
}
int OnlineHistogram::nPageInstances() {
  int out=0;
  ResultSet *rset;
  Statement *query = 
    m_conn->createStatement("SELECT HISTO FROM SHOWHISTO WHERE HISTO=:1");
  query->setString(1,m_hid);
  try{
    rset =  query->executeQuery();
    while(rset->next ()) 
      out++;
  } 
  catch(SQLException ex) {
    dumpError(ex,"OnlineHistogram::nPageInstances");
    out=0;
  }
  return out;
}
int OnlineHistogram::nThisPageInstances() {
  int out=0;
  ResultSet *rset;
  Statement *query = 
    m_conn->createStatement("SELECT HISTO FROM SHOWHISTO WHERE HISTO=:1 AND PAGE=:2");
  query->setString(1,m_hid);
  query->setString(2,m_page);
  try{
    rset =  query->executeQuery();
    while(rset->next ()) 
      out++;
  } 
  catch(SQLException ex) {
    dumpError(ex,"OnlineHistogram::nThisPageInstances");
    out=0;
  }
  return out;
}

bool OnlineHistogram::useSetForHistDisplay() {
  // decide if associate display options to set or single histogram
  bool out=true;
  if(m_nhs > 1) { // use set for single histogram sets
    if (m_InitDOmode >= SET) // use hist if some display options were already defined.. 
      out = false;
    else {
      // .. or have been defined by another histogram in set in the meantime
      if(!m_hsdisp && checkHSDisplayFromDB())
	out = false;
    }
  }
  return out;
}
bool OnlineHistogram::useHistForPageDisplay() {
  // decide if associate display options to hist or histInPage
  bool out=true;
  if("_NONE_" != m_page) { // use hist if no page is specified
    if( m_nhs == 1 ) { // check for set options
      if (m_InitDOmode >= SET) // use page if some display options were already defined... 
	out = false;
      else {
	// ... or have been defined by another histogram instance in the meantime
	if(!m_hsdisp && checkHSDisplayFromDB())
	  out = false;
      }
    }
    else {
      if (m_InitDOmode >= HIST) // use page if some hist display options were already defined... 
	out = false;
      else {
	// ... or have been defined by another histogram instance in the meantime
	if(!m_hdisp && checkHDisplayFromDB())
	  out = false;
      }
    }
  }
  return out;
}

bool OnlineHistogram::initHistDisplayOptionsFromSet() {
  bool out=true;
  if (m_hsdisp == 0) load();
  if (m_hsdisp == 0) {
    cout << "Warning from OnlineHistogram::initHistDisplayOptionsFromSet :" <<
      " no Set display options available"<<endl;
    out = false;
  }
  else {
    if (m_hdisp != 0) 
      errorMessage("Warning from OnlineHistogram::initHistDisplayOptionsFromSet: HistDisplayOptions were already defined and will be reinitialized");
    if (m_domode != SET) getDisplayOptions(m_hsdisp);
    std::string function="DeclareHistDisplayOptions('"+m_hid+"'";
    int newhsdisp= setDisplayOptions(function);
    if(newhsdisp) {
      m_hdisp = newhsdisp;
      if (m_domode < HIST) m_domode = HIST;     
    }
    else 
      out= false;
  }
  return out;
}

bool OnlineHistogram::initHistoPageDisplayOptionsFromSet(std::string PageName,
							 int Instance) {
  bool out=true;
  
  if(Instance == -1) Instance=m_instance;
  if (PageName == "_DEFAULT_") PageName = m_page;
  if (!verifyPage(PageName,Instance)) {
    cout << "OnlineHistogram::initHistoPageDisplayOptionsFromSet aborted:" <<
	" page/instance specified are not correct"<<endl;
      out = false;
  }
  else {
    if (m_hsdisp == 0) load();
    if (m_hsdisp == 0) {
	  cout << "Warning from OnlineHistogram::initHistoPageDisplayOptionsFromSet :" <<
	    " non Set display options available"<<endl;
	  out = false;
	}
	else {
	  if (m_shdisp != 0) 
	    errorMessage("Warning from OnlineHistogram::initHistoPageDisplayOptionsFromSet: HistoPageDisplayOptions were already defined and will be reinitialized");
	  if (m_domode != SET) {
	    getDisplayOptions(m_hsdisp);
	    m_domode = SET;
	  }
	  stringstream function;
	  function << "DeclareHistoPageDisplayOptions(theHID => '" <<
	    m_hid << "',thePage => '" << PageName << "',TheInstance =>" << 
	    Instance;
	  int newshdisp=  setDisplayOptions(function.str());
	  if (newshdisp > 0) {
	    m_shdisp = newshdisp;
	    m_domode = HISTPAGE;
	    m_page = PageName;
	    m_instance = Instance;
	  }
	  else 
	    out= false;
	}
  }
  return out;
}

bool OnlineHistogram::initHistoPageDisplayOptionsFromHist(std::string PageName,
							  int Instance) {
  bool out=true;
  if(Instance == -1) Instance=m_instance;
  if (PageName == "_DEFAULT_") PageName = m_page;
  if (!verifyPage(PageName,Instance)) {
    cout << "OnlineHistogram::initHistoPageDisplayOptionsFromHist aborted:" <<
      " page/instance specified are not correct"<<endl;
    out = false;
  }
  else {
    if (m_hdisp == 0) load();
    if (m_hdisp == 0) {
      cout << "Warning from OnlineHistogram::initHistoPageDisplayOptionsFromSet :" <<
	" non Hist display options available"<<endl;
      out = false;
    }
    else {
      if (m_shdisp != 0) 
	errorMessage("Warning from OnlineHistogram::initHistoPageDisplayOptionsFromHist: HistoPageDisplayOptions were already defined and will be reinitialized");
      if (m_domode != HIST) {
	getDisplayOptions(m_hdisp);
	m_domode = HIST;
      }
      stringstream function;
      function << "DeclareHistoPageDisplayOptions(theHID => '" <<
	m_hid << "',thePage => '" << PageName << "',TheInstance =>" << 
	Instance;
      int newshdisp=  setDisplayOptions(function.str());
      if (newshdisp > 0) {
	m_shdisp = newshdisp;
	m_domode = HISTPAGE;
	m_page = PageName;
	m_instance = Instance;
      }
    }
  }
  return out;
}

bool OnlineHistogram::linkDisplayParameter(std::string& ParameterName,
					   Statement *st, 
					   int pos,
					   void* value) {
  bool out=true;
  OnlineDisplayOption* mydo=getDO(ParameterName);

  if (mydo) {
    std::string myvalue ="";
    switch (mydo->type()) {
    case OnlineDisplayOption::STRING :
      myvalue = *(static_cast<std::string*> (value));
      // never send empty strings since they are taken as NULL by PL/SQL
      if (myvalue == "") myvalue=" "; 
      st->setString(pos, myvalue);
      break;
    case OnlineDisplayOption::FLOAT :
      st->setFloat(pos,*(static_cast<float*> (value)));
      break;
    case OnlineDisplayOption::INT :
      st->setInt(pos,*(static_cast<int*> (value)));
      break;
    }
  }
  else {
    cout << "Error in OnlineHistogram::linkDisplayParameter : don't know parameter " 
	 << ParameterName <<endl;
    out= false;
  }
  return out;
}


bool OnlineHistogram::setHistoSetDisplayOption(std::string ParameterName, 
					       void* value) {
  bool out = true;
  int thedoid=0;
  stringstream statement;
  // update HistoSet DB options
  statement << "begin :out := OnlineHistDB.DeclareHistoSetDisplayOptions(:x1,K" << 
    ParameterName << " => :x2);end;";
  if(debug() > 3) cout << "Sending statement: "<<statement.str() << endl; 
  Statement *fstmt=m_conn->createStatement (statement.str());
  try{
    fstmt->registerOutParam(1, OCCIINT); 
    fstmt->setInt(2,m_hsid);
    if (!linkDisplayParameter(ParameterName,fstmt,3,value)) {
      m_conn->terminateStatement (fstmt); 
      out = false;
    }
    if (out) {
      fstmt->execute();
      if ((thedoid = fstmt->getInt(1)) >0) m_hsdisp = thedoid;
      else out = false;
    }
  }catch(SQLException ex)
    {
      dumpError(ex,"OnlineHistogram::setHistoSetDisplayOption for parameter "
		+ParameterName);
      out=false;
    }
  m_conn->terminateStatement (fstmt); 
  if( out ) {
    if(SET == m_domode) // update current option value
      getDO(ParameterName)->set(value);
    if(m_domode < SET) { // reload all current options from HistoSet
      m_domode = SET;
      getDisplayOptions(m_hsdisp);
    }
  }
  return out;
}

bool OnlineHistogram::setHistDisplayOption(std::string ParameterName, 
				       void* value) {
  bool out = true;
  int thedoid=0;
  stringstream statement;
  if( useSetForHistDisplay() ) // use the set if we don't need to be more specific
    out= setHistoSetDisplayOption(ParameterName, value);
  else {
    if(m_hdisp == 0 && m_hsdisp != 0)
      initHistDisplayOptionsFromSet();

    // update Hist DB options
    statement << "begin :out := OnlineHistDB.DeclareHistDisplayOptions(:x1,K" << 
      ParameterName << " => :x2);end;";
    Statement *fstmt=m_conn->createStatement (statement.str());
    try{
      fstmt->registerOutParam(1, OCCIINT); 
      fstmt->setString(2,m_hid);
      if (!linkDisplayParameter(ParameterName,fstmt,3,value)) {
	m_conn->terminateStatement (fstmt); 
	out = false;
      }
      if (out) {
	fstmt->execute();
	if ((thedoid = fstmt->getInt(1)) >0) m_hdisp = thedoid;
	else out = false;
      }
    }catch(SQLException ex)
      {
	dumpError(ex,"OnlineHistogram::setHistDisplayOption for parameter"+ParameterName);
	out = false;
      }
    m_conn->terminateStatement (fstmt); 
    if(out) { 
      if (HIST == m_domode) // update current option value
	getDO(ParameterName)->set(value);
      if (m_domode < HIST) {// reload all current options from Hist
	m_domode = HIST;
	getDisplayOptions(m_hdisp);
      }
    }
  }
  return out;
}


bool OnlineHistogram::setHistoPageDisplayOption(std::string ParameterName, 
						void* value,
						std::string PageName,
						int Instance) {
  bool out = true;
  if (Instance == -1) Instance=m_instance;
  if (PageName == "_DEFAULT_") PageName = m_page;
  if (!verifyPage(PageName,Instance)) {
    errorMessage("OnlineHistogram::setHistoPageDisplayOption aborted: bad page/instance");
    out = false;
  }				 
  else {
    int thedoid=0;
    
    if (m_shdisp == 0) {
      if(m_hdisp != 0)
	initHistoPageDisplayOptionsFromHist(PageName,Instance);
      else if (m_hsdisp != 0)
	initHistoPageDisplayOptionsFromSet(PageName,Instance);
    }

    stringstream statement;
    statement << "begin :out := OnlineHistDB.DeclareHistoPageDisplayOptions(:x1,:x2,:x3,K" 
	      << ParameterName << " => :x4);end;";
    Statement *fstmt=m_conn->createStatement (statement.str());
    try{
      fstmt->registerOutParam(1, OCCIINT);
      fstmt->setString(2,m_hid);
      fstmt->setString(3,PageName);
      fstmt->setInt(4,Instance);
      if (!linkDisplayParameter(ParameterName,fstmt,5,value)) {
	m_conn->terminateStatement (fstmt); 
	out = false;
      }
      if (out) {
	fstmt->execute();
	if ((thedoid = fstmt->getInt(1)) >0) m_shdisp = thedoid;
	else out = false;
      }
    }catch(SQLException ex)
      {
	dumpError(ex,"OnlineHistogram::setHistoPageDisplayOption for parameter "+ParameterName);
	out = false;
      }
    m_conn->terminateStatement (fstmt); 
    if (out) {
      if (HISTPAGE == m_domode && PageName == m_page && Instance == m_instance) // update current option value
	getDO(ParameterName)->set(value);
      else { // reload all current options from HistoPage
	m_domode = HISTPAGE;
	m_page = PageName;
	m_instance = Instance;
	getDisplayOptions(m_shdisp);
      }
    }
  }
  return out;
}


bool OnlineHistogram::setDisplayOption(std::string ParameterName, 
					       void* value) {
  bool out;
  if (useHistForPageDisplay()) // use the hist  if we don't need to be page-specific
    out=setHistDisplayOption(ParameterName,value);
  else
    out=setHistoPageDisplayOption(ParameterName,value);
  return out;
}



bool OnlineHistogram::unsetHistoSetDisplayOption(std::string ParameterName) {
  bool out = true;
  OnlineDisplayOption* mydo= getDO(ParameterName);
  if (!mydo) {
    errorMessage("OnlineHistogram::unsetHistoSetDisplayOption : not existing Parameter "+ParameterName);
    out=false;
  }  
  if (!m_hsdisp) {
    stringstream error;
    error << "OnlineHistogram::unsetHistoSetDisplayOption : no display options defined for set " << m_hsid;
    errorMessage(error.str());
    out=false;
  }
  if(out) {
    int curdoid=0;
    if (SET !=  m_domode) { 
      // get current histogram set options
      if (HIST == m_domode) curdoid = m_hdisp;
      if (HISTPAGE  == m_domode) curdoid = m_shdisp;
      getDisplayOptions(m_hsdisp);
    }
    mydo->unset();
    stringstream function;
    function << "DeclareHistoSetDisplayOptions('" << m_hsid << "'";
    setDisplayOptions(function.str());
    if (SET !=  m_domode) { 
      // reload options for current mode
      getDisplayOptions(curdoid);
    } 
  }
  return out;
}


bool OnlineHistogram::unsetHistDisplayOption(std::string ParameterName) {
  bool out = true;
  if(0 == m_hdisp && m_hsdisp) // take the set 
    out= unsetHistoSetDisplayOption(ParameterName);
  else {
    OnlineDisplayOption* mydo= getDO(ParameterName);
    
    if (!mydo) {
      errorMessage("OnlineHistogram::unsetDisplayOption : not existing Parameter "+ParameterName);
      out=false;
    }  
    if (!m_hdisp) {
      stringstream error;
      error << "OnlineHistogram::unsetDisplayOption : no display options defined for histogram  " << m_hid;
      errorMessage(error.str());
      out=false;
    }
    if(out) {
      int curdoid=0;
      if (HIST !=  m_domode) { 
	// get current histogram options
	if (HISTPAGE  == m_domode) curdoid = m_shdisp;
	getDisplayOptions(m_hdisp);
      }
      mydo->unset();
      stringstream function;
      function << "DeclareHistDisplayOptions('" << m_hid << "'";
      setDisplayOptions(function.str());
      if (HIST !=  m_domode) { 
	// reload options for current mode
	getDisplayOptions(curdoid);
      } 
    }
  }
  return out;
}

bool OnlineHistogram::unsetHistoPageDisplayOption(std::string ParameterName)
{
  bool out = true;
  if (m_page == "_NONE_") {
    errorMessage("OnlineHistogram::unsetHistoPageDisplayOption aborted: histogram is not on a page");
    out = false;
  }				 
  else {
    OnlineDisplayOption* mydo= getDO(ParameterName);    
    if (!mydo) {
      errorMessage("OnlineHistogram::unsetHistoPageDisplayOption : not existing Parameter "+ParameterName);
      out=false;
    }  
    if (!m_shdisp) {
      stringstream error;
      error << "OnlineHistogram::unsetHistoPageDisplayOption : no display options defined for histogram " << m_hid;
      errorMessage(error.str());
      out=false;
    }
    if(out) {
      if (HISTPAGE != m_domode) { // should never happen
	getDisplayOptions(m_shdisp);
	m_domode = HISTPAGE;
      }
      mydo->unset();
      stringstream function;
      function << "DeclareHistoPageDisplayOptions(theHID => '" <<
	m_hid << "',thePage => '" << m_page << "',TheInstance =>" << 
	m_instance;
      setDisplayOptions(function.str());
    }
  }
  return out;
}

bool OnlineHistogram::unsetDisplayOption(std::string ParameterName) {
  bool out = true;
  if (HISTPAGE == m_domode)
    out = unsetHistoPageDisplayOption(ParameterName);
  else
    out = unsetHistDisplayOption(ParameterName);
  return out;
}

bool OnlineHistogram::getDisplayOption(std::string ParameterName,
				       void* option) 
{
  bool out=true;
  OnlineDisplayOption* mydo= getDO(ParameterName);
  if (mydo) {
    if (mydo->isSet()) {
      switch (mydo->type()) {
      case OnlineDisplayOption::STRING :
	*(static_cast<std::string*>(option)) = 
	  *(static_cast<const std::string*>(mydo->value()));
	break;
      case OnlineDisplayOption::FLOAT :
	*(static_cast<float*>(option)) = 
	  *(static_cast<const float*>(mydo->value()));
	break;
      case OnlineDisplayOption::INT :
	*(static_cast<int*>(option)) = 
	  *(static_cast<const int*>(mydo->value()));
	break;
      }
    }
    else
      out=false;
  }
  else {
    cout << "Warning from OnlineHistogram::getDisplayOption :" <<
      "parameter " << ParameterName << " is unknown"<<endl;
    out=false;
  }
  return out;
}

OnlineHistogram::OnlineDisplayOption::~OnlineDisplayOption() {
  unset();
}

void OnlineHistogram::OnlineDisplayOption::set(void *value){
  switch (m_type) {
  case STRING :
    set(*(static_cast<std::string*>(value)));
    break;
  case FLOAT :
    set(*(static_cast<float*>(value)));
    break;
  case INT :
    set(*(static_cast<int*>(value)));
    break;
  }
}

void OnlineHistogram::OnlineDisplayOption::set(std::string value) { 
  if (m_type == STRING && value != "_unset_") {
    if ( value == "") value = " ";
    if(m_value) *(static_cast<std::string*>(m_value)) = value;
    else m_value = new std::string(value);
  }
}
void OnlineHistogram::OnlineDisplayOption::set(int value) { 
  if (m_type == INT && value != -99999) {
    if(m_value) *(static_cast<int*>(m_value)) = value;
    else m_value = new int(value);
  }
}
void OnlineHistogram::OnlineDisplayOption::set(float value) { 
  if (m_type == FLOAT && fabs(value+999999.)>0.1 ) {
    if(m_value) *(static_cast<float*>(m_value)) = value;
    else m_value = new float(value);
  }
}

void OnlineHistogram::OnlineDisplayOption::unset() {
  if(m_value) {
    if (m_type == STRING) delete  static_cast<std::string*>(m_value);
    if (m_type == FLOAT) delete  static_cast<float*>(m_value);
    if (m_type == INT) delete  static_cast<int*>(m_value);
    m_value = 0;
  }
}


OnlineHistogram::OnlineDisplayOption* OnlineHistogram::getDO(std::string ParameterName) {
  OnlineDisplayOption* out=0;
  std::vector<OnlineDisplayOption*>::iterator ip;
  for (ip = m_do.begin();ip != m_do.end(); ++ip) {
    if((*ip)->name() == ParameterName) {
      out=*ip;
      break;
    }
  }
  return out;
}

void OnlineHistogram::dump() {
  cout<<" ----- HISTOGRAM RECORD " << m_hid << "------------------"<<endl;
  cout<<" Pointer "<< this <<endl;
  cout<<" Identifier: "<< m_identifier<<endl;
  if (m_page != "_NONE_")
    cout<<" attached to Page :" << m_page << " instance "<<m_instance <<endl;
  dumpDisplayOptions();
}

void OnlineHistogram::dumpDisplayOptions() {
  std::vector<OnlineDisplayOption*>::iterator ip;
  cout<<" ----- Display Options for Histogram "<<m_identifier << " -----"<<endl;
  cout<<"   Mode : " << m_domode <<" HSDISP="<<m_hsdisp<<" HDISP="<<m_hdisp
      <<" SHDISP="<<m_shdisp<<endl;
  for (ip = m_do.begin();ip != m_do.end(); ++ip) {
    cout << "    " <<  (*ip)->name() << " : ";
    if ((*ip)->isSet()) {
      if((*ip)->type() == OnlineDisplayOption::STRING) 
	cout << *(static_cast<const std::string*>((*ip)->value()));
      if((*ip)->type() == OnlineDisplayOption::FLOAT)
	cout << *(static_cast<const float*>((*ip)->value()));
      if((*ip)->type() == OnlineDisplayOption::INT)
	cout << *(static_cast<const int*>((*ip)->value()));
    }
    else
      cout << "unset";
    cout <<endl;
  }
}





// analysys methods

int OnlineHistogram::algNpars(std::string* Algorithm,
			      int* anaid) const {
  int Np=0;
  ResultSet *rset=0;
  std::string sqlStmt;
  if (Algorithm) 
    sqlStmt = "SELECT npars FROM algorithm where algname=:n";
  else
    sqlStmt = "SELECT npars FROM algorithm al,analysis an where al.algname=an.algorithm and an.aid=:n";
  Statement *query = m_conn->createStatement(sqlStmt);
  if (Algorithm)
    query->setString(1, *Algorithm);
  else
    query->setInt(1,*anaid);
  try{
    rset =  query->executeQuery ();
    if (rset->next ()) 
      Np=rset->getInt (1);
    else {
      cout<<"Error in OnlineHistogram::algNpars :";
      if (Algorithm) cout << " Algorithm "<<Algorithm;
      else cout << " Analysis " << anaid;
      cout <<" unknown to DB"<<endl;
      Np=-1;
    }
  } catch(SQLException ex)
    {
      dumpError(ex,"OnlineHistogram::algNpars");
      Np=-1;
    }
  query->closeResultSet (rset);
  m_conn->terminateStatement (query);
  return Np;
}




int OnlineHistogram::formatThresholds(std::string* Algorithm,
				      int* anaid,
				      std::vector<float>* warningThr, 
				      std::vector<float>* alarmThr,
				      std::string * warnString, 
				      std::string* alarmString) const {
  bool badinput = false;
  int Np=algNpars(Algorithm,anaid);
  if (Np<0) badinput = true;
  if (Np>0) { 
    if (0 == warningThr || 0 == alarmThr || 
	warningThr->size() != (unsigned int)Np || 
	alarmThr->size() != (unsigned int)Np) {
      cout<<"Error in OnlineHistogram::formatThresholds :";
      if (Algorithm) cout << " Algorithm "<<Algorithm;
      else cout << " Analysis " << anaid;
      cout << " requires "<<Np<<" parameters"<<endl;
      badinput = true;
      Np=-1;
    }
  }
  
  if (false == badinput) {
    stringstream wstst,astst;
    wstst<<"thresholds(";
    astst<<"thresholds(";
    int ipar=0;
    while (ipar++<Np) {
      wstst << ":w" << ipar;
      astst << ":a" << ipar;
      if(ipar<Np) {
	wstst << ",";
	astst << ",";
      }
    } 
    wstst << ")";
    astst << ")";
    *warnString=wstst.str();
    *alarmString=astst.str();
  }
  return Np;
}

bool OnlineHistogram::getCreationDirections(std::string &Algorithm,
					    std::vector<std::string> &source_list,
					    std::vector<float> &parameters) {
  bool out = false;
  if (m_isAnaHist) { 
    if (loadCreationDirections()) {
      Algorithm = CreationAlgorithm;
      source_list.clear();
      if (Sources.size() > 0) 
	source_list.insert( source_list.begin(),  
			    Sources.begin(), Sources.end() );
      parameters.clear();
      if(SourcePar.size() > 0) 
	parameters.insert( parameters.begin(),  
			   SourcePar.begin(), SourcePar.end() );
	
      out = true;
    }
  }
  return out;
}


int OnlineHistogram::declareAnalysis(std::string Algorithm, 
				     std::vector<float>* warningThr, 
				     std::vector<float>* alarmThr, 
				     int instance ) {
  // check that Algorithm exists and get number of input parameters
  int out=0;
  bool known = false;
  std::string warnString,alarmString;
  int Np=formatThresholds(&Algorithm,0,warningThr,alarmThr,&warnString,&alarmString);
  
  if (Np>=0) {
    stringstream sstatement;
    sstatement << "begin :out := OnlineHistDB.DeclareAnalysis(:x1,:x2,"
	       << warnString << ","  << alarmString << "," << instance 
	       << "); end;" ;
    
    Statement *astmt=m_conn->createStatement (sstatement.str());
    try{
      astmt->registerOutParam(1, OCCIINT); 
      astmt->setInt(2, m_hsid);
      astmt->setString(3, Algorithm);
      for (int ipar=0; ipar<Np ; ipar++) {
	astmt->setFloat(4+ipar, (*warningThr)[ipar]);
	astmt->setFloat(4+Np+ipar, (*alarmThr)[ipar]);
      }
      astmt->execute();
      out=astmt->getInt(1);
    }catch(SQLException ex)
      {
	dumpError(ex,"OnlineHistogram::DeclareAnalysis for Algorithm "+Algorithm);
	out=0;
      }
    m_conn->terminateStatement (astmt);  
    for (int ia=0;ia<m_nanalysis;ia++) {
      if (m_anaId[ia] == out) { // analysis already known
	known = true;
	break;
      }
    }
    if (!known) {
      m_anaId.push_back(out);
      m_anaName.push_back(Algorithm);
      m_nanalysis++;
    }
  }
  return out;
}

bool OnlineHistogram::setAnalysis(int AnaID, 
				  std::vector<float>* warningThr, 
				  std::vector<float>* alarmThr)
{
  bool out=true;
  std::string warnString,alarmString;
  int Np=formatThresholds(0,&AnaID,warningThr,alarmThr,&warnString,&alarmString);
  if (Np>=0) {
    stringstream statement;
    statement << "begin OnlineHistDB.SetSpecialAnalysis("<< AnaID <<",'" << 
      m_hsid << "/" << m_ihs << "',"<< warnString << "," << alarmString <<
      "); end;";
    
    Statement *astmt=m_conn->createStatement (statement.str());
    try{
      for (int ipar=0; ipar<Np ; ipar++) {
	astmt->setFloat(1+ipar, (*warningThr)[ipar]);
	astmt->setFloat(1+Np+ipar, (*alarmThr)[ipar]);
      }
      astmt->execute();
    }catch(SQLException ex) {
      dumpError(ex,"OnlineHistogram::setAnalysis");
      out = false;
    }
    m_conn->terminateStatement (astmt);  
  }
  return out;
}

bool OnlineHistogram::getAnaSettings(int AnaID,
				     std::vector<float>* warn, 
				     std::vector<float>* alarm) const {
  bool out=true;
  int Np=algNpars(0,&AnaID);
  if (Np>0) { 
    if (0 == warn || 0 == alarm) {
      cout<<"Error in OnlineHistogram::getAnaSettings : Analysis " << AnaID;
      cout << " requires "<<Np<<" parameters"<<endl;
      Np=-1;
      out=false;
    }
    else {
      warn->clear();
      alarm->clear();
    }
  }
  
  if (out) {
    stringstream statement;
    statement << "begin OnlineHistDB.GetAnaSettings("<<AnaID << ",'" <<
      m_hid << "',:1,:2,:3); end;";
    Statement *astmt=m_conn->createStatement (statement.str());
    for (int jp=0 ; jp<Np ; jp++) {
      try{
	astmt->setInt(1,jp+1);
	astmt->registerOutParam(2, OCCIFLOAT); 
	astmt->registerOutParam(3, OCCIFLOAT); 
	astmt->execute();
	warn->push_back(astmt->getFloat(2));
	alarm->push_back(astmt->getFloat(3));
      }catch(SQLException ex)
	{
	  dumpError(ex,"OnlineHistogram::getAnaSettings");
	  out=false;
	}
    }
    m_conn->terminateStatement (astmt);  
  }
  return out;
}


bool OnlineHistogram::maskAnalysis(int AnaID,bool Mask) {
  bool out=true;
  Statement *astmt=m_conn->createStatement 
    ("update ANASETTINGS set MASK=:1 where ANA=:2 and HISTO=:3");
  astmt->setInt(1,(int)Mask);
  astmt->setInt(2,AnaID);
  astmt->setString(3,m_hid);
  try{
    astmt->execute();
  }catch(SQLException ex)
    {
      dumpError(ex,"OnlineHistogram::maskAnalysis");
      out=false;
    }
  return out;
}






OnlineHistogramStorage::OnlineHistogramStorage(OnlineHistDBEnv* Env) :
  m_Histenv(Env), m_avoid_hdup(true)  {}

OnlineHistogramStorage::~OnlineHistogramStorage() 
{
  if (m_Histenv->debug() > 2) cout << "Deleting "<<
    m_myHist.size() << " OnlineHistogram objects"<<endl;
  std::vector<OnlineHistogram*>::iterator ih;
  for (ih = m_myHist.begin();ih != m_myHist.end(); ++ih) 
    delete *ih; 
}

void OnlineHistogramStorage::updateHistograms() {
  std::vector<OnlineHistogram*>::iterator ih;
  for (ih = m_myHist.begin();ih != m_myHist.end(); ++ih) 
    (*ih)->checkServiceName(); 
}

OnlineHistogram* OnlineHistogramStorage::getHistogram(std::string Identifier, 
					    std::string Page,
					    int Instance) {
  OnlineHistogram* h=0;
  if (m_avoid_hdup) {
    std::string folder="_NONE_";
    // see if the histogram object exists already
    std::vector<OnlineHistogram*>::iterator ih;
    for (ih = m_myHist.begin(); ih != m_myHist.end(); ++ih) {
      if (Page != "_NONE_" && folder=="_NONE_") Page = (*ih)->PagenameSyntax(Page, folder);
      if ((*ih)->identifier() == Identifier && (*ih)->page() == Page && 
	  (*ih)->instance() == Instance ) {
	h = *ih;
	break;
      }
    }
  }
  if (!h) {
    h= new OnlineHistogram(*m_Histenv,Identifier,Page,Instance);
    if (h->isAbort()) {
      cout<<"Error from OnlineHistDB::getHistogram : cannot create histogram object " 
	  << Identifier <<endl;
      delete h;
      h=0;
    }
    else 
      m_myHist.push_back(h);
  }
  return h;
}

OnlineHistogram* OnlineHistogramStorage::getNewHistogram(std::string Identifier, 
							 std::string Page,
							 int Instance) {
  OnlineHistogram* h=0;
  m_avoid_hdup = false;
  h = getHistogram(Identifier, Page, Instance);
  m_avoid_hdup = true;
  return h;
}

bool OnlineHistogramStorage::removeHistogram(OnlineHistogram* h,
				   bool RemoveWholeSet) {
  bool out=h->remove(RemoveWholeSet);
  if (out) {
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

