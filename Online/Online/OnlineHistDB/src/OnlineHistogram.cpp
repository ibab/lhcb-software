// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/OnlineHistDB/src/OnlineHistogram.cpp,v 1.6 2007-07-09 17:29:59 ggiacomo Exp $
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
  OnlineHistDBEnv(env), m_isAbort(false), m_identifier(Identifier), 
  m_page(Page), m_instance(Instance),
  m_domode(NONE), m_hsdisp(0), m_hdisp(0), m_shdisp(0) {
    load();
  }

void OnlineHistogram::update() {
  // update the object from DB if the NIM service name has changed (or is not found)
  ResultSet *rset;
  Statement *query = m_conn->createStatement("SELECT SN FROM DIMSERVICENAME WHERE PUBHISTO=:1");
  query->setString(1,m_hid);
  rset =  query->executeQuery ();
  if(rset->next ()) {
    if (rset->getString(1) != m_dimServiceName) load();
  }
  else
    load();
}

void OnlineHistogram::setPage(std::string Page,
			      int Instance) {
  if ( Page != m_page && Instance != m_instance) {
    m_page =Page;
    m_instance =Instance;
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
  if (!(query->getInt(1))) 
    m_isAbort=true;
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
    } // end of analysis part
    
    // display options
    createDisplayOptions();
  }
}

void OnlineHistogram::createDisplayOptions() {
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

bool OnlineHistogram::initDisplayOptionsFromSet() {
  bool out=true;
  if (m_hsdisp == 0) load();
  if (m_hsdisp == 0) {
    cout << "Warning from OnlineHistogram::initDisplayOptionsFromSet :" <<
      " no Set display options available"<<endl;
    out = false;
  }
  else {
    //if (m_hdisp != 0) {
    //  cout << "OnlineHistogram::initDisplayOptionsFromSet aborted:" <<
    //	" special display options already defined"<<endl;
    //  out = false;
    //}
    //else {
    if (m_domode != SET) getDisplayOptions(m_hsdisp);
    m_domode = HIST;
    std::string function="DeclareHistDisplayOptions('"+m_hid+"'";
    m_hdisp = setDisplayOptions(function);
    //}
  }
  return out;
}

bool OnlineHistogram::initHistoPageDisplayOptionsFromSet(std::string PageName,
							 int Instance) {
  bool out=true;
  if(Instance == -1) Instance=m_instance;
  if (PageName == "_DEFAULT_") PageName = m_page;
  if (PageName == "_NONE_") {
    cout << "OnlineHistogram::initHistoPageDisplayOptionsFromSet aborted:" <<
	" no page specified"<<endl;
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
//        if (m_shdisp != 0) {
// 	    cout << "OnlineHistogram::initHistoPageDisplayOptionsFromSet aborted:" <<
// 	      " special HistoPage display options already defined"<<endl;
// 	    out = false;
// 	  }
// 	  else {
	  if (m_domode != SET) {
	    getDisplayOptions(m_hsdisp);
	    m_domode = SET;
	  }
	  stringstream function;
	  function << "DeclareHistoPageDisplayOptions(theHID => '" <<
	    m_hid << "',thePage => '" << PageName << "',TheInstance =>" << 
	    Instance;
	  if ((m_shdisp=setDisplayOptions(function.str())) > 0) {
	    m_domode = HISTPAGE;
	    m_page = PageName;
	    m_instance = Instance;
	  }
//	  }
	}
  }
  return out;
}

bool OnlineHistogram::initHistoPageDisplayOptionsFromHist(std::string PageName,
							  int Instance) {
  bool out=true;
  if(Instance == -1) Instance=m_instance;
  if (PageName == "_DEFAULT_") PageName = m_page;
  if (PageName == "_NONE_") {
    cout << "OnlineHistogram::initHistoPageDisplayOptionsFromHist aborted:" <<
      " no page specified"<<endl;
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
// 	  if (m_shdisp != 0) {
// 	    cout << "OnlineHistogram::initHistoPageDisplayOptionsFromHist aborted:" <<
// 	      " special HistoPage display options already defined"<<endl;
// 	    out = false;
// 	  }
// 	  else {
      if (m_domode != HIST) {
	getDisplayOptions(m_hdisp);
	m_domode = HIST;
      }
      stringstream function;
      function << "DeclareHistoPageDisplayOptions(theHID => '" <<
	m_hid << "',thePage => '" << PageName << "',TheInstance =>" << 
	Instance;
      if ((m_shdisp=setDisplayOptions(function.str())) > 0) {
	m_domode = HISTPAGE;
	m_page = PageName;
	m_instance = Instance;
      }
    }
// 	}
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
    }
  }catch(SQLException ex)
    {
      dumpError(ex,"OnlineHistogram::setHistoSetDisplayOption for parameter "
		+ParameterName);
      out=false;
    }
  m_conn->terminateStatement (fstmt); 
  if( out && m_domode <= SET && thedoid > 0) {
    m_domode = SET;
    getDisplayOptions(thedoid);
  }
  return out;
}

bool OnlineHistogram::setDisplayOption(std::string ParameterName, 
				       void* value) {
  bool out = true;
  int thedoid=0;
  stringstream statement;
  if(m_nhs ==1) // take the set and never mind
    out= setHistoSetDisplayOption(ParameterName, value);
  else {
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
      }
    }catch(SQLException ex)
      {
	dumpError(ex,"OnlineHistogram::setDisplayOption for parameter"+ParameterName);
	out = false;
      }
    m_conn->terminateStatement (fstmt); 
    if(out && m_domode <= HIST && thedoid > 0) { // update current display options
      m_domode = HIST;
      getDisplayOptions(thedoid);
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
  if (PageName == "_NONE_") {
    cout << "OnlineHistogram::setHistoPageDisplayOption aborted:" <<
      " no page specified"<<endl;
    out = false;
  }				 
  else {
    int thedoid=0;
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
      }
    }catch(SQLException ex)
      {
	dumpError(ex,"OnlineHistogram::setHistoPageDisplayOption for parameter "+ParameterName);
	out = false;
      }
    m_conn->terminateStatement (fstmt); 
    if(out && m_domode <= HISTPAGE  && thedoid > 0 ) { // update current display options
      m_domode = HISTPAGE;
      m_page = PageName;
      m_instance = Instance;
      getDisplayOptions(thedoid);
    }
  }
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


bool OnlineHistogram::unsetDisplayOption(std::string ParameterName) {
  bool out = true;
  if(m_nhs ==1) // take the set and never mind
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
    if (0 == warn || 0 == alarm || 
	warn->size() != (unsigned int)Np || 
	alarm->size() != (unsigned int)Np) {
      cout<<"Error in OnlineHistogram::getAnaSettings : Analysis " << AnaID;
      cout << " requires "<<Np<<" parameters"<<endl;
      Np=-1;
      out=false;
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
	warn->at(jp)=astmt->getFloat(2);
	alarm->at(jp)=astmt->getFloat(3);
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
  m_Histenv(Env) {}

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
    (*ih)->update(); 
}

OnlineHistogram* OnlineHistogramStorage::getHistogram(std::string Identifier, 
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
