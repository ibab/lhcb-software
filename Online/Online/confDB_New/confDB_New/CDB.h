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
	//////////////////////////
	//getting connections by link type:
	int followPath_normal_LinkType(int link_type, eConnectivityCache *ecache, iConnectivityCache *icache, int elnkidx, HopTyp *hop, int *i, int dep);
	int followPath_reverse_LinkType(int link_type, eConnectivityCache *ecache, iConnectivityCache *icache, int elnkidx, HopTyp *hop, int *i, int dep);
	int followPath_device_LinkType(char* dev_name, int link_type, eConnectivityCache *ecache, iConnectivityCache *icache, int elnkidx, HopTyp *hop, int *i, int dep);
	int GetConnectivityBetweenDevices_LinkType(char* dev_from, char* dev_to, int link_type, eConnectivityCache *ecache, iConnectivityCache *icache, int system_name, int reload_cache);
	int GetAllPathsPerDevice_LinkType(char* dev_name, int link_type, eConnectivityCache *ecache, iConnectivityCache *icache, int system_name, int reload_cache);

	/////
		//followPath_device used in GetConnectivityBetweenDevices
	int followPath_device(char* dev_name, eConnectivityCache *ecache, iConnectivityCache *icache, int elnkidx, HopTyp *hop, int *i, int dep, int& len_array, int &pfrom1_arraylen,int &nfrom1_arraylen,int &pto1_arraylen,int &nto1_arraylen,int &lkinfo_arraylen,char* nfrom1_list,char* nto1_list,char* pfrom1_list,int* pfrom_list,char* pto1_list,int* pto_list,int* pathid_list,int* link_pos_list,char* lkinfo_list);//ok	
		//followPath_devType and followPath_devType_reverse are used in GetDetailedConnBetweenDeviceDevType
	int followPath_devType(char* dType_to, eConnectivityCache *ecache, iConnectivityCache *icache, int elnkidx, HopTyp *hop, int *i, int dep,char* nfrom_list,char* pfrom_list,char* pto_list,int& len_array,int & nfrom_arraylen,int & pfrom_arraylen, int & nto_arraylen, int & pto_arraylen,int &lkinfo_arraylen, int* pfrom1_list,int* pto1_list,char* nto_list,int* pathid_list, int* link_pos_list,char* lkinfo_list);//ok
	int followPath_devType_reverse(char* dType_from, eConnectivityCache *ecache, iConnectivityCache *icache, int elnkidx, HopTyp *hop, int *i, int dep,char* nfrom_list,char* pfrom_list,char* pto_list,int& len_array,int & nfrom_arraylen,int & pfrom_arraylen, int & nto_arraylen, int & pto_arraylen,int &lkinfo_arraylen, int* pfrom1_list,int* pto1_list,char* nto_list,int* pathid_list, int* link_pos_list,char* lkinfo_list);//ok
		//followPath_normal and followPath_reverse are used in GetAllPathsPerDevice
	int followPath_normal(eConnectivityCache *ecache, iConnectivityCache *icache, int elnkidx, HopTyp *hop, int *i, int dep,int& len_array, int* lkid_list5,int* pathid_list,int* link_pos_list);
	int followPath_reverse(eConnectivityCache *ecache, iConnectivityCache *icache, int elnkidx, HopTyp *hop, int *i, int dep,int& len_array, int* lkid_list5,int* pathid_list,int* link_pos_list);
	//int GetConnectivityBetweenDevices(char* dev_from, char* dev_to, eConnectivityCache *ecache, iConnectivityCache *icache, int system_name, int reload_cache, int *path_list, int *nfrom_list, int *nto_list, char* nfrom1_list, char* nto1_list, int *pfrom_list, int *pto_list, char* pfrom1_list, char* pto1_list);
	//int GetConnectivityBetweenDevices(char* dev_from, char* dev_to, eConnectivityCache *ecache, iConnectivityCache *icache, int system_name, int reload_cache, int& len_array, int &nfrom1_arraylen,int &nto1_arraylen,int &pfrom1_arraylen,int &pto1_arraylen,int &lkinfo_arraylen, int *path_list,int* link_pos_list, int *nfrom_list, int *nto_list, char *nfrom1_list, char *nto1_list, int *pfrom_list, int *pto_list, char *pfrom1_list, char *pto1_list, char* lkinfo_list);	
	int GetConnectivityBetweenDevices(char* dev_from, char* dev_to, eConnectivityCache *ecache, iConnectivityCache *icache, int& len_array, int &pfrom1_arraylen,int &nfrom1_arraylen,int &pto1_arraylen,int &nto1_arraylen,int &lkinfo_arraylen,char* nfrom1_list,char* nto1_list,char* pfrom1_list,int* pfrom_list,char* pto1_list,int* pto_list,int* pathid_list,int* link_pos_list,char* lkinfo_list,int reload_connectivity,int delete_connectivity,char* ErrMess);//ok
	int GetDetailedConnBetweenDeviceDevType(char* dev_from, char* dev_to,int devto_type_given,eConnectivityCache *ecache, iConnectivityCache *icache, char* nfrom_list,char* pfrom_list,char* pto_list,int& len_array,int & nfrom_arraylen,int & pfrom_arraylen, int & nto_arraylen, int & pto_arraylen,int &lkinfo_arraylen, int* pfrom1_list,int* pto1_list,char* nto_list,int* pathid_list, int* link_pos_list,char* lkinfo_list,int reload_connectivity,int delete_connectivity,char* ErrMess);//ok
	int GetAllPathsPerDevice(char* systemnameList,char* dev_name, eConnectivityCache *ecache, iConnectivityCache *icache, int& len_array, int* lkid_list5,int* pathid_list,int* link_pos_list,int reload_connectivity,int delete_connectivity,char* ErrMessage);
	
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
  char *selectstring;
	char *cntselect;

  ConnectivityCache (DBContainer *dbase);
  virtual void alloc_buffs(void)=0;
	//virtual void realloc_buffs(void)=0;
	virtual void print_row(int idx)=0;
	int get_col_wid(OCISvcCtx* ociHdbc, void * env, char *selectstr);
	int getnumrows(int system);
  int Fill(int system);
  int FindinCol(ColDesc_int *col, int val, int typ, HopTyp *res);
  int FindinCol(ColDesc_char *col, char *val, int typ, HopTyp *res);
  
  ///////////////////////////////
  char* cntselect_2;
 // int getnumrows(char* system_list);
 // int Fill(char* system_list);
  int FindinCol_LinkType(ColDesc_int *col, int val, ColDesc_int *lk, int lktype, int typ, HopTyp *res);
  int FindinCol_LinkType(ColDesc_char *col, char *val, ColDesc_int *lk, int lktype, int typ, HopTyp *res);
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
  ColDesc_int devtypefrom_id;
  ColDesc_char devtypefrom;
  ColDesc_int devtypeto_id;
  ColDesc_char devtypeto;

  eConnectivityCache (DBContainer *dbase) ;
  virtual void alloc_buffs(void);
  void print_row(int r);

  //////////////////////////////////////
  int Fill_list(char* system_list);
  int getnumrows_list(char* system_list);
  void print_row_devType(int r);
  void print_row_linkType(int r);
  void deleteConnectivity();
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
  virtual void alloc_buffs(void);
  void print_row(int r);
  /////////////////////
  int Fill_list(char* system_list);
  int getnumrows_list(char* system_list);
  void deleteConnectivity();
};

#endif
