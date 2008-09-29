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

////////////////////////////////////
class Device
{
public:
	char* name;
	char* serialnb;
	char* location;
	char* type;
	char* responsible;
	char* comments;
	char* hwtype;
	char* system;
	char* function;

	int name_null;
	int location_null;
	int serialnb_null;
	int type_null;
	int hwtype_null;
	int comments_null;
	int responsible_null;
	int function_null;

	int namelen;
	int serialnblen;
	int locationlen;
	int typelen;
	int responsiblelen;
	int commentslen;
	int hwtypelen;
	int systemlen;
	int functionlen;
	
	int sysID;
	int node;
	int prom_mode;

	Device(void);
	~Device(void);
	Device(char* systemnameList,char* devicename,char* devicetype,int dnode,int promiscuous_mode,char* serial_nb,char* dhwtype,char* dresponsible,char* dcomments,char* dlocation,char* function_list);
};
class Cpnt: public Device
{
public: 
	char* motherboard;
	int motherboard_null;
	int motherboardlen;

	int replacable;

	Cpnt(void);
	~Cpnt(void);
	Cpnt(char* cpntname,char* cpnttype,int dreplacable,char* motherboardname,char* serial_nb,char* dhwtype,char* dresponsible,char* dcomments,char* dlocation);
};
class DevType
{
public:
	char* type;
	char* rgbcol;
	char* descrip;

	int type_null;
	int rgbcol_null;
	int descrip_null;

	int typelen;
	int rgbcollen;
	int descriplen;

	int sysID;
	int nbin;
	int nbout;

	DevType(void);
	~DevType(void);
	DevType(char* systemnameList,char* devicetype,int nbrofinput,int nbrofoutput, char* description,char* rgbcolor);
};
class LinkType
{
public:
	char* ltype;
	char* lklist;
	int ltype_null;
	int ltypelen;
	int lklist_null;
	int lklistlen;

	LinkType(void);
	~LinkType(void);
	LinkType(char* name);
	LinkType(char* name, char* list);
};
class Link
{
public:
    char* nodefrom;
	char* nodeto;
	char* linktype;

	int bidirectional_lkused;
		
	int nodefrom_null;
	int nodeto_null;
	int linktype_null;

	int nodefromlen;
	int nodetolen;
	int linktypelen;

	Link(void);
	~Link(void);
};
class MacroLink: public Link
{
public:
	
	char* lkinfo;	
	char* portnbfrom;	
	char* portnbto;
	char* portypefrom;
	char* portypeto;
	
	int portnbfrom_null;	
	int portnbto_null;
	int portypefrom_null;
	int portypeto_null;	
	int lkinfo_null;
	
	int lkinfolen;
	int portnbfromlen;	
	int portnbtolen;
	int portypefromlen;
	int portypetolen;
	
	MacroLink();
	~MacroLink();
	MacroLink(char* node_from,char* node_to,char* port_nbfrom,char* port_nbto,char* port_typefrom,char* port_typeto,char* link_type,char* link_information,int bidirectional_link_used);
};
class MicroLink: public Link
{
public:
	int portnbfrom;	
	int portnbto;
	
	MicroLink();
	~MicroLink();
	MicroLink(char* node_from,char* node_to,int port_nbfrom,int port_nbto,char* link_type,int bidirectional_link_used);
};
class SparePort
{
public:
	char* serial;
	char* portype;
	char* dbia;
	char* mac;	
	char* portnb;

	int portway;
	
	int serial_null;
	int portype_null;
	int dbia_null;
	int mac_null;
	int portnb_null;

	int seriallen;
	int portypelen;
	int dbialen;
	int maclen;
	int portnblen;

	SparePort();
	~SparePort();
	SparePort(char* serialnb,char* port_nb,int port_way,char* port_type,char* bia,char* macadd);
};
class Port
{
public:
	char* devname;
	char* dphy;
	char* portype;
	char* dbia;
	char* mac;
	char* ip;
	char* dipname;
	char* dsubnet;
	char* portnb;

	int adminstatus;
	int portway;
	int pspeed;
	int pxibooting;

