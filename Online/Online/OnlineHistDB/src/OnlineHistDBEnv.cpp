//$Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/OnlineHistDB/src/OnlineHistDBEnv.cpp,v 1.25 2010-10-25 14:09:28 ggiacomo Exp $
#include <cctype>
#include <cstring>
#include <cstdlib>
#include "OnlineHistDB/OnlineHistDBEnv.h"
using namespace std;
using namespace OnlineHistDBEnv_constants;

// constructor for creating new DB session
OnlineHistDBEnv::OnlineHistDBEnv(std::string passwd,
                                 std::string user,
                                 std::string db,
                                 bool newsession) 
  :  m_ownEnv(newsession), m_envhp(NULL), m_errhp(NULL), m_svchp(NULL),
     OCIthresholds(NULL), OCIparameters(NULL), OCIlabels(NULL), 
     OCIintlist(NULL), OCIinttlist(NULL), OCIanalist(NULL), OCIhnalist(NULL), OCIflolist(NULL),    
     m_TaggedStatement(NULL),
     m_refRoot(NULL), m_savesetsRoot(NULL),
     m_TStorage(NULL), m_HStorage(NULL), m_PStorage(NULL), 
     m_user(user), m_debug(0), m_excLevel(1)
{
  toUpper(m_user);
  initOCIBinds();
  if(newsession) {
    // initialize Oracle session and log in
    m_StmtMethod = "OnlineHistDB::OnlineHistDB";
    m_ownEnv = true;
    if (m_debug > 2) std::cout<< "creating new OCI session "<<std::endl;
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
    m_refRoot = new std::string(OnlineHistDBEnv_constants::StdRefRoot);
    char * envRefRoot = getenv ("HISTREFPATH");
    if (envRefRoot !=NULL)
      *m_refRoot = envRefRoot;
    
    m_savesetsRoot = new std::string(OnlineHistDBEnv_constants::StdSavesetsRoot);
    char * envSvsRoot = getenv ("HISTSAVESETSPATH");
    if (envSvsRoot !=NULL)
      *m_savesetsRoot = envSvsRoot;
  }
}

// constructor for passing already existing session environment
OnlineHistDBEnv::OnlineHistDBEnv(OnlineHistDBEnv &m) :
  m_ownEnv(false) {
  copyEnv(m);
}

void OnlineHistDBEnv::copyEnv(OnlineHistDBEnv &m) {
  m_envhp = m.m_envhp; m_errhp=m.m_errhp; m_svchp = m.m_svchp;
  m_user=m.m_user;
  m_debug = m.debug(); m_excLevel = m.excLevel();
  OCIthresholds = m.OCIthresholds;
  OCIparameters = m.OCIparameters;
  OCIlabels = m.OCIlabels;
  OCIintlist = m.OCIintlist;
  OCIinttlist = m.OCIinttlist;
  OCIanalist = m.OCIanalist;
  OCIhnalist = m.OCIhnalist;
  OCIflolist = m.OCIflolist;
  OCIdispopt = m.OCIdispopt;
  m_TaggedStatement = m.m_TaggedStatement;
  m_TStorage = m.m_TStorage;
  m_HStorage = m.m_HStorage;
  m_PStorage = m.m_PStorage;
  m_refRoot = m.m_refRoot;
  m_savesetsRoot = m.m_savesetsRoot;
  initOCIBinds();
}


// dummy constructor for no-DB operation
OnlineHistDBEnv::OnlineHistDBEnv()
  :  m_ownEnv(false), m_envhp(NULL), m_errhp(NULL), m_svchp(NULL),
     OCIthresholds(NULL), OCIparameters(NULL), OCIlabels(NULL), 
     OCIintlist(NULL), OCIinttlist(NULL), OCIanalist(NULL), OCIhnalist(NULL), OCIflolist(NULL),    
     m_TaggedStatement(NULL),
     m_refRoot(NULL), m_savesetsRoot(NULL),
     m_TStorage(NULL), m_HStorage(NULL), m_PStorage(NULL), 
     m_user(""), m_debug(0), m_excLevel(1)
{}


