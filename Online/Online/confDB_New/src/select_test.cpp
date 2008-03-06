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
	
/*EXTERN_CONFDB
int _GetDeviceTypeRow(char* devitype,int &len_devtype,char* devtype_result,char* ErrorMessage){
	printf("\n----------------------------\n");

	char appliName[100]="GetDeviceTypeRow";		int i = 0;		int j=0;		int pos1=0;		int pos2=0;		int len1=0;		int pos3=0;		int rescode=0;		int free_mem=0;
	ub4 numcols = 0;		ub4 collen=0;		ub2 type = 0;		sword status;		OCIStmt *stmthp;		OCIBind  *bnd1p = (OCIBind *) 0; 		OCIDefine* def[7];		sb4  descriptionlen=0;
	sb4 devtypelen=0;		sb4 rgbcolorlen=0;		OCIParam *parmdp;		char * devicetype_temp=NULL;		char * description_temp=NULL;		char* rgbcolor_temp=NULL;		int dtype_null=0;
	int description_null=0;		int rgbcolor_null=0;		char  buffer[10];		int nbrofinput=0;		int nbrofoutput=0;		int dtypeID=0;		char logmessage[100];		int res1=0;		int sysID=0;		char* sysIDList=NULL;
	
	int res=0;
	int errcode;
	char errbuf[100];
	
	if(devitype==NULL)
		{
			GetErrorMess(appliName," devicetype MUST be given",ErrorMessage,1);
			return -1;
		}
	/////////////////////////////////////
		try{
			HandleAlloc(ociEnv,&stmthp,ociError,&status);
			
			char selectdevtype[1000];
			sprintf(selectdevtype,"select devicetype||'?',devicetypeID,nbrofinput,nbrofoutput, nvl(description,'none')||'?',nvl(rgbcolor,'none')||'?',nvl(system_name,-1) from %s where devicetype=:dtype ",DEVICETYPE_TABLE);
	
			StmtPrepare(stmthp,ociError, selectdevtype,&status);
			BindByName(stmthp,&bnd1p,ociError,":dtype",devitype,&status);
			StmtExecute(ociHdbc, stmthp, ociError, &status);

			ParamGet(stmthp, ociError, &parmdp, 1, &status);
			AttrGet(parmdp, devtypelen, ociError, &status);
			ParamGet(stmthp, ociError, &parmdp, 5, &status);
			AttrGet(parmdp, descriptionlen, ociError, &status);
			ParamGet(stmthp, ociError, &parmdp, 6, &status);
			AttrGet(parmdp, rgbcolorlen, ociError, &status);

			MinStringLength(devtypelen); //case it will be a null value need a buffer bigger than 5
			MinStringLength(descriptionlen);

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
			
			DefineByPos(stmthp,def,ociError,1, devicetype_temp,devtypelen+1,dtype_null,&status);							
			DefineByPos(stmthp,def,ociError,2,&dtypeID,&status);				
			DefineByPos(stmthp,def,ociError,3,&nbrofinput,&status);
			DefineByPos(stmthp,def,ociError,4,&nbrofoutput,&status);
			DefineByPos(stmthp,def,ociError,5, description_temp, descriptionlen + 1,description_null,&status);				
			DefineByPos(stmthp,def,ociError,6, rgbcolor_temp, descriptionlen + 1,rgbcolor_null,&status);	
			DefineByPos(stmthp,def,ociError,7,&sysID,&status);		
		
		}catch(Error err){
			sprintf(appliName,"GetDeviceTypeRow");	///////
			rescode=ShowErrors (status, err.ociError, err.log);
			
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			else
				GetErrorMess(appliName,err.msg,ErrorMessage,1);
			
			return -1;
		}
	
	if(status==OCI_SUCCESS)
			status =OCIStmtFetch2(stmthp,ociError, 1, OCI_FETCH_NEXT,1, OCI_DEFAULT);

		if (status == OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO) 
		{	
			Format_output(description_null,description_temp,  logmessage,'?');
			Format_output(dtype_null,devicetype_temp, logmessage,'?');
			Format_output(rgbcolor_null,rgbcolor_temp, logmessage,'?');
			sysIDList=(char*)malloc(200*sizeof(char));
			if(sysIDList!=NULL)
				res1=DecomposeSysID_bis(sysID, sysIDList);		//sysID musi byc rozne od 0!!!
			else
			{
				GetErrorMess(appliName, "Malloc unsuccessfull",ErrorMessage,1);
				if(devicetype_temp!=NULL)
					devicetype_temp = (char *) realloc(devicetype_temp,(0)*sizeof(char));
				if(description_temp!=NULL)
					description_temp= (char *) realloc(description_temp,(0)*sizeof(char));
				if(rgbcolor_temp!=NULL)
					rgbcolor_temp= (char *) realloc(rgbcolor_temp,(0)*sizeof(char));
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				return -1;
			}
			//case where value has been truncated
			pos1=strcspn(description_temp,"?");
			pos2=strcspn(devicetype_temp,"?");
			pos3=strcspn(rgbcolor_temp,"?");
			description_temp[pos1]='\0';
			devicetype_temp[pos2]='\0';
			rgbcolor_temp[pos3]='\0';
			len1=strlen("|devicetype (C): |devicetypeID (I): |nbrofinput (I): |nbrofoutput (I): |description (C): |rgbcolor (C): |system_name (C): |")+strlen(devicetype_temp)+strlen(description_temp)+strlen(rgbcolor_temp)+strlen(sysIDList);
			sprintf(buffer,"%d",nbrofinput);
			len1+=strlen(buffer);
			sprintf(buffer,"%d",nbrofoutput);
			len1+=strlen(buffer);
			sprintf(buffer,"%d",dtypeID);
			len1+=strlen(buffer);
			if(len_devtype<len1)
			{	
				len1++;
				len_devtype=len1;
				GetErrorMess(appliName, "BUFFER_TOO_SMALL",ErrorMessage,1);
				if(devicetype_temp!=NULL)
					devicetype_temp = (char *) realloc(devicetype_temp,(0)*sizeof(char));
				free(sysIDList);
				if(description_temp!=NULL)
					description_temp= (char *) realloc(description_temp,(0)*sizeof(char));
				if(rgbcolor_temp!=NULL)
					rgbcolor_temp= (char *) realloc(rgbcolor_temp,(0)*sizeof(char));
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				return -1;


			}
			else
			{
				sprintf(devtype_result,"|devicetype (C):%s |devicetypeID (I):%d |nbrofinput (I):%d |nbrofoutput (I):%d |description (C):%s |rgbcolor (C):%s |system_name (C):%s |",devicetype_temp,dtypeID,nbrofinput,nbrofoutput,description_temp,rgbcolor_temp,sysIDList);
			}

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
			devicetype_temp = (char *) realloc(devicetype_temp,(0)*sizeof(char));

		if(description_temp!=NULL)
			description_temp= (char *) realloc(description_temp,(0)*sizeof(char));
		if(rgbcolor_temp!=NULL)
			rgbcolor_temp= (char *) realloc(rgbcolor_temp,(0)*sizeof(char));
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
		
}*/
EXTERN_CONFDB
	int _GetDeviceTypeRow(char* devitype,int &len_devtype,char* devtype_result,char* ErrorMessage)
	{
		printf("\n----------------------------\n");
		char appliName[100]="GetDeviceTypeRow";
		int i = 0;
		int j=0;
		int pos1=0;
		int pos2=0;
		int len1=0;
		int pos3=0;
		int rescode=0;
		int free_mem=0;
		ub4 numcols = 0;
		ub4 collen=0;
		ub2 type = 0;
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
		int dtype_null=0;
		int description_null=0;
		int rgbcolor_null=0;
		char  buffer[10];
		int nbrofinput=0;
		int nbrofoutput=0;
		int dtypeID=0;
		char logmessage[100];
		int res1=0;
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
			AttrGet(parmdp, devtypelen, ociError, &status);
			ParamGet(stmthp, ociError, &parmdp, 5, &status);
			AttrGet(parmdp, descriptionlen, ociError, &status);
			ParamGet(stmthp, ociError, &parmdp, 6, &status);
			AttrGet(parmdp, rgbcolorlen, ociError, &status);

			// Use the retrieved length of dept to allocate an output buffer, and then define the output variable. If the define call returns an error,exit the application 
			MinStringLength(devtypelen); //case it will be a null value need a buffer bigger than 5
			MinStringLength(descriptionlen);

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
			
			DefineByPos(stmthp,def,ociError,1, devicetype_temp,devtypelen+1,dtype_null,&status);							
			DefineByPos(stmthp,def,ociError,2,&dtypeID,&status);				
			DefineByPos(stmthp,def,ociError,3,&nbrofinput,&status);
			DefineByPos(stmthp,def,ociError,4,&nbrofoutput,&status);
			DefineByPos(stmthp,def,ociError,5, description_temp, descriptionlen + 1,description_null,&status);				
			DefineByPos(stmthp,def,ociError,6, rgbcolor_temp, descriptionlen + 1,rgbcolor_null,&status);	
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
			Format_output(description_null,description_temp,  logmessage,'?');
			Format_output(dtype_null,devicetype_temp, logmessage,'?');
			Format_output(rgbcolor_null,rgbcolor_temp, logmessage,'?');
			sysIDList=(char*)malloc(200*sizeof(char));
			if(sysIDList!=NULL)
				res1=DecomposeSysID_bis(sysID, sysIDList);
			else
			{
				GetErrorMess(appliName, "Malloc unsuccessfull",ErrorMessage,1);
				if(devicetype_temp!=NULL)
					devicetype_temp = (char *) realloc(devicetype_temp,(0)*sizeof(char));
				if(description_temp!=NULL)
					description_temp= (char *) realloc(description_temp,(0)*sizeof(char));
				if(rgbcolor_temp!=NULL)
					rgbcolor_temp= (char *) realloc(rgbcolor_temp,(0)*sizeof(char));
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				return -1;
			}
			//case where value has been truncated
			pos1=strcspn(description_temp,"?");
			pos2=strcspn(devicetype_temp,"?");
			pos3=strcspn(rgbcolor_temp,"?");
			description_temp[pos1]='\0';
			devicetype_temp[pos2]='\0';
			rgbcolor_temp[pos3]='\0';
			len1=strlen("|devicetype (C): |devicetypeID (I): |nbrofinput (I): |nbrofoutput (I): |description (C): |rgbcolor (C): |system_name (C): |")+strlen(devicetype_temp)+strlen(description_temp)+strlen(rgbcolor_temp)+strlen(sysIDList);
			sprintf(buffer,"%d",nbrofinput);
			len1+=strlen(buffer);
			sprintf(buffer,"%d",nbrofoutput);
			len1+=strlen(buffer);
			sprintf(buffer,"%d",dtypeID);
			len1+=strlen(buffer);
			if(len_devtype<len1)
			{	
				len1++;
				len_devtype=len1;
				GetErrorMess(appliName, "BUFFER_TOO_SMALL",ErrorMessage,1);
				if(devicetype_temp!=NULL)
					devicetype_temp = (char *) realloc(devicetype_temp,(0)*sizeof(char));
				free(sysIDList);
				if(description_temp!=NULL)
					description_temp= (char *) realloc(description_temp,(0)*sizeof(char));
				if(rgbcolor_temp!=NULL)
					rgbcolor_temp= (char *) realloc(rgbcolor_temp,(0)*sizeof(char));
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				return -1;


			}
			else
			{
				sprintf(devtype_result,"|devicetype (C):%s |devicetypeID (I):%d |nbrofinput (I):%d |nbrofoutput (I):%d |description (C):%s |rgbcolor (C):%s |system_name (C):%s |",devicetype_temp,dtypeID,nbrofinput,nbrofoutput,description_temp,rgbcolor_temp,sysIDList);
			}

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
			devicetype_temp = (char *) realloc(devicetype_temp,(0)*sizeof(char));

		if(description_temp!=NULL)
			description_temp= (char *) realloc(description_temp,(0)*sizeof(char));
		if(rgbcolor_temp!=NULL)
			rgbcolor_temp= (char *) realloc(rgbcolor_temp,(0)*sizeof(char));
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
/*********************************************************************/
EXTERN_CONFDB
	int _GetDeviceRow_devicename(char* functionaldeviname,int &len_device, char* device_result,char* ErrorMessage)
	{
		printf("\n***********************************************\n");
		char appliName[100]="GetDeviceRow_devicename";
		int i = 0;
		int j=0;
		int pos1=0;
		int pos2=0;
		int pos3=0;
		int pos4=0;
		int pos5=0;
		int free_mem=0;
		ub4 numcols = 0;
		ub4 collen=0;
		ub2 type = 0;
		sword status;
		int rescode=0;
		OCIStmt *stmthp;
		OCIBind  *bnd1p = (OCIBind *) 0; 
		OCIBind  *bnd2p = (OCIBind *) 0;
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
		int status1=0;
		int nodeused=0;
		char buffer[20];
		int len=0;
		char * devicename_temp=NULL;
		char * devicetype_temp=NULL;
		char * location_temp=NULL;
		char * serialnb_temp=NULL;
		char * devicefunction_temp=NULL;
		int sysID=0;
		int dname_null=0;
		int dtype_null=0;
		int serialnb_null=0;
		int location_null=0;
		int devfunction_null=0;
		char logmessage[100];
		char* sysIDlist=NULL;
		int res1=0;
		if(functionaldeviname==NULL)
		{
			GetErrorMess(appliName,"devicename MUST be given",ErrorMessage,1);
			return -1;
		}	
		char selectdev[1000];
		status =OCIHandleAlloc (ociEnv, (void**)&stmthp, OCI_HTYPE_STMT , 0, 0);
		if(status!=OCI_SUCCESS)
		{
			rescode=ShowErrors (status, ociError, "OCIStmtHAndleStmt unsuccessful");
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			else
				GetErrorMess(appliName,"NOT CONNECTED TO ANY DB",ErrorMessage,1);
			return -1;

		}
		else
		{
			sprintf(selectdev,"select t.devicename||'?',e.devicetype||'?',t.active,t.node,t.deviceid,t.promiscuous_mode,nvl(t.location,'none')||'?',t.nodeused,t.serialnb||'?',nvl(t.system_name,-1),%s(t.functionID)||'?' from %s e,%s t where t.devicename=:dname and e.devicetypeID=t.devicetypeID",_DecomposeFctID,DEVICETYPE_TABLE,LOGICAL_DEVICE_TABLE);
			status=OCIStmtPrepare(stmthp, ociError, (text*) selectdev,(ub4) strlen(selectdev),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else	
			//////////////////////////////////
			try{
				BindByName(stmthp,&bnd1p,ociError,":dname",functionaldeviname,&status);
			}catch(Error err){
				if(rescode==0){
					rescode=ShowErrors (status, err.ociError, err.log);
				}
			
				if(ociError!=0)
					OCIReportError(ociError,appliName,ErrorMessage,1); 
				else
					GetErrorMess(appliName,err.msg,ErrorMessage,1);
			}
			
			/////////////////////////////////
	/*		status=OCIBindByName(stmthp, &bnd1p, ociError,(text*)":dname",-1,(dvoid*) functionaldeviname,strlen(functionaldeviname)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);


		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIBindByPos unsuccessful");
		}
		else	*/
		if(status==OCI_SUCCESS)			//
			status=OCIStmtExecute(ociHdbc, stmthp, ociError, 0, 0,(OCISnapshot *) 0, (OCISnapshot *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
		}
		else
		{
			/* The next two statements describe the select-list item, dept, and return its length to allocate the memory*/
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 1);
		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet1 unsuccessful");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &devlen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIAttrGet1 unsuccessful");
		}
		else
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 2);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet2 unsuccessful");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &devtypelen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIAttrGet2 unsuccessful");
		}
		else
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 7);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet3 unsuccessful");
		}
		else 
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &locationlen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIAttrGet3 unsuccessful");
		}
		else
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 9);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet4  unsuccessful");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &serialnblen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIAttrGet2 unsuccessful");
		}
		else
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 11);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet5 unsuccessful");
		}
		else 
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &devfunctionlen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);


		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIAttrGet4 unsuccessful");
		}
		else
		{
			/* Use the retrieved length of dept to allocate an output buffer, and
			then define the output variable. If the define call returns an error,
			exit the application */
			MinStringLength(devtypelen);
			MinStringLength(devlen);
			MinStringLength(locationlen);
			MinStringLength(serialnblen);
			MinStringLength(devfunctionlen);
			//std::cout << "devtype_len" << devtypelen<< std::endl;
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
			else
				/////////////////////
					try{
                    DefineByPos(stmthp,def,ociError,1, devicename_temp,devlen+1,dname_null,&status);							
					DefineByPos(stmthp,def,ociError,2, devicetype_temp,devtypelen+1,dtype_null,&status);							
					DefineByPos(stmthp,def,ociError,3,&active,&status);				
					DefineByPos(stmthp,def,ociError,4,&node,&status);
					DefineByPos(stmthp,def,ociError,5,&deviceid,&status);
					DefineByPos(stmthp,def,ociError,6,&promiscuous_mode,&status);
					DefineByPos(stmthp,def,ociError,7, location_temp, locationlen+1,location_null,&status);				
					DefineByPos(stmthp,def,ociError,8,&nodeused,&status);
					DefineByPos(stmthp,def,ociError,9, serialnb_temp, serialnblen + 1,serialnb_null,&status);	
					DefineByPos(stmthp,def,ociError,10,&sysID,&status);
					DefineByPos(stmthp,def,ociError,11, devicefunction_temp, devfunctionlen+1,0,&status);	
											
			}catch(Error err){
				if(rescode==0){
					rescode=ShowErrors (status, err.ociError, err.log);
				}
			
				if(ociError!=0)
					OCIReportError(ociError,appliName,ErrorMessage,1); 
				else
					GetErrorMess(appliName,err.msg,ErrorMessage,1);
			}
		
				////////////////////////
			//	status =OCIDefineByPos(stmthp, &def[0], ociError,1, (ub1 *) (devicename_temp), devlen + 1,SQLT_STR, (dvoid *) &dname_null,(ub2 *) 0,0, OCI_DEFAULT);
		}
		/*if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "DefineByPos0 unsuccessful");

			}
		}
		else
			status=OCIDefineByPos (stmthp, &def[1], ociError, 2,devicetype_temp,devtypelen+1, SQLT_STR, (dvoid *) &dtype_null,0, 0,OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "DefineByPos1 unsuccessful");

			}
		}
		else
			status=OCIDefineByPos (stmthp, &def[2], ociError, 3,&active,sizeof(active), SQLT_INT, 0, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "DefineByPos2 unsuccessful");
			}
		}
		else
			status=OCIDefineByPos (stmthp, &def[3], ociError, 4,&node,sizeof(node), SQLT_INT, 0, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "DefineByPos3 unsuccessful");
			}
		}
		else
			status=OCIDefineByPos (stmthp, &def[4], ociError, 5,&deviceid,sizeof(deviceid), SQLT_INT, 0, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "DefineByPos4 unsuccessful");
			}
		}
		else
			status=OCIDefineByPos (stmthp, &def[5], ociError,6,&promiscuous_mode,sizeof(promiscuous_mode), SQLT_INT, 0, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "DefineByPos5 unsuccessful");
			}
		}
		else
			status=OCIDefineByPos (stmthp, &def[6], ociError, 7,location_temp,locationlen+1, SQLT_STR, (dvoid *) &location_null,0, 0,OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "DefineByPos6 unsuccessful");

			}
		}
		else	
			status=OCIDefineByPos (stmthp, &def[7], ociError,8,&nodeused,sizeof(nodeused), SQLT_INT, 0, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "DefineByPos7 unsuccessful");

			}
		}
		else
			status =OCIDefineByPos(stmthp, &def[8], ociError,9, (ub1 *)(serialnb_temp), serialnblen + 1,SQLT_STR, (dvoid *) &serialnb_null,(ub2 *) 0,0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "DefineByPos7 unsuccessful");

			}
		}
		else
			status =OCIDefineByPos(stmthp, &def[9], ociError,10, (ub1 *) &sysID,sizeof(int),SQLT_INT, (dvoid *)0,(ub2 *) 0,0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "DefineByPos10 unsuccessful");

			}
		}
		else
			status =OCIDefineByPos(stmthp, &def[10], ociError,11, (ub1 *) devicefunction_temp,devfunctionlen+1,SQLT_STR, (dvoid *)0,(ub2 *) 0,0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "DefineByPos8 unsuccessful");

			}
		}
		else*/
		if(status==OCI_SUCCESS)

			status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT,1,OCI_DEFAULT);

		if (status==OCI_SUCCESS || status==OCI_SUCCESS_WITH_INFO) 
		{	
			Format_output(serialnb_null,serialnb_temp, logmessage,'?');
			Format_output(dtype_null,devicetype_temp,logmessage,'?');
			Format_output(location_null,location_temp,logmessage,'?');
			Format_output(dname_null,devicename_temp,logmessage,'?');
			Format_output(devfunction_null,devicefunction_temp,logmessage,'?');

			pos1=strcspn(devicename_temp,"?");
			pos2=strcspn(devicetype_temp,"?");
			pos3=strcspn(location_temp,"?");
			pos4=strcspn(serialnb_temp,"?");
			pos5=strcspn(devicefunction_temp,"?");
			devicename_temp[pos1]='\0';
			devicetype_temp[pos2]='\0';
			location_temp[pos3]='\0';
			serialnb_temp[pos4]='\0';
			devicefunction_temp[pos5]='\0';
			sysIDlist=(char*)malloc(200*sizeof(char));
			if(sysIDlist!=NULL)
				res1=DecomposeSysID_bis(sysID, sysIDlist);
			else
			{
				GetErrorMess(appliName, "Malloc unsuccessfull",ErrorMessage,1);
				if(devicetype_temp!=NULL)
					devicetype_temp = (char *) realloc(devicetype_temp,(0)*sizeof(char));
				if(devicename_temp!=NULL)
					devicename_temp= (char *) realloc(devicename_temp,(0)*sizeof(char));
				if(serialnb_temp!=NULL)
					serialnb_temp = (char *) realloc(serialnb_temp,(0)*sizeof(char));
				if(location_temp!=NULL)
					location_temp= (char *) realloc(location_temp,(0)*sizeof(char));
				if(devicefunction_temp!=NULL)
					devicefunction_temp= (char *) realloc(devicefunction_temp,(0)*sizeof(char));
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				return -1;
			}
			len=strlen(sysIDlist)+strlen(devicefunction_temp)+strlen(devicetype_temp)+strlen(devicename_temp)+strlen(location_temp)+strlen(serialnb_temp)+strlen("|devicename (C): |devicetype (C): |active (I):1 |node (I):1 |deviceid  (I): |promiscuous_mode (I):1 |location (C): |nodeused (I):1 |serialnb  (C): |device_function (C): |system_name (C): |");
			sprintf(buffer,"%d",deviceid);
			len+=strlen(buffer);

			if(len_device<len)
			{
				len++;
				len_device=len;
				GetErrorMess(appliName, "BUFFER_TOO_SMALL",ErrorMessage,1);
				if(devicetype_temp!=NULL)
					devicetype_temp = (char *) realloc(devicetype_temp,(0)*sizeof(char));
				if(devicename_temp!=NULL)
					devicename_temp= (char *) realloc(devicename_temp,(0)*sizeof(char));
				if(serialnb_temp!=NULL)
					serialnb_temp = (char *) realloc(serialnb_temp,(0)*sizeof(char));
				if(location_temp!=NULL)
					location_temp= (char *) realloc(location_temp,(0)*sizeof(char));
				if(devicefunction_temp!=NULL)
					devicefunction_temp= (char *) realloc(devicefunction_temp,(0)*sizeof(char));
				free(sysIDlist);
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				return -1;

			}
			else
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
			devicetype_temp = (char *) realloc(devicetype_temp,(0)*sizeof(char));
		if(devicename_temp!=NULL)
			devicename_temp= (char *) realloc(devicename_temp,(0)*sizeof(char));
		if(location_temp!=NULL)
			location_temp = (char *) realloc(location_temp,(0)*sizeof(char));
		if(serialnb_temp!=NULL)
			serialnb_temp= (char *) realloc(serialnb_temp,(0)*sizeof(char));
		if(devicefunction_temp!=NULL)
			devicefunction_temp= (char *) realloc(devicefunction_temp,(0)*sizeof(char));
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



#if defined(__cplusplus) || defined(c_plusplus)
}
#endif