	int devname_null;
	int dphy_null;
	int portype_null;
	int dbia_null;
	int mac_null;
	int ip_null;
	int dipname_null;
	int dsubnet_null;
	int portnb_null;

	int devnamelen;
	int dphylen;
	int portypelen;
	int dbialen;
	int maclen;
	int iplen;
	int dipnamelen;
	int dsubnetlen;
	int portnblen;

	Port();
	~Port();
	Port(char* devicename,char* port_nb,int admin_status,int port_way,int speed,int pxi_booting,char* port_type,char* bia,char* ipadd,char* ipname,char* subnet,char* macadd,char* phy);
};
class IPAlias
{
public:
	char* ip;
	char* dipname;
	char* dipalias;
	
	int ip_null;
	int dipname_null;
	int dipalias_null;
	
	int iplen;
	int dipnamelen;
	int dipaliaslen;
	
	IPAlias();
	~IPAlias();
	IPAlias(char* ipaddress,char* ipname,char* ipalias);
};
class UpdtPort
{
public:
	char* devname;
	char* dphy;
	char* portype;
	char* portnb;

	int portway;
	int pspeed;
	int pxibooting;
	int adminstatus;

	int devname_null;
	int dphy_null;
	int portype_null;
	int portnb_null;

	int devnamelen;
	int dphylen;
	int portypelen;
	int portnblen;

	UpdtPort();
	~UpdtPort();
	UpdtPort(char* devicename,char* port_nb,int port_way,char* port_type,int speed,char* phy,int pxi_booting);
	UpdtPort(char* devicename,char* port_nb,int port_way,char* port_type,int admin_status);
};
class UpdtLink: public Link
{
public:
	char* portnbfrom;	
	char* portnbto;
	char* portypefrom;
	char* portypeto;
	char* lkinfo;

	int linkused;
	
	int portnbfrom_null;	
	int portnbto_null;
	int portypefrom_null;
	int portypeto_null;	
	int lkinfo_null;
		
	int portnbfromlen;	
	int portnbtolen;
	int portypefromlen;
	int portypetolen;
	int lkinfolen;
	
	UpdtLink();
	~UpdtLink();
	
	UpdtLink(char* node_from,char* node_to,char* port_nbrfrom,char* port_nbrto,char* port_typefrom,char* port_typeto,int lkused,char* link_information, char* link_type, int bidirectional_link_used);
};
class UpdtDevice
{
	public:
	char* name;
	char* nameold;			
	char* dlocation;
	char* function;

	int name_null;
	int nameold_null;
	int location_null;
	int function_null;

	int namelen;
	int nameoldlen;
	int locationlen;
	int functionlen;
	
	int sysID;
	int dnode;
	int prom_mode;
	int nused;
	int act;

	UpdtDevice(void);
	~UpdtDevice(void);
	UpdtDevice(char* devicename,int node,int promiscuous_mode,char* location,char* function_list);
	UpdtDevice(char* devicename_old,char* devicename);
	UpdtDevice(char* devicename,int nodeused,int active=NULL);
};
class UpdtDevType: public DevType
{
public:
	char* typeold;
	int typeold_null;
	int typeoldlen;
	
	UpdtDevType(void);
	~UpdtDevType(void);
	UpdtDevType(char* devicetypeold,char* devicetype);
};
class UpdtLinkType: public LinkType
{
public:
	char* typeold;
	int typeold_null;
	int typeoldlen;

	UpdtLinkType(void);
	~UpdtLinkType(void);
	UpdtLinkType(char *old_link_name, char *link_name);
};
class macIP
{
public:
	char* ipadd;
	char* iname;
	char* submask;
			
	int ipadd_null;
	int iname_null;
	int submask_null;
	
	int ipaddlen;
	int inamelen;
	int submasklen;
	
	macIP(void);
	~macIP(void);
	macIP(char* ip_add,char* subnet_mask,char* ipname);
};
class UpdtmacIP: public macIP
{
public:
	char* ipaddold;
	int ipaddold_null;
	int ipaddoldlen;

	UpdtmacIP(void);
	~UpdtmacIP(void);
	UpdtmacIP(char* ip_add,char* oldip_add);
};
#endif
