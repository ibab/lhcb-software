#include "list_structures.h"
#include "system_info.h"
#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif


#if defined( _WIN32) || defined(WIN32)
#define EXTERN_CONFDB  _declspec(dllexport)
#else
#include <stdlib.h> 
#define EXTERN_CONFDB 
#endif

	extern   OCIError* ociError ;
	extern   OCIEnv* ociEnv ;
	extern   OCISvcCtx* ociHdbc ;
	extern   OCIServer *mysrvhp;
	extern   OCISession *myusrhp; 
	
	/************************************************************************/
	// return the row of the given type in devtype_result
	/***********************************************************************/
	/**
	* Get the row of the given device type in devtype_result,  returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* @param devitype : device type name: it's CASE SENSITIVE.
	* @param len_devtype : length of the devtype_result. If the buffer is too small, we put the size needed here.
	* @param devtype_result :char* buffer where to put the result
	* @param devtype_result : formatted as follows |db_column_name (db_column_type):db_column_value|
	* @param devtype_result : db_column_type is I for int and C for char*
	* @param devtype_result : equals to "NO_ROWS_SELECTED" if no rows selected
	* @param ErrorMessage : error message in case of failure
	* @return 0 if it is successful
	*/
EXTERN_CONFDB
	int GetDeviceTypeRow(char* devitype,int &len_devtype,char* devtype_result,char* ErrorMessage)
	{
		char appliName[100]="GetDeviceTypeRow";
		int rescode=0;
		sword status;
		OCIStmt *stmthp;
		OCIBind  *bnd1p = (OCIBind *) 0; 
		OCIDefine* def[7];
		sb4  descriptionlen=0;
		sb4 devtypelen=0;
		sb4 rgbcolorlen=0;
		OCIParam *parmdp;
		char * devicetype_temp=NULL;
		char * description_temp=NULL;
		char* rgbcolor_temp=NULL;
		int nbrofinput=0;
		int nbrofoutput= 0;
		int dtypeID=0;
		int sysID=0;
		char* sysIDList=NULL;
		if(devitype==NULL)
		{
			GetErrorMess(appliName," devicetype MUST be given",ErrorMessage,1);
			return -1;
		}
		
		try{
			HandleAlloc(ociEnv,&stmthp,ociError,&status);
			
			char selectdevtype[1000];
			sprintf(selectdevtype,"select devicetype||'?',devicetypeID,nbrofinput,nbrofoutput, nvl(description,'none')||'?',nvl(rgbcolor,'none')||'?',nvl(system_name,-1) from %s where devicetype=:dtype ",DEVICETYPE_TABLE);
	
			StmtPrepare(stmthp,ociError, selectdevtype,&status);
			BindByName(stmthp,&bnd1p,ociError,":dtype",devitype,&status);
			StmtExecute(ociHdbc, stmthp, ociError, &status);

			ParamGet(stmthp, ociError, &parmdp, 1, &status);
			AttrGet(parmdp, &devtypelen, ociError, &status);
			ParamGet(stmthp, ociError, &parmdp, 5, &status);
			AttrGet(parmdp, &descriptionlen, ociError, &status);
			ParamGet(stmthp, ociError, &parmdp, 6, &status);
			AttrGet(parmdp, &rgbcolorlen, ociError, &status);

			devicetype_temp = (char *) realloc(devicetype_temp,(devtypelen + 1)*sizeof(char));
			description_temp= (char *) realloc(description_temp,(descriptionlen + 1)*sizeof(char));
			rgbcolor_temp= (char *) realloc(rgbcolor_temp,(rgbcolorlen + 1)*sizeof(char));

			if(devicetype_temp==NULL || description_temp==NULL ||rgbcolor_temp==NULL)
			{
				status=-10;
				rescode=ShowErrors (status, ociError, "  pointer allocation unsuccessful");
				rescode=-1;
				if(devicetype_temp!=NULL)
					free(devicetype_temp);
				if(description_temp!=NULL)
					free(description_temp);
				if(rgbcolor_temp!=NULL)
					free(rgbcolor_temp);
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				GetErrorMess(appliName,"REALLOC UNSUCCESSFUL",ErrorMessage,1);
				return rescode;
			}
			
			DefineByPos(stmthp,def,ociError,1, devicetype_temp,devtypelen+1,&status);							
			DefineByPos(stmthp,def,ociError,2,&dtypeID,&status);				
			DefineByPos(stmthp,def,ociError,3,&nbrofinput,&status);
			DefineByPos(stmthp,def,ociError,4,&nbrofoutput,&status);
			DefineByPos(stmthp,def,ociError,5, description_temp, descriptionlen + 1,&status);				
			DefineByPos(stmthp,def,ociError,6, rgbcolor_temp, descriptionlen + 1,&status);	
			DefineByPos(stmthp,def,ociError,7,&sysID,&status);		
		
		}catch(Error err){
			sprintf(appliName,"GetDeviceTypeRow");	///////
			rescode=ShowErrors (status, err.ociError, err.log);
			
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			else
				GetErrorMess(appliName,err.msg,ErrorMessage,1);
			
			if(strstr(err.msg,"NOT CONNECTED TO ANY DB")!=NULL)
			return -1;
		}

		if(status==OCI_SUCCESS)
			status =OCIStmtFetch2(stmthp,ociError, 1, OCI_FETCH_NEXT,1, OCI_DEFAULT);

		if (status == OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO) 
		{	
			RemoveSeparator(description_temp,"?");
			RemoveSeparator(devicetype_temp, "?");
			RemoveSeparator(rgbcolor_temp,  "?");

			sysIDList=(char*)malloc(200*sizeof(char));
			if(sysIDList!=NULL)
				rescode=DecomposeSysID_bis(sysID, sysIDList);
			else
			{
				GetErrorMess(appliName, "Malloc unsuccessfull",ErrorMessage,1);
				if(devicetype_temp!=NULL)
					free(devicetype_temp);
				if(description_temp!=NULL)
					free(description_temp);
				if(rgbcolor_temp!=NULL)
					free(rgbcolor_temp);
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				return -1;
			}
			sprintf(devtype_result,"|devicetype (C):%s |devicetypeID (I):%d |nbrofinput (I):%d |nbrofoutput (I):%d |description (C):%s |rgbcolor (C):%s |system_name (C):%s |",devicetype_temp,dtypeID,nbrofinput,nbrofoutput,description_temp,rgbcolor_temp,sysIDList);
		} 
		else 
		{
			if(rescode==0)
			{
				rescode=0;
				strcpy(devtype_result,"NO_ROWS_SELECTED");
				GetErrorMess(appliName, "NO_ROWS_SELECTED",ErrorMessage,1);
			}
		}
		if(sysIDList!=NULL)
			free(sysIDList);
		if(devicetype_temp!=NULL)
			free(devicetype_temp);
		if(description_temp!=NULL)
			free(description_temp);
		if(rgbcolor_temp!=NULL)
			free(rgbcolor_temp);
		status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
		rescode+=status;
		if(strstr(ErrorMessage,"NO_ROWS_SELECTED")==NULL)
		{
			if(rescode==0)
			{
				OCIReportError(ociError,appliName,ErrorMessage,0); 
			}
			else
			{
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			}
		}
		return rescode;
	}
/*************************************************************************************/
	//return the device row in device_result
	/*************************************************************************************/
	/**
	* Get the row of the given device name (a device which is used in the macrosc. connect., i.e. a chip won't be found with this fct. in device_result,  returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* @param functionaldeviname : functional device name (corresponds to its function in the system: it's  case sensitive.
	* @param len_device : length of the devtype_result. If the buffer is too small, we put the size needed here.
	* @param device_result :char* buffer where to put the result
	* @param device_result : formatted as follows |db_column_name (db_column_type):db_column_value|
	* @param device_result : db_column_type is I for int and C for char*
	* @param device_result : equals to "NO_ROWS_SELECTED" if no rows selected
	* @param ErrorMessage : error message in case of failure
	* @return 0 if it is successful
	*/
EXTERN_CONFDB
	int GetDeviceRow_devicename(char* functionaldeviname,int &len_device, char* device_result,char* ErrorMessage)
	{
		char appliName[100]="GetDeviceRow_devicename";
		sword status;
		int rescode=0;
		OCIStmt *stmthp;
		OCIBind  *bnd1p = (OCIBind *) 0; 
		OCIDefine* def[11];
		sb4  devtypelen=0;
		sb4 devlen=0;
		sb4  locationlen=0;
		sb4 serialnblen=0;
		sb4 devfunctionlen=0;
		OCIParam *parmdp;
		int node=0;
		int active=0;
		int deviceid=0;
		int promiscuous_mode=0;
		int nodeused=0;
		char * devicename_temp=NULL;
		char * devicetype_temp=NULL;
		char * location_temp=NULL;
		char * serialnb_temp=NULL;
		char * devicefunction_temp=NULL;
		int sysID=0;
		char* sysIDlist=NULL;
		char selectdev[1000];
		if(functionaldeviname==NULL)
		{
			GetErrorMess(appliName,"devicename MUST be given",ErrorMessage,1);
			return -1;
		}	
		try{
			HandleAlloc(ociEnv,&stmthp,ociError,&status);
			sprintf(selectdev,"select t.devicename||'?',e.devicetype||'?',t.active,t.node,t.deviceid,t.promiscuous_mode,nvl(t.location,'none')||'?',t.nodeused,t.serialnb||'?',nvl(t.system_name,-1),%s(t.functionID)||'?' from %s e,%s t where t.devicename=:dname and e.devicetypeID=t.devicetypeID",_DecomposeFctID,DEVICETYPE_TABLE,LOGICAL_DEVICE_TABLE);
			StmtPrepare(stmthp,ociError, selectdev, &status);
			BindByName(stmthp,&bnd1p,ociError,":dname",functionaldeviname,&status);
			StmtExecute(ociHdbc, stmthp, ociError, &status);
			ParamGet(stmthp, ociError, &parmdp, 1, &status);
			AttrGet(parmdp, &devlen, ociError, &status);
			ParamGet(stmthp, ociError, &parmdp, 2, &status);
			AttrGet(parmdp, &devtypelen, ociError, &status);
			ParamGet(stmthp, ociError, &parmdp, 7, &status);
			AttrGet(parmdp, &locationlen, ociError, &status);
			ParamGet(stmthp, ociError, &parmdp, 9, &status);
			AttrGet(parmdp, &serialnblen, ociError, &status);
			ParamGet(stmthp, ociError, &parmdp, 11, &status);
			AttrGet(parmdp, &devfunctionlen, ociError, &status);
		
			devicetype_temp = (char *) realloc(devicetype_temp,(devtypelen + 1)*sizeof(char));
			devicename_temp= (char *) realloc(devicename_temp,(devlen + 1)*sizeof(char));
			location_temp = (char *) realloc(location_temp,(locationlen + 1)*sizeof(char));
			serialnb_temp= (char *) realloc(serialnb_temp,(serialnblen + 1)*sizeof(char));
			devicefunction_temp= (char *) realloc(devicefunction_temp,(devfunctionlen + 1)*sizeof(char));

			if(devicetype_temp==NULL || devicename_temp==NULL || location_temp==NULL || serialnb_temp==NULL||devicefunction_temp==NULL)
			{
				rescode=ShowErrors (status, ociError, "  pointer allocation unsuccessful");
				rescode=-1;
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				if(devicetype_temp!=NULL)
					free(devicetype_temp);
				if(devicename_temp!=NULL)
					free(devicename_temp);
				if(location_temp!=NULL)
					free(location_temp);
				if(serialnb_temp!=NULL)
					free(serialnb_temp);
				if(devicefunction_temp!=NULL)
					free(devicefunction_temp);
				GetErrorMess(appliName,"REALLOC UNSUCCESSFUL",ErrorMessage,1);
				return rescode;
			}
			
			DefineByPos(stmthp,def,ociError,1, devicename_temp,devlen + 1,&status);							
			DefineByPos(stmthp,def,ociError,2, devicetype_temp,devtypelen+1,&status);		
			DefineByPos(stmthp,def,ociError,3,&active,&status);			
			DefineByPos(stmthp,def,ociError,4,&node,&status);
			DefineByPos(stmthp,def,ociError,5,&deviceid,&status);
			DefineByPos(stmthp,def,ociError,6,&promiscuous_mode,&status);
			DefineByPos(stmthp,def,ociError,7, location_temp, locationlen+1,&status);				
			DefineByPos(stmthp,def,ociError,8,&nodeused,&status);
			DefineByPos(stmthp,def,ociError,9, serialnb_temp, serialnblen + 1,&status);	
			DefineByPos(stmthp,def,ociError,10,&sysID,&status);
			DefineByPos(stmthp,def,ociError,11, devicefunction_temp, devfunctionlen+1,&status);				
		
		}catch(Error err){
			sprintf(appliName,"GetDeviceRow_devicename");	///////
			rescode=ShowErrors (status, err.ociError, err.log);
			
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			else
				GetErrorMess(appliName,err.msg,ErrorMessage,1);
			
			if(strstr(err.msg,"NOT CONNECTED TO ANY DB")!=NULL)
			return -1;
		}

		if(status==OCI_SUCCESS)
			status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT,1,OCI_DEFAULT);

		if (status==OCI_SUCCESS || status==OCI_SUCCESS_WITH_INFO) 
		{	
			RemoveSeparator(serialnb_temp,"?");
			RemoveSeparator(devicetype_temp, "?");
			RemoveSeparator(location_temp,"?");
			RemoveSeparator(devicename_temp, "?");
			RemoveSeparator(devicefunction_temp,"?");

			sysIDlist=(char*)malloc(200*sizeof(char));
			if(sysIDlist!=NULL)
				rescode=DecomposeSysID_bis(sysID, sysIDlist);
			else
			{
				GetErrorMess(appliName, "Malloc unsuccessfull",ErrorMessage,1);
				if(devicetype_temp!=NULL)
					free(devicetype_temp);
				if(devicename_temp!=NULL)
					free(devicename_temp);
				if(serialnb_temp!=NULL)
					free(serialnb_temp);
				if(location_temp!=NULL)
					free(location_temp);
				if(devicefunction_temp!=NULL)
					free(devicefunction_temp);
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				return -1;
			}
				sprintf(device_result,"|devicename (C):%s |devicetype (C):%s |active (I):%d |node (I):%d |deviceid  (I):%d |promiscuous_mode (I):%d |location (C):%s |nodeused (I):%d |serialnb  (C):%s |device_function (C):%s |system_name (C):%s |" ,devicename_temp,devicetype_temp,active,node,deviceid,promiscuous_mode,location_temp,nodeused,serialnb_temp,devicefunction_temp,sysIDlist);
		} 
		else 
		{
			if(rescode==0)
			{
				rescode=0;
				strcpy(device_result,"NO_ROWS_SELECTED");
				GetErrorMess(appliName, "NO_ROWS_SELECTED",ErrorMessage,1);
			}
		}

		if(devicetype_temp!=NULL)
			free(devicetype_temp);
		if(devicename_temp!=NULL)
			free(devicename_temp);
		if(location_temp!=NULL)
			free(location_temp);
		if(serialnb_temp!=NULL)
			free(serialnb_temp);
		if(devicefunction_temp!=NULL)
			free(devicefunction_temp);
		if(sysIDlist!=NULL)
			free(sysIDlist);
		status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
		rescode+=status;
		if(strstr(ErrorMessage,"NO_ROWS_SELECTED")==NULL)
		{
			if(rescode==0)
			{
				OCIReportError(ociError,appliName,ErrorMessage,0); 
			}
			else
			{
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			}
		}
		return rescode;
	}
/***************************************************************************************/
	/**
	* Get the row of the given deviceID of the device in device_result ((a device which is used in the macrosc. connect., i.e. a chip won't be found with this fct and a spare device too: so the devices which arein this tables are automatically in state IN_USE (meaning installed)),  returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* @param deviceID : deviceID of the functional device
	* @param len_device : length of the devtype_result. If the buffer is too small, we put the size needed here.
	* @param device_result :char* buffer where to put the result
	* @param device_result : formatted as follows |db_column_name (db_column_type):db_column_value|
	* @param device_result : db_column_type is I for int and C for char*
	* @param device_result : equals to "NO_ROWS_SELECTED" if no rows selected
	* @param ErrorMessage : error message in case of failure
	* @return 0 if it is successful
	*/
EXTERN_CONFDB
	int GetDeviceRow_devid(int deviceID,int &len_device, char* device_result,char* ErrorMessage)
	{
		char appliName[100]="GetDeviceRow_devid";
		int node=0;
		int active=0;
		int deviceid=0;
		int promiscuous_mode=0;
		int nodeused=0;
		int rescode=0;
		sword status;
		OCIStmt *stmthp;
		OCIBind  *bnd1p = (OCIBind *) 0; 
		OCIDefine* def[11];
		sb4  devtypelen=0;
		sb4 devlen=0;
		sb4  locationlen=0;
		sb4 serialnblen=0;
		sb4 functiondevlen=0;
		OCIParam *parmdp;
		char * devicename_temp=NULL;
		char * devicetype_temp=NULL;
		char * serialnb_temp=NULL;
		char * location_temp=NULL;
		char* functiondev_temp=NULL;
		char selectdev[1000];
		int sysID=0;
		char* sysIDlist=NULL;
		
		try{
			HandleAlloc(ociEnv,&stmthp,ociError,&status);
			sprintf(selectdev,"select t.devicename||'?',e.devicetype||'?',t.active,t.node,t.deviceid,t.promiscuous_mode,nvl(t.location,'none')||'?',t.nodeused,t.serialnb,nvl(t.system_name,-1), %s(t.functionid)||'?' from %s t,%s e where t.deviceid=:dID and e.devicetypeID=t.devicetypeID",_DecomposeFctID,LOGICAL_DEVICE_TABLE,DEVICETYPE_TABLE);
			StmtPrepare(stmthp,ociError, selectdev, &status);
			BindByName(stmthp,&bnd1p,ociError,":dID",&deviceID,&status);
			StmtExecute(ociHdbc, stmthp, ociError, &status);
			ParamGet(stmthp, ociError, &parmdp, 1, &status);
			AttrGet(parmdp, &devlen, ociError, &status);
			ParamGet(stmthp, ociError, &parmdp, 2, &status);
			AttrGet(parmdp, &devtypelen, ociError, &status);
			ParamGet(stmthp, ociError, &parmdp, 7, &status);
			AttrGet(parmdp, &locationlen, ociError, &status);
			ParamGet(stmthp, ociError, &parmdp, 9, &status);
			AttrGet(parmdp, &serialnblen, ociError, &status);
			ParamGet(stmthp, ociError, &parmdp, 11, &status);
			AttrGet(parmdp, &functiondevlen, ociError, &status);
			
			devicetype_temp = (char *) realloc(devicetype_temp,(devtypelen + 1)*sizeof(char));
			devicename_temp= (char *) realloc(devicename_temp,(devlen + 1)*sizeof(char));
			location_temp = (char *) realloc(location_temp,(locationlen + 1)*sizeof(char));
			serialnb_temp= (char *) realloc(serialnb_temp,(serialnblen + 1)*sizeof(char));
			functiondev_temp= (char *) realloc(functiondev_temp,(functiondevlen + 1)*sizeof(char));

			if(devicetype_temp==NULL || functiondev_temp==NULL||devicename_temp==NULL || location_temp==NULL || serialnb_temp==NULL)
			{
				status=-10;
				rescode=ShowErrors (status, ociError, "  pointer allocation unsuccessful");
				rescode=-1;
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				if(devicetype_temp!=NULL)
					free(devicetype_temp);
				if(devicename_temp!=NULL)
					free(devicename_temp);
				if(location_temp!=NULL)
					free(location_temp);
				if(serialnb_temp!=NULL)
					free(serialnb_temp);
				if(functiondev_temp!=NULL)
					free(functiondev_temp);
				GetErrorMess(appliName,"REALLOC UNSUCCESSFUL",ErrorMessage,1);
				return rescode;
			}
		
			DefineByPos(stmthp,def,ociError,1, devicename_temp,devlen + 1,&status);							
			DefineByPos(stmthp,def,ociError,2, devicetype_temp,devtypelen+1,&status);		
			DefineByPos(stmthp,def,ociError,3,&active,&status);			
			DefineByPos(stmthp,def,ociError,4,&node,&status);
			DefineByPos(stmthp,def,ociError,5,&deviceid,&status);
			DefineByPos(stmthp,def,ociError,6,&promiscuous_mode,&status);
			DefineByPos(stmthp,def,ociError,7, location_temp, locationlen+1,&status);				
			DefineByPos(stmthp,def,ociError,8,&nodeused,&status);
			DefineByPos(stmthp,def,ociError,9, serialnb_temp, serialnblen + 1,&status);	
			DefineByPos(stmthp,def,ociError,10,&sysID,&status);
			DefineByPos(stmthp,def,ociError,11, functiondev_temp, functiondevlen,&status);					
		
		}catch(Error err){
			sprintf(appliName,"GetDeviceRow_devid");	///////
			rescode=ShowErrors (status, err.ociError, err.log);
			
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			else
				GetErrorMess(appliName,err.msg,ErrorMessage,1);
			
			if(strstr(err.msg,"NOT CONNECTED TO ANY DB")!=NULL)
			return -1;
		}

		if(status==OCI_SUCCESS)
			status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT,1, OCI_DEFAULT);

		if (status==OCI_SUCCESS || status==OCI_SUCCESS_WITH_INFO) 
		{	
			RemoveSeparator(serialnb_temp,"?");
			RemoveSeparator(devicetype_temp, "?");
			RemoveSeparator(location_temp,"?");
			RemoveSeparator(devicename_temp,"?");
			RemoveSeparator(functiondev_temp,"?");

			sysIDlist=(char*)malloc(200*sizeof(char));
			if(sysIDlist!=NULL)
				rescode=DecomposeSysID_bis(sysID, sysIDlist);
			else
			{
				GetErrorMess(appliName, "Malloc unsuccessfull",ErrorMessage,1);
				if(devicetype_temp!=NULL)
					devicetype_temp = (char *) realloc(devicetype_temp,(0)*sizeof(char));
				if(functiondev_temp!=NULL)
					functiondev_temp= (char *) realloc(functiondev_temp,(0)*sizeof(char));
				if(devicename_temp!=NULL)
					devicename_temp= (char *) realloc(devicename_temp,(0)*sizeof(char));
				if(serialnb_temp!=NULL)
					serialnb_temp = (char *) realloc(serialnb_temp,(0)*sizeof(char));
				if(location_temp!=NULL)
					location_temp= (char *) realloc(location_temp,(0)*sizeof(char));
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				return -1;
			}
				sprintf(device_result,"|devicename (C):%s |devicetype (C):%s |active (I):%d |node (I):%d |deviceid  (I):%d |promiscuous_mode (I):%d |location (C):%s |nodeused (I):%d |serialnb  (C):%s |device_function (C):%s |system_name (C):%s |",devicename_temp,devicetype_temp,active,node,deviceid,promiscuous_mode,location_temp,nodeused,serialnb_temp,functiondev_temp,sysIDlist);
		} 
		else 
		{
			if(rescode==0)
			{
				rescode=0;
				strcpy(device_result,"NO_ROWS_SELECTED");
				GetErrorMess(appliName, "NO_ROWS_SELECTED",ErrorMessage,1);
			}
		}

		if(devicetype_temp!=NULL)
			free(devicetype_temp);
		if(devicename_temp!=NULL)
			free(devicename_temp);
		if(serialnb_temp!=NULL)
			free(serialnb_temp);
		if(location_temp!=NULL)
			free(location_temp);
		if(functiondev_temp!=NULL)
			free(functiondev_temp);
		if(sysIDlist!=NULL)
			free(sysIDlist);
		status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
		rescode+=status;
		if(strstr(ErrorMessage,"NO_ROWS_SELECTED")==NULL)
		{
			if(rescode==0)
			{
				OCIReportError(ociError,appliName,ErrorMessage,0); 
			}
			else
			{
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			}
		}
		return rescode;
	}
/**************************************************************************************/
	//return port row in port_row_result
	/**************************************************************************************/

	/**
	* Get the port row in port_row_result, returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* @param portID : portID of a functional device
	* @param len_port : length of the port_row_result. If the buffer is too small, we put the size needed here.
	* @param port_row_result :char* buffer where to put the result
	* @param port_row_result : formatted as follows |db_column_name (db_column_type):db_column_value|
	* @param port_row_result : db_column_type is I for int and C for char*
	* @param port_row_result : equals to "NO_ROWS_SELECTED" if no rows selected
	* @param ErrorMessage : error message in case of failure
	* @return 0 if it is successful
	*/
