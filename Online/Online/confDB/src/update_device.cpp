/********************************************************************************/
//	Author: L.Abadie
// version:v3.5
// changes: handling error such that can be extended to C++: (create a class SQLException) (strlen ck + update stmt)
// file which performs bulk collect update for the device table
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
	int FIRST_TIME_DEVICEUPDATE=0; //attributes
	int FIRST_TIME_DEVICEUPDATE5=0; //devicename
	int FIRST_TIME_DEVICEUPDATE6=0; //device type

	int FIRST_TIME_ACTIVEUPDATE=0;
	int FIRST_TIME_STATUSUPDATE=0;
	int FIRST_TIME_NODEUSEDUPDATE=0;
	int FIRST_TIME_SYSUPDATE=0;

	char* _DeviceListUpdate;
	int _max_deviceLengthUpdate;
	int _deviceListLengthUpdate;


	char* _DeviceList5Update;
	int _max_deviceLength5Update;
	int _deviceListLength5Update;

	char* _DeviceList6Update;
	int _max_deviceLength6Update;
	int _deviceListLength6Update;

	char* _OldDeviceListUpdate;
	int _max_OlddeviceLengthUpdate;
	int _OlddeviceListLengthUpdate;

	char* _NodeUsedDeviceListUpdate;
	int _max_NodeUseddeviceLengthUpdate;
	int _NodeUseddeviceListLengthUpdate;

	char* _SysDeviceListUpdate;
	int _max_SysdeviceLengthUpdate;
	int _SysdeviceListLengthUpdate;


	char* _StatusDeviceListUpdate;
	int _max_StatusdeviceLengthUpdate;
	int _StatusdeviceListLengthUpdate;

	char* _ActiveDeviceListUpdate;
	int _max_ActivedeviceLengthUpdate;
	int _ActivedeviceListLengthUpdate;

	char* _locationListUpdate;
	int _max_location_lengthUpdate;
	int _locationListLengthUpdate;

	char* _devfunctionListUpdate;
	int _max_devfunction_lengthUpdate;
	int _devfunctionListLengthUpdate;


	char* _DevtypeListUpdate;
	int _max_devtype_lengthUpdate;
	int _devtypeListLengthUpdate;



	int*  _NodeUpdate;

	int*  _Promiscuous_modeUpdate;
	int*  _NodeUsedUpdate;
	int*  _SysUpdate;

	int*  _ActiveUpdate;
	int*  _StatusUpdate;
	int _StatusNbElementUpdate;
	int _ActiveNbElementUpdate;
	int _NodeUsedNbElementUpdate;
	int _deviceNbElementUpdate;
	int _deviceNbElement5Update;
	int _SysNbElementUpdate;
	int _deviceNbElement6Update;
	int _devfunctionNbElementUpdate;

	int _NodeUseddeviceNbElementUpdate;
	int _StatusdeviceNbElementUpdate;
	int _ActivedeviceNbElementUpdate;
	int _OlddeviceNbElementUpdate;
	int _devtypeNbElementUpdate;
	int _nodeNbElementUpdate;
	int _promiscuous_modeNbElementUpdate;
	int _locationNbElementUpdate;

	int _SysdeviceNbElementUpdate;

	int* _deviceList_nullvalueUpdate;
	int _deviceListNbElUpdate;
	int* _deviceList_nullvalue5Update;
	int _deviceListNbEl5Update;
	int* _deviceList_nullvalue6Update;
	int _deviceListNbEl6Update;
	int* _OlddeviceList_nullvalueUpdate;
	int _OlddeviceListNbElUpdate;

	int* _StatusdeviceList_nullvalueUpdate;
	int _StatusdeviceListNbElUpdate;

	int* _ActivedeviceList_nullvalueUpdate;
	int _ActivedeviceListNbElUpdate;

	int* _NodeUseddeviceList_nullvalueUpdate;
	int _NodeUseddeviceListNbElUpdate;

	int* _SysdeviceList_nullvalueUpdate;
	int _SysdeviceListNbElUpdate;



	int* _location_nullvalueUpdate;
	int _locationNbElUpdate;

	int* _devfunction_nullvalueUpdate;
	int _devfunctionNbElUpdate;

	int* _devtype_nullvalueUpdate;
	int _devtypeNbElUpdate;

	int freeDeviceUpdate()
	{
		int status=0;
		_DeviceListUpdate=(char*)realloc(_DeviceListUpdate,0*sizeof(char));

		_locationListUpdate=(char*)realloc(_locationListUpdate,0*sizeof(char));
		_devfunctionListUpdate=(char*)realloc(_devfunctionListUpdate,0*sizeof(char));

		_NodeUpdate=(int*)realloc(_NodeUpdate,0*sizeof(int));
		_Promiscuous_modeUpdate=(int*)realloc(_Promiscuous_modeUpdate,0*sizeof(int));
		_deviceList_nullvalueUpdate=(int*)realloc(_deviceList_nullvalueUpdate,0*sizeof(int));

		_location_nullvalueUpdate=(int*)realloc(_location_nullvalueUpdate,0*sizeof(int));
		_deviceList_nullvalueUpdate=NULL;
		_location_nullvalueUpdate=NULL;
		_devfunctionListUpdate=NULL;
		_DeviceListUpdate=NULL;
		_locationListUpdate=NULL;
		_NodeUpdate=NULL;
		_Promiscuous_modeUpdate=NULL;
		FIRST_TIME_DEVICEUPDATE=0;

		return status;
	}
	int freeDeviceUpdate5()
	{
		int status=0;
		_DeviceList5Update=(char*)realloc(_DeviceList5Update,0*sizeof(char));
		_OldDeviceListUpdate=(char*)realloc(_OldDeviceListUpdate,0*sizeof(char));

		_deviceList_nullvalue5Update=(int*)realloc(_deviceList_nullvalue5Update,0*sizeof(int));
		_OlddeviceList_nullvalueUpdate=(int*)realloc(_OlddeviceList_nullvalueUpdate,0*sizeof(int));

		_deviceList_nullvalue5Update=NULL;
		_OlddeviceList_nullvalueUpdate=NULL;
		_DeviceList5Update=NULL;
		_OldDeviceListUpdate=NULL;
		FIRST_TIME_DEVICEUPDATE5=0;

		return status;
	}
	int freeDeviceUpdate6()
	{
		int status=0;
		_DeviceList6Update=(char*)realloc(_DeviceList6Update,0*sizeof(char));

		_DevtypeListUpdate=(char*)realloc(_DevtypeListUpdate,0*sizeof(char));
		_deviceList_nullvalue6Update=(int*)realloc(_deviceList_nullvalue6Update,0*sizeof(int));
		_devtype_nullvalueUpdate=(int*)realloc(_devtype_nullvalueUpdate,0*sizeof(int));
		_devtype_nullvalueUpdate=NULL;
		_deviceList_nullvalue6Update=NULL;
		_DeviceList6Update=NULL;
		_DevtypeListUpdate=NULL;
		FIRST_TIME_DEVICEUPDATE6=0;

		return status;
	}
	int freeActiveDeviceUpdate()
	{
		int status=0;
		_ActiveDeviceListUpdate=(char*)realloc(_ActiveDeviceListUpdate,0*sizeof(char));
		_ActiveUpdate=(int*)realloc(_ActiveUpdate,0*sizeof(int));
		_ActivedeviceList_nullvalueUpdate=(int*)realloc(_ActivedeviceList_nullvalueUpdate,0*sizeof(int));
		FIRST_TIME_ACTIVEUPDATE=0;

		return status;
	}

	int freeStatusDeviceUpdate()
	{
		int status=0;
		_StatusDeviceListUpdate=(char*)realloc(_StatusDeviceListUpdate,0*sizeof(char));
		_StatusUpdate=(int*)realloc(_StatusUpdate,0*sizeof(int));
		_StatusdeviceList_nullvalueUpdate=(int*)realloc(_StatusdeviceList_nullvalueUpdate,0*sizeof(int));
		FIRST_TIME_STATUSUPDATE=0;
		return status;
	}

	int freeNodeUsedDeviceUpdate()
	{
		int status=0;
		_NodeUsedDeviceListUpdate=(char*)realloc(_NodeUsedDeviceListUpdate,0*sizeof(char));
		_NodeUsedUpdate=(int*)realloc(_NodeUsedUpdate,0*sizeof(int));
		_NodeUseddeviceList_nullvalueUpdate=(int*)realloc(_NodeUseddeviceList_nullvalueUpdate,0*sizeof(int));
		FIRST_TIME_NODEUSEDUPDATE=0;

		return status;
	}

	int freeSysDeviceUpdate()
	{
		int status=0;
		_SysDeviceListUpdate=(char*)realloc(_SysDeviceListUpdate,0*sizeof(char));
		_SysUpdate=(int*)realloc(_SysUpdate,0*sizeof(int));
		_SysdeviceList_nullvalueUpdate=(int*)realloc(_SysdeviceList_nullvalueUpdate,0*sizeof(int));
		FIRST_TIME_SYSUPDATE=0;

		return status;
	}
	/**
	* Update a device  returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* @param devicename : new value of the devicename, if no change you put  none (lower case)
	* @param node : new value of node 1 if it's a host node (usually starts or ends a path such as L1FEs) , 0 otherwise.Put -1 if thereis no change
	* @param promiscuous_mode : new value of prom.mode 1 if put in this mode (default mode), 0 otherwise. Put -1 not to change the value
	* @param location : new value of the location of the device if no changes you put none (lower case)
	* @param function_list : new value of the functions of the device. If several, separate them with a coma. Put "none", if it doesn't have any.
	* @param first_time1 : 1 if it's your first MultipleDevice update (save it into the database), 0 otherwise
	* @param last_rows1 : 1 if it's your MultipleDevice last update (save it into the database), 0 otherwise
	* @param ErrMess : error message in case of failure
	* @return 0 if it is successful
	*/

	/******************************************************************/
	EXTERN_CONFDB
		int UpdateMultipleAttributesDevices(char* devicename,int node,int promiscuous_mode,char* location,char* function_list,int first_time1,int last_rows1,char* ErrMess)
	{
		char appliName[100]="UpdateMultipleAttributesDevices";
		char sqlstmt[1000];
		OCIStmt *hstmt;
		OCIBind  *bndp[5]; 
		int rescode=0;
		int free_mem=0;
		char res_query1[100]="blabal";
		int res_query=0;
		int first_time=first_time1;
		int last_rows=last_rows1;
		int force_insert=0;
		sword status=0;
		char* dname=NULL;
		int i=0;
		char* dlocation=NULL;
		char* dfunctiondev=NULL;
		int* numrows_inserted=NULL;
		int numrows=0;
		int need_to_update=0;
		OCIBind  *bndp3 = (OCIBind *) 0;

		int dname1=null_charptr(devicename);
		int dlocation1=null_charptr(location);
		int dfunctiondev1=null_charptr(function_list);
		if(first_time==1)
		{

			status+=freeDeviceUpdate();
			FIRST_TIME_DEVICEUPDATE=1;
		}
		if(FIRST_TIME_DEVICEUPDATE==1 && _DeviceListUpdate==NULL)
			first_time=1;
		res_query=AppendString(devicename,_DeviceListUpdate,_deviceListLengthUpdate,_deviceNbElementUpdate,_max_deviceLengthUpdate,first_time);
		res_query+=AppendString(location,_locationListUpdate,_locationListLengthUpdate,_locationNbElementUpdate,_max_location_lengthUpdate,first_time);
		res_query+=AppendString(function_list,_devfunctionListUpdate,_devfunctionListLengthUpdate,_devfunctionNbElementUpdate,_max_devfunction_lengthUpdate,first_time);

		res_query+=AppendInt(node,_NodeUpdate,_nodeNbElementUpdate,first_time);
		res_query+=AppendInt(promiscuous_mode,_Promiscuous_modeUpdate,_promiscuous_modeNbElementUpdate,first_time);

		res_query+=AppendInt(dname1,_deviceList_nullvalueUpdate,_deviceListNbElUpdate,first_time);
		res_query+=AppendInt(dlocation1,_location_nullvalueUpdate,_locationNbElUpdate,first_time);




		if(last_rows!=1 && _deviceNbElementUpdate==MAXROWS)
		{
			last_rows=1;
			force_insert=1;
		}

		status=res_query;
		if(last_rows==1 && res_query==0 && ociEnv!=0 && FIRST_TIME_DEVICEUPDATE==1)
		{
			int len_host=LOGIN_LEN;
			char login[LOGIN_LEN];
			char host[LOGIN_LEN];
			char* errmessg1=(char*)malloc(ERROR_BUFFER*sizeof(char));
			if(errmessg1==NULL)
			{
				GetErrorMess(appliName, "Malloc unsuccessful 1",ErrMess,1);
				return -1;
			}
			char* errmessg2=(char*)malloc(ERROR_BUFFER*sizeof(char));
			if(errmessg2==NULL)
			{
				free(errmessg1);
				GetErrorMess(appliName, "Malloc unsuccessful 2",ErrMess,1);
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
				status+=freeDeviceUpdate();
				if(dname!=NULL)
					free(dname);
				if(dlocation!=NULL)
					free(dlocation);


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
					"BEGIN update %s set node=decode(:node,1,1,0,0,node),promiscuous_mode=decode(:pmode,1,1,0,0,promiscuous_mode),location=decode(nvl(:location,'none'),'none',location,:location),functionID=decode(:fctlist,'none',0,%s(:fctlist)),last_update=sysdate,user_update='%s' where devicename=:dname; :numrows:=%s; END;",LOGICAL_DEVICE_TABLE,_ComposeFctID,login,SQLROWCOUNT);
				status=OCIStmtPrepare(hstmt, ociError, (text*)sqlstmt, (ub4)strlen((char *)sqlstmt), (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
			}

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
			}
			else
			{
				dname=(char*)malloc( _deviceNbElementUpdate*_max_deviceLengthUpdate*sizeof(char));
				if(dname!=NULL)
					status=NormalizeVector(_DeviceListUpdate, _deviceNbElementUpdate,_max_deviceLengthUpdate,dname);
				dlocation=(char*)malloc( _locationNbElementUpdate*_max_location_lengthUpdate*sizeof(char));
				if(dlocation!=NULL)
					status+=NormalizeVector(_locationListUpdate, _locationNbElementUpdate,_max_location_lengthUpdate,dlocation);
				dfunctiondev=(char*)malloc( _devfunctionNbElementUpdate*_max_devfunction_lengthUpdate*sizeof(char));
				if(dfunctiondev!=NULL)
					status+=NormalizeVector(_devfunctionListUpdate, _devfunctionNbElementUpdate,_max_devfunction_lengthUpdate,dfunctiondev);

				free_mem=1;
				numrows_inserted=(int*)malloc(sizeof(int)*_deviceNbElementUpdate);

				if(dname==NULL ||dlocation==NULL ||dfunctiondev==NULL||numrows_inserted==NULL)
				{

					rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");
					GetErrorMess(appliName, "Malloc unsuccessful 3",ErrMess,1);
					status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
					status+=freeDeviceUpdate();
					if(dname!=NULL)
						free(dname);
					if(dlocation!=NULL)
						free(dlocation);
					if(dfunctiondev!=NULL)
						free(dfunctiondev);
					if(numrows_inserted!=NULL)
						free(numrows_inserted);
					return -1;
				}
				else
				{
					for(i=0;i<_deviceNbElementUpdate;i++)
						numrows_inserted[i]=0;
					status =OCIBindByName(hstmt, &bndp[0], ociError,(text*) ":dname", -1,(dvoid*)dname, _max_deviceLengthUpdate,  SQLT_STR, (dvoid *) & _deviceList_nullvalueUpdate[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
				}
			}
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp[1], ociError,(text*) ":node", -1,(dvoid*)&_NodeUpdate[0], sizeof(node),  SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp[2], ociError,(text*) ":pmode", -1,(dvoid*)&_Promiscuous_modeUpdate[0], sizeof(promiscuous_mode),  SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp[3], ociError,(text*) ":location", -1,(dvoid*)dlocation, _max_location_lengthUpdate,  SQLT_STR, (dvoid *) & _location_nullvalueUpdate[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp[4], ociError,(text*) ":fctlist", -1,(dvoid*)dfunctiondev, _max_devfunction_lengthUpdate,  SQLT_STR, (dvoid *) & _devfunction_nullvalueUpdate[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);


			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp3, ociError,(text*) ":numrows", -1,(dvoid*)&numrows_inserted[0], sizeof(node),SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);


			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp[0], ociError,_max_deviceLengthUpdate, sizeof(int),0, 0);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp[1], ociError, sizeof(int), 0, 0, 0);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp[2], ociError,  sizeof(int), 0, 0, 0);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp[3], ociError, _max_location_lengthUpdate, sizeof(int), 0, 0);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp[4], ociError, _max_devfunction_lengthUpdate, sizeof(int), 0, 0);


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
				status= OCIStmtExecute(ociHdbc, hstmt, ociError, _deviceNbElementUpdate, 0, 0, 0, OCI_DEFAULT );
			//std:://cout<<"after execute 1="<<std::endl;
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
			}
			else
			{
				numrows=-1;
				for(i=0;i< _deviceNbElementUpdate;i++)
				{
					if(numrows_inserted[i]==0)
					{
						numrows=0;
						i= _deviceNbElementUpdate+5;
					}
					else
						numrows=numrows_inserted[i];
				}
				if(numrows==0)
				{
					status = OCITransCommit(ociHdbc, ociError, 0);
					status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
					status+=freeDeviceUpdate();
					if(dname!=NULL)
						free(dname);
					if(dlocation!=NULL)
						free(dlocation);
					if(dfunctiondev!=NULL)
						free(dfunctiondev);
					if(numrows_inserted!=NULL)
						free(numrows_inserted);
					GetErrorMess(appliName, "COULDNOT_UPDATE_ALL_ROWS",ErrMess,1);
					return -1;
				}
				else
				{
					i=0;
					status = OCITransCommit(ociHdbc, ociError, 0);
					//check if we need to update the lk_weight
					while(i<_deviceNbElementUpdate)
					{
						if(_NodeUpdate[i]==0 || _NodeUpdate[i]==1)
						{
							need_to_update=1;
							i=_deviceNbElementUpdate+5;
						}
						else
							i++;
					}
					if(need_to_update==1)
					{
						sprintf(sqlstmt,"BEGIN update %s d set d.link_weight=(select decode(f.node,1,t.node+f.node+1,t.node+f.node) from %s t,%s f, %s l,%s g where d.portidfrom=l.portid and l.deviceid=t.deviceid and d.portidto=g.portid and g.deviceid=f.deviceid),d.last_update=sysdate,d.user_update='%s'; :rescode:=%s(1); :numrows:=%s; END;",MACRO_CONNECTIVITY_TABLE,LOGICAL_DEVICE_TABLE,LOGICAL_DEVICE_TABLE,PORT_TABLE,PORT_TABLE,login,_UpdateDynamicTables,SQLROWCOUNT);
						status=OCIStmtPrepare(hstmt, ociError, (text*)sqlstmt, (ub4)strlen((char *)sqlstmt), (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
						//std:://cout<<"after sqlstmt2="<<sqlstmt<<std::endl;
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
							status =OCIBindByName(hstmt, &bndp[0], ociError,(text*) ":rescode", -1,(dvoid*)res_query1, 101,SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);

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
								rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
						}
						else
							status = OCITransCommit(ociHdbc, ociError, 0);
					}

				}

				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)	
						rescode=ShowErrors (status, ociError, "OCITransCommit unsuccessful");
				}		
			}
			status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
			status+=freeDeviceUpdate();
			//std:://cout<<"after free device"<<std::endl;
			if(dname!=NULL)
				free(dname);
			//std:://cout<<"after free dname 1"<<std::endl;
			if(dlocation!=NULL)
				free(dlocation);
			if(dfunctiondev!=NULL)
				free(dfunctiondev);
			//std:://cout<<"after free dlocation 1"<<std::endl;
			if(numrows_inserted!=NULL)
				free(numrows_inserted);
			//std:://cout<<"after free device 1"<<std::endl;
			if(rescode!=0)
				if(ociError!=0)
					OCIReportError(ociError,appliName, ErrMess,1); 
				else
					GetErrorMess(appliName,  "NOT CONNECTED TO ANY DB",ErrMess,1); 
			else
				OCIReportError(ociError,appliName, ErrMess,0); 

			if(force_insert==1 && rescode==0)
				FIRST_TIME_DEVICEUPDATE=1;
		}
		else
		{
			if(res_query!=0)
			{
				status=freeDeviceUpdate();
				GetErrorMess(appliName, "Cache Problem",ErrMess,1);
			}
			if(FIRST_TIME_DEVICEUPDATE!=1)
			{
				status=freeDeviceUpdate();
				res_query=-1;
				GetErrorMess(appliName, "CACHE HAS NOT BEEN INITIALIZED",ErrMess,1);
			}
			if(ociEnv==0)
			{
				status=freeDeviceUpdate();
				res_query=-1;
				GetErrorMess(appliName, "NOT CONNECTED TO ANY DB",ErrMess,1);
			}
			if(ociEnv!=0 && FIRST_TIME_DEVICEUPDATE==1 && res_query==0)
			{
				status=0;
				GetErrorMess(appliName, "NO_ERROR",ErrMess,0);
			}
			status+=res_query;
		}
		//std:://cout<<"end of fct "<<std::endl;
		return (status+rescode);
	}

	/******************************************************/
	//nodeused either 1 or 0 
	//0 means it won't be take into acount in routing tables and so on
	/**
	* Update the nodeused parameter of a device  returning an integer value.
	* The user should manage the memory : there is no memory allocation.

	* @param devicename : name of the device
	* @param nodeused : 1 if it's used , 0 otherwise.If not equal to 1 or 0 we don't change the field
	* @param first_time1 : 1 if it's your first MultipleDeviceNodeUsed update (save it into the database), 0 otherwise
	* @param last_rows1 : 1 if it's your MultipleDeviceNodeUsed last update (save it into the database), 0 otherwise
	* @param ErrMess : error message in case of failure
	* @return 0 if it is successful
	*/
	/*********************************************************************/
	EXTERN_CONFDB int UpdateMultipleDeviceNodeUsed(char* devicename,int nodeused,int first_time1,int last_rows1,char* ErrMess)
	{

		//extern "C" __declspec(dllexport)
		char appliName[100]="updateMultipleDeviceNodeUsed";
		char sqlstmt[2000];
		OCIStmt *hstmt;
		OCIBind  *bndp[2]; 
		int rescode=0;
		int free_mem=0;
		int res_query=0;
		sword status=0;
		char* dname=NULL;
		int i=0;
		int first_time=first_time1;
		int last_rows=last_rows1;
		int force_insert=0;
		int* numrows_inserted=NULL;
		int* numrows_inserted1=NULL;

		int numrows=0;
		OCIBind  *bndp3 = (OCIBind *) 0;
		OCIBind  *bndp4 = (OCIBind *) 0;
		OCIBind  *bndp5 = (OCIBind *) 0;

		int dname1=null_charptr(devicename);
		if(FIRST_TIME_NODEUSEDUPDATE==1 && _NodeUsedDeviceListUpdate==NULL)
			first_time;
		if(first_time==1)
		{
			freeNodeUsedDeviceUpdate();
			FIRST_TIME_NODEUSEDUPDATE=1;

		}
		res_query=AppendString(devicename,_NodeUsedDeviceListUpdate,_NodeUseddeviceListLengthUpdate,_NodeUseddeviceNbElementUpdate,_max_NodeUseddeviceLengthUpdate,first_time);

		res_query+=AppendInt(nodeused,_NodeUsedUpdate,_NodeUsedNbElementUpdate,first_time);

		res_query+=AppendInt(dname1,_NodeUseddeviceList_nullvalueUpdate,_NodeUseddeviceListNbElUpdate,first_time);


		status=res_query;

		if(last_rows!=1 && _NodeUseddeviceNbElementUpdate==MAXROWS)
		{
			force_insert=1;
			last_rows=1;

		}

		if(last_rows==1 && res_query==0 && ociEnv!=0 && FIRST_TIME_NODEUSEDUPDATE==1 )
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
			int sysID=GetSubsystemID("DAQ,TFC"); //only devices part of these subsystems affect routing and or destina tables
			//std:://cout<<"sysID= "<<sysID<<std::endl;
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
				status+=freeNodeUsedDeviceUpdate();
				if(dname!=NULL)
					free(dname);


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
				//sprintf(sqlstmt,"BEGIN update %s set nodeused=decode(:nused,1,1,0,0,nodeused),last_update=sysdate,user_update='%s' where devicename=:dname; if :numrows1=1 then update %s e set e.last_update=sysdate,e.user_update='%s',e.lkused=(select t.nodeused*f.nodeused from %s t,%s f, %s l,%s m where t.deviceid=l.deviceid and l.portid=e.portidfrom and f.deviceid=m.deviceid and m.portid=e.portidto); end if; :numrows:=%s; END;",LOGICAL_DEVICE_TABLE,login,MACRO_CONNECTIVITY_TABLE,login,LOGICAL_DEVICE_TABLE,LOGICAL_DEVICE_TABLE,PORT_TABLE,PORT_TABLE,SQLROWCOUNT);
				sprintf(sqlstmt,"BEGIN update %s set nodeused=decode(:nused,1,1,0,0,nodeused),last_update=sysdate,user_update='%s' where devicename=:dname ; if :numrows1=1 then update %s e set e.last_update=sysdate,e.user_update='%s',e.lkused=(select t.nodeused*f.nodeused from %s t,%s f, %s l,%s m where t.deviceid=l.deviceid and l.portid=e.portidfrom and f.deviceid=m.deviceid and m.portid=e.portidto); :rescode:=%s(%d); end if; :numrows:=%s; END;",LOGICAL_DEVICE_TABLE,login,MACRO_CONNECTIVITY_TABLE,login,LOGICAL_DEVICE_TABLE,LOGICAL_DEVICE_TABLE,PORT_TABLE,PORT_TABLE,_UpdatePathUsed,sysID,SQLROWCOUNT);
				status=OCIStmtPrepare(hstmt, ociError, (text*)sqlstmt, (ub4)strlen((char *)sqlstmt), (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
				//std:://cout<<"sql stmt "<<sqlstmt<<std::endl;

			}
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
			}
			else
			{
				dname=(char*)malloc(_NodeUseddeviceNbElementUpdate*_max_NodeUseddeviceLengthUpdate*sizeof(char));
				if(dname!=NULL)
					status+=NormalizeVector(_NodeUsedDeviceListUpdate, _NodeUseddeviceNbElementUpdate,_max_NodeUseddeviceLengthUpdate,dname);

				numrows_inserted=(int*)malloc(sizeof(int)*_NodeUseddeviceNbElementUpdate);
				numrows_inserted1=(int*)malloc(sizeof(int)*_NodeUseddeviceNbElementUpdate);
				if(dname==NULL ||numrows_inserted==NULL ||numrows_inserted1==NULL)
				{

					rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");
					GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
					status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
					status+=freeNodeUsedDeviceUpdate();
					if(dname!=NULL)
						free(dname);

					if(numrows_inserted!=NULL)
						free(numrows_inserted);
					if(numrows_inserted1!=NULL)
						free(numrows_inserted1);

					return -1;
				}
				else
				{
					for(i=0;i<_NodeUseddeviceNbElementUpdate;i++)
					{
						numrows_inserted[i]=0;
						numrows_inserted1[i]=0;

					}
					numrows_inserted1[_NodeUseddeviceNbElementUpdate-1]=1;
				}
				status =OCIBindByName(hstmt, &bndp[0], ociError,(text*) ":dname", -1,(dvoid*)dname, _max_NodeUseddeviceLengthUpdate,  SQLT_STR, (dvoid *) & _NodeUseddeviceList_nullvalueUpdate[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);

			}
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp[1], ociError,(text*) ":nused", -1,(dvoid*)&_NodeUsedUpdate[0], sizeof(nodeused),  SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp3, ociError,(text*) ":numrows", -1,(dvoid*)&numrows_inserted[0], sizeof(nodeused),SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp4, ociError,(text*) ":numrows1", -1,(dvoid*)&numrows_inserted1[0], sizeof(nodeused),SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp5, ociError,(text*) ":rescode", -1,(dvoid*)&res_query, sizeof(nodeused),SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);


			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp[0], ociError,_max_NodeUseddeviceLengthUpdate, sizeof(int),0, 0);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp[1], ociError, sizeof(int), 0, 0, 0);

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
				status=OCIBindArrayOfStruct(bndp5, ociError, sizeof(int),0, 0, 0);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status= OCIStmtExecute(ociHdbc, hstmt, ociError, _NodeUseddeviceNbElementUpdate, 0, 0, 0, OCI_DEFAULT );
			//std:://cout<<"res_query "<<res_query<<std::endl;
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
			}
			else
			{
				numrows=-1;
				for(i=0;i< _NodeUseddeviceNbElementUpdate;i++)
				{
					if(numrows_inserted[i]==0)
					{
						numrows=0;
						i= _NodeUseddeviceNbElementUpdate+5;
					}
					else
						numrows=numrows_inserted[i];
				}
				if(numrows=0)
				{
					status = OCITransCommit(ociHdbc, ociError, 0);
					status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
					status+=freeNodeUsedDeviceUpdate();
					if(dname!=NULL)
						free(dname);

					if(numrows_inserted!=NULL)
						free(numrows_inserted);
					if(numrows_inserted1!=NULL)
						free(numrows_inserted1);

					GetErrorMess(appliName, "COULDNOT_UPDATE_ALL_ROWS",ErrMess,1);
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
			status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
			status+=freeNodeUsedDeviceUpdate();
			//std:://cout<<"after free device"<<std::endl;
			if(dname!=NULL)
				free(dname);

			if(numrows_inserted!=NULL)
				free(numrows_inserted);
			if(numrows_inserted1!=NULL)
				free(numrows_inserted1);

			if(rescode!=0)
			{
				if(ociError!=0)
					OCIReportError(ociError,appliName, ErrMess,1); 
				else
					GetErrorMess(appliName,  "NOT CONNECTED TO ANY DB",ErrMess,1); 
			}
			else
				OCIReportError(ociError,appliName, ErrMess,0); 

			if(force_insert==1 &&rescode==0)
				FIRST_TIME_NODEUSEDUPDATE=1;
		}
		else
		{
			if(res_query!=0)
			{
				status=freeNodeUsedDeviceUpdate();
				GetErrorMess(appliName, "Cache Problem",ErrMess,1);
			}
			if(FIRST_TIME_NODEUSEDUPDATE!=1)
			{
				status=freeNodeUsedDeviceUpdate();
				res_query=-1;
				GetErrorMess(appliName, "CACHE HAS NOT BEEN INITIALIZED",ErrMess,1);
			}
			if(ociEnv==0)
			{
				status=freeNodeUsedDeviceUpdate();
				res_query=-1;
				GetErrorMess(appliName, "NOT CONNECTED TO ANY DB",ErrMess,1);
			}
			if(ociEnv!=0 && FIRST_TIME_NODEUSEDUPDATE==1 && res_query==0)
			{
				status=0;
				GetErrorMess(appliName, "NO_ERROR",ErrMess,0);
			}
			status+=res_query;
		}
		//std:://cout<<"end of fct "<<std::endl;
		return (status+rescode);
	}

	/****************************************************************************************/
	//nodeused either 1 or 0
	//0 means it won't be take into acount in routing tables and so on
	/**
	* Update the nodeused field for a devicetype or a set of devicetype whose name begins with  returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* @param devicetype : name of the devicetype or (the prefix : all devices which begin with devicetype)
	* @param nodeused : 1 if it's used , 0 otherwise.If not equal to 1 or 0 we don't change the field.
	* @param devtype_given : 1 if you give a devicetype, 0 otherwise
	* @param last_rows : 1 if you want to commit, 0 otherwise (in that case we just insert in the DB)
	* @param ErrMess : error message in case of failure
	* @return 0 if it is successful
	*/
	/************************************************************************/
	EXTERN_CONFDB int UpdateMultipleDeviceNodeUsedByType(char* devicetype,int nodeused,int devtype_given,int last_rows,char* ErrMess)
	{
		char appliName[100]="updateMultipleDeviceNodeUsedByType";
		char sqlstmt[1000];
		OCIStmt *hstmt;
		OCIBind  *bndp[3]; 
		int rescode=0;
		int free_mem=0;
		int res_query=0;
		sword status=0;

		int i=0;

		int numrows_inserted=0;
		int numrows=0;
		int sysID=GetSubsystemID("DAQ,TFC");;
		OCIBind  *bndp3 = (OCIBind *) 0;

		int dname1=-1;
		if(devicetype!=NULL)
			dname1=0;



		if(ociEnv!=0)
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
				if(devtype_given==1)
				{
					sprintf(sqlstmt,
						"BEGIN update %s set nodeused=decode(:nused,1,1,0,0,nodeused),last_update=sysdate,user_update='%s' where devicetypeid=(select t.devicetypeid from %s t where t.devicetype=:dname) returning system_name into :sysID; update %s e set e.last_update=sysdate,e.user_update='%s',e.lkused=(select t.nodeused*f.nodeused from %s t,%s f, %s l,%s m where t.deviceid=l.deviceid and l.portid=e.portidfrom and f.deviceid=m.deviceid and m.portid=e.portidto); :rescode:=%s(:sysID);  :numrows:=%s; END;",LOGICAL_DEVICE_TABLE,login,DEVICETYPE_TABLE,MACRO_CONNECTIVITY_TABLE,login,LOGICAL_DEVICE_TABLE,LOGICAL_DEVICE_TABLE,PORT_TABLE,PORT_TABLE,_UpdatePathUsed,SQLROWCOUNT);
					status=OCIStmtPrepare(hstmt, ociError, (text*)sqlstmt, (ub4)strlen((char *)sqlstmt), (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);

					if(status!=OCI_SUCCESS)
					{
						if(rescode==0)	
							rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
					}
					else
						status =OCIBindByName(hstmt, &bndp3, ociError,(text*) ":dname", -1,(dvoid*)devicetype, strlen(devicetype)+1,SQLT_STR, (dvoid *) &dname1,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);



				}
				else
				{
					sprintf(sqlstmt,
						"BEGIN update %s set nodeused=decode(:nused,1,1,0,0,nodeused),last_update=sysdate,user_update='%s' where devicename like '%s%%'; update %s e set e.last_update=sysdate,e.user_update='%s',e.lkused=(select t.nodeused*f.nodeused from %s t,%s f, %s l,%s m where t.deviceid=l.deviceid and l.portid=e.portidfrom and f.deviceid=m.deviceid and m.portid=e.portidto); :rescode:=%s(%d);  :numrows:=%s; END;",LOGICAL_DEVICE_TABLE,login,devicetype,MACRO_CONNECTIVITY_TABLE,login,LOGICAL_DEVICE_TABLE,LOGICAL_DEVICE_TABLE,PORT_TABLE,PORT_TABLE,_UpdatePathUsed,sysID,SQLROWCOUNT);
					status=OCIStmtPrepare(hstmt, ociError, (text*)sqlstmt, (ub4)strlen((char *)sqlstmt), (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
				}

			}
			//std:://cout<<"after binding "<<sqlstmt<<std::endl;
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp[0], ociError,(text*) ":nused", -1,(dvoid*)&nodeused, sizeof(nodeused),  SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp[1], ociError,(text*) ":numrows", -1,(dvoid*)&numrows_inserted, sizeof(nodeused),SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp[2], ociError,(text*) ":rescode", -1,(dvoid*)&res_query, sizeof(nodeused),SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);


			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status= OCIStmtExecute(ociHdbc, hstmt, ociError,1, 0, 0, 0, OCI_DEFAULT );
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
			}

			if(numrows_inserted==0)
			{
				//status = OCITransRollback(ociHdbc, ociError, 0);
				status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
				GetErrorMess(appliName, "COULDNOT_UPDATE_ALL_ROWS",ErrMess,1);
				return -1;
			}
			else
			{
				if(last_rows==1)
					status = OCITransCommit(ociHdbc, ociError, 0);
			}

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCITransCommit unsuccessful");
			}		

			status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
			if(rescode!=0)
			{
				if(ociError!=0)
					OCIReportError(ociError,appliName, ErrMess,1); 
				else
					GetErrorMess(appliName,  "NOT CONNECTED TO ANY DB",ErrMess,1); 
			}
			else
				OCIReportError(ociError,appliName, ErrMess,0); 
		}
		else
		{

			if(ociEnv==0)
			{
				res_query=-1;
				GetErrorMess(appliName, "NOT CONNECTED TO ANY DB",ErrMess,1);
			}
			status+=res_query;
		}
		//std:://cout<<"end of fct "<<std::endl;
		return (status+rescode);
	}
	/***************************************************************************************/
	//active either 1 or 0 it means it's used by sb else:
	/**
	* Update the active field for a devicetype or a set of devicetype whose name begins with  returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* @param devicetype : name of the devicetype or (the prefix : all devices which begin with devicetype)
	* @param active : 1 if it's busy , 0 otherwise (means free).If not equal to 1 or 0 we don't change the field.
	* @param devtype_given : 1 if you give a devicetype, 0 otherwise
	* @param last_rows : 1 if you want to commit, 0 otherwise (in that case we just insert in the DB)
	* @param ErrMess : error message in case of failure
	* @return 0 if it is successful   */
	EXTERN_CONFDB int UpdateMultipleDeviceActiveByType(char* devicetype,int active,int devtype_given,int last_rows,char* ErrMess)
	{
		char appliName[100]="updateMultipleDeviceActiveByType";
		char sqlstmt[1000];
		OCIStmt *hstmt;
		OCIBind  *bndp[2]; 
		int rescode=0;
		int free_mem=0;
		int res_query=0;
		sword status=0;

		int i=0;

		int numrows_inserted=0;
		int numrows=0;
		OCIBind  *bndp3 = (OCIBind *) 0;

		int dname1=-1;
		if(devicetype!=NULL)
			dname1=0;



		if(ociEnv!=0)
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
				if(devtype_given==1)
				{
					sprintf(sqlstmt,
						"BEGIN update %s set active=decode(:nused,1,1,0,0,active),last_update=sysdate,user_update='%s' where devicetypeid=(select devicetypeid from %s where devicetype=:dname); :numrows:=%s; END;",LOGICAL_DEVICE_TABLE,login,DEVICETYPE_TABLE,SQLROWCOUNT);
					status=OCIStmtPrepare(hstmt, ociError, (text*)sqlstmt, (ub4)strlen((char *)sqlstmt), (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);

					if(status!=OCI_SUCCESS)
					{
						if(rescode==0)	
							rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
					}
					else
						status =OCIBindByName(hstmt, &bndp3, ociError,(text*) ":dname", -1,(dvoid*)devicetype, strlen(devicetype)+1,SQLT_STR, (dvoid *) &dname1,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
				}
				else
				{
					sprintf(sqlstmt,
						"BEGIN update %s set active=decode(:nused,1,1,0,0,active),last_update=sysdate,user_update='%s' where devicename like '%s%%'; :numrows:=%s; END;",LOGICAL_DEVICE_TABLE,login,devicetype,SQLROWCOUNT);
					status=OCIStmtPrepare(hstmt, ociError, (text*)sqlstmt, (ub4)strlen((char *)sqlstmt), (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
				}

			}
			//std:://cout<<"after binding "<<sqlstmt<<std::endl;
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp[0], ociError,(text*) ":nused", -1,(dvoid*)&active, sizeof(active),  SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp[1], ociError,(text*) ":numrows", -1,(dvoid*)&numrows_inserted, sizeof(active),SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);



			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status= OCIStmtExecute(ociHdbc, hstmt, ociError,1, 0, 0, 0, OCI_DEFAULT );
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
			}

			if(numrows_inserted==0)
			{
				//status = OCITransRollback(ociHdbc, ociError, 0);
				status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
				GetErrorMess(appliName, "COULDNOT_UPDATE_ALL_ROWS",ErrMess,1);
				return -1;
			}
			else
			{
				if(last_rows==1)
					status = OCITransCommit(ociHdbc, ociError, 0);
			}

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCITransCommit unsuccessful");
			}		

			status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
			if(rescode!=0)
			{
				if(ociError!=0)
					OCIReportError(ociError,appliName, ErrMess,1); 
				else
					GetErrorMess(appliName,  "NOT CONNECTED TO ANY DB",ErrMess,1); 
			}
			else
				OCIReportError(ociError,appliName, ErrMess,0); 
		}
		else
		{

			if(ociEnv==0)
			{
				res_query=-1;
				GetErrorMess(appliName, "NOT CONNECTED TO ANY DB",ErrMess,1);
			}
			status+=res_query;
		}
		//std:://cout<<"end of fct "<<std::endl;
		return (status+rescode);
	}

	/*************************************************************************/
	//active either 1 or 0 
	//0 means it's free
	/**
	* Update the active field of a  device  returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* @param devicename : name of the device
	* @param active :  1 if this node is busy (booked) , 0 otherwise. if not equals to 0 or 1, we don't change the value
	* @param first_time1 : 1 if it's your first MultipleDeviceActive update (save it into the database), 0 otherwise
	* @param last_rows1 : 1 if it's your MultipleDeviceActive last update (save it into the database), 0 otherwise
	* @param ErrMess : error message in case of failure
	* @return 0 if it is successful
	*/
	EXTERN_CONFDB int UpdateMultipleDeviceActive(char* devicename,int active,int first_time1,int last_rows1,char* ErrMess)
	{

		//extern "C" __declspec(dllexport)
		char appliName[100]="updateMultipleDeviceActive";
		char sqlstmt[1000];
		OCIStmt *hstmt;
		OCIBind  *bndp[2]; 
		int rescode=0;
		int free_mem=0;
		int res_query=0;
		sword status=0;
		char* dname=NULL;
		int i=0;
		int first_time=first_time1;
		int last_rows=last_rows1;
		int force_insert=0;
		int* numrows_inserted=NULL;
		int numrows=0;
		OCIBind  *bndp3 = (OCIBind *) 0;
		if(first_time==1)
		{

			status+=freeActiveDeviceUpdate();
			FIRST_TIME_ACTIVEUPDATE=1;
		}

		int dname1=null_charptr(devicename);
		if(FIRST_TIME_ACTIVEUPDATE==1 && _ActiveDeviceListUpdate==NULL)
			first_time=1;

		res_query=AppendString(devicename,_ActiveDeviceListUpdate,_ActivedeviceListLengthUpdate,_ActivedeviceNbElementUpdate,_max_ActivedeviceLengthUpdate,first_time);

		res_query+=AppendInt(active,_ActiveUpdate,_ActiveNbElementUpdate,first_time);

		res_query+=AppendInt(dname1,_ActivedeviceList_nullvalueUpdate,_ActivedeviceListNbElUpdate,first_time);

		status=res_query;


		if(_ActivedeviceNbElementUpdate==MAXROWS && last_rows!=1)
		{
			last_rows=1;
			force_insert=1;
		}
		if(last_rows==1 && res_query==0 && ociEnv!=0 && FIRST_TIME_ACTIVEUPDATE==1)
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
				status+=freeActiveDeviceUpdate();
				if(dname!=NULL)
					free(dname);


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
					"BEGIN update %s set active=decode(:nused,1,1,0,0,active),last_update=sysdate,user_update='%s' where devicename=:dname; :numrows:=%s; END;",LOGICAL_DEVICE_TABLE,login,SQLROWCOUNT);
				status=OCIStmtPrepare(hstmt, ociError, (text*)sqlstmt, (ub4)strlen((char *)sqlstmt), (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
			}
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
			}
			else
			{
				dname=(char*)malloc(_ActivedeviceNbElementUpdate*_max_ActivedeviceLengthUpdate*sizeof(char));
				if(dname!=NULL)
					status+=NormalizeVector(_ActiveDeviceListUpdate, _ActivedeviceNbElementUpdate,_max_ActivedeviceLengthUpdate,dname);
				free_mem=1;
				numrows_inserted=(int*)malloc(sizeof(int)*_ActivedeviceNbElementUpdate);

				if(dname==NULL)
				{

					rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");
					GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
					status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
					status+=freeActiveDeviceUpdate();
					if(dname!=NULL)
						free(dname);

					if(numrows_inserted!=NULL)
						free(numrows_inserted);
					return -1;
				}
				else
				{
					for(i=0;i<_ActiveNbElementUpdate;i++)
						numrows_inserted[i]=0;
					status =OCIBindByName(hstmt, &bndp[0], ociError,(text*) ":dname", -1,(dvoid*)dname, _max_ActivedeviceLengthUpdate,  SQLT_STR, (dvoid *) & _ActivedeviceList_nullvalueUpdate[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
				}
			}
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp[1], ociError,(text*) ":nused", -1,(dvoid*)&_ActiveUpdate[0], sizeof(active),  SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp3, ociError,(text*) ":numrows", -1,(dvoid*)&numrows_inserted[0], sizeof(active),SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);


			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp[0], ociError,_max_ActivedeviceLengthUpdate, sizeof(int),0, 0);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp[1], ociError, sizeof(int), 0, 0, 0);

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
				status= OCIStmtExecute(ociHdbc, hstmt, ociError, _ActivedeviceNbElementUpdate, 0, 0, 0, OCI_DEFAULT );
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
			}
			else
			{
				numrows=-1;
				for(i=0;i< _ActivedeviceNbElementUpdate;i++)
				{
					if(numrows_inserted[i]==0)
					{
						numrows=0;
						i= _ActivedeviceNbElementUpdate+5;
					}
					else
						numrows=numrows_inserted[i];
				}
				if(numrows==0)
				{
					status = OCITransCommit(ociHdbc, ociError, 0);
					status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
					status+=freeActiveDeviceUpdate();
					if(dname!=NULL)
						free(dname);

					if(numrows_inserted!=NULL)
						free(numrows_inserted);
					GetErrorMess(appliName, "COULDNOT_UPDATE_ALL_ROWS",ErrMess,1);
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
			status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
			status+=freeActiveDeviceUpdate();
			//std:://cout<<"after free device"<<std::endl;
			if(dname!=NULL)
				free(dname);

			if(numrows_inserted!=NULL)
				free(numrows_inserted);

			if(rescode!=0)
				if(ociError!=0)
					OCIReportError(ociError,appliName, ErrMess,1); 
				else
					GetErrorMess(appliName,  "NOT CONNECTED TO ANY DB",ErrMess,1); 
			else
				OCIReportError(ociError,appliName, ErrMess,0); 
			if(force_insert==1 && rescode==0)
				FIRST_TIME_ACTIVEUPDATE=1;
		}
		else
		{
			if(res_query!=0)
			{
				status=freeActiveDeviceUpdate();
				res_query=-1;
				GetErrorMess(appliName, "Cache Problem",ErrMess,1);
			}
			if(FIRST_TIME_ACTIVEUPDATE!=1)
			{
				status=freeActiveDeviceUpdate();
				res_query=-1;
				GetErrorMess(appliName, "CACHE HAS NOT BEEN INITIALIZED",ErrMess,1);
			}
			if(ociEnv==0)
			{
				status=freeActiveDeviceUpdate();
				res_query=-1;
				GetErrorMess(appliName, "NOT CONNECTED TO ANY DB",ErrMess,1);
			}
			status+=res_query;
		}
		//std:://cout<<"end of fct "<<std::endl;
		return (status+rescode);
	}

	/****************************************************/
	//there is one board with a serial nb XXX and has log_dev_1 as a functional name (its function)
	//there is one board with a serial nb YYY and has log_dev_2 as a functional name (its function)
	//Swapping two functional devices (log_dev_1 and log_dev_2) means:
	//the board with the serial nb XXX has log_dev_2 as a functional name
	//the board with the serial nb YYY has log_dev_1 as a functional name
	//Connectivity is bound to the functional_device, so after the swapping the board XXX will be connected as the board YYY was connected and vice-versa
	//For the DAQ, we also swap the mac addresses, but not the IP @ because they are bound to the functional device.
	//we also report the swapping into the history table
	/**
	* Swap two devices  returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* @param functional_devicename1 : functional name of the device 1 
	* @param functional_devicename2 :  functional name of the device 2
	* @param comments : put "none" if you want to put the default comment which is "Swapping functional_devicename1 and functional_devicename1", otherwise, it will append your comment to the default.
	* @param ErrMess : error message in case of failure
	* @return 0 if it is successful
	*/
	/*****************************************************/

	EXTERN_CONFDB int SwapTwoDevices(char* functional_devicename1,char* functional_devicename2,char* comments,char* ErrMess)
	{

		char appliName[100]="SwapTwoDevices";
		char sqlstmt[1000];
		OCIStmt *hstmt;
		OCIBind  *bndp[3]; 
		int rescode=0;
		char res_query[200]="blabla";
		int len3=200;
		sword status=0;
		int i=0;
		int numrows=0;
		int len1=0;
		int len2=0;
		OCIBind  *bndp3 = (OCIBind *) 0;

		if(ociEnv!=0)
		{
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
			int len_host=LOGIN_LEN;
			char login[LOGIN_LEN];
			char host[LOGIN_LEN];
			rescode=getTerminalName(host,len_host,errmessg1);
			len_host=LOGIN_LEN;
			rescode=getLoginUser(login,len_host,errmessg2);
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
				return rescode;
			}
			if (functional_devicename1!=NULL)
				len1=strlen(functional_devicename1);
			else
			{
				GetErrorMess(appliName, "functional_devicename1 can't be null",ErrMess,1);
				return -1;
			}
			if (functional_devicename2!=NULL)
				len2=strlen(functional_devicename2);
			else
			{
				GetErrorMess(appliName, "functional_devicename2 can't be null",ErrMess,1);
				return -1;
			}

			status =OCIHandleAlloc (ociEnv, (void**)&hstmt, OCI_HTYPE_STMT , 0, 0);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIHandleAlloc unsuccessful");
			}
			else
			{
				sprintf(sqlstmt,
					"BEGIN :rescode:=%s(:logName1 ,logName2, '%s', :usercomments); :numrows:=%s; END;", _SwapTwoDevices,login,SQLROWCOUNT);
				status=OCIStmtPrepare(hstmt, ociError, (text*)sqlstmt, (ub4)strlen((char *)sqlstmt), (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
			}
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp[0], ociError,(text*) ":logName1", -1,(dvoid*)functional_devicename1,len1+1,  SQLT_STR, (dvoid *)0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp[1], ociError,(text*) ":logName2", -1,(dvoid*)functional_devicename2, len2+1,  SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp[2], ociError,(text*) ":rescode", -1,(dvoid*)res_query, len3+1,  SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);

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
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status= OCIStmtExecute(ociHdbc, hstmt, ociError, _StatusdeviceNbElementUpdate, 0, 0, 0, OCI_DEFAULT );

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
			}
			else
			{
				if(strncmp(res_query,"SUCCESSFUL_SWAPPING",19)!=0)
				{
					status = OCITransRollback(ociHdbc, ociError, 0);
					status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
					GetErrorMess(appliName, res_query,ErrMess,1);
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
			status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
			if(status!=0)
				if(ociError!=0)
					OCIReportError(ociError,appliName, ErrMess,1); 
				else
					GetErrorMess(appliName,  "NOT CONNECTED TO ANY DB",ErrMess,1); 
			else
				OCIReportError(ociError,appliName, ErrMess,0);
		}
		else
		{
			GetErrorMess(appliName,  "NOT CONNECTED TO ANY DB",ErrMess,1);
			return -1;
		}





		//std:://cout<<"end of fct "<<std::endl;
		return (status+rescode);
	}


	/**
	* Update the devicename field of a device (typically when there is a mistyping) returning an integer value.
	* The user should manage the memory : there is no memory allocation.

	* @param devicename_old : name of the device you want to update
	* @param devicename : new value of the devicename. can't be NULL!
	* @param first_time1 : 1 if it's your first MultipleDevice update (save it into the database), 0 otherwise
	* @param last_rows1 : 1 if it's your MultipleDevice last update (save it into the database), 0 otherwise
	* @param ErrMess : error message in case of failure
	* @return 0 if it is successful
	*/

	/******************************************************************/
	EXTERN_CONFDB
		int UpdateMultipleDevNamesDevices(char* devicename_old,char* devicename,int first_time1,int last_rows1,char* ErrMess)
	{
		char appliName[100]="UpdateMultipleDevNamesDevices";
		char sqlstmt[1000];
		OCIStmt *hstmt;
		OCIBind  *bndp[2]; 
		int rescode=0;
		int res_query=0;
		sword status=0;
		char* dname=NULL;
		int i=0;
		char* dname_old=NULL;
		int* numrows_inserted=NULL;
		int first_time=first_time1;
		int last_rows=last_rows1;
		int force_insert=0;
		int numrows=0;
		OCIBind  *bndp3 = (OCIBind *) 0;

		int dname1=null_charptr(devicename);
		int dname1_old=null_charptr(devicename_old);
		if(first_time==1)
		{

			status+=freeDeviceUpdate5();
			FIRST_TIME_DEVICEUPDATE5=1;
		}

		if(FIRST_TIME_DEVICEUPDATE5==1 && _OldDeviceListUpdate==NULL)
			first_time=1;


		res_query=AppendString(devicename_old,_OldDeviceListUpdate,_OlddeviceListLengthUpdate,_OlddeviceNbElementUpdate,_max_OlddeviceLengthUpdate,first_time);

		res_query=AppendString(devicename,_DeviceList5Update,_deviceListLength5Update,_deviceNbElement5Update,_max_deviceLength5Update,first_time);

		res_query+=AppendInt(dname1,_deviceList_nullvalue5Update,_deviceListNbEl5Update,first_time);
		res_query+=AppendInt(dname1_old,_OlddeviceList_nullvalueUpdate,_OlddeviceListNbElUpdate,first_time);




		if(last_rows!=1 && _deviceListNbEl5Update==MAXROWS)
		{
			force_insert=1;
			last_rows=1;
		}
		status=res_query;
		if(last_rows==1 && res_query==0 && ociEnv!=0 && FIRST_TIME_DEVICEUPDATE5==1)
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
				status+=freeDeviceUpdate5();
				if(dname!=NULL)
					free(dname);
				if(dname_old!=NULL)
					free(dname_old);

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
					"BEGIN update %s set devicename=decode(nvl(:dname,'none'),'none',devicename,:dname),last_update=sysdate,user_update='%s' where devicename=:dname_old; :numrows:=%s; END;",LOGICAL_DEVICE_TABLE,login,SQLROWCOUNT);
				status=OCIStmtPrepare(hstmt, ociError, (text*)sqlstmt, (ub4)strlen((char *)sqlstmt), (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
				//std:://cout<<"sqlstmt "<<sqlstmt<<std::endl;
			}
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
			}
			else
			{
				dname=(char*)malloc( _deviceNbElement5Update*_max_deviceLength5Update*sizeof(char));
				if(dname!=NULL)
					status=NormalizeVector(_DeviceList5Update, _deviceNbElement5Update,_max_deviceLength5Update,dname);
				dname_old=(char*)malloc(_OlddeviceNbElementUpdate*_max_OlddeviceLengthUpdate*sizeof(char));
				if(dname_old!=NULL)
					status+=NormalizeVector(_OldDeviceListUpdate, _OlddeviceNbElementUpdate,_max_OlddeviceLengthUpdate,dname_old);
				numrows_inserted=(int*)malloc(sizeof(int)*_OlddeviceNbElementUpdate);

				if(dname==NULL ||numrows_inserted==NULL||dname_old==NULL)
				{

					rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");
					GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
					status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
					status+=freeDeviceUpdate5();
					if(dname!=NULL)
						free(dname);
					if(dname_old!=NULL)
						free(dname_old);
					if(numrows_inserted!=NULL)
						free(numrows_inserted);
					return -1;
				}
				else
				{
					for(i=0;i<_deviceNbElement5Update;i++)
						numrows_inserted[i]=0;
					status =OCIBindByName(hstmt, &bndp[0], ociError,(text*) ":dname", -1,(dvoid*)dname, _max_deviceLength5Update,  SQLT_STR, (dvoid *) & _deviceList_nullvalue5Update[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
				}
			}


			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp[1], ociError,(text*) ":dname_old", -1,(dvoid*)dname_old, _max_OlddeviceLengthUpdate,  SQLT_STR, (dvoid *) & _OlddeviceList_nullvalueUpdate[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);

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
				status=OCIBindArrayOfStruct(bndp[0], ociError,_max_deviceLength5Update, sizeof(int),0, 0);


			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp[1], ociError,_max_OlddeviceLengthUpdate, sizeof(int),0, 0);

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
				status= OCIStmtExecute(ociHdbc, hstmt, ociError, _deviceNbElement5Update, 0, 0, 0, OCI_DEFAULT );
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
			}
			else
			{
				numrows=-1;
				//std:://cout<<"value of num "<<numrows_inserted[0]<<"value of deviceNb "<< _deviceNbElement5Update<<std::endl;
				for(i=0;i< _deviceNbElement5Update;i++)
				{
					if(numrows_inserted[i]==0)
					{
						numrows=0;
						i= _deviceNbElement5Update+5;
					}
					else
						numrows=numrows_inserted[i];
				}
				if(numrows==0)
				{
					status = OCITransCommit(ociHdbc, ociError, 0);
					status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
					status+=freeDeviceUpdate5();
					if(dname!=NULL)
						free(dname);

					if(dname_old!=NULL)
						free(dname_old);
					if(numrows_inserted!=NULL)
						free(numrows_inserted);
					GetErrorMess(appliName, "COULDNOT_UPDATE_ALL_ROWS",ErrMess,1);
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
			status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
			status+=freeDeviceUpdate5();
			//std:://cout<<"after free device"<<std::endl;
			if(dname!=NULL)
				free(dname);

			if(dname_old!=NULL)
				free(dname_old);
			if(numrows_inserted!=NULL)
				free(numrows_inserted);

			if(rescode!=0)
				if(ociError!=0)
					OCIReportError(ociError,appliName, ErrMess,1); 
				else
					GetErrorMess(appliName,  "NOT CONNECTED TO ANY DB",ErrMess,1); 
			else
				OCIReportError(ociError,appliName, ErrMess,0); 
			if(force_insert==1 && rescode==0)
				FIRST_TIME_DEVICEUPDATE5=1;
		}
		else
		{
			if(res_query!=0)
			{
				status=freeDeviceUpdate5();
				GetErrorMess(appliName, "Cache Problem",ErrMess,1);
			}
			if(FIRST_TIME_DEVICEUPDATE5!=1)
			{
				status=freeDeviceUpdate5();
				res_query=-1;
				GetErrorMess(appliName, "CACHE HAS NOT BEEN INITIALIZED",ErrMess,1);
			}
			if(ociEnv==0)
			{
				status=freeDeviceUpdate5();
				res_query=-1;
				GetErrorMess(appliName, "NOT CONNECTED TO ANY DB",ErrMess,1);
			}
			if(ociEnv!=0 && FIRST_TIME_DEVICEUPDATE5==1 && res_query==0)
			{
				status=0;
				GetErrorMess(appliName, "NO_ERROR",ErrMess,0);
			}
			status+=res_query;
		}
		//std:://cout<<"end of fct "<<std::endl;
		return (status+rescode);
	}


	/**
	* Update the devicetype field of a device :make sure that the devicetype exists 1  returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* @param devicename : name of the device you want to update
	* @param devicetype : new value of the devicetype, 
	* @param first_time1 : 1 if it's your first MultipleDevice update (save it into the database), 0 otherwise
	* @param last_rows1 : 1 if it's your MultipleDevice last update (save it into the database), 0 otherwise
	* @param ErrMess : error message in case of failure
	* @return 0 if it is successful
	*/

	/******************************************************************/
	EXTERN_CONFDB
		int UpdateMultipleDTypeDevices(char* devicename,char* devicetype,int first_time1,int last_rows1,char* ErrMess)
	{
		char appliName[100]="UpdateMultipleDTypeDevices";
		char sqlstmt[1000];
		OCIStmt *hstmt;
		OCIBind  *bndp[2]; 
		int rescode=0;
		int res_query=0;
		sword status=0;
		char* dname=NULL;
		int i=0;
		int first_time=first_time1;
		int last_rows=last_rows1;
		int force_insert=0;
		char* dtype=NULL;
		int* numrows_inserted=NULL;
		int numrows=0;
		OCIBind  *bndp3 = (OCIBind *) 0;

		int dname1=null_charptr(devicename);
		int dtype1=null_charptr(devicetype);

		if(FIRST_TIME_DEVICEUPDATE6==1 && _DeviceList6Update==NULL)
		{
			first_time=1;

		}
		if(first_time==1)
		{

			status+=freeDeviceUpdate6();
			FIRST_TIME_DEVICEUPDATE6=1;
		}
		res_query=AppendString(devicename,_DeviceList6Update,_deviceListLength6Update,_deviceNbElement6Update,_max_deviceLength6Update,first_time);
		res_query+=AppendString(devicetype,_DevtypeListUpdate,_devtypeListLengthUpdate,_devtypeNbElementUpdate,_max_devtype_lengthUpdate,first_time);


		res_query+=AppendInt(dname1,_deviceList_nullvalue6Update,_deviceListNbEl6Update,first_time);
		res_query+=AppendInt(dtype1,_devtype_nullvalueUpdate,_devtypeNbElUpdate,first_time);




		if(last_rows!=1 && _deviceNbElement6Update==MAXROWS)
		{
			force_insert=1;
			last_rows=1;
		}
		status=res_query;
		if(last_rows==1 && res_query==0 && ociEnv!=0 && FIRST_TIME_DEVICEUPDATE6==1)
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
				status+=freeDeviceUpdate6();
				if(dname!=NULL)
					free(dname);
				if(dtype!=NULL)
					free(dtype);


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
					"BEGIN update %s set devicetypeID=nvl((select t.devicetypeID from %s t where t.devicetype=:dtype),-1), last_update=sysdate,user_update='%s' where devicename=:dname; :numrows:=%s; END;",LOGICAL_DEVICE_TABLE,DEVICETYPE_TABLE,login,SQLROWCOUNT);
				status=OCIStmtPrepare(hstmt, ociError, (text*)sqlstmt, (ub4)strlen((char *)sqlstmt), (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
			}
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
			}
			else
			{
				dname=(char*)malloc( _deviceNbElement6Update*_max_deviceLength6Update*sizeof(char));
				if(dname!=NULL)
					status=NormalizeVector(_DeviceList6Update, _deviceNbElement6Update,_max_deviceLength6Update,dname);
				dtype=(char*)malloc(_devtypeNbElementUpdate*_max_devtype_lengthUpdate*sizeof(char));
				if(dtype!=NULL)
					status+=NormalizeVector(_DevtypeListUpdate, _devtypeNbElementUpdate,_max_devtype_lengthUpdate,dtype);

				numrows_inserted=(int*)malloc(sizeof(int)*_deviceNbElement6Update);

				if(dtype==NULL || dname==NULL|| numrows_inserted==NULL)
				{

					rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");
					GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
					status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
					status+=freeDeviceUpdate6();
					if(dname!=NULL)
						free(dname);
					if(dtype!=NULL)
						free(dtype);

					if(numrows_inserted!=NULL)
						free(numrows_inserted);
					return -1;
				}
				else
				{
					for(i=0;i<_deviceNbElement6Update;i++)
						numrows_inserted[i]=0;
					status =OCIBindByName(hstmt, &bndp[0], ociError,(text*) ":dname", -1,(dvoid*)dname, _max_deviceLength6Update,  SQLT_STR, (dvoid *) & _deviceList_nullvalue6Update[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
				}
			}
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp[1], ociError,(text*) ":dtype", -1,(dvoid*)dtype, _max_devtype_lengthUpdate,  SQLT_STR, (dvoid *) & _devtype_nullvalueUpdate[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);

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
				status=OCIBindArrayOfStruct(bndp[0], ociError,_max_deviceLength6Update, sizeof(int),0, 0);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp[1], ociError,  _max_devtype_lengthUpdate, sizeof(int), 0, 0);

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
				status= OCIStmtExecute(ociHdbc, hstmt, ociError, _deviceNbElement6Update, 0, 0, 0, OCI_DEFAULT );
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
			}
			else
			{
				numrows=-1;
				for(i=0;i< _deviceNbElement6Update;i++)
				{
					if(numrows_inserted[i]==0)
					{
						numrows=0;
						i= _deviceNbElement6Update+5;
					}
					else
						numrows=numrows_inserted[i];
				}
				if(numrows==0)
				{
					status = OCITransCommit(ociHdbc, ociError, 0);
					status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
					status+=freeDeviceUpdate6();
					if(dname!=NULL)
						free(dname);
					if(dtype!=NULL)
						free(dtype);

					if(numrows_inserted!=NULL)
						free(numrows_inserted);
					GetErrorMess(appliName, "COULDNOT_UPDATE_ALL_ROWS",ErrMess,1);
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
			status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
			status+=freeDeviceUpdate6();
			//std:://cout<<"after free device"<<std::endl;
			if(dname!=NULL)
				free(dname);
			if(dtype!=NULL)
				free(dtype);

			if(numrows_inserted!=NULL)
				free(numrows_inserted);

			if(rescode!=0)
				if(ociError!=0)
					OCIReportError(ociError,appliName, ErrMess,1); 
				else
					GetErrorMess(appliName,  "NOT CONNECTED TO ANY DB",ErrMess,1); 
			else
				OCIReportError(ociError,appliName, ErrMess,0); 

			if(rescode==0 && force_insert==1)
				FIRST_TIME_DEVICEUPDATE6=1;
		}
		else
		{
			if(res_query!=0)
			{
				status=freeDeviceUpdate6();
				GetErrorMess(appliName, "Cache Problem",ErrMess,1);
			}
			if(FIRST_TIME_DEVICEUPDATE6!=1)
			{
				status=freeDeviceUpdate6();
				res_query=-1;
				GetErrorMess(appliName, "CACHE HAS NOT BEEN INITIALIZED",ErrMess,1);
			}
			if(ociEnv==0)
			{
				status=freeDeviceUpdate6();
				res_query=-1;
				GetErrorMess(appliName, "NOT CONNECTED TO ANY DB",ErrMess,1);
			}
			if(ociEnv!=0 && FIRST_TIME_DEVICEUPDATE6==1 && res_query==0)
			{
				status=0;
				GetErrorMess(appliName, "NO_ERROR",ErrMess,0);
			}
			status+=res_query;
		}
		//std:://cout<<"end of fct "<<std::endl;
		return (status+rescode);
	}
	/******************************************************/
	//nodeused either 1 or 0 
	//0 means it won't be take into acount in routing tables and so on
	/**
	* Update the nodeused parameter of a device  returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* @param devicename : name of the device
	* @param new_systemList : new list of system, UPPER CASE, separated with a coma
	* @param first_time1 : 1 if it's your first MultipleDeviceNodeUsed update (save it into the database), 0 otherwise
	* @param last_rows1 : 1 if it's your MultipleDeviceNodeUsed last update (save it into the database), 0 otherwise
	* @param ErrMess : error message in case of failure
	* @return 0 if it is successful
	*/
	/*********************************************************************/
	EXTERN_CONFDB int UpdateMultipleDeviceSystemList(char* devicename,char* new_systemList,int first_time1,int last_rows1,char* ErrMess)
	{

		//extern "C" __declspec(dllexport)
		char appliName[100]="UpdateMultipleDeviceSystemList";
		char sqlstmt[2000];
		OCIStmt *hstmt;
		OCIBind  *bndp[2]; 
		int rescode=0;
		int free_mem=0;
		char res_query1[100]="blabla";
		int res_query=0;

		sword status=0;
		char* dname=NULL;
		int i=0;
		int first_time=first_time1;
		int last_rows=last_rows1;
		int force_insert=0;
		int* numrows_inserted=NULL;
		int* numrows_inserted1=NULL;
		int numrows=0;
		OCIBind  *bndp3 = (OCIBind *) 0;
		OCIBind  *bndp4 = (OCIBind *) 0;
		OCIBind  *bndp5 = (OCIBind *) 0;


		int sysID=GetSubsystemID(new_systemList);
		if(sysID==-1)
		{

			GetErrorMess(appliName, "new_systemList is invalid: not inserted in the cache",ErrMess,1);
			return -1;

		}
		int dname1=null_charptr(devicename);
		if(FIRST_TIME_SYSUPDATE==1 && _SysDeviceListUpdate==NULL)
			first_time;
		if(first_time==1)
		{
			freeSysDeviceUpdate();
			FIRST_TIME_SYSUPDATE=1;

		}
		res_query=AppendString(devicename,_SysDeviceListUpdate,_SysdeviceListLengthUpdate,_SysdeviceNbElementUpdate,_max_SysdeviceLengthUpdate,first_time);

		res_query+=AppendInt(sysID,_SysUpdate,_SysNbElementUpdate,first_time);

		res_query+=AppendInt(dname1,_SysdeviceList_nullvalueUpdate,_SysdeviceListNbElUpdate,first_time);


		status=res_query;

		if(last_rows!=1 && _SysdeviceNbElementUpdate==MAXROWS)
		{
			force_insert=1;
			last_rows=1;

		}

		if(last_rows==1 && res_query==0 && ociEnv!=0 && FIRST_TIME_SYSUPDATE==1 )
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
				status+=freeSysDeviceUpdate();
				if(dname!=NULL)
					free(dname);


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
				int sysID=GetSubsystemID("DAQ,TFC");
				//sprintf(sqlstmt,"BEGIN update %s set nodeused=decode(:nused,1,1,0,0,nodeused),last_update=sysdate,user_update='%s' where devicename=:dname; if :numrows1=1 then update %s e set e.last_update=sysdate,e.user_update='%s',e.lkused=(select t.nodeused*f.nodeused from %s t,%s f, %s l,%s m where t.deviceid=l.deviceid and l.portid=e.portidfrom and f.deviceid=m.deviceid and m.portid=e.portidto); end if; :numrows:=%s; END;",LOGICAL_DEVICE_TABLE,login,MACRO_CONNECTIVITY_TABLE,login,LOGICAL_DEVICE_TABLE,LOGICAL_DEVICE_TABLE,PORT_TABLE,PORT_TABLE,SQLROWCOUNT);
				sprintf(sqlstmt,"BEGIN update %s set system_name=:sysid,last_update=sysdate,user_update='%s' where devicename=:dname; if :numrows1=1 then update %s g set g.last_update=sysdate,g.user_update='%s',g.system_name=(select case when ceil(power(10,sum(log(10,t.systemID))))-power(10,sum(log(10,t.systemID)))>0.5  then floor(power(10,sum(log(10,t.systemid)))) else ceil(power(10,sum(log(10,t.systemid)))) end from %s t,%s o,%s d,%s p,%s h where t.systemid not in (select v.systemid from %s v,%s q where mod(q.systemid,v.systemid)=0 and q.systemid!=v.systemid) and mod(o.system_name,t.systemid)=0 and mod(d.system_name,t.systemid)=0 AND g.portidfrom=p.portid and p.deviceid=o.deviceid and g.portidto=h.portid and h.deviceid=d.deviceid ) where g.system_name is not null; :rescode:=%s(%d);end if; :numrows:=%s; END;",LOGICAL_DEVICE_TABLE,login,MACRO_CONNECTIVITY_TABLE,login,SUBSYSTEM_TABLE,LOGICAL_DEVICE_TABLE,LOGICAL_DEVICE_TABLE,PORT_TABLE,PORT_TABLE,SUBSYSTEM_TABLE,SUBSYSTEM_TABLE,_UpdateDynamicTables,sysID,SQLROWCOUNT);
				status=OCIStmtPrepare(hstmt, ociError, (text*)sqlstmt, (ub4)strlen((char *)sqlstmt), (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
				//std:://cout<<"sql stmt "<<sqlstmt<<std::endl;

			}
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
			}
			else
			{
				dname=(char*)malloc(_SysdeviceNbElementUpdate*_max_SysdeviceLengthUpdate*sizeof(char));
				if(dname!=NULL)
					status+=NormalizeVector(_SysDeviceListUpdate, _SysdeviceNbElementUpdate,_max_SysdeviceLengthUpdate,dname);

				numrows_inserted=(int*)malloc(sizeof(int)*_SysdeviceNbElementUpdate);
				numrows_inserted1=(int*)malloc(sizeof(int)*_SysdeviceNbElementUpdate);
				if(dname==NULL ||numrows_inserted==NULL ||numrows_inserted1==NULL)
				{

					rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");
					GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
					status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
					status+=freeSysDeviceUpdate();
					if(dname!=NULL)
						free(dname);

					if(numrows_inserted!=NULL)
						free(numrows_inserted);
					if(numrows_inserted1!=NULL)
						free(numrows_inserted1);
					return -1;
				}
				else
				{
					for(i=0;i<_SysdeviceNbElementUpdate;i++)
					{
						numrows_inserted[i]=0;
						numrows_inserted1[i]=0;
					}
					numrows_inserted1[_SysdeviceNbElementUpdate-1]=1;
				}
				status =OCIBindByName(hstmt, &bndp[0], ociError,(text*) ":dname", -1,(dvoid*)dname, _max_SysdeviceLengthUpdate,  SQLT_STR, (dvoid *) & _SysdeviceList_nullvalueUpdate[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);

			}
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp[1], ociError,(text*) ":sysid", -1,(dvoid*)&_SysUpdate[0], sizeof(int),  SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);

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
				status =OCIBindByName(hstmt, &bndp4, ociError,(text*) ":numrows1", -1,(dvoid*)&numrows_inserted1[0], sizeof(int),SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp5, ociError,(text*) ":rescode", -1,(dvoid*)res_query1, 101,SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);


			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp[0], ociError,_max_SysdeviceLengthUpdate, sizeof(int),0, 0);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp[1], ociError, sizeof(int), 0, 0, 0);

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
				status=OCIBindArrayOfStruct(bndp5, ociError, sizeof(int),0, 0, 0);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status= OCIStmtExecute(ociHdbc, hstmt, ociError, _SysdeviceNbElementUpdate, 0, 0, 0, OCI_DEFAULT );
			//std:://cout<<"res_query "<<res_query<<std::endl;
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
			}
			else
			{
				numrows=-1;
				for(i=0;i< _SysdeviceNbElementUpdate;i++)
				{
					if(numrows_inserted[i]==0)
					{
						numrows=0;
						i= _SysdeviceNbElementUpdate+5;
					}
					else
						numrows=numrows_inserted[i];
				}
				if(res_query!=0)
				{
					status = OCITransRollback(ociHdbc, ociError, 0);
					status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
					status+=freeSysDeviceUpdate();
					if(dname!=NULL)
						free(dname);

					if(numrows_inserted!=NULL)
						free(numrows_inserted);
					if(numrows_inserted1!=NULL)
						free(numrows_inserted1);
					GetErrorMess(appliName, "COULDNOT_UPDATE_ALL_ROWS",ErrMess,1);
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
			status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
			status+=freeSysDeviceUpdate();
			//std:://cout<<"after free device"<<std::endl;
			if(dname!=NULL)
				free(dname);

			if(numrows_inserted!=NULL)
				free(numrows_inserted);
			if(numrows_inserted1!=NULL)
				free(numrows_inserted1);
			if(rescode!=0)
			{
				if(ociError!=0)
					OCIReportError(ociError,appliName, ErrMess,1); 
				else
					GetErrorMess(appliName,  "NOT CONNECTED TO ANY DB",ErrMess,1); 
			}
			else
				OCIReportError(ociError,appliName, ErrMess,0); 

			if(force_insert==1 &&rescode==0)
				FIRST_TIME_SYSUPDATE=1;
		}
		else
		{
			if(res_query!=0)
			{
				status=freeSysDeviceUpdate();
				GetErrorMess(appliName, "Cache Problem",ErrMess,1);
			}
			if(FIRST_TIME_SYSUPDATE!=1)
			{
				status=freeSysDeviceUpdate();
				res_query=-1;
				GetErrorMess(appliName, "CACHE HAS NOT BEEN INITIALIZED",ErrMess,1);
			}
			if(ociEnv==0)
			{
				status=freeSysDeviceUpdate();
				res_query=-1;
				GetErrorMess(appliName, "NOT CONNECTED TO ANY DB",ErrMess,1);
			}
			if(ociEnv!=0 && FIRST_TIME_SYSUPDATE==1 && res_query==0)
			{
				status=0;
				GetErrorMess(appliName, "NO_ERROR",ErrMess,0);
			}
			status+=res_query;
		}
		//std:://cout<<"end of fct "<<std::endl;
		return (status+rescode);
	}


	/******************************************************/
	/**
	* Update the name of the function, typically when there was a mistyping
	* The user should manage the memory : there is no memory allocation.
	* @param  old_function_name : name of the old function
	* @param new_function_name : name of the new function
	* @param ErrMess : error message in case of failure
	* @return 0 if it is successful
	*/
	/*********************************************************************/
	EXTERN_CONFDB int UpdateDeviceFunctionName(char* old_function_name,char* new_function_name,char* ErrMess)
	{

		//extern "C" __declspec(dllexport)
		char appliName[100]="UpdateDeviceFunctionName";
		char sqlstmt[2000];
		OCIStmt *hstmt;
		OCIBind  *bndp[2]; 
		int rescode=0;
		int free_mem=0;
		char res_query1[100]="blabla";
		int res_query=0;

		sword status=0;
		int i=0;
		int numrows_inserted;
		int numrows=0;
		OCIBind  *bndp3 = (OCIBind *) 0;




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

			//sprintf(sqlstmt,"BEGIN update %s set nodeused=decode(:nused,1,1,0,0,nodeused),last_update=sysdate,user_update='%s' where devicename=:dname; if :numrows1=1 then update %s e set e.last_update=sysdate,e.user_update='%s',e.lkused=(select t.nodeused*f.nodeused from %s t,%s f, %s l,%s m where t.deviceid=l.deviceid and l.portid=e.portidfrom and f.deviceid=m.deviceid and m.portid=e.portidto); end if; :numrows:=%s; END;",LOGICAL_DEVICE_TABLE,login,MACRO_CONNECTIVITY_TABLE,login,LOGICAL_DEVICE_TABLE,LOGICAL_DEVICE_TABLE,PORT_TABLE,PORT_TABLE,SQLROWCOUNT);
			sprintf(sqlstmt,"BEGIN update %s set function_name=:new_fct,last_update=sysdate,user_update='%s' where function_name=:old_fct; :numrows:=%s; END;",DEVICE_FUNCTION_TABLE,login,SQLROWCOUNT);
			status=OCIStmtPrepare(hstmt, ociError, (text*)sqlstmt, (ub4)strlen((char *)sqlstmt), (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
			//std:://cout<<"sql stmt "<<sqlstmt<<std::endl;

		}
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else
		{

			status =OCIBindByName(hstmt, &bndp[0], ociError,(text*) ":new_fct", -1,(dvoid*)new_function_name,strlen(new_function_name)+1,  SQLT_STR, (dvoid *)0 ,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);

		}
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else
			status =OCIBindByName(hstmt, &bndp[1], ociError,(text*) ":old_fct", -1,(dvoid*)old_function_name,strlen(old_function_name)+1,  SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else
			status =OCIBindByName(hstmt, &bndp3, ociError,(text*) ":numrows", -1,(dvoid*)&numrows_inserted, sizeof(int),SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);


		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else
			status= OCIStmtExecute(ociHdbc, hstmt, ociError, 1, 0, 0, 0, OCI_DEFAULT );
		//std:://cout<<"res_query "<<res_query<<std::endl;
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
		}
		else
		{
			if (numrows_inserted=0)
			{
				status = OCITransCommit(ociHdbc, ociError, 0);
				status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
				GetErrorMess(appliName, "COULDNOT_UPDATE_ALL_ROWS",ErrMess,1);
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
		status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);

		if(rescode!=0)
		{
			if(ociError!=0)
				OCIReportError(ociError,appliName, ErrMess,1); 
			else
				GetErrorMess(appliName,  "NOT CONNECTED TO ANY DB",ErrMess,1); 
		}
		else
			OCIReportError(ociError,appliName, ErrMess,0); 


		//std:://cout<<"end of fct "<<std::endl;
		return (status+rescode);
	}

	/******************************************************/
	/**
	* Update the serialnb , typically when there was a mistyping
	* The user should manage the memory : there is no memory allocation.
	* @param  old_serialnb : old serialnb
	* @param new_serialnb : new serialnb
	* @param ErrMess : error message in case of failure
	* @return 0 if it is successful
	*/
	/*********************************************************************/
	EXTERN_CONFDB int UpdateHWSerialNB(char* old_serialnb,char* new_serialnb,char* ErrMess)
	{

		//extern "C" __declspec(dllexport)
		char appliName[100]="UpdateHWSerialNB";
		char sqlstmt[2000];
		OCIStmt *hstmt;
		OCIBind  *bndp[3]; 
		int rescode=0;
		int free_mem=0;
		char res_query1[100]="blabla";
		int res_query=0;
		int deviceid=-1;
		sword status=0;
		int i=0;
		int numrows_inserted=0;
		int numrows=0;
		OCIBind  *bndp3 = (OCIBind *) 0;




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

			//sprintf(sqlstmt,"BEGIN update %s set nodeused=decode(:nused,1,1,0,0,nodeused),last_update=sysdate,user_update='%s' where devicename=:dname; if :numrows1=1 then update %s e set e.last_update=sysdate,e.user_update='%s',e.lkused=(select t.nodeused*f.nodeused from %s t,%s f, %s l,%s m where t.deviceid=l.deviceid and l.portid=e.portidfrom and f.deviceid=m.deviceid and m.portid=e.portidto); end if; :numrows:=%s; END;",LOGICAL_DEVICE_TABLE,login,MACRO_CONNECTIVITY_TABLE,login,LOGICAL_DEVICE_TABLE,LOGICAL_DEVICE_TABLE,PORT_TABLE,PORT_TABLE,SQLROWCOUNT);
			sprintf(sqlstmt,"BEGIN update %s set serialnb=NULL where serialnb=:old_snb returning deviceid into :devid;   if %s=1 then :numrows:=%s(:old_snb,:new_snb,:devid); else :numrows:=%s(:old_snb,:new_snb,:devid); end if; end if; END;",LOGICAL_DEVICE_TABLE,SQLROWCOUNT,_UpdateHistory,_UpdateHistory);
			status=OCIStmtPrepare(hstmt, ociError, (text*)sqlstmt, (ub4)strlen((char *)sqlstmt), (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
			//std:://cout<<"sql stmt "<<sqlstmt<<std::endl;

		}
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else
		{

			status =OCIBindByName(hstmt, &bndp[0], ociError,(text*) ":old_snb", -1,(dvoid*)old_serialnb,strlen(old_serialnb)+1,  SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);

		}
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else
			status =OCIBindByName(hstmt, &bndp[1], ociError,(text*) ":new_snb", -1,(dvoid*)new_serialnb,strlen(new_serialnb)+1,  SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else
			status =OCIBindByName(hstmt, &bndp[2], ociError,(text*) ":devid", -1,(dvoid*)&deviceid,sizeof(int),  SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else
			status =OCIBindByName(hstmt, &bndp3, ociError,(text*) ":numrows", -1,(dvoid*)&numrows_inserted, sizeof(int),SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);


		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else
			status= OCIStmtExecute(ociHdbc, hstmt, ociError, 1, 0, 0, 0, OCI_DEFAULT );
		//std:://cout<<"res_query "<<res_query<<std::endl;
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
		}
		else
		{
			std::cout<<"numrows_inserted "<<numrows_inserted<<std::endl;
			std::cout<<"deviceid"<<deviceid<<std::endl;
			if (numrows_inserted==0)
			{
				status = OCITransCommit(ociHdbc, ociError, 0);

			}
			else
			{

				status = OCITransRollback(ociHdbc, ociError, 0);
				status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
				GetErrorMess(appliName, "COULDNOT_UPDATE_ROW",ErrMess,1);
				return -1;
			}

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCITransCommit unsuccessful");
			}		
		}
		status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);

		if(rescode!=0)
		{
			if(ociError!=0)
				OCIReportError(ociError,appliName, ErrMess,1); 
			else
				GetErrorMess(appliName,  "NOT CONNECTED TO ANY DB",ErrMess,1); 
		}
		else
			OCIReportError(ociError,appliName, ErrMess,0); 


		//std:://cout<<"end of fct "<<std::endl;
		return (status+rescode);
	}
#if defined(__cplusplus) || defined(c_plusplus)
}
#endif