OnlineHistDBEnv::~OnlineHistDBEnv() {
  if (m_debug > 3) std::cout<< "removing OnlineHistDBEnv"<<std::endl;
  if(m_ownEnv) {
    if (m_debug > 2) std::cout<< "removing OCI session "<<std::endl;
    if(m_TaggedStatement) delete m_TaggedStatement;
    if(m_refRoot) delete m_refRoot;
    if(m_savesetsRoot) delete m_savesetsRoot;
    OCICacheFree ( m_envhp, m_errhp, m_svchp);
    OCILogoff ( m_svchp, m_errhp);
    if (m_envhp)
      OCIHandleFree((dvoid *) m_envhp, OCI_HTYPE_ENV);
  }
}

void OnlineHistDBEnv::initOCIBinds() {
  for (int i=0; i<20 ;++i)
    m_bnd[i] = (OCIBind *) NULL;
  m_curBind =0;
}

void OnlineHistDBEnv::checkCurBind() {
  if(m_curBind == 40) {
    std::string error("FATAL ERROR IN OnlineHistDBEnv::checkCurBind : maximum number of binds reached");
    if (m_debug > -1) {
      std::cout << error<<std::endl;
    }
    resetTaggedStatements();
    throw error; 
  }
}

void OnlineHistDBEnv::getOCITypes() {

  checkerr(OCITypeByName(m_envhp, m_errhp, m_svchp, (const text *) 0,
			 (ub4) 0, (const text *) "VTHRESHOLDS",
			 (ub4) strlen((const char *) "VTHRESHOLDS"),
			 (const_ora_text *) 0, (ub4) 0,
			 OCI_DURATION_SESSION,  OCI_TYPEGET_HEADER,
			 &OCIthresholds)); 
  checkerr(OCITypeByName(m_envhp, m_errhp, m_svchp, (const text *) 0,
			 (ub4) 0, (const text *) "VPARAMETERS",
			 (ub4) strlen((const char *) "VPARAMETERS"),
			 (const_ora_text *) 0, (ub4) 0,
			 OCI_DURATION_SESSION,  OCI_TYPEGET_HEADER,
			 &OCIparameters)); 
  checkerr(OCITypeByName(m_envhp, m_errhp, m_svchp, (const text *) 0,
			 (ub4) 0, (const text *) "LABELS",
			 (ub4) strlen((const char *) "LABELS"),
			 (const_ora_text *) 0, (ub4) 0,
			 OCI_DURATION_SESSION,  OCI_TYPEGET_HEADER,
			 &OCIlabels)); 
  checkerr(OCITypeByName(m_envhp, m_errhp, m_svchp, (const text *) 0,
			 (ub4) 0, (const text *) "INTLIST",
			 (ub4) strlen((const char *) "INTLIST"),
			 (const_ora_text *) 0, (ub4) 0,
			 OCI_DURATION_SESSION,  OCI_TYPEGET_HEADER,
			 &OCIintlist)); 
  checkerr(OCITypeByName(m_envhp, m_errhp, m_svchp, (const text *) 0,
			 (ub4) 0, (const text *) "INTTLIST",
			 (ub4) strlen((const char *) "INTTLIST"),
			 (const_ora_text *) 0, (ub4) 0,
			 OCI_DURATION_SESSION,  OCI_TYPEGET_HEADER,
			 &OCIinttlist));
  checkerr(OCITypeByName(m_envhp, m_errhp, m_svchp, (const text *) 0,
			 (ub4) 0, (const text *) "ANALIST",
			 (ub4) strlen((const char *) "ANALIST"),
			 (const_ora_text *) 0, (ub4) 0,
			 OCI_DURATION_SESSION,  OCI_TYPEGET_HEADER,
			 &OCIanalist)); 
  checkerr(OCITypeByName(m_envhp, m_errhp, m_svchp, (const text *) 0,
			 (ub4) 0, (const text *) "HNALIST",
			 (ub4) strlen((const char *) "HNALIST"),
			 (const_ora_text *) 0, (ub4) 0,
			 OCI_DURATION_SESSION,  OCI_TYPEGET_HEADER,
			 &OCIhnalist)); 
  checkerr(OCITypeByName(m_envhp, m_errhp, m_svchp, (const text *) 0,
			 (ub4) 0, (const text *) "FLOLIST",
			 (ub4) strlen((const char *) "FLOLIST"),
			 (const_ora_text *) 0, (ub4) 0,
			 OCI_DURATION_SESSION,  OCI_TYPEGET_HEADER,
			 &OCIflolist)); 
  checkerr(OCITypeByName(m_envhp, m_errhp, m_svchp, (const text *) 0,
			 (ub4) 0, (const text *) "DISPOPT",
			 (ub4) strlen((const char *) "DISPOPT"),
			 (const_ora_text *) 0, (ub4) 0,
			 OCI_DURATION_SESSION,  OCI_TYPEGET_ALL,
			 &OCIdispopt)); 
}

