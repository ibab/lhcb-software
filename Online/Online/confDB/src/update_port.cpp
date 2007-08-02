/********************************************************************************/
//	Author: L.Abadie
// version:v3.5
// changes: handling error such that can be extended to C++: (create a class SQLException) (strlen ck + update stmt)
// file which performs bulk collect insert for the port table
// N.B : Memory management (allocation+release) should be handled by the user application thus, for all the fct provided by this lib
/********************************************************************************/

#include "list_structures.h"
#include "system_info.h"
#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

#if defined( _WIN32) || defined(WIN32)
#define EXTERN_CONFDB _declspec(dllexport)
#else
#define EXTERN_CONFDB extern
#endif

	extern   OCIError* ociError ;
	extern   OCIEnv* ociEnv ;
	extern   OCISvcCtx* ociHdbc ;
	extern   OCIServer *mysrvhp;
	extern   OCISession *myusrhp; 



	/***********************************GV for port***************************************************/
	int FIRST_TIME_PORTUPDATE=0;
	int FIRST_TIME_PORTUPDATESTATUS=0;
	char* _devicenameList1Update;
	int _devicenameNbElement1Update;
	int _max_devicename_len1Update;
	int _devicenameList_len1Update;

	int* _devicename2_nullvalueUpdate;
	int _devicename2NbElUpdate;

	char* _devicenameList5Update;
	int _devicenameNbElement5Update;
	int _max_devicename_len5Update;
	int _devicenameList_len5Update;

	int* _devicename5_nullvalueUpdate;
	int _devicename5NbElUpdate;

	char* _ptnbList1Update;
	int _ptnbNbElement1Update;
	int _max_ptnb_len1Update;
	int _ptnbList_len1Update;

	int* _ptnb2_nullvalueUpdate;
	int _ptnb2NbElUpdate;

	char* _ptnbList5Update;
	int _ptnbNbElement5Update;
	int _max_ptnb_len5Update;
	int _ptnbList_len5Update;

	int* _ptnb5_nullvalueUpdate;
	int _ptnb5NbElUpdate;


	char* _ptypeList1Update;
	int _ptypeNbElement1Update;
	int _max_ptype_len1Update;
	int _ptypeList_len1Update;

	int* _ptype2_nullvalueUpdate;
	int _ptype2NbElUpdate;

	char* _ptypeList5Update;
	int _ptypeNbElement5Update;
	int _max_ptype_len5Update;
	int _ptypeList_len5Update;

	int* _ptype5_nullvalueUpdate;
	int _ptype5NbElUpdate;



	int* _speedListUpdate;
	int _speedNbElementUpdate;





	char* _phyListUpdate;
	int _phyNbElementUpdate;
	int _max_phy_lenUpdate;
	int _phyList_lenUpdate;

	int* _phy_nullvalueUpdate;
	int _phyNbElUpdate;

	int* _pxi_bootingListUpdate;
	int _pxi_bootingNbElementUpdate;


	int* _adminstatusListUpdate;
	int  _adminstatusNbElementUpdate;

	int* _portwayListUpdate;
	int  _portwayNbElementUpdate;

	int* _portway1ListUpdate;
	int  _portway1NbElementUpdate;


	int freePortUpdate()
	{
		int status=0;
		_devicenameList1Update=(char*)realloc(_devicenameList1Update,0*sizeof(char));
		_devicenameList1Update=NULL;

		_ptnbList1Update=(char*)realloc(_ptnbList1Update,0*sizeof(char));
		_ptnbList1Update=NULL;

		_ptypeList1Update=(char*)realloc(_ptypeList1Update,0*sizeof(char));
		_ptypeList1Update=NULL;

		_speedListUpdate=(int*)realloc(_speedListUpdate,0*sizeof(int));
		_speedListUpdate=NULL;

		_phyListUpdate=(char*)realloc(_phyListUpdate,0*sizeof(char));
		_phyListUpdate=NULL;

		_pxi_bootingListUpdate=(int*)realloc(_pxi_bootingListUpdate,0*sizeof(int));
		_pxi_bootingListUpdate=NULL;

		_phy_nullvalueUpdate=(int*)realloc(_phy_nullvalueUpdate,0*sizeof(int));
		_phy_nullvalueUpdate=NULL;

		_devicename2_nullvalueUpdate=(int*)realloc(_devicename2_nullvalueUpdate,0*sizeof(int));
		_devicename2_nullvalueUpdate=NULL;

		_ptnb2_nullvalueUpdate=(int*)realloc(_ptnb2_nullvalueUpdate,0*sizeof(int));
		_ptnb2_nullvalueUpdate=NULL;
		_ptype2_nullvalueUpdate=(int*)realloc(_ptype2_nullvalueUpdate,0*sizeof(int));
		_ptype2_nullvalueUpdate=NULL;
		_portwayListUpdate=(int*)realloc(_portwayListUpdate,0*sizeof(int));
		_portwayListUpdate=NULL;
		FIRST_TIME_PORTUPDATE=0;

		return status;
	}

	int freePortUpdateStatus()
	{
		int status=0;
		_devicenameList5Update=(char*)realloc(_devicenameList5Update,0*sizeof(char));
		_devicenameList5Update=NULL;

		_ptnbList5Update=(char*)realloc(_ptnbList5Update,0*sizeof(char));
		_ptnbList5Update=NULL;

		_ptypeList5Update=(char*)realloc(_ptypeList5Update,0*sizeof(char));
		_ptypeList5Update=NULL;


		_portway1ListUpdate=(int*)realloc(_portway1ListUpdate,0*sizeof(int));
		_portway1ListUpdate=NULL;


		_adminstatusListUpdate=(int*)realloc(_adminstatusListUpdate,0*sizeof(int));
		_adminstatusListUpdate=NULL;




		_devicename5_nullvalueUpdate=(int*)realloc(_devicename5_nullvalueUpdate,0*sizeof(int));
		_devicename5_nullvalueUpdate=NULL;

		_ptnb5_nullvalueUpdate=(int*)realloc(_ptnb5_nullvalueUpdate,0*sizeof(int));
		_ptnb5_nullvalueUpdate=NULL;
		_ptype5_nullvalueUpdate=(int*)realloc(_ptype5_nullvalueUpdate,0*sizeof(int));
		_ptype5_nullvalueUpdate=NULL;
		FIRST_TIME_PORTUPDATESTATUS=0;

		return status;
	}
	/**
	* Upadte Multiple Ports, returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* @param devicename : name of the device which will be updated
	* @param port_nb   : port nb of the device to update
	* @param port_way   : specify the way of the port to update :1 if the port is an input and 2 if it's an output 
	* @param port_type   : specify the type of the port of the device to update. Put "" if there is none.
	* @param speed   : new value of speed of the port: PUT -1 to keep the same value
	* @param pxi_booting : new value of pxi_booting 1 if the port is the pxi_booting one, 0 otherwise. Put a value different from 1 or 0 to keep the same value.
	* @param phy: new value of phy possible values :"SX","T","SL". if no changes put "none"
	* @param first_time1 : 1 if it's your first Port insert (save it into the database), 0 otherwise   
	* @param last_rows1 : 1 if it's your last Port insert (save it into the database), 0 otherwise
	* @param ErrMess : error message in case of failure
	* @return 0 if it is successful
	*/ 
	EXTERN_CONFDB
		int UpdateMultiplePorts(char* devicename,char* port_nb,int port_way,char* port_type,int speed,char* phy,int pxi_booting,int first_time1,int last_rows1,char* ErrMess)
	{
		char appliName[100]="UpdateMultiplePorts";
		char sqlstmt[1000];
		OCIStmt *hstmt;
		OCIBind  *bndp[7]; 
		int res_query=0;
		sword status=0;
		char* devname=NULL;
		char* dphy=NULL;
		char* dptnb=NULL;
		char* dptype=NULL;
		int rescode=0;
		int i=0;
		int* numrows_inserted=NULL;

		int numrows=0;
		OCIBind  *bndp3 = (OCIBind *) 0;
		int first_time=first_time1;
		int force_insert=0;
		int last_rows=last_rows1;
		char port_type1[100];
		if(port_type!=NULL)
		{
			if(strlen(port_type)==0)
				strcpy(port_type1,"none");
			else
				strcpy(port_type1,port_type);
		}
		else
		{
			strcpy(port_type1,"none");
		}

		int devname1=null_charptr(devicename);
		int dphy1=null_charptr(phy);
		int dptnb1=null_charptr(port_nb);
		int dptype1=null_charptr(port_type1);
		if(FIRST_TIME_PORTUPDATE==1 && _devicenameList1Update==NULL)
			first_time=1;

		res_query=AppendString(devicename,_devicenameList1Update,_devicenameList_len1Update,_devicenameNbElement1Update, _max_devicename_len1Update,first_time);
		res_query+=AppendString(phy,_phyListUpdate,_phyList_lenUpdate,_phyNbElementUpdate,_max_phy_lenUpdate,first_time);
		res_query=AppendString(port_nb,_ptnbList1Update,_ptnbList_len1Update,_ptnbNbElement1Update, _max_ptnb_len1Update,first_time);
		res_query=AppendString(port_type1,_ptypeList1Update,_ptypeList_len1Update,_ptypeNbElement1Update, _max_ptype_len1Update,first_time);

		res_query+=AppendInt(speed,_speedListUpdate,_speedNbElementUpdate,first_time);
		res_query+=AppendInt(pxi_booting,_pxi_bootingListUpdate,_pxi_bootingNbElementUpdate,first_time);
		res_query+=AppendInt(port_way,_portwayListUpdate,_portwayNbElementUpdate,first_time);

		res_query+=AppendInt(devname1,_devicename2_nullvalueUpdate,_devicename2NbElUpdate,first_time);
		res_query+=AppendInt(dphy1,_phy_nullvalueUpdate,_phyNbElUpdate,first_time);
		res_query+=AppendInt(dptnb1,_ptnb2_nullvalueUpdate,_ptnb2NbElUpdate,first_time);
		res_query+=AppendInt(dptype1,_ptype2_nullvalueUpdate,_ptype2NbElUpdate,first_time);

		if(first_time==1)
			FIRST_TIME_PORTUPDATE=1;
		status=res_query;
		if(last_rows!=1 && _devicenameNbElement1Update==MAXROWS)
		{
			force_insert=1;
			last_rows=1;
		}
		if(last_rows==1 && res_query==0 && ociEnv!=0 && FIRST_TIME_PORTUPDATE==1)
		{
			int len_host=LOGIN_LEN;
			char login[LOGIN_LEN];
			char host[LOGIN_LEN];
			char* errmessg1=(char*)malloc(ERROR_BUFFER*sizeof(char));

			if(errmessg1==NULL)
			{
				GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
				return -1;
			}
			char* errmessg2=(char*)malloc(ERROR_BUFFER*sizeof(char));
			if(errmessg2==NULL)
			{
				free(errmessg1);
				GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
				return -1;
			}
			res_query=getTerminalName(host,len_host,errmessg1);
			len_host=LOGIN_LEN;
			res_query=getLoginUser(login,len_host,errmessg2);
			if(strncmp("NO_ERROR",errmessg1,8)==0 && strncmp("NO_ERROR",errmessg2,8)==0 )
			{
				free(errmessg1);
				free(errmessg2);
				rescode=0;
			}
			else
			{
				status=-1;
				rescode=-1;
				if(strncmp("NO_ERROR",errmessg1,8)!=0)
				{
					strcpy(ErrMess,errmessg1);
					rescode=-2;
				}
				if(strncmp("NO_ERROR",errmessg2,8)!=0)
				{
					if(rescode==-2)
						sprintf(ErrMess,"%s , %s",errmessg1,errmessg2);
					else
						strcpy(ErrMess,errmessg2);
				}
				rescode=-1;
				free(errmessg1);
				free(errmessg2);
				status=freePortUpdate();
				if(devname!=NULL)
					free(devname);
				if(dptnb!=NULL)
					free(dptnb);
				if(dphy!=NULL)
					free(dphy);
				if(dptype!=NULL)
					free(dptype);
				return -1;
			}
			//need to proceed with messages

			status =OCIHandleAlloc (ociEnv, (void**)&hstmt, OCI_HTYPE_STMT , 0, 0);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIHandleAlloc unsuccessful");
			}
			else
				sprintf(sqlstmt,"BEGIN update %s set speed=decode(nvl(:speed,-1),-1,speed,:speed),phy=decode(nvl(:phy,'none'),'none',phy,:phy),pxi_booting=decode(:pxi_booting,1,1,0,0,pxi_booting),last_update=sysdate,user_update='%s' where deviceid=(select deviceid from %s where devicename=:dname) and port_nbr=:port_nbr and port_way=:port_way and nvl(port_type,'none')=:port_type; :numrows:=%s; END;",PORT_TABLE,login,LOGICAL_DEVICE_TABLE,SQLROWCOUNT);
			if (OCIStmtPrepare(hstmt, ociError, (text*)sqlstmt, (ub4)strlen((char *)sqlstmt), (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT))
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");

			}
			else
			{

				devname=(char*)malloc(_devicenameNbElement1Update*_max_devicename_len1Update*sizeof(char));
				if(devname!=NULL)
					status+=NormalizeVector(_devicenameList1Update,_devicenameNbElement1Update, _max_devicename_len1Update,devname);
				dphy=(char*)malloc(_phyNbElementUpdate*_max_phy_lenUpdate*sizeof(char));
				if(dphy!=NULL)
					status+=NormalizeVector(_phyListUpdate, _phyNbElementUpdate,_max_phy_lenUpdate,dphy);
				dptnb=(char*)malloc(_ptnbNbElement1Update*_max_ptnb_len1Update*sizeof(char));
				if(dptnb!=NULL)
					status+=NormalizeVector(_ptnbList1Update,_ptnbNbElement1Update,_max_ptnb_len1Update,dptnb);
				dptype=(char*)malloc(_ptypeNbElement1Update*_max_ptype_len1Update*sizeof(char));
				if(dptype!=NULL)
					status+=NormalizeVector(_ptypeList1Update,_ptypeNbElement1Update,_max_ptype_len1Update,dptype);
				numrows_inserted=(int*)malloc(sizeof(int)*_ptnbNbElement1Update);

				if(devname==NULL||dphy==NULL||dptype==NULL||dptnb==NULL||numrows_inserted==NULL)
				{
					status=-10;
					rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");
					status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
					GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
					status+=freePortUpdate();
					if(devname!=NULL)
						free(devname);
					if(dptnb!=NULL)
						free(dptnb);
					if(dphy!=NULL)
						free(dphy);
					if(dptype!=NULL)
						free(dptype);
					if(numrows_inserted!=NULL)
						free(numrows_inserted);
					return -1;
				}
				else
				{
					for(i=0;i<_devicenameNbElement1Update;i++)
						numrows_inserted[i]=0;
					status=OCIBindByName(hstmt, &bndp[0], ociError,(text*) ":port_nbr", -1,(dvoid*)dptnb, _max_ptnb_len1Update,  SQLT_STR, (dvoid *) &_ptnb2_nullvalueUpdate[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
				}

			}

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName 2 unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp[1], ociError,(text*) ":speed", -1,(dvoid*)&_speedListUpdate[0], sizeof(speed),  SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName 5 unsuccessful");

			}
			else
				status =OCIBindByName(hstmt, &bndp[2], ociError,(text*) ":port_way", -1,(dvoid*)&_portwayListUpdate[0], sizeof(port_way),  SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName 6 unsuccessful");

			}
			else
				status =OCIBindByName(hstmt, &bndp[3], ociError,(text*) ":port_type", -1,(dvoid*)dptype, _max_ptype_len1Update,  SQLT_STR, (dvoid *)&_ptype2_nullvalueUpdate[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName 7 unsuccessful");

			}
			else
				status =OCIBindByName(hstmt, &bndp[4], ociError,(text*) ":phy", -1,(dvoid*)dphy, _max_phy_lenUpdate,  SQLT_STR, (dvoid *) &_phy_nullvalueUpdate[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName  9 unsuccessful");


			}
			else
				status =OCIBindByName(hstmt, &bndp[5], ociError,(text*) ":pxi_booting", -1,(dvoid*)&_pxi_bootingListUpdate[0], sizeof(pxi_booting),  SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName 12  unsuccessful");

			}
			else
				status =OCIBindByName(hstmt, &bndp[6], ociError,(text*) ":dname", -1,(dvoid*)devname,  _max_devicename_len1Update,  SQLT_STR, (dvoid *) &_devicename2_nullvalueUpdate[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName 13 unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp3, ociError,(text*) ":numrows", -1,(dvoid*)&numrows_inserted[0], sizeof(numrows),SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName numrows unsuccessful");

			}
			else
				status=OCIBindArrayOfStruct(bndp[0], ociError,_max_ptnb_len1Update,sizeof(int), 0, 0);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");

			}
			else
				status=OCIBindArrayOfStruct(bndp[1], ociError,  sizeof(int), 0, 0, 0);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");

			}
			else
				status=OCIBindArrayOfStruct(bndp[2], ociError, sizeof(int), 0, 0, 0);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");	
			}
			else
				status=OCIBindArrayOfStruct(bndp[3], ociError, _max_ptype_len1Update, sizeof(int),  0, 0);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp[4], ociError, _max_phy_lenUpdate, sizeof(int), 0, 0);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp[5], ociError,  sizeof(int), 0, 0, 0);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp[6], ociError, _max_devicename_len1Update,  sizeof(int),  0, 0);


			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp3, ociError, sizeof(int),0, 0, 0);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status= OCIStmtExecute(ociHdbc, hstmt, ociError, _devicenameNbElement1Update, 0, 0, 0, OCI_DEFAULT );


			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
			}
			else
			{
				numrows=-1;
				for(i=0;i< _devicenameNbElement1Update;i++)
				{
					if(numrows_inserted[i]==0)
					{
						numrows=0;
						i=  _devicenameNbElement1Update+5;
					}
					else
						numrows=numrows_inserted[i];
				}
				if(numrows==0)
				{
					status = OCITransCommit(ociHdbc, ociError, 0);
					status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
					status=freePortUpdate();
					if(devname!=NULL)
						free(devname);
					if(dptnb!=NULL)
						free(dptnb);
					if(dphy!=NULL)
						free(dphy);
					if(dptype!=NULL)
						free(dptype);

					free(numrows_inserted);
					GetErrorMess(appliName,  "COULDNOT_UPDATE_ALL_ROWS",ErrMess,1);
					return -1;
				}
				else
					status = OCITransCommit(ociHdbc, ociError, 0);
				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)	
						rescode=ShowErrors (status, ociError, "OCITransCommit unsuccessful");

				}	
			}

			if(devname!=NULL)
				free(devname);
			if(dptnb!=NULL)
				free(dptnb);
			if(dphy!=NULL)
				free(dphy);
			if(dptype!=NULL)
				free(dptype);
			if(numrows_inserted!=NULL)
				free(numrows_inserted);

			status=freePortUpdate();
			status+=OCIHandleFree (hstmt,OCI_HTYPE_STMT);

			if(rescode!=0)
				if(ociError!=0)
					OCIReportError(ociError,appliName, ErrMess,1); 
				else
					GetErrorMess(appliName,  "NOT CONNECTED TO ANY DB",ErrMess,1); 
			else
				OCIReportError(ociError,appliName, ErrMess,0); 
			if(force_insert==1 && rescode==0)
				FIRST_TIME_PORTUPDATE=1;
		}
		else
		{
			if (res_query!=0)
			{
				status=freePortUpdate();
				rescode=res_query;
				GetErrorMess(appliName, "Cache Problem",ErrMess,1);
			}
			if (FIRST_TIME_PORTUPDATE!=1)
			{
				status=freePortUpdate();
				rescode=-1;
				GetErrorMess(appliName, "CACHE HAS NOT BEEN INITIALIZED",ErrMess,1);
			}
			if(ociEnv==0)
			{
				status=freePortUpdate();
				rescode=-1;
				GetErrorMess(appliName, "NOT CONNECTED TO ANY DB",ErrMess,1);
			}	
			if(ociEnv!=0 && FIRST_TIME_PORTUPDATE==1 && res_query==0)
			{
				status=0;
				GetErrorMess(appliName, "NO_ERROR",ErrMess,0);
			}

		}
		return (rescode+status);
	}

	/**
	* Upadte Multiple Ports status, returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* @param devicename : name of the device which will be updated
	* @param port_nb   : port nb of the device to update
	* @param port_way   : specify the way of the port to update :1 if the port is an input and 2 if it's an output 
	* @param port_type   : specify the type of the port of the device to update. Put "" if there is none.
	* @param admin_status   : new value of speed of the port: PUT -1 to keep the same value
	* @param first_time1 : 1 if it's your first Port insert (save it into the database), 0 otherwise   
	* @param last_rows1 : 1 if it's your last Port insert (save it into the database), 0 otherwise
	* @param ErrMess : error message in case of failure
	* @return 0 if it is successful
	*/ 
	EXTERN_CONFDB
		int UpdateMultiplePortStatuses(char* devicename,char* port_nb,int port_way,char* port_type,int admin_status,int first_time1,int last_rows1,char* ErrMess)
	{
		char appliName[100]="UpdateMultiplePortStatuses";
		char sqlstmt[1000];
		OCIStmt *hstmt;
		OCIBind  *bndp[5]; 
		int res_query=0;
		sword status=0;
		char* devname=NULL;
		char* dptnb=NULL;
		char* dptype=NULL;
		int rescode=0;
		int i=0;
		int* numrows_inserted=NULL;
		int* portid_list=NULL;
		int numrows=0;
		OCIBind  *bndp4 = (OCIBind *) 0;
		OCIBind  *bndp3 = (OCIBind *) 0;
		int first_time=first_time1;
		int force_insert=0;
		int last_rows=last_rows1;
		char port_type1[100];
		if(port_type!=NULL)
		{
			if(strlen(port_type)==0)
				strcpy(port_type1,"none");
			else
				strcpy(port_type1,port_type);
		}
		else
		{
			strcpy(port_type1,"none");
		}

		int devname1=null_charptr(devicename);
		int dptnb1=null_charptr(port_nb);
		int dptype1=null_charptr(port_type1);
		if(FIRST_TIME_PORTUPDATESTATUS==1 && _devicenameList5Update==NULL)
			first_time=1;

		res_query=AppendString(devicename,_devicenameList5Update,_devicenameList_len5Update,_devicenameNbElement5Update, _max_devicename_len5Update,first_time);
		res_query=AppendString(port_nb,_ptnbList5Update,_ptnbList_len5Update,_ptnbNbElement5Update, _max_ptnb_len5Update,first_time);
		res_query=AppendString(port_type1,_ptypeList5Update,_ptypeList_len5Update,_ptypeNbElement5Update, _max_ptype_len5Update,first_time);

		res_query+=AppendInt(admin_status,_adminstatusListUpdate,_adminstatusNbElementUpdate,first_time);
		res_query+=AppendInt(port_way,_portway1ListUpdate,_portway1NbElementUpdate,first_time);

		res_query+=AppendInt(devname1,_devicename5_nullvalueUpdate,_devicename5NbElUpdate,first_time);
		res_query+=AppendInt(dptnb1,_ptnb5_nullvalueUpdate,_ptnb5NbElUpdate,first_time);
		res_query+=AppendInt(dptype1,_ptype5_nullvalueUpdate,_ptype5NbElUpdate,first_time);

		if(first_time==1)
			FIRST_TIME_PORTUPDATESTATUS=1;
		status=res_query;
		if(last_rows!=1 && _devicenameNbElement5Update==MAXROWS)
		{
			force_insert=1;
			last_rows=1;
		}
		if(last_rows==1 && res_query==0 && ociEnv!=0 && FIRST_TIME_PORTUPDATESTATUS==1)
		{
			int len_host=LOGIN_LEN;
			char login[LOGIN_LEN];
			char host[LOGIN_LEN];
			char* errmessg1=(char*)malloc(ERROR_BUFFER*sizeof(char));

			if(errmessg1==NULL)
			{
				GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
				return -1;
			}
			char* errmessg2=(char*)malloc(ERROR_BUFFER*sizeof(char));
			if(errmessg2==NULL)
			{
				free(errmessg1);
				GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
				return -1;
			}
			res_query=getTerminalName(host,len_host,errmessg1);
			len_host=LOGIN_LEN;
			res_query=getLoginUser(login,len_host,errmessg2);
			if(strncmp("NO_ERROR",errmessg1,8)==0 && strncmp("NO_ERROR",errmessg2,8)==0 )
			{
				free(errmessg1);
				free(errmessg2);
				rescode=0;
			}
			else
			{
				status=-1;
				rescode=-1;
				if(strncmp("NO_ERROR",errmessg1,8)!=0)
				{
					strcpy(ErrMess,errmessg1);
					rescode=-2;
				}
				if(strncmp("NO_ERROR",errmessg2,8)!=0)
				{
					if(rescode==-2)
						sprintf(ErrMess,"%s , %s",errmessg1,errmessg2);
					else
						strcpy(ErrMess,errmessg2);
				}
				rescode=-1;
				free(errmessg1);
				free(errmessg2);
				status=freePortUpdateStatus();
				if(devname!=NULL)
					free(devname);
				if(dptnb!=NULL)
					free(dptnb);

				if(dptype!=NULL)
					free(dptype);
				return -1;
			}
			//need to proceed with messages

			status =OCIHandleAlloc (ociEnv, (void**)&hstmt, OCI_HTYPE_STMT , 0, 0);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIHandleAlloc unsuccessful");
			}
			else
				sprintf(sqlstmt,"BEGIN update %s set administrative_status=decode(:admin_status,1,1,0,0,administrative_status),last_update=sysdate,user_update='%s' where deviceid=(select deviceid from %s where devicename=:dname) and port_nbr=:port_nbr and port_way=:port_way and nvl(port_type,'none')=:port_type returning portid into :portid; update %s t set t.last_update=sysdate,t.user_update='%s',t.lkused=(select decode(t.lkused,1,p.administrative_status*g.administrative_status,t.lkused) from %s p,%s g where  t.portidfrom=p.portid and t.portidto=g.portid and (g.portid=:portid or p.portid=:portid)) where t.portidto=:portid or t.portidfrom=:portid;:numrows:=%s; END;",PORT_TABLE,login,LOGICAL_DEVICE_TABLE,MACRO_CONNECTIVITY_TABLE,login,PORT_TABLE,PORT_TABLE,SQLROWCOUNT);
			if (OCIStmtPrepare(hstmt, ociError, (text*)sqlstmt, (ub4)strlen((char *)sqlstmt), (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT))
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");

			}
			else
			{

				devname=(char*)malloc(_devicenameNbElement5Update*_max_devicename_len5Update*sizeof(char));
				if(devname!=NULL)
					status+=NormalizeVector(_devicenameList5Update,_devicenameNbElement5Update, _max_devicename_len5Update,devname);
				dptnb=(char*)malloc(_ptnbNbElement5Update*_max_ptnb_len5Update*sizeof(char));
				if(dptnb!=NULL)
					status+=NormalizeVector(_ptnbList5Update,_ptnbNbElement5Update,_max_ptnb_len5Update,dptnb);
				dptype=(char*)malloc(_ptypeNbElement5Update*_max_ptype_len5Update*sizeof(char));
				if(dptype!=NULL)
					status+=NormalizeVector(_ptypeList5Update,_ptypeNbElement5Update,_max_ptype_len5Update,dptype);
				numrows_inserted=(int*)malloc(sizeof(int)*_ptnbNbElement5Update);
				portid_list=(int*)malloc(sizeof(int)*_ptnbNbElement5Update);
				if(devname==NULL||dptype==NULL||dptnb==NULL||numrows_inserted==NULL||portid_list==NULL)
				{
					status=-10;
					rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");
					status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
					GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
					status+=freePortUpdateStatus();
					if(devname!=NULL)
						free(devname);
					if(dptnb!=NULL)
						free(dptnb);

					if(dptype!=NULL)
						free(dptype);
					if(numrows_inserted!=NULL)
						free(numrows_inserted);
					if(portid_list!=NULL)
						free(portid_list);
					return -1;
				}
				else
				{
					for(i=0;i<_devicenameNbElement5Update;i++)
					{
						numrows_inserted[i]=0;
						portid_list[i]=0;
					}
					status=OCIBindByName(hstmt, &bndp[0], ociError,(text*) ":port_nbr", -1,(dvoid*)dptnb, _max_ptnb_len5Update,  SQLT_STR, (dvoid *) &_ptnb5_nullvalueUpdate[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
				}

			}

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName 2 unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp[1], ociError,(text*) ":admin_status", -1,(dvoid*)&_adminstatusListUpdate[0], sizeof(admin_status),  SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName 5 unsuccessful");

			}
			else
				status =OCIBindByName(hstmt, &bndp[2], ociError,(text*) ":port_way", -1,(dvoid*)&_portway1ListUpdate[0], sizeof(port_way),  SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName 6 unsuccessful");

			}
			else
				status =OCIBindByName(hstmt, &bndp[3], ociError,(text*) ":port_type", -1,(dvoid*)dptype, _max_ptype_len5Update,  SQLT_STR, (dvoid *)&_ptype5_nullvalueUpdate[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName 12  unsuccessful");

			}
			else
				status =OCIBindByName(hstmt, &bndp[4], ociError,(text*) ":dname", -1,(dvoid*)devname,  _max_devicename_len5Update,  SQLT_STR, (dvoid *) &_devicename5_nullvalueUpdate[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName 13 unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp3, ociError,(text*) ":numrows", -1,(dvoid*)&numrows_inserted[0], sizeof(numrows),SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName 13 unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp4, ociError,(text*) ":portid", -1,(dvoid*)&portid_list[0], sizeof(numrows),SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName numrows unsuccessful");

			}
			else
				status=OCIBindArrayOfStruct(bndp[0], ociError,_max_ptnb_len5Update,sizeof(int), 0, 0);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");

			}
			else
				status=OCIBindArrayOfStruct(bndp[1], ociError,  sizeof(int), 0, 0, 0);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");

			}
			else
				status=OCIBindArrayOfStruct(bndp[2], ociError, sizeof(int), 0, 0, 0);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");	
			}
			else
				status=OCIBindArrayOfStruct(bndp[3], ociError, _max_ptype_len1Update, sizeof(int),  0, 0);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp[4], ociError, _max_devicename_len1Update,  sizeof(int),  0, 0);


			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp3, ociError, sizeof(int),0, 0, 0);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp4, ociError, sizeof(int),0, 0, 0);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status= OCIStmtExecute(ociHdbc, hstmt, ociError, _devicenameNbElement5Update, 0, 0, 0, OCI_DEFAULT );

			//std::cout<<"value of portid_list[0]="<<portid_list[0]<<std::endl;
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
			}
			else
			{
				numrows=-1;
				for(i=0;i< _devicenameNbElement5Update;i++)
				{
					if(numrows_inserted[i]==0)
					{
						numrows=0;
						i=  _devicenameNbElement5Update+5;
					}
					else
						numrows=numrows_inserted[i];
				}
				if(numrows==0)
				{
					status = OCITransCommit(ociHdbc, ociError, 0);
					status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
					status=freePortUpdateStatus();
					if(devname!=NULL)
						free(devname);
					if(dptnb!=NULL)
						free(dptnb);

					if(dptype!=NULL)
						free(dptype);

					free(numrows_inserted);
					GetErrorMess(appliName,  "COULDNOT_UPDATE_ALL_ROWS",ErrMess,1);
					return -1;
				}
				else
				{
					status = OCITransCommit(ociHdbc, ociError, 0);
					int res_query1;
					sprintf(sqlstmt,"BEGIN :rescode:=%s(1); :numrows:=%s; END; ",_UpdatePathUsed,SQLROWCOUNT);
					status=OCIStmtPrepare(hstmt, ociError, (text*)sqlstmt, (ub4)strlen((char *)sqlstmt), (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
					//std::cout<<"value of sqlstmt 2 "<<sqlstmt<<std::endl;			
					if(status!=OCI_SUCCESS)
					{
						if(rescode==0)	
							rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
					}
					else
						status =OCIBindByName(hstmt, &bndp3, ociError,(text*) ":numrows", -1,(dvoid*)&numrows, sizeof(int),SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);

					if(status!=OCI_SUCCESS)
					{
						if(rescode==0)	
							rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
					}
					else
						status =OCIBindByName(hstmt, &bndp[0], ociError,(text*) ":rescode", -1,(dvoid*)&res_query1, sizeof(int),SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);

					if(status!=OCI_SUCCESS)
					{
						if(rescode==0)	
							rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
					}
					else
						status= OCIStmtExecute(ociHdbc, hstmt, ociError, 1, 0, 0, 0, OCI_DEFAULT );
					if(status!=OCI_SUCCESS)
					{
						if(rescode==0)	
							rescode=ShowErrors (status, ociError, "OCIExecute unsuccessful");
					}
					else
						status = OCITransCommit(ociHdbc, ociError, 0);

				}
				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)	
						rescode=ShowErrors (status, ociError, "OCITransCommit unsuccessful");

				}	
			}

			if(devname!=NULL)
				free(devname);
			if(dptnb!=NULL)
				free(dptnb);

			if(dptype!=NULL)
				free(dptype);
			if(numrows_inserted!=NULL)
				free(numrows_inserted);

			status=freePortUpdateStatus();
			status+=OCIHandleFree (hstmt,OCI_HTYPE_STMT);

			if(rescode!=0)
				if(ociError!=0)
					OCIReportError(ociError,appliName, ErrMess,1); 
				else
					GetErrorMess(appliName,  "NOT CONNECTED TO ANY DB",ErrMess,1); 
			else
				OCIReportError(ociError,appliName, ErrMess,0); 
			if(force_insert==1 && rescode==0)
				FIRST_TIME_PORTUPDATESTATUS=1;
		}
		else
		{
			if (res_query!=0)
			{
				status=freePortUpdate();
				rescode=res_query;
				GetErrorMess(appliName, "Cache Problem",ErrMess,1);
			}
			if (FIRST_TIME_PORTUPDATESTATUS!=1)
			{
				status=freePortUpdateStatus();
				rescode=-1;
				GetErrorMess(appliName, "CACHE HAS NOT BEEN INITIALIZED",ErrMess,1);
			}
			if(ociEnv==0)
			{
				status=freePortUpdateStatus();
				rescode=-1;
				GetErrorMess(appliName, "NOT CONNECTED TO ANY DB",ErrMess,1);
			}	
			if(ociEnv!=0 && FIRST_TIME_PORTUPDATESTATUS==1 && res_query==0)
			{
				status=0;
				GetErrorMess(appliName, "NO_ERROR",ErrMess,0);
			}

		}
		return (rescode+status);
	}

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif