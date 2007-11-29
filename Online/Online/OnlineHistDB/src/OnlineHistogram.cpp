// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/OnlineHistDB/src/OnlineHistogram.cpp,v 1.26 2007-11-29 11:22:22 ggiacomo Exp $
/*
   C++ interface to the Online Monitoring Histogram DB
   G. Graziani (INFN Firenze)

*/



#include <math.h>

#include "OnlineHistDB/OnlineHistogram.h"
using namespace OnlineHistDBEnv_constants;
OnlineHistogram::OnlineHistogram(OnlineHistDBEnv &env,
				 std::string Identifier,
				 std::string Page,
				 int Instance) :
  OnlineHistDBEnv(env), SourceSet(0),
  m_isAbort(false), m_identifier(Identifier), 
  m_page(Page), m_instance(Instance), m_page_verified(false),
  m_DOinit(false), m_InitDOmode(NONE), m_domode(NONE), m_hsdisp(0), m_hdisp(0), m_shdisp(0),
  m_dispopt(NULL), m_dispopt_null(NULL),
  m_anadirLoaded(false), m_credirLoaded(false) {
    if (m_page  != "_NONE_") {
      if (!verifyPage(m_page,Instance)) {
	std::stringstream error;
	error << "Instance "<<Instance<<" of Histogram "<<m_identifier<<" not found on page "<<m_page;
	if (debug()) warningMessage(error.str());
	m_page = "_NONE_";
	m_instance =1;
      }
    }
    load();
      
  }

void OnlineHistogram::checkServiceName() {
  // update the object from DB if the NIM service name has changed 
  text currentSN[VSIZE_SN]="";
  OCIStmt *stmt=NULL;
  bool unchanged=false;
  m_StmtMethod = "OnlineHistogram::checkServiceName";
  if ( OCI_SUCCESS == prepareOCIStatement(stmt, "SELECT SN FROM DIMSERVICENAME WHERE PUBHISTO=:1") ) {
    myOCIBindString(stmt,":1", m_hid);
    myOCIDefineString(stmt, 1, currentSN , VSIZE_SN);
    if (OCI_SUCCESS == myOCIStmtExecute(stmt)) {
      if(std::string((char *) currentSN) == m_dimServiceName) 
	unchanged=true;
    }
    releaseOCIStatement(stmt);
  }
  if (!unchanged) {
    if (debug() > 3) cout << "Reloading Histogram " << identifier() << endl;
    load();
  }
}

bool OnlineHistogram::setPage(std::string Page,
			      int Instance) {
  bool out=true;
  if ( Page != m_page || Instance != m_instance) {
    out=verifyPage(Page,Instance); // Page may change
    if (out && (Page != m_page || Instance != m_instance)) {
      m_page = Page;
      m_instance = Instance;
      m_page_verified = true;
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
  int out=0;
  OCIStmt *stmt=NULL;
  m_StmtMethod = "OnlineHistogram::setDimServiceName";
  if ( OCI_SUCCESS == prepareOCIStatement
       (stmt, "BEGIN :out := ONLINEHISTDB.SETDIMSERVICENAME(:2,:3); END;") ) {
    myOCIBindInt   (stmt,":out", out);  
    myOCIBindString(stmt,":2", m_hid);  
    myOCIBindString(stmt,":3", DimServiceName);
    myOCIStmtExecute(stmt);
    releaseOCIStatement(stmt);
  }
  return (out>0);
}


bool OnlineHistogram::setDoc(std::string doc) {
  bool out=false;
  OCIStmt *stmt=NULL;
  if (doc.size() > (VSIZE_DOC-1) ) {
    warningMessage("doc too long for "+m_identifier);
  }
  else {
    m_StmtMethod = "OnlineHistogram::setDoc";
    if ( OCI_SUCCESS == prepareOCIStatement(stmt,"UPDATE HISTOGRAMSET set DOC=:1 where HSID=:2" ) ) {
      myOCIBindString(stmt,":1", doc);
      myOCIBindInt   (stmt,":2", m_hsid);
      out = (OCI_SUCCESS == myOCIStmtExecute(stmt));
      releaseOCIStatement(stmt);
    }
  }
  return out;
}


bool OnlineHistogram::setDescr(std::string descr) {
  bool out=false;
  OCIStmt *stmt=NULL;
  if (descr.size() > (VSIZE_DESCR-1) ) {
    warningMessage("descr too long for "+m_identifier);
  }
  else {
    m_StmtMethod = "OnlineHistogram::setDescr";
    if ( OCI_SUCCESS == prepareOCIStatement(stmt,"UPDATE HISTOGRAMSET set DESCR=:1 where HSID=:2" ) ) {
      myOCIBindString(stmt,":1", descr);
      myOCIBindInt   (stmt,":2", m_hsid);
      out = (OCI_SUCCESS == myOCIStmtExecute(stmt));
      releaseOCIStatement(stmt);
    }
  }
  return out;
}


bool OnlineHistogram::verifyPage(std::string &Page, int Instance) {
  // check that page exists and histogram is on it
  bool out=false;
  if (m_page_verified && Page == m_page && Instance == m_instance) {
    out = true;
  }
  else {
    std::string fold;
    Page = PagenameSyntax(Page, fold);
    if (Page != "_NONE_") {
      OCIStmt *stmt=NULL;
      m_StmtMethod = "OnlineHistogram::verifyPage";
      if ( OCI_SUCCESS == prepareOCITaggedStatement
	   (stmt, "SELECT COUNT(*) FROM SHOWHISTO SH ,VIEWHISTOGRAM VH WHERE VH.NAME=:1 AND SH.HISTO=VH.HID AND SH.PAGE=:2 AND SH.INSTANCE=:3",
	    "VRFPAGE") ) {
	int Nc;
	myOCIBindString(stmt,":1", m_identifier);
	myOCIBindString(stmt,":2", Page);
	myOCIBindInt   (stmt,":3", Instance);  
	myOCIDefineInt (stmt, 1, Nc);
	if (OCI_SUCCESS == myOCIStmtExecute(stmt)) {
	  if (Nc>0) {
	    out=true;
	    if (Page == m_page && Instance == m_instance) m_page_verified = true;
	  }
	}
	releaseOCITaggedStatement(stmt, "VRFPAGE");
      }
    }
  }
  return out;
}

void OnlineHistogram::load() {
  int out=0;
  m_anadirLoaded = m_credirLoaded = false;
  OCIStmt *stmt=NULL;
  m_StmtMethod = "OnlineHistogram::load";
  std::string command = 
      "BEGIN :out := ONLINEHISTDB.GETHISTOGRAMDATA(:nm,:pg,:in,:hid,:hsd,:ihs,:nhs,:ty,:hst,:sub,:tk,:alg,:na,:des,:doc,:isa,:cre,:obs,:dis,:hdi,:shd, :dsn); END;";
  if (OCI_SUCCESS == prepareOCITaggedStatement(stmt, command.c_str(), "HLOAD") ) {

    text  hid[VSIZE_HID]="";
    text  ty[VSIZE_TYPE]="";
    text  hst[VSIZE_HSTITLE]="";
    text  sub[VSIZE_SUBTITLE]="";
    text  tk[VSIZE_HSTASK]="";
    text  alg[VSIZE_HSALG]="";
    text  des[VSIZE_DESCR]="";
    text  doc[VSIZE_DOC]="";
    text  dsn[VSIZE_SN]="";
    int isAnaHist=0;
    myOCIBindInt   (stmt,":out", out);  
    myOCIBindString(stmt,":nm" , m_identifier);
    myOCIBindString(stmt,":pg" , m_page);
    myOCIBindInt   (stmt,":in" , m_instance);
    myOCIBindString(stmt,":hid", hid, VSIZE_HID);
    myOCIBindInt   (stmt,":hsd", m_hsid);
    myOCIBindInt   (stmt,":ihs", m_ihs);
    myOCIBindInt   (stmt,":nhs", m_nhs);
    myOCIBindString(stmt,":ty" , ty, VSIZE_TYPE);
    myOCIBindString(stmt,":hst", hst, VSIZE_HSTITLE);
    myOCIBindString(stmt,":sub", sub, VSIZE_SUBTITLE, &m_subname_null);
    myOCIBindString(stmt,":tk" , tk, VSIZE_HSTASK);
    myOCIBindString(stmt,":alg", alg, VSIZE_HSALG);
    myOCIBindInt   (stmt,":na" , m_nanalysis);
    myOCIBindString(stmt,":des", des, VSIZE_DESCR, &m_Descr_null);
    myOCIBindString(stmt,":doc", doc, VSIZE_DOC, &m_Doc_null);
    myOCIBindInt   (stmt,":isa", isAnaHist);
    myOCIBindInt   (stmt,":cre", m_creation);
    myOCIBindInt   (stmt,":obs", m_obsoleteness, &m_obsoleteness_null);
    myOCIBindInt   (stmt,":dis", m_hdisp, &m_hdisp_null);
    myOCIBindInt   (stmt,":hdi", m_hsdisp, &m_hsdisp_null);
    myOCIBindInt   (stmt,":shd", m_shdisp, &m_shdisp_null);
    myOCIBindString(stmt,":dsn", dsn, VSIZE_SN, &m_dimServiceName_null);

    sword status=myOCIStmtExecute(stmt);

    if (OCI_SUCCESS == status) {
      if (0 == out) {
	m_isAbort=true;
      }
      else {
	m_hid= std::string((const char *) hid);
	m_hstype=std::string((const char *) ty);
	m_type=OnlineHistDBEnv::H1D;
	for (int it=0; it<NHTYPES; it++) {
	  if (m_hstype == std::string(HistTypeName[it])) m_type=(OnlineHistDBEnv::HistType)it;
	}
	m_hsname=std::string((const char *) hst);
	m_subname= m_subname_null ?  "" : std::string((const char *) sub);
	m_task=std::string((const char *) tk);
	m_algorithm=std::string((const char *) alg);
	m_Descr= m_Descr_null ? "" : std::string((const char *) des);
	m_Doc=m_Doc_null ? "" : std::string((const char *) doc);
	if (m_obsoleteness_null) m_obsoleteness=0;
	if (m_hdisp_null) m_hdisp = 0;
	if (m_hsdisp_null) m_hsdisp = 0;
	if (m_shdisp_null) m_shdisp = 0;
	m_dimServiceName = m_dimServiceName_null ? "" : std::string((const char *) dsn);
	m_isAnaHist = (isAnaHist>0);
      }
      
    }
    else {
      m_isAbort=true;
    }
    releaseOCITaggedStatement(stmt, "HLOAD");
    
    if (!m_isAbort) {
      // display options
      createDisplayOptions();

    }
  }
  else {
    m_isAbort=true;
  }
}


void OnlineHistogram::createDisplayOptions() {
  if(!m_DOinit) {
    checkerr( OCIObjectNew ( m_envhp, m_errhp, m_svchp, OCI_TYPECODE_OBJECT,
			     OCIdispopt, (dvoid *) NULL, OCI_DURATION_SESSION, TRUE,
			     (dvoid **) &m_dispopt), SEVERE);
    checkerr( OCIObjectGetInd ( m_envhp,   m_errhp,
				(dvoid *) m_dispopt,
				(dvoid **) &m_dispopt_null), SEVERE);


    m_do.reserve(39);
    m_do.push_back(new OnlineDisplayOption("LABEL_X",OnlineDisplayOption::STRING,
					   (void*) &(m_dispopt->LABEL_X),
					   &(m_dispopt_null->LABEL_X), this ) );
    m_do.push_back(new OnlineDisplayOption("LABEL_Y",OnlineDisplayOption::STRING,
					   (void*) &(m_dispopt->LABEL_Y),
					   &(m_dispopt_null->LABEL_Y), this ) );
    m_do.push_back(new OnlineDisplayOption("LABEL_Z",OnlineDisplayOption::STRING,
					   (void*) &(m_dispopt->LABEL_Z),
					   &(m_dispopt_null->LABEL_Z), this ) );
    m_do.push_back(new OnlineDisplayOption("YMIN",OnlineDisplayOption::FLOAT,
					   (void*) &(m_dispopt->YMIN),
					   &(m_dispopt_null->YMIN), this ) );
    m_do.push_back(new OnlineDisplayOption("YMAX",OnlineDisplayOption::FLOAT,
					   (void*) &(m_dispopt->YMAX),
					   &(m_dispopt_null->YMAX), this ) );
    m_do.push_back(new OnlineDisplayOption("STATS",OnlineDisplayOption::INT,
					   (void*) &(m_dispopt->STATS),
					   &(m_dispopt_null->STATS), this ) );
    m_do.push_back(new OnlineDisplayOption("FILLSTYLE",OnlineDisplayOption::INT,
					   (void*) &(m_dispopt->FILLSTYLE),
					   &(m_dispopt_null->FILLSTYLE), this ) );
    m_do.push_back(new OnlineDisplayOption("FILLCOLOR",OnlineDisplayOption::INT,
					   (void*) &(m_dispopt->FILLCOLOR),
					   &(m_dispopt_null->FILLCOLOR), this ) );
    m_do.push_back(new OnlineDisplayOption("LINESTYLE",OnlineDisplayOption::INT,
					   (void*) &(m_dispopt->LINESTYLE),
					   &(m_dispopt_null->LINESTYLE), this ) );
    m_do.push_back(new OnlineDisplayOption("LINECOLOR",OnlineDisplayOption::INT,
					   (void*) &(m_dispopt->LINECOLOR),
					   &(m_dispopt_null->LINECOLOR), this ) );
    m_do.push_back(new OnlineDisplayOption("LINEWIDTH",OnlineDisplayOption::INT,
					   (void*) &(m_dispopt->LINEWIDTH),
					   &(m_dispopt_null->LINEWIDTH), this ) );
    m_do.push_back(new OnlineDisplayOption("DRAWOPTS",OnlineDisplayOption::STRING,
					   (void*) &(m_dispopt->DRAWOPTS),
					   &(m_dispopt_null->DRAWOPTS), this ) );
    m_do.push_back(new OnlineDisplayOption("XMIN",OnlineDisplayOption::FLOAT,
					   (void*) &(m_dispopt->XMIN),
					   &(m_dispopt_null->XMIN), this ) );
    m_do.push_back(new OnlineDisplayOption("XMAX",OnlineDisplayOption::FLOAT,
					   (void*) &(m_dispopt->XMAX),
					   &(m_dispopt_null->XMAX), this ) );
    m_do.push_back(new OnlineDisplayOption("ZMIN",OnlineDisplayOption::FLOAT,
					   (void*) &(m_dispopt->ZMIN),
					   &(m_dispopt_null->ZMIN), this ) );
    m_do.push_back(new OnlineDisplayOption("ZMAX",OnlineDisplayOption::FLOAT,
					   (void*) &(m_dispopt->ZMAX),
					   &(m_dispopt_null->ZMAX), this ) );
    m_do.push_back(new OnlineDisplayOption("LOGX",OnlineDisplayOption::INT,
					   (void*) &(m_dispopt->LOGX),
					   &(m_dispopt_null->LOGX), this ) );
    m_do.push_back(new OnlineDisplayOption("LOGY",OnlineDisplayOption::INT,
					   (void*) &(m_dispopt->LOGY),
					   &(m_dispopt_null->LOGY), this ) );
    m_do.push_back(new OnlineDisplayOption("LOGZ",OnlineDisplayOption::INT,
					   (void*) &(m_dispopt->LOGZ),
					   &(m_dispopt_null->LOGZ), this ) );
    m_do.push_back(new OnlineDisplayOption("TIMAGE",OnlineDisplayOption::STRING,
					   (void*) &(m_dispopt->TIMAGE),
					   &(m_dispopt_null->TIMAGE), this ) );
    m_do.push_back(new OnlineDisplayOption("REF",OnlineDisplayOption::STRING,
					   (void*) &(m_dispopt->REF),
					   &(m_dispopt_null->REF), this ) );
    m_do.push_back(new OnlineDisplayOption("REFRESH",OnlineDisplayOption::FLOAT,
					   (void*) &(m_dispopt->REFRESH),
					   &(m_dispopt_null->REFRESH), this ) );
    m_do.push_back(new OnlineDisplayOption("TIT_X_SIZE",OnlineDisplayOption::FLOAT,
					   (void*) &(m_dispopt->TIT_X_SIZE),
					   &(m_dispopt_null->TIT_X_SIZE), this ) );
    m_do.push_back(new OnlineDisplayOption("TIT_X_OFFS",OnlineDisplayOption::FLOAT,
					   (void*) &(m_dispopt->TIT_X_OFFS),
					   &(m_dispopt_null->TIT_X_OFFS), this ) );
    m_do.push_back(new OnlineDisplayOption("TIT_Y_SIZE",OnlineDisplayOption::FLOAT,
					   (void*) &(m_dispopt->TIT_Y_SIZE),
					   &(m_dispopt_null->TIT_Y_SIZE), this ) );
    m_do.push_back(new OnlineDisplayOption("TIT_Y_OFFS",OnlineDisplayOption::FLOAT,
					   (void*) &(m_dispopt->TIT_Y_OFFS),
					   &(m_dispopt_null->TIT_Y_OFFS), this ) );
    m_do.push_back(new OnlineDisplayOption("TIT_Z_SIZE",OnlineDisplayOption::FLOAT,
					   (void*) &(m_dispopt->TIT_Z_SIZE),
					   &(m_dispopt_null->TIT_Z_SIZE), this ) );
    m_do.push_back(new OnlineDisplayOption("TIT_Z_OFFS",OnlineDisplayOption::FLOAT,
					   (void*) &(m_dispopt->TIT_Z_OFFS),
					   &(m_dispopt_null->TIT_Z_OFFS), this ) );
    m_do.push_back(new OnlineDisplayOption("LAB_X_SIZE",OnlineDisplayOption::FLOAT,
					   (void*) &(m_dispopt->LAB_X_SIZE),
					   &(m_dispopt_null->LAB_X_SIZE), this ) );
    m_do.push_back(new OnlineDisplayOption("LAB_X_OFFS",OnlineDisplayOption::FLOAT,
					   (void*) &(m_dispopt->LAB_X_OFFS),
					   &(m_dispopt_null->LAB_X_OFFS), this ) );
    m_do.push_back(new OnlineDisplayOption("LAB_Y_SIZE",OnlineDisplayOption::FLOAT,
					   (void*) &(m_dispopt->LAB_Y_SIZE),
					   &(m_dispopt_null->LAB_Y_SIZE), this ) );
    m_do.push_back(new OnlineDisplayOption("LAB_Y_OFFS",OnlineDisplayOption::FLOAT,
					   (void*) &(m_dispopt->LAB_Y_OFFS),
					   &(m_dispopt_null->LAB_Y_OFFS), this ) );
    m_do.push_back(new OnlineDisplayOption("LAB_Z_SIZE",OnlineDisplayOption::FLOAT,
					   (void*) &(m_dispopt->LAB_Z_SIZE),
					   &(m_dispopt_null->LAB_Z_SIZE), this ) );
    m_do.push_back(new OnlineDisplayOption("LAB_Z_OFFS",OnlineDisplayOption::FLOAT,
					   (void*) &(m_dispopt->LAB_Z_OFFS),
					   &(m_dispopt_null->LAB_Z_OFFS), this ) );
    m_do.push_back(new OnlineDisplayOption("GRIDX",OnlineDisplayOption::INT,
					   (void*) &(m_dispopt->GRIDX),
					   &(m_dispopt_null->GRIDX), this ) );
    m_do.push_back(new OnlineDisplayOption("GRIDY",OnlineDisplayOption::INT,
					   (void*) &(m_dispopt->GRIDY),
					   &(m_dispopt_null->GRIDY), this ) );
    m_do.push_back(new OnlineDisplayOption("THETA",OnlineDisplayOption::FLOAT,
					   (void*) &(m_dispopt->THETA),
					   &(m_dispopt_null->THETA), this ) );
    m_do.push_back(new OnlineDisplayOption("PHI",OnlineDisplayOption::FLOAT,
					   (void*) &(m_dispopt->PHI),
					   &(m_dispopt_null->PHI), this ) );
    m_do.push_back(new OnlineDisplayOption("CNTPLOT",OnlineDisplayOption::STRING,
					   (void*) &(m_dispopt->CNTPLOT),
					   &(m_dispopt_null->CNTPLOT), this ) );
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

bool OnlineHistogram::loadAnalysisDirections() {
  bool out = false;
  // analysis options
  if (!m_isAbort) {
    OCIStmt *astmt=NULL;
    OCIArray *intlist=NULL;
    OCIArray *analist=NULL;
    m_StmtMethod = "OnlineHistogram::loadAnalysisDirections";
    if ( OCI_SUCCESS == prepareOCIStatement
	 (astmt, "BEGIN :out := ONLINEHISTDB.GETHISTOANALYSIS(:2, :3, :4); END;") ) {
      
      checkerr( OCIObjectNew ( m_envhp, m_errhp, m_svchp, OCI_TYPECODE_VARRAY,
			       OCIintlist, (dvoid *) 0, OCI_DURATION_SESSION, TRUE,
			       (dvoid **) &intlist));
      checkerr( OCIObjectNew ( m_envhp, m_errhp, m_svchp, OCI_TYPECODE_VARRAY,
			       OCIanalist, (dvoid *) 0, OCI_DURATION_SESSION, TRUE,
			       (dvoid **) &analist));
      
      myOCIBindInt   (astmt,":out", m_nanalysis); 
      myOCIBindInt   (astmt,":2", m_hsid);  
      myOCIBindObject(astmt,":3", (void **) &intlist, OCIintlist);
      myOCIBindObject(astmt,":4", (void **) &analist, OCIanalist);
      
      if (OCI_SUCCESS == myOCIStmtExecute(astmt)) {
	intVarrayToVector(intlist, m_anaId);
	stringVarrayToVector(analist, m_anaName);
	out = true;
	m_anadirLoaded = true;
      }
      else {
	m_nanalysis=0;
	m_anaId.clear();
	m_anaName.clear();
      }
      checkerr(OCIObjectFree ( m_envhp, m_errhp, intlist, OCI_OBJECTFREE_FORCE) );
      checkerr(OCIObjectFree ( m_envhp, m_errhp, analist, OCI_OBJECTFREE_FORCE) );
      releaseOCIStatement(astmt);
    }
  } 
  return out;
}


bool OnlineHistogram::loadCreationDirections() {
  bool out = false;
  if(m_isAnaHist) { 
    OCIStmt *stmt=NULL;
    m_StmtMethod = "OnlineHistogram::loadCreationDirections";
    if ( OCI_SUCCESS == prepareOCIStatement
	 (stmt, "BEGIN ONLINEHISTDB.GETANAHISTDIRECTIONS(theHCID=> :1,Alg => :2,Sset => :3, Sources => :4, Pars =>:5 ); END;") ) {
      text alg[VSIZE_ALGNAME]="";
      OCIArray *flolist;
      OCIArray *hnalist;
      checkerr( OCIObjectNew ( m_envhp, m_errhp, m_svchp, OCI_TYPECODE_VARRAY,
			       OCIflolist, (dvoid *) 0, OCI_DURATION_SESSION, TRUE,
			       (dvoid **) &flolist));
      checkerr( OCIObjectNew ( m_envhp, m_errhp, m_svchp, OCI_TYPECODE_VARRAY,
			       OCIhnalist, (dvoid *) 0, OCI_DURATION_SESSION, TRUE,
			       (dvoid **) &hnalist));
      
      myOCIBindString(stmt,":1", m_hid);
      myOCIBindString(stmt,":2", alg, VSIZE_ALGNAME);
      myOCIBindInt   (stmt,":3", SourceSet, &SourceSet_null);
      myOCIBindObject(stmt,":4", (void **) &hnalist, OCIhnalist);
      myOCIBindObject(stmt,":5", (void **) &flolist, OCIflolist);
      if (OCI_SUCCESS == myOCIStmtExecute(stmt)) {
	out = true;
	CreationAlgorithm = std::string((const char *) alg);
	if (SourceSet_null) SourceSet=0;
	stringVarrayToVector(hnalist, Sources);
	floatVarrayToVector(flolist,  SourcePar);
	m_credirLoaded = true;
      }
      checkerr(OCIObjectFree ( m_envhp, m_errhp, flolist, OCI_OBJECTFREE_FORCE) );
      checkerr(OCIObjectFree ( m_envhp, m_errhp, hnalist, OCI_OBJECTFREE_FORCE) );
      releaseOCIStatement(stmt);
    }
  }
  return out;
}


OnlineHistogram::~OnlineHistogram()
{
  std::vector<OnlineDisplayOption*>::iterator ip;
  for (ip = m_do.begin();ip != m_do.end(); ++ip) 
    delete *ip; 
  if(m_dispopt) {
    checkerr(OCIObjectFree ( m_envhp, m_errhp, m_dispopt, OCI_OBJECTFREE_FORCE) );
  }
}


bool OnlineHistogram::remove(bool RemoveWholeSet) {
  int out=0;
  string command;
  if (RemoveWholeSet)
    command = "BEGIN :out := ONLINEHISTDB.DELETEHISTOGRAMSET(:1); END;";
  else
    command = "BEGIN :out := ONLINEHISTDB.DELETEHISTOGRAM(:1); END;";

  OCIStmt *stmt=NULL;
  m_StmtMethod = "OnlineHistogram::remove";
  if ( OCI_SUCCESS == prepareOCIStatement(stmt, command.c_str() ) ) {
    myOCIBindInt   (stmt,":out", out);  
    if (RemoveWholeSet)
      myOCIBindInt   (stmt,":1", m_hsid);
    else
      myOCIBindString(stmt,":1", m_hid);
    if (OCI_SUCCESS != myOCIStmtExecute(stmt)) {
      warningMessage("Histogram is probably on a page, remove it from all pages first");
      out = 0;
    }
    releaseOCIStatement(stmt);
  }
  if(out>0) m_isAbort=true;
  return (out == 1);  
}


// DISPLAY OPTIONS 

void OnlineHistogram::getDisplayOptions(int doid) {
  int out=0;
  OCIStmt *stmt=NULL;
  m_StmtMethod = "OnlineHistogram::getDisplayOptions";

  if ( OCI_SUCCESS == prepareOCIStatement
       (stmt, "BEGIN :out := ONLINEHISTDB.GETDISPLAYOPTIONS(:1,:2); END;") ) {

    myOCIBindInt   (stmt,":out", out);
    myOCIBindInt   (stmt,":1"  , doid);
    myOCIBindObject(stmt,":2"  , (void **) &m_dispopt, OCIdispopt,(void **) &m_dispopt_null);
    if (OCI_SUCCESS == myOCIStmtExecute(stmt, SEVERE)) {
      if (!out) m_domode = NONE;
    }
    else {
      m_domode = NONE;
    }
    releaseOCIStatement(stmt);
  }
}


int OnlineHistogram::setDisplayOptions(std::string function) {
  //update the DB with the current display options
  int out=0;
  std::string statement;
  statement = "BEGIN :out := ONLINEHISTDB." + function + ",theOptions => :opt); END;";
  OCIStmt *stmt=NULL;
  if ( OCI_SUCCESS == prepareOCIStatement(stmt, statement.c_str()) ) {
    myOCIBindInt   (stmt,":out", out);  
    myOCIBindObject(stmt,":opt", (void **) &m_dispopt, OCIdispopt, (void**) &m_dispopt_null);

    myOCIStmtExecute(stmt);
    releaseOCIStatement(stmt);
  }
  return out;
}


bool OnlineHistogram::checkHSDisplayFromDB() {
  bool out=false;
  OCIStmt *stmt=NULL;
  int hsd=0;
  m_StmtMethod = "OnlineHistogram::checkHSDisplayFromDB";
  if ( OCI_SUCCESS == prepareOCIStatement
       (stmt, "SELECT HSDISPLAY FROM HISTOGRAMSET WHERE HSID=:1 AND HSDISPLAY IS NOT NULL") ) {
    myOCIDefineInt(stmt, 1, hsd );  
    myOCIBindInt  (stmt,":1",m_hsid);
    if (OCI_SUCCESS == myOCIStmtExecute(stmt)) {
      out=true;
    }
    releaseOCIStatement(stmt);
  }
  return out;
}

bool OnlineHistogram::checkHDisplayFromDB() {
  bool out=false;
  OCIStmt *stmt=NULL;
  int hsd=0;
  m_StmtMethod = "OnlineHistogram::checkHSDisplayFromDB";
  if ( OCI_SUCCESS == prepareOCIStatement
       (stmt, "SELECT DISPLAY FROM HISTOGRAM WHERE HID=:1 AND DISPLAY IS NOT NULL") ) {
    myOCIDefineInt (stmt, 1, hsd );  
    myOCIBindString(stmt,":1",m_hid);
    if (OCI_SUCCESS == myOCIStmtExecute(stmt)) {
      out=true;
    }
    releaseOCIStatement(stmt);
  }
  return out; 
}


int OnlineHistogram::nPageInstances() {
  int out=0;
  OCIStmt *stmt=NULL;
  m_StmtMethod = "OnlineHistogram::nPageInstances";
  if ( OCI_SUCCESS == prepareOCIStatement
       (stmt,"SELECT COUNT(*) FROM SHOWHISTO WHERE HISTO=:1") ) {
    myOCIBindString(stmt,":1",m_hid);
    myOCIDefineInt (stmt, 1, out );  
    myOCIStmtExecute(stmt);   
    releaseOCIStatement(stmt);
  }
  return out;
}

int OnlineHistogram::nThisPageInstances() {
  int out=0;
  OCIStmt *stmt=NULL;
  m_StmtMethod = "OnlineHistogram::nPageInstances";
  if ( OCI_SUCCESS == prepareOCIStatement
       (stmt,"SELECT COUNT(*) FROM SHOWHISTO WHERE HISTO=:1 AND PAGE=:2") ) {
    myOCIBindString(stmt,":1",m_hid);
    myOCIBindString(stmt,":2",m_page);
    myOCIDefineInt (stmt, 1, out );  
    myOCIStmtExecute(stmt);   
    releaseOCIStatement(stmt);
  }
  return out;
}
OnlineHistTask* OnlineHistogram::getTask()
{
  return m_TStorage->getTask(m_task); 
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
    warningMessage(" no Set display options available");
    out = false;
  }
  else {
    if (m_hdisp != 0) 
      warningMessage("HistDisplayOptions were already defined and will be reinitialized");
    if (m_domode != SET) getDisplayOptions(m_hsdisp);
    std::string function="DECLAREHISTDISPLAYOPTIONS('"+m_hid+"'";
    m_StmtMethod = "OnlineHistogram::initHistDisplayOptionsFromSet";
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
    warningMessage("aborted: page/instance specified are not correct" );
    out = false;
  }
  else {
    if (m_hsdisp == 0) load();
    if (m_hsdisp == 0) {
      warningMessage("no Set display options available");
      out = false;
    }
    else {
      if (m_shdisp != 0) 
	warningMessage("HistoPageDisplayOptions were already defined and will be reinitialized");
      if (m_domode != SET) {
	getDisplayOptions(m_hsdisp);
	m_domode = SET;
      }
      std::stringstream function;
      function << "DECLAREHISTOPAGEDISPLAYOPTIONS(theHID => '" <<
	m_hid << "',thePage => '" << PageName << "',TheInstance =>" << 
	Instance;
      m_StmtMethod = "OnlineHistogram::initHistoPageDisplayOptionsFromSet";
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
    warningMessage("aborted: page/instance specified are not correct" );
    out = false;
  }
  else {
    if (m_hdisp == 0) load();
    if (m_hdisp == 0) {
      warningMessage("no Hist display options available");
      out = false;
    }
    else {
      if (m_shdisp != 0) 
	warningMessage("HistoPageDisplayOptions were already defined and will be reinitialized");
      if (m_domode != HIST) {
	getDisplayOptions(m_hdisp);
	m_domode = HIST;
      }
      std::stringstream function;
      function << "DECLAREHISTOPAGEDISPLAYOPTIONS(theHID => '" <<
	m_hid << "',thePage => '" << PageName << "',TheInstance =>" << 
	Instance;
      m_StmtMethod = "OnlineHistogram::initHistoPageDisplayOptionsFromHist";
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


bool OnlineHistogram::saveHistoSetDisplayOptions() {
  int out = 0;
  std::stringstream function;
  function << "DECLAREHISTOSETDISPLAYOPTIONS("<<m_hsid;
  m_StmtMethod = "OnlineHistogram::saveHistoSetDisplayOptions";
  out = setDisplayOptions(function.str());
  if(out > 0) {
    m_hsdisp = out;
    if (m_domode < SET) m_domode = SET;     
  }
  return (out>0);
}

bool OnlineHistogram::saveHistDisplayOptions() {

  int out = 0;
  if( useSetForHistDisplay() ) // use the set if we don't need to be more specific
    out= (int) saveHistoSetDisplayOptions();
  else {
    std::string function="DECLAREHISTDISPLAYOPTIONS('"+m_hid+"'";
    m_StmtMethod = "OnlineHistogram::saveHistDisplayOptions";
    out = setDisplayOptions(function);
    if(out > 0) {
      m_hdisp = out;
      if (m_domode < HIST) m_domode = HIST;     
    }
  }
  return (out>0);
}


bool OnlineHistogram::saveHistoPageDisplayOptions(std::string PageName,
						  int Instance) {
  int out = 0;
  if (Instance == -1) Instance=m_instance;
  if (PageName == "_DEFAULT_") PageName = m_page;
  if (!verifyPage(PageName,Instance)) {
    warningMessage("aborted: bad page/instance");
    out = 0;
  }				 
  else {
    std::stringstream function;
    function << "DECLAREHISTOPAGEDISPLAYOPTIONS(theHID => '" <<
      m_hid << "',thePage => '" << PageName << "',TheInstance =>" << 
      Instance;
      m_StmtMethod = "OnlineHistogram::saveHistoPageDisplayOptions";
      out = setDisplayOptions(function.str());
      if (out > 0) {
	m_shdisp = out;
	m_domode = HISTPAGE;
	m_page = PageName;
	m_instance = Instance;
      }
  }
  return (out>0);
}


bool OnlineHistogram::saveDisplayOptions() {
  bool out;
  if (useHistForPageDisplay()) // use the hist  if we don't need to be page-specific
    out=saveHistDisplayOptions();
  else
    out=saveHistoPageDisplayOptions();
  return out;
}

bool OnlineHistogram::getDisplayOption(std::string ParameterName,
				       void* option) 
{
  bool out=true;
  OnlineDisplayOption* mydo= getDO(ParameterName);
  if (mydo) {
    out=mydo->get(option);
  }
  else {
    warningMessage("parameter "+ParameterName+" is unknown");
    out=false;
  }
  return out;
}

bool OnlineHistogram::isSetDisplayOption(std::string ParameterName)
{
  bool out=true;
  OnlineDisplayOption* mydo= getDO(ParameterName);
  if (mydo) {
    out=mydo->isSet();
  }
  else {
    warningMessage("parameter "+ParameterName+" is unknown");
    out=false;
  }
  return out;
}

bool OnlineHistogram::setDisplayOption(std::string ParameterName, 
				       void* option)
{
  bool out=true;
  OnlineDisplayOption* mydo= getDO(ParameterName);
  if (mydo) {
    mydo->set(option);
  }
  else {
    warningMessage("parameter "+ParameterName+" is unknown");
    out=false;
  }
  return out;
}

bool OnlineHistogram::unsetDisplayOption(std::string ParameterName) {
  bool out=true;
  OnlineDisplayOption* mydo= getDO(ParameterName);
  if (mydo) {
    mydo->unset();
  }
  else {
    warningMessage("parameter "+ParameterName+" is unknown");
    out=false;
  }
  return out;
}


// private OnlineDisplayOption class to ease handling of many display options

OnlineHistogram::OnlineDisplayOption::OnlineDisplayOption(std::string Name, 
							  DisplayOptionType Type,
							  void* OCIvar,
							  OCIInd* OCIvar_null,
							  OnlineHistDBEnv * Env) 
  : m_name(Name), m_type(Type), 
    m_value(OCIvar), m_value_null(OCIvar_null),
    m_env(Env) 
{ if(Type == STRING) { // needs to be created (otherwise the allocation duration is 
                       // apparently not guaranteed)
    OCIObjectNew ( m_env->envhp(), m_env->errhp(), m_env->svchp(), OCI_TYPECODE_VARCHAR2,
    		   (OCIType *) NULL, (dvoid *) NULL, OCI_DURATION_SESSION, TRUE,
		   (dvoid **) m_value);
  } 
}

OnlineHistogram::OnlineDisplayOption::~OnlineDisplayOption() {
  if(m_type == STRING) {
    // remove allocated string (apparently, contrary to what stated in OCI doc, freeing
    // the dispopt object doesn't free the secondary  memory segments)
    OCIObjectFree ( m_env->envhp(), m_env->errhp(), (dvoid *) (*((OCIString **) m_value)), 
		    OCI_OBJECTFREE_FORCE);
    (*((OCIString **) m_value)) = NULL;
    *m_value_null = -1;
  }
}

void OnlineHistogram::OnlineDisplayOption::set(void *option){
  *m_value_null = 0;
  std::string* cont;
  std::string debstr;
  switch (m_type) {
  case STRING :
    cont = static_cast<std::string*>(option);
    OCIStringAssignText(m_env->envhp(), m_env->errhp(), (CONST OraText *) cont->c_str(),
			(ub2) ( strlen(cont->c_str())), (OCIString **) m_value);
    
    debstr=*cont;
    break;
  case FLOAT :
    OCINumberFromReal(m_env->errhp(), (dvoid *) option, (uword) sizeof(float),(OCINumber *) m_value);
    break;
  case INT :
    OCINumberFromInt(m_env->errhp(), (dvoid *) option, (uword) sizeof(int),
		     OCI_NUMBER_SIGNED,(OCINumber *) m_value);
    break;
  }
}

bool OnlineHistogram::OnlineDisplayOption::get(void *option){
  bool out=isSet();
  if (out) {
    switch (m_type) {
    case STRING :
      * (static_cast<std::string*>(option)) = 
	(const char*) OCIStringPtr(m_env->envhp(), (*((OCIString **) m_value)) );
      break;
    case FLOAT :
      OCINumberToReal(m_env->errhp(), (OCINumber *) m_value, sizeof(float), option);
      break;
    case INT :
      OCINumberToInt(m_env->errhp(),  (OCINumber *) m_value, sizeof(int),
		     OCI_NUMBER_SIGNED, option);
      break;
    }
  }
  return out;
}

void OnlineHistogram::OnlineDisplayOption::unset() {
  *m_value_null = -1;
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

// dump functions


void OnlineHistogram::dump() {

  cout<<" ----- HISTOGRAM RECORD " << m_hid << "------------------"<<endl;
  cout<<" Identifier: "<< m_identifier<< "    Type "<<hstype() <<endl;
  if (m_page != "_NONE_")
    cout<<" attached to Page :" << m_page << " instance "<<m_instance <<endl;
  if(m_domode == NONE)
    cout<<" No Special Display Options set" <<endl;
  else
    dumpDisplayOptions();

  if(m_nanalysis>0) {
    if(!m_anadirLoaded) loadAnalysisDirections();
    cout << endl<< m_nanalysis << " automatic Analysis defined:"<<endl;
    std::vector<float> warn,alarm;
    bool mask;
    for (int ia =0; ia< m_nanalysis; ia++) {
      warn.clear();
      alarm.clear();
      getAnaSettings( m_anaId[ia], &warn, &alarm, mask);
      if (mask) cout <<"   MASKED ANALYSIS ";
      cout << "   Algorithm "<<m_anaName[ia]<< "  parameters :"<<endl;
      for (unsigned int ip=0; ip< warn.size(); ip++) {
	cout << "          "<<getAlgParName(m_anaName[ia], ip+1) <<
	  "  warn=" << warn[ip] <<"  alarm="<<alarm[ip] << endl;
      }
    }
  }
  
  if(m_isAnaHist) {
    if (!m_credirLoaded) loadCreationDirections(); 
    cout<<"Histogram is produced at Analysis or Display level"<<endl;
    cout << "  using algorithm "<<CreationAlgorithm << " from sources"<<endl;
    for (unsigned int is =0; is< Sources.size(); is++) {
      cout <<"          "<<Sources[is]<<endl;;
    }
    if(SourcePar.size()>0) {
      cout <<" with parameters"<<endl;
      for (unsigned int ip =0; ip< SourcePar.size(); ip++) {
	cout << "          "<<getAlgParName(CreationAlgorithm, ip+1) <<
	  "  value=" << SourcePar[ip]  << endl;
      }
    }
  }

  cout<<" ----- END OF HISTOGRAM RECORD " << endl;

}

void OnlineHistogram::dumpDisplayOptions() {
  std::vector<OnlineDisplayOption*>::iterator ip;
  cout<<" ----- Display Options for Histogram "<<m_identifier << " -----"<<endl;
  cout<<"   Mode : " << m_domode <<" HSDISP="<<m_hsdisp<<" HDISP="<<m_hdisp
      <<" SHDISP="<<m_shdisp<<endl;
  std::string sopt;
  int iopt;
  float fopt;
  for (ip = m_do.begin();ip != m_do.end(); ++ip) {
    cout << "    " <<  (*ip)->name() << " : ";
    if ((*ip)->isSet()) {
      switch ((*ip)->type()) {
      case OnlineDisplayOption::STRING :
	(*ip)->get(&sopt);
	cout << sopt;
	break;
      case OnlineDisplayOption::FLOAT : 
	(*ip)->get(&fopt);
	cout << fopt;
	break;
      case OnlineDisplayOption::INT :
	(*ip)->get(&iopt);
	cout << iopt;
	break;
      }
    }
    else
      cout << "unset";
    cout <<endl;
  }
}





// analysys methods 

std::string OnlineHistogram::algFromID(int anaid) {
  if(!m_anadirLoaded) loadAnalysisDirections();
  std::string out="UNKNOWN";
  for (unsigned int i=0 ; i < m_anaId.size() ; i++) {
    if(m_anaId[i]  == anaid) {
      out = m_anaName[i];
      break;
    }
  }
  return out;
}





bool OnlineHistogram::getCreationDirections(std::string &Algorithm,
					    std::vector<std::string> &source_list,
					    std::vector<float> &parameters) {
  bool out = false;
  if (m_isAnaHist) { 
    if (!m_credirLoaded) loadCreationDirections(); 
    if (m_credirLoaded) {
      Algorithm = CreationAlgorithm;
      source_list.clear();
      if (!Sources.empty()) 
	source_list.insert( source_list.begin(),  
			    Sources.begin(), Sources.end() );
      parameters.clear();
      if(!SourcePar.empty()) 
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
  if(!m_anadirLoaded) loadAnalysisDirections();
  // check that Algorithm exists and get number of input parameters
  int out=0;
  bool known = false;
  int Np=getAlgorithmNpar( Algorithm );
  if(Np<0) {
    warningMessage("unknown algorithm "+Algorithm);
  }
  else {
    OCIStmt *stmt=NULL;
    m_StmtMethod = "OnlineHistogram::declareAnalysis";
    if ( OCI_SUCCESS == prepareOCIStatement
	 (stmt, "BEGIN :out := ONLINEHISTDB.DECLAREANALYSIS(:id,:alg,:w,:a,:in); END;") ) {
      OCITable *warn,*alarm;
      checkerr( OCIObjectNew ( m_envhp, m_errhp, m_svchp, OCI_TYPECODE_TABLE,
			       OCIthresholds, (dvoid *) 0, OCI_DURATION_SESSION, TRUE,
			       (dvoid **) &warn));
      checkerr( OCIObjectNew ( m_envhp, m_errhp, m_svchp, OCI_TYPECODE_TABLE,
			       OCIthresholds, (dvoid *) 0, OCI_DURATION_SESSION, TRUE,
			       (dvoid **) &alarm));
      if(Np>0) {
	floatVectorToVarray(*warningThr, warn);
	floatVectorToVarray(*alarmThr, alarm);
      }
      myOCIBindInt   (stmt,":out", out);
      myOCIBindInt   (stmt,":id" , m_hsid);
      myOCIBindString(stmt,":alg", Algorithm);
      myOCIBindObject(stmt,":w"  , (void **) &warn, OCIthresholds);
      myOCIBindObject(stmt,":a"  , (void **) &alarm, OCIthresholds);
      myOCIBindInt   (stmt,":in" , instance);
      if (OCI_SUCCESS == myOCIStmtExecute(stmt)) {
	if (out>0) {
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
	    // update also histograms of save set
	    if(m_nhs > 1)
	      m_HStorage->reloadAnalysisForSet(m_hsid, m_ihs);
	  }
	}
      }
      checkerr(OCIObjectFree ( m_envhp, m_errhp, warn, OCI_OBJECTFREE_FORCE) );
      checkerr(OCIObjectFree ( m_envhp, m_errhp, alarm, OCI_OBJECTFREE_FORCE) );
      releaseOCIStatement(stmt);
    }
  }
  return out;
}

bool OnlineHistogram::setAnalysis(int AnaID, 
				  std::vector<float>* warningThr, 
				  std::vector<float>* alarmThr)
{
  if(!m_anadirLoaded) loadAnalysisDirections();
  bool out=false;
  int Np=getAlgorithmNpar(algFromID(AnaID));
  if(Np<0) {
    std::stringstream error;
    error<< "unknown analysis "<<AnaID;
    warningMessage(error.str());
  }
  else {
    OCIStmt *stmt=NULL;
    m_StmtMethod = "OnlineHistogram::setAnalysis";
    if ( OCI_SUCCESS == prepareOCIStatement
	 (stmt, "BEGIN ONLINEHISTDB.SETSPECIALANALYSIS(:id,:hid,:w,:a);END;"  ) ) {
      OCITable *warn,*alarm;
      checkerr( OCIObjectNew ( m_envhp, m_errhp, m_svchp, OCI_TYPECODE_TABLE,
			       OCIthresholds, (dvoid *) 0, OCI_DURATION_SESSION, TRUE,
			       (dvoid **) &warn));
      checkerr( OCIObjectNew ( m_envhp, m_errhp, m_svchp, OCI_TYPECODE_TABLE,
			       OCIthresholds, (dvoid *) 0, OCI_DURATION_SESSION, TRUE,
			       (dvoid **) &alarm));
      if(Np>0) {
	floatVectorToVarray(*warningThr, warn);
	floatVectorToVarray(*alarmThr, alarm);
      }
      myOCIBindInt   (stmt,":id" , AnaID);
      myOCIBindString(stmt,":hid", m_hid);
      myOCIBindObject(stmt,":w"  , (void **) &warn, OCIthresholds);
      myOCIBindObject(stmt,":a"  , (void **) &alarm, OCIthresholds);
      out = (OCI_SUCCESS == myOCIStmtExecute(stmt));
      checkerr(OCIObjectFree ( m_envhp, m_errhp, warn, OCI_OBJECTFREE_FORCE) );
      checkerr(OCIObjectFree ( m_envhp, m_errhp, alarm, OCI_OBJECTFREE_FORCE) );
      releaseOCIStatement(stmt);
    }
  }
  return out;
}

bool OnlineHistogram::getAnaSettings(int AnaID,
				     std::vector<float>* warnTh, 
				     std::vector<float>* alarmTh,
				     bool &mask)  {
  if(!m_anadirLoaded) loadAnalysisDirections();
  bool out=true;
  int Np=getAlgorithmNpar(algFromID(AnaID));
  if (Np>0) { 
    if (0 == warnTh || 0 == alarmTh) {
      std::stringstream message;
      message <<"Error in OnlineHistogram::getAnaSettings : Analysis " << AnaID
	      << " requires "<<Np<<" parameters";
      warningMessage(message.str());
      Np=-1;
      out=false;
    }
    else {
      warnTh->clear();
      alarmTh->clear();
    }
  }
  
  if (out) {
    int imask=0;
    OCIStmt *stmt=NULL;
    m_StmtMethod = "OnlineHistogram::getAnaSettings";
    if ( OCI_SUCCESS == prepareOCIStatement
	 (stmt, "BEGIN ONLINEHISTDB.GETANASETTINGS(:id,:hid,:w,:a,:m); END;" ) ) {
      OCITable *warn,*alarm;
      checkerr( OCIObjectNew ( m_envhp, m_errhp, m_svchp, OCI_TYPECODE_TABLE,
			       OCIthresholds, (dvoid *) 0, OCI_DURATION_SESSION, TRUE,
			       (dvoid **) &warn));
      checkerr( OCIObjectNew ( m_envhp, m_errhp, m_svchp, OCI_TYPECODE_TABLE,
			       OCIthresholds, (dvoid *) 0, OCI_DURATION_SESSION, TRUE,
			       (dvoid **) &alarm));
      myOCIBindInt   (stmt,":id" , AnaID);
      myOCIBindString(stmt,":hid", m_hid);
      myOCIBindObject(stmt,":w"  , (void **) &warn, OCIthresholds);
      myOCIBindObject(stmt,":a"  , (void **) &alarm, OCIthresholds);
      myOCIBindInt   (stmt,":m"  , imask);
      if (OCI_SUCCESS == myOCIStmtExecute(stmt)) {
	mask = (imask == 1);
	if(Np>0) {
	  floatVarrayToVector(warn, *warnTh);
	  floatVarrayToVector(alarm, *alarmTh);
	}
      }
      else {
	out=false;
      }
      checkerr(OCIObjectFree ( m_envhp, m_errhp, warn, OCI_OBJECTFREE_FORCE) );
      checkerr(OCIObjectFree ( m_envhp, m_errhp, alarm, OCI_OBJECTFREE_FORCE) );
      releaseOCIStatement(stmt);
    }
  }
  return out;
}


bool OnlineHistogram::maskAnalysis(int AnaID,bool Mask) {
  if(!m_anadirLoaded) loadAnalysisDirections();
  bool out=false;
  OCIStmt *stmt=NULL;
  int iMask = (int) Mask;
  m_StmtMethod = "OnlineHistogram::maskAnalysis";
  if ( OCI_SUCCESS == prepareOCIStatement
       (stmt, "UPDATE ANASETTINGS SET MASK=:1 WHERE ANA=:2 AND HISTO=:3" ) ) {
    myOCIBindInt   (stmt,":1", iMask);  
    myOCIBindInt   (stmt,":2", AnaID);
    myOCIBindString(stmt,":3", m_hid);
    out = (OCI_SUCCESS == myOCIStmtExecute(stmt));
    releaseOCIStatement(stmt);
  }
  return out;
 }

// container class OnlineHistogramStorage

OnlineHistogramStorage::OnlineHistogramStorage() :
  m_avoid_hdup(true)  {}

OnlineHistogramStorage::~OnlineHistogramStorage() 
{
  if (m_Histenv->debug() > 2) cout << "Deleting "<<
    m_myHist.size() << " OnlineHistogram objects"<<endl;
  std::vector<OnlineHistogram*>::iterator ih;
  //for (ih = m_myHist.begin();ih != m_myHist.end(); ++ih) 
  //delete *ih; 
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
      m_Histenv->warningMessage("error from getHistogram:cannot create histogram object "+Identifier);
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

void OnlineHistogramStorage::reloadAnalysisForSet(int Hsid, 
						  int Ihs) {
  std::vector<OnlineHistogram*>::iterator ih;
  for (ih = m_myHist.begin(); ih != m_myHist.end(); ++ih) {
    if( (*ih)->hsid() == Hsid && (*ih)->ihs() != Ihs)
      (*ih)->loadAnalysisDirections();
  }
}

void OnlineHistogramStorage::reloadHistograms() {
  std::vector<OnlineHistogram*>::iterator ih;
  for (ih = m_myHist.begin(); ih != m_myHist.end(); ++ih) {
    (*ih)->load();
  }
}
