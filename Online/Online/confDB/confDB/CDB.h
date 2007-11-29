#ifndef CDB_INTERNAL
#define CDB_INTERNAL
#include "oci.h"
int ShowErrors (sword status, OCIError *errhp, const char* log) ;
class eConnectivityCache;
class iConnectivityCache;
class HopTyp
{
public:
  int typ;
  int h;
};

class DBContainer
{
private:
	OCIError* ociError; /* the error handle */ 
	OCIEnv* ociEnv; /* the environment handle */ 
	OCISvcCtx* ociHdbc; /* the context handle */
	OCIServer *mysrvhp; /* the server handle */
	OCISession *myusrhp; /* user session handle */
	char *serv;
	char *user;
	char *passw;
	void setup(char* server,char* usr,char* pwd);
	int connected;
public:
	DBContainer();
	~DBContainer();
	DBContainer(char* server,char* usr,char* pwd);
	DBContainer(char* server,char* usr,char* pwd,OCIEnv*& myenvhp,OCIServer*& mysrvhp1,OCISession*& myusrhp1, OCISvcCtx*& mysvchp, OCIError*& myerrhp, char* ErrorMessage);

	OCIError* gErrorHandle(void)
	{
		return ociError;
	};
	OCIEnv* gOCIEnv(void)
	{
		return ociEnv;
	};
	OCISvcCtx* gContxtHdl(void)
	{
		return ociHdbc;
	};
	void* gServerHandle(void)
	{
		return mysrvhp;
	};
	void* gSessionHandle(void)
	{
		return myusrhp;
	};
	void set_ErrorHandle(OCIError* e)
	{
		ociError = e;
	};
	void set_OCIEnv(OCIEnv* e)
	{
		ociEnv = e;
	};
	void set_ContxtHdl(OCISvcCtx* ctx)
	{
		 ociHdbc	= ctx;
	};
	void set_ServerHandle(OCIServer *s)
	{
		mysrvhp	= s;
	};
	void set_SessionHandle(OCISession *s)
	{
		myusrhp	= s;
	};
	int Connect(void);
	int Disconnect(void);
	//sword Disconnect(OCIEnv* env,OCIError* err,OCIServer* mysrvhp1,OCISession* myusrhp1, OCISvcCtx* mysvchp, int count_free, char* ErrorMessage);   

	//sword Connect(char* server,char* usr,char* pwd,OCIEnv*& myenvhp,OCIServer*& mysrvhp1,OCISession*& myusrhp1, OCISvcCtx*& mysvchp, OCIError*& myerrhp, char* ErrorMessage);

	/**
	* Load the subsystem table  returning an integer value.
	* @param sysnameList :list of system_name accroding to the ECS convention.
	* @param sysIDlist : user login.
	* @param len_array : len_array of sysIDlist(=nb of subsystem)
	* @param ErrorMessage : error message in case of failure
	* @return 0 if the disconnection is successful
	*/
	int LoadSubsystemTable(char sysnameList[50][10],int* sysIDlist, int &len_array,char* ErrorMessage);
	int followPath(eConnectivityCache *ecache, iConnectivityCache *icache, int elnkidx, HopTyp *hop, int *i, int dep);
	//int DBConnexion(char* server,char* usr,char* pwd,char* ErrorMessage);
	//int DBDeconnexion(char* ErrorMessage);
};



class ColDesc
{
public:
  int colwid;
  OCIDefine* def;
	OCIStmt *stmt;
	int Position;
	int nrow;
	int dtyp;
	char format[4];
	void **bptr;
	int bsize;
	int incrow;	//Number of new rows
	int ind;
	ub2	rcod;

ColDesc();
virtual ~ColDesc();


void set_colwid(int cw);
};
	
class ColDesc_int : public ColDesc
{
public:
  int *bufferp;

  ColDesc_int();
  void print(int indx);

int allocate(OCIStmt *statement, int pos, int numrow);

};

class ColDesc_char : public ColDesc
{
public:
  char *bufferp;
  ColDesc_char();
  ColDesc_char(int cwid);
  void print(int indx);
  int allocate(OCIStmt *statement, int pos, int numrow);

};
class ConnectivityCache
{
public:
  OCIStmt *stmthp;
  OCIBind  *bndp;
  DBContainer *db;
  OCIEnv* ociEnv;
  OCIError* ociError;
  OCISvcCtx* ociHdbc;
  int fetched_row;
  int maxrow;
  const char *selectstring;
  const char *cntselect;

  ConnectivityCache (DBContainer *dbase);
  virtual void alloc_buffs(void)=0;
	//virtual void realloc_buffs(void)=0;
	virtual void print_row(int idx)=0;
	int get_col_wid(OCISvcCtx* ociHdbc, void * env, char *selectstr);
	int getnumrows(int system);
  int Fill(int system);
  int FindinCol(ColDesc_int *col, int val, int typ, HopTyp *res);
  int FindinCol(ColDesc_char *col, char *val, int typ, HopTyp *res);
};


class eConnectivityCache : public ConnectivityCache
{
public:
  ColDesc_int linkid;
  ColDesc_int devfrom;
  ColDesc_int devto;
  ColDesc_char pnumfrom;
  ColDesc_char pnumto;
  ColDesc_int linktyp;
  ColDesc_int bidir;
  ColDesc_int linkused;
  ColDesc_int linkweight;
  ColDesc_char devnamfrom;
  ColDesc_char devnamto;
  ColDesc_int sysnam;
  ColDesc_int portidfrom;
  ColDesc_int portidto;
  ColDesc_char linkinfo;
  ColDesc_int odevid;

  eConnectivityCache (DBContainer *dbase) ;
  virtual ~eConnectivityCache() {}
  virtual void alloc_buffs(void);
  void print_row(int r);
};

class iConnectivityCache : public ConnectivityCache
{
public:
  ColDesc_int linkid;
  ColDesc_int devfrom;
  ColDesc_char devnamfrom;
  ColDesc_int portidfrom;
  ColDesc_char pnumfrom;

  ColDesc_int devto;
  ColDesc_char devnamto;
  ColDesc_int portidto;
  ColDesc_char pnumto;

  iConnectivityCache (DBContainer *dbase) ;
  virtual ~iConnectivityCache() {}
  virtual void alloc_buffs(void);
  void print_row(int r);
};

#endif