EXTERN_CONFDB
	int GetPortRow_pid(int portID, int &len_port,char* port_row_result,char* ErrorMessage)
	{
		char appliName[100]="GetPortRow_pid";
		int rescode=0;
		sword status;
		OCIStmt *stmthp;
		OCIBind  *bnd1p = (OCIBind *) 0; 
		OCIDefine* def[12];
		sb4  phylen=0;
		sb4 iplen=0;
		sb4  dnamelen=0;
		sb4 portypelen=0;
		sb4  portnblen=0;
		sb4 maclen=0;
		sb4 bialen=0;
		OCIParam *parmdp;
		int portid=0;
		int admin_status=0; 
		int pxi_booting=0;
		int speed=0;
		int portway=0;
		char * ip_temp=NULL;
		char * phy_temp=NULL;
		char * mac_temp=NULL;
		char * bia_temp=NULL;
		char * dname_temp=NULL;
		char * portype_temp=NULL;
		char * portnb_temp=NULL;
		char selectport[1000];
		
		try{
			HandleAlloc(ociEnv,&stmthp,ociError,&status);
			sprintf(selectport,"select e.portid,t.devicename||'?',e.port_nbr||'?',e.port_way,e.administrative_status,nvl(e.speed,0),nvl(e.port_type,'None')||'?',nvl(e.phy,'None')||'?',nvl(e.pxi_booting,-1),nvl(f.bia,'None')||'?',nvl(f.macaddress,'None')||'?',nvl(e.ipaddress,'None')||'?' from %s e,%s t,%s f where e.portid=:portID and t.deviceid=e.deviceid and f.serialnb=t.serialnb and f.port_nbr=e.port_nbr and nvl(f.port_type,'none')=nvl(e.port_type,'none') and f.port_way=e.port_way",PORT_TABLE,LOGICAL_DEVICE_TABLE,HWPORT_TABLE);
			StmtPrepare(stmthp,ociError, selectport, &status);
			BindByName(stmthp,&bnd1p,ociError,":portID",&portID,&status);
			StmtExecute(ociHdbc, stmthp, ociError, &status);
			ParamGet(stmthp, ociError, &parmdp, 2, &status);
			AttrGet(parmdp, &dnamelen, ociError, &status);
			ParamGet(stmthp, ociError, &parmdp, 3, &status);
			AttrGet(parmdp, &portnblen, ociError, &status);
			ParamGet(stmthp, ociError, &parmdp, 7, &status);
			AttrGet(parmdp, &portypelen, ociError, &status);		
			ParamGet(stmthp, ociError, &parmdp, 8, &status);
			AttrGet(parmdp, &phylen, ociError, &status);
			ParamGet(stmthp, ociError, &parmdp, 10, &status);
			AttrGet(parmdp, &bialen, ociError, &status);
			ParamGet(stmthp, ociError, &parmdp, 11, &status);
			AttrGet(parmdp, &maclen, ociError, &status);
            ParamGet(stmthp, ociError, &parmdp, 12, &status);
			AttrGet(parmdp, &iplen, ociError, &status);
			
			portype_temp = (char *) realloc(portype_temp,(portypelen + 1)*sizeof(char));
			phy_temp= (char *) realloc(phy_temp,(phylen + 1)*sizeof(char));
			portnb_temp = (char *) realloc(portnb_temp,(portnblen + 1)*sizeof(char));
			bia_temp= (char *) realloc(bia_temp,(bialen + 1)*sizeof(char));
			mac_temp = (char *) realloc(mac_temp,(maclen + 1)*sizeof(char));
			ip_temp= (char *) realloc(ip_temp,(iplen + 1)*sizeof(char));
			dname_temp= (char *) realloc(dname_temp,(dnamelen + 1)*sizeof(char));

			if(dname_temp==NULL || phy_temp==NULL ||bia_temp==NULL||mac_temp==NULL|| ip_temp==NULL||portnb_temp==NULL||portype_temp==NULL)
			{
				status=-10;
				rescode=ShowErrors (status, ociError, "  pointer allocation unsuccessful");
				rescode=-1;
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				if(phy_temp!=NULL)
					free(phy_temp);
				if(dname_temp!=NULL)
					free(dname_temp);
				if(portype_temp!=NULL)
					free(portype_temp);
				if(portnb_temp!=NULL)
					free(portnb_temp);
				if(mac_temp!=NULL)
					free(mac_temp);
				if(ip_temp!=NULL)
					free(ip_temp);
				if(bia_temp!=NULL)
					free(bia_temp);

				GetErrorMess(appliName,"REALLOC UNSUCCESSFUL",ErrorMessage,1);
				return rescode;
			}
			DefineByPos(stmthp,def,ociError,1,&portid,&status);			
			DefineByPos(stmthp,def,ociError,2, dname_temp,dnamelen+1,&status);							
			DefineByPos(stmthp,def,ociError,3, portnb_temp,portnblen+1,&status);		
			DefineByPos(stmthp,def,ociError,4,&portway,&status);
			DefineByPos(stmthp,def,ociError,5,&admin_status,&status);
			DefineByPos(stmthp,def,ociError,6,&speed,&status);
			DefineByPos(stmthp,def,ociError,7, portype_temp, portypelen+1,&status);	
			DefineByPos(stmthp,def,ociError,8, phy_temp,phylen+1,&status);		
			DefineByPos(stmthp,def,ociError,9,&pxi_booting,&status);
			DefineByPos(stmthp,def,ociError,10, bia_temp, bialen + 1,&status);	
			DefineByPos(stmthp,def,ociError,11, mac_temp, maclen+1,&status);				
			DefineByPos(stmthp,def,ociError,12, ip_temp, iplen+1,&status);						

		}catch(Error err){
			sprintf(appliName,"GetPortRow_pid");	///////
			rescode=ShowErrors (status, err.ociError, err.log);
			
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			else
				GetErrorMess(appliName,err.msg,ErrorMessage,1);
			
			if(strstr(err.msg,"NOT CONNECTED TO ANY DB")!=NULL)
			return -1;
		}

		if(status==OCI_SUCCESS)
			status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT,1,OCI_DEFAULT);

		if (status==OCI_SUCCESS || status==OCI_SUCCESS_WITH_INFO) 
		{	
			RemoveSeparator(ip_temp,"?");
			RemoveSeparator(phy_temp, "?");
			RemoveSeparator(mac_temp,"?");
			RemoveSeparator(portnb_temp,"?");
			RemoveSeparator(dname_temp,"?");
			RemoveSeparator(bia_temp,"?");
			RemoveSeparator(portype_temp,"?");

			sprintf(port_row_result,"|portid (I):%d |devicename (C):%s |port_nbr (C):%s |port_way (I):%d |administrative_status (I):%d |speed (I):%d |port_type (C):%s |phy (C):%s |pxi_booting (I):%d |bia (C):%s |macaddress (C):%s |ipaddress (C):%s |",portid,dname_temp,portnb_temp,portway,admin_status,speed,portype_temp,phy_temp,pxi_booting,bia_temp,mac_temp,ip_temp);
		} 
		else 
		{
			if(rescode==0)
			{
				sprintf(selectport,"select e.portid,t.devicename||'?',e.port_nbr||'?',e.port_way,e.administrative_status,nvl(e.speed,0),nvl(e.port_type,'None')||'?',nvl(e.phy,'None')||'?',nvl(e.pxi_booting,0),nvl(e.ipaddress,'None')||'?' from %s e,%s t where e.portid=:portID and t.deviceid=e.deviceid  ",PORT_TABLE,LOGICAL_DEVICE_TABLE);
				
				try{
                    StmtPrepare(stmthp,ociError, selectport, &status);
                    BindByName(stmthp,&bnd1p,ociError,":portID",&portID,&status);
                    StmtExecute(ociHdbc, stmthp, ociError, &status);
					ParamGet(stmthp, ociError, &parmdp, 2, &status);
					AttrGet(parmdp, &dnamelen, ociError, &status);
					ParamGet(stmthp, ociError, &parmdp, 3, &status);
					AttrGet(parmdp, &portnblen, ociError, &status);
					ParamGet(stmthp, ociError, &parmdp, 7, &status);
					AttrGet(parmdp, &portypelen, ociError, &status);		
					ParamGet(stmthp, ociError, &parmdp, 8, &status);
					AttrGet(parmdp, &phylen, ociError, &status);
					ParamGet(stmthp, ociError, &parmdp, 10, &status);
					AttrGet(parmdp, &iplen, ociError, &status);
					
					portype_temp = (char *) realloc(portype_temp,(portypelen + 1)*sizeof(char));
					phy_temp= (char *) realloc(phy_temp,(phylen + 1)*sizeof(char));
					portnb_temp = (char *) realloc(portnb_temp,(portnblen + 1)*sizeof(char));
					ip_temp= (char *) realloc(ip_temp,(iplen + 1)*sizeof(char));
					dname_temp= (char *) realloc(dname_temp,(dnamelen + 1)*sizeof(char));

					if(dname_temp==NULL || phy_temp==NULL || ip_temp==NULL||portnb_temp==NULL||portype_temp==NULL)
					{
						status=-10;
						rescode=ShowErrors (status, ociError, "  pointer allocation unsuccessful");
						rescode=-1;
						status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
						if(phy_temp!=NULL)
							free(phy_temp);
						if(dname_temp!=NULL)
							free(dname_temp);
						if(portype_temp!=NULL)
							free(portype_temp);
						if(portnb_temp!=NULL)
							free(portnb_temp);
						if(ip_temp!=NULL)
							free(ip_temp);

						GetErrorMess(appliName,"REALLOC UNSUCCESSFUL",ErrorMessage,1);
						return rescode;
					}
					DefineByPos(stmthp,def,ociError,1,&portid,&status);			
					DefineByPos(stmthp,def,ociError,2, dname_temp,dnamelen+1,&status);							
					DefineByPos(stmthp,def,ociError,3, portnb_temp,portnblen+1,&status);	
					DefineByPos(stmthp,def,ociError,4,&portway,&status);			  
					DefineByPos(stmthp,def,ociError,5,&admin_status,&status);
					DefineByPos(stmthp,def,ociError,6,&speed,&status);
					DefineByPos(stmthp,def,ociError,7, portype_temp, portypelen+1,&status);				
					DefineByPos(stmthp,def,ociError,8, phy_temp,phylen+1,&status);					
					DefineByPos(stmthp,def,ociError,9,&pxi_booting,&status);
					DefineByPos(stmthp,def,ociError,10, ip_temp, iplen+1,&status);						

				}catch(Error err){
					sprintf(appliName,"GetPortRow_pid");	///////
					rescode=ShowErrors (status, err.ociError, err.log);
				
					if(ociError!=0)
						OCIReportError(ociError,appliName,ErrorMessage,1); 
					else
						GetErrorMess(appliName,err.msg,ErrorMessage,1);
				}
				
				if(status==OCI_SUCCESS)

					status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT,1,OCI_DEFAULT);

				if (status==OCI_SUCCESS || status==OCI_SUCCESS_WITH_INFO) 
				{	
					RemoveSeparator(ip_temp,"?");
					RemoveSeparator(phy_temp,"?");
					RemoveSeparator(portnb_temp,"?");
					RemoveSeparator(dname_temp,"?");
					RemoveSeparator(portype_temp,"?");
					
					sprintf(port_row_result,"|portid (I):%d |devicename (C):%s |port_nbr (C):%s |port_way (I):%d |administrative_status (I):%d |speed (I):%d |port_type (C):%s |phy (C):%s |pxi_booting (I):%d |bia (C):none |macaddress (C):none |ipaddress (C):%s |",portid,dname_temp,portnb_temp,portway,admin_status,speed,portype_temp,phy_temp,pxi_booting,ip_temp);
				} 
				else
				{
					if(rescode==0)
					{
						rescode=0;
						strcpy(port_row_result,"NO_ROWS_SELECTED");
						GetErrorMess(appliName,"NO_ROWS_SELECTED",ErrorMessage,1);
					}
				}
			}
		}
		if(phy_temp!=NULL)
			free(phy_temp);
		if(bia_temp!=NULL)
			free(bia_temp);
		if(portnb_temp!=NULL)
			free(portnb_temp);
		if(mac_temp!=NULL)
			free(mac_temp);
		if(ip_temp!=NULL)
			free(ip_temp);
		if(portype_temp!=NULL)
			free(portype_temp);
		if(dname_temp!=NULL)
			free(dname_temp);
		if(strstr(ErrorMessage,"NO_ROWS_SELECTED")==NULL)
		{
			if(rescode==0)
			{
				OCIReportError(ociError,appliName,ErrorMessage,0); 
			}
			else
			{
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			}
		}
		status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
		return (rescode+status);
	}
/**************************************************************************************/
	//return port row in port_row_result
	/**************************************************************************************/
	/**
	* Get the port row in port_row_result, returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* @param devicename : functional name of the device
	* @param port_nb : port nb of the device: it's a char*, because for some system, the port nb is defined with characters and numbers (case of the daq)
	* @param port_way : 1 if it's an input,2 if it's an output 
	* @param port_type : interface type of the port (control,bmc, data used for the DAQ system). put "" if it's null
	* @param len_port : length of the port_row_result. If the buffer is too small, we put the size needed here.
	* @param port_row_result :char* buffer where to put the result
	* @param port_row_result : formatted as follows |db_column_name (db_column_type):db_column_value|
	* @param port_row_result : db_column_type is I for int and C for char*
	* @param port_row_result : equals to "NO_ROWS_SELECTED" if no rows selected
	* @param ErrorMessage : error message in case of failure
	* @return 0 if it is successful
	*/
EXTERN_CONFDB
	int GetPortRow_devname(char* devicename, char* port_nb,int port_way, char* port_type,int &len_port, char* port_row_result,char* ErrorMessage)
	{
		char appliName[100]="GetPortRow_devname";
		int rescode=0;
		sword status;
		OCIStmt *stmthp;
		OCIBind  *bnd1p[4] ; 
		OCIDefine* def[12];
		sb4  phylen=0;
		sb4 iplen=0;
		sb4  dnamelen=0;
		sb4 portypelen=0;
		sb4  portnblen=0;
		sb4 maclen=0;
		sb4 bialen=0;
		OCIParam *parmdp;
		int portid=0;
		int admin_status=0; 
		int pxi_booting=0;
		int speed=0;
		int portway=0;
		char * ip_temp=NULL;
		char * phy_temp=NULL;
		char * mac_temp=NULL;
		char * bia_temp=NULL;
		char * dname_temp=NULL;
		char * portype_temp=NULL;
		char * portnb_temp=NULL;
		char selectport[1000];
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

		try{
			HandleAlloc(ociEnv,&stmthp,ociError,&status);
			sprintf(selectport,"select e.portid,t.devicename||'?',e.port_nbr||'?',e.port_way,e.administrative_status,nvl(e.speed,0),nvl(e.port_type,'none')||'?',nvl(e.phy,'none')||'?',nvl(e.pxi_booting,-1),nvl(f.bia,'none')||'?',nvl(f.macaddress,'none')||'?',nvl(e.ipaddress,'none')||'?' from %s e,%s t , %s f where t.devicename=:dname and e.port_nbr=:portnb and e.port_way=:portway and nvl(e.port_type,'none')=:portype and t.deviceid=e.deviceid and f.serialnb=t.serialnb and nvl(f.port_type,'none')=:portype and f.port_way=:portway and f.port_nbr=:portnb",PORT_TABLE,LOGICAL_DEVICE_TABLE,HWPORT_TABLE);
			StmtPrepare(stmthp,ociError, selectport, &status);
			BindByName(stmthp,&bnd1p[0],ociError,":dname",devicename,&status);
			BindByName(stmthp,&bnd1p[1],ociError,":portnb",port_nb,&status);
			BindByName(stmthp,&bnd1p[2],ociError,":portway",&port_way,&status);
			BindByName(stmthp,&bnd1p[3],ociError,":portype",port_type1,&status);
			StmtExecute(ociHdbc, stmthp, ociError, &status);
			ParamGet(stmthp, ociError, &parmdp, 2, &status);
			AttrGet(parmdp, &dnamelen, ociError, &status);
			ParamGet(stmthp, ociError, &parmdp, 3, &status);
			AttrGet(parmdp, &portnblen, ociError, &status);			
			ParamGet(stmthp, ociError, &parmdp, 7, &status);
			AttrGet(parmdp, &portypelen, ociError, &status);					
			ParamGet(stmthp, ociError, &parmdp, 8, &status);
			AttrGet(parmdp, &phylen, ociError, &status);			
			ParamGet(stmthp, ociError, &parmdp, 10, &status);
			AttrGet(parmdp, &bialen, ociError, &status);			
			ParamGet(stmthp, ociError, &parmdp, 11, &status);
			AttrGet(parmdp, &maclen, ociError, &status);            
			ParamGet(stmthp, ociError, &parmdp, 12, &status);
			AttrGet(parmdp, &iplen, ociError, &status);
			
			portype_temp = (char *) realloc(portype_temp,(portypelen + 1)*sizeof(char));
			phy_temp= (char *) realloc(phy_temp,(phylen + 1)*sizeof(char));
			portnb_temp = (char *) realloc(portnb_temp,(portnblen + 1)*sizeof(char));
			bia_temp= (char *) realloc(bia_temp,(bialen + 1)*sizeof(char));
			mac_temp = (char *) realloc(mac_temp,(maclen + 1)*sizeof(char));
			ip_temp= (char *) realloc(ip_temp,(iplen + 1)*sizeof(char));
			dname_temp= (char *) realloc(dname_temp,(dnamelen + 1)*sizeof(char));

			if(dname_temp==NULL || phy_temp==NULL ||bia_temp==NULL||mac_temp==NULL|| ip_temp==NULL||portnb_temp==NULL||portype_temp==NULL)
			{
				status=-10;
				rescode=ShowErrors (status, ociError, "  pointer allocation unsuccessful");
				rescode=-1;
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				if(phy_temp!=NULL)
					free(phy_temp);
				if(dname_temp!=NULL)
					free(dname_temp);
				if(portype_temp!=NULL)
					free(portype_temp);
				if(portnb_temp!=NULL)
					free(portnb_temp);
				if(mac_temp!=NULL)
					free(mac_temp);
				if(ip_temp!=NULL)
					free(ip_temp);
				if(bia_temp!=NULL)
					free(bia_temp);

				GetErrorMess(appliName,"REALLOC UNSUCCESSFUL",ErrorMessage,1);
				return rescode;
			}						
			DefineByPos(stmthp,def,ociError,1,&portid,&status);						
			DefineByPos(stmthp,def,ociError,2, dname_temp,dnamelen+1,&status);										 
			DefineByPos(stmthp,def,ociError,3, portnb_temp,portnblen+1,&status);					
			DefineByPos(stmthp,def,ociError,4,&portway,&status);
			DefineByPos(stmthp,def,ociError,5,&admin_status,&status);
			DefineByPos(stmthp,def,ociError,6,&speed,&status);
			DefineByPos(stmthp,def,ociError,7, portype_temp, portypelen+1,&status);				
			DefineByPos(stmthp,def,ociError,8, phy_temp,phylen+1,&status);		
			DefineByPos(stmthp,def,ociError,9,&pxi_booting,&status);			
			DefineByPos(stmthp,def,ociError,10, bia_temp, bialen + 1,&status);				
			DefineByPos(stmthp,def,ociError,11, mac_temp, maclen+1,&status);				
			DefineByPos(stmthp,def,ociError,12, ip_temp, iplen+1,&status);						

		}catch(Error err){
			sprintf(appliName,"GetPortRow_devname");	///////
			rescode=ShowErrors (status, err.ociError, err.log);
			
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			else
				GetErrorMess(appliName,err.msg,ErrorMessage,1);
			
			if(strstr(err.msg,"NOT CONNECTED TO ANY DB")!=NULL)
			return -1;
		}

		if(status==OCI_SUCCESS)
			status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT,1,OCI_DEFAULT);
		
		if (status==OCI_SUCCESS || status==OCI_SUCCESS_WITH_INFO) 
		{	
			RemoveSeparator(ip_temp,"?");
			RemoveSeparator(phy_temp,"?");
			RemoveSeparator(mac_temp,"?");
			RemoveSeparator(portnb_temp,"?");
			RemoveSeparator(dname_temp,"?");
			RemoveSeparator(bia_temp,"?");
			RemoveSeparator(portype_temp,"?");

			sprintf(port_row_result,"|portid (I):%d |devicename (C):%s |port_nbr (C):%s |port_way (I):%d |administrative_status (I):%d |speed (I):%d |port_type (C):%s |phy (C):%s |pxi_booting (I):%d |bia (C):%s |macaddress (C):%s |ipaddress (C):%s |",portid,dname_temp,portnb_temp,portway,admin_status,speed,portype_temp,phy_temp,pxi_booting,bia_temp,mac_temp,ip_temp);
		} 
		else 
		{
			if(rescode==0)
			{				
				sprintf(selectport,"select e.portid,t.devicename||'?',e.port_nbr||'?',e.port_way,e.administrative_status,nvl(e.speed,0),nvl(e.port_type,'none')||'?',nvl(e.phy,'none')||'?',nvl(e.pxi_booting,-1),nvl(e.ipaddress,'none')||'?' from %s e,%s t where t.devicename=:dname and e.port_nbr=:portnb and e.port_way=:portway and nvl(e.port_type,'none')=:portype and t.deviceid=e.deviceid",PORT_TABLE,LOGICAL_DEVICE_TABLE);
				
				try{
                    StmtPrepare(stmthp,ociError, selectport, &status);
                    BindByName(stmthp,&bnd1p[0],ociError,":dname",devicename,&status);
					BindByName(stmthp,&bnd1p[1],ociError,":portnb",port_nb,&status);
					BindByName(stmthp,&bnd1p[2],ociError,":portway",&port_way,&status);
					BindByName(stmthp,&bnd1p[3],ociError,":portype",port_type1,&status);
                    StmtExecute(ociHdbc, stmthp, ociError, &status);
					ParamGet(stmthp, ociError, &parmdp, 2, &status);
					AttrGet(parmdp, &dnamelen, ociError, &status);
					ParamGet(stmthp, ociError, &parmdp, 3, &status);
					AttrGet(parmdp, &portnblen, ociError, &status);
					ParamGet(stmthp, ociError, &parmdp, 7, &status);
					AttrGet(parmdp, &portypelen, ociError, &status);		
					ParamGet(stmthp, ociError, &parmdp, 8, &status);
					AttrGet(parmdp, &phylen, ociError, &status);
					ParamGet(stmthp, ociError, &parmdp, 10, &status);
					AttrGet(parmdp, &iplen, ociError, &status);
				
					portype_temp = (char *) realloc(portype_temp,(portypelen + 1)*sizeof(char));
					phy_temp= (char *) realloc(phy_temp,(phylen + 1)*sizeof(char));
					portnb_temp = (char *) realloc(portnb_temp,(portnblen + 1)*sizeof(char));
					ip_temp= (char *) realloc(ip_temp,(iplen + 1)*sizeof(char));
					dname_temp= (char *) realloc(dname_temp,(dnamelen + 1)*sizeof(char));

					if(dname_temp==NULL || phy_temp==NULL || ip_temp==NULL||portnb_temp==NULL||portype_temp==NULL)
					{
						status=-10;
						rescode=ShowErrors (status, ociError, "  pointer allocation unsuccessful");
						rescode=-1;
						status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
						if(phy_temp!=NULL)
							free(phy_temp);
						if(dname_temp!=NULL)
							free(dname_temp);
						if(portype_temp!=NULL)
							free(portype_temp);
						if(portnb_temp!=NULL)
							free(portnb_temp);
						if(ip_temp!=NULL)
							free(ip_temp);

						GetErrorMess(appliName,"REALLOC UNSUCCESSFUL",ErrorMessage,1);
						return rescode;
					}				
					DefineByPos(stmthp,def,ociError,1,&portid,&status);			
					DefineByPos(stmthp,def,ociError,2, dname_temp,dnamelen+1,&status);							
					DefineByPos(stmthp,def,ociError,3, portnb_temp,portnblen+1,&status);	
					DefineByPos(stmthp,def,ociError,4,&portway,&status);			  
					DefineByPos(stmthp,def,ociError,5,&admin_status,&status);
					DefineByPos(stmthp,def,ociError,6,&speed,&status);
					DefineByPos(stmthp,def,ociError,7, portype_temp, portypelen+1,&status);				
					DefineByPos(stmthp,def,ociError,8, phy_temp,phylen+1,&status);					
					DefineByPos(stmthp,def,ociError,9,&pxi_booting,&status);
					DefineByPos(stmthp,def,ociError,10, ip_temp, iplen+1,&status);		

				}catch(Error err){
					sprintf(appliName,"GetPortRow_devname");	///////
					rescode=ShowErrors (status, err.ociError, err.log);
				
					if(ociError!=0)
						OCIReportError(ociError,appliName,ErrorMessage,1); 
					else
						GetErrorMess(appliName,err.msg,ErrorMessage,1);
				}			
				
				if(status==OCI_SUCCESS)
					status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT,1,OCI_DEFAULT);

				if (status==OCI_SUCCESS || status==OCI_SUCCESS_WITH_INFO) 
				{	
					RemoveSeparator(ip_temp,"?");
					RemoveSeparator(phy_temp,"?");
					RemoveSeparator(portnb_temp,"?");
					RemoveSeparator(dname_temp,"?");
					RemoveSeparator(portype_temp,"?");
					
					sprintf(port_row_result,"|portid (I):%d |devicename (C):%s |port_nbr (C):%s |port_way (I):%d |administrative_status (I):%d |speed (I):%d |port_type (C):%s |phy (C):%s |pxi_booting (I):%d |bia (C):none |macaddress (C):none |ipaddress (C):%s |",portid,dname_temp,portnb_temp,portway,admin_status,speed,portype_temp,phy_temp,pxi_booting,ip_temp);
				} 
				else 
				{
					if(rescode==0)
					{
						rescode=0;
						strcpy(port_row_result,"NO_ROWS_SELECTED");
						GetErrorMess(appliName,"NO_ROWS_SELECTED",ErrorMessage,1);
					}
				}
			}
		}
		if(phy_temp!=NULL)
			free(phy_temp);
		if(bia_temp!=NULL)
			free(bia_temp);
		if(portnb_temp!=NULL)
			free(portnb_temp);
		if(mac_temp!=NULL)
			free(mac_temp);
		if(ip_temp!=NULL)
			free(ip_temp);
		if(portype_temp!=NULL)
			free(portype_temp);
		if(dname_temp!=NULL)
			free(dname_temp);
		if(strstr(ErrorMessage,"NO_ROWS_SELECTED")==NULL)
		{
			if(rescode==0)
			{
				OCIReportError(ociError,appliName,ErrorMessage,0); 
			}
			else
			{
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			}
		}
		status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
		return (rescode+status);
	}
/**************************************************************************************/
	//return port row in port_row_result
	/**************************************************************************************/
	/**
	* Get the port row in port_row_result, 
	* The user should manage the memory : there is no memory allocation.
	* @param deviceid : deviceid of the functional device
	* @param port_nb : port nb of the device: it's a char*, because for some system, the port nb is defined with characters and numbers (case of the daq)
	* @param port_way : 1 if it's an input,2 if it's an output, 
	* @param port_type : interface type of the port (control,bmc, data used for the DAQ system). put "" if it's null
	* @param len_port : length of the port_row_result. If the buffer is too small, we put the size needed here.
	* @param port_row_result :char* buffer where to put the result
	* @param port_row_result : formatted as follows |db_column_name (db_column_type):db_column_value|
	* @param port_row_result : db_column_type is I for int and C for char*
	* @param port_row_result : equals to "NO_ROWS_SELECTED" if no rows selected
	* @param ErrorMessage : error message in case of failure
	* @return 0 if it is successful
	*/
