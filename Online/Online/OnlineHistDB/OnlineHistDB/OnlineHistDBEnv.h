// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/OnlineHistDB/OnlineHistDB/OnlineHistDBEnv.h,v 1.13 2007-11-22 17:38:35 ggiacomo Exp $
#ifndef ONLINEHISTDBENV_H
#define ONLINEHISTDBENV_H 1
/** @class  OnlineHistDBEnv OnlineHistDBEnv.h OnlineHistDB/OnlineHistDBEnv.h
 * 
 *  environment class for C++ interface to the Online Monitoring Histogram DB
 *  @author Giacomo Graziani (INFN Firenze)
 *  @date 1/12/2006
 */

#include <iostream>
#include <sstream>
#include <set>
#include <vector>
#include <cctype>
#include <oci.h>
using namespace std;
class OnlineTaskStorage;
class OnlineHistogramStorage;
class OnlinePageStorage;

namespace OnlineHistDBEnv_constants {
  static const std::string DB="lbora01:1528/HISTOGRAMDB";
  static const std::string ACCOUNT="HIST_WRITER";
  static const unsigned int DBschema = 7;

  static const int NHTYPES=6;
  static const char HistTypeName[][4] = {
    "H1D", "H2D", "P1D", "P2D", "CNT", "SAM"
  };  

  static std::string RefRoot = "/home/online/Histograms/Reference"; 
  static const std::string m_SetSeparator = "_$";

  static const unsigned int VSIZE_SN          = 131;
  static const unsigned int VSIZE_HID         =  13;
  static const unsigned int VSIZE_TYPE        =   4;
  static const unsigned int VSIZE_HSTITLE     = 101;
  static const unsigned int VSIZE_SUBTITLE    =  51;
  static const unsigned int VSIZE_HSTASK      =  65;
  static const unsigned int VSIZE_HSALG       =  65;
  static const unsigned int VSIZE_DESCR       =2001;
  static const unsigned int VSIZE_DOC         = 201;
  static const unsigned int VSIZE_ALGNAME     =  31; 
  static const unsigned int VSIZE_NAME        = 131;
  static const unsigned int VSIZE_FOLDER      = 301;
  static const unsigned int VSIZE_PAGENAME    = 351;
  static const unsigned int VSIZE_PAGEDOC     = 101;
  static const unsigned int VSIZE_SSNAME      =  11;
  static const unsigned int VSIZE_REFERENCE   = 101;
  static const unsigned int VSIZE_PARAMETERS  =  16;
}




class OnlineHistDBEnv {
 public:
  /// get verbosity level (0 for no debug messages, up to 3)
  typedef enum { H1D, H2D, P1D, P2D, CNT, SAM} HistType;
  inline int debug() const { return m_debug;}
  /// set verbosity level
  void setDebug(int DebugLevel) { m_debug=DebugLevel;}
  /// exception level: 0 means never throw exc. to client code, 1 means only
  /// severe errors (default value), 2 means throw all exceptions.
  /// The default value is 1, meaning that exceptions are thrown only in
  /// case of severe DB inconsistency. All other errors, e.g. syntax errors,
  /// can be checked from the method return values and from the warning
  /// messages on the standard output.
  /// Exceptions can be catched using {\it catch(std::string message)}
  int excLevel() const {return m_excLevel;}
  ///
  void setExcLevel(int ExceptionLevel) {m_excLevel=ExceptionLevel;}
  /// check the syntax of the page full name, returning the correct syntax and the folder name 
  std::string PagenameSyntax(std::string fullname, std::string &folder);
  /// gets the number of parameters, and optionally the number of input histograms, needed by algorithm AlgName.
  int getAlgorithmNpar(std::string AlgName,
		       int* Ninput = NULL);
  /// gets the name of parameter Ipar (starting from 1) of algorithm AlgName
  std::string getAlgParName(std::string AlgName,
			    int Ipar);
  void errorMessage(std::string Error) const; 
  inline OCIEnv *envhp() const {return m_envhp;}
  inline OCIError *errhp() const {return m_errhp;}
  inline OCISvcCtx *svchp() const {return m_svchp;}


