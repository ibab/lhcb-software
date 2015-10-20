// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/OnlineHistDB/src/OnlineHistogram.cpp,v 1.50 2010-10-21 10:16:46 ggiacomo Exp $
/*
   C++ interface to the Online Monitoring Histogram DB
   G. Graziani (INFN Firenze)

*/

#include <cmath>
#include <cstring>
#include "OnlineHistDB/OnlineHistogram.h"
#include "OnlineHistDB/OnlineHistoOnPage.h"
#include "OnlineHistDB/OnlineHistPage.h"
using namespace std;
using namespace OnlineHistDBEnv_constants;
OnlineHistogram::OnlineHistogram(OnlineHistDBEnv &env,
                                 std::string Identifier,
                                 std::string Page,
                                 int Instance) :
  OnlineHistDBEnv(env), Sources(0), SourceSet(0),
  m_isAbort(false), m_identifier(Identifier), 
  m_page(Page), m_onpage(NULL), m_instance(Instance), m_page_verified(false),
  m_DOinit(false), m_InitDOmode(NONE), m_domode(NONE), m_hsdisp(0), m_hdisp(0), m_shdisp(0),
  m_dispopt(NULL), m_dispopt_null(NULL), m_fitfun_null(1), 
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
    if (debug() > 3) std::cout << "Reloading Histogram " << identifier() << std::endl;
    load();
  }
}