EXTERN_CONFDB
	int GetPortRow_devid(int deviceid, char* port_nb,int port_way,char* port_type,int &len_port, char* port_row_result,char* ErrorMessage)
	{
		char appliName[100]="GetPortRow_devid";
		int rescode=0;
		sword status;
		OCIStmt *stmthp;
		OCIBind  *bnd1p[4] ; 
		OCIDefine* def[12];
		sb4  phylen=0;
		sb4 iplen=0;
		sb4  dnamelen=0;
		sb4 portypelen=0;
		sb4  portnblen=0;
		sb4 maclen=0;
		sb4 bialen=0;
		OCIParam *parmdp;
		int portid=0;
		int admin_status=0; 
		int pxi_booting=0;
		int speed=0;
		int portway=0;
		char * ip_temp=NULL;
		char * phy_temp=NULL;
		char * mac_temp=NULL;
		char * bia_temp=NULL;
		char * dname_temp=NULL;
		char * portype_temp=NULL;
		char * portnb_temp=NULL;
		char selectport[1000];
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
		
		try{
			HandleAlloc(ociEnv,&stmthp,ociError,&status);
			sprintf(selectport,"select e.portid,t.devicename||'?',e.port_nbr||'?',e.port_way,e.administrative_status,nvl(e.speed,0),nvl(e.port_type,'none')||'?',nvl(e.phy,'none')||'?',nvl(e.pxi_booting,-1),nvl(f.bia,'none')||'?',nvl(f.macaddress,'none')||'?',nvl(e.ipaddress,'none')||'?' from %s e,%s t,%s f where t.deviceid=:dID and e.port_nbr=:portnb and e.port_way=:portway and nvl(e.port_type,'none')=:portype and t.deviceid=e.deviceid and f.serialnb=t.serialnb and f.port_way=:portway and nvl(f.port_type,'none')=:portype and f.port_nbr=:portnb",PORT_TABLE,LOGICAL_DEVICE_TABLE,HWPORT_TABLE);
			StmtPrepare(stmthp,ociError, selectport, &status);
			BindByName(stmthp,&bnd1p[0],ociError,":dID",&deviceid,&status);
			BindByName(stmthp,&bnd1p[1],ociError,":portnb",port_nb,&status);
			BindByName(stmthp,&bnd1p[2],ociError,":portway",&port_way,&status);
			BindByName(stmthp,&bnd1p[3],ociError,":portype",port_type1,&status);
			StmtExecute(ociHdbc, stmthp, ociError, &status);
			ParamGet(stmthp, ociError, &parmdp, 2, &status);
			AttrGet(parmdp, &dnamelen, ociError, &status);
			ParamGet(stmthp, ociError, &parmdp, 3, &status);
			AttrGet(parmdp, &portnblen, ociError, &status);			
			ParamGet(stmthp, ociError, &parmdp, 7, &status);
			AttrGet(parmdp, &portypelen, ociError, &status);					
			ParamGet(stmthp, ociError, &parmdp, 8, &status);
			AttrGet(parmdp, &phylen, ociError, &status);			
			ParamGet(stmthp, ociError, &parmdp, 10, &status);
			AttrGet(parmdp, &bialen, ociError, &status);			
			ParamGet(stmthp, ociError, &parmdp, 11, &status);
			AttrGet(parmdp, &maclen, ociError, &status);            
			ParamGet(stmthp, ociError, &parmdp, 12, &status);
			AttrGet(parmdp, &iplen, ociError, &status);

			portype_temp = (char *) realloc(portype_temp,(portypelen + 1)*sizeof(char));
			phy_temp= (char *) realloc(phy_temp,(phylen + 1)*sizeof(char));
			portnb_temp = (char *) realloc(portnb_temp,(portnblen + 1)*sizeof(char));
			bia_temp= (char *) realloc(bia_temp,(bialen + 1)*sizeof(char));
			mac_temp = (char *) realloc(mac_temp,(maclen + 1)*sizeof(char));
			ip_temp= (char *) realloc(ip_temp,(iplen + 1)*sizeof(char));
			dname_temp= (char *) realloc(dname_temp,(dnamelen + 1)*sizeof(char));

			if(dname_temp==NULL || phy_temp==NULL ||bia_temp==NULL||mac_temp==NULL|| ip_temp==NULL||portnb_temp==NULL||portype_temp==NULL)
			{
				status=-10;
				rescode=ShowErrors (status, ociError, "  pointer allocation unsuccessful");
				rescode=-1;
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				if(phy_temp!=NULL)
					free(phy_temp);
				if(dname_temp!=NULL)
					free(dname_temp);
				if(portype_temp!=NULL)
					free(portype_temp);
				if(portnb_temp!=NULL)
					free(portnb_temp);
				if(mac_temp!=NULL)
					free(mac_temp);
				if(ip_temp!=NULL)
					free(ip_temp);
				if(bia_temp!=NULL)
					free(bia_temp);

				GetErrorMess(appliName,"REALLOC UNSUCCESSFUL",ErrorMessage,1);
				return rescode;
			}							
			DefineByPos(stmthp,def,ociError,1,&portid,&status);						
			DefineByPos(stmthp,def,ociError,2, dname_temp,dnamelen+1,&status);										 
			DefineByPos(stmthp,def,ociError,3, portnb_temp,portnblen+1,&status);					
			DefineByPos(stmthp,def,ociError,4,&portway,&status);
			DefineByPos(stmthp,def,ociError,5,&admin_status,&status);
			DefineByPos(stmthp,def,ociError,6,&speed,&status);
			DefineByPos(stmthp,def,ociError,7, portype_temp, portypelen+1,&status);				
			DefineByPos(stmthp,def,ociError,8, phy_temp,phylen+1,&status);		
			DefineByPos(stmthp,def,ociError,9,&pxi_booting,&status);			
			DefineByPos(stmthp,def,ociError,10, bia_temp, bialen + 1,&status);				
			DefineByPos(stmthp,def,ociError,11, mac_temp, maclen+1,&status);				
			DefineByPos(stmthp,def,ociError,12, ip_temp, iplen+1,&status);

		}catch(Error err){
			sprintf(appliName,"GetPortRow_devid");	///////
			rescode=ShowErrors (status, err.ociError, err.log);
			
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			else
				GetErrorMess(appliName,err.msg,ErrorMessage,1);
			
			if(strstr(err.msg,"NOT CONNECTED TO ANY DB")!=NULL)
			return -1;
		}
		
		if(status==OCI_SUCCESS)
			status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT,1,OCI_DEFAULT);

		if (status==OCI_SUCCESS || status==OCI_SUCCESS_WITH_INFO) 
		{	
			RemoveSeparator(ip_temp,"?");
			RemoveSeparator(phy_temp,"?");
			RemoveSeparator(mac_temp,"?");
			RemoveSeparator(portnb_temp,"?");
			RemoveSeparator(dname_temp,"?");
			RemoveSeparator(bia_temp,"?");
			RemoveSeparator(portype_temp,"?");

			sprintf(port_row_result,"|portid (I):%d |devicename (C):%s |port_nbr (C):%s |port_way (I):%d |administrative_status (I):%d |speed (I):%d |port_type (C):%s |phy (C):%s |pxi_booting (I):%d |bia (C):%s |macaddress (C):%s |ipaddress (C):%s |",portid,dname_temp,portnb_temp,portway,admin_status,speed,portype_temp,phy_temp,pxi_booting,bia_temp,mac_temp,ip_temp);
		} 
		else 
		{
			status=0;
			if(rescode==0)
			{
				sprintf(selectport,"select e.portid,t.devicename||'?',e.port_nbr||'?',e.port_way,e.administrative_status,nvl(e.speed,0),nvl(e.port_type,'none')||'?',nvl(e.phy,'none')||'?',nvl(e.pxi_booting,-1),nvl(e.ipaddress,'none')||'?' from %s e,%s t where t.deviceid=:dID and e.port_nbr=:portnb and e.port_way=:portway and nvl(e.port_type,'none')=:portype and t.deviceid=e.deviceid  ",PORT_TABLE,LOGICAL_DEVICE_TABLE);
			
				try{
                    StmtPrepare(stmthp,ociError, selectport, &status);
                    BindByName(stmthp,&bnd1p[0],ociError,":dID",&deviceid,&status);
					BindByName(stmthp,&bnd1p[1],ociError,":portnb",port_nb,&status);
					BindByName(stmthp,&bnd1p[2],ociError,":portway",&port_way,&status);
					BindByName(stmthp,&bnd1p[3],ociError,":portype",port_type1,&status);                  
                    StmtExecute(ociHdbc, stmthp, ociError, &status);
					ParamGet(stmthp, ociError, &parmdp, 2, &status);
					AttrGet(parmdp, &dnamelen, ociError, &status);
					ParamGet(stmthp, ociError, &parmdp, 3, &status);
					AttrGet(parmdp, &portnblen, ociError, &status);
					ParamGet(stmthp, ociError, &parmdp, 7, &status);
					AttrGet(parmdp, &portypelen, ociError, &status);		
					ParamGet(stmthp, ociError, &parmdp, 8, &status);
					AttrGet(parmdp, &phylen, ociError, &status);
					ParamGet(stmthp, ociError, &parmdp, 10, &status);
					AttrGet(parmdp, &iplen, ociError, &status);
		
					portype_temp = (char *) realloc(portype_temp,(portypelen + 1)*sizeof(char));
					phy_temp= (char *) realloc(phy_temp,(phylen + 1)*sizeof(char));
					portnb_temp = (char *) realloc(portnb_temp,(portnblen + 1)*sizeof(char));
					ip_temp= (char *) realloc(ip_temp,(iplen + 1)*sizeof(char));
					dname_temp= (char *) realloc(dname_temp,(dnamelen + 1)*sizeof(char));

					if(dname_temp==NULL || phy_temp==NULL || ip_temp==NULL||portnb_temp==NULL||portype_temp==NULL)
					{
						status=-10;
						rescode=ShowErrors (status, ociError, "  pointer allocation unsuccessful");
						rescode=-1;
						status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
						if(phy_temp!=NULL)
							free(phy_temp);
						if(dname_temp!=NULL)
							free(dname_temp);
						if(portype_temp!=NULL)
							free(portype_temp);
						if(portnb_temp!=NULL)
							free(portnb_temp);	
						if(ip_temp!=NULL)
							free(ip_temp);

						GetErrorMess(appliName,"REALLOC UNSUCCESSFUL",ErrorMessage,1);
						return rescode;
					}									
					DefineByPos(stmthp,def,ociError,1,&portid,&status);			
					DefineByPos(stmthp,def,ociError,2, dname_temp,dnamelen+1,&status);							
					DefineByPos(stmthp,def,ociError,3, portnb_temp,portnblen+1,&status);	
					DefineByPos(stmthp,def,ociError,4,&portway,&status);			  
					DefineByPos(stmthp,def,ociError,5,&admin_status,&status);
					DefineByPos(stmthp,def,ociError,6,&speed,&status);
					DefineByPos(stmthp,def,ociError,7, portype_temp, portypelen+1,&status);				
					DefineByPos(stmthp,def,ociError,8, phy_temp,phylen+1,&status);					
					DefineByPos(stmthp,def,ociError,9,&pxi_booting,&status);
					DefineByPos(stmthp,def,ociError,10, ip_temp, iplen+1,&status);
					
				}catch(Error err){
					sprintf(appliName,"GetPortRow_devid");	///////
					rescode=ShowErrors (status, err.ociError, err.log);
				
					if(ociError!=0)
						OCIReportError(ociError,appliName,ErrorMessage,1); 
					else
						GetErrorMess(appliName,err.msg,ErrorMessage,1);
				}
		
				if(status==OCI_SUCCESS)
                    status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT,1,OCI_DEFAULT);
	
				if (status==OCI_SUCCESS || status==OCI_SUCCESS_WITH_INFO) 
				{	
					RemoveSeparator(ip_temp,"?");
					RemoveSeparator(phy_temp,"?");
					RemoveSeparator(portnb_temp,"?");
					RemoveSeparator(dname_temp,"?");
					RemoveSeparator(portype_temp,"?");
		
					sprintf(port_row_result,"|portid (I):%d |devicename (C):%s |port_nbr (C):%s |port_way (I):%d |administrative_status (I):%d |speed (I):%d |port_type (C):%s |phy (C):%s |pxi_booting (I):%d |bia (C):none |macaddress (C):none |ipaddress (C):%s |",portid,dname_temp,portnb_temp,portway,admin_status,speed,portype_temp,phy_temp,pxi_booting,ip_temp);
				} 
				else 
				{
					if(rescode==0)
					{
						rescode=0;
						strcpy(port_row_result,"NO_ROWS_SELECTED");
						GetErrorMess(appliName,"NO_ROWS_SELECTED",ErrorMessage,1);
					}
				}
			}
		}
		if(phy_temp!=NULL)
			free(phy_temp);
		if(bia_temp!=NULL)
			free(bia_temp);
		if(portnb_temp!=NULL)
			free(portnb_temp);
		if(mac_temp!=NULL)
			free(mac_temp);
		if(ip_temp!=NULL)
			free(ip_temp);
		if(portype_temp!=NULL)
			free(portype_temp);
		if(dname_temp!=NULL)
			free(dname_temp);
		if(strstr(ErrorMessage,"NO_ROWS_SELECTED")==NULL)
		{
			if(rescode==0)
			{
				OCIReportError(ociError,appliName,ErrorMessage,0); 
			}
			else
			{
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			}
		}
		status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
		return (rescode+status);
	}
/**************************************************************************************/
	//return link row in  Conn_row
	/**************************************************************************************/
	/**
	* Get the link row in  Conn_row, returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* @param lkID : lkID of the link
	* @param len_conn : length of the Conn_row. If the buffer is too small, we put the size needed here.
	* @param Conn_row :char* buffer where to put the result
	* @param Conn_row : formatted as follows |db_column_name (db_column_type):db_column_value|
	* @param Conn_row : db_column_type is I for int and C for char*
	* @param Conn_row : equals to "NO_ROWS_SELECTED" if no rows selected
	* @param ErrorMessage : error message in case of failure
	* @return 0 if it is successful
	*/
EXTERN_CONFDB
	int GetMacroConnectivityRow_lkid(int lkID, int &len_conn,char* Conn_row,char* ErrorMessage)
	{
		char appliName[100]="GetMacroConnectivityRow_lkid";
		sword status;
		int rescode=0;
		OCIStmt *stmthp;
		OCIBind  *bnd1p = (OCIBind *) 0; 
		OCIDefine* def[11];
		OCIParam *parmdp;
		sb4  dnamefromlen=0;
		sb4 dnametolen=0;
		sb4  portnbfromlen=0;
		sb4 portnbtolen=0;
		sb4  porttypefromlen=0;
		sb4 porttypetolen=0;
		sb4  lknamelen=0;
		char * dnamefrom_temp=NULL;
		char * dnameto_temp=NULL;
		char * portnbfrom_temp=NULL;
		char * portnbto_temp=NULL;
		char * lkname_temp=NULL;
		int linkid=0;
		char * porttypefrom_temp=NULL;
		char * porttypeto_temp=NULL;
		int lkinfolen=0;
		int bidirectional_link_used=0;
		int lkused=0;
		char selectconn[1000];
		char * lkinfo_temp=NULL;

		try{
			HandleAlloc(ociEnv,&stmthp,ociError,&status);
			sprintf(selectconn,"select t.linkid,e.devicename||'?',f.devicename||'?',p.port_nbr||'?',nvl(p.port_type,'none')||'?',g.port_nbr||'?',nvl(g.port_type,'none')||'?',nvl(l.link_name,'none')||'?',t.bidirectional_link_used, t.lkused,nvl(t.link_info,'none')||'?' from %s t,%s p,%s g,%s l,%s e,%s f where t.linkid=:lkid and t.link_type=l.linktypeID and p.portid=t.portidfrom and g.portid=t.portidto and p.deviceid=e.deviceid and g.deviceid=f.deviceid",MACRO_CONNECTIVITY_TABLE,PORT_TABLE,PORT_TABLE,LINKTYPE_TABLE,LOGICAL_DEVICE_TABLE,LOGICAL_DEVICE_TABLE);
			StmtPrepare(stmthp,ociError, selectconn, &status);
			BindByName(stmthp,&bnd1p,ociError,":lkid",&lkID,&status);
			StmtExecute(ociHdbc, stmthp, ociError, &status);
			ParamGet(stmthp, ociError, &parmdp, 2, &status);
			AttrGet(parmdp, &dnamefromlen, ociError, &status);
			ParamGet(stmthp, ociError, &parmdp, 3, &status);
			AttrGet(parmdp, &dnametolen, ociError, &status);			
			ParamGet(stmthp, ociError, &parmdp, 4, &status);
			AttrGet(parmdp, &portnbfromlen, ociError, &status);					
			ParamGet(stmthp, ociError, &parmdp, 5, &status);
			AttrGet(parmdp, &porttypefromlen, ociError, &status);			
			ParamGet(stmthp, ociError, &parmdp, 6, &status);
			AttrGet(parmdp, &portnbtolen, ociError, &status);			
			ParamGet(stmthp, ociError, &parmdp, 7, &status);
			AttrGet(parmdp, &porttypetolen, ociError, &status);            
			ParamGet(stmthp, ociError, &parmdp, 8, &status);
			AttrGet(parmdp, &lknamelen, ociError, &status);
			ParamGet(stmthp, ociError, &parmdp, 11, &status);
			AttrGet(parmdp, &lkinfolen, ociError, &status);

			portnbfrom_temp = (char *) realloc(portnbfrom_temp,(portnbfromlen + 1)*sizeof(char));
			portnbto_temp= (char *) realloc(portnbto_temp,(portnbtolen + 1)*sizeof(char));
			dnameto_temp = (char *) realloc(dnameto_temp,(dnametolen + 1)*sizeof(char));
			dnamefrom_temp= (char *) realloc(dnamefrom_temp,(dnamefromlen + 1)*sizeof(char));
			lkname_temp= (char *) realloc(lkname_temp,(lknamelen + 1)*sizeof(char));
			porttypefrom_temp = (char *) realloc(porttypefrom_temp,(porttypefromlen + 1)*sizeof(char));
			porttypeto_temp= (char *) realloc(porttypeto_temp,(porttypetolen + 1)*sizeof(char));
			lkinfo_temp= (char *) realloc(lkinfo_temp,(lkinfolen + 1)*sizeof(char));
			
			if(lkinfo_temp==NULL||dnamefrom_temp==NULL || dnameto_temp==NULL ||portnbto_temp==NULL||portnbfrom_temp==NULL||porttypeto_temp==NULL||porttypefrom_temp==NULL||lkname_temp==NULL)
			{
				status=-10;
				rescode=ShowErrors (status, ociError, "  pointer allocation unsuccessful");
				rescode=-1;
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				if(portnbfrom_temp!=NULL)
					free(portnbfrom_temp);
				if(portnbto_temp!=NULL)
					free(portnbto_temp);
				if(dnamefrom_temp!=NULL)
					free(dnamefrom_temp);
				if(dnameto_temp!=NULL)
					free(dnameto_temp);
				if(lkname_temp!=NULL)
					free(lkname_temp);
				if(porttypefrom_temp!=NULL)
					free(porttypefrom_temp);
				if(porttypeto_temp!=NULL)
					free(porttypeto_temp);
				if(lkinfo_temp!=NULL)
					free(lkinfo_temp);

				GetErrorMess(appliName,"REALLOC UNSUCCESSFUL",ErrorMessage,1);
				return rescode;
			}			
			DefineByPos(stmthp,def,ociError,1,&linkid,&status);						
			DefineByPos(stmthp,def,ociError,2, dnamefrom_temp,dnamefromlen+1,&status);										 
			DefineByPos(stmthp,def,ociError,3, dnameto_temp,dnametolen+1,&status);					
			DefineByPos(stmthp,def,ociError,4, portnbfrom_temp,portnbfromlen+1,&status);					
			DefineByPos(stmthp,def,ociError,5, porttypefrom_temp,porttypefromlen+1,&status);					
			DefineByPos(stmthp,def,ociError,6, portnbto_temp,portnbtolen+1,&status);					
			DefineByPos(stmthp,def,ociError,7, porttypeto_temp,porttypetolen+1,&status);					
			DefineByPos(stmthp,def,ociError,8, lkname_temp,lknamelen,&status);					
			DefineByPos(stmthp,def,ociError,9,&bidirectional_link_used,&status);
			DefineByPos(stmthp,def,ociError,10,&lkused,&status);
			DefineByPos(stmthp,def,ociError,11, lkinfo_temp,lkinfolen+1,&status);					
						
		}catch(Error err){
			sprintf(appliName,"GetMacroConnectivityRow_lkid");	///////
			rescode=ShowErrors (status, err.ociError, err.log);
			
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			else
				GetErrorMess(appliName,err.msg,ErrorMessage,1);
			
			if(strstr(err.msg,"NOT CONNECTED TO ANY DB")!=NULL)
			return -1;
		}
		
		if(status==OCI_SUCCESS)
			status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT,1, OCI_DEFAULT);
		if (status==OCI_SUCCESS || status==OCI_SUCCESS_WITH_INFO) 
		{	
			RemoveSeparator(dnamefrom_temp,"?");
			RemoveSeparator(dnameto_temp,"?");
			RemoveSeparator(portnbfrom_temp, "?");
			RemoveSeparator(portnbto_temp,"?");
			RemoveSeparator(lkname_temp,"?");
			RemoveSeparator(lkinfo_temp,"?");
			RemoveSeparator(porttypefrom_temp,"?");
			RemoveSeparator(porttypeto_temp,"?");
			
			sprintf(Conn_row,"|linkid (I):%d |node_from (C):%s |node_to (C):%s |port_nbrfrom (C):%s |port_typefrom (C): %s|port_nbrto (C):%s |port_typeto (C):%s |link_type (C):%s |bidirectional_link_used (I):%d |lkused (I):%d |link_info (C):%s |",linkid,dnamefrom_temp,dnameto_temp,portnbfrom_temp,porttypefrom_temp,portnbto_temp,porttypeto_temp,lkname_temp,bidirectional_link_used, lkused,lkinfo_temp);

			if(portnbfrom_temp!=NULL)
				free(portnbfrom_temp);
			if(portnbto_temp!=NULL)
				free(portnbto_temp);
			if(dnamefrom_temp!=NULL)
				free(dnamefrom_temp);
			if(dnameto_temp!=NULL)
				free(dnameto_temp);
			if(lkname_temp!=NULL)
				free(lkname_temp);
			if(porttypefrom_temp!=NULL)
				free(porttypefrom_temp);
			if(porttypeto_temp!=NULL)
				free(porttypeto_temp);
			if(lkinfo_temp!=NULL)
				free(lkinfo_temp);
		} 
		else 
		{
			if(rescode==0)
			{
				rescode=0;
				strcpy(Conn_row,"NO_ROWS_SELECTED");
				GetErrorMess(appliName,"NO_ROWS_SELECTED",ErrorMessage,1);
			}
		}

		status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
		if(strstr(ErrorMessage,"NO_ROWS_SELECTED")==NULL)
		{
			if(rescode==0)
			{
				OCIReportError(ociError,appliName,ErrorMessage,0); 
			}
			else
			{
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			}
		}
		return rescode;

	}
/**************************************************************************************/
	//return link row in  Conn_row
	/**************************************************************************************/
	/**
	* Get the link row in  Conn_row,  returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* @param nodeID : deviceid of the functional device
	* @param port_nb : port_nb of the device
	* @param port_type : put "" if it's null or the port type which has been for the connectivity part
	* @param port_way : 1 if the port_nb is an input (so it means thta the link ends up to this port),2 if the port nb is an output (so the link starts from this port)
	* @param len_conn : length of the Conn_row. If the buffer is too small, we put the size needed here.
	* @param Conn_row :char* buffer where to put the result
	* @param Conn_row : formatted as follows |db_column_name (db_column_type):db_column_value|
	* @param Conn_row : db_column_type is I for int and C for char*
	* @param Conn_row : equals to "NO_ROWS_SELECTED" if no rows selected
	* @param ErrorMessage : error message in case of failure
	* @return 0 if it is successful
	*/
EXTERN_CONFDB
	int GetMacroConnectivityRow_node(int nodeID, char* port_nb, int port_way,char* port_type,int &len_conn, char* Conn_row,char* ErrorMessage)
	{
		char appliName[100]="GetMacroConnectivityRow_node";
		sword status;
		int rescode=0;
		sb4  porttypefromlen=0;
		sb4 porttypetolen=0;
		OCIStmt *stmthp;
		OCIBind  *bnd1p[3] ; 
		OCIDefine* def[11];
		OCIParam *parmdp;
		sb4  dnamefromlen=0;
		sb4 dnametolen=0;
		sb4  portnbfromlen=0;
		sb4 portnbtolen=0;
		sb4  lknamelen=0;
		sb4 lkinfolen=0;
		char * dnamefrom_temp=NULL;
		char * dnameto_temp=NULL;
		char * portnbfrom_temp=NULL;
		char * portnbto_temp=NULL;
		char * lkname_temp=NULL;
		char * lkinfo_temp=NULL;
		int linkid=0;
		char port_type1[100];
		char * porttypefrom_temp=NULL;
		char * porttypeto_temp=NULL;
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
		int bidirectional_link_used=0;
		int lkused=0;
		char selectconn[1000];
				
		try{
			HandleAlloc(ociEnv,&stmthp,ociError,&status);
			if(port_way==1) //port nb is an output so we have info about the node_to part of a link
				sprintf(selectconn,"select t.linkid,e.devicename||'?',f.devicename||'?',p.port_nbr||'?',nvl(p.port_type,'none')||'?',g.port_nbr||'?',nvl(g.port_type,'none')||'?',l.link_name||'?',t.bidirectional_link_used, t.lkused,nvl(t.link_info,'none')||'?' from %s t,%s p,%s g,%s l,%s e,%s f where nvl(g.port_type,'none')=:portype and g.port_nbr=:portnb and g.port_way=1  and t.link_type=l.linktypeID and p.portid=t.portidfrom and g.portid=t.portidto and g.deviceid=f.deviceid and p.deviceid=e.deviceid and f.deviceid=:dID",MACRO_CONNECTIVITY_TABLE,PORT_TABLE,PORT_TABLE,LINKTYPE_TABLE,LOGICAL_DEVICE_TABLE,LOGICAL_DEVICE_TABLE);
			else if(port_way==2)// info about the node_from part of a link
				sprintf(selectconn,"select t.linkid,e.devicename||'?',f.devicename||'?',p.port_nbr||'?',nvl(p.port_type,'none')||'?',g.port_nbr||'?',nvl(g.port_type,'none')||'?',l.link_name||'?',t.bidirectional_link_used, t.lkused,nvl(t.link_info,'none')||'?' from %s t,%s p,%s g,%s l,%s e,%s f where nvl(p.port_type,'none')=:portype and p.port_nbr=:portnb and p.port_way=2 and t.link_type=l.linktypeID and p.portid=t.portidfrom and g.portid=t.portidto and g.deviceid=f.deviceid and p.deviceid=e.deviceid and e.deviceid=:dID",MACRO_CONNECTIVITY_TABLE,PORT_TABLE,PORT_TABLE,LINKTYPE_TABLE,LOGICAL_DEVICE_TABLE,LOGICAL_DEVICE_TABLE);
			else {
				printf("\nport_way must be 1 or 2\n");
				GetErrorMess(appliName,"port_way must be 1 or 2",ErrorMessage,1);
				return -1;
			}
				
			StmtPrepare(stmthp,ociError, selectconn, &status);
			BindByName(stmthp,&bnd1p[0],ociError,":dID",&nodeID,&status);
			BindByName(stmthp,&bnd1p[1],ociError,":portnb",port_nb,&status);
			BindByName(stmthp,&bnd1p[2],ociError,":portype",port_type1,&status);			
			StmtExecute(ociHdbc, stmthp, ociError, &status);
			ParamGet(stmthp, ociError, &parmdp, 2, &status);
			AttrGet(parmdp, &dnamefromlen, ociError, &status);
			ParamGet(stmthp, ociError, &parmdp, 3, &status);
			AttrGet(parmdp, &dnametolen, ociError, &status);			
			ParamGet(stmthp, ociError, &parmdp, 4, &status);
			AttrGet(parmdp, &portnbfromlen, ociError, &status);					
			ParamGet(stmthp, ociError, &parmdp, 5, &status);
			AttrGet(parmdp, &porttypefromlen, ociError, &status);			
			ParamGet(stmthp, ociError, &parmdp, 6, &status);
			AttrGet(parmdp, &portnbtolen, ociError, &status);			
			ParamGet(stmthp, ociError, &parmdp, 7, &status);
			AttrGet(parmdp, &porttypetolen, ociError, &status);            			
			ParamGet(stmthp, ociError, &parmdp, 8, &status);
			AttrGet(parmdp, &lknamelen, ociError, &status);
			ParamGet(stmthp, ociError, &parmdp, 11, &status);
			AttrGet(parmdp, &lkinfolen, ociError, &status);
			
			portnbfrom_temp = (char *) realloc(portnbfrom_temp,(portnbfromlen + 1)*sizeof(char));
			portnbto_temp= (char *) realloc(portnbto_temp,(portnbtolen + 1)*sizeof(char));
			dnameto_temp = (char *) realloc(dnameto_temp,(dnametolen + 1)*sizeof(char));
			dnamefrom_temp= (char *) realloc(dnamefrom_temp,(dnamefromlen + 1)*sizeof(char));
			lkname_temp= (char *) realloc(lkname_temp,(lknamelen + 1)*sizeof(char));
			lkinfo_temp= (char *) realloc(lkinfo_temp,(lkinfolen + 1)*sizeof(char));
			porttypefrom_temp = (char *) realloc(porttypefrom_temp,(porttypefromlen + 1)*sizeof(char));
			porttypeto_temp= (char *) realloc(porttypeto_temp,(porttypetolen + 1)*sizeof(char));

			if(lkinfo_temp==NULL||dnamefrom_temp==NULL || dnameto_temp==NULL ||portnbto_temp==NULL||portnbfrom_temp==NULL||porttypefrom_temp==NULL||porttypeto_temp==NULL||lkname_temp==NULL)
			{
				status=-10;
				rescode=ShowErrors (status, ociError, "  pointer allocation unsuccessful");
				rescode=-1;
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				if(portnbfrom_temp!=NULL)
					free(portnbfrom_temp);
				if(portnbto_temp!=NULL)
					free(portnbto_temp);
				if(dnamefrom_temp!=NULL)
					free(dnamefrom_temp);
				if(dnameto_temp!=NULL)
					free(dnameto_temp);
				if(lkname_temp!=NULL)
					free(lkname_temp);
				if(porttypefrom_temp!=NULL)
					free(porttypefrom_temp);
				if(porttypeto_temp!=NULL)
					free(porttypeto_temp);
				if(lkinfo_temp!=NULL)
					free(lkinfo_temp);

				GetErrorMess(appliName,"REALLOC UNSUCCESSFUL",ErrorMessage,1);
				return rescode;
			}
			DefineByPos(stmthp,def,ociError,1,&linkid,&status);						
			DefineByPos(stmthp,def,ociError,2, dnamefrom_temp,dnamefromlen+1,&status);	
			DefineByPos(stmthp,def,ociError,3, dnameto_temp,dnametolen+1,&status);			
			DefineByPos(stmthp,def,ociError,4, portnbfrom_temp,portnbfromlen+1,&status);	
			DefineByPos(stmthp,def,ociError,5, porttypefrom_temp,porttypefromlen+1,&status);			
			DefineByPos(stmthp,def,ociError,6, portnbto_temp,portnbtolen+1,&status);		
			DefineByPos(stmthp,def,ociError,7, porttypeto_temp,porttypetolen+1,&status);		
			DefineByPos(stmthp,def,ociError,8, lkname_temp,lknamelen,&status);
			DefineByPos(stmthp,def,ociError,9,&bidirectional_link_used,&status);
			DefineByPos(stmthp,def,ociError,10,&lkused,&status);
			DefineByPos(stmthp,def,ociError,11, lkinfo_temp,lkinfolen+1,&status);
			
		}catch(Error err){
			sprintf(appliName,"GetMacroConnectivityRow_node");	///////
			rescode=ShowErrors (status, err.ociError, err.log);
			
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			else
				GetErrorMess(appliName,err.msg,ErrorMessage,1);
			
			if(strstr(err.msg,"NOT CONNECTED TO ANY DB")!=NULL)
			return -1;
		}
		
		if(status==OCI_SUCCESS)
			status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT,1, OCI_DEFAULT);
		if (status==OCI_SUCCESS || status==OCI_SUCCESS_WITH_INFO) 
		{	
			RemoveSeparator(dnamefrom_temp,"?");
			RemoveSeparator(dnameto_temp,"?");
			RemoveSeparator(portnbfrom_temp,"?");
			RemoveSeparator(portnbto_temp,"?");
			RemoveSeparator(lkname_temp,"?");
			RemoveSeparator(lkinfo_temp,"?");
			RemoveSeparator(porttypefrom_temp,"?");
			RemoveSeparator(porttypeto_temp,"?");

			sprintf(Conn_row,"|linkid (I):%d |node_from (C):%s |node_to (C):%s |port_nbrfrom (C):%s |port_typefrom (C):%s |port_nbrto (C):%s |port_typeto (C):%s |link_type (C):%s |bidirectional_link_used (I):%d |lkused (I):%d |link_info (C):%s |",linkid,dnamefrom_temp,dnameto_temp,portnbfrom_temp,porttypefrom_temp,portnbto_temp,porttypeto_temp,lkname_temp,bidirectional_link_used, lkused,lkinfo_temp);

			if(portnbfrom_temp!=NULL)
				free(portnbfrom_temp);
			if(portnbto_temp!=NULL)
				free(portnbto_temp);
			if(dnamefrom_temp!=NULL)
				free(dnamefrom_temp);
			if(dnameto_temp!=NULL)
				free(dnameto_temp);
			if(lkname_temp!=NULL)
				free(lkname_temp);
			if(lkinfo_temp!=NULL)
				free(lkinfo_temp);
			if(porttypefrom_temp!=NULL)
				free(porttypefrom_temp);
			if(porttypeto_temp!=NULL)
				free(porttypeto_temp);
		} 
		else if(rescode==0)
            {
				rescode=0;
				strcpy(Conn_row,"NO_ROWS_SELECTED");
				GetErrorMess(appliName,"NO_ROWS_SELECTED",ErrorMessage,1);
			}

		status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
		if(strstr(ErrorMessage,"NO_ROWS_SELECTED")==NULL)
		{
			if(rescode==0)
			{
				OCIReportError(ociError,appliName,ErrorMessage,0); 
			}
			else
			{
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			}
		}
		return rescode;
	}