void OnlineHistDBEnv::resetTaggedStatements() 
{
  if(m_TaggedStatement)
    m_TaggedStatement->clear();  
}


void OnlineHistDBEnv::warningMessage(std::string Error) const {
  if (m_debug > -1) {
    std::cout << "------- WARNING: ---------- from "  << m_StmtMethod << std::endl;
    std::cout<< Error <<std::endl;
  }     
}

void OnlineHistDBEnv::toUpper(std::string& str) {
  std::string::iterator i;
  for (i = str.begin() ; i != str.end() ; i++) {
    *i = (char) toupper(*i);
  }
}

std::string OnlineHistDBEnv::PagenameSyntax(std::string &fullname, std::string &folder) {
  text outname[VSIZE_PAGENAME]="";
  text fold[VSIZE_FOLDER]="";
  std::string command = "BEGIN :1 := ONLINEHISTDB.PAGENAMESYNTAX(TheName => :name,Folder => :2); END;";
  OCIStmt *stmt=NULL;
  m_StmtMethod = "OnlineHistDBEnv::PagenameSyntax";
  if ( OCI_SUCCESS == prepareOCITaggedStatement(stmt, command.c_str(), "PAGESYN") ) {
    myOCIBindString(stmt, ":name", fullname);
    myOCIBindString(stmt, ":1", outname, VSIZE_PAGENAME);
    myOCIBindString(stmt, ":2", fold , VSIZE_FOLDER);
    if (OCI_SUCCESS == myOCIStmtExecute(stmt)) {
      folder = (char *) fold;
    }
    releaseOCITaggedStatement(stmt, "PAGESYN");
  }
  return std::string((char *) outname);
}

int OnlineHistDBEnv::getAlgorithmNpar(std::string& AlgName, int* Ninput)
{
  int xNpar= -1, xNinp=0;
  m_StmtMethod = "OnlineHistDB::getAlgorithmNpar";
  OCIStmt *stmt=NULL;
  if (OCI_SUCCESS == 
      prepareOCIStatement(stmt, "BEGIN ONLINEHISTDB.GETALGONPAR(:x1,:x2, :x3); END;") ) {
    myOCIBindString(stmt, ":x1", AlgName);
    myOCIBindInt(stmt, ":x2", xNpar);
    myOCIBindInt(stmt, ":x3", xNinp);
    if (OCI_SUCCESS == myOCIStmtExecute(stmt)) {
      if(Ninput) *Ninput = xNinp;
    }
    releaseOCIStatement(stmt);
  }
  return xNpar;
}

