#include "oci.h"
#include <iostream>
#include <math.h>
#include <ctime>
#include "DBparam.h"

#define MINSTRINGLEN 50
#define ERROR_BUFFER 200
#define MAXROWS 10000
//#if defined( _TEST33_) || defined( __INITIALIZE_STRUCTURE_)
//#include "test33.h"
//#endif




int NormalizeVector(char* VectorList, int nbofElement,int max_sourceLength,char* VectorDestination);
int AppendInt(int source_copy,int* &VectorList, int &Vector_currentsize, int first_time);
int AppendString(char* source_copy,char* &VectorList, int &Vector_currentsize,int &nbofElement,int &max_sourceLength, int first_time);
int ShowErrors (sword status, OCIError *errhp, const char* log);
int null_charptr(char* ptr_name);
void Format_output(int output_oraclecode, char* attribute_value, char* logmessage, char separator);
void MinStringLength(int &actual_len);
void OCIReportError(OCIError *errhp,const char* ApplicName, char* errMessg, int errnum);
void GetErrorMess(const char* ApplicName, const char* logMess,char* errMessg, int errnum); 
//int GetSubsystemList(char* subList,int &lenarray, char* subList1,int &lenarray1);
int GetSubsystemID(char * sysnameList);
int RetrieveSysIDList(char * sysnameList,char* sysIDlist);
int DecomposeSysID(int sysID,char* sysIDlist);
int GetDeviceStatus(int devstatus,char* device_status);
int GetDeviceStatusID(char* device_status);
int DecomposeSysID_bis(int sysID,char* sysIDlist);
int StmtPrepare(OCIStmt* stmthp,OCIError* ociError, char* stmt, sword* status);
int ParamGet(OCIStmt* stmthp, OCIError* ociError, OCIParam** parmdp, int pos, sword* status);
int AttrGet(OCIParam* parmdp, sb4* attrval, OCIError* ociError, sword* status);
int BindByName(OCIStmt* stmthp,OCIBind** bnd1p, OCIError* ociError, char* placeholder, dvoid* valuep,sb4 value_sz,ub2 dty,int* indp, sword* status);
int StmtExecute(OCISvcCtx* ociHdbc, OCIStmt* stmthp, OCIError* ociError,ub4 iters, sword* status);
//
void RemoveSeparator(char* attribute_value, char* separator);
///
int EnvCreate(OCIEnv*& myenvhp, sword* status);
int HandleAlloc(OCIEnv* ociEnv,dvoid** stmthp,ub4 type, sword* status);
int ServerAttach(OCIServer* srvhp,OCIError* errhp,char* dblink, sword* status);
int AttrSet (dvoid* hndlp,ub4 hndltype,dvoid* attributep,ub4 sizep, ub4 attrtype,OCIError* ociError, sword* status);
int SessionBegin(OCISvcCtx *svchp,OCIError *errhp,OCISession *usrhp,ub4 credt,sword* status);
int DefineByPos(OCIStmt* stmthp,OCIDefine** def, OCIError* ociError, int pos,  dvoid* valuep, sb4 value_sz, ub2 dtype, sword* status);
int BindArrayOfStruct(OCIBind *bindp,OCIError *errhp,ub4 pvskip,ub4 indskip,sword* status );
int TransCommit(OCISvcCtx *svchp,OCIError *errhp, ub4 flags, sword* status );
/////////
int null_charptr(char* ptr_name, int* len);

class Error
{
public:
	OCIError* ociError;
	char log[100];
	char* msg;
};