/**************************************************************************************/
	//return link row in  Conn_row
	/**************************************************************************************/
	/**
	* Get the link row in  Conn_row, 
	* The user should manage the memory : there is no memory allocation.
	* @param node_name : node_name of the functional device
	* @param port_nb : port_nb of the device
	* @param port_type : put "" if it's null or the port type which has been for the connectivity part
	* @param port_way : 1 if the port_nb is an input (so it means thta the link ends up to this port),2 if the port nb is an output (so the link starts from this port)
	* @param port_way way : 1 if the deviceID corresponds to the start of the link otherwise it's considered as the end of the link
	* @param len_conn : length of the Conn_row. If the buffer is too small, we put the size needed here.
	* @param Conn_row :char* buffer where to put the result
	* @param Conn_row : formatted as follows |db_column_name (db_column_type):db_column_value|
	* @param Conn_row :attribute devicename_nodefrom and devicename_nodeto are not part of the row, it's just to get the devicename of nodefrom and nodeto
	* @param Conn_row : db_column_type is I for int and C for char*
	* @param Conn_row : equals to "NO_ROWS_SELECTED" if no rows selected
	* @param ErrorMessage : error message in case of failure
	* @return 0 if it is successful
	*/
EXTERN_CONFDB
	int GetMacroConnectivityRow_nodename(char* node_name, char* port_nb, int port_way,char* port_type,int &len_conn, char* Conn_row,char* ErrorMessage)
	{
		char appliName[100]="GetMacroConnectivityRow_nodename";
		sword status;
		int rescode=0;
		sb4  porttypefromlen=0;
		sb4 porttypetolen=0;
		OCIStmt *stmthp;
		OCIBind  *bnd1p[3] ; 
		OCIDefine* def[11];
		OCIParam *parmdp;
		sb4  dnamefromlen=0;
		sb4 dnametolen=0;
		sb4  portnbfromlen=0;
		sb4 portnbtolen=0;
		sb4  lknamelen=0;
		sb4 lkinfolen=0;
		char * dnamefrom_temp=NULL;
		char * dnameto_temp=NULL;
		char * portnbfrom_temp=NULL;
		char * portnbto_temp=NULL;
		char * porttypefrom_temp=NULL;
		char * porttypeto_temp=NULL;
		char * lkname_temp=NULL;
		char* lkinfo_temp=NULL;
		int linkid=0;
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
		int bidirectional_link_used=0;
		int lkused=0;
		char selectconn[1000];
		
		try{
			HandleAlloc(ociEnv,&stmthp,ociError,&status);

			if(port_way==1) //port nb is an input so we have info about the node_to part of a link
				sprintf(selectconn,"select t.linkid,e.devicename||'?',f.devicename||'?',p.port_nbr||'?',nvl(p.port_type,'none')||'?',g.port_nbr||'?',nvl(g.port_type,'none')||'?',l.link_name||'?',t.bidirectional_link_used, t.lkused,nvl(t.link_info,'none')||'?' from %s t,%s p,%s g,%s l,%s e,%s f where nvl(g.port_type,'none')=:portype and g.port_nbr=:portnb and g.port_way=1  and t.link_type=l.linktypeID and p.portid=t.portidfrom and g.portid=t.portidto and g.deviceid=f.deviceid and p.deviceid=e.deviceid and f.devicename=:dname",MACRO_CONNECTIVITY_TABLE,PORT_TABLE,PORT_TABLE,LINKTYPE_TABLE,LOGICAL_DEVICE_TABLE,LOGICAL_DEVICE_TABLE);
			else if(port_way==2)// info about the node_from part of a link
				sprintf(selectconn,"select t.linkid,e.devicename||'?',f.devicename||'?',p.port_nbr||'?',nvl(p.port_type,'none')||'?',g.port_nbr||'?',nvl(g.port_type,'none')||'?',l.link_name||'?',t.bidirectional_link_used, t.lkused,nvl(t.link_info,'none')||'?' from %s t,%s p,%s g,%s l,%s e,%s f where nvl(p.port_type,'none')=:portype and p.port_nbr=:portnb and p.port_way=2 and t.link_type=l.linktypeID and p.portid=t.portidfrom and g.portid=t.portidto and g.deviceid=f.deviceid and p.deviceid=e.deviceid and e.devicename=:dname",MACRO_CONNECTIVITY_TABLE,PORT_TABLE,PORT_TABLE,LINKTYPE_TABLE,LOGICAL_DEVICE_TABLE,LOGICAL_DEVICE_TABLE);
			else {
				printf("\nport_way must be 1 or 2\n");
				GetErrorMess(appliName,"port_way must be 1 or 2",ErrorMessage,1);
				return -1;
			}				
			StmtPrepare(stmthp,ociError, selectconn, &status);
			BindByName(stmthp,&bnd1p[0],ociError,":dname",node_name,&status);
			BindByName(stmthp,&bnd1p[1],ociError,":portnb",port_nb,&status);
			BindByName(stmthp,&bnd1p[2],ociError,":portype",port_type1,&status);			
			StmtExecute(ociHdbc, stmthp, ociError, &status);						
			ParamGet(stmthp, ociError, &parmdp, 2, &status);
			AttrGet(parmdp, &dnamefromlen, ociError, &status);
			ParamGet(stmthp, ociError, &parmdp, 3, &status);
			AttrGet(parmdp, &dnametolen, ociError, &status);			
			ParamGet(stmthp, ociError, &parmdp, 4, &status);
			AttrGet(parmdp, &portnbfromlen, ociError, &status);					
			ParamGet(stmthp, ociError, &parmdp, 5, &status);
			AttrGet(parmdp, &porttypefromlen, ociError, &status);			
			ParamGet(stmthp, ociError, &parmdp, 6, &status);
			AttrGet(parmdp, &portnbtolen, ociError, &status);			
			ParamGet(stmthp, ociError, &parmdp, 7, &status);
			AttrGet(parmdp, &porttypetolen, ociError, &status);            			
			ParamGet(stmthp, ociError, &parmdp, 8, &status);
			AttrGet(parmdp, &lknamelen, ociError, &status);
			ParamGet(stmthp, ociError, &parmdp, 11, &status);
			AttrGet(parmdp, &lkinfolen, ociError, &status);

			portnbfrom_temp = (char *) realloc(portnbfrom_temp,(portnbfromlen + 1)*sizeof(char));
			portnbto_temp= (char *) realloc(portnbto_temp,(portnbtolen + 1)*sizeof(char));
			dnameto_temp = (char *) realloc(dnameto_temp,(dnametolen + 1)*sizeof(char));
			dnamefrom_temp= (char *) realloc(dnamefrom_temp,(dnamefromlen + 1)*sizeof(char));
			lkname_temp= (char *) realloc(lkname_temp,(lknamelen + 1)*sizeof(char));
			lkinfo_temp= (char *) realloc(lkinfo_temp,(lkinfolen + 1)*sizeof(char));
			porttypefrom_temp = (char *) realloc(porttypefrom_temp,(porttypefromlen + 1)*sizeof(char));
			porttypeto_temp= (char *) realloc(porttypeto_temp,(porttypetolen + 1)*sizeof(char));

			if(lkinfo_temp==NULL||dnamefrom_temp==NULL || dnameto_temp==NULL ||portnbto_temp==NULL||portnbfrom_temp==NULL||porttypeto_temp==NULL||porttypefrom_temp==NULL||lkname_temp==NULL)
			{
				status=-10;
				rescode=ShowErrors (status, ociError, "  pointer allocation unsuccessful");
				rescode=-1;
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				if(portnbfrom_temp!=NULL)
					free(portnbfrom_temp);
				if(portnbto_temp!=NULL)
					free(portnbto_temp);
				if(dnamefrom_temp!=NULL)
					free(dnamefrom_temp);
				if(dnameto_temp!=NULL)
					free(dnameto_temp);
				if(lkname_temp!=NULL)
					free(lkname_temp);
				if(porttypefrom_temp!=NULL)
					free(porttypefrom_temp);
				if(porttypeto_temp!=NULL)
					free(porttypeto_temp);
				if(lkinfo_temp!=NULL)
					free(lkinfo_temp);

				GetErrorMess(appliName,"REALLOC UNSUCCESSFUL",ErrorMessage,1);
				return rescode;
			}						
			DefineByPos(stmthp,def,ociError,1,&linkid,&status);						
			DefineByPos(stmthp,def,ociError,2, dnamefrom_temp,dnamefromlen+1,&status);	
			DefineByPos(stmthp,def,ociError,3, dnameto_temp,dnametolen+1,&status);			
			DefineByPos(stmthp,def,ociError,4, portnbfrom_temp,portnbfromlen+1,&status);	
			DefineByPos(stmthp,def,ociError,5, porttypefrom_temp,porttypefromlen+1,&status);			
			DefineByPos(stmthp,def,ociError,6, portnbto_temp,portnbtolen+1,&status);		
			DefineByPos(stmthp,def,ociError,7, porttypeto_temp,porttypetolen+1,&status);		
			DefineByPos(stmthp,def,ociError,8, lkname_temp,lknamelen,&status);
			DefineByPos(stmthp,def,ociError,9,&bidirectional_link_used,&status);
			DefineByPos(stmthp,def,ociError,10,&lkused,&status);
			DefineByPos(stmthp,def,ociError,11, lkinfo_temp,lkinfolen+1,&status);
						
		}catch(Error err){
			sprintf(appliName,"GetMacroConnectivityRow_nodename");	///////
			rescode=ShowErrors (status, err.ociError, err.log);
			
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			else
				GetErrorMess(appliName,err.msg,ErrorMessage,1);
			
			if(strstr(err.msg,"NOT CONNECTED TO ANY DB")!=NULL)
			return -1;
		}
		
		if(status==OCI_SUCCESS)
			status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT,1, OCI_DEFAULT);
		if (status==OCI_SUCCESS || status==OCI_SUCCESS_WITH_INFO) 
		{
			RemoveSeparator(dnamefrom_temp,"?");
			RemoveSeparator(dnameto_temp, "?");
			RemoveSeparator(portnbfrom_temp,"?");
			RemoveSeparator(portnbto_temp,"?");
			RemoveSeparator(lkname_temp, "?");
			RemoveSeparator(lkinfo_temp,"?");
			RemoveSeparator(porttypefrom_temp,"?");
			RemoveSeparator(porttypeto_temp,"?");

			sprintf(Conn_row,"|linkid (I):%d |node_from (C):%s |node_to (C):%s |port_nbrfrom (C):%s |port_typefrom (C): %s|port_nbrto (C):%s |port_typeto (C):%s |link_type (C):%s |bidirectional_link_used (I):%d |lkused (I):%d |link_info (C):%s |",linkid,dnamefrom_temp,dnameto_temp,portnbfrom_temp,porttypefrom_temp,portnbto_temp,porttypeto_temp,lkname_temp,bidirectional_link_used, lkused,lkinfo_temp);

			if(portnbfrom_temp!=NULL)
				free(portnbfrom_temp);
			if(portnbto_temp!=NULL)
				free(portnbto_temp);
			if(dnamefrom_temp!=NULL)
				free(dnamefrom_temp);
			if(dnameto_temp!=NULL)
				free(dnameto_temp);
			if(lkname_temp!=NULL)
				free(lkname_temp);
			if(lkinfo_temp!=NULL)
				free(lkinfo_temp);
			if(porttypefrom_temp!=NULL)
				free(porttypefrom_temp);
			if(porttypeto_temp!=NULL)
				free(porttypeto_temp);
		} 
		else 
		{
			if(rescode==0)
			{
				rescode=0;
				strcpy(Conn_row,"NO_ROWS_SELECTED");
				GetErrorMess(appliName,"NO_ROWS_SELECTED",ErrorMessage,1);
			}
		}
		status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
		if(strstr(ErrorMessage,"NO_ROWS_SELECTED")==NULL)
		{
			if(rescode==0)
			{
				OCIReportError(ociError,appliName,ErrorMessage,0); 
			}
			else
			{
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			}
		}
		return rescode;
	}
/**************************************************************************************/
	//return mac row in  MacIP_row
	/**************************************************************************************/
	/**
	* Get the mac row in  IP_row, returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* @param ip_address : ip address about which you want to get some info
	* @param len_ip : length of the MacIP_row. If the buffer is too small, we put the size needed here.
	* @param IP_row : char* buffer where to put the result
	* @param IP_row : formatted as follows |db_column_name (db_column_type):db_column_value|
	* @param IP_row : db_column_type is I for int and C for char*
	* @param IP_row : equals to "NO_ROWS_SELECTED" if no rows selected
	* @param ErrorMessage : error message in case of failure
	* @return 0 if it is successful
	*/
EXTERN_CONFDB
	int GetIPInfoRow(char* ip_address,int &len_ip, char*  IP_row,char* ErrorMessage)
	{
		char appliName[100]="GetIPInfoRow";
		int rescode=0;
		sb4 ipnamelen=0;
		sb4 ipaddlen=0;
		sb4 subnetlen=0;
		sword status;
		OCIStmt *stmthp;
		OCIBind  *bnd1p = (OCIBind *) 0; 
		OCIDefine* def[3];
		OCIParam *parmdp;
		char * ipadd_temp=NULL;
		char * ipname_temp=NULL;
		char * subnet_temp=NULL;
		char selectIP[1000];
		if(ip_address==NULL)
		{
			GetErrorMess(appliName," IP add MUST be given",ErrorMessage,1);
			return -1;
		}
		try{
			HandleAlloc(ociEnv,&stmthp,ociError,&status);
			sprintf(selectIP,"select ipaddress||'?',subnet_info||'?', ipname||'?' from %s where ipaddress=:IPadd",IPINFO_TABLE);				
			StmtPrepare(stmthp,ociError, selectIP, &status);
			BindByName(stmthp,&bnd1p,ociError,":IPadd",ip_address,&status);
			StmtExecute(ociHdbc, stmthp, ociError, &status);
			ParamGet(stmthp, ociError, &parmdp, 1, &status);
			AttrGet(parmdp, &ipaddlen, ociError, &status);
			ParamGet(stmthp, ociError, &parmdp, 2, &status);
			AttrGet(parmdp, &subnetlen, ociError, &status);			
			ParamGet(stmthp, ociError, &parmdp, 3, &status);
			AttrGet(parmdp, &ipnamelen, ociError, &status);					

			ipadd_temp = (char *) realloc(ipadd_temp,(ipaddlen + 1)*sizeof(char));
			ipname_temp= (char *) realloc(ipname_temp,(ipnamelen + 1)*sizeof(char));
			subnet_temp = (char *) realloc(subnet_temp,(subnetlen + 1)*sizeof(char));

			if(ipadd_temp==NULL || ipname_temp==NULL ||subnet_temp==NULL)
			{
				status=-10;
				rescode=ShowErrors (status, ociError, "  pointer allocation unsuccessful");
				rescode=-1;
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				if(ipadd_temp!=NULL)
					free(ipadd_temp);
				if(ipname_temp!=NULL)
					free(ipname_temp);
				if(subnet_temp!=NULL)
					free(subnet_temp);

				GetErrorMess(appliName,"REALLOC UNSUCCESSFUL",ErrorMessage,1);
				return rescode;
			}
		
			DefineByPos(stmthp,def,ociError,1, ipadd_temp,ipaddlen + 1,&status);	
			DefineByPos(stmthp,def,ociError,2, subnet_temp, subnetlen + 1,&status);			
			DefineByPos(stmthp,def,ociError,3, ipname_temp,ipnamelen + 1,&status);	
						
		}catch(Error err){
			sprintf(appliName,"GetIPInfoRow");	///////
			rescode=ShowErrors (status, err.ociError, err.log);
			
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			else
				GetErrorMess(appliName,err.msg,ErrorMessage,1);
			
			if(strstr(err.msg,"NOT CONNECTED TO ANY DB")!=NULL)
			return -1;
		}
		
		if(status==OCI_SUCCESS)
			status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT,1,OCI_DEFAULT);

		if (status == OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO) 
		{	
			RemoveSeparator(ipname_temp,"?");
			RemoveSeparator(ipadd_temp,"?");
			RemoveSeparator(subnet_temp,"?");

			sprintf(IP_row,"|ipaddress (C):%s|subnet_info (C):%s |ipname (C):%s |",ipadd_temp,subnet_temp,ipname_temp);
		} 
		else 
		{
			if(rescode==0)
			{
				rescode=0;
				strcpy(IP_row,"NO_ROWS_SELECTED");
				GetErrorMess(appliName,"NO_ROWS_SELECTED",ErrorMessage,1);
			}
		}

		if(ipadd_temp!=NULL)
			free(ipadd_temp);
		if(ipname_temp!=NULL)
			free(ipname_temp);
		if(subnet_temp!=NULL)
			free(subnet_temp);

		status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
		if(strstr(ErrorMessage,"NO_ROWS_SELECTED")==NULL)
		{
			if(rescode==0)
			{
				OCIReportError(ociError,appliName,ErrorMessage,0); 
			}
			else
			{
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			}
		}
		return rescode;
	}
/*****************************************************************************************/
	/**
	* Get the link type row in  LkType_row, returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* @param lktype_name :link type name about which you want to get some info
	* @param len_lktype : length of the LkType_row. If the buffer is too small, we put the size needed here.
	* @param LkType_row:  char* buffer where to put the result
	* @param LkType_row: formatted as follows |db_column_name (db_column_type):db_column_value|
	* @param LkType_row : db_column_type is I for int and C for char*
	* @param LkType_row : equals to "NO_ROWS_SELECTED" if no rows selected
	* @param ErrorMessage : error message in case of failure
	* @return 0 if it is successful
	*/
EXTERN_CONFDB
	int GetLkTypeRow_lkname(char* lktype_name,int &len_lktype,char* LkType_row,char* ErrorMessage)
	{
		char appliName[100]="GetLkTypeRow_lkname";
		int link_nbr=0;
		int rescode=0;
		int linktypeID=0;
		sword status;
		OCIStmt *stmthp;
		OCIBind  *bnd1p = (OCIBind *) 0; 
		OCIDefine* def[3];
		sb4 lktypelen=0;
		OCIParam *parmdp;
		char * lktype_temp=NULL;
		char selectlktype[1000];
		if(lktype_name==NULL)
		{
			GetErrorMess(appliName," lktype_name MUST be given",ErrorMessage,1);
			return -1;
		}
		
		try{
			HandleAlloc(ociEnv,&stmthp,ociError,&status);
			sprintf(selectlktype,"select  link_name||'?',link_nbr,linktypeID from %s where link_name=:ltype",LINKTYPE_TABLE);
			StmtPrepare(stmthp,ociError, selectlktype, &status);
			BindByName(stmthp,&bnd1p,ociError,":ltype",lktype_name,&status);
			StmtExecute(ociHdbc, stmthp, ociError, &status);
						
			/* The next two statements describe the select-list item, dept, and return its length to allocate the memory*/
			ParamGet(stmthp, ociError, &parmdp, 1, &status);
			AttrGet(parmdp, &lktypelen, ociError, &status);
			
			lktype_temp = (char *) realloc(lktype_temp,(lktypelen + 1)*sizeof(char));
			if(lktype_temp==NULL)
			{
				status=-10;
				rescode=ShowErrors (status, ociError, "  pointer allocation unsuccessful");
				rescode=-1;
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				GetErrorMess(appliName,"REALLOC UNSUCCESSFUL",ErrorMessage,1);
				return rescode;
			}
			DefineByPos(stmthp,def,ociError,1, lktype_temp, lktypelen + 1,&status);	
			DefineByPos(stmthp,def,ociError,2,&link_nbr,&status);	
			DefineByPos(stmthp,def,ociError,3,&linktypeID,&status);	
						
		}catch(Error err){
			sprintf(appliName,"GetLkTypeRow_lkname");	///////
			rescode=ShowErrors (status, err.ociError, err.log);
			
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			else
				GetErrorMess(appliName,err.msg,ErrorMessage,1);
			
			if(strstr(err.msg,"NOT CONNECTED TO ANY DB")!=NULL)
			return -1;
		}
		
		if(status==OCI_SUCCESS)
			status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT,1,OCI_DEFAULT);

		if (status == OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO) 
		{	
			RemoveSeparator(lktype_temp,"?");
			sprintf(LkType_row,"|link_name (C):%s |link_nbr (I):%d |linktypeID (I) :%d |",lktype_temp,link_nbr,linktypeID);
		} 
		else 
		{
			if(rescode==0)
			{
				rescode=0;
				strcpy(LkType_row,"NO_ROWS_SELECTED");
				GetErrorMess(appliName,"NO_ROWS_SELECTED",ErrorMessage,1);
			}
		}
		if(lktype_temp!=NULL)
			free(lktype_temp);
		status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);

		if(strstr(ErrorMessage,"NO_ROWS_SELECTED")==NULL)
		{
			if(rescode==0)
			{
				OCIReportError(ociError,appliName,ErrorMessage,0); 
			}
			else
			{
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			}
		}
		return rescode;
	}
/*****************************************************************************************/
	/**
	* Get the link type row in  LkType_row,returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* @param lktype_nbr :link type id  about which you want to get some info
	* @param len_lktype : length of the LkType_row. If the buffer is too small, we put the size needed here.
	* @param LkType_row:  char* buffer where to put the result
	* @param LkType_row: formatted as follows |db_column_name (db_column_type):db_column_value|
	* @param LkType_row : db_column_type is I for int and C for char*
	* @param LkType_row : equals to "NO_ROWS_SELECTED" if no rows selected
	* @param ErrorMessage : error message in case of failure
	* @return 0 if it is successful
	*/
EXTERN_CONFDB
	int GetLkTypeRow_lknb(int lktype_nbr,int &len_lktype,char* LkType_row,char* ErrorMessage)
	{
		char appliName[100]="GetLkTypeRow_lknb";
		sword status;
		OCIStmt *stmthp;
		OCIBind  *bnd1p = (OCIBind *) 0; 
		OCIDefine* def[3];
		ub4 lktypelen=0;
		int linktypeID=0;
		int rescode=0;
		OCIParam *parmdp;
		char * lktype_temp=NULL;
		int link_nbr=0;
		char selectlktype[1000];

		try{
			HandleAlloc(ociEnv,&stmthp,ociError,&status);			
			sprintf(selectlktype,"select link_name||'?',link_nbr,linktypeID from %s where linktypeID=:ltype",LINKTYPE_TABLE);
			StmtPrepare(stmthp,ociError, selectlktype, &status);
			BindByName(stmthp,&bnd1p,ociError,":ltype",&lktype_nbr,&status);
			StmtExecute(ociHdbc, stmthp, ociError, &status);						
			ParamGet(stmthp, ociError, &parmdp, 1, &status);
			AttrGet(parmdp, (sb4*) &lktypelen, ociError, &status);
			
			lktype_temp=(char *) realloc(lktype_temp,(lktypelen + 1)*sizeof(char));
			if(lktype_temp==NULL)
			{
				status=-10;
				rescode=ShowErrors (status, ociError, "  pointer allocation unsuccessful");
				rescode=-1;
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				GetErrorMess(appliName,"REALLOC UNSUCCESSFUL",ErrorMessage,1);
				return rescode;
			}						
			DefineByPos(stmthp,def,ociError,1, lktype_temp, lktypelen + 1,&status);	
			DefineByPos(stmthp,def,ociError,2,&link_nbr,&status);	
			DefineByPos(stmthp,def,ociError,3,&linktypeID,&status);	
									
		}catch(Error err){
			sprintf(appliName,"GetLkTypeRow_lknb");	///////
			rescode=ShowErrors (status, err.ociError, err.log);
			
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			else
				GetErrorMess(appliName,err.msg,ErrorMessage,1);
			
			if(strstr(err.msg,"NOT CONNECTED TO ANY DB")!=NULL)
			return -1;
		}
		
		if(status==OCI_SUCCESS)
			status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT,1,OCI_DEFAULT);

		if (status == OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO) 
		{	
			RemoveSeparator(lktype_temp,"?");

			sprintf(LkType_row,"|link_name (C):%s |link_nbr (I):%d |linktypeID (I):%d |",lktype_temp,link_nbr,linktypeID);
		} 
		else 
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "OCIFETCH unsuccessful\n");
				strcpy(LkType_row,"NO_ROWS_SELECTED");
				GetErrorMess(appliName,"NO_ROWS_SELECTED",ErrorMessage,1);
			}
		}
		if(lktype_temp!=NULL)
			free(lktype_temp);
	
		status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);

		if(strstr(ErrorMessage,"NO_ROWS_SELECTED")==NULL)
		{
			if(rescode==0)
			{
				OCIReportError(ociError,"GetLkTypeRow_lknb ",ErrorMessage,0); 
			}
			else
			{
				OCIReportError(ociError,"GetLkTypeRow_lknb ",ErrorMessage,1); 
			}
		}
		return rescode;
	}
/*****************************************************************************************/
	/**
	* Get the link type decomposition for Composite link types in  LkType_row,returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* @param lktype_nbr :link type id  about which you want to get some info
	* @param len_array : length of the LkType_row. If the buffer is too small, we put the size needed here.
	* @param LkType_row:  char* buffer where to put the result
	* @param LkType_row: separated with '\\0', ex: lkname|linktypeid
	* @param LkType_row : equals to "NO_ROWS_SELECTED" if no rows selected
	* @param ErrorMessage : error message in case of failure
	* @return 0 if it is successful
	*/