std::string OnlineHistDBEnv::getAlgParName(std::string& AlgName,
					int Ipar)
{
  text out[VSIZE_PARAMETERS]="";
  m_StmtMethod = "OnlineHistDB::getAlgParName";
  OCIStmt *stmt=NULL;
  float defv;
  if (OCI_SUCCESS == 
      prepareOCIStatement(stmt, "BEGIN ONLINEHISTDB.GETALGOPARNAME(:x1,:x2,:x3, :def); END;") ) {
    myOCIBindString(stmt, ":x1", AlgName);
    myOCIBindInt(stmt, ":x2", Ipar);
    myOCIBindString(stmt, ":x3", out, VSIZE_PARAMETERS);
    myOCIBindFloat(stmt, ":def", defv);
    myOCIStmtExecute(stmt);
    releaseOCIStatement(stmt);
  }
  return std::string((const char *) out);
}



sword OnlineHistDBEnv::prepareOCIStatement(OCIStmt* & stmt, 
					  const char *sqlcommand,
					  bool cleanBinds) {
  if (cleanBinds)
    initOCIBinds();
  sword out=
    checkerr(OCIHandleAlloc( (dvoid *) m_envhp, (dvoid **) &stmt,
			     OCI_HTYPE_STMT, (size_t) 0, (dvoid **) 0));
 
  if (OCI_SUCCESS == out) {
    if (m_debug>3) std::cout << "preparing untagged statement "<<sqlcommand<<std::endl;
    out = checkerr(OCIStmtPrepare2(m_svchp, &stmt, m_errhp, (text *) sqlcommand,
				   (ub4) strlen((char *) sqlcommand), (const_ora_oratext  *) NULL, (ub4) 0,
				   (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT) );
  }
  return out;
}
 
sword OnlineHistDBEnv::prepareOCITaggedStatement(OCIStmt* & stmt, 
						 const char *sqlcommand,
						 const char *StmtKey,
						 bool cleanBinds) {
  sword out;
  if (cleanBinds)
    initOCIBinds();
  std::string key(StmtKey);

  if(m_TaggedStatement->count(key) ) {// use tagged statement
    if (m_debug>3) std::cout << "using tagged statement "<<StmtKey<<std::endl;
    out = checkerr(OCIStmtPrepare2(m_svchp, &stmt, m_errhp, (text *) NULL,
				   (ub4) 0, (const OraText*) StmtKey, (ub4) (ub4)strlen(StmtKey),
				   (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT));
  }
  else {// first call of this statement
    if (m_debug>3) std::cout << "first call for statement "<<StmtKey<<std::endl;
    out = checkerr(OCIStmtPrepare2(m_svchp, &stmt, m_errhp, (text *) sqlcommand,
				   (ub4) strlen((char *) sqlcommand), (const_ora_oratext  *) NULL, (ub4) 0,
				   (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT));
  }
  return out;
}
 
sword OnlineHistDBEnv::releaseOCIStatement(OCIStmt* & stmt) {
  return checkerr(OCIStmtRelease ((OCIStmt *)stmt, (OCIError *)m_errhp,(const OraText*) NULL,
				  0, OCI_STRLS_CACHE_DELETE) );
}


sword OnlineHistDBEnv::releaseOCITaggedStatement(OCIStmt* & stmt, 
						 const char *StmtKey) {
  sword out;
  std::string key(StmtKey);
  out = checkerr(OCIStmtRelease ((OCIStmt *)stmt, (OCIError *)m_errhp,(const OraText*) StmtKey,
				 (ub4)strlen(StmtKey), OCI_DEFAULT) );
  if (OCI_SUCCESS == out) m_TaggedStatement->insert(key);
  return out;
}


sword OnlineHistDBEnv::myOCIBindInt(OCIStmt* stmt,
				    const char *str, 
				    int &var,
				    sb2 * ind) {
  checkCurBind();
  return
    checkerr( OCIBindByName(stmt, &(m_bnd[m_curBind++]), m_errhp, (text*) str,
			    -1, (dvoid *) &var,
			    (sword) sizeof(var), SQLT_INT,(dvoid *) ind,
			    (ub2 *) 0, (ub2 *) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT));
}

sword OnlineHistDBEnv::myOCIBindInt(OCIStmt* stmt,
				    const char *str, 
				    long  &var,
				    sb2 * ind) {
  checkCurBind();
  return
    checkerr( OCIBindByName(stmt, &(m_bnd[m_curBind++]), m_errhp, (text*) str,
			    -1, (dvoid *) &var,
			    (sword) sizeof(var), SQLT_INT,(dvoid *) ind,
			    (ub2 *) 0, (ub2 *) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT));
}


sword OnlineHistDBEnv::myOCIBindFloat(OCIStmt* stmt,
				      const char *str, 
				      float &var,
				      sb2 * ind) {
  checkCurBind();
  return
    checkerr( OCIBindByName(stmt, &(m_bnd[m_curBind++]), m_errhp, (text*) str,
			    -1, (dvoid *) &var,
			    (sword) sizeof(var), SQLT_FLT,(dvoid *) ind,
			    (ub2 *) 0, (ub2 *) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT));
}

sword OnlineHistDBEnv::myOCIBindString(OCIStmt* stmt,
				       const char *str, 
				       std::string &var,
				       sb2 * ind) {
  // for input variables (known size, variable doesn't change)
  checkCurBind();
  return 
    checkerr( OCIBindByName(stmt, &(m_bnd[m_curBind++]), m_errhp, (text*) str,
			    -1, (dvoid *) var.c_str(),
			    (sword) var.size()+1, SQLT_STR,(dvoid *) ind,
			    (ub2 *) 0, (ub2 *) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT));
}

sword OnlineHistDBEnv::myOCIBindString(OCIStmt* stmt,
				       const char *str, 
				       text* var,
				       int size,
				       sb2 * ind) {
  // for output variables; text allocated by caller
  checkCurBind();
  return
    checkerr( OCIBindByName(stmt, &(m_bnd[m_curBind++]), m_errhp, (text*) str,
			    -1, (dvoid *) var,
			    (sword) size, SQLT_STR,(dvoid *) ind,
			    (ub2 *) 0, (ub2 *) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT));
}

sword OnlineHistDBEnv::myOCIBindObject(OCIStmt* stmt,
				       const char *str, 
				       void** var,
				       OCIType* type,
				       void** ind_struct) {
  checkCurBind();
  OCIBind* thisbind;
  sword out =
    checkerr( OCIBindByName(stmt, &(m_bnd[m_curBind]), m_errhp, (text*) str,
			    -1, (dvoid *) 0,
			    (sword) 0, SQLT_NTY,(dvoid*) 0,
			    (ub2 *) 0, (ub2 *) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT));
  thisbind = m_bnd[m_curBind];
  m_curBind++;
  if ( OCI_SUCCESS == out) {
    out = checkerr( OCIBindObject(thisbind,  m_errhp, type, 
				  (dvoid **) var, (ub4 *) NULL,
				  (dvoid **) ind_struct, (ub4 *) NULL ) );
  }
  return out;
}


sword OnlineHistDBEnv::myOCIDefineInt(OCIStmt* stmt,
				      int position, 
				      int &var,
				      sb2 *isNULL) {
  OCIDefine *defnp = (OCIDefine *) 0;
  return
    checkerr( OCIDefineByPos ( stmt, &defnp, m_errhp,
			       (ub4) position,
			       (dvoid *) &var, (sword) sizeof(var), SQLT_INT,
			       (dvoid *) isNULL , (ub2 *) 0, (ub2 *) 0, OCI_DEFAULT ) );
}

sword OnlineHistDBEnv::myOCIDefineFloat(OCIStmt* stmt,
					int position, 
					float &var,
					sb2 *isNULL) {
  OCIDefine *defnp = (OCIDefine *) 0;
  return
    checkerr( OCIDefineByPos ( stmt, &defnp, m_errhp,
			       (ub4) position,
			       (dvoid *) &var, (sword) sizeof(var), SQLT_FLT,
			       (dvoid *) isNULL , (ub2 *) 0, (ub2 *) 0, OCI_DEFAULT ) );
}

sword OnlineHistDBEnv::myOCIDefineString(OCIStmt* stmt,
					 int position, 
					 text* var,
					 int size,
					 sb2 *isNULL) {
  OCIDefine *defnp = (OCIDefine *) 0;
  return
    checkerr( OCIDefineByPos ( stmt, &defnp, m_errhp,
			       (ub4) position,
			       (dvoid *) var, (sword) size, SQLT_STR,
			       (dvoid *) isNULL , (ub2 *) 0, (ub2 *) 0, OCI_DEFAULT ) );
}


sword OnlineHistDBEnv::myOCISelectExecute(OCIStmt* stmt,
                                       OCIErrorLevel level) {
  return
    checkerr (OCIStmtExecute(m_svchp, stmt, m_errhp, (ub4) 0, (ub4) 0,
                             (const_ora_ocisnapshot *) NULL, (OCISnapshot *) NULL, OCI_STMT_SCROLLABLE_READONLY),
              level);
}

sword OnlineHistDBEnv::myOCIStmtExecute(OCIStmt* stmt,
					OCIErrorLevel level) {
  return
    checkerr (OCIStmtExecute(m_svchp, stmt, m_errhp, (ub4) 1, (ub4) 0,
			     (const_ora_ocisnapshot *) NULL, (OCISnapshot *) NULL, OCI_DEFAULT),
	      level);
}


int OnlineHistDBEnv::myOCIFetch(OCIStmt* stmt,
				int Nf) {
  OCIStmtFetch2 ( stmt, m_errhp, Nf,
		  OCI_FETCH_NEXT, 0, OCI_DEFAULT);
  return(myOCIFetchedRows(stmt));
}


int OnlineHistDBEnv::myOCIFetchedRows(OCIStmt* stmt) {
  int nr=0;
  checkerr (OCIAttrGet((dvoid *) stmt, (ub4) OCI_HTYPE_STMT,
		       (dvoid *) &nr, (ub4 *) NULL, (ub4)OCI_ATTR_ROWS_FETCHED, m_errhp));
  return nr;
}


sword OnlineHistDBEnv::checkerr(sword status,
			       OCIErrorLevel level) {
  text errbuf[512];
  sb4 errcode = 0;
  bool out=true;
  int dboff=0;
  std::string error("");
  switch (status) {
  case OCI_SUCCESS:
    break;
  case OCI_SUCCESS_WITH_INFO:
    error += "OCI_SUCCESS_WITH_INFO";
    dboff=1;
    break;
  case OCI_NEED_DATA:
    error += "OCI_NEED_DATA";
    out=false;
    break;
  case OCI_NO_DATA:
    error += "OCI_NODATA";
    dboff=2;
    break;
  case OCI_ERROR:
    (void) OCIErrorGet((dvoid *)m_errhp, (ub4) 1, (text *) NULL, &errcode,
		       errbuf, (ub4) sizeof(errbuf), OCI_HTYPE_ERROR);
    error += (char *) errbuf;
    out=false;
    break;
  case OCI_INVALID_HANDLE:
    error += "OCI_INVALID_HANDLE";
    out=false;
    break;
  case OCI_STILL_EXECUTING:
    error += "OCI_STILL_EXECUTE";
    dboff=3;
    break;
  case OCI_CONTINUE:
    error += "OCI_CONTINUE";
    dboff=3;
    break;
  default:
    break;
  }

  if(!out) {
    std::stringstream message;
    message << ( level == NORMAL ?  "------- WARNING " : "------- ERROR ");
    message << "from "+m_StmtMethod + " -------" <<endl;
    message << error << endl;
    if (m_debug > -1)
      std::cout << message.str();
    if( (level == SEVERE && m_excLevel >0) ||
        (m_excLevel >1  && level > NORMAL) ) {
      resetTaggedStatements();
      throw message.str();
    }
  }
  else {
    if (m_debug > dboff && error.size() >0) {
      std::cout << "warning from "<<m_StmtMethod <<": "<<
        error << std::endl;
    }
  }

  return status;
}


void OnlineHistDBEnv::intVarrayToVector(OCIColl* col, std::vector<int> &v) {
  v.clear();
  unsigned int nc;
  checkerr( OCICollSize (m_envhp, m_errhp, col,(sb4 *) &nc));
  if (nc >0) {
    OCIIter *iterator; 
    boolean eoc = false; 
    checkerr( OCIIterCreate(m_envhp,m_errhp, col, &iterator));
    v.reserve(nc);
    while (!eoc) {
      void * element;
      int num;
      OCIIterNext(m_envhp, m_errhp, iterator,(dvoid **) &element, NULL, &eoc);
      if (!eoc) {
	checkerr( OCINumberToInt(m_errhp, (OCINumber *) element, 
				 sizeof(int), OCI_NUMBER_SIGNED, & num));
	v.push_back(num );
      }
    }
    checkerr(OCIIterDelete(m_envhp, m_errhp, &iterator));
  }
}

void OnlineHistDBEnv::floatVarrayToVector(OCIColl* col, std::vector<float> &v) {
  v.clear();
  int nc;
  checkerr( OCICollSize (m_envhp, m_errhp, col,(sb4 *) &nc));
  if (nc >0) {
    OCIIter *iterator; 
    boolean eoc = false; 
    checkerr( OCIIterCreate(m_envhp, m_errhp, col, &iterator));
    v.reserve(nc);
    while (!eoc) {
      void *element;
      float num;
      OCIIterNext(m_envhp, m_errhp, iterator,(dvoid **) &element, NULL, &eoc);
      if (!eoc) {
        checkerr( OCINumberToReal(m_errhp, (OCINumber *) element, 
				  sizeof(float),  & num));
        v.push_back(num );
      }
    }
     checkerr(OCIIterDelete(m_envhp, m_errhp, &iterator));
  }
}

void OnlineHistDBEnv::stringVarrayToVector(OCIColl* col, std::vector<std::string> &v) {
  v.clear();
  int nc;
  checkerr( OCICollSize (m_envhp, m_errhp, col,(sb4 *) &nc));
  if (nc >0) {
    OCIIter *iterator; 
    boolean eoc = false; 
    checkerr( OCIIterCreate(m_envhp, m_errhp, col, &iterator));
    v.reserve(nc);
    while (!eoc) {
      void *element;
      OCIIterNext(m_envhp, m_errhp, iterator,(dvoid **) &element, NULL, &eoc);
      if (!eoc)
        v.push_back( std::string( (const char*) OCIStringPtr(m_envhp,*(OCIString **)element)) );
    }
     checkerr(OCIIterDelete(m_envhp, m_errhp, &iterator));
  }
}

void OnlineHistDBEnv::floatVectorToVarray(std::vector<float> &v, OCIColl* col) {
  std::vector<float>::iterator iv;
  OCINumber num;
  for (iv = v.begin(); iv<v.end() ; iv++) {
    float x=*iv;
    checkerr ( OCINumberFromReal(m_errhp, (dvoid *) &x, (uword) sizeof(float), &num) );
    checkerr( OCICollAppend ( m_envhp, m_errhp, (dvoid *) &num,
			      (dvoid *) NULL, col) );
  }
}

void OnlineHistDBEnv::stringVectorToVarray(std::vector<std::string> &v,OCIColl* col) {
  std::vector<std::string>::iterator iv;
  OCIString *os = NULL;
  for (iv = v.begin(); iv<v.end() ; iv++) {
    checkerr ( OCIStringAssignText(m_envhp, m_errhp, (text *) iv->c_str(),
				   (ub4) iv->size(), &os) );
    checkerr( OCICollAppend ( m_envhp, m_errhp, (dvoid *) os,
			      (dvoid *) NULL, col) );
  }
}