OnlineHistPage* OnlineHistogram::pageObject() {
  return (m_onpage ? m_onpage->page() : NULL); 
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

bool OnlineHistogram::setDimServiceName(std::string& DimServiceName) {
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

void OnlineHistogram::reload() {
  OCIStmt *stmt=NULL;
  m_StmtMethod = "OnlineHistogram::reload";
  if (OCI_SUCCESS == 
      prepareOCIStatement(stmt,"SELECT NAME FROM HISTOGRAM WHERE HID=:1") ) {
    myOCIBindString(stmt,":1", m_hid);
    if (OCI_SUCCESS == myOCISelectExecute(stmt) ) {
      text Name[VSIZE_NAME];
      Name[0]= '\0';
      myOCIDefineString(stmt, 1, Name  , VSIZE_NAME);
      myOCIFetch(stmt, 1);
      m_identifier = (const char*) Name;
      load();
    }
    releaseOCIStatement(stmt);
  }
}

bool OnlineHistogram::rename(std::string &newTask,
                             std::string &newAlgo,
                             std::string &newName) {
  int out=0;
  OCIStmt *stmt=NULL;
  m_StmtMethod = "OnlineHistogram::rename";
  if ( OCI_SUCCESS == prepareOCIStatement
       (stmt, "BEGIN :out := ONLINEHISTDB.RENAMEHISTOGRAM(:id,:1,:2,:3); END;") ) {
    myOCIBindInt   (stmt,":out", out);  
    myOCIBindString(stmt,":id",m_identifier);
    myOCIBindString(stmt,":1", newTask);  
    myOCIBindString(stmt,":2", newAlgo);
    myOCIBindString(stmt,":3", newName);
    myOCIStmtExecute(stmt, SEVERE);
    releaseOCIStatement(stmt);
  }
  return (out>0);
}


void OnlineHistogram::load() {
  int out=0;
  m_anadirLoaded = m_credirLoaded = false;
  OCIStmt *stmt=NULL;
  m_StmtMethod = "OnlineHistogram::load";
  std::string command = 
    "BEGIN :out := ONLINEHISTDB.GETHISTOGRAMDATA(:nm,:pg,:in,:hid,:hsd,:ihs,:nhs,:ty,:hst,:sub,:tk,:alg,:na,:des,:doc,:isa,:cre,:obs,:dis,:hdi,:shd, :dsn,:lbl,:nxlb,:nylb,:refp); END;";
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
    text  pgref[VSIZE_PAGENAME]="";
    int isAnaHist=0;
    OCIArray *labels;
    checkerr( OCIObjectNew ( m_envhp, m_errhp, m_svchp, OCI_TYPECODE_TABLE,
			     OCIlabels, (dvoid *) 0, OCI_DURATION_SESSION, TRUE,
			     (dvoid **) &labels));
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
    myOCIBindObject(stmt,":lbl", (void **) &labels, OCIlabels );
    myOCIBindInt   (stmt,":nxlb",m_xbinlab, &m_xbinlab_null );
    myOCIBindInt   (stmt,":nylb",m_ybinlab, &m_ybinlab_null );    
    myOCIBindString(stmt,":refp", pgref, VSIZE_PAGENAME, &m_refPage_null);

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
        stringVarrayToVector(labels, m_binlabels);
        if (m_xbinlab_null) m_xbinlab = 0; 
        if (m_ybinlab_null) m_ybinlab = 0; 
        m_refPage= m_refPage_null ? "" : std::string((const char *) pgref);
      }
      
    }
    else {
      m_isAbort=true;
    }
    checkerr(OCIObjectFree ( m_envhp, m_errhp, labels, OCI_OBJECTFREE_FORCE) );
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
    
    m_do.clear();
    // the next is generated by autodispopt.pl
    // ---------------------------------------

    m_do.reserve(70);
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
    m_do.push_back(new OnlineDisplayOption("DRAWPATTERN",OnlineDisplayOption::STRING,
					   (void*) &(m_dispopt->DRAWPATTERN),
					   &(m_dispopt_null->DRAWPATTERN), this ) );
    m_do.push_back(new OnlineDisplayOption("STAT_X_SIZE",OnlineDisplayOption::FLOAT,
					   (void*) &(m_dispopt->STAT_X_SIZE),
					   &(m_dispopt_null->STAT_X_SIZE), this ) );
    m_do.push_back(new OnlineDisplayOption("STAT_X_OFFS",OnlineDisplayOption::FLOAT,
					   (void*) &(m_dispopt->STAT_X_OFFS),
					   &(m_dispopt_null->STAT_X_OFFS), this ) );
    m_do.push_back(new OnlineDisplayOption("STAT_Y_SIZE",OnlineDisplayOption::FLOAT,
					   (void*) &(m_dispopt->STAT_Y_SIZE),
					   &(m_dispopt_null->STAT_Y_SIZE), this ) );
    m_do.push_back(new OnlineDisplayOption("STAT_Y_OFFS",OnlineDisplayOption::FLOAT,
					   (void*) &(m_dispopt->STAT_Y_OFFS),
					   &(m_dispopt_null->STAT_Y_OFFS), this ) );
    m_do.push_back(new OnlineDisplayOption("HTIT_X_SIZE",OnlineDisplayOption::FLOAT,
					   (void*) &(m_dispopt->HTIT_X_SIZE),
					   &(m_dispopt_null->HTIT_X_SIZE), this ) );
    m_do.push_back(new OnlineDisplayOption("HTIT_X_OFFS",OnlineDisplayOption::FLOAT,
					   (void*) &(m_dispopt->HTIT_X_OFFS),
					   &(m_dispopt_null->HTIT_X_OFFS), this ) );
    m_do.push_back(new OnlineDisplayOption("HTIT_Y_SIZE",OnlineDisplayOption::FLOAT,
					   (void*) &(m_dispopt->HTIT_Y_SIZE),
					   &(m_dispopt_null->HTIT_Y_SIZE), this ) );
    m_do.push_back(new OnlineDisplayOption("HTIT_Y_OFFS",OnlineDisplayOption::FLOAT,
					   (void*) &(m_dispopt->HTIT_Y_OFFS),
					   &(m_dispopt_null->HTIT_Y_OFFS), this ) );
    m_do.push_back(new OnlineDisplayOption("NDIVX",OnlineDisplayOption::INT,
					   (void*) &(m_dispopt->NDIVX),
					   &(m_dispopt_null->NDIVX), this ) );
    m_do.push_back(new OnlineDisplayOption("NDIVY",OnlineDisplayOption::INT,
					   (void*) &(m_dispopt->NDIVY),
					   &(m_dispopt_null->NDIVY), this ) );
    m_do.push_back(new OnlineDisplayOption("MARKERSIZE",OnlineDisplayOption::FLOAT,
					   (void*) &(m_dispopt->MARKERSIZE),
					   &(m_dispopt_null->MARKERSIZE), this ) );
    m_do.push_back(new OnlineDisplayOption("MARKERCOLOR",OnlineDisplayOption::INT,
					   (void*) &(m_dispopt->MARKERCOLOR),
					   &(m_dispopt_null->MARKERCOLOR), this ) );
    m_do.push_back(new OnlineDisplayOption("MARKERSTYLE",OnlineDisplayOption::INT,
					   (void*) &(m_dispopt->MARKERSTYLE),
					   &(m_dispopt_null->MARKERSTYLE), this ) );
    m_do.push_back(new OnlineDisplayOption("NORM",OnlineDisplayOption::FLOAT,
					   (void*) &(m_dispopt->NORM),
					   &(m_dispopt_null->NORM), this ) );
    m_do.push_back(new OnlineDisplayOption("TICK_X",OnlineDisplayOption::INT,
					   (void*) &(m_dispopt->TICK_X),
					   &(m_dispopt_null->TICK_X), this ) );
    m_do.push_back(new OnlineDisplayOption("TICK_Y",OnlineDisplayOption::INT,
					   (void*) &(m_dispopt->TICK_Y),
					   &(m_dispopt_null->TICK_Y), this ) );
    m_do.push_back(new OnlineDisplayOption("MARGIN_TOP",OnlineDisplayOption::FLOAT,
					   (void*) &(m_dispopt->MARGIN_TOP),
					   &(m_dispopt_null->MARGIN_TOP), this ) );
    m_do.push_back(new OnlineDisplayOption("MARGIN_BOTTOM",OnlineDisplayOption::FLOAT,
					   (void*) &(m_dispopt->MARGIN_BOTTOM),
					   &(m_dispopt_null->MARGIN_BOTTOM), this ) );
    m_do.push_back(new OnlineDisplayOption("MARGIN_LEFT",OnlineDisplayOption::FLOAT,
					   (void*) &(m_dispopt->MARGIN_LEFT),
					   &(m_dispopt_null->MARGIN_LEFT), this ) );
    m_do.push_back(new OnlineDisplayOption("MARGIN_RIGHT",OnlineDisplayOption::FLOAT,
					   (void*) &(m_dispopt->MARGIN_RIGHT),
					   &(m_dispopt_null->MARGIN_RIGHT), this ) );
    m_do.push_back(new OnlineDisplayOption("PADCOLOR",OnlineDisplayOption::INT,
					   (void*) &(m_dispopt->PADCOLOR),
					   &(m_dispopt_null->PADCOLOR), this ) );
    m_do.push_back(new OnlineDisplayOption("STATTRANSP",OnlineDisplayOption::INT,
					   (void*) &(m_dispopt->STATTRANSP),
					   &(m_dispopt_null->STATTRANSP), this ) );
    m_do.push_back(new OnlineDisplayOption("REFDRAWOPTS",OnlineDisplayOption::STRING,
					   (void*) &(m_dispopt->REFDRAWOPTS),
					   &(m_dispopt_null->REFDRAWOPTS), this ) );
    m_do.push_back(new OnlineDisplayOption("SPAREI1",OnlineDisplayOption::INT,
					   (void*) &(m_dispopt->SPAREI1),
					   &(m_dispopt_null->SPAREI1), this ) );
    m_do.push_back(new OnlineDisplayOption("SPAREI2",OnlineDisplayOption::INT,
					   (void*) &(m_dispopt->SPAREI2),
					   &(m_dispopt_null->SPAREI2), this ) );
    m_do.push_back(new OnlineDisplayOption("SPAREF1",OnlineDisplayOption::FLOAT,
					   (void*) &(m_dispopt->SPAREF1),
					   &(m_dispopt_null->SPAREF1), this ) );
    m_do.push_back(new OnlineDisplayOption("SPAREF2",OnlineDisplayOption::FLOAT,
					   (void*) &(m_dispopt->SPAREF2),
					   &(m_dispopt_null->SPAREF2), this ) );
    m_do.push_back(new OnlineDisplayOption("SPARES",OnlineDisplayOption::STRING,
					   (void*) &(m_dispopt->SPARES),
					   &(m_dispopt_null->SPARES), this ) );
    m_do.push_back(new OnlineDisplayOption("NOTITLE",OnlineDisplayOption::INT,
					   (void*) &(m_dispopt->NOTITLE),
					   &(m_dispopt_null->NOTITLE), this ) );
    m_do.push_back(new OnlineDisplayOption("SHOWTITLE",OnlineDisplayOption::STRING,
					   (void*) &(m_dispopt->SHOWTITLE),
					   &(m_dispopt_null->SHOWTITLE), this ) );


    // ---------------------------------------
    // end of generated code 
    // ---------------------------------------
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

        if(SourceSet) {
          OCIStmt *stmt2=NULL;
           const int Nfetch=100;
          if (OCI_SUCCESS == 
              prepareOCIStatement(stmt2,"SELECT NAME FROM HISTOGRAM WHERE HSET=:1") ) {
            myOCIBindInt(stmt2,":1", SourceSet);
            if (OCI_SUCCESS == myOCISelectExecute(stmt2) ) {
              text Name[Nfetch][VSIZE_NAME];
              for (int k=0; k<Nfetch; k++) {
                Name[k][0]= '\0';
              }
              myOCIDefineString(stmt2, 1, Name[0]  , VSIZE_NAME);
              int xf = Nfetch;
              while ( xf == Nfetch) {
                xf = myOCIFetch(stmt2, Nfetch);
                for(int j=0; j<xf ; j++)
                  Sources.push_back(std::string((const char*) Name[j]));
              }
            }
            releaseOCIStatement(stmt2);
          }          
        }
        
        

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
  for (ip = m_do.begin();ip != m_do.end(); ++ip)  {
    if (debug() > 3) std::cout << "removing OnlineDisplayOption object for "<<m_identifier <<
      ": "<<  (*ip)->name() << " type "<< (*ip)->type() << std::endl;
    delete *ip; 
  }
  m_do.clear();
  if (debug() > 3) std::cout << "removing m_dispopt object"<< std::endl;
  if(m_dispopt) 
    checkerr(OCIObjectFree ( m_envhp, m_errhp, m_dispopt, OCI_OBJECTFREE_FORCE) );
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
       (stmt, "BEGIN :out := ONLINEHISTDB.GETDISPLAYOPTIONS(:id,:do,:fitf,:fitp,:fitr); END;") ) {
    text  fitfun[VSIZE_FITFUN]="";
    OCIArray *fitPars;
    checkerr( OCIObjectNew ( m_envhp, m_errhp, m_svchp, OCI_TYPECODE_TABLE,
                             OCIthresholds, (dvoid *) 0, OCI_DURATION_SESSION, TRUE,
                             (dvoid **) &fitPars));
    OCIArray *fitRange;
    checkerr( OCIObjectNew ( m_envhp, m_errhp, m_svchp, OCI_TYPECODE_TABLE,
                             OCIthresholds, (dvoid *) 0, OCI_DURATION_SESSION, TRUE,
                             (dvoid **) &fitRange));
    myOCIBindInt   (stmt,":out" , out);
    myOCIBindInt   (stmt,":id"  , doid);
    myOCIBindObject(stmt,":do"  , (void **) &m_dispopt, OCIdispopt,(void **) &m_dispopt_null);
    myOCIBindString(stmt,":fitf", fitfun, VSIZE_FITFUN, &m_fitfun_null);
    myOCIBindObject(stmt,":fitp", (void **) &fitPars, OCIthresholds);
    myOCIBindObject(stmt,":fitr", (void **) &fitRange, OCIthresholds);
    if (OCI_SUCCESS == myOCIStmtExecute(stmt, SEVERE)) {
      if (out) {
        m_fitfun = m_fitfun_null ? "" : std::string((const char *) fitfun);
        if(!m_fitfun_null) {
          floatVarrayToVector(fitPars , m_fitPars);
          floatVarrayToVector(fitRange, m_fitRange);
        }
      }
      else m_domode = NONE;
    }
    else {
      m_domode = NONE;
    }
    checkerr(OCIObjectFree ( m_envhp, m_errhp, fitPars , OCI_OBJECTFREE_FORCE) );
    checkerr(OCIObjectFree ( m_envhp, m_errhp, fitRange, OCI_OBJECTFREE_FORCE) );
    releaseOCIStatement(stmt);
  }
}