EXTERN_CONFDB
	int GetLkTypeDecomposition_lknb(int lktype_nbr,int &len_array,char* LkType_row,char* ErrorMessage)
	{
		char appliName[100]="GetLkTypeDecomposition_lknb";
		int i = 0;
		int pos1=0;
		int pos2=0;
		int pos3=0;
		int lkname_null=0;
		int prefetch_rows=50;
		sword status;
		OCIStmt *stmthp;
		OCIBind  *bnd1p = (OCIBind *) 0; 
		OCIDefine* def[2];
		ub4 lktypelen=0;
		int rescode=0;
		OCIParam *parmdp;
		char * lktype_temp=NULL;
		char * lkTypeslist_temp=NULL;
		int link_nbr=0;
		char selectlktype[1000];

		try{
			HandleAlloc(ociEnv,&stmthp,ociError,&status);
			sprintf(selectlktype,"select e.link_name||'|'||e.linktypeid||'?' from %s t, %s e,%s f where mod(t.link_nbr,e.link_nbr)=0 and e.link_nbr!=t.link_nbr and e.link_nbr=f.prime_nb and t.linktypeID=:ltype",LINKTYPE_TABLE,LINKTYPE_TABLE,PRIME_NUMBER_TABLE);
			StmtPrepare(stmthp,ociError, selectlktype, &status);
			BindByName(stmthp,&bnd1p,ociError,":ltype",&lktype_nbr,&status);
			AttrSet(stmthp,&prefetch_rows,ociError, &status);
			StmtExecute(ociHdbc, stmthp, ociError, &status);
			ParamGet(stmthp, ociError, &parmdp, 1, &status);
			AttrGet(parmdp, (sb4*) &lktypelen, ociError, &status);

			lktype_temp=(char *) realloc(lktype_temp,(lktypelen + 1)*sizeof(char));

			if(lktype_temp==NULL)
			{
				status=-10;
				rescode=ShowErrors (status, ociError, "  pointer allocation unsuccessful");
				rescode=-1;
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				GetErrorMess(appliName,"REALLOC UNSUCCESSFUL",ErrorMessage,1);
				return rescode;
			}
			DefineByPos(stmthp,def,ociError,1, lktype_temp, lktypelen + 1,&status);	
									
		}catch(Error err){
			sprintf(appliName,"GetLkTypeDecomposition_lknb");	///////
			rescode=ShowErrors (status, err.ociError, err.log);
			
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			else
				GetErrorMess(appliName,err.msg,ErrorMessage,1);
			
			if(strstr(err.msg,"NOT CONNECTED TO ANY DB")!=NULL)
			return -1;
		}
		
		if(status==OCI_SUCCESS)
		{
			while(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
			{
				status =OCIStmtFetch2 (stmthp, ociError, 1, OCI_FETCH_NEXT,1, OCI_DEFAULT);
				if(status==OCI_SUCCESS)
				{
					RemoveSeparator(lktype_temp,"?");
					pos1=strlen(lktype_temp);
					pos3=pos2;
					pos2+=pos1+1;
					lkTypeslist_temp=(char*) realloc(lkTypeslist_temp,pos2*sizeof(char));
					if(lkTypeslist_temp==NULL)
					{
						status=-10;
						rescode=ShowErrors (status, ociError, "  pointer allocation unsuccessful");
						rescode=-1;
						free(lktype_temp);
						status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
						GetErrorMess(appliName,"REALLOC UNSUCCESSFUL",ErrorMessage,1);
						return rescode;	
					}
					else
					{
						memcpy(lkTypeslist_temp+pos3,lktype_temp,pos1+1);
						i++;
					}
				}
			}
			if(rescode==0)
			{	
                if(pos2>len_array)
				{
					len_array=pos2;
					rescode= -1;	
					free(lktype_temp);
					free(lkTypeslist_temp);
					status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
					GetErrorMess(appliName,"BUFFER_TOO_SMALL",ErrorMessage,1);
					return rescode;	
				}
				else
				{
					if(i>0)
					{
						memcpy(LkType_row,lkTypeslist_temp,pos2);
						len_array=pos2;
					}
					else
					{
						rescode=ShowErrors (status, ociError, "OCIFETCH unsuccessful");
						strcpy(LkType_row,"NO_ROWS_SELECTED");
						GetErrorMess(appliName,"NO_ROWS_SELECTED",ErrorMessage,1);

					}
				}
				free(lkTypeslist_temp);
				free(lktype_temp);
			}
		}
		status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
		if(strstr(ErrorMessage,"NO_ROWS_SELECTED")==NULL)
		{
			if(rescode==0)
			{
				OCIReportError(ociError,appliName,ErrorMessage,0); 
			}
			else
			{
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			}
		}

		return rescode;
	}
/*****************************************************************************************/
	/**
	* Get the list of devicename, deviceid  of the given device type, and returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* @param devitype : device type name
	* @param len_array : length of the devIDs_list. If the buffer is too small, we put the size needed here.
	* @param devIDs_list :  char* buffer where to put the result (list of functional deviceNames and deviceid,devicename|deviceid) separated by a '\\0'
	* @param devIDs_list : equals to "NO_ROWS_SELECTED" if no rows selected
	* @param ErrorMessage : error message in case of failure
	* @return 0 if it is successful
	*/
EXTERN_CONFDB
	int GetDeviceNamesPerType(char* devitype, int &len_array, char* devIDs_list,char* ErrorMessage)
	{
		char appliName[100]="GetDeviceNamesPerType";
		int i = 0;
		int rescode=0;
		int pos1=0;
		int pos3=0;
		int pos2=0;
		int len_devname=0;
		char* devName=NULL;
		char* devIDs_temp=NULL;
		sword status;
		OCIParam *parmdp;
		int prefetch_rows=2000;
		OCIStmt *stmthp;
		OCIBind  *bnd1p = (OCIBind *) 0; 
		OCIDefine* def;
		char selectdevID[1000];
		
		if(devitype==NULL)
		{
			GetErrorMess(appliName," device type MUST be given",ErrorMessage,1);
			return -1;
		}

		try{
			HandleAlloc(ociEnv,&stmthp,ociError,&status);
			sprintf(selectdevID,"select t.devicename||'|'||t.deviceid||'?' from %s t,%s f where t.devicetypeID=f.devicetypeID and f.devicetype=:dtype",LOGICAL_DEVICE_TABLE,DEVICETYPE_TABLE);
			StmtPrepare(stmthp,ociError,selectdevID, &status);
			BindByName(stmthp,&bnd1p,ociError,":dtype",devitype,&status);
			AttrSet(stmthp,&prefetch_rows,ociError, &status);
			StmtExecute(ociHdbc, stmthp, ociError, &status);						
			ParamGet(stmthp, ociError, &parmdp, 1, &status);
			AttrGet(parmdp, (sb4*) &len_devname, ociError, &status);
			
			devName=(char*)malloc((len_devname+1)*sizeof(char));
			if(devName!=NULL)
			DefineByPos(stmthp,&def,ociError,1, devName, len_devname+1, &status);
			else
			{
				status=-10;
				rescode=ShowErrors (status, ociError, "pointer allocation unsuccessful");
				rescode=-1;
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);

				GetErrorMess(appliName,"REALLOC UNSUCCESSFUL",ErrorMessage,1);
				return rescode;
			}
									
		}catch(Error err){
			sprintf(appliName,"GetDeviceNamesPerType");	///////
			rescode=ShowErrors (status, err.ociError, err.log);
			
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			else
				GetErrorMess(appliName,err.msg,ErrorMessage,1);
			
			if(strstr(err.msg,"NOT CONNECTED TO ANY DB")!=NULL)
			return -1;
		}
		
		if(status==OCI_SUCCESS)
		{
			while(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
			{
				status =OCIStmtFetch2 (stmthp, ociError, 1, OCI_FETCH_NEXT,1, OCI_DEFAULT);
				if(status==OCI_SUCCESS)
				{
					RemoveSeparator(devName,"?");
					pos1=strlen(devName);
					pos3=pos2;
					pos2+=pos1+1;					
					devIDs_temp=(char*) realloc(devIDs_temp,pos2*sizeof(char));

					if(devIDs_temp==NULL)
					{
						status=-10;
						rescode=ShowErrors (status, ociError, "  pointer allocation unsuccessful");
						rescode=-1;
						free(devName);
						status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
						GetErrorMess(appliName,"REALLOC UNSUCCESSFUL",ErrorMessage,1);
						return rescode;	
					}
					else
					{
						memcpy(devIDs_temp+pos3,devName,pos1+1);
						i++;
					}
				}
			}
			if(rescode==0)
			{
				if(pos2>len_array)
				{
					len_array=pos2;
					rescode= -1;	
					free(devName);
					free(devIDs_temp);
					status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
					GetErrorMess(appliName,"BUFFER_TOO_SMALL",ErrorMessage,1);
					return rescode;	
				}
				else
				{
					if(i>0)
					{
						memcpy(devIDs_list,devIDs_temp,pos2);
						len_array=pos2;
					}
					else
					{
						rescode=0;
						strcpy(devIDs_list,"NO_ROWS_SELECTED");
						GetErrorMess(appliName,"NO_ROWS_SELECTED",ErrorMessage,1);

					}
				}
				free(devIDs_temp);
				free(devName);
			}
		}
		status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
		if(strstr(ErrorMessage,"NO_ROWS_SELECTED")==NULL)
		{
			if(rescode==0)
			{
				OCIReportError(ociError,appliName,ErrorMessage,0); 
			}
			else
			{
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			}
		}
		return rescode;
	}
/*****************************************************************************************/
	/**
	* Get the list of deviceIDs  of the given device type,  returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* @param devitype : device type name
	* @param len_array : length of the devIDs_list. If the buffer is too small, we put the size needed here.
	* @param devIDs_list :  int* buffer where to put the result (list of deviceIDs)
	* @param devIDs_list : equals to (-1,-1) if no rows selected
	* @param ErrorMessage : error message in case of failure
	* @return 0 if it is successful
	*/
EXTERN_CONFDB
	int GetDeviceIDsPerType(char* devitype, int &len_array, int* devIDs_list,char* ErrorMessage)
	{
		char appliName[100]="GetDeviceIDsPerType";
		int i = 0;
		int j=0;
		int rescode=0;
		int devID=0;
		int* devIDs_temp=NULL;
		sword status;
		int nb_rdtrip=1;
		int prefetch_rows=2000;
		OCIStmt *stmthp;
		OCIBind  *bnd1p = (OCIBind *) 0; 
		OCIDefine* def;
		char selectdevID[1000];
		if(devitype==NULL)
		{
			GetErrorMess(appliName," device type MUST be given",ErrorMessage,1);
			return -1;
		}
		
		try{
			HandleAlloc(ociEnv,&stmthp,ociError,&status);
			sprintf(selectdevID,"select t.deviceid from %s t, %s f where t.devicetypeID=f.devicetypeID and f.devicetype=:dtype",LOGICAL_DEVICE_TABLE,DEVICETYPE_TABLE);
			StmtPrepare(stmthp,ociError,selectdevID, &status);
			BindByName(stmthp,&bnd1p,ociError,":dtype",devitype,&status);
			AttrSet(stmthp,&prefetch_rows,ociError, &status);
			StmtExecute(ociHdbc, stmthp, ociError, &status);
			DefineByPos(stmthp,&def,ociError,1, &devID,&status);
												
		}catch(Error err){
			sprintf(appliName,"GetDeviceIDsPerType");	///////
			rescode=ShowErrors (status, err.ociError, err.log);
			
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			else
				GetErrorMess(appliName,err.msg,ErrorMessage,1);
			
			if(strstr(err.msg,"NOT CONNECTED TO ANY DB")!=NULL)
			return -1;
		}
		
		if(status==OCI_SUCCESS)
		{
			devIDs_temp=(int*) realloc(devIDs_temp,prefetch_rows*sizeof(int));
			if(devIDs_temp==NULL)
			{
				status=-10;
				rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");
				rescode=-1;
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);

				GetErrorMess(appliName,"REALLOC UNSUCCESSFUL",ErrorMessage,1);
				return rescode;	
			}
			while(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
			{
				status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT,1, OCI_DEFAULT);
				if(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
				{
					devIDs_temp[i]=devID;	
					i++;
				}
				if(i>0 && i%100==0)
				{
					nb_rdtrip++;
					devIDs_temp=(int*) realloc(devIDs_temp,nb_rdtrip*prefetch_rows*sizeof(int));
					if(devIDs_temp==NULL)
					{
						status=-10;
						rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");
						rescode=-1;
						status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);

						GetErrorMess(appliName,"REALLOC UNSUCCESSFUL",ErrorMessage,1);
						return rescode;
					}
				}
			}
			if(rescode==0)
			{
				if(i>len_array)
				{
					len_array=i;
					rescode=-1;
					status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
					devIDs_temp=(int*) realloc(devIDs_temp,0*sizeof(int));
					GetErrorMess(appliName,"BUFFER_TOO_SMALL",ErrorMessage,1);
					return rescode;	
				}
				else
				{
					if(i>0)
					{	
						for(j=0;j<i;j++)
						{
							devIDs_list[j]=devIDs_temp[j];
						}
						len_array=i;
					}
					else
					{
						rescode=0;
						devIDs_list[0]=-1;
						devIDs_list[1]=-1;
						GetErrorMess(appliName,"NO_ROWS_SELECTED",ErrorMessage,1);
					}
				}
				free(devIDs_temp);
			}
		}
		status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
		if(strstr(ErrorMessage,"NO_ROWS_SELECTED")==NULL)
		{
			if(rescode==0)
			{
				OCIReportError(ociError,appliName,ErrorMessage,0); 
			}
			else
			{
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			}
		}
		return rescode;
	}

/*****************************************************************************************/
	/**
	* Get the list of LkID  which start from the given deviceID, returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* @param node_from : deviceID
	* @param len_array : length of the lkfrom_list. If the buffer is too small, we put the size needed here.
	* @param lkfrom_list :  int* buffer where to put the result (list of deviceIDs)
	* @param lkfrom_list : equals to (-1,-1) if no rows selected
	* @param ErrorMessage : error message in case of failure
	* @return 0 if it is successful
	*/
EXTERN_CONFDB
	int GetLkFromDevID(int node_from, int &len_array, int* lkfrom_list,char* ErrorMessage)
	{
		char appliName[100]="GetLkFromDevID";
		int i = 0;
		int j=0;
		int rescode=0;
		int lkID=0;
		int* lkIDs_temp=NULL;
		sword status;
		int nb_rdtrip=1;
		int prefetch_rows=2000;
		OCIStmt *stmthp;
		OCIBind  *bnd1p = (OCIBind *) 0; 
		OCIDefine* def;
		char select_query[1000];

		try{
			HandleAlloc(ociEnv,&stmthp,ociError,&status);
			sprintf(select_query,"select t.linkid from %s t,%s e where t.portIDfrom=e.portid and e.deviceid=:nfrom and e.port_way=2",MACRO_CONNECTIVITY_TABLE,PORT_TABLE);
			StmtPrepare(stmthp,ociError,select_query, &status);
			BindByName(stmthp,&bnd1p,ociError,":nfrom",&node_from,&status);
			AttrSet(stmthp,&prefetch_rows,ociError, &status);
			StmtExecute(ociHdbc, stmthp, ociError, &status);
			DefineByPos(stmthp,&def,ociError,1, &lkID,&status);
												
		}catch(Error err){
			sprintf(appliName,"GetLkFromDevID");	///////
			rescode=ShowErrors (status, err.ociError, err.log);
			
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			else
				GetErrorMess(appliName,err.msg,ErrorMessage,1);
			
			if(strstr(err.msg,"NOT CONNECTED TO ANY DB")!=NULL)
			return -1;
		}
		
		if(status==OCI_SUCCESS)
		{
			lkIDs_temp=(int*) realloc(lkIDs_temp,prefetch_rows*sizeof(int));
			if(lkIDs_temp==NULL)
			{
				rescode=ShowErrors (status, ociError, "  pointer allocation unsuccessful");
				rescode=-1;
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);

				GetErrorMess(appliName,"REALLOC UNSUCCESSFUL",ErrorMessage,1);
				return rescode;
			}
			while(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
			{	
				status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT,1,OCI_DEFAULT);
				if(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
				{
					lkIDs_temp[i]=lkID;	
					i++;
				}
				if(i>0 && i%100==0)
				{
					nb_rdtrip++;
					lkIDs_temp=(int*) realloc(lkIDs_temp,nb_rdtrip*prefetch_rows*sizeof(int));
					if(lkIDs_temp==NULL)
					{
						rescode=ShowErrors (status, ociError, "  pointer allocation unsuccessful");
						rescode=-1;
						status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
						GetErrorMess(appliName,"REALLOC UNSUCCESSFUL",ErrorMessage,1);
						return rescode;
					}
				}
			}
			if(rescode==0)
			{
				if(i>len_array)
				{
					len_array=i;
					rescode=-1;
					lkIDs_temp=(int*) realloc(lkIDs_temp,0*sizeof(int));
					status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
					GetErrorMess(appliName,"BUFFER_TOO_SMALL",ErrorMessage,1);
					return rescode;
				}
				else
				{
					if(i>0)
					{
						for(j=0;j<i;j++)
						{
							lkfrom_list[j]=lkIDs_temp[j];
						}
						len_array=i;	
					}
					else
					{	
						rescode=0;
						lkfrom_list[0]=-1;
						lkfrom_list[1]=-1;
						GetErrorMess(appliName,"NO_ROWS_SELECTED",ErrorMessage,1);
					}
				}
				free(lkIDs_temp);
			}
		}
		status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
		if(strstr(ErrorMessage,"NO_ROWS_SELECTED")==NULL)
		{
			if(rescode==0)
			{
				OCIReportError(ociError,appliName,ErrorMessage,0); 
			}
			else
			{
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			}
		}
		return rescode;
	}
/***************************************************************************************/
	//returns all the links which end to that node
	/*****************************************************************************************/
	/**
	* Get the list of LkID  which end at the given deviceID, returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* @param node_to : deviceID
	* @param len_array : length of the lkfrom_list. If the buffer is too small, we put the size needed here.
	* @param lkfrom_list :  int* buffer where to put the result (list of deviceIDs)
	* @param lkfrom_list : equals to (-1,-1) if no rows selected
	* @param ErrorMessage : error message in case of failure
	* @return 0 if it is successful
	*/
