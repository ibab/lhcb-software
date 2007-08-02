/********************************************************************************/
//	Author: L.Abadie
// version:v3.5
// changes: handling error such that can be extended to C++: (create a class SQLException) (strlen ck + update stmt)
// file which performs bulk collect insert for the device table
// N.B : Memory management (allocation+release) should be handled by the user application thus, for all the fct provided by this lib
/********************************************************************************/


#include "list_structures.h"
#include "system_info.h"
#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

#if defined( _WIN32) ||defined(WIN32)
#define EXTERN_CONFDB  _declspec(dllexport)
#else
#define EXTERN_CONFDB extern
#endif

	extern   OCIError* ociError ;
	extern   OCIEnv* ociEnv ;
	extern   OCISvcCtx* ociHdbc ;
	extern   OCIServer *mysrvhp;
	extern   OCISession *myusrhp; 

	/************************global variables for the device***********************/
	int FIRST_TIME_DEVICE=0;
	int FIRST_TIME_DEVICE_SPARE=0;
	char* _DeviceList;
	int _max_deviceLength;
	int _deviceListLength;


	char* _SpareDeviceList;
	int _max_SparedeviceLength;
	int _SparedeviceListLength;

	char* _hwtype1List;
	int _max_hwtype1Length;
	int _hwtype1ListLength;

	char* _Sparehwtype1List;
	int _max_Sparehwtype1Length;
	int _Sparehwtype1ListLength;

	char* _responsible1List;
	int _max_responsible1Length;
	int _responsible1ListLength;

	char* _devfunction1List;
	int _max_devfunction1Length;
	int _devfunction1ListLength;

	char* _Spareresponsible1List;
	int _max_Spareresponsible1Length;
	int _Spareresponsible1ListLength;

	char*  _comments1List;
	int _max_comments1Length;
	int _comments1ListLength;

	char*  _Sparecomments1List;
	int _max_Sparecomments1Length;
	int _Sparecomments1ListLength;


	char* _serialnb1List;
	int _max_serialnb1_length;
	int _serialnb1ListLength;

	char* _Spareserialnb1List;
	int _max_Spareserialnb1_length;
	int _Spareserialnb1ListLength;

	char* _DevtypeList;
	int _max_devtype_length;
	int _devtypeListLength;

	int*  _Node;
	int*  _Promiscuous_mode;
	int*  _sysIDList2;



	int _deviceNbElement;
	int _SparedeviceNbElement;
	int _devtypeNbElement;
	int _hwtype1NbElement;
	int _Sparehwtype1NbElement;

	int _responsible1NbElement;
	int _devfunction1NbElement;

	int _Spareresponsible1NbElement;
	int _comments1NbElement;
	int _Sparecomments1NbElement;
	int _nodeNbElement;
	int _promiscuous_modeNbElement;
	int _serialnb1NbElement;
	int _Spareserialnb1NbElement;
	int _nodeusedNbElement;
	int _sysIDList2NbElement;
	int _locationNbElement;
	int _SparelocationNbElement;

	char* _locationList;
	int _max_location_length;
	int _locationListLength;

	char* _SparelocationList;
	int _max_Sparelocation_length;
	int _SparelocationListLength;

	int* _deviceList_nullvalue;
	int _deviceListNbEl;

	int* _SparedeviceList_nullvalue;
	int _SparedeviceListNbEl;

	int* _serialnb1_nullvalue;
	int _serialnb1NbEl;

	int* _Spareserialnb1_nullvalue;
	int _Spareserialnb1NbEl;

	int* _devtype_nullvalue;
	int _devtypeNbEl;

	int* _responsible1_nullvalue;
	int _responsible1NbEl;

	int* _devfunction1_nullvalue;
	int _devfunction1NbEl;


	int* _Spareresponsible1_nullvalue;
	int _Spareresponsible1NbEl;

	int* _hwtype1_nullvalue;
	int _hwtype1NbEl;

	int* _Sparehwtype1_nullvalue;
	int _Sparehwtype1NbEl;

	int* _comments1_nullvalue;
	int _comments1NbEl;

	int* _Sparecomments1_nullvalue;
	int _Sparecomments1NbEl;

	int* _locationList_nullvalue;
	int _locationListNbEl;

	int* _SparelocationList_nullvalue;
	int _SparelocationListNbEl;
	int freeDevice()
	{
		int status=0;
		_DeviceList=(char*)realloc(_DeviceList,0*sizeof(char));
		_serialnb1List=(char*)realloc(_serialnb1List,0*sizeof(char));
		_DevtypeList=(char*)realloc(_DevtypeList,0*sizeof(char));
		_locationList=(char*)realloc(_locationList,0*sizeof(char));
		_hwtype1List=(char*)realloc(_hwtype1List,0*sizeof(char));
		_responsible1List=(char*)realloc(_responsible1List,0*sizeof(char));
		_comments1List=(char*)realloc(_comments1List,0*sizeof(char));
		_devfunction1List=(char*)realloc(_devfunction1List,0*sizeof(char));

		_Node=(int*)realloc(_Node,0*sizeof(int));
		_Promiscuous_mode=(int*)realloc(_Promiscuous_mode,0*sizeof(int));
		_deviceList_nullvalue=(int*)realloc(_deviceList_nullvalue,0*sizeof(int));
		_serialnb1_nullvalue=(int*)realloc(_serialnb1_nullvalue,0*sizeof(int));
		_locationList_nullvalue=(int*)realloc(_locationList_nullvalue,0*sizeof(int));
		_hwtype1_nullvalue=(int*)realloc(_hwtype1_nullvalue,0*sizeof(int));
		_comments1_nullvalue=(int*)realloc(_comments1_nullvalue,0*sizeof(int));
		_responsible1_nullvalue=(int*)realloc(_responsible1_nullvalue,0*sizeof(int));
		_devfunction1_nullvalue=(int*)realloc(_devfunction1_nullvalue,0*sizeof(int));

		_devtype_nullvalue=(int*)realloc(_devtype_nullvalue,0*sizeof(int));
		_devtype_nullvalue=NULL;
		_deviceList_nullvalue=NULL;
		_serialnb1_nullvalue=NULL;
		_sysIDList2=(int*)realloc(_sysIDList2,0*sizeof(int));

		_DeviceList=NULL;
		_serialnb1List=NULL;
		_Node=NULL;
		_locationList=NULL;
		_DevtypeList=NULL;
		_Promiscuous_mode=NULL;
		_sysIDList2=NULL;
		_hwtype1_nullvalue=NULL;
		_comments1_nullvalue=NULL;
		_responsible1_nullvalue=NULL;
		_devfunction1_nullvalue=NULL;

		_comments1List=NULL;
		_responsible1List=NULL;
		_hwtype1List=NULL;
		FIRST_TIME_DEVICE=0;
		return status;
	}
	int freeDeviceSpare()
	{
		int status=0;
		_SpareDeviceList=(char*)realloc(_SpareDeviceList,0*sizeof(char));
		_SparedeviceList_nullvalue=(int*)realloc(_SparedeviceList_nullvalue,0*sizeof(int));

		_Spareserialnb1List=(char*)realloc(_Spareserialnb1List,0*sizeof(char));
		_SparelocationList=(char*)realloc(_SparelocationList,0*sizeof(char));
		_Sparehwtype1List=(char*)realloc(_Sparehwtype1List,0*sizeof(char));
		_Spareresponsible1List=(char*)realloc(_Spareresponsible1List,0*sizeof(char));
		_Sparecomments1List=(char*)realloc(_Sparecomments1List,0*sizeof(char));

		_Spareserialnb1_nullvalue=(int*)realloc(_Spareserialnb1_nullvalue,0*sizeof(int));
		_SparelocationList_nullvalue=(int*)realloc(_SparelocationList_nullvalue,0*sizeof(int));
		_Sparehwtype1_nullvalue=(int*)realloc(_Sparehwtype1_nullvalue,0*sizeof(int));
		_Sparecomments1_nullvalue=(int*)realloc(_Sparecomments1_nullvalue,0*sizeof(int));
		_Spareresponsible1_nullvalue=(int*)realloc(_Spareresponsible1_nullvalue,0*sizeof(int));
		_SparedeviceList_nullvalue=NULL;
		_SpareDeviceList=NULL;
		_Spareserialnb1_nullvalue=NULL;
		_SparelocationList=NULL;
		_Sparehwtype1_nullvalue=NULL;
		_Sparecomments1_nullvalue=NULL;
		_Spareresponsible1_nullvalue=NULL;
		_Sparecomments1List=NULL;
		_Spareresponsible1List=NULL;
		_Sparehwtype1List=NULL;



		FIRST_TIME_DEVICE_SPARE=0;
		return status;
	}
	/**
	* Insert a device into the functional_device table and the hw table :it means that the hw device is currently IN_USE and it's having the given functional fct returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* @param systemnameList : list of the subsystems (UPPER CASE) by which this device type is used: a VELO_TELL1 board  is used by the VELO, TFC and DAQ. so you type VELO,TFC,DAQ. you separate the different subsystems with a coma.
	* @param devicename : functional name of the device
	* @param devicetype : name of the devicetype
	* @param node : 1 if it's a host node (usually starts or ends a path such as L1FEs) , 0 otherwise.
	* @param promiscuous_mode : 1 if put in this mode (default mode), 0 otherwise
	* @param serial_nb :  serial_nb of the device
	* @param hwtype :hardware type : this one should be the same as you declared for your spare: in the case of a VELO_TELL1_board, here you can put Tell1_board as they are common to all the subsystems
	* @param responsible :guy responsible for this hw
	* @param comments : some comments about the hw (max.1000 characters)
	* @param location :  location of the device : crate\\rack\\slot (as defined in the equipment DB) or on the detector
	* @param function_list : list of the functions of your devices (essentially for the DAQ, for instance DHCP_SERVER, DNS_SERVER...). Functions must be declared using insertSimpleDeviceFunction...If several, separate them with a coma. For ex. DHCP,DNS, NFS. if none, put "none"
	* @param first_time1 : 1 if it's your first MultipleDevice insert (save it into the database), 0 otherwise
	* @param last_rows1 : 1 if it's your MultipleDevice last insert (save it into the database), 0 otherwise  Maximum in one go: 10000 rows.( we force the insert)
	* @param ErrMess : error message in case of failure
	* @return 0 if it is successful
	*/

	//extern "C" __declspec(dllexport)
	EXTERN_CONFDB
		int InsertMultipleFunctionalDevices(char* systemnameList,char* devicename,char* devicetype,int node,int promiscuous_mode,char* serial_nb,char* hwtype,char* responsible,char* comments,char* location,char* function_list,int first_time1,int last_rows1,char* ErrMess)
	{
		char appliName[100]="InsertMultipleFunctionalDevices";
		char sqlstmt[2000];
		OCIStmt *hstmt;
		OCIBind  *bndp[11]; 
		int rescode=0;
		int free_mem=0;
		int res_query=0;
		sword status=0;
		char* dname=NULL;
		int i=0;
		char* dlocation=NULL;
		char* dserialnb=NULL;
		char* dtype=NULL;
		char* dresponsible=NULL;
		char* dcomments=NULL;
		char* dhwtype=NULL;
		char* dfunctiondev=NULL;

		int force_insert=0;
		int first_time=first_time1;
		char seqname[100]="lhcb_history_deviceseq.nextval";
		//std::cout<<"value of first_time "<<first_time <<std::endl;
		int last_rows=last_rows1;
		char seq_name[100]="lhcb_lg_deviceseq.nextval";
		int* numrows_inserted=NULL;
		int numrows=0;
		OCIBind  *bndp3 = (OCIBind *) 0;

		int dname1=null_charptr(devicename);
		int dserialnb1=null_charptr(serial_nb);
		int dtype1=null_charptr(devicetype);
		int dlocation1=null_charptr(location);
		int dresponsible1=null_charptr(responsible);
		int dhwtype1=null_charptr(hwtype);
		int dcomments1=null_charptr(comments);
		int dfunctiondev1=null_charptr(function_list);
		if(FIRST_TIME_DEVICE==1 && _DeviceList==NULL)
		{
			first_time=1;
			//std::cout<<"case of FIRST_TIME_DEVICE==1 && _DeviceList==NULL"<<std::endl;

		}
		if(first_time==1)
		{
			status=freeDevice(); //delete the cache in case it was exiting with error
			FIRST_TIME_DEVICE=1;

		}
		int sysID=GetSubsystemID(systemnameList);
		if(sysID==-1)
		{
			GetErrorMess(appliName, "systemnameList is null",ErrMess,1);

			return -1;

		}
		res_query=AppendString(devicename,_DeviceList,_deviceListLength,_deviceNbElement,_max_deviceLength,first_time);
		res_query+=AppendString(serial_nb,_serialnb1List,_serialnb1ListLength,_serialnb1NbElement,_max_serialnb1_length,first_time);
		res_query+=AppendString(devicetype,_DevtypeList,_devtypeListLength,_devtypeNbElement,_max_devtype_length,first_time);
		res_query+=AppendString(location,_locationList,_locationListLength,_locationNbElement,_max_location_length,first_time);
		res_query+=AppendString(responsible,_responsible1List,_responsible1ListLength,_responsible1NbElement,_max_responsible1Length,first_time);
		res_query+=AppendString(comments,_comments1List,_comments1ListLength,_comments1NbElement,_max_comments1Length,first_time);
		res_query+=AppendString(hwtype,_hwtype1List,_hwtype1ListLength,_hwtype1NbElement,_max_hwtype1Length,first_time);
		res_query+=AppendString(function_list,_devfunction1List,_devfunction1ListLength,_devfunction1NbElement,_max_devfunction1Length,first_time);

		res_query+=AppendInt(node,_Node,_nodeNbElement,first_time);
		res_query+=AppendInt(sysID,_sysIDList2,_sysIDList2NbElement,first_time);
		res_query+=AppendInt(promiscuous_mode,_Promiscuous_mode,_promiscuous_modeNbElement,first_time);


		res_query+=AppendInt(dname1,_deviceList_nullvalue,_deviceListNbEl,first_time);
		res_query+=AppendInt(dlocation1,_locationList_nullvalue,_locationListNbEl,first_time);
		res_query+=AppendInt(dserialnb1,_serialnb1_nullvalue,_serialnb1NbEl,first_time);
		res_query+=AppendInt(dtype1,_devtype_nullvalue,_devtypeNbEl,first_time);
		res_query+=AppendInt(dhwtype1,_hwtype1_nullvalue,_hwtype1NbEl,first_time);
		res_query+=AppendInt(dcomments1,_comments1_nullvalue,_comments1NbEl,first_time);
		res_query+=AppendInt(dresponsible1,_responsible1_nullvalue,_responsible1NbEl,first_time);
		res_query+=AppendInt(dfunctiondev1,_devfunction1_nullvalue,_devfunction1NbEl,first_time);

		//std::cout<<"value of devicelist "<<_DeviceList<<std::endl;

		//std::cout<<"value of FIRST_TIME_DEVICE "<<FIRST_TIME_DEVICE <<std::endl;

		if(_deviceNbElement==MAXROWS && last_rows!=1)
		{
			force_insert=1;
			last_rows=1;
		}
		status=res_query;
		if(last_rows==1 && res_query==0 && ociEnv!=0 && FIRST_TIME_DEVICE==1)
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
				status+=freeDevice();
				if(dname!=NULL)
					free(dname);
				if(dtype!=NULL)
					free(dtype);
				if(dlocation!=NULL)
					free(dlocation);
				if(dserialnb!=NULL)
					free(dserialnb);
				if(dresponsible!=NULL)
					free(dresponsible);
				if(dcomments!=NULL)
					free(dcomments);
				if(dhwtype!=NULL)
					free(dhwtype);
				if(dfunctiondev!=NULL)
					free(dfunctiondev);
				free(errmessg1);
				free(errmessg2);
				return rescode;
			}
			//need to proceed with messages

			status =OCIHandleAlloc (ociEnv, (void**)&hstmt, OCI_HTYPE_STMT , 0, 0);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIHandleAlloc unsuccessful");
			}
			else
			{
				sprintf(sqlstmt,
					"BEGIN insert all into %s(serialnb,hwtype,responsible,user_comments,device_status) values(:serialnb,:hwtype,:responsible,:comments,%d) select 1 from %s where devicetype=:dtype; insert all into %s(devicename,devicetypeID,node,deviceid,promiscuous_mode,system_name,serialnb,location,author,user_update,terminal_name,nodeused,functionID) values (:dname,devicetypeID,:dnode,%s,:dpromiscuous_mode,:sysID,:serialnb,:location,'%s','%s','%s',1,fctID) select devicetypeID,decode(:fct_list,'none',0,%s(:fct_list)) as fctID  from %s  where devicetype=:dtype; insert all into %s(historydeviceID,serialnb,deviceid,device_status,user_comment,location,status_change,system_name,created,author,terminal_name) values(%s,:serialnb,deviceid,'IN_USE',:comments,:location,statuschange,:sysID,sysdate,'%s','%s') select deviceid,created as statuschange from %s where devicename=:dname; :numrows:=%s; END;",HW_DEVICE_TABLE,IN_USE,DEVICETYPE_TABLE,LOGICAL_DEVICE_TABLE,seq_name,login,login,host,_ComposeFctID,DEVICETYPE_TABLE,HISTORY_DEVICE_TABLE,seqname,login,host,LOGICAL_DEVICE_TABLE,SQLROWCOUNT);
				status=OCIStmtPrepare(hstmt, ociError, (text*)sqlstmt, (ub4)strlen((char *)sqlstmt), (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
			}

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
			}
			else
			{
				dname=(char*)malloc( _deviceNbElement*_max_deviceLength*sizeof(char));
				if(dname!=NULL)
				{
					status=NormalizeVector(_DeviceList, _deviceNbElement,_max_deviceLength,dname);
					//std::cout<<"dname not null "<<_max_deviceLength<<std::endl;
				}
				dserialnb=(char*)malloc( _serialnb1NbElement*_max_serialnb1_length*sizeof(char));
				if(dserialnb!=NULL)
				{
					status+=NormalizeVector(_serialnb1List, _serialnb1NbElement,_max_serialnb1_length,dserialnb);
					//std::cout<<"dserialnb not null "<<_max_serialnb1_length<<std::endl;
				}
				dlocation=(char*)malloc( _locationNbElement*_max_location_length*sizeof(char));
				if(dlocation!=NULL)
				{
					status+=NormalizeVector(_locationList, _locationNbElement,_max_location_length,dlocation);
					//std::cout<<"dlocation not null "<<_max_location_length<<std::endl;
				}
				dtype=(char*)malloc(_devtypeNbElement*_max_devtype_length*sizeof(char));
				if(dtype!=NULL)
				{
					status+=NormalizeVector(_DevtypeList, _devtypeNbElement,_max_devtype_length,dtype);
					//std::cout<<"dtype not null "<<_max_devtype_length<<std::endl;
				}
				dhwtype=(char*)malloc(_hwtype1NbElement*_max_hwtype1Length*sizeof(char));
				if(dhwtype!=NULL)
					status+=NormalizeVector(_hwtype1List, _hwtype1NbElement,_max_hwtype1Length,dhwtype);
				dcomments=(char*)malloc(_comments1NbElement*_max_comments1Length*sizeof(char));
				if(dcomments!=NULL)
					status+=NormalizeVector(_comments1List, _comments1NbElement,_max_comments1Length,dcomments);
				dresponsible=(char*)malloc(_responsible1NbElement*_max_responsible1Length*sizeof(char));
				if(dresponsible!=NULL)
				{
					status+=NormalizeVector(_responsible1List, _responsible1NbElement,_max_responsible1Length,dresponsible);
					//std::cout<<"dresponsible not null "<<_max_responsible1Length<<std::endl;
				}
				dfunctiondev=(char*)malloc(_devfunction1NbElement*_max_devfunction1Length*sizeof(char));
				if(dfunctiondev!=NULL)
				{
					status+=NormalizeVector(_devfunction1List, _devfunction1NbElement,_max_devfunction1Length,dfunctiondev);
					//std::cout<<"dresponsible not null "<<_max_responsible1Length<<std::endl;
				}
				numrows_inserted=(int*)malloc(sizeof(int)*_deviceNbElement);
				//std::cout<<"after memory allocation "<<_deviceNbElement<<std::endl;
				if( (dtype==NULL) || (dname==NULL) || dfunctiondev==NULL || (dresponsible==NULL) || (dserialnb==NULL) || (dresponsible==NULL)||dcomments==NULL||dhwtype==NULL||numrows_inserted==NULL)
				{
					//std::cout<<"memory pb "<<std::endl;

					rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");
					GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
					status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
					//std::cout<<"memory pb 1"<<std::endl;
					status+=freeDevice();
					//std::cout<<"memory pb 2"<<std::endl;
					if(dname!=NULL)
						free(dname);
					if(dtype!=NULL)
						free(dtype);
					if(dlocation!=NULL)
						free(dlocation);
					//std::cout<<"memory pb 3"<<std::endl;
					if(dserialnb!=NULL)
						free(dserialnb);
					if(dfunctiondev!=NULL)
						free(dfunctiondev);
					if(dresponsible!=NULL)
						free(dresponsible);
					if(dcomments!=NULL)
						free(dcomments);
					//std::cout<<"memory pb 4"<<std::endl;
					if(dhwtype!=NULL)
						free(dhwtype);
					if(numrows_inserted!=NULL)
						free(numrows_inserted);
					//if(errmessg1!=NULL)
					//free(errmessg1);
					//if(errmessg2!=NULL)
					//free(errmessg2);
					return -1;
				}
				else
				{
					for(i=0;i<_deviceNbElement;i++)
						numrows_inserted[i]=0;
					//std::cout<<"before binding 0"<<std::endl;
					status =OCIBindByName(hstmt, &bndp[0], ociError,(text*) ":dname", -1,(dvoid*)dname, _max_deviceLength,  SQLT_STR, (dvoid *) & _deviceList_nullvalue[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
				}
			}
			//std::cout<<"before binding "<<std::endl;
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp[1], ociError,(text*) ":dtype", -1,(dvoid*)dtype, _max_devtype_length,  SQLT_STR, (dvoid *) & _devtype_nullvalue[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp[2], ociError,(text*) ":dnode", -1,(dvoid*)&_Node[0], sizeof(node),  SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp[3], ociError,(text*) ":dpromiscuous_mode", -1,(dvoid*)&_Promiscuous_mode[0], sizeof(promiscuous_mode),  SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp[4], ociError,(text*) ":sysID", -1,(dvoid*)&_sysIDList2[0], sizeof(sysID),  SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp[5], ociError,(text*) ":serialnb", -1,(dvoid*)dserialnb, _max_serialnb1_length,  SQLT_STR, (dvoid *) & _serialnb1_nullvalue[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp[6], ociError,(text*) ":location", -1,(dvoid*)dlocation, _max_location_length,  SQLT_STR, (dvoid *)& _locationList_nullvalue[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp[7], ociError,(text*) ":hwtype", -1,(dvoid*)dhwtype, _max_hwtype1Length,SQLT_STR, (dvoid *)&_hwtype1_nullvalue[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp[8], ociError,(text*) ":responsible", -1,(dvoid*)dresponsible, _max_responsible1Length,SQLT_STR, (dvoid *) &_responsible1_nullvalue[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp[9], ociError,(text*) ":comments", -1,(dvoid*)dcomments, _max_comments1Length,SQLT_STR, (dvoid *) &_comments1_nullvalue[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp[10], ociError,(text*) ":fct_list", -1,(dvoid*)dfunctiondev, _max_devfunction1Length,SQLT_STR, (dvoid *) &_devfunction1_nullvalue[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp3, ociError,(text*) ":numrows", -1,(dvoid*)&numrows_inserted[0], sizeof(int),SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);




			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp[0], ociError,_max_deviceLength, sizeof(int),0, 0);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp[1], ociError,  _max_devtype_length, sizeof(int), 0, 0);
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
				status=OCIBindArrayOfStruct(bndp[3], ociError,  sizeof(int), 0, 0, 0);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp[4], ociError, sizeof(int), 0, 0, 0);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp[5], ociError, _max_serialnb1_length, sizeof(int), 0, 0);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp[6], ociError, _max_location_length, sizeof(int), 0, 0);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp[7], ociError, _max_hwtype1Length, sizeof(int), 0, 0);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp[8], ociError, _max_responsible1Length, sizeof(int), 0, 0);


			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp[9], ociError, _max_comments1Length, sizeof(int), 0, 0);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp[10], ociError, _max_devfunction1Length, sizeof(int), 0, 0);


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
				status= OCIStmtExecute(ociHdbc, hstmt, ociError, _deviceNbElement, 0, 0, 0, OCI_DEFAULT );
			//std::cout<<"after execute "<<std::endl;
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
			}
			else
			{
				numrows=-1;
				for(i=0;i< _deviceNbElement;i++)
				{
					if(numrows_inserted[i]==0)
					{
						numrows=0;
						i= _deviceNbElement+5;
					}
					else
						numrows=numrows_inserted[i];
				}
				if(numrows==0)
				{
					status = OCITransCommit(ociHdbc, ociError, 0);
					status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
					status+=freeDevice();
					if(dname!=NULL)
						free(dname);
					if(dtype!=NULL)
						free(dtype);
					if(dlocation!=NULL)
						free(dlocation);
					if(dserialnb!=NULL)
						free(dserialnb);
					if(dresponsible!=NULL)
						free(dresponsible);
					if(dcomments!=NULL)
						free(dcomments);
					if(dhwtype!=NULL)
						free(dhwtype);
					if(dfunctiondev!=NULL)
						free(dfunctiondev);
					if(numrows_inserted!=NULL)
						free(numrows_inserted);

					GetErrorMess(appliName, "COULDNOT_INSERT_ALL_ROWS",ErrMess,1);
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

			status+=freeDevice();
			//std::cout<<"after free device final"<<std::endl;
			if(dname!=NULL)
				free(dname);
			if(dtype!=NULL)
				free(dtype);
			if(dlocation!=NULL)
				free(dlocation);
			if(dserialnb!=NULL)
				free(dserialnb);
			if(dresponsible!=NULL)
				free(dresponsible);
			if(dcomments!=NULL)
				free(dcomments);
			if(dhwtype!=NULL)
				free(dhwtype);
			if(dfunctiondev!=NULL)
				free(dfunctiondev);
			if(numrows_inserted!=NULL)
				free(numrows_inserted);

			status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);

			if(rescode!=0)
				if(ociError!=0)
					OCIReportError(ociError,appliName, ErrMess,1); 
				else
					GetErrorMess(appliName,  "NOT CONNECTED TO ANY DB",ErrMess,1); 
			else
				OCIReportError(ociError,appliName, ErrMess,0); 
			if(force_insert==1 && rescode==0)
			{
				status=freeDevice();
				FIRST_TIME_DEVICE=1;
				force_insert=0;

			}
		}
		else
		{
			if(res_query!=0)
			{
				status=freeDevice();
				GetErrorMess(appliName, "Cache Problem",ErrMess,1);
			}
			if(FIRST_TIME_DEVICE!=1)
			{
				status=freeDevice();

				res_query=-1;
				GetErrorMess(appliName, "CACHE HAS NOT BEEN INITIALIZED",ErrMess,1);
			}
			if(ociEnv==0)
			{
				status=freeDevice();
				res_query=-1;
				GetErrorMess(appliName, "NOT CONNECTED TO ANY DB",ErrMess,1);
			}
			status+=res_query;
			if(ociEnv!=0 && FIRST_TIME_DEVICE==1 && res_query==0)
			{
				status=0;
				GetErrorMess(appliName, "NO_ERROR",ErrMess,0);
			}

		}
		//std::cout<<"end of fct "<<std::endl;
		return (status+rescode);
	}

	/**
	* Insert a spare device into the  the hw table :it means that the hw device has the status SPARE and it's having the given functional fct returning an integer value.
	* The user should manage the memory : there is no memory allocation.

	* @param serial_nb :  serial_nb of the device
	* @param hwname : you can put "spare_nb";
	* @param hwtype : hardware type (market specif) prefixed by the subsystem like "VELO_", if the spare is specific to it.hw type is sth which specifies whta type of spare it is so that we can group spare. Ex. Tell1 board is a hw type. 
	* @param responsible :guy responsible for this hw
	* @param comments : some comments about the hw (max.1000 characters)
	* @param location :  location of the spare :which building shelf?
	* @param first_time1 : 1 if it's your first MultipleDevice insert (save it into the database), 0 otherwise
	* @param last_rows1 : 1 if it's your MultipleDevice last insert (save it into the database), 0 otherwise  Maximum in one go: 10000 rows.( we force the insert)
	* @param ErrMess : error message in case of failure
	* @return 0 if it is successful
	*/

	//extern "C" __declspec(dllexport)
	EXTERN_CONFDB
		int InsertMultipleSpareDevices(char* hwname,char* hwtype,char* serial_nb,char* responsible,char* comments,char* location,int first_time1,int last_rows1,char* ErrMess)
	{
		char appliName[100]="InsertMultipleSpareDevices";
		char sqlstmt[2000];
		OCIStmt *hstmt;
		OCIBind  *bndp[7]; 
		int rescode=0;
		int free_mem=0;
		int res_query=0;
		sword status=0;
		char* dname=NULL;
		int i=0;
		char* dlocation=NULL;
		char* dserialnb=NULL;
		char* dresponsible=NULL;
		char* dcomments=NULL;
		char* dhwtype=NULL;
		int force_insert=0;
		int first_time=first_time1;
		char seqname[100]="lhcb_history_deviceseq.nextval";
		//std::cout<<"value of first_time "<<first_time <<std::endl;
		int last_rows=last_rows1;
		int* numrows_inserted=NULL;
		int numrows=0;
		OCIBind  *bndp3 = (OCIBind *) 0;

		int dname1=null_charptr(hwname);
		int dserialnb1=null_charptr(serial_nb);
		int dlocation1=null_charptr(location);
		int dresponsible1=null_charptr(responsible);
		int dhwtype1=null_charptr(hwtype);
		int dcomments1=null_charptr(comments);
		if(FIRST_TIME_DEVICE_SPARE==1 && _SpareDeviceList==NULL)
		{
			first_time=1;
			//std::cout<<"case of FIRST_TIME_DEVICE==1 && _DeviceList==NULL"<<std::endl;

		}
		if(first_time==1)
		{
			status=freeDeviceSpare(); //delete the cache in case it was exiting with error
			FIRST_TIME_DEVICE_SPARE=1;

		}

		res_query=AppendString(hwname,_SpareDeviceList,_SparedeviceListLength,_SparedeviceNbElement,_max_SparedeviceLength,first_time);
		res_query+=AppendString(serial_nb,_Spareserialnb1List,_Spareserialnb1ListLength,_Spareserialnb1NbElement,_max_Spareserialnb1_length,first_time);
		res_query+=AppendString(location,_SparelocationList,_SparelocationListLength,_SparelocationNbElement,_max_Sparelocation_length,first_time);
		res_query+=AppendString(responsible,_Spareresponsible1List,_Spareresponsible1ListLength,_Spareresponsible1NbElement,_max_Spareresponsible1Length,first_time);
		res_query+=AppendString(comments,_Sparecomments1List,_Sparecomments1ListLength,_Sparecomments1NbElement,_max_Sparecomments1Length,first_time);
		res_query+=AppendString(hwtype,_Sparehwtype1List,_Sparehwtype1ListLength,_Sparehwtype1NbElement,_max_Sparehwtype1Length,first_time);




		res_query+=AppendInt(dname1,_SparedeviceList_nullvalue,_SparedeviceListNbEl,first_time);
		res_query+=AppendInt(dlocation1,_SparelocationList_nullvalue,_SparelocationListNbEl,first_time);
		res_query+=AppendInt(dserialnb1,_Spareserialnb1_nullvalue,_Spareserialnb1NbEl,first_time);
		res_query+=AppendInt(dhwtype1,_Sparehwtype1_nullvalue,_Sparehwtype1NbEl,first_time);
		res_query+=AppendInt(dcomments1,_Sparecomments1_nullvalue,_Sparecomments1NbEl,first_time);
		res_query+=AppendInt(dresponsible1,_Spareresponsible1_nullvalue,_Spareresponsible1NbEl,first_time);
		//std::cout<<"value of devicelist "<<_DeviceList<<std::endl;

		//std::cout<<"value of FIRST_TIME_DEVICE "<<FIRST_TIME_DEVICE <<std::endl;

		if(_SparedeviceNbElement==MAXROWS && last_rows!=1)
		{
			force_insert=1;
			last_rows=1;
		}
		status=res_query;
		if(last_rows==1 && res_query==0 && ociEnv!=0 && FIRST_TIME_DEVICE_SPARE==1)
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
				status+=freeDeviceSpare();
				if(dname!=NULL)
					free(dname);

				if(dlocation!=NULL)
					free(dlocation);
				if(dserialnb!=NULL)
					free(dserialnb);
				if(dresponsible!=NULL)
					free(dresponsible);
				if(dcomments!=NULL)
					free(dcomments);
				if(dhwtype!=NULL)
					free(dhwtype);

				free(errmessg1);
				free(errmessg2);
				return rescode;
			}
			//need to proceed with messages

			status =OCIHandleAlloc (ociEnv, (void**)&hstmt, OCI_HTYPE_STMT , 0, 0);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIHandleAlloc unsuccessful");
			}
			else
			{
				sprintf(sqlstmt,"BEGIN insert into %s(serialnb,hwtype,hwname,responsible,user_comments,location,device_status) values(:serialnb,:hwtype,:hwname,:responsible,:comments,:location,%d);insert into %s(historydeviceID,serialnb,device_status,user_comment,location,status_change,created,author,terminal_name) values(%s,:serialnb,'SPARE',:comments,:location,sysdate,sysdate,'%s','%s'); :numrows:=%s; END;",HW_DEVICE_TABLE,SPARE,HISTORY_DEVICE_TABLE,seqname,login,host,SQLROWCOUNT);
				status=OCIStmtPrepare(hstmt, ociError, (text*)sqlstmt, (ub4)strlen((char *)sqlstmt), (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
			}

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
			}
			else
			{
				dname=(char*)malloc( _SparedeviceNbElement*_max_SparedeviceLength*sizeof(char));
				if(dname!=NULL)
				{
					status=NormalizeVector(_SpareDeviceList, _SparedeviceNbElement,_max_SparedeviceLength,dname);
					//std::cout<<"dname not null "<<_max_deviceLength<<std::endl;
				}
				dserialnb=(char*)malloc( _Spareserialnb1NbElement*_max_Spareserialnb1_length*sizeof(char));
				if(dserialnb!=NULL)
				{
					status+=NormalizeVector(_Spareserialnb1List, _Spareserialnb1NbElement,_max_Spareserialnb1_length,dserialnb);
					//std::cout<<"dserialnb not null "<<_max_serialnb1_length<<std::endl;
				}
				dlocation=(char*)malloc( _SparelocationNbElement*_max_Sparelocation_length*sizeof(char));
				if(dlocation!=NULL)
				{
					status+=NormalizeVector(_SparelocationList, _SparelocationNbElement,_max_Sparelocation_length,dlocation);
					//std::cout<<"dlocation not null "<<_max_location_length<<std::endl;
				}

				dhwtype=(char*)malloc(_Sparehwtype1NbElement*_max_Sparehwtype1Length*sizeof(char));
				if(dhwtype!=NULL)
					status+=NormalizeVector(_Sparehwtype1List, _Sparehwtype1NbElement,_max_Sparehwtype1Length,dhwtype);
				dcomments=(char*)malloc(_Sparecomments1NbElement*_max_Sparecomments1Length*sizeof(char));
				if(dcomments!=NULL)
					status+=NormalizeVector(_Sparecomments1List, _Sparecomments1NbElement,_max_Sparecomments1Length,dcomments);
				dresponsible=(char*)malloc(_Spareresponsible1NbElement*_max_Spareresponsible1Length*sizeof(char));
				if(dresponsible!=NULL)
				{
					status+=NormalizeVector(_Spareresponsible1List, _Spareresponsible1NbElement,_max_Spareresponsible1Length,dresponsible);
					//std::cout<<"dresponsible not null "<<_max_responsible1Length<<std::endl;
				}
				numrows_inserted=(int*)malloc(sizeof(int)*_SparedeviceNbElement);
				//std::cout<<"after memory allocation "<<_deviceNbElement<<std::endl;
				if(  (dname==NULL) || (dresponsible==NULL) || (dserialnb==NULL) || (dresponsible==NULL)||dcomments==NULL||dhwtype==NULL||numrows_inserted==NULL)
				{
					//std::cout<<"memory pb "<<std::endl;

					rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");
					GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
					status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
					//std::cout<<"memory pb 1"<<std::endl;
					status+=freeDeviceSpare();
					//std::cout<<"memory pb 2"<<std::endl;
					if(dname!=NULL)
						free(dname);

					if(dlocation!=NULL)
						free(dlocation);
					//std::cout<<"memory pb 3"<<std::endl;
					if(dserialnb!=NULL)
						free(dserialnb);
					if(dresponsible!=NULL)
						free(dresponsible);
					if(dcomments!=NULL)
						free(dcomments);
					//std::cout<<"memory pb 4"<<std::endl;
					if(dhwtype!=NULL)
						free(dhwtype);
					if(numrows_inserted!=NULL)
						free(numrows_inserted);
					//if(errmessg1!=NULL)
					//free(errmessg1);
					//if(errmessg2!=NULL)
					//free(errmessg2);
					return -1;
				}
				else
				{
					for(i=0;i<_SparedeviceNbElement;i++)
						numrows_inserted[i]=0;
					//std::cout<<"before binding 0"<<std::endl;
					status =OCIBindByName(hstmt, &bndp[0], ociError,(text*) ":hwname", -1,(dvoid*)dname, _max_SparedeviceLength,  SQLT_STR, (dvoid *) & _SparedeviceList_nullvalue[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
				}
			}
			//std::cout<<"before binding "<<std::endl;
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp[1], ociError,(text*) ":serialnb", -1,(dvoid*)dserialnb, _max_Spareserialnb1_length,  SQLT_STR, (dvoid *) & _Spareserialnb1_nullvalue[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp[2], ociError,(text*) ":location", -1,(dvoid*)dlocation, _max_Sparelocation_length,  SQLT_STR, (dvoid *)& _SparelocationList_nullvalue[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp[3], ociError,(text*) ":hwtype", -1,(dvoid*)dhwtype, _max_Sparehwtype1Length,SQLT_STR, (dvoid *)&_Sparehwtype1_nullvalue[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp[4], ociError,(text*) ":responsible", -1,(dvoid*)dresponsible, _max_Spareresponsible1Length,SQLT_STR, (dvoid *) &_Spareresponsible1_nullvalue[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp[5], ociError,(text*) ":comments", -1,(dvoid*)dcomments, _max_Sparecomments1Length,SQLT_STR, (dvoid *) &_Sparecomments1_nullvalue[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp3, ociError,(text*) ":numrows", -1,(dvoid*)&numrows_inserted[0], sizeof(int),SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);



			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp[0], ociError,_max_SparedeviceLength, sizeof(int),0, 0);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp[1], ociError, _max_Spareserialnb1_length, sizeof(int), 0, 0);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp[2], ociError, _max_Sparelocation_length, sizeof(int), 0, 0);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp[3], ociError, _max_Sparehwtype1Length, sizeof(int), 0, 0);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp[4], ociError, _max_Spareresponsible1Length, sizeof(int), 0, 0);


			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp[5], ociError, _max_Sparecomments1Length, sizeof(int), 0, 0);



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
				status= OCIStmtExecute(ociHdbc, hstmt, ociError, _SparedeviceNbElement, 0, 0, 0, OCI_DEFAULT );
			//std::cout<<"after execute "<<std::endl;
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
			}
			else
			{
				numrows=-1;
				for(i=0;i< _SparedeviceNbElement;i++)
				{
					if(numrows_inserted[i]==0)
					{
						numrows=0;
						i= _SparedeviceNbElement+5;
					}
					else
						numrows=numrows_inserted[i];
				}
				if(numrows==0)
				{
					status = OCITransCommit(ociHdbc, ociError, 0);
					status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
					status+=freeDeviceSpare();
					if(dname!=NULL)
						free(dname);

					if(dlocation!=NULL)
						free(dlocation);
					if(dserialnb!=NULL)
						free(dserialnb);
					if(dresponsible!=NULL)
						free(dresponsible);
					if(dcomments!=NULL)
						free(dcomments);
					if(dhwtype!=NULL)
						free(dhwtype);
					if(numrows_inserted!=NULL)
						free(numrows_inserted);

					GetErrorMess(appliName, "COULDNOT_INSERT_ALL_ROWS",ErrMess,1);
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

			status+=freeDeviceSpare();
			//std::cout<<"after free device final"<<std::endl;
			if(dname!=NULL)
				free(dname);

			if(dlocation!=NULL)
				free(dlocation);
			if(dserialnb!=NULL)
				free(dserialnb);
			if(dresponsible!=NULL)
				free(dresponsible);
			if(dcomments!=NULL)
				free(dcomments);
			if(dhwtype!=NULL)
				free(dhwtype);
			if(numrows_inserted!=NULL)
				free(numrows_inserted);

			status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);

			if(rescode!=0)
				if(ociError!=0)
					OCIReportError(ociError,appliName, ErrMess,1); 
				else
					GetErrorMess(appliName,  "NOT CONNECTED TO ANY DB",ErrMess,1); 
			else
				OCIReportError(ociError,appliName, ErrMess,0); 
			if(force_insert==1 && rescode==0)
			{
				status=freeDeviceSpare();
				FIRST_TIME_DEVICE_SPARE=1;
				force_insert=0;

			}
		}
		else
		{
			if(res_query!=0)
			{
				status=freeDeviceSpare();
				GetErrorMess(appliName, "Cache Problem",ErrMess,1);
			}
			if(FIRST_TIME_DEVICE_SPARE!=1)
			{
				status=freeDeviceSpare();

				res_query=-1;
				GetErrorMess(appliName, "CACHE HAS NOT BEEN INITIALIZED",ErrMess,1);
			}
			if(ociEnv==0)
			{
				status=freeDeviceSpare();
				res_query=-1;
				GetErrorMess(appliName, "NOT CONNECTED TO ANY DB",ErrMess,1);
			}
			status+=res_query;
			if(ociEnv!=0 && FIRST_TIME_DEVICE_SPARE==1 && res_query==0)
			{
				status=0;
				GetErrorMess(appliName, "NO_ERROR",ErrMess,0);
			}

		}
		//std::cout<<"end of fct "<<std::endl;
		return (status+rescode);

	}
#if defined(__cplusplus) || defined(c_plusplus)
}
#endif