int OnlineHistogram::setDisplayOptions(std::string function) {
  //update the DB with the current display options
  int out=0;
  std::string statement;
  bool userFitRange=false;
  statement = "BEGIN :out := ONLINEHISTDB." + function + ",theOptions => :opt" +
    ",theFitFun => :fitf, theFitPars => :fitp";
  if (m_fitRange.size() >1) {
    if (m_fitRange[0] < m_fitRange[1]) {
      userFitRange = true;
      statement += ",theFitMin => :fitmin, theFitMax => :fitmax";
    }
  }
  statement += "); END;";

  OCIStmt *stmt=NULL;
  if ( OCI_SUCCESS == prepareOCIStatement(stmt, statement.c_str()) ) {
    OCIArray *fitPars;
    checkerr( OCIObjectNew ( m_envhp, m_errhp, m_svchp, OCI_TYPECODE_TABLE,
                             OCIthresholds, (dvoid *) 0, OCI_DURATION_SESSION, TRUE,
                             (dvoid **) &fitPars));
    floatVectorToVarray(m_fitPars , fitPars);
    
    myOCIBindInt   (stmt,":out", out);  
    myOCIBindObject(stmt,":opt", (void **) &m_dispopt, OCIdispopt, (void**) &m_dispopt_null);
    myOCIBindString(stmt,":fitf", m_fitfun, &m_fitfun_null);
    myOCIBindObject(stmt,":fitp", (void **) &fitPars, OCIthresholds);
    if(userFitRange) {
      myOCIBindFloat (stmt,":fitmin", m_fitRange[0]);
      myOCIBindFloat (stmt,":fitmax", m_fitRange[1]);
    }
    myOCIStmtExecute(stmt);
    releaseOCIStatement(stmt);
    checkerr(OCIObjectFree ( m_envhp, m_errhp, fitPars, OCI_OBJECTFREE_FORCE) );
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

void OnlineHistogram::unsetAllDisplayOptions() {
  std::vector<OnlineDisplayOption*>::iterator ip;
  for (ip = m_do.begin();ip != m_do.end(); ++ip) {
    (*ip)->unset();
  }
}


bool OnlineHistogram::changedDisplayOption(std::string ParameterName, 
					   void* option)
{
  bool out=false;
  OnlineDisplayOption* mydo= getDO(ParameterName);
  if (mydo) {
    if ( mydo->isSet() ) {
      out = mydo->differentfrom(option);
    }
  }
  else {
    warningMessage("parameter "+ParameterName+" is unknown");
  }
  return out;
}


bool OnlineHistogram::setPage2display(std::string& pageName) {
  bool out = false;
  OCIStmt *stmt=NULL;
  int ans;
  m_StmtMethod = "OnlineHistogram::setPage2display";
  if ( OCI_SUCCESS == prepareOCIStatement
         (stmt, "BEGIN :out := ONLINEHISTDB.SETPAGETODISPLAY(theHID=> :1,thePage => :2); END;") ) {
    myOCIBindInt   (stmt,":out", ans);
    myOCIBindString(stmt,":1", m_hid);
    myOCIBindString(stmt,":2", pageName);
    if (OCI_SUCCESS == myOCIStmtExecute(stmt)) {
      if(ans>0) {
        out = true;
        m_refPage= pageName;
      }
    }
    releaseOCIStatement(stmt);
  }
  return out;
}

bool OnlineHistogram::getFitFunction(std::string &Name, std::vector<float> *initValues, std::vector<float> *fitRange) {
  if (!m_fitfun_null) {
    Name=m_fitfun;
    if(NULL != initValues)
      initValues->insert(initValues->end(), m_fitPars.begin(), m_fitPars.end());
    if(NULL != fitRange)
      fitRange->insert(fitRange->end(), m_fitRange.begin(), m_fitRange.end());
  }
  return (!m_fitfun_null);
}

void OnlineHistogram::setFitFunction(std::string Name, std::vector<float> *initValues, std::vector<float> *fitRange) {
  m_fitfun_null=0;
  m_fitfun=Name;
  if(NULL != initValues)
    m_fitPars.insert(m_fitPars.end(), initValues->begin(), initValues->end());
  if(NULL != fitRange) {
    if (fitRange->size()>1)
      m_fitRange.insert(m_fitRange.end(), fitRange->begin(), fitRange->end());
  }
  return;
}


bool OnlineHistogram::setBinLabels(std::vector<std::string> *Xlabels, 
                                   std::vector<std::string> *Ylabels) {
  bool out=false;
  std::vector<std::string> newlabels;
  int newXsize=m_xbinlab;
  int newYsize=m_ybinlab;
  if (Xlabels) {
    newlabels.insert( newlabels.end(),Xlabels->begin(),
                      Xlabels->end() );
    newXsize=Xlabels->size();
  }
  else if(m_xbinlab) {
    newlabels.insert( newlabels.end(), m_binlabels.begin(),
                      m_binlabels.begin()+m_xbinlab );
  }
  if (Ylabels) {
    newlabels.insert( newlabels.end(),Ylabels->begin(),
                      Ylabels->end() );
    newYsize=Ylabels->size();
  }
  else if(m_ybinlab) {
    newlabels.insert( newlabels.end(), m_binlabels.begin()+m_xbinlab,
                      m_binlabels.end() );
  }

  m_StmtMethod = "OnlineHistogram::setBinLabels";
  OCIStmt *stmt=NULL;
  std::stringstream command;
  command << "BEGIN ONLINEHISTDB.DECLAREBINLABELS(theSet => :set, theHID => :hid, thelabels => labels(";
  for (unsigned int il=0;il<newlabels.size();il++) {
    if (il>0) command << ",";
    command << "'" << newlabels[il] <<"'";
  }
  command << "), Nx => :nx);END;";

  if ( OCI_SUCCESS == prepareOCIStatement(stmt,command.str().c_str()) ) {
    myOCIBindInt   (stmt, ":set", m_hsid);
    myOCIBindString(stmt, ":hid", m_hid);
    myOCIBindInt   (stmt, ":nx" , newXsize);
    out = (OCI_SUCCESS == myOCIStmtExecute(stmt));
    releaseOCIStatement(stmt);
  }
  if (out) {
    m_xbinlab=newXsize;
    m_ybinlab=newYsize;
    m_binlabels.clear();
    m_binlabels.insert(m_binlabels.end(), newlabels.begin(), newlabels.end());
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
    m_locString(false),
    m_env(Env) 
{ 
}

OnlineHistogram::OnlineDisplayOption::~OnlineDisplayOption() {
  if(m_type == STRING && m_locString) {
    // remove allocated OCIString 
    OCIObjectFree ( m_env->envhp(), m_env->errhp(), (dvoid *) (*((OCIString **) m_value)), 
                    OCI_OBJECTFREE_FORCE);
    (*((OCIString **) m_value)) = NULL;
    *m_value_null = -1;
  }
}

void OnlineHistogram::OnlineDisplayOption::set(void *option){
  std::string* cont;
  switch (m_type) {
  case STRING :
    if (m_env->debug() > 3) {
      std::cout <<" setting string option "<<m_name <<": before m_value_null="<<*m_value_null;
      if (! *m_value_null) std::cout <<" value="<<getStringValue();
      std::cout<<std::endl;
    }
    if (*m_value_null) { // create the OCI object
      if (m_env->debug() > 3) std::cout << "creating string object for option "<<m_name<<std::endl;
      OCIObjectNew ( m_env->envhp(), m_env->errhp(), m_env->svchp(), OCI_TYPECODE_VARCHAR2,
                     (OCIType *) NULL, (dvoid *) NULL, OCI_DURATION_SESSION, TRUE,
                     (dvoid **) m_value);
      m_locString = true;
    }
    cont = static_cast<std::string*>(option);
    OCIStringAssignText(m_env->envhp(), m_env->errhp(), (CONST OraText *) cont->c_str(),
                        (ub2) ( strlen(cont->c_str())), (OCIString **) m_value);
    break;
  case FLOAT :
    OCINumberFromReal(m_env->errhp(), (dvoid *) option, (uword) sizeof(float),(OCINumber *) m_value);
    break;
  case INT :
    OCINumberFromInt(m_env->errhp(), (dvoid *) option, (uword) sizeof(int),
		     OCI_NUMBER_SIGNED,(OCINumber *) m_value);
    break;
  }
  *m_value_null = 0;
}


std::string OnlineHistogram::OnlineDisplayOption::getStringValue()
{return std::string( (const char*) OCIStringPtr(m_env->envhp(), (*((OCIString **) m_value))) );}

float OnlineHistogram::OnlineDisplayOption::getFloatValue()
{ float out;
  OCINumberToReal(m_env->errhp(), (OCINumber *) m_value, sizeof(float), (void*) &out);
  return out; }

int OnlineHistogram::OnlineDisplayOption::getIntValue()
{ int out;
  OCINumberToInt(m_env->errhp(),  (OCINumber *) m_value, sizeof(int),
		     OCI_NUMBER_SIGNED, (void*) &out );
  return out;
}


bool OnlineHistogram::OnlineDisplayOption::get(void *option){
  bool out=isSet();
  if (out) {
    switch (m_type) {
    case STRING :
      (* (static_cast<std::string*>(option)) ) = getStringValue();
      break;
    case FLOAT :
      (* (static_cast<float*>(option)) ) = getFloatValue();
      break;
    case INT :
      (* (static_cast<int*>(option)) ) = getIntValue();
      break;
    }
  }
  return out;
}

void OnlineHistogram::OnlineDisplayOption::unset() {
  if (m_env->debug() > 3) std::cout << "unsetting option "<<m_name<<std::endl;
  *m_value_null = -1;
   if(m_type == STRING && m_locString) {
    // remove allocated string 
    OCIObjectFree ( m_env->envhp(), m_env->errhp(), (dvoid *) (*((OCIString **) m_value)), 
                    OCI_OBJECTFREE_FORCE);
    (*((OCIString **) m_value)) = NULL;
  }
}

bool OnlineHistogram::OnlineDisplayOption::differentfrom(void *option) {
  bool out=true;
  if (isSet()) {
    switch (m_type) {
    case STRING :
      out = ( (*(static_cast<std::string*>(option))) !=  getStringValue() );
      break;
    case FLOAT :
      out = ( fabs( (*(static_cast<float*>(option))) - getFloatValue()  ) 
	      > 0.001 );
      break;
    case INT :
      out = ( (*(static_cast<int*>(option))) != getIntValue() ) ;
      break;
    }
  }
  return out;
}



// ----------------------------------------------------------- //

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

bool OnlineHistogram::copyDisplayOptions(OnlineHistoOnPage* h) {
  std::string pagename="";
  if(h->page()) pagename = h->page()->name();
  return copyDO(h->histo->hid(), pagename, h->instance);
}
bool OnlineHistogram::copyDisplayOptions(OnlineHistogram* h) {
  if(h->onpage())
    return copyDisplayOptions(h->onpage());                       
  return copyDO(h->hid());
}


bool OnlineHistogram::copyDO(std::string hid, std::string page, unsigned int instance) {
  bool out=false;
  int doid=0;
  OCIStmt *stmt=NULL;
  if (page.length() == 0) page="NULL";
  m_StmtMethod = "OnlineHistogram::copyDO";
  std::stringstream function;
  function << "BEGIN :out := ONLINEHISTDB.GetBestDO(theHID => '" << hid <<"',thePage => '" << page  << "',TheInstance =>" << instance
           << "); END;";
  
  if ( OCI_SUCCESS == prepareOCIStatement
       (stmt, function.str().c_str()) ) {
    myOCIBindInt   (stmt,":out", doid);
    if (OCI_SUCCESS == myOCIStmtExecute(stmt)) {
      out=true;
      getDisplayOptions(doid);
    }
    releaseOCIStatement(stmt);
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
    std::vector<float> warn,alarm,inputs;
    bool mask;
    long status;
    float minstat,minstatfrac;
    for (int ia =0; ia< m_nanalysis; ia++) {
      warn.clear();
      alarm.clear();
      getAnaSettings( m_anaId[ia], &warn, &alarm, &inputs, mask, status, minstat, minstatfrac);
      if (mask) cout <<"   MASKED ANALYSIS ";
      cout << "   Algorithm "<<m_anaName[ia]<< "  parameters :"<<endl;
      if(!inputs.empty()) {
        for (unsigned int ip=0; ip< inputs.size(); ip++) {
          cout << "    INPUT "<<getAlgParName(m_anaName[ia], warn.size()+ip+1) <<
            " = "<<inputs[ip]<< endl;
        }
      }
      for (unsigned int ip=0; ip< warn.size(); ip++) {
        cout << "          "<<getAlgParName(m_anaName[ia], ip+1) <<
          "  warn=" << warn[ip] <<"  alarm="<<alarm[ip] << endl;
      }
      cout << "  Conditions: required status bits:"<<status <<
        " Min stat per bin:"<<minstat<<"    min bin fraction above threshold:"<<minstatfrac<<endl;
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

std::string& OnlineHistogram::algFromID(int anaid) {
  if(!m_anadirLoaded) loadAnalysisDirections();
  std::string& out=OnlineHistDBEnv_constants::unknown;
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
                                            std::vector<float> &parameters,
                                            int* optionalSourceSet
                                            ) {
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
      if(optionalSourceSet)
        *optionalSourceSet = SourceSet;
      out = true;
    }
  }
  return out;
}


int OnlineHistogram::declareAnalysis(std::string Algorithm, 
                                     std::vector<float>* warningThr, 
                                     std::vector<float>* alarmThr, 
                                     std::vector<float>* inputs, 
                                     int instance,
                                     std::string *Doc,
                                     std::string *Message) {
  if(!m_anadirLoaded) loadAnalysisDirections();
  // check that Algorithm exists and get number of parameters
  int out=0;
  bool known = false;
  OCIStmt *stmt=NULL;
  m_StmtMethod = "OnlineHistogram::declareAnalysis";
  std::string command = "BEGIN :out := ONLINEHISTDB.DECLAREANALYSIS(theSet =>:id,Algo =>:alg,warn=>:w,alr=>:a,instance=>:ins,inputs=>:inp";
  if (Doc)
    command += ",Doc=>'" + (*Doc) +"'";
  if (Message)
    command += ",Message=>'" + (*Message) +"'";
  command += "); END;";
  if ( OCI_SUCCESS == prepareOCIStatement
       (stmt, command.c_str()) ) {
    OCIArray *warn,*alarm,*inps;
    checkerr( OCIObjectNew ( m_envhp, m_errhp, m_svchp, OCI_TYPECODE_TABLE,
                             OCIthresholds, (dvoid *) 0, OCI_DURATION_SESSION, TRUE,
                             (dvoid **) &warn));
    checkerr( OCIObjectNew ( m_envhp, m_errhp, m_svchp, OCI_TYPECODE_TABLE,
                             OCIthresholds, (dvoid *) 0, OCI_DURATION_SESSION, TRUE,
                             (dvoid **) &alarm));
    checkerr( OCIObjectNew ( m_envhp, m_errhp, m_svchp, OCI_TYPECODE_TABLE,
                             OCIthresholds, (dvoid *) 0, OCI_DURATION_SESSION, TRUE,
                             (dvoid **) &inps));
    if(warningThr && alarmThr) {
      floatVectorToVarray(*warningThr, warn);
      floatVectorToVarray(*alarmThr, alarm);
    }
    if(inputs) {
      floatVectorToVarray(*inputs, inps);
    }
    myOCIBindInt   (stmt,":out", out);
    myOCIBindInt   (stmt,":id" , m_hsid);
    myOCIBindString(stmt,":alg", Algorithm);
    myOCIBindObject(stmt,":w"  , (void **) &warn, OCIthresholds);
    myOCIBindObject(stmt,":a"  , (void **) &alarm, OCIthresholds);
    myOCIBindInt   (stmt,":ins", instance);
    myOCIBindObject(stmt,":inp", (void **) &inps, OCIthresholds);
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
    checkerr(OCIObjectFree ( m_envhp, m_errhp, inps, OCI_OBJECTFREE_FORCE) );
    releaseOCIStatement(stmt);
  }
  return out;
}

bool OnlineHistogram::setAnalysis(int AnaID, 
                                  std::vector<float>* warningThr, 
                                  std::vector<float>* alarmThr, 
                                  std::vector<float>* inputs)
{
  if(!m_anadirLoaded) loadAnalysisDirections();
  bool out=false;
  OCIStmt *stmt=NULL;
  m_StmtMethod = "OnlineHistogram::setAnalysis";
  if ( OCI_SUCCESS == prepareOCIStatement
       (stmt, "BEGIN ONLINEHISTDB.SETSPECIALANALYSIS(theAna=>:id,theHisto=>:hid,warn=>:w,alr=>:a,inputs=>:inp);END;"  ) ) {
    OCIArray *warn,*alarm,*inps;
    checkerr( OCIObjectNew ( m_envhp, m_errhp, m_svchp, OCI_TYPECODE_TABLE,
                             OCIthresholds, (dvoid *) 0, OCI_DURATION_SESSION, TRUE,
                             (dvoid **) &warn));
    checkerr( OCIObjectNew ( m_envhp, m_errhp, m_svchp, OCI_TYPECODE_TABLE,
                             OCIthresholds, (dvoid *) 0, OCI_DURATION_SESSION, TRUE,
                             (dvoid **) &alarm));
    checkerr( OCIObjectNew ( m_envhp, m_errhp, m_svchp, OCI_TYPECODE_TABLE,
                             OCIthresholds, (dvoid *) 0, OCI_DURATION_SESSION, TRUE,
                             (dvoid **) &inps));
    if(warningThr && alarmThr) {
      floatVectorToVarray(*warningThr, warn);
      floatVectorToVarray(*alarmThr, alarm);
    }
    if(inputs) {
      floatVectorToVarray(*inputs, inps);
    }
    myOCIBindInt   (stmt,":id" , AnaID);
    myOCIBindString(stmt,":hid", m_hid);
    myOCIBindObject(stmt,":w"  , (void **) &warn, OCIthresholds);
    myOCIBindObject(stmt,":a"  , (void **) &alarm, OCIthresholds);
    myOCIBindObject(stmt,":inp", (void **) &inps, OCIthresholds);
    out = (OCI_SUCCESS == myOCIStmtExecute(stmt));
    checkerr(OCIObjectFree ( m_envhp, m_errhp, warn, OCI_OBJECTFREE_FORCE) );
    checkerr(OCIObjectFree ( m_envhp, m_errhp, alarm, OCI_OBJECTFREE_FORCE) );
    checkerr(OCIObjectFree ( m_envhp, m_errhp, inps, OCI_OBJECTFREE_FORCE) );
    releaseOCIStatement(stmt);
  }
  return out;
}

bool OnlineHistogram::getAnaSettings(int AnaID,
                                     std::vector<float>* warnTh, 
                                     std::vector<float>* alarmTh,
                                     std::vector<float>* inputs,
                                     bool &mask,
                                     long &statusbits,
                                     float &minstat,
                                     float &minstatfrac)  {
  if(!m_anadirLoaded) loadAnalysisDirections();
  bool out=true;
  
  int imask=0;
  long stb=0;
  float mst=0.,msf=0.;
  OCIStmt *stmt=NULL;
  sb2 statusbits_null, minstat_null, minstatfrac_null;
  m_StmtMethod = "OnlineHistogram::getAnaSettings";
  if ( OCI_SUCCESS == prepareOCIStatement
       (stmt, "BEGIN ONLINEHISTDB.GETANASETTINGS(theAna=>:id,theHisto=>:hid,warn=>:w,alr=>:a,mask=>:m,inputs=>:inp,stBits=>:stb,minstat=>:mst,minstatfrac=>:msf); END;" ) ) {
    OCIArray *warn,*alarm,*inps;
    checkerr( OCIObjectNew ( m_envhp, m_errhp, m_svchp, OCI_TYPECODE_TABLE,
                             OCIthresholds, (dvoid *) 0, OCI_DURATION_SESSION, TRUE,
                             (dvoid **) &warn));
    checkerr( OCIObjectNew ( m_envhp, m_errhp, m_svchp, OCI_TYPECODE_TABLE,
                             OCIthresholds, (dvoid *) 0, OCI_DURATION_SESSION, TRUE,
                             (dvoid **) &alarm));
    checkerr( OCIObjectNew ( m_envhp, m_errhp, m_svchp, OCI_TYPECODE_TABLE,
                             OCIthresholds, (dvoid *) 0, OCI_DURATION_SESSION, TRUE,
                             (dvoid **) &inps));
    myOCIBindInt   (stmt,":id" , AnaID);
    myOCIBindString(stmt,":hid", m_hid);
    myOCIBindObject(stmt,":w"  , (void **) &warn, OCIthresholds);
    myOCIBindObject(stmt,":a"  , (void **) &alarm, OCIthresholds);
    myOCIBindInt   (stmt,":m"  , imask);
    myOCIBindObject(stmt,":inp", (void **) &inps, OCIthresholds);
    myOCIBindInt   (stmt,":stb", stb, &statusbits_null);
    myOCIBindFloat (stmt,":mst", mst, &minstat_null);
    myOCIBindFloat (stmt,":msf", msf, &minstatfrac_null);
    if (OCI_SUCCESS == myOCIStmtExecute(stmt)) {
      mask = (imask == 1);
      if(warnTh && alarmTh) {
        floatVarrayToVector(warn, *warnTh);
        floatVarrayToVector(alarm, *alarmTh);
      }
      if(inputs) {
        floatVarrayToVector(inps, *inputs);
      }
      statusbits = statusbits_null ? -1 : stb;
      minstat = minstat_null ? -1.f : mst;
      minstatfrac = minstatfrac_null ? -1.f : msf;
    }
    else {
      out=false;
    }
    checkerr(OCIObjectFree ( m_envhp, m_errhp, warn, OCI_OBJECTFREE_FORCE) );
    checkerr(OCIObjectFree ( m_envhp, m_errhp, alarm, OCI_OBJECTFREE_FORCE) );
    checkerr(OCIObjectFree ( m_envhp, m_errhp, inps, OCI_OBJECTFREE_FORCE) );
    releaseOCIStatement(stmt);
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
  if (m_Histenv->debug() > 2) std::cout << "Deleting "<<
    m_myHist.size() << " OnlineHistogram objects"<<std::endl;
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
  m_Histenv->setStmtMethod("OnlineHistDB::getHistogram");
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
      m_Histenv->warningMessage("cannot create histogram object "+Identifier);
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
    (*ih)->reload();
  }
}

//=========================================================================
//  Retrieve all the set options of type int
//=========================================================================
void OnlineHistogram::getIntDisplayOptions( std::map<std::string, int>&  buf ) {
  int iopt;
  buf.clear();
  std::vector<OnlineDisplayOption*>::iterator ip;
  for (ip = m_do.begin();ip != m_do.end(); ++ip) {
    if ((*ip)->isSet()) {
      if ( (*ip)->type() == OnlineDisplayOption::INT ) {
        (*ip)->get(&iopt);
        buf[(*ip)->name()] = iopt;
      }
    }
  }
}
//=========================================================================
//  Retrieve all the set options of type float
//=========================================================================
void OnlineHistogram::getFloatDisplayOptions( std::map<std::string, float>&  buf ) {
  float fopt;
  buf.clear();
  std::vector<OnlineDisplayOption*>::iterator ip;
  for (ip = m_do.begin();ip != m_do.end(); ++ip) {
    if ((*ip)->isSet()) {
      if ( (*ip)->type() == OnlineDisplayOption::FLOAT ) {
        (*ip)->get(&fopt);
        buf[(*ip)->name()] = fopt;
      }
    }
  }
}
//=========================================================================
//  Retrieve all the set options of type string
//=========================================================================
void OnlineHistogram::getStringDisplayOptions( std::map<std::string, std::string>&  buf ) {
  std::string sopt;
  buf.clear();
  std::vector<OnlineDisplayOption*>::iterator ip;
  for (ip = m_do.begin();ip != m_do.end(); ++ip) {
    if ((*ip)->isSet()) {
      if ( (*ip)->type() == OnlineDisplayOption::STRING ) {
        (*ip)->get(&sopt);
        buf[(*ip)->name()] = sopt;
      }
    }
  }
}