EXTERN_CONFDB
	int GetLkToDevID(int node_to, int &len_array, int* lkfrom_list,char* ErrorMessage)
	{
		char appliName[100]="GetLkToDevID";
		int i =0;
		int j=0;
		int rescode=0;
		int lkID=0;
		int* lkIDs_temp=NULL;
		sword status;
		int nb_rdtrip=1;
		int prefetch_rows=2000;
		OCIStmt *stmthp;
		OCIBind  *bnd1p = (OCIBind *) 0; 
		OCIDefine* def;
		char select_query[1000];
		
		try{
			HandleAlloc(ociEnv,&stmthp,ociError,&status);
			sprintf(select_query,"select t.linkid from %s t,%s e where t.portIDTO=e.portid and e.deviceid=:nTO and e.port_way=1",MACRO_CONNECTIVITY_TABLE,PORT_TABLE);
			StmtPrepare(stmthp,ociError,select_query, &status);
			BindByName(stmthp,&bnd1p,ociError,":nto",&node_to,&status);
			AttrSet(stmthp,&prefetch_rows,ociError, &status);
			StmtExecute(ociHdbc, stmthp, ociError, &status);
			DefineByPos(stmthp,&def,ociError,1, &lkID,&status);
												
		}catch(Error err){
			sprintf(appliName,"GetLkToDevID");	///////
			rescode=ShowErrors (status, err.ociError, err.log);
			
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			else
				GetErrorMess(appliName,err.msg,ErrorMessage,1);
			
			if(strstr(err.msg,"NOT CONNECTED TO ANY DB")!=NULL)
			return -1;
		}
		if(status==OCI_SUCCESS)
		{
			lkIDs_temp=(int*) realloc(lkIDs_temp,prefetch_rows*sizeof(int));
			i=0;
			if(lkIDs_temp==NULL)
			{
				status=-10;
				rescode=ShowErrors (status, ociError, "  pointer allocation unsuccessful");
				rescode=-1;
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				GetErrorMess(appliName,"REALLOC UNSUCCESSFUL",ErrorMessage,1);
				return rescode;
			}
			while(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
			{
				status =OCIStmtFetch (stmthp, ociError, 1, OCI_FETCH_NEXT, OCI_DEFAULT);
				if(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
				{
					lkIDs_temp[i]=lkID;	
					i++;
				}
				if(i>0 && i%100==0)
				{
					nb_rdtrip++;
					lkIDs_temp=(int*) realloc(lkIDs_temp,nb_rdtrip*prefetch_rows*sizeof(int));
					if(lkIDs_temp==NULL)
					{
						status=-10;
						rescode=ShowErrors (status, ociError, "  pointer allocation unsuccessful");
						rescode=-1;
						status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
						GetErrorMess(appliName,"REALLOC UNSUCCESSFUL",ErrorMessage,1);
						return rescode;	
					}
				}
			}
			if(rescode==0)
			{
				if(i>len_array)
				{
					len_array=i;
					rescode=-1;
					lkIDs_temp=(int*) realloc(lkIDs_temp,0*sizeof(int));
					status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
					GetErrorMess(appliName,"BUFFER_TOO_SMALL",ErrorMessage,1);
					return rescode;	
				}
				else
				{
					if(i>0)
					{
						for(j=0;j<i;j++)
						{
							lkfrom_list[j]=lkIDs_temp[j];
						}
						len_array=i;	
					}
					else
					{
						rescode=0;
						lkfrom_list[0]=-1;
						lkfrom_list[1]=-1;
						GetErrorMess(appliName,"NO_ROWS_SELECTED",ErrorMessage,1);
					}
				}
				free(lkIDs_temp);
			}
		}
		status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
		if(strstr(ErrorMessage,"NO_ROWS_SELECTED")==NULL)
		{
			if(rescode==0)
			{
				OCIReportError(ociError,appliName,ErrorMessage,0); 
			}
			else
			{
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			}
		}
		return rescode;
	}
/****************************************************************************************/
	/**
	* Get the list of portID  of the given deviceID, returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* @param devID : deviceID
	* @param len_array : length of the portID_list. If the buffer is too small, we put the size needed here.
	* @param portID_list :  int* buffer where to put the result (list of deviceIDs)
	* @param portID_list : equals to  (-1,-1) if no rows selected
	* @param ErrorMessage : error message in case of failure
	* @return 0 if it is successful
	*/
EXTERN_CONFDB
	int GetPortIDPerDevID(int devID, int &len_array, int* portID_list,char* ErrorMessage)
	{
		char appliName[100]="GetPortIDPerDevID";
		int i = 0;
		int j=0;
		int rescode=0;
		int portID=0;
		int* portIDs_temp=NULL;
		sword status;
		int nb_rdtrip=1;
		int prefetch_rows=2000;
		OCIStmt *stmthp;
		OCIBind  *bnd1p = (OCIBind *) 0; 
		OCIDefine* def;
		char select_query[1000];
		
		try{
			HandleAlloc(ociEnv,&stmthp,ociError,&status);
			sprintf(select_query,"select portid from %s where deviceid=:dID",PORT_TABLE);
			StmtPrepare(stmthp,ociError,select_query, &status);
			BindByName(stmthp,&bnd1p,ociError,":dID",&devID,&status);
			AttrSet(stmthp,&prefetch_rows,ociError, &status);
			StmtExecute(ociHdbc, stmthp, ociError, &status);
			DefineByPos(stmthp,&def,ociError,1, &portID,&status);												
		}catch(Error err){
			sprintf(appliName,"GetPortIDPerDevID");	///////
			rescode=ShowErrors (status, err.ociError, err.log);
			
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			else
				GetErrorMess(appliName,err.msg,ErrorMessage,1);
			
			if(strstr(err.msg,"NOT CONNECTED TO ANY DB")!=NULL)
			return -1;
		}
		
		if(status==OCI_SUCCESS)
		{
			portIDs_temp=(int*) realloc(portIDs_temp,prefetch_rows*sizeof(int));
			if(portIDs_temp==NULL)
			{
				status=-10;
				rescode=ShowErrors (status, ociError, "  pointer allocation unsuccessful");
				rescode=-1;
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				GetErrorMess(appliName,"REALLOC UNSUCCESSFUL",ErrorMessage,1);
				return rescode;
			}
			while(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
			{
				status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT, 1,OCI_DEFAULT);
				if(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
				{
					portIDs_temp[i]=portID;	
					i++;
				}
				if(i>0 && i%100==0)
				{
					nb_rdtrip++;
					portIDs_temp=(int*) realloc(portIDs_temp,nb_rdtrip*prefetch_rows*sizeof(int));
					if(portIDs_temp==NULL)
					{
						status=-10;
						rescode=ShowErrors (status, ociError, "  pointer allocation unsuccessful");
						rescode=-1;
						status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
						GetErrorMess(appliName,"REALLOC UNSUCCESSFUL",ErrorMessage,1);
						return rescode;	
					}
				}
			}
			if(rescode==0)
			{
				if(i>len_array)
				{
					len_array=i;
					rescode=-1;
					portIDs_temp=(int*) realloc(portIDs_temp,0*sizeof(int));
					status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
					GetErrorMess(appliName,"BUFFER_TOO_SMALL",ErrorMessage,1);
					return rescode;
				}
				else
				{
					if(i>0)
					{
						for(j=0;j<i;j++)
						{
							portID_list[j]=portIDs_temp[j];
						}	
						len_array=i;
					}
					else
					{
						rescode=0;			
						portID_list[0]=-1;
						portID_list[1]=-1;
						GetErrorMess(appliName,"NO_ROWS_SELECTED",ErrorMessage,1);
					}
				}
				free(portIDs_temp);
			}
		}
		status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
		if(strstr(ErrorMessage,"NO_ROWS_SELECTED")==NULL)
		{
			if(rescode==0)
			{
				OCIReportError(ociError,appliName,ErrorMessage,0); 
			}
			else
			{
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			}
		}
		return rescode;
	}
/****************************************************************************************/
	/**
	* Get the list of spare port information  of the given serialnb, returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* @param  serialnb : serialnb of the spare
	* @param len_array : length of the portID_list. If the buffer is too small, we put the size needed here.
	* @param port_list :  char* buffer where to put the result each row is separated '\\0'. It is formatted as followed ptnb|port_type|port_way|macaddress|bia
	* @param port_list :  'NO_ROWS_SELECTED' if there is no port
	* @param ErrorMessage : error message in case of failure
	* @return 0 if it is successful
	*/
EXTERN_CONFDB
	int GetPortInfoPerSpare(char* serialnb, int &len_array, char* port_list,char* ErrorMessage)
	{
		char appliName[100]="GetPortInfoPerSpare";
		int i = 0;
		int rescode=0;
		int len_port=0;
		int pos1=0;
		int pos2=0;
		int pos3=0;
		char* port_info=NULL;
		char* portInfos_temp=NULL;
		sword status;
		int prefetch_rows=2000;
		OCIStmt *stmthp;
		OCIBind  *bnd1p = (OCIBind *) 0; 
		OCIDefine* def;
		OCIParam *parmdp;
		char select_query[1000];
		
		try{
			HandleAlloc(ociEnv,&stmthp,ociError,&status);
			sprintf(select_query,"select port_nbr||'|'||nvl(port_type,'none')||'|'||port_way||'|'||nvl(macaddress,'none')||'|'||nvl(bia, 'none')||'?' from %s where serialnb=:snb",HWPORT_TABLE);
			StmtPrepare(stmthp,ociError,select_query, &status);
			BindByName(stmthp,&bnd1p,ociError,":snb",serialnb,&status);
			AttrSet(stmthp,&prefetch_rows,ociError, &status);
			StmtExecute(ociHdbc, stmthp, ociError, &status);
			/* The next two statements describe the select-list item, dept, and return its length to allocate the memory*/
			ParamGet(stmthp, ociError, &parmdp, 1, &status);
			AttrGet(parmdp, (sb4*) &len_port, ociError, &status);
			
			port_info=(char*) malloc((len_port+1)*sizeof(char));
			if(port_info!=NULL)
				DefineByPos(stmthp,&def,ociError,1, port_info,len_port+1,&status);												
			else
			{
				status=-10;
				rescode=ShowErrors (status, ociError, "  pointer allocation unsuccessful");
				rescode=-1;
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				GetErrorMess(appliName,"MALLOC UNSUCCESSFUL",ErrorMessage,1);
				return rescode;
			}				
		}catch(Error err){
			sprintf(appliName,"GetPortInfoPerSpare");	///////
			rescode=ShowErrors (status, err.ociError, err.log);
			
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			else
				GetErrorMess(appliName,err.msg,ErrorMessage,1);
			
			if(strstr(err.msg,"NOT CONNECTED TO ANY DB")!=NULL)
			return -1;
		}
		if(status==OCI_SUCCESS)
		{
			while(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
			{
				status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT, 1,OCI_DEFAULT);
				if(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
				{
					RemoveSeparator(port_info,"?");
					pos1=strlen(port_info);
					pos3=pos2;
					pos2+=pos1+1;					
					portInfos_temp=(char*) realloc(portInfos_temp,(pos2)*sizeof(char));
					if(portInfos_temp==NULL)
					{
						status=-10;
						rescode=ShowErrors (status, ociError, "  pointer allocation unsuccessful");
						rescode=-1;
						free(port_info);
						status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
						GetErrorMess(appliName,"REALLOC UNSUCCESSFUL",ErrorMessage,1);
						return rescode;
					}
					else
					{
						memcpy(portInfos_temp+pos3,port_info,pos1+1);
						i++;
					}
				}
			}
			if(rescode==0)
			{
				if(pos2>len_array)
				{
					len_array=i;
					rescode=-1;
					free(port_info);
					free(portInfos_temp);
					status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
					GetErrorMess(appliName,"BUFFER_TOO_SMALL",ErrorMessage,1);
					return rescode;
				}
				else
				{
					if(pos2>0)
					{
						memcpy(port_list,portInfos_temp,pos2);
						len_array=pos2;
					}
					else
					{
						rescode=ShowErrors (status, ociError, "OCIFETCH unsuccessful");
						GetErrorMess(appliName,"NO_ROWS_SELECTED",ErrorMessage,1);
						strcpy(port_list,"NO_ROWS_SELECTED");
					}
				}
				free(port_info);
				free(portInfos_temp);
			}
		}
		status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
		if(strstr(ErrorMessage,"NO_ROWS_SELECTED")==NULL)
		{
			if(rescode==0)
			{
				OCIReportError(ociError,appliName,ErrorMessage,0); 
			}
			else
			{
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			}
		}
		return rescode;
	}
/****************************************************************************************/
	/**
	* Get the list of ethernet @ and ip @ concatenated with port_nbr and port_way of the given deviceID,  returning an integer value.
	* The user should manage the memory : there is no memory allocation. // useful to load the ARP table of a device
	* @param devID : deviceID
	* @param len_mac : length of the MacAdd_list. If the buffer is too small, we put the size needed here.
	* @param MacAdd_list :  char* buffer where to put the result : port_nbr|port_way|port_type|macaddress|ipaddress. each row is separated by '\\0'
	* @param MacAdd_list  : equals to "NO_ROWS_SELECTED" if no rows selected
	* @param ErrorMessage : error message in case of failure
	* @return 0 if it is successful
	*/
EXTERN_CONFDB
	int GetMacIPAddPerDevID(int devID, int &len_mac,char* MacAdd_list,char* ErrorMessage)
	{
		char appliName[100]="GetMacIPAddPerDevID";
		int i = 0;
		int rescode=0;
		int pos1=0;
		int pos2=0;
		int pos3=0;
		int etherlen=0;
		char* etheradd=NULL;
		char* MacAdds_temp=NULL;
		sword status;
		OCIParam *parmdp;
		int prefetch_rows=2000;
		OCIStmt *stmthp;
		OCIBind  *bnd1p = (OCIBind *) 0; 
		OCIDefine* def;
		char select_query[1000];
		
		try{
			HandleAlloc(ociEnv,&stmthp,ociError,&status);
			sprintf(select_query,"select t.port_nbr||'|'||t.port_way||'|'||nvl(t.port_type,'none')||'|'||f.macaddress||'|'||t.ipaddress||'?' from %s t,%s f,%s d where d.deviceid=:dID and d.serialnb=f.serialnb and d.deviceid=t.deviceid and f.port_nbr=t.port_nbr and nvl(f.port_type,'none')=nvl(t.port_type,'none') and f.port_way=t.port_way and t.ipaddress is not null ",PORT_TABLE,HWPORT_TABLE,LOGICAL_DEVICE_TABLE);
			StmtPrepare(stmthp,ociError,select_query, &status);
			BindByName(stmthp,&bnd1p,ociError,":dID",&devID,&status);
			AttrSet(stmthp,&prefetch_rows,ociError, &status);
			StmtExecute(ociHdbc, stmthp, ociError, &status);
			ParamGet(stmthp, ociError, &parmdp, 1, &status);
			AttrGet(parmdp, (sb4*) &etherlen, ociError, &status);
						
			etheradd=(char*)realloc(etheradd,(etherlen+1)*sizeof(char));
			if(etheradd==NULL)
			{
				rescode=ShowErrors (status, ociError, "  pointer allocation unsuccessful");
				rescode=-1;
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				GetErrorMess(appliName,"REALLOC UNSUCCESSFUL",ErrorMessage,1);
				return rescode;	
			}
			else
				//add 2 because 1 for \0 and 1 for ?
				DefineByPos(stmthp,&def,ociError,1, etheradd,etherlen+1,&status);																			
		}catch(Error err){
			sprintf(appliName,"GetMacIPAddPerDevID");	///////
			rescode=ShowErrors (status, err.ociError, err.log);
			
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			else
				GetErrorMess(appliName,err.msg,ErrorMessage,1);
			
			if(strstr(err.msg,"NOT CONNECTED TO ANY DB")!=NULL)
			return -1;
		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				etheradd=(char*)realloc(etheradd,0*sizeof(char));
			}
		}
		else
		{
			while(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
			{
				status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT,1, OCI_DEFAULT);
				if(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
				{
					RemoveSeparator(etheradd,"?");
					pos1=strlen(etheradd);
					pos3=pos2;
					pos2+=pos1+1;					
					
					MacAdds_temp=(char*) realloc(MacAdds_temp,pos2*sizeof(char));
					if(MacAdds_temp==NULL)
					{
						status=-10;
						rescode=ShowErrors (status, ociError, "  pointer allocation unsuccessful");
						rescode=-1;
						etheradd=(char*)realloc(etheradd,0*sizeof(char));
						status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
						GetErrorMess(appliName,"REALLOC UNSUCCESSFUL",ErrorMessage,1);
						return rescode;	
					}
					else
					{
						memcpy(MacAdds_temp+pos3,etheradd,pos1+1);
						i++;//
					}
				}
			}
			if(rescode==0)
			{
				if(pos2>len_mac)
				{
					len_mac=pos2;
					rescode=-1;
					free(etheradd);
					if(MacAdds_temp!=NULL)
						free(MacAdds_temp);
					status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
					GetErrorMess(appliName,"BUFFER_TOO_SMALL",ErrorMessage,1);
					return rescode;		
				}
				else
				{
					if(i>0)
					{
						memcpy(MacAdd_list,MacAdds_temp,pos2);
						len_mac=pos2;
					}
					else
					{
						strcpy(MacAdd_list,"NO_ROWS_SELECTED");
						GetErrorMess(appliName,"NO_ROWS_SELECTED",ErrorMessage,1);
					}
				}
			}
		}
		if(etheradd!=NULL)
			free(etheradd);
		if(MacAdds_temp!=NULL)
			free(MacAdds_temp);
			
		status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);

		if(strstr(ErrorMessage,"NO_ROWS_SELECTED")==NULL)
		{
			if(rescode==0)
			{
				OCIReportError(ociError,appliName,ErrorMessage,0); 
			}
			else
			{
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			}
		}
		return (rescode+status);
	}

/****************************************************************************************/
	/**
	* Get the list of possible destination  by devicename,port_nb,  returning an integer value.
	* The user should manage the memory : there is no memory allocation.  case sensitive
	* @param devname : device name ex, "THOR_00"
	* @param port_nb : port nb of the device 
	* @param port_type : port type  which corresponds to the port_nbr (port_way is automatically=2). put "none" if there is not
	* @param len_Destin_list : length of the MacAdd_list. If the buffer is too small, we put the size needed here.
	* @param Destin_list :  char* buffer where to put the result : the different functional devicenames are separated  by '\\0'
	* @param Destin_list  : equals to "NO_ROWS_SELECTED" if no rows selected
	* @param ErrorMessage : error message in case of failure  
	* @return 0 if it is successful
	*/
EXTERN_CONFDB
	int GetDestinationNamePerDevPort(char* devname,char* port_nb,char* port_type, int &len_Destin_list,char* Destin_list,char* ErrorMessage)
	{
		char appliName[100]="GetDestinationNamePerDevPort";
		int i = 0;
		int rescode=0;
		int pos1=0;
		int pos2=0;
		int pos3=0;
		int devlen=0;
		char* destin=NULL;
		char* destin_temp=NULL;
		sword status;
		OCIParam *parmdp;
		int prefetch_rows=5000;
		OCIStmt *stmthp;
		OCIBind  *bnd1p = (OCIBind *) 0; 
		OCIDefine* def;
		char select_query[1000];
		if(devname==NULL)
		{
			GetErrorMess(appliName," devname MUST be given",ErrorMessage,1);
			return -1;
		}
		
		try{
			HandleAlloc(ociEnv,&stmthp,ociError,&status);
			sprintf(select_query,"select distinct s.devicename||'|' from %s t, %s s, %s l, %s g ,%s d,%s f where t.devicename=:dname and l.node1=t.deviceid and l.node11=s.deviceid and  g.deviceid=t.deviceid  and g.port_nbr=:ptnb and g.port_way=2 and g.portid=d.portidfrom and nvl(g.port_type,'none')=:ptype and d.portidto=f.portid and f.deviceid=l.node2", LOGICAL_DEVICE_TABLE,LOGICAL_DEVICE_TABLE,PATHDETAILS_TABLE,PORT_TABLE,MACRO_CONNECTIVITY_TABLE,PORT_TABLE);
			StmtPrepare(stmthp,ociError, select_query, &status);
			BindByName(stmthp,&bnd1p,ociError,":ptnb",port_nb,&status);
            BindByName(stmthp,&bnd1p,ociError,":ptype",port_type,&status);
			BindByName(stmthp,&bnd1p,ociError,":dname",devname,&status);
			AttrSet(stmthp,&prefetch_rows,ociError, &status);
			StmtExecute(ociHdbc, stmthp, ociError, &status);

			ParamGet(stmthp, ociError, &parmdp, 1, &status);
			AttrGet(parmdp, &devlen, ociError, &status);
			
			destin=(char*)realloc(destin,(devlen+1)*sizeof(char));
			if(destin==NULL)
			{
				status=-10;
				rescode=ShowErrors (status, ociError, "  pointer allocation unsuccessful");
				rescode=-1;
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				GetErrorMess(appliName,"REALLOC UNSUCCESSFUL",ErrorMessage,1);
				return rescode;		
			}
			else
				//add 2 because 1 for \0 and 1 for ?
				DefineByPos(stmthp,&def,ociError,1, destin,devlen+1,&status);										
		}catch(Error err){
			sprintf(appliName,"GetDestinationNamePerDevPort");	///////
			rescode=ShowErrors (status, err.ociError, err.log);
			
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			else
				GetErrorMess(appliName,err.msg,ErrorMessage,1);
			
			if(strstr(err.msg,"NOT CONNECTED TO ANY DB")!=NULL)
			return -1;
		}

		if(status==OCI_SUCCESS)
		{
			while(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
			{
				status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT,1,OCI_DEFAULT);
				if(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
				{
					RemoveSeparator(destin,"|");
					pos1=strlen(destin);
					pos3=pos2;
					pos2+=pos1+1;

					destin_temp=(char*) realloc(destin_temp,pos2*sizeof(char));
					if(destin_temp==NULL)
					{
						status=-10;
						rescode=ShowErrors (status, ociError, "  pointer allocation unsuccessful");
						destin=(char*)realloc(destin,(0)*sizeof(char));
						status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
						GetErrorMess(appliName,"REALLOC UNSUCCESSFUL",ErrorMessage,1);
						return rescode;	
					}
					else
					{
						memcpy(destin_temp+pos3,destin,pos1+1);
						i++;
					}
				}
			}
			if(i==0) //it means it doesn't fail due to empty cursors
			{
				if(rescode==0)
				{
					rescode=ShowErrors (status, ociError, "OCIFETCH  unsuccessful");
					strcpy(Destin_list,"NO_ROWS_SELECTED");
					GetErrorMess(appliName,"NO_ROWS_SELECTED",ErrorMessage,1);
				}
			}
			if(rescode==0 && i>0)
			{
				if(pos2>len_Destin_list)
				{
					len_Destin_list=pos2;
					free(destin);
					status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
					rescode=-1;
					if(destin_temp!=NULL)
						free(destin_temp);
					GetErrorMess(appliName,"BUFFER_TOO_SMALL",ErrorMessage,1);
					return rescode;	
				}
				else
				{
					memcpy(Destin_list,destin_temp,pos2);
					len_Destin_list=pos2;
				}
			}
		}
		if(destin!=NULL)
			free(destin);
		status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
		if(strstr(ErrorMessage,"NO_ROWS_SELECTED")==NULL)
		{
			if(rescode==0)
			{
				OCIReportError(ociError,appliName,ErrorMessage,0); 
			}
			else
			{
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			}
		}
		return (rescode+status);
	}
/*****************************************************************************************/
	/**
	* Get the list of free devices  of the given device type, returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* @param devitype : device type name
	* @param len_array : length of the devIDs_list. If the buffer is too small, we put the size needed here.
	* @param devIDs_list :  char* buffer where to put the result (list of deviceIDs), elements separated by '\\0'
	* @param devIDs_list : NO_ROWS_SELECTED is set in case there is no free devices
	* @param ErrorMessage : error message in case of failure
	* @return 0 if it is successful
	*/
EXTERN_CONFDB
	int GetFreeDeviceNamesPerType(char* devitype, int &len_array, char* devIDs_list,char* ErrorMessage)
	{
		char appliName[100]="GetFreeDeviceNamesPerType";
		int i = 0;
		int rescode=0;
		int pos1=0;
		int pos3=0;
		int pos2=0;
		int len_devname=0;
		char* devName=NULL;
		char* devIDs_temp=NULL;
		sword status;
		int prefetch_rows=2000;
		OCIStmt *stmthp;
		OCIBind  *bnd1p = (OCIBind *) 0; 
		OCIDefine* def;
		OCIParam *parmdp;
		char selectdevID[1000];
		if(devitype==NULL)
		{
			GetErrorMess(appliName," device type MUST be given",ErrorMessage,1);
			return -1;
		}

		try{
			HandleAlloc(ociEnv,&stmthp,ociError,&status);
			sprintf(selectdevID,"select t.devicename||'|' from %s t, %s f where t.devicetypeID=f.devicetypeID and f.devicetype=:dtype and t.active=0 and t.nodeused=1",LOGICAL_DEVICE_TABLE,DEVICETYPE_TABLE);
			StmtPrepare(stmthp,ociError,selectdevID, &status);
			BindByName(stmthp,&bnd1p,ociError,":dtype",devitype,&status);
			AttrSet(stmthp,&prefetch_rows,ociError, &status);
			StmtExecute(ociHdbc, stmthp, ociError, &status);
						
			ParamGet(stmthp, ociError, &parmdp, 1, &status);
			AttrGet(parmdp, &len_devname, ociError, &status);
			
			devName=(char*)malloc((len_devname+1)*sizeof(char));
			if(devName!=NULL)
				DefineByPos(stmthp,&def,ociError,1, devName,len_devname+1,&status);
			else
			{
				status=-10;
				rescode=ShowErrors (status, ociError, "pointer allocation unsuccessful");
				rescode=-1;
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				GetErrorMess(appliName,"REALLOC UNSUCCESSFUL",ErrorMessage,1);
				return rescode;	
			}			
		}catch(Error err){
			sprintf(appliName,"GetFreeDeviceNamesPerType");	///////
			rescode=ShowErrors (status, err.ociError, err.log);
			
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			else
				GetErrorMess(appliName,err.msg,ErrorMessage,1);
			
			if(strstr(err.msg,"NOT CONNECTED TO ANY DB")!=NULL)
			return -1;
		}
		if(status==OCI_SUCCESS)
		{
			while(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
			{
				status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT,1,OCI_DEFAULT);
				if(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
				{
					RemoveSeparator(devName,"|");
					pos1=strlen(devName);
					pos3=pos2;
					pos2+=pos1+1;				

					devIDs_temp=(char*) realloc(devIDs_temp,pos2*sizeof(char));
					if(devIDs_temp==NULL)
					{
						status=-10;
						rescode=ShowErrors (status, ociError, "  pointer allocation unsuccessful");
						rescode=-1;
						free(devName);
						status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
						GetErrorMess(appliName,"REALLOC UNSUCCESSFUL",ErrorMessage,1);
						return rescode;
					}
					else
					{
						memcpy(devIDs_temp+pos3,devName,pos1+1);
						i++;
					}
				}
			}
			if(rescode==0)
			{	
				if(pos2>len_array)
				{
					len_array=pos2;
					free(devIDs_temp);
					free(devName);
					status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
					GetErrorMess(appliName,"BUFFER_TOO_SMALL",ErrorMessage,1);
					return -1;
				}
				else
				{
					if(i>0)
					{
						memcpy(devIDs_list,devIDs_temp,pos2);
						len_array=pos2;
					}
					else
					{
						rescode=0;
						strcpy(devIDs_list,"NO_ROWS_SELECTED");
						GetErrorMess(appliName,"NO_ROWS_SELECTED",ErrorMessage,1);
					}
				}
				free(devIDs_temp);
				free(devName);
			}
		}
		status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
		if(strstr(ErrorMessage,"NO_ROWS_SELECTED")==NULL)
		{
			if(rescode==0)
			{
				OCIReportError(ociError,appliName,ErrorMessage,0); 
			}
			else
			{
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			}
		}
		return rescode;
	}
/*************************************************************************************/
	//return the deviceid 
	/*************************************************************************************/
	/**
	* Get the deviceid of the given device name in deviceID,  returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* @param deviname : device name: it's case sensitive.
	* @param deviceID : deviceID (to use if return value is 0) always>0
	* @param deviceID is set -1 if no matching device is found
	* @param ErrorMessage : error message in case of failure
	* @return 0 if it is successful
	*/
EXTERN_CONFDB
	int GetDeviceID_devicename(char* deviname,int &deviceID,char* ErrorMessage)
	{
		char appliName[100]="GetDeviceID_devicename";
		sword status;
		int rescode=0;
		OCIStmt *stmthp;
		OCIBind  *bnd1p = (OCIBind *) 0; 
		OCIDefine* def[1];
		char selectdev[1000];
		if(deviname==NULL)
		{
			GetErrorMess(appliName," device name MUST be given",ErrorMessage,1);
			return -1;
		}

		try{
			HandleAlloc(ociEnv,&stmthp,ociError,&status);
			sprintf(selectdev,"select deviceid from %s where devicename=:dname",LOGICAL_DEVICE_TABLE);
			StmtPrepare(stmthp,ociError, selectdev, &status);
			BindByName(stmthp,&bnd1p,ociError,":dname",deviname,&status);
            StmtExecute(ociHdbc, stmthp, ociError, &status);
			DefineByPos(stmthp,def,ociError,1, &deviceID,&status);										
		}catch(Error err){
			sprintf(appliName,"GetDeviceID_devicename");	///////
			rescode=ShowErrors (status, err.ociError, err.log);
			
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			else
				GetErrorMess(appliName,err.msg,ErrorMessage,1);
			
			if(strstr(err.msg,"NOT CONNECTED TO ANY DB")!=NULL)
			return -1;
		}

		if(status==OCI_SUCCESS)
			status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT,1, OCI_DEFAULT);

		if (status!=OCI_SUCCESS && status!=OCI_SUCCESS_WITH_INFO)  
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "Fetch unsuccessful");
				strcpy(ErrorMessage,"NO_ROWS_SELECTED");
				GetErrorMess(appliName,"NO_ROWS_SELECTED",ErrorMessage,1);
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				return 0;
			}
		}
		status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
		if(rescode==0)
		{
			OCIReportError(ociError,appliName,ErrorMessage,0); 
		}
		else
		{
			OCIReportError(ociError,appliName,ErrorMessage,1); 
		}
		rescode+=status;
		return rescode;
	}
/*************************************************************************************/
	//return the portid
	/*************************************************************************************/
	/**
	* Get the portid of the given deviceid, portnb, port_type,port_way,  returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* @param deviceID : deviceID of the fctnal device: it's case sensitive.
	* @param port_nb : port nb of the device
	* @param port_type : port type. put 'none', if it's NULL.
	* @param port_way : port way. 1 or 2
	* @param portID is set -1 if no matching device is found; always>0
	* @param ErrorMessage : error message in case of failure
	* @return 0 if it is successful
	*/
EXTERN_CONFDB
	int GetPortID_portinfo(int deviceID,char* port_nb,char* port_type,int port_way,int &portID,char* ErrorMessage)
	{
		char appliName[100]="GetPortID_portinfo";
		sword status;
		int rescode=0;
		OCIStmt *stmthp;
		OCIBind  *bnd1p[4]; 
		OCIDefine* def[1];
		char selectdev[1000];

		try{
			HandleAlloc(ociEnv,&stmthp,ociError,&status);
			sprintf(selectdev,"select portid from %s where deviceid=:devid and port_nbr=:pnb and nvl(port_type,'none')=:ptype and port_way=:pway",PORT_TABLE);
			StmtPrepare(stmthp,ociError, selectdev, &status);
			BindByName(stmthp,&bnd1p[0],ociError,":devid",&deviceID,&status);
			BindByName(stmthp,&bnd1p[1],ociError,":pnb",port_nb,&status);
			BindByName(stmthp,&bnd1p[2],ociError,":ptype",port_type,&status);
			BindByName(stmthp,&bnd1p[3],ociError,":pway",&port_way,&status);
			StmtExecute(ociHdbc, stmthp, ociError, &status);
			DefineByPos(stmthp,def,ociError,1, &portID,&status);																		
		}catch(Error err){
			sprintf(appliName,"GetPortID_portinfo");	///////
			rescode=ShowErrors (status, err.ociError, err.log);
			
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			else
				GetErrorMess(appliName,err.msg,ErrorMessage,1);
			
			if(strstr(err.msg,"NOT CONNECTED TO ANY DB")!=NULL)
			return -1;
		}

		if(status==OCI_SUCCESS)
			status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT,1, OCI_DEFAULT);

		if (status!=OCI_SUCCESS && status!=OCI_SUCCESS_WITH_INFO) 
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "Fetch unsuccessful");
				strcpy(ErrorMessage,"NO_ROWS_SELECTED");
				GetErrorMess(appliName,"NO_ROWS_SELECTED",ErrorMessage,1);
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				return rescode;
			}
		}
		status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
		if(rescode==0)
		{
			OCIReportError(ociError,appliName,ErrorMessage,0); 
		}
		else
		{
			OCIReportError(ociError,appliName,ErrorMessage,1); 
		}
		rescode+=status;
		return rescode;
	}
/*************************************************************************************/
	//return the devicename
	/*************************************************************************************/
	/**
	* Get the devicename corresponding to the given deviceID in devicename, returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* @param deviceID : deviceID always>0
	* @param devicename : device name (return); Error if failed
	* @param devicename : if nothing is found, devicename is set to NO_ROWS_SELECTED
	* @param ErrorMessage : error message in case of failure
	* @return 0 if it is successful
	*/
EXTERN_CONFDB
	int GetDeviceName_deviceid(int deviceID,char* devicename,char* ErrorMessage)
	{
		char appliName[100]="GetDeviceName_deviceid";
		sword status;
		int rescode=0;
		OCIStmt *stmthp;
		OCIBind  *bnd1p = (OCIBind *) 0; 
		OCIDefine* def[1];
		ub4 devlen=0;
		OCIParam *parmdp;
		char selectdev[1000];

		try{
			HandleAlloc(ociEnv,&stmthp,ociError,&status);
			sprintf(selectdev,"select devicename||'?' from %s where deviceid=:devid",LOGICAL_DEVICE_TABLE);
			StmtPrepare(stmthp,ociError, selectdev, &status);			
			BindByName(stmthp,&bnd1p,ociError,":devid",&deviceID,&status);			
			StmtExecute(ociHdbc, stmthp, ociError, &status);
			/* The next two statements describe the select-list item, dept, and return its length to allocate the memory*/
			ParamGet(stmthp, ociError, &parmdp, 1, &status);
			AttrGet(parmdp, (sb4*) &devlen, ociError, &status);	
			DefineByPos(stmthp,def,ociError,1, devicename,devlen+1,&status);	

		}catch(Error err){
			sprintf(appliName,"GetDeviceName_deviceid");	///////
			rescode=ShowErrors (status, err.ociError, err.log);
			
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			else
				GetErrorMess(appliName,err.msg,ErrorMessage,1);
			
			if(strstr(err.msg,"NOT CONNECTED TO ANY DB")!=NULL)
			return -1;
		}
		if(status==OCI_SUCCESS)
			status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT, 1,OCI_DEFAULT);

		if (status==OCI_SUCCESS || status==OCI_SUCCESS_WITH_INFO) 
            RemoveSeparator(devicename,"?");
		else 
		{
			if(rescode==0)
			{
				strcpy(devicename,"NO_ROWS_SELECTED");
				GetErrorMess(appliName,"NO_ROWS_SELECTED",ErrorMessage,1);
			}
		}
		status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
		if(strstr(ErrorMessage,"NO_ROWS_SELECTED")==NULL)
		{
			if(rescode==0)
			{
				OCIReportError(ociError,appliName,ErrorMessage,0); 
			}
			else
			{
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			}
		}
		rescode+=status;
		return rescode;
	}
/*****************************************************************************************/
	/**
	* Get the list of functional device names (only in IN_USE status)  which are at the given location, returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* @param location : location (you can put a prefix or the exact name)
	* @param len_array : length of the devIDs_list. If the buffer is too small, we put the size needed here.
	* @param devnames_list :  char* buffer where to put the result (list of devicenames). each line seaprated by '\\0'
	* @param ErrorMessage : error message in case of failure
	* @return 0 if it is successful
	*/