 protected:
  OnlineHistDBEnv(std::string User); 
  /* OnlineHistDBEnv(OCIEnv *Env, */
/* 		  OCIError *Err, */
/* 		  OCISvcCtx *Svc, */
/*   		  std::string User,  */
/*   		  int ExcLevel); */
  OnlineHistDBEnv(OnlineHistDBEnv &m);
  virtual ~OnlineHistDBEnv() 
  {  };

  // methods for OCI statements and OCI error handling
  std::string m_StmtMethod;
  typedef enum { 
    NORMAL,      // error that may occur normally  
    MODERATE,    // error that can be tolerated 
    SEVERE       // severe error that always triggers an exception
  } OCIErrorLevel;
  sword checkerr(sword status,
		OCIErrorLevel level = MODERATE);


  void initOCIBinds();
  void getOCITypes();

  sword prepareOCIStatement(OCIStmt* & stmt, 
			    const char *sqlcommand,
			    bool cleanBinds=true);
  sword prepareOCITaggedStatement(OCIStmt* & stmt, 
				  const char *sqlcommand,
				  const char *StmtKey,
				  bool cleanBinds=true);
  sword releaseOCITaggedStatement(OCIStmt* & stmt, 
				  const char *StmtKey);
  sword myOCIBindInt(OCIStmt* stmt,
		     const char *str, 
		     int &var,
		     sb2 * ind = NULL);
  sword myOCIBindFloat(OCIStmt* stmt,
		       const char *str, 
		       float &var,
		       sb2 * ind = NULL);
  sword myOCIBindString(OCIStmt* stmt,
			const char *str, 
			std::string &var,
			sb2 * ind = NULL);
  sword myOCIBindString(OCIStmt* stmt,
			const char *str, 
			text* var,
			int size,
			sb2 * ind = NULL);
  sword myOCIBindObject(OCIStmt* stmt,
			const char *str, 
			void** var,
			OCIType* type,
			void** ind_struct = NULL);
  sword myOCIDefineInt(OCIStmt* stmt,
		      int position, 
		      int &var,
		      sb2 *isNULL = NULL);
  sword myOCIDefineFloat(OCIStmt* stmt,
			 int position, 
			 float &var,
			 sb2 *isNULL = NULL);
  sword myOCIDefineString(OCIStmt* stmt,
			  int position, 
			  text* var,
			  int size,
			  sb2 *isNULL = NULL);
  sword myOCISelectExecute(OCIStmt* stmt,
			OCIErrorLevel level = MODERATE);
  sword myOCIStmtExecute(OCIStmt* stmt,
			OCIErrorLevel level = MODERATE);
  int myOCIFetch(OCIStmt* stmt,
		 int Nf);
  int myOCIFetchedRows(OCIStmt* stmt);
    
  void intVarrayToVector(OCIColl* col, std::vector<int> &v);
  void floatVarrayToVector(OCIColl* col, std::vector<float> &v);
  void stringVarrayToVector(OCIColl* col, std::vector<std::string> &v);
  void floatVectorToVarray(std::vector<float> &v, OCIColl* col);
  void stringVectorToVarray(std::vector<std::string> &v,OCIColl* col);
  OCIEnv *m_envhp;
  OCIError *m_errhp;
  OCISvcCtx *m_svchp;

  OCIType *OCIthresholds;
  OCIType *OCIparameters;
  OCIType *OCIintlist;
  OCIType *OCIanalist;
  OCIType *OCIhnalist;
  OCIType *OCIflolist;
  OCIType *OCIdispopt;

  std::set<std::string> *m_TaggedStatement;

  OnlineTaskStorage *m_TStorage;
  OnlineHistogramStorage *m_HStorage;
  OnlinePageStorage *m_PStorage;

 private: 
  std::string m_user;

  OCIBind* m_bnd[40];
  unsigned int m_curBind;

  void checkCurBind();
  std::string toUpper(string str);
  int m_debug;
  int m_excLevel;

};

#endif // ONLINEHISTDBENV_H