EXTERN_CONFDB
	int GetDeviceNamesPerLocation(char* location, int &len_array, char* devnames_list,char* ErrorMessage)
	{
		char appliName[100]="GetDeviceNamesPerLocation";
		int i = 0;
		int rescode=0;
		int pos1=0;
		int pos3=0;
		int pos2=0;
		int len_devname=0;
		char* devName=NULL;
		char* devIDs_temp=NULL;
		sword status;
		int prefetch_rows=2000;
		OCIStmt *stmthp;
		OCIBind  *bnd1p = (OCIBind *) 0; 
		OCIDefine* def;
		OCIParam *parmdp;
		char selectdevID[1000];
		if(location==NULL)
		{
			GetErrorMess(appliName," location MUST be given",ErrorMessage,1);
			return -1;
		}
		
		try{
			HandleAlloc(ociEnv,&stmthp,ociError,&status);
			sprintf(selectdevID,"select devicename||'|' from %s where instr(nvl(location,'none'),:location)>0",LOGICAL_DEVICE_TABLE);
			StmtPrepare(stmthp,ociError, selectdevID, &status);
			BindByName(stmthp,&bnd1p,ociError,":location", location,&status);
			AttrSet(stmthp,&prefetch_rows,ociError, &status);
			StmtExecute(ociHdbc, stmthp, ociError, &status);
			/* The next two statements describe the select-list item, dept, and return its length to allocate the memory*/
			ParamGet(stmthp, ociError, &parmdp, 1, &status);
			AttrGet(parmdp, &len_devname, ociError, &status);
			
			devName=(char*)malloc((len_devname+1)*sizeof(char));
			if(devName!=NULL)
				DefineByPos(stmthp,&def,ociError,1, devName,len_devname+1,&status);	
			else
			{
				status=-10;
				rescode=ShowErrors (status, ociError, "pointer allocation unsuccessful");
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				GetErrorMess(appliName,"REALLOC UNSUCCESSFUL",ErrorMessage,1);
				return -1;	
			}

		}catch(Error err){
			sprintf(appliName,"GetDeviceNamesPerLocation");	///////
			rescode=ShowErrors (status, err.ociError, err.log);
			
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			else
				GetErrorMess(appliName,err.msg,ErrorMessage,1);
			
			if(strstr(err.msg,"NOT CONNECTED TO ANY DB")!=NULL)
			return -1;
		}	

		if(status==OCI_SUCCESS)
		{
			while(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
			{
				status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT,1,OCI_DEFAULT);
				if(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
				{
					RemoveSeparator(devName,"|");
					pos1=strlen(devName);
					pos3=pos2;
					pos2+=pos1+1;
					devIDs_temp=(char*) realloc(devIDs_temp,pos2*sizeof(char));
					if(devIDs_temp==NULL)
					{
						status=-10;
						rescode=ShowErrors (status, ociError, "  pointer allocation unsuccessful");
						free(devName);
						status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
						GetErrorMess(appliName,"REALLOC UNSUCCESSFUL",ErrorMessage,1);
						return -1;
					}
					else
					{
						memcpy(devIDs_temp+pos3,devName,pos1+1);
						i++;
					}
				}
			}
			if(rescode==0)
			{	
				if(pos2>len_array)
				{
					len_array=pos2;
					free(devIDs_temp);					
					free(devName);
					status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
					GetErrorMess(appliName,"BUFFER_TOO_SMALL",ErrorMessage,1);
					return -1;
				}
				else
				{
					if(i>0)
					{
						memcpy(devnames_list,devIDs_temp,pos2);
						len_array=pos2;
					}
					else
					{
						rescode=0;
						strcpy(devnames_list,"NO_ROWS_SELECTED");
						GetErrorMess(appliName,"NO_ROWS_SELECTED",ErrorMessage,1);
					}
				}
				free(devIDs_temp);
				free(devName);
			}
		}
		status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
		if(strstr(ErrorMessage,"NO_ROWS_SELECTED")==NULL)
		{
			if(rescode==0)
			{
				OCIReportError(ociError,appliName,ErrorMessage,0); 
			}
			else
			{
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			}
		}
		return rescode;
	}
/*****************************************************************************************/
	/**
	* Get the list of functional device names (only in IN_USE status)  which are at the given location, returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* @param function : function of device. Put "none", for none (you can put a prefix or the exact name)
	* @param len_array : length of the devIDs_list. If the buffer is too small, we put the size needed here.
	* @param devnames_list :  char* buffer where to put the result (list of devicenames). each line seaprated by '\\0'
	* @param ErrorMessage : error message in case of failure
	* @return 0 if it is successful
	*/
EXTERN_CONFDB
	int GetDeviceNamesPerFunction(char* function, int &len_array, char* devnames_list,char* ErrorMessage)
	{
		char appliName[100]="GetDeviceNamesPerFunction";
		int i = 0;
		int rescode=0;
		int pos1=0;
		int pos3=0;
		int pos2=0;
		int len_devname=0;
		char* devName=NULL;
		char* devIDs_temp=NULL;
		sword status;
		int prefetch_rows=2000;
		OCIStmt *stmthp;
		OCIBind  *bnd1p = (OCIBind *) 0; 
		OCIDefine* def;
		OCIParam *parmdp;
		char selectdevID[1000];
		if(function==NULL)
		{
			GetErrorMess(appliName," function MUST be given",ErrorMessage,1);
			return -1;
		}
		
		try{
			HandleAlloc(ociEnv,&stmthp,ociError,&status);
			if(strstr(function,"none")==NULL)
				sprintf(selectdevID,"select t.devicename||'|' from %s t , %s e where mod(t.functionid,e.functionid)=0 and e.function_name=:fct and t.functionID!=0 ",LOGICAL_DEVICE_TABLE,DEVICE_FUNCTION_TABLE);
			else
				sprintf(selectdevID," select t.devicename||'|' from %s t where functionid=0",LOGICAL_DEVICE_TABLE);
			StmtPrepare(stmthp,ociError, selectdevID, &status);
			if(strstr(function,"none")==NULL)
				BindByName(stmthp,&bnd1p,ociError,":fct",function,&status);
			AttrSet(stmthp,&prefetch_rows,ociError, &status);
			StmtExecute(ociHdbc, stmthp, ociError, &status);
			/* The next two statements describe the select-list item, dept, and return its length to allocate the memory*/
			ParamGet(stmthp, ociError, &parmdp, 1, &status);
			AttrGet(parmdp, &len_devname, ociError, &status);
			devName=(char*)malloc((len_devname+1)*sizeof(char));
			if(devName!=NULL)
				DefineByPos(stmthp,&def,ociError,1, devName,len_devname+1,&status);	
			else
			{
				status=-10;
				rescode=ShowErrors (status, ociError, "pointer allocation unsuccessful");
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				GetErrorMess(appliName,"REALLOC UNSUCCESSFUL",ErrorMessage,1);
				return -1;	
			}
		}catch(Error err){
			sprintf(appliName,"GetDeviceNamesPerFunction");	///////
			rescode=ShowErrors (status, err.ociError, err.log);
			
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			else
				GetErrorMess(appliName,err.msg,ErrorMessage,1);
			
			if(strstr(err.msg,"NOT CONNECTED TO ANY DB")!=NULL)
			return -1;
		}	
		if(status==OCI_SUCCESS)
		{
			while(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
			{
				status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT,1,OCI_DEFAULT);
				if(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
				{
					RemoveSeparator(devName,"|");
					pos1=strlen(devName);
					pos3=pos2;
					pos2+=pos1+1;					
					devIDs_temp=(char*) realloc(devIDs_temp,pos2*sizeof(char));
					if(devIDs_temp==NULL)
					{
						status=-10;
						rescode=ShowErrors (status, ociError, "  pointer allocation unsuccessful");
						free(devName);
						status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
						GetErrorMess(appliName,"REALLOC UNSUCCESSFUL",ErrorMessage,1);
						return -1;
					}
					else
					{
						memcpy(devIDs_temp+pos3,devName,pos1+1);
						i++;
					}
				}
			}
			if(rescode==0)
			{	
				if(pos2>len_array)
				{
					len_array=pos2;
					free(devIDs_temp);
					free(devName);
					status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
					GetErrorMess(appliName,"BUFFER_TOO_SMALL",ErrorMessage,1);
					return -1;
				}
				else
				{
					if(i>0)
					{
						memcpy(devnames_list,devIDs_temp,pos2);
						len_array=pos2;
					}
					else
					{
						rescode=0;
						strcpy(devnames_list,"NO_ROWS_SELECTED");
						GetErrorMess(appliName,"NO_ROWS_SELECTED",ErrorMessage,1);
					}
				}
				free(devIDs_temp);
				free(devName);
			}
		}
		status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
		if(strstr(ErrorMessage,"NO_ROWS_SELECTED")==NULL)
		{
			if(rescode==0)
			{
				OCIReportError(ociError,appliName,ErrorMessage,0); 
			}
			else
			{
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			}
		}
		return rescode;
	}
/*****************************************************************************************/
	/**
	* Get the list of spare hw serialnb concatenated with the hw name if it exists  which are at the given location, returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* @param location : location 
	* @param len_array : length of the devnames_list. If the buffer is too small, we put the size needed here.
	* @param devnames_list :  char* buffer where to put the result serilanb|hwname (none if hwname=NULL). else separated by '\\0'
	* @param ErrorMessage : error message in case of failure
	* @return 0 if it is successful
	*/
EXTERN_CONFDB
	int GetSpareHWPerLocation(char* location, int &len_array, char* devnames_list,char* ErrorMessage)
	{
		char appliName[100]="GetSpareHWPerLocation";
		int i = 0;
		int rescode=0;
		int pos1=0;
		int pos3=0;
		int pos2=0;
		int len_devname=0;
		char* devName=NULL;
		char* devIDs_temp=NULL;
		sword status;
		int prefetch_rows=2000;
		OCIStmt *stmthp;
		OCIBind  *bnd1p = (OCIBind *) 0; 
		OCIDefine* def;
		OCIParam *parmdp;
		char selectdevID[1000];
		if(location==NULL)
		{
			GetErrorMess(appliName," location MUST be given",ErrorMessage,1);
			return -1;
		}
		
		try{
			HandleAlloc(ociEnv,&stmthp,ociError,&status);
			sprintf(selectdevID,"select serialnb||'|'||nvl(hwname,'none')||'?' from %s where nvl(location,'none')=:location and device_status=%d",HW_DEVICE_TABLE,SPARE);
			StmtPrepare(stmthp,ociError, selectdevID, &status);
			BindByName(stmthp,&bnd1p,ociError,":location",location,&status);
			AttrSet(stmthp,&prefetch_rows,ociError, &status);
			StmtExecute(ociHdbc, stmthp, ociError, &status);
			/*The next two statements describe the select-list item, dept, and return its length to allocate the memory*/
			ParamGet(stmthp, ociError, &parmdp, 1, &status);
			AttrGet(parmdp, &len_devname, ociError, &status);
			
			devName=(char*)malloc((len_devname+1)*sizeof(char));
			if(devName!=NULL)
				DefineByPos(stmthp,&def,ociError,1, devName,len_devname+1,&status);		
			else
			{
				status=-10;
				rescode=ShowErrors (status, ociError, "pointer allocation unsuccessful");
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				GetErrorMess(appliName,"REALLOC UNSUCCESSFUL",ErrorMessage,1);
				return -1;	
			}
		}catch(Error err){
			sprintf(appliName,"GetSpareHWPerLocation");	///////
			rescode=ShowErrors (status, err.ociError, err.log);
			
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			else
				GetErrorMess(appliName,err.msg,ErrorMessage,1);
			
			if(strstr(err.msg,"NOT CONNECTED TO ANY DB")!=NULL)
			return -1;
		}

		if(status==OCI_SUCCESS)
		{
			while(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
			{
				status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT,1,OCI_DEFAULT);
				if(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
				{
					RemoveSeparator(devName,"?");
					pos1=strlen(devName);
					pos3=pos2;
					pos2+=pos1+1;
					
					devIDs_temp=(char*) realloc(devIDs_temp,pos2*sizeof(char));
					if(devIDs_temp==NULL)
					{
						status=-10;
						rescode=ShowErrors (status, ociError, "  pointer allocation unsuccessful");
						free(devName);
						status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
						GetErrorMess(appliName,"REALLOC UNSUCCESSFUL",ErrorMessage,1);
						return -1;
					}
					else
					{
						memcpy(devIDs_temp+pos3,devName,pos1+1);
						i++;
					}
				}
			}
			if(rescode==0)
			{	
				if(pos2>len_array)
				{
					len_array=pos2;
					free(devIDs_temp);
					free(devName);
					status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
					GetErrorMess(appliName,"BUFFER_TOO_SMALL",ErrorMessage,1);
					return -1;
				}
				else
				{
					if(i>0)
					{
						memcpy(devnames_list,devIDs_temp,pos2);
						len_array=pos2;
					}
					else
					{
						rescode=ShowErrors (status, ociError, "OCIFETCH unsuccessful");
						GetErrorMess(appliName,"NO_ROWS_SELECTED",ErrorMessage,1);
						strcpy(devnames_list,"NO_ROWS_SELECTED");
					}
				}
				free(devIDs_temp);
				free(devName);
			}
		}
		status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
		if(strstr(ErrorMessage,"NO_ROWS_SELECTED")==NULL)
		{
			if(rescode==0)
			{
				OCIReportError(ociError,appliName,ErrorMessage,0); 
			}
			else
			{
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			}
		}
		return rescode;
	}
/*****************************************************************************************/
	/**
	* Get the list of spare serialnb + names (if it exists)  which are of the given hwtype, returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* @param hwtype : hwtype
	* @param len_array : length of the devIDs_list. If the buffer is too small, we put the size needed here.
	* @param devnames_list :  char* buffer where to put the result serialnb|hwname (none if hwname=NULL). element separated by '\\0'
	* @param ErrorMessage : error message in case of failure
	* @return 0 if it is successful
	*/
EXTERN_CONFDB
	int GetSpareHWPerType(char* hwtype, int &len_array, char* devnames_list,char* ErrorMessage)
	{
		char appliName[100]="GetSpareHWPerPerType";
		int i = 0;
		int rescode=0;
		int pos1=0;
		int pos3=0;
		int pos2=0;
		int len_devname=0;
		char* devName=NULL;
		char* devIDs_temp=NULL;
		sword status;
		int prefetch_rows=2000;
		OCIStmt *stmthp;
		OCIBind  *bnd1p = (OCIBind *) 0; 
		OCIDefine* def;
		OCIParam *parmdp;
		char selectdevID[1000];
		if(hwtype==NULL)
		{
			GetErrorMess(appliName," hwtype MUST be given",ErrorMessage,1);
			return -1;
		}
		
		try{
			HandleAlloc(ociEnv,&stmthp,ociError,&status);
			sprintf(selectdevID,"select serialnb||'|'||nvl(hwname,'none')||'?' from %s where nvl(hwtype,'none')=:hwtype and device_status=%d",HW_DEVICE_TABLE, SPARE);
			StmtPrepare(stmthp,ociError, selectdevID, &status);
			BindByName(stmthp,&bnd1p,ociError,":hwtype",hwtype,&status);
			AttrSet(stmthp,&prefetch_rows,ociError, &status);
			StmtExecute(ociHdbc, stmthp, ociError, &status);
			/* The next two statements describe the select-list item, dept, and return its length to allocate the memory*/
			ParamGet(stmthp, ociError, &parmdp, 1, &status);
			AttrGet(parmdp, &len_devname, ociError, &status);

			devName=(char*)malloc((len_devname+1)*sizeof(char));
			if(devName!=NULL)				
				DefineByPos(stmthp,&def,ociError,1, devName,len_devname+1,&status);		
			else
			{
				status=-10;
				rescode=ShowErrors (status, ociError, "pointer allocation unsuccessful");
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				GetErrorMess(appliName,"REALLOC UNSUCCESSFUL",ErrorMessage,1);
				return -1;	
			}			
		}catch(Error err){
			sprintf(appliName,"GetSpareHWPerType");	///////
			rescode=ShowErrors (status, err.ociError, err.log);
			
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			else
				GetErrorMess(appliName,err.msg,ErrorMessage,1);
			
			if(strstr(err.msg,"NOT CONNECTED TO ANY DB")!=NULL)
			return -1;
		}

		if(status==OCI_SUCCESS)
		{
			while(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
			{
				status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT,1,OCI_DEFAULT);
				if(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
				{
					RemoveSeparator(devName,"?");
					pos1=strlen(devName);
					pos3=pos2;
					pos2+=pos1+1;					
                    devIDs_temp=(char*) realloc(devIDs_temp,pos2*sizeof(char));
					if(devIDs_temp==NULL)
					{
						status=-10;
						rescode=ShowErrors (status, ociError, "  pointer allocation unsuccessful");
						free(devName);
						status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
						GetErrorMess(appliName,"REALLOC UNSUCCESSFUL",ErrorMessage,1);
						return -1;
					}
					else
					{
						memcpy(devIDs_temp+pos3,devName,pos1+1);
						i++;
					}
				}
			}
			if(rescode==0)
			{	
				if(pos2>len_array)
				{
					len_array=pos2;
					devIDs_temp=(char*) realloc(devIDs_temp,0*sizeof(char));
					free(devName);
					status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
					GetErrorMess(appliName,"BUFFER_TOO_SMALL",ErrorMessage,1);
					return -1;
				}
				else
				{
					if(i>0)
					{
						memcpy(devnames_list,devIDs_temp,pos2);
						len_array=pos2;
					}
					else
					{
						rescode=0;
						strcpy(devnames_list,"NO_ROWS_SELECTED");
						GetErrorMess(appliName,"NO_ROWS_SELECTED",ErrorMessage,1);
					}
				}
				free(devIDs_temp);
				free(devName);
			}
		}
		status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
		if(strstr(ErrorMessage,"NO_ROWS_SELECTED")==NULL)
		{
			if(rescode==0)
			{
				OCIReportError(ociError,appliName,ErrorMessage,0); 
			}
			else
			{
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			}
		}
		return rescode;
	}
/*************************************************************************************/
	//return the device row in device_result
	/*************************************************************************************/
	/**
	* Get the row of the given device name (a device which is used in the macrosc. connect., i.e. a chip won't be found with this fct. in device_result,  returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* @param serialnb : serialnb  of the hw: it's  case sensitive.
	* @param len_device : length of the devtype_result. If the buffer is too small, we put the size needed here.
	* @param device_result :char* buffer where to put the result
	* @param device_result : formatted as follows |db_column_name (db_column_type):db_column_value|
	* @param device_result : db_column_type is I for int and C for char*
	* @param device_result : equals to "NO_ROWS_SELECTED" if no rows selected
	* @param ErrorMessage : error message in case of failure
	* @return 0 if it is successful
	*/
EXTERN_CONFDB
	int GetHWDeviceRow_serialnb(char* serialnb,int &len_device, char* device_result,char* ErrorMessage)
	{
		char appliName[100]="GetHWDeviceRow_serialnb";
		sword status;
		int rescode=0;
		OCIStmt *stmthp;
		OCIBind  *bnd1p = (OCIBind *) 0; 
		OCIDefine* def[9];
		sb4  devtypelen=0;
		sb4 devlen=0;
		sb4  locationlen=0;
		sb4 serialnblen=0;
		sb4 responlen=0;
		sb4 commentslen=0;
		sb4 dnamelen=0;
		OCIParam *parmdp;
		int status1=0;
		char * hwname_temp=NULL;
		char * hwtype_temp=NULL;
		char * location_temp=NULL;
		char * serialnb_temp=NULL;
		char * responsible_temp=NULL;
		char * comments_temp=NULL;
		char * dname_temp=NULL;
		char device_status[20];
		if(serialnb==NULL)
		{
			GetErrorMess(appliName,"serialnb MUST be given",ErrorMessage,1);
			return -1;
		}	
		char selectdev[1000];

		try{
			HandleAlloc(ociEnv,&stmthp,ociError,&status);
			sprintf(selectdev," select nvl(t.hwname,'none')||'?',nvl(t.hwtype,'none')||'?',t.device_status,nvl(t.responsible,'none')||'?',nvl(t.user_comments,'none')||'?',nvl(t.location,'none')||'?',t.serialnb||'?',d.devicename||'?' from %s t,%s d where t.serialnb=:dname and t.serialnb=d.serialnb",HW_DEVICE_TABLE,LOGICAL_DEVICE_TABLE);
			StmtPrepare(stmthp,ociError, selectdev, &status);
			BindByName(stmthp,&bnd1p,ociError,":dname",serialnb,&status);
			StmtExecute(ociHdbc, stmthp, ociError, &status);
			/* The next two statements describe the select-list item, dept, and return its length to allocate the memory*/
			ParamGet(stmthp, ociError, &parmdp, 1, &status);
			AttrGet(parmdp, &devlen, ociError, &status);
			ParamGet(stmthp, ociError, &parmdp, 2, &status);
			AttrGet(parmdp, &devtypelen, ociError, &status);
			ParamGet(stmthp, ociError, &parmdp, 4, &status);
			AttrGet(parmdp, &responlen, ociError, &status);
			ParamGet(stmthp, ociError, &parmdp, 5, &status);
			AttrGet(parmdp, &commentslen, ociError, &status);
			ParamGet(stmthp, ociError, &parmdp, 6, &status);
			AttrGet(parmdp, &serialnblen, ociError, &status);
			ParamGet(stmthp, ociError, &parmdp, 7, &status);
			AttrGet(parmdp, &locationlen, ociError, &status);
			ParamGet(stmthp, ociError, &parmdp, 8, &status);
			AttrGet(parmdp, &dnamelen, ociError, &status);
	
			hwtype_temp = (char *) realloc(hwtype_temp,(devtypelen + 1)*sizeof(char));
			hwname_temp= (char *) realloc(hwname_temp,(devlen + 1)*sizeof(char));
			location_temp = (char *) realloc(location_temp,(locationlen + 1)*sizeof(char));
			serialnb_temp= (char *) realloc(serialnb_temp,(serialnblen + 1)*sizeof(char));
			dname_temp= (char *) realloc(dname_temp,(dnamelen + 1)*sizeof(char));
			comments_temp= (char *) realloc(comments_temp,(commentslen + 1)*sizeof(char));
			responsible_temp= (char *) realloc(responsible_temp,(responlen + 1)*sizeof(char));

			if(hwtype_temp==NULL || hwname_temp==NULL || location_temp==NULL || serialnb_temp==NULL ||dname_temp==NULL||responsible_temp==NULL||comments_temp==NULL)
			{
				rescode=ShowErrors (status, ociError, "  pointer allocation unsuccessful");
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				if(hwtype_temp!=NULL)
					free(hwtype_temp);
				if(hwname_temp!=NULL)
					free(hwname_temp);
				if(location_temp!=NULL)
					free(location_temp);
				if(serialnb_temp!=NULL)
					free(serialnb_temp);
				if(dname_temp!=NULL)
					free(dname_temp);
				if(responsible_temp!=NULL)
					free(responsible_temp);
				if(comments_temp!=NULL)
					free(comments_temp);
				GetErrorMess(appliName,"REALLOC UNSUCCESSFUL",ErrorMessage,1);
				return -1;
			}
			DefineByPos(stmthp,def,ociError,1, hwname_temp,devlen + 1,&status);		
			DefineByPos(stmthp,def,ociError,2, hwtype_temp,devtypelen+1,&status);		
			DefineByPos(stmthp,def,ociError,3, &status1,&status);		
			DefineByPos(stmthp,def,ociError,4, responsible_temp,responlen+1,&status);		
			DefineByPos(stmthp,def,ociError,5, comments_temp,commentslen+1, &status);		
			DefineByPos(stmthp,def,ociError,6, location_temp,locationlen+1, &status);		
			DefineByPos(stmthp,def,ociError,7, serialnb_temp,serialnblen+1, &status);		
			DefineByPos(stmthp,def,ociError,8, dname_temp,dnamelen+1, &status);		
			
		}catch(Error err){
			sprintf(appliName,"GetHWDeviceRow_serialnb");	///////
			rescode=ShowErrors (status, err.ociError, err.log);
			
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			else
				GetErrorMess(appliName,err.msg,ErrorMessage,1);
			
			if(strstr(err.msg,"NOT CONNECTED TO ANY DB")!=NULL)
			return -1;
		}

		if(status==OCI_SUCCESS)
			status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT,1,OCI_DEFAULT);

		if (status==OCI_SUCCESS || status==OCI_SUCCESS_WITH_INFO) 
		{	
			RemoveSeparator(serialnb_temp,"?");
			RemoveSeparator(hwtype_temp,"?");
			RemoveSeparator(location_temp,"?");
			RemoveSeparator(hwname_temp,"?");
			RemoveSeparator(dname_temp,"?");
			RemoveSeparator(responsible_temp,"?");
			RemoveSeparator(comments_temp,"?");
			GetDeviceStatus(status1, device_status);
			sprintf(device_result,"|hwname (C):%s |hwtype (C):%s |device_status (C):%s |location (C):%s |responsible  (C):%s |user_comments (C):%s |devicename (C):%s |serialnb  (C):%s |" ,hwname_temp,hwtype_temp,device_status,location_temp,responsible_temp,comments_temp,dname_temp,serialnb_temp);
		} 
		else 
		{
			if(rescode==0)
			{
				sprintf(selectdev," select nvl(hwname,'none')||'?',nvl(hwtype,'none')||'?',device_status,nvl(responsible,'none')||'?',nvl(user_comments,'none')||'?',nvl(location,'none')||'?',serialnb||'?' from %s where serialnb=:dname",HW_DEVICE_TABLE);
				try{
                    StmtPrepare(stmthp,ociError, selectdev, &status);              
					BindByName(stmthp,&bnd1p,ociError,":dname",serialnb,&status);
    				StmtExecute(ociHdbc, stmthp, ociError, &status);
					DefineByPos(stmthp,def,ociError,1, hwname_temp,devlen + 1,&status);		
					DefineByPos(stmthp,def,ociError,2, hwtype_temp,devtypelen+1,&status);		
					DefineByPos(stmthp,def,ociError,3, &status1,&status);		
					DefineByPos(stmthp,def,ociError,4, responsible_temp,responlen+1,&status);		
					DefineByPos(stmthp,def,ociError,5, comments_temp,commentslen+1,&status);		
					DefineByPos(stmthp,def,ociError,6, location_temp,locationlen+1,&status);		
					DefineByPos(stmthp,def,ociError,7, serialnb_temp,serialnblen+1,&status);						
				}catch(Error err){
					sprintf(appliName,"GetHWDeviceRow_serialnb");	///////
					rescode=ShowErrors (status, err.ociError, err.log);
				
					if(ociError!=0)
						OCIReportError(ociError,appliName,ErrorMessage,1); 
					else
						GetErrorMess(appliName,err.msg,ErrorMessage,1);
				}

				if(status==OCI_SUCCESS)
					status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT,1,OCI_DEFAULT);

				if (status==OCI_SUCCESS || status==OCI_SUCCESS_WITH_INFO) 
				{	
					RemoveSeparator(serialnb_temp,"?");
					RemoveSeparator(hwtype_temp,"?");
					RemoveSeparator(location_temp,"?");
					RemoveSeparator(hwname_temp,"?");
					RemoveSeparator(responsible_temp,"?");
					RemoveSeparator(comments_temp,"?");
					GetDeviceStatus(status1, device_status);
					sprintf(device_result,"|hwname (C):%s |hwtype (C):%s |device_status (C):%s |location (C):%s |responsible  (C):%s |user_comments (C):%s |devicename (C):none |serialnb  (C):%s |" ,hwname_temp,hwtype_temp,device_status,location_temp,responsible_temp,comments_temp,serialnb_temp);
				}
				else
				{
					if(rescode==0)
					{
						rescode=0;
						strcpy(device_result,"NO_ROWS_SELECTED");
						GetErrorMess(appliName,"NO_ROWS_SELECTED",ErrorMessage,1);
					}
				}
			}
		}
		if(hwtype_temp!=NULL)
			free(hwtype_temp);
		if(hwname_temp!=NULL)
			free(hwname_temp);
		if(dname_temp!=NULL)
			free(dname_temp);
		if(serialnb_temp!=NULL)
			free(serialnb_temp);
		if(location_temp!=NULL)
			free(location_temp);
		if(responsible_temp!=NULL)
			free(responsible_temp);
		if(comments_temp!=NULL)
			free(comments_temp);
		status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
		rescode+=status;
		if(strstr(ErrorMessage,"NO_ROWS_SELECTED")==NULL)
		{
			if(rescode==0)
			{
				OCIReportError(ociError,appliName,ErrorMessage,0); 
			}
			else
			{
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			}
		}
		return rescode;
	}
/*****************************************************************************************/
	/**
	* Get the list of functional device names (only in IN_USE status)  which are at the given location, returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* @param ipname : ipname 
	* @param len_array : length of the devIDs_list. If the buffer is too small, we put the size needed here.
	* @param ipaliases_list :  char* buffer where to put the result (list of devicenames). each line seaprated by '\\0'
	* @param ErrorMessage : error message in case of failure
	* @return 0 if it is successful
	*/
EXTERN_CONFDB
	int GetIPAliasesPerIPName(char* ipname, int &len_array, char* ipaliases_list,char* ErrorMessage)
	{
		char appliName[100]="GetIPAliasesPerIPName";
		int i = 0;
		int rescode=0;
		int pos1=0;
		int pos3=0;
		int pos2=0;
		int len_devname=0;
		char* devName=NULL;
		char* devIDs_temp=NULL;
		sword status;
		int prefetch_rows=2000;
		OCIStmt *stmthp;
		OCIBind  *bnd1p = (OCIBind *) 0; 
		OCIDefine* def;
		OCIParam *parmdp;
		char selectdevID[1000];
		if(ipname==NULL)
		{
			GetErrorMess(appliName," location MUST be given",ErrorMessage,1);
			return -1;
		}
		
		try{
			HandleAlloc(ociEnv,&stmthp,ociError,&status);
			sprintf(selectdevID,"select t.ipalias||'|' from %s t , %s d where t.ipaddress=d.ipaddress and d.ipname=:ipname",IPALIAS_TABLE,IPINFO_TABLE);
			StmtPrepare(stmthp,ociError, selectdevID, &status);
			BindByName(stmthp,&bnd1p,ociError,":ipname",ipname,&status);
			AttrSet(stmthp,&prefetch_rows,ociError, &status);
			StmtExecute(ociHdbc, stmthp, ociError, &status);
			/* The next two statements describe the select-list item, dept, and return its length to allocate the memory*/
			ParamGet(stmthp, ociError, &parmdp, 1, &status);
			AttrGet(parmdp, &len_devname, ociError, &status);

			devName=(char*)malloc((len_devname+1)*sizeof(char));
			if(devName!=NULL)
				DefineByPos(stmthp,&def,ociError,1, devName,len_devname+1,&status);		
			else
			{
				status=-10;
				rescode=ShowErrors (status, ociError, "pointer allocation unsuccessful");
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				GetErrorMess(appliName,"REALLOC UNSUCCESSFUL",ErrorMessage,1);
				return -1;	
			}			
		}catch(Error err){
			sprintf(appliName,"GetIPAliasesPerIPName");	///////
			rescode=ShowErrors (status, err.ociError, err.log);
			
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			else
				GetErrorMess(appliName,err.msg,ErrorMessage,1);
			
			if(strstr(err.msg,"NOT CONNECTED TO ANY DB")!=NULL)
			return -1;
		}

		if(status==OCI_SUCCESS)
		{
			while(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
			{
				status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT,1,OCI_DEFAULT);
				if(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
				{
					RemoveSeparator(devName,"|");
					pos1=strlen(devName);
					pos3=pos2;
					pos2+=pos1+1;
					devIDs_temp=(char*) realloc(devIDs_temp,pos2*sizeof(char));
					if(devIDs_temp==NULL)
					{
						status=-10;
						rescode=ShowErrors (status, ociError, "  pointer allocation unsuccessful");
						free(devName);
						status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
						GetErrorMess(appliName,"REALLOC UNSUCCESSFUL",ErrorMessage,1);
						return -1;
					}
					else
					{
						memcpy(devIDs_temp+pos3,devName,pos1+1);
						i++;
					}
				}
			}
			if(rescode==0)
			{	
				if(pos2>len_array)
				{
					len_array=pos2;
					devIDs_temp=(char*) realloc(devIDs_temp,0*sizeof(char));
					free(devName);
					status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
					GetErrorMess(appliName,"BUFFER_TOO_SMALL",ErrorMessage,1);
					return -1;
				}
				else
				{
					if(i>0)
					{
						memcpy(ipaliases_list,devIDs_temp,pos2);
						len_array=pos2;
					}
					else
					{
						rescode=0;
						strcpy(ipaliases_list,"NO_ROWS_SELECTED");
						GetErrorMess(appliName,"NO_ROWS_SELECTED",ErrorMessage,1);
					}
				}
				free(devIDs_temp);
				free(devName);
			}
		}
		status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
		if(strstr(ErrorMessage,"NO_ROWS_SELECTED")==NULL)
		{
			if(rescode==0)
			{
				OCIReportError(ociError,appliName,ErrorMessage,0); 
			}
			else
			{
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			}
		}
		return rescode;
	}
/*****************************************************************************************/
	/**
	* Get the list of device functions returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* @param len_array : length of the devIDs_list. If the buffer is too small, we put the size needed here.
	* @param function_list :  char* buffer where to put the result (list of devicenames). each line seaprated by '\\0'
	* @param ErrorMessage : error message in case of failure
	* @return 0 if it is successful
	*/
EXTERN_CONFDB
	int GetAvailableFunctions(int &len_array, char* function_list,char* ErrorMessage)
	{
		char appliName[100]="GetAvailableFunctions";
		int i = 0;
		int rescode=0;
		int pos1=0;
		int pos3=0;
		int pos2=0;
		int len_devname=0;
		char* devName=NULL;
		char* devIDs_temp=NULL;
		sword status;
		int prefetch_rows=2000;
		OCIStmt *stmthp;
		OCIDefine* def;
		OCIParam *parmdp;
		char selectdevID[1000];

		try{
			HandleAlloc(ociEnv,&stmthp,ociError,&status);
			sprintf(selectdevID,"select function_name||'|' from %s",DEVICE_FUNCTION_TABLE);
			StmtPrepare(stmthp,ociError, selectdevID, &status);
			AttrSet(stmthp,&prefetch_rows,ociError, &status);
			StmtExecute(ociHdbc, stmthp, ociError, &status);
			/* The next two statements describe the select-list item, dept, and return its length to allocate the memory*/
			ParamGet(stmthp, ociError, &parmdp, 1, &status);
			AttrGet(parmdp, &len_devname, ociError, &status);
			
			devName=(char*)malloc((len_devname+1)*sizeof(char));
			if(devName!=NULL)
				DefineByPos(stmthp,&def,ociError,1, devName,len_devname+1,&status);		
			else
			{
				status=-10;
				rescode=ShowErrors (status, ociError, "pointer allocation unsuccessful");
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				GetErrorMess(appliName,"REALLOC UNSUCCESSFUL",ErrorMessage,1);
				return -1;	
			}
		}catch(Error err){
			sprintf(appliName,"GetAvailableFunctions");	///////
			rescode=ShowErrors (status, err.ociError, err.log);
			
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			else
				GetErrorMess(appliName,err.msg,ErrorMessage,1);
			
			if(strstr(err.msg,"NOT CONNECTED TO ANY DB")!=NULL)
			return -1;
		}

		if(status==OCI_SUCCESS)
		{
			while(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
			{
				status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT,1,OCI_DEFAULT);
				if(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
				{
					RemoveSeparator(devName,"|");
					pos1=strlen(devName);
					pos3=pos2;
					pos2+=pos1+1;
					devIDs_temp=(char*) realloc(devIDs_temp,pos2*sizeof(char));
					if(devIDs_temp==NULL)
					{
						status=-10;
						rescode=ShowErrors (status, ociError, "  pointer allocation unsuccessful");
						free(devName);
						status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
						GetErrorMess(appliName,"REALLOC UNSUCCESSFUL",ErrorMessage,1);
						return -1;
					}
					else
					{
						memcpy(devIDs_temp+pos3,devName,pos1+1);
						i++;
					}
				}
			}
			if(rescode==0)
			{	
				if(pos2>len_array)
				{
					len_array=pos2;
					free(devIDs_temp);
					free(devName);
					status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
					GetErrorMess(appliName,"BUFFER_TOO_SMALL",ErrorMessage,1);
					return -1;
				}
				else
				{
					if(i>0)
					{
						memcpy(function_list,devIDs_temp,pos2);
						len_array=pos2;
					}
					else
					{
						rescode=0;
						strcpy(function_list,"NO_ROWS_SELECTED");
						GetErrorMess(appliName,"NO_ROWS_SELECTED",ErrorMessage,1);
					}
				}
				free(devIDs_temp);
				free(devName);
			}
		}
		status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
		if(strstr(ErrorMessage,"NO_ROWS_SELECTED")==NULL)
		{
			if(rescode==0)
			{
				OCIReportError(ociError,appliName,ErrorMessage,0); 
			}
			else
			{
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			}
		}
		return rescode;
	}
/************************************************************************/
	// return the row of the given ipalias in ipalias_result
	/***********************************************************************/
	/**
	* Get the row of the given device type in devtype_result,  returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* @param ipalias : ipalias name: it's CASE SENSITIVE.
	* @param len_ipalias : length of the devtype_result. If the buffer is too small, we put the size needed here.
	* @param ipalias_result :char* buffer where to put the result
	* @param ipalias_result : formatted as follows |db_column_name (db_column_type):db_column_value|
	* @param ipalias_result : db_column_type is I for int and C for char*
	* @param ipalias_result : equals to "NO_ROWS_SELECTED" if no rows selected
	* @param ErrorMessage : error message in case of failure
	* @return 0 if it is successful
	*/
EXTERN_CONFDB
	int GetIPAliasRow(char* ipalias,int &len_ipalias,char* ipalias_result,char* ErrorMessage)
	{
		char appliName[100]="GetIPAliasRow";
		int rescode=0;
		sword status;
		OCIStmt *stmthp;
		OCIBind  *bnd1p = (OCIBind *) 0; 
		OCIDefine* def[7];
		sb4  ipadd_len=20;
		sb4 ipname_len=0;
		sb4 ipalias_len=0;
		OCIParam *parmdp;
		char * ipalias_temp=NULL;
		char * ipname_temp=NULL;
		char* ipaddress_temp=NULL;
		char* subnet_temp=NULL;
		char selectdevtype[1000];
		if(ipalias==NULL)
		{
			GetErrorMess(appliName," ipalias MUST be given",ErrorMessage,1);
			return -1;
		}
		
		try{
			HandleAlloc(ociEnv,&stmthp,ociError,&status);
			sprintf(selectdevtype,"select t.ipaddress||'?',t.ipalias||'?',e.ipname||'?',e.subnet_info||'?' from %s t,%s e where t.ipalias=:ipalias and t.ipaddress=e.ipaddress ",IPALIAS_TABLE,IPINFO_TABLE);
			StmtPrepare(stmthp,ociError, selectdevtype, &status);
			BindByName(stmthp,&bnd1p,ociError,":ipalias",ipalias,&status);
			StmtExecute(ociHdbc, stmthp, ociError, &status);
			/* The next two statements describe the select-list item, dept, and return its length to allocate the memory*/
			ParamGet(stmthp, ociError, &parmdp, 2, &status);
			AttrGet(parmdp,  &ipalias_len, ociError, &status);
			ParamGet(stmthp, ociError, &parmdp, 3, &status);
			AttrGet(parmdp,  &ipname_len, ociError, &status);

			ipname_temp = (char *) realloc(ipname_temp,(ipname_len + 1)*sizeof(char));
			ipalias_temp= (char *) realloc(ipalias_temp,(ipalias_len + 1)*sizeof(char));
			ipaddress_temp= (char *) realloc(ipaddress_temp,(ipadd_len + 1)*sizeof(char));
			subnet_temp= (char *) realloc(subnet_temp,(ipadd_len + 1)*sizeof(char));
			if(subnet_temp==NULL || ipaddress_temp==NULL ||ipalias_temp==NULL || ipname_temp==NULL)
			{
				status=-10;
				rescode=ShowErrors (status, ociError, "  pointer allocation unsuccessful");
				if(subnet_temp!=NULL)
					free(subnet_temp);
				if(ipaddress_temp!=NULL)
					free(ipaddress_temp);
				if(ipalias_temp!=NULL)
					free(ipalias_temp);
				if(ipname_temp!=NULL)
					free(ipname_temp);
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				GetErrorMess(appliName,"REALLOC UNSUCCESSFUL",ErrorMessage,1);
				return -1;
			}
			DefineByPos(stmthp,def,ociError,1,ipaddress_temp, ipadd_len + 1,&status);		
			DefineByPos(stmthp,def,ociError,2,ipalias_temp,ipalias_len,&status);		
			DefineByPos(stmthp,def,ociError,3,ipname_temp,ipname_len,&status);		
			DefineByPos(stmthp,def,ociError,4,subnet_temp,ipadd_len,&status);		
						
		}catch(Error err){
			sprintf(appliName,"GetIPAliasRow");	///////
			rescode=ShowErrors (status, err.ociError, err.log);
			
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			else
				GetErrorMess(appliName,err.msg,ErrorMessage,1);
			
			if(strstr(err.msg,"NOT CONNECTED TO ANY DB")!=NULL)
			return -1;
		}

		if(status==OCI_SUCCESS)
			status =OCIStmtFetch2(stmthp,ociError, 1, OCI_FETCH_NEXT,1, OCI_DEFAULT);

		if (status == OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO) 
		{	
			RemoveSeparator(ipaddress_temp,"?");
			RemoveSeparator(ipname_temp,"?");
			RemoveSeparator(ipalias_temp,"?");
			RemoveSeparator(subnet_temp,"?");
			sprintf(ipalias_result,"|ipalias (C):%s |ipaddress (C):%s |ipname (C):%s |subnet_mask (C):%s |",ipalias_temp,ipaddress_temp,ipname_temp,subnet_temp);
		} 
		else 
		{
			if(rescode==0)
			{
				rescode=0;
				strcpy(ipalias_result,"NO_ROWS_SELECTED");
				GetErrorMess(appliName, "NO_ROWS_SELECTED",ErrorMessage,1);
			}
		}
		if(ipalias_temp!=NULL)
			free(ipalias_temp);
		if(subnet_temp!=NULL)
			free(subnet_temp);
		if(ipname_temp!=NULL)
			free(ipname_temp);
		if(ipaddress_temp!=NULL)
			free(ipaddress_temp);
		status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
		rescode+=status;
		if(strstr(ErrorMessage,"NO_ROWS_SELECTED")==NULL)
		{
			if(rescode==0)
			{
				OCIReportError(ociError,appliName,ErrorMessage,0); 
			}
			else
			{
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			}
		}
		return rescode;
	}
/*****************************************************************************************/
	/**
	* Get the list of deviceIDs  of the given device type,  returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* @param lkinfo : linkinfo value of the link (can be part of it or the exct value). Put none, if you want to select rows where lkinfo is null
	* @param len_array : length of the devIDs_list. If the buffer is too small, we put the size needed here.
	* @param lkIDs_list :  int* buffer where to put the result (list of deviceIDs)
	* @param lkIDs_list : equals to (-1,-1) if no rows selected
	* @param ErrorMessage : error message in case of failure
	* @return 0 if it is successful
	*/
EXTERN_CONFDB
	int GetLkIDsPerLkInfo(char* lkinfo, int &len_array, int* lkIDs_list,char* ErrorMessage)
	{
		char appliName[100]="GetLkIDsPerLkInfo";
		int i = 0;
		int j=0;
		int rescode=0;
		int lkID=0;
		int* lkIDs_temp=NULL;
		sword status;
		int nb_rdtrip=1;
		int prefetch_rows=2000;
		OCIStmt *stmthp;
		OCIBind  *bnd1p = (OCIBind *) 0; 
		OCIDefine* def;
		char selectdevID[1000];
		if(lkinfo==NULL)
		{
			GetErrorMess(appliName," lkinfo type MUST be given",ErrorMessage,1);
			return -1;
		}
		
		try{
			HandleAlloc(ociEnv,&stmthp,ociError,&status);
			sprintf(selectdevID,"select t.linkid from %s t where instr(nvl(t.link_info,'none'),:lkinfo)>0",MACRO_CONNECTIVITY_TABLE);
			StmtPrepare(stmthp,ociError, selectdevID, &status);
			BindByName(stmthp,&bnd1p,ociError,":lkinfo",lkinfo,&status);
			AttrSet(stmthp,&prefetch_rows,ociError, &status);
			StmtExecute(ociHdbc, stmthp, ociError, &status);
			DefineByPos(stmthp,&def,ociError,1,&lkID,&status);

		}catch(Error err){
			sprintf(appliName,"GetLkIDsPerLkInfo");	///////
			rescode=ShowErrors (status, err.ociError, err.log);
			
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			else
				GetErrorMess(appliName,err.msg,ErrorMessage,1);
			
			if(strstr(err.msg,"NOT CONNECTED TO ANY DB")!=NULL)
			return -1;
		}

		if(status==OCI_SUCCESS)
		{
			lkIDs_temp=(int*) realloc(lkIDs_temp,prefetch_rows*sizeof(int));
			if(lkIDs_temp==NULL)
			{
				status=-10;
				rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				GetErrorMess(appliName,"REALLOC UNSUCCESSFUL",ErrorMessage,1);
				return -1;	
			}
			while(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
			{
				status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT,1, OCI_DEFAULT);
				if(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
				{
					lkIDs_temp[i]=lkID;	
					i++;
				}
				if(i>0 && i%100==0)
				{
					nb_rdtrip++;
					lkIDs_temp=(int*) realloc(lkIDs_temp,nb_rdtrip*prefetch_rows*sizeof(int));
					if(lkIDs_temp==NULL)
					{
						status=-10;
						rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");
						status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
						GetErrorMess(appliName,"REALLOC UNSUCCESSFUL",ErrorMessage,1);
						return -1;
					}
				}
			}
			if(rescode==0)
			{
				if(i>len_array)
				{
					len_array=i;
					status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
					lkIDs_temp=(int*) realloc(lkIDs_temp,0*sizeof(int));
					GetErrorMess(appliName,"BUFFER_TOO_SMALL",ErrorMessage,1);
					return -1;	
				}
				else
				{
					if(i>0)
					{
						for(j=0;j<i;j++)
						{
							lkIDs_list[j]=lkIDs_temp[j];
						}
						len_array=i;
					}
					else
					{
						rescode=0;
						//means NO_ROWS_SELECTED
						lkIDs_list[0]=-1;
						lkIDs_list[1]=-1;
						GetErrorMess(appliName,"NO_ROWS_SELECTED",ErrorMessage,1);
					}
				}
				free(lkIDs_temp);
			}
		}
		status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
		if(strstr(ErrorMessage,"NO_ROWS_SELECTED")==NULL)
		{
			if(rescode==0)
			{
				OCIReportError(ociError,appliName,ErrorMessage,0); 
			}
			else
			{
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			}
		}
		return rescode;
	}
/*****************************************************************************************/
	/**
	* Get the list of subsystems returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* @param len_array : length of the devIDs_list. If the buffer is too small, we put the size needed here.
	* @param sysname_list :  char* buffer where to put the result (list of sysnames). each line seaprated by '\\0'
	* @param ErrorMessage : error message in case of failure
	* @return 0 if it is successful
	*/
EXTERN_CONFDB
	int GetListOfSubsystems(int &len_array, char* sysname_list,char* ErrorMessage)
	{
		char appliName[100]="GetListOfSubsystems";
		int i = 0;
		int rescode=0;
		int pos1=0;
		int pos3=0;
		int pos2=0;
		int len_devname=0;
		char* devName=NULL;
		char* devIDs_temp=NULL;
		sword status;
		int prefetch_rows=2000;
		OCIStmt *stmthp;
		OCIDefine* def;
		OCIParam *parmdp;
		char selectdevID[1000];

		try{
			HandleAlloc(ociEnv,&stmthp,ociError,&status);
			sprintf(selectdevID,"select system_name||'|' from %s",SUBSYSTEM_TABLE);
			StmtPrepare(stmthp,ociError, selectdevID, &status);
			AttrSet(stmthp,&prefetch_rows,ociError, &status);
			StmtExecute(ociHdbc, stmthp, ociError, &status);
			/* The next two statements describe the select-list item, dept, and return its length to allocate the memory*/
			ParamGet(stmthp, ociError, &parmdp, 1, &status);
			AttrGet(parmdp, &len_devname, ociError, &status);
			
			devName=(char*)malloc((len_devname+1)*sizeof(char));
			if(devName!=NULL)
				DefineByPos(stmthp,&def,ociError,1,devName,len_devname+1,&status);
			else
			{
				status=-10;
				rescode=ShowErrors (status, ociError, "pointer allocation unsuccessful");
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				GetErrorMess(appliName,"REALLOC UNSUCCESSFUL",ErrorMessage,1);
				return -1;	
			}
		}catch(Error err){
			sprintf(appliName,"GetListOfSubsystems");	///////
			rescode=ShowErrors (status, err.ociError, err.log);
			
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			else
				GetErrorMess(appliName,err.msg,ErrorMessage,1);
			
			if(strstr(err.msg,"NOT CONNECTED TO ANY DB")!=NULL)
			return -1;
		}

		if(status==OCI_SUCCESS)
		{
			while(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
			{
				status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT,1,OCI_DEFAULT);
				if(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
				{
					RemoveSeparator(devName,"|");
					pos1=strlen(devName);
					pos3=pos2;
					pos2+=pos1+1;
					devIDs_temp=(char*) realloc(devIDs_temp,pos2*sizeof(char));
					if(devIDs_temp==NULL)
					{
						status=-10;
						rescode=ShowErrors (status, ociError, "  pointer allocation unsuccessful");
						free(devName);
						status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
						GetErrorMess(appliName,"REALLOC UNSUCCESSFUL",ErrorMessage,1);
						return -1;
					}
					else
					{
						memcpy(devIDs_temp+pos3,devName,pos1+1);
						i++;
					}
				}
			}
			if(rescode==0)
			{
				if(pos2>len_array)
				{
					len_array=pos2;
					devIDs_temp=(char*) realloc(devIDs_temp,0*sizeof(char));
					free(devName);
					status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
					GetErrorMess(appliName,"BUFFER_TOO_SMALL",ErrorMessage,1);
					return -1;
				}
				else
				{
					if(i>0)
					{
						memcpy(sysname_list,devIDs_temp,pos2);
						len_array=pos2;
					}
					else
					{
						rescode=0;
						strcpy(sysname_list,"NO_ROWS_SELECTED");
						GetErrorMess(appliName,"NO_ROWS_SELECTED",ErrorMessage,1);
					}
				}
				free(devIDs_temp);
				free(devName);
			}
		}
		status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
		if(strstr(ErrorMessage,"NO_ROWS_SELECTED")==NULL)
		{
			if(rescode==0)
			{
				OCIReportError(ociError,appliName,ErrorMessage,0); 
			}
			else
			{
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			}
		}
		return rescode;
	}
/*****************************************************************************************/
	/**
	* Get the list of spare hwtype, returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* @param len_array : length of the devIDs_list. If the buffer is too small, we put the size needed here.
	* @param hwtypes_list :  char* buffer where to put the result serialnb|hwname (none if hwname=NULL). element separated by '\\0'
	* @param ErrorMessage : error message in case of failure
	* @return 0 if it is successful
	*/
EXTERN_CONFDB
	int GetSpareHWTypeList(int &len_array, char* hwtypes_list,char* ErrorMessage)
	{
		char appliName[100]="GetSpareHWTypeList";
		int i = 0;
		int rescode=0;
		int pos1=0;
		int pos3=0;
		int actual_len=0;
		int pos2=0;
		int len_devname=0;
		char* devName=NULL;
		char* devIDs_temp=NULL;
		sword status;
		int prefetch_rows=2000;
		OCIStmt *stmthp;
		OCIBind  *bnd1p = (OCIBind *) 0; 
		OCIDefine* def;
		OCIParam *parmdp;
		char selectdevID[1000];

		try{
			HandleAlloc(ociEnv,&stmthp,ociError,&status);
			sprintf(selectdevID,"select distinct nvl(hwtype,'none')||'?' from %s where device_status=%d",HW_DEVICE_TABLE,SPARE);
			StmtPrepare(stmthp,ociError, selectdevID, &status);
			AttrSet(stmthp,&prefetch_rows,ociError, &status);
			StmtExecute(ociHdbc, stmthp, ociError, &status);
			/* The next two statements describe the select-list item, dept, and return its length to allocate the memory*/
			ParamGet(stmthp, ociError, &parmdp, 1, &status);
			AttrGet(parmdp, &len_devname, ociError, &status);
			
			devName=(char*)malloc((len_devname+1)*sizeof(char));
			if(devName!=NULL)
				DefineByPos(stmthp,&def,ociError,1, devName,len_devname+1,&status);		
			else
			{
				status=-10;
				rescode=ShowErrors (status, ociError, "pointer allocation unsuccessful");
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				GetErrorMess(appliName,"REALLOC UNSUCCESSFUL",ErrorMessage,1);
				return -1;	
			}
		}catch(Error err){
			sprintf(appliName,"GetSpareHWTypeList");	///////
			rescode=ShowErrors (status, err.ociError, err.log);
			
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			else
				GetErrorMess(appliName,err.msg,ErrorMessage,1);
			
			if(strstr(err.msg,"NOT CONNECTED TO ANY DB")!=NULL)
			return -1;
		}

		if(status==OCI_SUCCESS)
		{
			while(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
			{
				status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT,1,OCI_DEFAULT);
				if(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
				{
					RemoveSeparator(devName,"?");
					pos1=strlen(devName);
					pos3=pos2;
					pos2+=pos1+1;

					devIDs_temp=(char*) realloc(devIDs_temp,pos2*sizeof(char));
					if(devIDs_temp==NULL)
					{
						status=-10;
						rescode=ShowErrors (status, ociError, "  pointer allocation unsuccessful");
						free(devName);
						status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
						GetErrorMess(appliName,"REALLOC UNSUCCESSFUL",ErrorMessage,1);
						return -1;
					}
					else
					{
						memcpy(devIDs_temp+pos3,devName,pos1+1);
						i++;
					}
				}
			}
			if(rescode==0)
			{	
				if(pos2>len_array)
				{
					len_array=pos2;
					devIDs_temp=(char*) realloc(devIDs_temp,0*sizeof(char));
					free(devName);
					status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
					GetErrorMess(appliName,"BUFFER_TOO_SMALL",ErrorMessage,1);
					return -1;
				}
				else
				{
					if(i>0)
					{
						memcpy(hwtypes_list,devIDs_temp,pos2);
						len_array=pos2;
					}
					else
					{
						rescode=0;
						strcpy(hwtypes_list,"NO_ROWS_SELECTED");
						GetErrorMess(appliName,"NO_ROWS_SELECTED",ErrorMessage,1);
					}
				}
				free(devIDs_temp);
				free(devName);
			}
		}
		status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
		if(strstr(ErrorMessage,"NO_ROWS_SELECTED")==NULL)
		{
			if(rescode==0)
			{
				OCIReportError(ociError,appliName,ErrorMessage,0); 
			}
			else
			{
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			}
		}
		return rescode;
	}
/**************************************************************************************/
	//return port row in port_row_result
	/**************************************************************************************/
	/**
	* Get the spare port row in port_row_result, returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* @param serialnb : serialnb of the spare
	* @param port_nb : port nb of the device: it's a char*, because for some system, the port nb is defined with characters and numbers (case of the daq)
	* @param port_way : 1 if it's an input,2 if it's an output 
	* @param port_type : interface type of the port (control,bmc, data used for the DAQ system). put "" if it's null
	* @param len_port : length of the port_row_result. If the buffer is too small, we put the size needed here.
	* @param port_row_result :char* buffer where to put the result
	* @param port_row_result : formatted as follows |db_column_name (db_column_type):db_column_value|
	* @param port_row_result : db_column_type is I for int and C for char*
	* @param port_row_result : equals to "NO_ROWS_SELECTED" if no rows selected
	* @param ErrorMessage : error message in case of failure
	* @return 0 if it is successful
	*/
EXTERN_CONFDB
	int GetSparePortRow_snb(char* serialnb, char* port_nb,int port_way, char* port_type,int &len_port, char* port_row_result,char* ErrorMessage)
	{
		char appliName[100]="GetSparePortRow_snb";
		int rescode=0;
		sword status;
		OCIStmt *stmthp;
		OCIBind  *bnd1p[4] ; 
		OCIDefine* def[6];
		sb4  dnamelen=0;
		sb4 portypelen=0;
		sb4  portnblen=0;
		sb4 maclen=0;
		sb4 bialen=0;
		OCIParam *parmdp;
		int portway=0;
		char * mac_temp=NULL;
		char * bia_temp=NULL;
		char * snb_temp=NULL;
		char * portype_temp=NULL;
		char * portnb_temp=NULL;
		char selectport[1000];
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

		try{
			HandleAlloc(ociEnv,&stmthp,ociError,&status);
			sprintf(selectport,"select e.serialnb||'?',e.port_nbr||'?',e.port_way,nvl(e.port_type,'none')||'?',nvl(e.bia,'none')||'?',nvl(e.macaddress,'none')||'?' from %s e where e.serialnb=:snb and e.port_nbr=:portnb and e.port_way=:portway and nvl(e.port_type,'none')=:portype",HWPORT_TABLE);
			StmtPrepare(stmthp,ociError, selectport, &status);
			BindByName(stmthp,&bnd1p[0],ociError,":snb",serialnb,&status);
			BindByName(stmthp,&bnd1p[1],ociError,":portnb",port_nb,&status);
			BindByName(stmthp,&bnd1p[2],ociError,":portway",&port_way,&status);
			BindByName(stmthp,&bnd1p[3],ociError,":portype",port_type1,&status);
			StmtExecute(ociHdbc, stmthp, ociError, &status);
			ParamGet(stmthp, ociError, &parmdp, 1, &status);
			AttrGet(parmdp, &dnamelen, ociError, &status);
			ParamGet(stmthp, ociError, &parmdp, 2, &status);
			AttrGet(parmdp, &portnblen, ociError, &status);
			ParamGet(stmthp, ociError, &parmdp, 4, &status);
			AttrGet(parmdp, &portypelen, ociError, &status);
			ParamGet(stmthp, ociError, &parmdp, 5, &status);
			AttrGet(parmdp, &bialen, ociError, &status);			
			ParamGet(stmthp, ociError, &parmdp, 6, &status);
			AttrGet(parmdp, &maclen, ociError, &status);

			portype_temp = (char *) realloc(portype_temp,(portypelen + 1)*sizeof(char));
			portnb_temp = (char *) realloc(portnb_temp,(portnblen + 1)*sizeof(char));
			bia_temp= (char *) realloc(bia_temp,(bialen + 1)*sizeof(char));
			mac_temp = (char *) realloc(mac_temp,(maclen + 1)*sizeof(char));
			snb_temp= (char *) realloc(snb_temp,(dnamelen + 1)*sizeof(char));

			if(snb_temp==NULL || bia_temp==NULL||mac_temp==NULL|| portnb_temp==NULL||portype_temp==NULL)
			{
				status=-10;
				rescode=ShowErrors (status, ociError, "  pointer allocation unsuccessful");
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				if(snb_temp!=NULL)
					free(snb_temp);
				if(portype_temp!=NULL)
					free(portype_temp);
				if(portnb_temp!=NULL)
					free(portnb_temp);
				if(mac_temp!=NULL)
					free(mac_temp);
				if(bia_temp!=NULL)
					free(bia_temp);

				GetErrorMess(appliName,"REALLOC UNSUCCESSFUL",ErrorMessage,1);
				return -1;
			}
			DefineByPos(stmthp,def,ociError,1, snb_temp,dnamelen+1,&status);		
			DefineByPos(stmthp,def,ociError,2, portnb_temp,portnblen+1,&status);		
			DefineByPos(stmthp,def,ociError,3, &portway,&status);		
			DefineByPos(stmthp,def,ociError,4, portype_temp,portypelen+1,&status);		
			DefineByPos(stmthp,def,ociError,5, bia_temp, bialen + 1,&status);		
			DefineByPos(stmthp,def,ociError,6, mac_temp,maclen+1,&status);		
						
		}catch(Error err){
			sprintf(appliName,"GetSparePortRow_snb");	///////
			rescode=ShowErrors (status, err.ociError, err.log);
			
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			else
				GetErrorMess(appliName,err.msg,ErrorMessage,1);
			
			if(strstr(err.msg,"NOT CONNECTED TO ANY DB")!=NULL)
			return -1;
		}

		if(status==OCI_SUCCESS)
			status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT,1,OCI_DEFAULT);

		if (status==OCI_SUCCESS || status==OCI_SUCCESS_WITH_INFO) 
		{	
			RemoveSeparator(mac_temp,"?");
			RemoveSeparator(portnb_temp,"?");
			RemoveSeparator(snb_temp,"?");
			RemoveSeparator(bia_temp,"?");
			RemoveSeparator(portype_temp,"?");
			sprintf(port_row_result,"|serialnb (C):%s |port_nbr (C):%s |port_way (I):%d |port_type (C):%s |bia (C):%s |macaddress (C):%s |",snb_temp,portnb_temp,portway,portype_temp,bia_temp,mac_temp);
		} 
		else 
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "OCIFETCH unsuccessful\n");
				strcpy(port_row_result,"NO_ROWS_SELECTED");
				GetErrorMess(appliName,"NO_ROWS_SELECTED",ErrorMessage,1);
			}
		}

		if(bia_temp!=NULL)
			free(bia_temp);
		if(portnb_temp!=NULL)
			free(portnb_temp);
		if(mac_temp!=NULL)
			free(mac_temp);
		if(portype_temp!=NULL)
			free(portype_temp);
		if(snb_temp!=NULL)
			free(snb_temp);
		if(strstr(ErrorMessage,"NO_ROWS_SELECTED")==NULL)
		{
			if(rescode==0)
			{
				OCIReportError(ociError,appliName,ErrorMessage,0); 
			}
			else
			{
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			}
		}
		status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
		return (rescode+status);
	}

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif