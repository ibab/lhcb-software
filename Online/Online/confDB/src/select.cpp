/********************************************************************************/
// Author: L.Abadie
// version:v3.5
// changes: handling error such that can be extended to C++: (create a class SQLException) (strlen ck + update stmt)
// file which performs select statements
// N.B : Memory management (allocation+release) should be handled by the user application thus, for all the fct provided by this lib
/********************************************************************************/
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
			char selectdevtype[1000];
			sprintf(selectdevtype,"select devicetype||'?',devicetypeID,nbrofinput,nbrofoutput, nvl(description,'none')||'?',nvl(rgbcolor,'none')||'?',nvl(system_name,-1) from %s where devicetype=:dtype ",DEVICETYPE_TABLE);
			status=OCIStmtPrepare(stmthp, ociError, (text*) selectdevtype,(ub4) strlen(selectdevtype),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
		}

		if(status!=OCI_SUCCESS )
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
			}
		}
		else
			status=OCIBindByName(stmthp, &bnd1p, ociError,(text*)":dtype",-1,(dvoid*) devitype,strlen(devitype)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "OCIBindByPos unsuccessful");
			}
		}
		else
			status=OCIStmtExecute(ociHdbc, stmthp, ociError, 0, 0,(OCISnapshot *) 0, (OCISnapshot *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
			}
		}
		else
		{
			/* The next two statements describe the select-list item, dept, and return its length to allocate the memory*/
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 1);
		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "OCIParamGet1 unsuccessful");
			}
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &devtypelen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);
		//std:://cout<<"devtypelen="<<devtypelen<<std::endl;	

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "OCIAttrGet1 unsuccessful");
			}
		}
		else 
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT,ociError,(dvoid **) &parmdp, (ub4) 5);
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "OCIParamGet2 unsuccessful");
			}
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &descriptionlen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);
		//std:://cout<<"descriptionlen="<<descriptionlen<<std::endl;	
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "OCIAttrGet1 unsuccessful");
			}
		}
		else 
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT,ociError,(dvoid **) &parmdp, (ub4) 6);
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "OCIParamGet3 unsuccessful");
			}
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &rgbcolorlen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);
		//std:://cout<<"rgbcolorlen="<<rgbcolorlen<<std::endl;		
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{

				rescode=ShowErrors (status, ociError, "OCIAttrGet2 unsuccessful");
			}
		}
		else
		{
			/* Use the retrieved length of dept to allocate an output buffer, and then define the output variable. If the define call returns an error,exit the application */
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
			else
				status =OCIDefineByPos(stmthp, &def[0], ociError,1, (ub1 *) (devicetype_temp), devtypelen + 1,SQLT_STR, (dvoid *)&dtype_null,(ub2 *) 0,0, OCI_DEFAULT);
		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{

				rescode=ShowErrors (status, ociError, "OCIDefineByPos0 unsuccessful");		
			}
		}
		else
			status=OCIDefineByPos (stmthp, &def[1],ociError, 2,&dtypeID,sizeof(nbrofinput), SQLT_INT, 0, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{

				rescode=ShowErrors (status, ociError, "OCIDefineByPos0 unsuccessful");		
			}
		}
		else
			status=OCIDefineByPos (stmthp, &def[2],ociError, 3,&nbrofinput,sizeof(nbrofinput), SQLT_INT, 0, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "OCIDefineByPos1 unsuccessful");

			}
		}
		else
			status=OCIDefineByPos (stmthp, &def[3], ociError, 4,&nbrofoutput,sizeof(nbrofoutput), SQLT_INT, 0, 0, 0, OCI_DEFAULT);
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "OCIDefineByPos2 unsuccessful");

			}
		}
		else
			status =OCIDefineByPos(stmthp, &def[4], ociError,5, (ub1 *)(description_temp), descriptionlen + 1,SQLT_STR, (dvoid *) &description_null,(ub2 *) 0,0, OCI_DEFAULT);
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "OCIDefineByPos3 unsuccessful");

			}
		}
		else
			status =OCIDefineByPos(stmthp, &def[5], ociError,6, (ub1 *)(rgbcolor_temp), descriptionlen + 1,SQLT_STR, (dvoid *) &rgbcolor_null,(ub2 *) 0,0, OCI_DEFAULT);
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "OCIDefineByPos1 unsuccessful");

			}
		}
		else
			status=OCIDefineByPos (stmthp, &def[6], ociError, 7,&sysID,sizeof(int), SQLT_INT, 0, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "OCIDefineByPos4 unsuccessful");
			}
		}
		else
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
			status=OCIBindByName(stmthp, &bnd1p, ociError,(text*)":dname",-1,(dvoid*) functionaldeviname,strlen(functionaldeviname)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);


		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIBindByPos unsuccessful");
		}
		else	
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
			//std:://cout << "devtype_len" << devtypelen<< std::endl;
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
				status =OCIDefineByPos(stmthp, &def[0], ociError,1, (ub1 *) (devicename_temp), devlen + 1,SQLT_STR, (dvoid *) &dname_null,(ub2 *) 0,0, OCI_DEFAULT);
		}
		if(status!=OCI_SUCCESS)
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
		else
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
		int i = 0;
		int j=0;
		int pos1=0;
		int pos2=0;
		int pos3=0;
		int pos4=0;
		int pos5=0;
		int node=0;
		int active=0;
		int deviceid=0;
		int promiscuous_mode=0;
		int status1=0;
		int nodeused=0;
		ub4 numcols = 0;
		int rescode=0;
		int free_mem=0;
		ub4 collen=0;
		ub2 type = 0;
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
		int len=0;
		char buffer[20];
		char * devicename_temp=NULL;
		char * devicetype_temp=NULL;
		char * serialnb_temp=NULL;
		char * location_temp=NULL;
		char* functiondev_temp=NULL;
		char selectdev[1000];

		int dname_null=0;
		int dtype_null=0;
		int location_null=0;
		int serialnb_null=0;
		int devfunction_null=0;
		int sysID=0;
		char* sysIDlist=NULL;
		int res1=0;
		char logmessage[100];
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
			sprintf(selectdev,"select t.devicename||'?',e.devicetype||'?',t.active,t.node,t.deviceid,t.promiscuous_mode,nvl(t.location,'none')||'?',t.nodeused,t.serialnb,nvl(t.system_name,-1), %s(t.functionid)||'?' from %s t,%s e where t.deviceid=:dID and e.devicetypeID=t.devicetypeID",_DecomposeFctID,LOGICAL_DEVICE_TABLE,DEVICETYPE_TABLE);
			status=OCIStmtPrepare(stmthp, ociError, (text*) selectdev,(ub4) strlen(selectdev),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bnd1p, ociError,(text*)":dID",-1,(dvoid*) &deviceID,sizeof(deviceID), SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIBindByPos unsuccessful");
		}
		else	
			status=OCIStmtExecute(ociHdbc, stmthp, ociError, 0, 0,(OCISnapshot *) 0, (OCISnapshot *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIExecute unsuccessful");
		}
		else
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 1);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet1  unsuccessful");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &devlen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIAttrGet1  unsuccessful");
		}
		else
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 2);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet2  unsuccessful");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &devtypelen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIAttrGet2  unsuccessful");
		}
		else
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 7);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet3  unsuccessful");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &locationlen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIAttrGet3  unsuccessful");
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
				rescode=ShowErrors (status, ociError, "OCIAttrGet3  unsuccessful");
		}
		else
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 11);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet4  unsuccessful");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &functiondevlen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);


		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIAttrGet4  unsuccessful");
		}
		else
		{
			MinStringLength(devtypelen);
			MinStringLength(devlen);
			MinStringLength(locationlen);
			MinStringLength(serialnblen);
			MinStringLength(functiondevlen);

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
			else
				status =OCIDefineByPos(stmthp, &def[0], ociError,1, (ub1 *) (devicename_temp), devlen + 1,SQLT_STR, (dvoid *) &dname_null,(ub2 *) 0,0, OCI_DEFAULT);
		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "DefineByPos0  unsuccessful");
			}
		}
		else
			status=OCIDefineByPos (stmthp, &def[1], ociError, 2,devicetype_temp,devtypelen+1, SQLT_STR, (dvoid *)&dtype_null,(ub2 *) 0,0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "DefineByPos1  unsuccessful");
			}
		}
		else
			status=OCIDefineByPos (stmthp, &def[2], ociError, 3,&active,sizeof(active), SQLT_INT, 0, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "DefineByPos2  unsuccessful");
			}
		}
		else
			status=OCIDefineByPos (stmthp, &def[3], ociError, 4,&node,sizeof(node), SQLT_INT, 0, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "DefineByPos3  unsuccessful");
			}
		}
		else
			status=OCIDefineByPos (stmthp, &def[4], ociError, 5,&deviceid,sizeof(deviceid), SQLT_INT, 0, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "DefineByPos4  unsuccessful");
			}
		}
		else
			status=OCIDefineByPos (stmthp, &def[5], ociError,6,&promiscuous_mode,sizeof(promiscuous_mode), SQLT_INT, 0, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "DefineByPos5  unsuccessful");
			}
		}
		else
			status=OCIDefineByPos (stmthp, &def[6], ociError,7,location_temp,locationlen+1, SQLT_STR, (dvoid *) &location_null, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "DefineByPos6  unsuccessful");

			}
		}
		else
			status=OCIDefineByPos (stmthp, &def[7], ociError,8,&nodeused,sizeof(nodeused), SQLT_INT, 0, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "DefineByPos7  unsuccessful");

			}
		}
		else
			status =OCIDefineByPos(stmthp, &def[8], ociError,9, (ub1 *)(serialnb_temp), serialnblen + 1,SQLT_STR, (dvoid *) &serialnb_null,(ub2 *) 0,0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "DefineByPos7  unsuccessful");

			}
		}
		else
			status =OCIDefineByPos(stmthp, &def[9], ociError,10, (ub1 *) &sysID,sizeof(int),SQLT_INT, (dvoid *) 0,(ub2 *) 0,0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "DefineByPos7  unsuccessful");

			}
		}
		else
			status =OCIDefineByPos(stmthp, &def[10], ociError,11, (ub1 *) functiondev_temp,functiondevlen,SQLT_STR, (dvoid *) 0,(ub2 *) 0,0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "DefineByPos8  unsuccessful");

			}
		}
		else
			status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT,1, OCI_DEFAULT);

		if (status==OCI_SUCCESS || status==OCI_SUCCESS_WITH_INFO) 
		{	
			Format_output(serialnb_null,serialnb_temp, logmessage,'?');
			//std:://cout <<"logmessage bar "<<logmessage << "oracle "<<bar_null <<std::endl;
			Format_output(dtype_null,devicetype_temp, logmessage,'?');
			//std:://cout <<"logmessage dtype "<<logmessage <<"oracle "<<dtype_null <<std::endl;
			Format_output(location_null,location_temp, logmessage,'?');
			//std:://cout <<"logmessage uuid "<<logmessage <<"oracle "<<uuid_null <<std::endl;
			Format_output(dname_null,devicename_temp, logmessage,'?');
			//std:://cout0 <<"logmessage dname "<<logmessage <<"oracle "<<dname_null <<std::endl;
			Format_output(devfunction_null,functiondev_temp, logmessage,'?');

			pos1=strcspn(devicename_temp,"?");
			pos2=strcspn(devicetype_temp,"?");
			pos3=strcspn(location_temp,"?");
			pos4=strcspn(serialnb_temp,"?");
			pos5=strcspn(functiondev_temp,"?");
			devicename_temp[pos1]='\0';
			devicetype_temp[pos2]='\0';
			serialnb_temp[pos4]='\0';
			location_temp[pos3]='\0';
			functiondev_temp[pos5]='\0';

			sysIDlist=(char*)malloc(200*sizeof(char));
			if(sysIDlist!=NULL)
				res1=DecomposeSysID_bis(sysID, sysIDlist);
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
			len=strlen(functiondev_temp)+strlen(sysIDlist)+strlen(devicetype_temp)+strlen(devicename_temp)+strlen(location_temp)+strlen(serialnb_temp)+strlen("|devicename (C): |devicetype (C): |active (I):1 |node (I):1 |deviceid  (I): |promiscuous_mode (I):1 |location (C): |nodeused (I):1 |serialnb  (C): |device_function (C): |system_name (C): |");
			sprintf(buffer,"%d",deviceid);
			len+=strlen(buffer);
			if(len_device<len)
			{
				len++;
				len_device=len;
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
					functiondev_temp= (char *) realloc(functiondev_temp,(0)*sizeof(char));

				free(sysIDlist);
				GetErrorMess(appliName,"BUFFER_TOO_SMALL",ErrorMessage,1);
				return rescode;
			}
			else
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
			devicetype_temp = (char *) realloc(devicetype_temp,(0)*sizeof(char));
		if(devicename_temp!=NULL)
			devicename_temp= (char *) realloc(devicename_temp,(0)*sizeof(char));
		if(serialnb_temp!=NULL)
			serialnb_temp = (char *) realloc(serialnb_temp,(0)*sizeof(char));
		if(location_temp!=NULL)
			location_temp= (char *) realloc(location_temp,(0)*sizeof(char));
		if(functiondev_temp!=NULL)
			functiondev_temp= (char *) realloc(functiondev_temp,(0)*sizeof(char));

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
				OCIReportError(ociError,"GetDeviceRow_devid ",ErrorMessage,1); 
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
		int i = 0;
		int j=0;
		int pos1=0;
		int pos2=0;
		int rescode=0;
		ub4 numcols = 0;
		ub4 collen=0;
		ub2 type = 0;
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
		int len=0;
		int free_mem=0;
		char buffer[20];
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
		int phy_null=0;
		int ip_null=0;
		int mac_null=0;
		int bia_null=0;
		int dname_null=0;
		int portype_null=0;
		int portnb_null=0;
		char logmessage[100];
		char selectport[1000];
		status =OCIHandleAlloc (ociEnv, (void**)&stmthp, OCI_HTYPE_STMT , 0, 0);
		if(status!=OCI_SUCCESS)
		{		
			rescode=ShowErrors (status, ociError, "OCIHandleAlloc unsuccessful");
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			else
				GetErrorMess(appliName,"NOT CONNECTED TO ANY DB",ErrorMessage,1);
			return -1;	
		}
		else
		{
			sprintf(selectport,"select e.portid,t.devicename||'?',e.port_nbr||'?',e.port_way,e.administrative_status,nvl(e.speed,0),nvl(e.port_type,'None')||'?',nvl(e.phy,'None')||'?',nvl(e.pxi_booting,-1),nvl(f.bia,'None')||'?',nvl(f.macaddress,'None')||'?',nvl(e.ipaddress,'None')||'?' from %s e,%s t,%s f where e.portid=:portID and t.deviceid=e.deviceid and f.serialnb=t.serialnb and f.port_nbr=e.port_nbr and nvl(f.port_type,'none')=nvl(e.port_type,'none') and f.port_way=e.port_way",PORT_TABLE,LOGICAL_DEVICE_TABLE,HWPORT_TABLE);
			status=OCIStmtPrepare(stmthp, ociError, (text*) selectport,(ub4) strlen(selectport),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bnd1p, ociError,(text*)":portID",-1,(dvoid*) &portID,sizeof(portID), SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else	
			status=OCIStmtExecute(ociHdbc, stmthp, ociError, 0, 0,(OCISnapshot *) 0, (OCISnapshot *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
		}
		else
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 2);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet unsuccessful");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &dnamelen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIAttrGet unsuccessful");
		}
		else
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 3);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet unsuccessful");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &portnblen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIAttrGet unsuccessful");
		}
		else
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 7);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet unsuccessful");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &portypelen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIAttrGet unsuccessful");
		}
		else
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 8);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet unsuccessful");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &phylen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIAttrGet unsuccessful");
		}
		else
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 10);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet unsuccessful");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &bialen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIAttrGet unsuccessful");
		}
		else
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 11);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet unsuccessful");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &maclen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIAttrGet unsuccessful");
		}
		else
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 12);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet unsuccessful");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &iplen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIAttrGet unsuccessful");
		}
		else
		{
			MinStringLength(portnblen);
			MinStringLength(phylen);
			MinStringLength(portypelen);
			MinStringLength(bialen);
			MinStringLength(maclen);
			MinStringLength(iplen);
			MinStringLength(dnamelen);

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
			else
				status=OCIDefineByPos (stmthp, &def[0], ociError, 1,&portid,sizeof(portid), SQLT_INT, 0, 0, 0, OCI_DEFAULT);
		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
			}
		}
		else
			status=OCIDefineByPos (stmthp, &def[1], ociError, 2,dname_temp,dnamelen+1, SQLT_STR, &dname_null, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
			}
		}
		else
			status=OCIDefineByPos (stmthp, &def[2], ociError, 3,portnb_temp,portnblen+1, SQLT_STR, &portnb_null, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
			}
		}
		else
			status=OCIDefineByPos (stmthp, &def[3], ociError, 4,&portway,sizeof(portway), SQLT_INT, 0, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
			}
		}
		else
			status=OCIDefineByPos (stmthp, &def[4], ociError, 5,&admin_status,sizeof(admin_status), SQLT_INT, 0, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
			}
		}
		else
			status=OCIDefineByPos (stmthp, &def[5], ociError, 6,&speed,sizeof(speed), SQLT_INT, 0, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
			}
		}
		else
			status=OCIDefineByPos (stmthp, &def[6], ociError, 7,portype_temp,portypelen+1, SQLT_STR, &portype_null, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
			}
		}
		else
			status=OCIDefineByPos (stmthp, &def[7], ociError, 8,phy_temp,phylen+1, SQLT_STR, &phy_null, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
			}
		}
		else
			status=OCIDefineByPos (stmthp, &def[8], ociError, 9,&pxi_booting,sizeof(pxi_booting), SQLT_INT, 0, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
			}
		}
		else
			status =OCIDefineByPos(stmthp, &def[9], ociError,10, (ub1 *) (bia_temp), bialen + 1,SQLT_STR, (dvoid *) &bia_null,(ub2 *) 0,0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
			}
		}
		else
			status=OCIDefineByPos (stmthp, &def[10], ociError, 11,mac_temp,maclen+1, SQLT_STR, (dvoid *) &mac_null,0,0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
			}
		}
		else
			status=OCIDefineByPos (stmthp, &def[11], ociError, 12,ip_temp,iplen+1, SQLT_STR,&ip_null, 0, 0, OCI_DEFAULT);


		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
			}
		}
		else
			status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT,1,OCI_DEFAULT);

		if (status==OCI_SUCCESS || status==OCI_SUCCESS_WITH_INFO) 
		{	
			Format_output(ip_null,ip_temp,  logmessage,'?');
			Format_output(phy_null,phy_temp, logmessage,'?');
			Format_output(mac_null,mac_temp,  logmessage,'?');
			Format_output(portnb_null,portnb_temp, logmessage,'?');
			Format_output(dname_null,dname_temp,  logmessage,'?');
			Format_output(bia_null,bia_temp, logmessage,'?');
			Format_output(portype_null,portype_temp,  logmessage,'?');

			pos1=strcspn(portype_temp,"?");
			pos2=strcspn(phy_temp,"?");
			portype_temp[pos1]='\0';
			phy_temp[pos2]='\0';
			pos1=strcspn(portnb_temp,"?");
			pos2=strcspn(bia_temp,"?");
			portnb_temp[pos1]='\0';
			bia_temp[pos2]='\0';
			pos1=strcspn(mac_temp,"?");
			pos2=strcspn(ip_temp,"?");
			mac_temp[pos1]='\0';
			ip_temp[pos2]='\0';
			pos1=strcspn(dname_temp,"?");
			dname_temp[pos1]='\0';


			len=strlen(mac_temp)+strlen(phy_temp)+strlen(ip_temp)+strlen(bia_temp)+strlen(dname_temp)+strlen(portype_temp)+strlen(portnb_temp)+strlen("|portid (I): |devicename (C): |port_nbr (C): |port_way (I):1 |administrative_status (I):1 |speed (I): |port_type (C): |phy (C): |pxi_booting (I):1 |bia (C): |macaddress (C): |ipaddress (C): |");

			sprintf(buffer,"%d",portid);
			len+=strlen(buffer);
			sprintf(buffer,"%d",speed);
			len+=strlen(buffer);


			if(len_port<len)
			{
				len++;
				len_port=len;
				rescode=-1;
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
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


				GetErrorMess(appliName,"BUFFER_TOO_SMALL",ErrorMessage,1);
				return rescode;
			}
			else
				sprintf(port_row_result,"|portid (I):%d |devicename (C):%s |port_nbr (C):%s |port_way (I):%d |administrative_status (I):%d |speed (I):%d |port_type (C):%s |phy (C):%s |pxi_booting (I):%d |bia (C):%s |macaddress (C):%s |ipaddress (C):%s |",portid,dname_temp,portnb_temp,portway,admin_status,speed,portype_temp,phy_temp,pxi_booting,bia_temp,mac_temp,ip_temp);
		} 
		else 
		{
			if(rescode==0)
			{
				sprintf(selectport,"select e.portid,t.devicename||'?',e.port_nbr||'?',e.port_way,e.administrative_status,nvl(e.speed,0),nvl(e.port_type,'None')||'?',nvl(e.phy,'None')||'?',nvl(e.pxi_booting,0),nvl(e.ipaddress,'None')||'?' from %s e,%s t where e.portid=:portID and t.deviceid=e.deviceid  ",PORT_TABLE,LOGICAL_DEVICE_TABLE);
				status=OCIStmtPrepare(stmthp, ociError, (text*) selectport,(ub4) strlen(selectport),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
				//std:://cout <<"logmessage bar "<<selectport << "oracle "<<selectport <<std::endl;
				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)
						rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
				}
				else
					status=OCIBindByName(stmthp, &bnd1p, ociError,(text*)":portID",-1,(dvoid*) &portID,sizeof(portID), SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)
						rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
				}
				else	
					status=OCIStmtExecute(ociHdbc, stmthp, ociError, 0, 0,(OCISnapshot *) 0, (OCISnapshot *) 0, OCI_DEFAULT);

				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)
						rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
				}
				else
					status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 2);

				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)
						rescode=ShowErrors (status, ociError, "OCIParamGet unsuccessful");
				}
				else
					status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &dnamelen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);

				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)
						rescode=ShowErrors (status, ociError, "OCIAttrGet unsuccessful");
				}
				else
					status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 3);

				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)
						rescode=ShowErrors (status, ociError, "OCIParamGet unsuccessful");
				}
				else
					status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &portnblen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);

				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)
						rescode=ShowErrors (status, ociError, "OCIAttrGet unsuccessful");
				}
				else
					status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 7);

				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)
						rescode=ShowErrors (status, ociError, "OCIParamGet unsuccessful");
				}
				else
					status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &portypelen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);

				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)
						rescode=ShowErrors (status, ociError, "OCIAttrGet unsuccessful");
				}
				else
					status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 8);

				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)
						rescode=ShowErrors (status, ociError, "OCIParamGet unsuccessful");
				}
				else
					status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &phylen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);



				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)
						rescode=ShowErrors (status, ociError, "OCIAttrGet unsuccessful");
				}
				else
					status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 10);

				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)
						rescode=ShowErrors (status, ociError, "OCIParamGet unsuccessful");
				}
				else
					status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &iplen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);

				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)
						rescode=ShowErrors (status, ociError, "OCIAttrGet unsuccessful");
				}
				else
				{
					MinStringLength(portnblen);
					MinStringLength(phylen);
					MinStringLength(portypelen);

					MinStringLength(iplen);
					MinStringLength(dnamelen);

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
					else
						status=OCIDefineByPos (stmthp, &def[0], ociError, 1,&portid,sizeof(portid), SQLT_INT, 0, 0, 0, OCI_DEFAULT);
				}

				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)
					{
						rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
					}
				}
				else
					status=OCIDefineByPos (stmthp, &def[1], ociError, 2,dname_temp,dnamelen+1, SQLT_STR, &dname_null, 0, 0, OCI_DEFAULT);

				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)
					{
						rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
					}
				}
				else
					status=OCIDefineByPos (stmthp, &def[2], ociError, 3,portnb_temp,portnblen+1, SQLT_STR, &portnb_null, 0, 0, OCI_DEFAULT);

				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)
					{
						rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
					}
				}
				else
					status=OCIDefineByPos (stmthp, &def[3], ociError, 4,&portway,sizeof(portway), SQLT_INT, 0, 0, 0, OCI_DEFAULT);

				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)
					{
						rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
					}
				}
				else
					status=OCIDefineByPos (stmthp, &def[4], ociError, 5,&admin_status,sizeof(admin_status), SQLT_INT, 0, 0, 0, OCI_DEFAULT);

				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)
					{
						rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
					}
				}
				else
					status=OCIDefineByPos (stmthp, &def[5], ociError, 6,&speed,sizeof(speed), SQLT_INT, 0, 0, 0, OCI_DEFAULT);

				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)
					{
						rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
					}
				}
				else
					status=OCIDefineByPos (stmthp, &def[6], ociError, 7,portype_temp,portypelen+1, SQLT_STR, &portype_null, 0, 0, OCI_DEFAULT);

				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)
					{
						rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
					}
				}
				else
					status=OCIDefineByPos (stmthp, &def[7], ociError, 8,phy_temp,phylen+1, SQLT_STR, &phy_null, 0, 0, OCI_DEFAULT);

				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)
					{
						rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
					}
				}
				else
					status=OCIDefineByPos (stmthp, &def[8], ociError, 9,&pxi_booting,sizeof(pxi_booting), SQLT_INT, 0, 0, 0, OCI_DEFAULT);


				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)
					{
						rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
					}
				}
				else
					status=OCIDefineByPos (stmthp, &def[9], ociError, 10,ip_temp,iplen+1, SQLT_STR,&ip_null, 0, 0, OCI_DEFAULT);



				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)
					{
						rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
					}
				}
				else
					status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT,1,OCI_DEFAULT);

				if (status==OCI_SUCCESS || status==OCI_SUCCESS_WITH_INFO) 
				{	
					Format_output(ip_null,ip_temp,  logmessage,'?');
					Format_output(phy_null,phy_temp, logmessage,'?');
					Format_output(portnb_null,portnb_temp, logmessage,'?');
					Format_output(dname_null,dname_temp,  logmessage,'?');
					Format_output(portype_null,portype_temp,  logmessage,'?');
					pos1=strcspn(portype_temp,"?");
					pos2=strcspn(phy_temp,"?");
					portype_temp[pos1]='\0';
					phy_temp[pos2]='\0';
					pos1=strcspn(portnb_temp,"?");
					portnb_temp[pos1]='\0';
					pos2=strcspn(ip_temp,"?");
					ip_temp[pos2]='\0';
					pos1=strcspn(dname_temp,"?");
					dname_temp[pos1]='\0';

					len=strlen(phy_temp)+strlen(ip_temp)+strlen(dname_temp)+strlen(portype_temp)+strlen(portnb_temp)+strlen("|portid (I): |devicename (C): |port_nbr (C): |port_way (I):1 |administrative_status (I):1 |speed (I): |port_type (C): |phy (C): |pxi_booting (I):1 |bia (C):none |macaddress (C):none |ipaddress (C): |");

					sprintf(buffer,"%d",portid);
					len+=strlen(buffer);
					sprintf(buffer,"%d",speed);
					len+=strlen(buffer);


					if(len_port<len)
					{
						len++;
						len_port=len;
						rescode=-1;
						status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
						if(phy_temp!=NULL)
							free(phy_temp);
						if(portnb_temp!=NULL)
							free(portnb_temp);

						if(ip_temp!=NULL)
							free(ip_temp);
						if(portype_temp!=NULL)
							free(portype_temp);
						if(dname_temp!=NULL)
							free(dname_temp);
						GetErrorMess(appliName,"BUFFER_TOO_SMALL",ErrorMessage,1);
						return rescode;
					}
					else
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
		int i = 0;
		int j=0;
		int pos1=0;
		int pos2=0;
		int rescode=0;
		ub4 numcols = 0;
		ub4 collen=0;
		ub2 type = 0;
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
		int len=0;
		int free_mem=0;
		char buffer[20];
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
		int phy_null=0;
		int ip_null=0;
		int mac_null=0;
		int bia_null=0;
		int dname_null=0;
		int portype_null=0;
		int portnb_null=0;
		char logmessage[100];
		char selectport[1000];
		char port_type1[100];
		status =OCIHandleAlloc (ociEnv, (void**)&stmthp, OCI_HTYPE_STMT , 0, 0);
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
		if(status!=OCI_SUCCESS)
		{		
			rescode=ShowErrors (status, ociError, "OCIHandleAlloc unsuccessful");
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			else
				GetErrorMess(appliName,"NOT CONNECTED TO ANY DB",ErrorMessage,1);
			return -1;	
		}
		else
		{
			sprintf(selectport,"select e.portid,t.devicename||'?',e.port_nbr||'?',e.port_way,e.administrative_status,nvl(e.speed,0),nvl(e.port_type,'none')||'?',nvl(e.phy,'none')||'?',nvl(e.pxi_booting,-1),nvl(f.bia,'none')||'?',nvl(f.macaddress,'none')||'?',nvl(e.ipaddress,'none')||'?' from %s e,%s t , %s f where t.devicename=:dname and e.port_nbr=:portnb and e.port_way=:portway and nvl(e.port_type,'none')=:portype and t.deviceid=e.deviceid and f.serialnb=t.serialnb and nvl(f.port_type,'none')=:portype and f.port_way=:portway and f.port_nbr=:portnb",PORT_TABLE,LOGICAL_DEVICE_TABLE,HWPORT_TABLE);
			status=OCIStmtPrepare(stmthp, ociError, (text*) selectport,(ub4) strlen(selectport),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bnd1p[0], ociError,(text*)":dname",-1,(dvoid*) devicename,strlen(devicename)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bnd1p[1], ociError,(text*)":portnb",-1,(dvoid*) port_nb,strlen(port_nb)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bnd1p[2], ociError,(text*)":portway",-1,(dvoid*) &port_way,sizeof(port_way), SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bnd1p[3], ociError,(text*)":portype",-1,(dvoid*) port_type1,strlen(port_type1)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else	
			status=OCIStmtExecute(ociHdbc, stmthp, ociError, 0, 0,(OCISnapshot *) 0, (OCISnapshot *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
		}
		else
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 2);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet unsuccessful");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &dnamelen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIAttrGet unsuccessful");
		}
		else
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 3);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet unsuccessful");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &portnblen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIAttrGet unsuccessful");
		}
		else
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 7);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet unsuccessful");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &portypelen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIAttrGet unsuccessful");
		}
		else
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 8);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet unsuccessful");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &phylen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIAttrGet unsuccessful");
		}
		else
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 10);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet unsuccessful");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &bialen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIAttrGet unsuccessful");
		}
		else
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 11);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet unsuccessful");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &maclen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIAttrGet unsuccessful");
		}
		else
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 12);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet unsuccessful");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &iplen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIAttrGet unsuccessful");
		}
		else
		{
			MinStringLength(portnblen);
			MinStringLength(phylen);
			MinStringLength(portypelen);
			MinStringLength(bialen);
			MinStringLength(maclen);
			MinStringLength(iplen);
			MinStringLength(dnamelen);

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
			else
				status=OCIDefineByPos (stmthp, &def[0], ociError, 1,&portid,sizeof(portid), SQLT_INT, 0, 0, 0, OCI_DEFAULT);
		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
			}
		}
		else
			status=OCIDefineByPos (stmthp, &def[1], ociError, 2,dname_temp,dnamelen+1, SQLT_STR, &dname_null, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
			}
		}
		else
			status=OCIDefineByPos (stmthp, &def[2], ociError, 3,portnb_temp,portnblen+1, SQLT_STR, &portnb_null, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
			}
		}
		else
			status=OCIDefineByPos (stmthp, &def[3], ociError, 4,&portway,sizeof(portway), SQLT_INT, 0, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
			}
		}
		else
			status=OCIDefineByPos (stmthp, &def[4], ociError, 5,&admin_status,sizeof(admin_status), SQLT_INT, 0, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
			}
		}
		else
			status=OCIDefineByPos (stmthp, &def[5], ociError, 6,&speed,sizeof(speed), SQLT_INT, 0, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
			}
		}
		else
			status=OCIDefineByPos (stmthp, &def[6], ociError, 7,portype_temp,portypelen+1, SQLT_STR, &portype_null, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
			}
		}
		else
			status=OCIDefineByPos (stmthp, &def[7], ociError, 8,phy_temp,phylen+1, SQLT_STR, &phy_null, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
			}
		}
		else
			status=OCIDefineByPos (stmthp, &def[8], ociError, 9,&pxi_booting,sizeof(pxi_booting), SQLT_INT, 0, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
			}
		}
		else
			status =OCIDefineByPos(stmthp, &def[9], ociError,10, (ub1 *) (bia_temp), bialen + 1,SQLT_STR, (dvoid *) &bia_null,(ub2 *) 0,0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
			}
		}
		else
			status=OCIDefineByPos (stmthp, &def[10], ociError, 11,mac_temp,maclen+1, SQLT_STR, (dvoid *) &mac_null,0,0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
			}
		}
		else
			status=OCIDefineByPos (stmthp, &def[11], ociError, 12,ip_temp,iplen+1, SQLT_STR,&ip_null, 0, 0, OCI_DEFAULT);


		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
			}
		}
		else
			status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT,1,OCI_DEFAULT);

		if (status==OCI_SUCCESS || status==OCI_SUCCESS_WITH_INFO) 
		{	
			Format_output(ip_null,ip_temp,  logmessage,'?');
			Format_output(phy_null,phy_temp, logmessage,'?');
			Format_output(mac_null,mac_temp,  logmessage,'?');
			Format_output(portnb_null,portnb_temp, logmessage,'?');
			Format_output(dname_null,dname_temp,  logmessage,'?');
			Format_output(bia_null,bia_temp, logmessage,'?');
			Format_output(portype_null,portype_temp,  logmessage,'?');

			pos1=strcspn(portype_temp,"?");
			pos2=strcspn(phy_temp,"?");
			portype_temp[pos1]='\0';
			phy_temp[pos2]='\0';
			pos1=strcspn(portnb_temp,"?");
			pos2=strcspn(bia_temp,"?");
			portnb_temp[pos1]='\0';
			bia_temp[pos2]='\0';
			pos1=strcspn(mac_temp,"?");
			pos2=strcspn(ip_temp,"?");
			mac_temp[pos1]='\0';
			ip_temp[pos2]='\0';
			pos1=strcspn(dname_temp,"?");
			dname_temp[pos1]='\0';


			len=strlen(mac_temp)+strlen(phy_temp)+strlen(ip_temp)+strlen(bia_temp)+strlen(dname_temp)+strlen(portype_temp)+strlen(portnb_temp)+strlen("|portid (I): |devicename (C): |port_nbr (C): |port_way (I):1 |administrative_status (I):1 |speed (I): |port_type (C): |phy (C): |pxi_booting (I):1 |bia (C): |macaddress (C): |ipaddress (C): |");

			sprintf(buffer,"%d",portid);
			len+=strlen(buffer);
			sprintf(buffer,"%d",speed);
			len+=strlen(buffer);


			if(len_port<len)
			{
				len++;
				len_port=len;
				rescode=-1;
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
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


				GetErrorMess(appliName,"BUFFER_TOO_SMALL",ErrorMessage,1);
				return rescode;
			}
			else
				sprintf(port_row_result,"|portid (I):%d |devicename (C):%s |port_nbr (C):%s |port_way (I):%d |administrative_status (I):%d |speed (I):%d |port_type (C):%s |phy (C):%s |pxi_booting (I):%d |bia (C):%s |macaddress (C):%s |ipaddress (C):%s |",portid,dname_temp,portnb_temp,portway,admin_status,speed,portype_temp,phy_temp,pxi_booting,bia_temp,mac_temp,ip_temp);
		} 
		else 
		{
			if(rescode==0)
			{
				sprintf(selectport,"select e.portid,t.devicename||'?',e.port_nbr||'?',e.port_way,e.administrative_status,nvl(e.speed,0),nvl(e.port_type,'none')||'?',nvl(e.phy,'none')||'?',nvl(e.pxi_booting,-1),nvl(e.ipaddress,'none')||'?' from %s e,%s t where t.devicename=:dname and e.port_nbr=:portnb and e.port_way=:portway and nvl(e.port_type,'none')=:portype and t.deviceid=e.deviceid",PORT_TABLE,LOGICAL_DEVICE_TABLE);
				status=OCIStmtPrepare(stmthp, ociError, (text*) selectport,(ub4) strlen(selectport),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
			}

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)
					rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
			}
			else
				status=OCIBindByName(stmthp, &bnd1p[0], ociError,(text*)":dname",-1,(dvoid*) devicename,strlen(devicename)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)
					rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
			}
			else
				status=OCIBindByName(stmthp, &bnd1p[1], ociError,(text*)":portnb",-1,(dvoid*) port_nb,strlen(port_nb)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)
					rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
			}
			else
				status=OCIBindByName(stmthp, &bnd1p[2], ociError,(text*)":portway",-1,(dvoid*) &port_way,sizeof(port_way), SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)
					rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
			}
			else
				status=OCIBindByName(stmthp, &bnd1p[3], ociError,(text*)":portype",-1,(dvoid*) port_type1,strlen(port_type1)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else	
				status=OCIStmtExecute(ociHdbc, stmthp, ociError, 0, 0,(OCISnapshot *) 0, (OCISnapshot *) 0, OCI_DEFAULT);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)
					rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
			}
			else
				status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 2);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)
					rescode=ShowErrors (status, ociError, "OCIParamGet unsuccessful");
			}
			else
				status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &dnamelen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)
					rescode=ShowErrors (status, ociError, "OCIAttrGet unsuccessful");
			}
			else
				status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 3);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)
					rescode=ShowErrors (status, ociError, "OCIParamGet unsuccessful");
			}
			else
				status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &portnblen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)
					rescode=ShowErrors (status, ociError, "OCIAttrGet unsuccessful");
			}
			else
				status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 7);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)
					rescode=ShowErrors (status, ociError, "OCIParamGet unsuccessful");
			}
			else
				status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &portypelen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)
					rescode=ShowErrors (status, ociError, "OCIAttrGet unsuccessful");
			}
			else
				status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 8);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)
					rescode=ShowErrors (status, ociError, "OCIParamGet unsuccessful");
			}
			else
				status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &phylen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);



			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)
					rescode=ShowErrors (status, ociError, "OCIAttrGet unsuccessful");
			}
			else
				status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 10);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)
					rescode=ShowErrors (status, ociError, "OCIParamGet unsuccessful");
			}
			else
				status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &iplen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)
					rescode=ShowErrors (status, ociError, "OCIAttrGet unsuccessful");
			}
			else
			{
				MinStringLength(portnblen);
				MinStringLength(phylen);
				MinStringLength(portypelen);

				MinStringLength(iplen);
				MinStringLength(dnamelen);

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
				else
					status=OCIDefineByPos (stmthp, &def[0], ociError, 1,&portid,sizeof(portid), SQLT_INT, 0, 0, 0, OCI_DEFAULT);
			}

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)
				{
					rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
				}
			}
			else
				status=OCIDefineByPos (stmthp, &def[1], ociError, 2,dname_temp,dnamelen+1, SQLT_STR, &dname_null, 0, 0, OCI_DEFAULT);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)
				{
					rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
				}
			}
			else
				status=OCIDefineByPos (stmthp, &def[2], ociError, 3,portnb_temp,portnblen+1, SQLT_STR, &portnb_null, 0, 0, OCI_DEFAULT);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)
				{
					rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
				}
			}
			else
				status=OCIDefineByPos (stmthp, &def[3], ociError, 4,&portway,sizeof(portway), SQLT_INT, 0, 0, 0, OCI_DEFAULT);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)
				{
					rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
				}
			}
			else
				status=OCIDefineByPos (stmthp, &def[4], ociError, 5,&admin_status,sizeof(admin_status), SQLT_INT, 0, 0, 0, OCI_DEFAULT);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)
				{
					rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
				}
			}
			else
				status=OCIDefineByPos (stmthp, &def[5], ociError, 6,&speed,sizeof(speed), SQLT_INT, 0, 0, 0, OCI_DEFAULT);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)
				{
					rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
				}
			}
			else
				status=OCIDefineByPos (stmthp, &def[6], ociError, 7,portype_temp,portypelen+1, SQLT_STR, &portype_null, 0, 0, OCI_DEFAULT);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)
				{
					rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
				}
			}
			else
				status=OCIDefineByPos (stmthp, &def[7], ociError, 8,phy_temp,phylen+1, SQLT_STR, &phy_null, 0, 0, OCI_DEFAULT);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)
				{
					rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
				}
			}
			else
				status=OCIDefineByPos (stmthp, &def[8], ociError, 9,&pxi_booting,sizeof(pxi_booting), SQLT_INT, 0, 0, 0, OCI_DEFAULT);



			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)
				{
					rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
				}
			}
			else
				status=OCIDefineByPos (stmthp, &def[9], ociError, 10,ip_temp,iplen+1, SQLT_STR,&ip_null, 0, 0, OCI_DEFAULT);


			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)
				{
					rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
				}
			}
			else
				status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT,1,OCI_DEFAULT);

			if (status==OCI_SUCCESS || status==OCI_SUCCESS_WITH_INFO) 
			{	
				Format_output(ip_null,ip_temp,  logmessage,'?');
				Format_output(phy_null,phy_temp, logmessage,'?');
				Format_output(portnb_null,portnb_temp, logmessage,'?');
				Format_output(dname_null,dname_temp,  logmessage,'?');
				Format_output(portype_null,portype_temp,  logmessage,'?');

				pos1=strcspn(portype_temp,"?");
				pos2=strcspn(phy_temp,"?");
				portype_temp[pos1]='\0';
				phy_temp[pos2]='\0';
				pos1=strcspn(portnb_temp,"?");
				pos2=strcspn(ip_temp,"?");
				portnb_temp[pos1]='\0';
				ip_temp[pos2]='\0';
				pos1=strcspn(dname_temp,"?");
				dname_temp[pos1]='\0';


				len=strlen(phy_temp)+strlen(ip_temp)+strlen(dname_temp)+strlen(portype_temp)+strlen(portnb_temp)+strlen("|portid (I): |devicename (C): |port_nbr (C): |port_way (I):1 |administrative_status (I):1 |speed (I): |port_type (C): |phy (C): |pxi_booting (I):1 |bia (C):none |macaddress (C):none |ipaddress (C): |");

				sprintf(buffer,"%d",portid);
				len+=strlen(buffer);
				sprintf(buffer,"%d",speed);
				len+=strlen(buffer);


				if(len_port<len)
				{
					len++;
					len_port=len;
					rescode=-1;
					status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
					if(phy_temp!=NULL)
						free(phy_temp);

					if(portnb_temp!=NULL)
						free(portnb_temp);

					if(ip_temp!=NULL)
						free(ip_temp);
					if(portype_temp!=NULL)
						free(portype_temp);
					if(dname_temp!=NULL)
						free(dname_temp);


					GetErrorMess(appliName,"BUFFER_TOO_SMALL",ErrorMessage,1);
					return rescode;
				}
				else
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
		int i = 0;
		int j=0;
		int pos1=0;
		int pos2=0;
		int rescode=0;
		ub4 numcols = 0;
		ub4 collen=0;
		ub2 type = 0;
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
		int len=0;
		int free_mem=0;
		char buffer[20];
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
		int phy_null=0;
		int ip_null=0;
		int mac_null=0;
		int bia_null=0;
		int dname_null=0;
		int portype_null=0;
		int portnb_null=0;
		char logmessage[100];
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
		status =OCIHandleAlloc (ociEnv, (void**)&stmthp, OCI_HTYPE_STMT , 0, 0);

		if(status!=OCI_SUCCESS)
		{		
			rescode=ShowErrors (status, ociError, "OCIHandleAlloc unsuccessful");
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			else
				GetErrorMess(appliName,"NOT CONNECTED TO ANY DB",ErrorMessage,1);
			return -1;	
		}
		else
		{
			sprintf(selectport,"select e.portid,t.devicename||'?',e.port_nbr||'?',e.port_way,e.administrative_status,nvl(e.speed,0),nvl(e.port_type,'none')||'?',nvl(e.phy,'none')||'?',nvl(e.pxi_booting,-1),nvl(f.bia,'none')||'?',nvl(f.macaddress,'none')||'?',nvl(e.ipaddress,'none')||'?' from %s e,%s t,%s f where t.deviceid=:dID and e.port_nbr=:portnb and e.port_way=:portway and nvl(e.port_type,'none')=:portype and t.deviceid=e.deviceid and f.serialnb=t.serialnb and f.port_way=:portway and nvl(f.port_type,'none')=:portype and f.port_nbr=:portnb",PORT_TABLE,LOGICAL_DEVICE_TABLE,HWPORT_TABLE);
			status=OCIStmtPrepare(stmthp, ociError, (text*) selectport,(ub4) strlen(selectport),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bnd1p[0], ociError,(text*)":dID",-1,(dvoid*)&deviceid,sizeof(deviceid), SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bnd1p[1], ociError,(text*)":portnb",-1,(dvoid*) port_nb,strlen(port_nb)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bnd1p[2], ociError,(text*)":portway",-1,(dvoid*) &port_way,sizeof(port_way), SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bnd1p[3], ociError,(text*)":portype",-1,(dvoid*) port_type1,strlen(port_type1)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else	
			status=OCIStmtExecute(ociHdbc, stmthp, ociError, 0, 0,(OCISnapshot *) 0, (OCISnapshot *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
		}
		else
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 2);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet unsuccessful");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &dnamelen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIAttrGet unsuccessful");
		}
		else
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 3);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet unsuccessful");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &portnblen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIAttrGet unsuccessful");
		}
		else
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 7);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet unsuccessful");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &portypelen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIAttrGet unsuccessful");
		}
		else
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 8);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet unsuccessful");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &phylen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIAttrGet unsuccessful");
		}
		else
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 10);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet unsuccessful");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &bialen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIAttrGet unsuccessful");
		}
		else
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 11);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet unsuccessful");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &maclen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIAttrGet unsuccessful");
		}
		else
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 12);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet unsuccessful");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &iplen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIAttrGet unsuccessful");
		}
		else
		{
			MinStringLength(portnblen);
			MinStringLength(phylen);
			MinStringLength(portypelen);
			MinStringLength(bialen);
			MinStringLength(maclen);
			MinStringLength(iplen);
			MinStringLength(dnamelen);

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
			else
				status=OCIDefineByPos (stmthp, &def[0], ociError, 1,&portid,sizeof(portid), SQLT_INT, 0, 0, 0, OCI_DEFAULT);
		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
			}
		}
		else
			status=OCIDefineByPos (stmthp, &def[1], ociError, 2,dname_temp,dnamelen+1, SQLT_STR, &dname_null, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
			}
		}
		else
			status=OCIDefineByPos (stmthp, &def[2], ociError, 3,portnb_temp,portnblen+1, SQLT_STR, &portnb_null, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
			}
		}
		else
			status=OCIDefineByPos (stmthp, &def[3], ociError, 4,&portway,sizeof(portway), SQLT_INT, 0, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
			}
		}
		else
			status=OCIDefineByPos (stmthp, &def[4], ociError, 5,&admin_status,sizeof(admin_status), SQLT_INT, 0, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
			}
		}
		else
			status=OCIDefineByPos (stmthp, &def[5], ociError, 6,&speed,sizeof(speed), SQLT_INT, 0, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
			}
		}
		else
			status=OCIDefineByPos (stmthp, &def[6], ociError, 7,portype_temp,portypelen+1, SQLT_STR, &portype_null, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
			}
		}
		else
			status=OCIDefineByPos (stmthp, &def[7], ociError, 8,phy_temp,phylen+1, SQLT_STR, &phy_null, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
			}
		}
		else
			status=OCIDefineByPos (stmthp, &def[8], ociError, 9,&pxi_booting,sizeof(pxi_booting), SQLT_INT, 0, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
			}
		}
		else
			status =OCIDefineByPos(stmthp, &def[9], ociError,10, (ub1 *) (bia_temp), bialen + 1,SQLT_STR, (dvoid *) &bia_null,(ub2 *) 0,0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
			}
		}
		else
			status=OCIDefineByPos (stmthp, &def[10], ociError, 11,mac_temp,maclen+1, SQLT_STR, (dvoid *) &mac_null,0,0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
			}
		}
		else
			status=OCIDefineByPos (stmthp, &def[11], ociError, 12,ip_temp,iplen+1, SQLT_STR,&ip_null, 0, 0, OCI_DEFAULT);


		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
			}
		}
		else
			status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT,1,OCI_DEFAULT);

		if (status==OCI_SUCCESS || status==OCI_SUCCESS_WITH_INFO) 
		{	
			Format_output(ip_null,ip_temp,  logmessage,'?');
			Format_output(phy_null,phy_temp, logmessage,'?');
			Format_output(mac_null,mac_temp,  logmessage,'?');
			Format_output(portnb_null,portnb_temp, logmessage,'?');
			Format_output(dname_null,dname_temp,  logmessage,'?');
			Format_output(bia_null,bia_temp, logmessage,'?');
			Format_output(portype_null,portype_temp,  logmessage,'?');

			pos1=strcspn(portype_temp,"?");
			pos2=strcspn(phy_temp,"?");
			portype_temp[pos1]='\0';
			phy_temp[pos2]='\0';
			pos1=strcspn(portnb_temp,"?");
			pos2=strcspn(bia_temp,"?");
			portnb_temp[pos1]='\0';
			bia_temp[pos2]='\0';
			pos1=strcspn(mac_temp,"?");
			pos2=strcspn(ip_temp,"?");
			mac_temp[pos1]='\0';
			ip_temp[pos2]='\0';
			pos1=strcspn(dname_temp,"?");
			dname_temp[pos1]='\0';


			len=strlen(mac_temp)+strlen(phy_temp)+strlen(ip_temp)+strlen(bia_temp)+strlen(dname_temp)+strlen(portype_temp)+strlen(portnb_temp)+strlen("|portid (I): |devicename (C): |port_nbr (C): |port_way (I):1 |administrative_status (I):1 |speed (I): |port_type (C): |phy (C): |pxi_booting (I):1 |bia (C): |macaddress (C): |ipaddress (C): |");

			sprintf(buffer,"%d",portid);
			len+=strlen(buffer);
			sprintf(buffer,"%d",speed);
			len+=strlen(buffer);


			if(len_port<len)
			{
				len++;
				len_port=len;
				rescode=-1;
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
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


				GetErrorMess(appliName,"BUFFER_TOO_SMALL",ErrorMessage,1);
				return rescode;
			}
			else
				sprintf(port_row_result,"|portid (I):%d |devicename (C):%s |port_nbr (C):%s |port_way (I):%d |administrative_status (I):%d |speed (I):%d |port_type (C):%s |phy (C):%s |pxi_booting (I):%d |bia (C):%s |macaddress (C):%s |ipaddress (C):%s |",portid,dname_temp,portnb_temp,portway,admin_status,speed,portype_temp,phy_temp,pxi_booting,bia_temp,mac_temp,ip_temp);
		} 
		else 
		{
			status=0;
			if(rescode==0)
			{
				sprintf(selectport,"select e.portid,t.devicename||'?',e.port_nbr||'?',e.port_way,e.administrative_status,nvl(e.speed,0),nvl(e.port_type,'none')||'?',nvl(e.phy,'none')||'?',nvl(e.pxi_booting,-1),nvl(e.ipaddress,'none')||'?' from %s e,%s t where t.deviceid=:dID and e.port_nbr=:portnb and e.port_way=:portway and nvl(e.port_type,'none')=:portype and t.deviceid=e.deviceid  ",PORT_TABLE,LOGICAL_DEVICE_TABLE);
				status=OCIStmtPrepare(stmthp, ociError, (text*) selectport,(ub4) strlen(selectport),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);

			}

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)
					rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
			}
			else
				status=OCIBindByName(stmthp, &bnd1p[0], ociError,(text*)":dID",-1,(dvoid*)&deviceid,sizeof(deviceid), SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)
					rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
			}
			else
				status=OCIBindByName(stmthp, &bnd1p[1], ociError,(text*)":portnb",-1,(dvoid*) port_nb,strlen(port_nb)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)
					rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
			}
			else
				status=OCIBindByName(stmthp, &bnd1p[2], ociError,(text*)":portway",-1,(dvoid*) &port_way,sizeof(port_way), SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)
					rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
			}
			else
				status=OCIBindByName(stmthp, &bnd1p[3], ociError,(text*)":portype",-1,(dvoid*) port_type1,strlen(port_type1)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else	
				status=OCIStmtExecute(ociHdbc, stmthp, ociError, 0, 0,(OCISnapshot *) 0, (OCISnapshot *) 0, OCI_DEFAULT);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)
					rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
			}
			else
				status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 2);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)
					rescode=ShowErrors (status, ociError, "OCIParamGet unsuccessful");
			}
			else
				status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &dnamelen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)
					rescode=ShowErrors (status, ociError, "OCIAttrGet unsuccessful");
			}
			else
				status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 3);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)
					rescode=ShowErrors (status, ociError, "OCIParamGet unsuccessful");
			}
			else
				status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &portnblen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)
					rescode=ShowErrors (status, ociError, "OCIAttrGet unsuccessful");
			}
			else
				status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 7);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)
					rescode=ShowErrors (status, ociError, "OCIParamGet unsuccessful");
			}
			else
				status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &portypelen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)
					rescode=ShowErrors (status, ociError, "OCIAttrGet unsuccessful");
			}
			else
				status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 8);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)
					rescode=ShowErrors (status, ociError, "OCIParamGet unsuccessful");
			}
			else
				status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &phylen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);



			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)
					rescode=ShowErrors (status, ociError, "OCIAttrGet unsuccessful");
			}
			else
				status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 10);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)
					rescode=ShowErrors (status, ociError, "OCIParamGet unsuccessful");
			}
			else
				status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &iplen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)
					rescode=ShowErrors (status, ociError, "OCIAttrGet unsuccessful");
			}
			else
			{
				MinStringLength(portnblen);
				MinStringLength(phylen);
				MinStringLength(portypelen);

				MinStringLength(iplen);
				MinStringLength(dnamelen);

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
				else
					status=OCIDefineByPos (stmthp, &def[0], ociError, 1,&portid,sizeof(portid), SQLT_INT, 0, 0, 0, OCI_DEFAULT);
			}

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)
				{
					rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
				}
			}
			else
				status=OCIDefineByPos (stmthp, &def[1], ociError, 2,dname_temp,dnamelen+1, SQLT_STR, &dname_null, 0, 0, OCI_DEFAULT);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)
				{
					rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
				}
			}
			else
				status=OCIDefineByPos (stmthp, &def[2], ociError, 3,portnb_temp,portnblen+1, SQLT_STR, &portnb_null, 0, 0, OCI_DEFAULT);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)
				{
					rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
				}
			}
			else
				status=OCIDefineByPos (stmthp, &def[3], ociError, 4,&portway,sizeof(portway), SQLT_INT, 0, 0, 0, OCI_DEFAULT);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)
				{
					rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
				}
			}
			else
				status=OCIDefineByPos (stmthp, &def[4], ociError, 5,&admin_status,sizeof(admin_status), SQLT_INT, 0, 0, 0, OCI_DEFAULT);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)
				{
					rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
				}
			}
			else
				status=OCIDefineByPos (stmthp, &def[5], ociError, 6,&speed,sizeof(speed), SQLT_INT, 0, 0, 0, OCI_DEFAULT);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)
				{
					rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
				}
			}
			else
				status=OCIDefineByPos (stmthp, &def[6], ociError, 7,portype_temp,portypelen+1, SQLT_STR, &portype_null, 0, 0, OCI_DEFAULT);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)
				{
					rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
				}
			}
			else
				status=OCIDefineByPos (stmthp, &def[7], ociError, 8,phy_temp,phylen+1, SQLT_STR, &phy_null, 0, 0, OCI_DEFAULT);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)
				{
					rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
				}
			}
			else
				status=OCIDefineByPos (stmthp, &def[8], ociError, 9,&pxi_booting,sizeof(pxi_booting), SQLT_INT, 0, 0, 0, OCI_DEFAULT);


			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)
				{
					rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
				}
			}
			else
				status=OCIDefineByPos (stmthp, &def[9], ociError, 10,ip_temp,iplen+1, SQLT_STR,&ip_null, 0, 0, OCI_DEFAULT);


			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)
				{
					rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
				}	
			}
			else
				status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT,1,OCI_DEFAULT);

			if (status==OCI_SUCCESS || status==OCI_SUCCESS_WITH_INFO) 
			{	
				Format_output(ip_null,ip_temp,  logmessage,'?');
				Format_output(phy_null,phy_temp, logmessage,'?');
				Format_output(portnb_null,portnb_temp, logmessage,'?');
				Format_output(dname_null,dname_temp,  logmessage,'?');

				Format_output(portype_null,portype_temp,  logmessage,'?');

				pos1=strcspn(portype_temp,"?");
				pos2=strcspn(phy_temp,"?");
				portype_temp[pos1]='\0';
				phy_temp[pos2]='\0';
				pos1=strcspn(portnb_temp,"?");
				pos2=strcspn(ip_temp,"?");
				ip_temp[pos2]='\0';
				portnb_temp[pos1]='\0';
				pos1=strcspn(dname_temp,"?");
				dname_temp[pos1]='\0';


				len=strlen(phy_temp)+strlen(ip_temp)+strlen(dname_temp)+strlen(portype_temp)+strlen(portnb_temp)+strlen("|portid (I): |devicename (C): |port_nbr (C): |port_way (I):1 |administrative_status (I):1 |speed (I): |port_type (C): |phy (C): |pxi_booting (I):1 |bia (C):none |macaddress (C):none |ipaddress (C): |");

				sprintf(buffer,"%d",portid);
				len+=strlen(buffer);
				sprintf(buffer,"%d",speed);
				len+=strlen(buffer);


				if(len_port<len)
				{
					len++;
					len_port=len;
					rescode=-1;
					status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
					if(phy_temp!=NULL)
						free(phy_temp);

					if(portnb_temp!=NULL)
						free(portnb_temp);

					if(ip_temp!=NULL)
						free(ip_temp);
					if(portype_temp!=NULL)
						free(portype_temp);
					if(dname_temp!=NULL)
						free(dname_temp);
					GetErrorMess(appliName,"BUFFER_TOO_SMALL",ErrorMessage,1);
					return rescode;
				}
				else
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
		int i = 0;
		int len=0;
		int j=0;
		int pos1=0;
		int pos2=0;
		sword status;
		int rescode=0;
		OCIStmt *stmthp;
		OCIBind  *bnd1p = (OCIBind *) 0; 
		OCIDefine* def[11];
		OCIParam *parmdp;
		char buffer[20];
		char logmessage[100];

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
		int dnamefrom_null=0;
		int dnameto_null=0;
		int portnbfrom_null=0;
		int portnbto_null=0;
		int lkname_null=0;
		int linkid=0;
		char * porttypefrom_temp=NULL;
		char * porttypeto_temp=NULL;
		int porttypefrom_null=0;
		int porttypeto_null=0;
		int lkinfo_null=0;
		int lkinfolen=0;
		int bidirectional_link_used=0;
		int lkused=0;
		char selectconn[1000];
		char * lkinfo_temp=NULL;
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
			sprintf(selectconn,"select t.linkid,e.devicename||'?',f.devicename||'?',p.port_nbr||'?',nvl(p.port_type,'none')||'?',g.port_nbr||'?',nvl(g.port_type,'none')||'?',nvl(l.link_name,'none')||'?',t.bidirectional_link_used, t.lkused,nvl(t.link_info,'none')||'?' from %s t,%s p,%s g,%s l,%s e,%s f where t.linkid=:lkid and t.link_type=l.linktypeID and p.portid=t.portidfrom and g.portid=t.portidto and p.deviceid=e.deviceid and g.deviceid=f.deviceid",MACRO_CONNECTIVITY_TABLE,PORT_TABLE,PORT_TABLE,LINKTYPE_TABLE,LOGICAL_DEVICE_TABLE,LOGICAL_DEVICE_TABLE);
			status=OCIStmtPrepare(stmthp, ociError, (text*) selectconn,(ub4) strlen(selectconn),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
			//std:://cout<<"value of selectconn "<<selectconn<<std::endl;
		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bnd1p, ociError,(text*)":lkid",-1,(dvoid*) &lkID,sizeof(lkID), SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else
			status=OCIStmtExecute(ociHdbc, stmthp, ociError, 0, 0,(OCISnapshot *) 0, (OCISnapshot *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
		}
		else
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 2);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet unsuccessful");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &dnamefromlen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);


		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
		}
		else
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 3);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet unsuccessful");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &dnametolen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);


		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
		}
		else
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 4);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet unsuccessful");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &portnbfromlen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
		}
		else
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 5);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet unsuccessful");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &porttypefromlen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
		}
		else
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 6);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet unsuccessful");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &portnbtolen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
		}
		else
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 7);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet unsuccessful");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &porttypetolen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
		}
		else
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 8);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet unsuccessful");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &lknamelen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);


		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
		}
		else
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 11);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet unsuccessful");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &lkinfolen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);




		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIAttrGet unsuccessful");
		}
		else
		{
			MinStringLength(lknamelen);
			MinStringLength(dnamefromlen);
			MinStringLength(dnametolen);
			MinStringLength(portnbfromlen);
			MinStringLength(portnbtolen);
			MinStringLength(porttypefromlen);
			MinStringLength(porttypetolen);
			MinStringLength(lkinfolen);

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
			else
				status=OCIDefineByPos (stmthp, &def[0], ociError, 1,&linkid,sizeof(linkid), SQLT_INT, 0, 0, 0, OCI_DEFAULT);
		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
		}
		else
			status=OCIDefineByPos (stmthp, &def[1], ociError,2,dnamefrom_temp,dnamefromlen+1, SQLT_STR, &dnamefrom_null, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
		}
		else
			status=OCIDefineByPos (stmthp, &def[2], ociError,3,dnameto_temp,dnametolen+1, SQLT_STR, &dnameto_null, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
		}
		else
			status=OCIDefineByPos (stmthp, &def[3], ociError, 4,portnbfrom_temp,portnbfromlen+1, SQLT_STR, &portnbfrom_null, 0, 0,OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
		}
		else
			status=OCIDefineByPos (stmthp, &def[4], ociError, 5,porttypefrom_temp,porttypefromlen+1, SQLT_STR, &porttypefrom_null, 0, 0,OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
		}
		else
			status=OCIDefineByPos (stmthp, &def[5], ociError, 6,portnbto_temp,portnbtolen+1, SQLT_STR, &portnbto_null, 0, 0, OCI_DEFAULT);
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
		}
		else
			status=OCIDefineByPos (stmthp, &def[6], ociError, 7,porttypeto_temp,porttypetolen+1, SQLT_STR, &porttypeto_null, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
		}
		else
			status=OCIDefineByPos (stmthp, &def[7], ociError, 8,lkname_temp,lknamelen, SQLT_STR, &lkname_null, 0, 0, OCI_DEFAULT);


		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
		}
		else
			status=OCIDefineByPos (stmthp, &def[8], ociError,9,&bidirectional_link_used,sizeof(bidirectional_link_used), SQLT_INT, 0, 0, 0, OCI_DEFAULT);


		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
		}
		else
			status=OCIDefineByPos (stmthp, &def[9], ociError,10,&lkused,sizeof(lkused), SQLT_INT, 0, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
		}
		else
			status=OCIDefineByPos (stmthp, &def[10], ociError,11,lkinfo_temp,lkinfolen+1, SQLT_STR, 0, 0, 0, OCI_DEFAULT);


		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
		}
		else
			status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT,1, OCI_DEFAULT);
		if (status==OCI_SUCCESS || status==OCI_SUCCESS_WITH_INFO) 
		{	

			Format_output(dnamefrom_null,dnamefrom_temp,  logmessage,'?');
			Format_output(dnameto_null,dnameto_temp, logmessage,'?');
			Format_output(portnbfrom_null,portnbfrom_temp,  logmessage,'?');
			Format_output(portnbto_null,portnbto_temp, logmessage,'?');
			Format_output(lkname_null,lkname_temp,  logmessage,'?');
			Format_output(lkinfo_null,lkinfo_temp,  logmessage,'?');
			Format_output(porttypefrom_null,porttypefrom_temp,  logmessage,'?');
			Format_output(porttypeto_null,porttypeto_temp, logmessage,'?');
			pos1=strcspn(dnamefrom_temp,"?");
			pos2=strcspn(dnameto_temp,"?");
			dnamefrom_temp[pos1]='\0';
			dnameto_temp[pos2]='\0';
			//std:://cout<<"value of lkname_temp "<<portnbfrom_temp<<" pp " <<portnbto_temp<<std::endl;
			pos1=strcspn(portnbfrom_temp,"?");
			pos2=strcspn(portnbto_temp,"?");
			portnbfrom_temp[pos1]='\0';
			portnbto_temp[pos2]='\0';
			pos1=strcspn(porttypefrom_temp,"?");
			pos2=strcspn(porttypeto_temp,"?");
			porttypefrom_temp[pos1]='\0';
			porttypeto_temp[pos2]='\0';
			pos1=strcspn(lkname_temp,"?");
			pos2=strcspn(lkinfo_temp,"?");
			lkinfo_temp[pos2]='\0';
			lkname_temp[pos1]='\0';
			//std:://cout<<"value of lkname_temp "<<lkname_temp<<std::endl;
			//std:://cout<<"value of linkid "<<linkid<<std::endl;
			len=strlen(lkinfo_temp)+strlen(lkname_temp)+strlen(dnamefrom_temp)+strlen(dnameto_temp)+strlen(portnbfrom_temp)+strlen(porttypeto_temp)+strlen(porttypefrom_temp)+strlen(portnbto_temp)+strlen("|linkid (I): |node_from (C): |node_to (C): |port_nbrfrom (C): |port_typefrom (C) : |port_nbrto (C): |port_typeto (C) : |link_type (C): |bidirectional_link_used (I):1 |lkused (I):1 |link_info (C): |");
			sprintf(buffer,"%d",linkid);
			//std:://cout<<"value of buffer "<<buffer<<std::endl;
			len+=strlen(buffer);
			//std:://cout<<"value of lkname_temp "<<lkname_temp<<std::endl;
			//std:://cout<<"value of len "<<len<<std::endl;
			if(len>len_conn)
			{
				len++;
				len_conn=len;
				rescode=-1;
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
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);


				GetErrorMess(appliName,"BUFFER_TOO_SMALL",ErrorMessage,1);
				return rescode;

			}
			else
			{
				//std:://cout<<"before sprintf connrow "<<std::endl;
				sprintf(Conn_row,"|linkid (I):%d |node_from (C):%s |node_to (C):%s |port_nbrfrom (C):%s |port_typefrom (C): %s|port_nbrto (C):%s |port_typeto (C):%s |link_type (C):%s |bidirectional_link_used (I):%d |lkused (I):%d |link_info (C):%s |",linkid,dnamefrom_temp,dnameto_temp,portnbfrom_temp,porttypefrom_temp,portnbto_temp,porttypeto_temp,lkname_temp,bidirectional_link_used, lkused,lkinfo_temp);

				//std:://cout<<"after sprintf connrow "<<Conn_row<<std::endl;
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
		int i = 0;
		int len=0;
		int j=0;
		int pos1=0;
		int pos2=0;
		sword status;
		int rescode=0;
		char logmessage[100];
		sb4  porttypefromlen=0;
		sb4 porttypetolen=0;
		OCIStmt *stmthp;
		OCIBind  *bnd1p[3] ; 
		OCIDefine* def[11];
		OCIParam *parmdp;
		char buffer[20];
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
		int dnamefrom_null=0;
		int dnameto_null=0;
		int portnbfrom_null=0;
		int portnbto_null=0;
		int lkname_null=0;
		int lkinfo_null=0;
		int linkid=0;
		char port_type1[100];
		char * porttypefrom_temp=NULL;
		char * porttypeto_temp=NULL;
		int porttypefrom_null=0;
		int porttypeto_null=0;
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
			if(port_way==1) //port nb is an output so we have info about the node_to part of a link
				sprintf(selectconn,"select t.linkid,e.devicename||'?',f.devicename||'?',p.port_nbr||'?',nvl(p.port_type,'none')||'?',g.port_nbr||'?',nvl(g.port_type,'none')||'?',l.link_name||'?',t.bidirectional_link_used, t.lkused,nvl(t.link_info,'none')||'?' from %s t,%s p,%s g,%s l,%s e,%s f where nvl(g.port_type,'none')=:portype and g.port_nbr=:portnb and g.port_way=1  and t.link_type=l.linktypeID and p.portid=t.portidfrom and g.portid=t.portidto and g.deviceid=f.deviceid and p.deviceid=e.deviceid and f.deviceid=:dID",MACRO_CONNECTIVITY_TABLE,PORT_TABLE,PORT_TABLE,LINKTYPE_TABLE,LOGICAL_DEVICE_TABLE,LOGICAL_DEVICE_TABLE);
			else // info about the node_from part of a link
				sprintf(selectconn,"select t.linkid,e.devicename||'?',f.devicename||'?',p.port_nbr||'?',nvl(p.port_type,'none')||'?',g.port_nbr||'?',nvl(g.port_type,'none')||'?',l.link_name||'?',t.bidirectional_link_used, t.lkused,nvl(t.link_info,'none')||'?' from %s t,%s p,%s g,%s l,%s e,%s f where nvl(p.port_type,'none')=:portype and p.port_nbr=:portnb and p.port_way=2 and t.link_type=l.linktypeID and p.portid=t.portidfrom and g.portid=t.portidto and g.deviceid=f.deviceid and p.deviceid=e.deviceid and e.deviceid=:dID",MACRO_CONNECTIVITY_TABLE,PORT_TABLE,PORT_TABLE,LINKTYPE_TABLE,LOGICAL_DEVICE_TABLE,LOGICAL_DEVICE_TABLE);

			status=OCIStmtPrepare(stmthp, ociError, (text*) selectconn,(ub4) strlen(selectconn),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bnd1p[0], ociError,(text*)":dID",-1,(dvoid*) &nodeID,sizeof(nodeID), SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bnd1p[1], ociError,(text*)":portnb",-1,(dvoid*) port_nb,strlen(port_nb)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bnd1p[2], ociError,(text*)":portype",-1,(dvoid*) port_type1,strlen(port_type1)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else
			status=OCIStmtExecute(ociHdbc, stmthp, ociError, 0, 0,(OCISnapshot *) 0, (OCISnapshot *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
		}
		else
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 2);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet unsuccessful");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &dnamefromlen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);


		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
		}
		else
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 3);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet unsuccessful");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &dnametolen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);


		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
		}
		else
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 4);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet unsuccessful");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &portnbfromlen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
		}
		else
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 5);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet unsuccessful");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &porttypefromlen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);


		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
		}
		else
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 6);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet unsuccessful");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &portnbtolen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);


		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
		}
		else
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 7);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet unsuccessful");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &porttypetolen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
		}
		else
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 8);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet unsuccessful");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &lknamelen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);


		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
		}
		else
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 11);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet unsuccessful");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &lkinfolen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);



		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIAttrGet unsuccessful");
		}
		else
		{
			MinStringLength(lknamelen);
			MinStringLength(dnamefromlen);
			MinStringLength(dnametolen);
			MinStringLength(portnbfromlen);
			MinStringLength(portnbtolen);
			MinStringLength(porttypefromlen);
			MinStringLength(porttypetolen);
			MinStringLength(lkinfolen);

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
			else
				status=OCIDefineByPos (stmthp, &def[0], ociError, 1,&linkid,sizeof(linkid), SQLT_INT, 0, 0, 0, OCI_DEFAULT);
		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
		}
		else
			status=OCIDefineByPos (stmthp, &def[1], ociError,2,dnamefrom_temp,dnamefromlen+1, SQLT_STR, &dnamefrom_null, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
		}
		else
			status=OCIDefineByPos (stmthp, &def[2], ociError,3,dnameto_temp,dnametolen+1, SQLT_STR, &dnameto_null, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
		}
		else
			status=OCIDefineByPos (stmthp, &def[3], ociError, 4,portnbfrom_temp,portnbfromlen+1, SQLT_STR, &portnbfrom_null, 0, 0,  OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
		}
		else
			status=OCIDefineByPos (stmthp, &def[4], ociError, 5,porttypefrom_temp,porttypefromlen+1, SQLT_STR, &porttypefrom_null, 0, 0,  OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
		}
		else
			status=OCIDefineByPos (stmthp, &def[5], ociError, 6,portnbto_temp,portnbtolen+1, SQLT_STR, &portnbto_null, 0, 0,  OCI_DEFAULT);
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
		}
		else
			status=OCIDefineByPos (stmthp, &def[6], ociError, 7,porttypeto_temp,porttypetolen+1, SQLT_STR, &porttypeto_null, 0, 0,  OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
		}
		else
			status=OCIDefineByPos (stmthp, &def[7], ociError, 8,lkname_temp,lknamelen, SQLT_STR, &lkname_null, 0, 0, OCI_DEFAULT);


		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
		}
		else
			status=OCIDefineByPos (stmthp, &def[8], ociError,9,&bidirectional_link_used,sizeof(bidirectional_link_used), SQLT_INT, 0, 0, 0, OCI_DEFAULT);


		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
		}
		else
			status=OCIDefineByPos (stmthp, &def[9], ociError,10,&lkused,sizeof(lkused), SQLT_INT, 0, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
		}
		else
			status=OCIDefineByPos (stmthp, &def[10], ociError,11,lkinfo_temp,lkinfolen+1, SQLT_STR, 0, 0, 0, OCI_DEFAULT);


		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
		}
		else
			status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT,1, OCI_DEFAULT);
		if (status==OCI_SUCCESS || status==OCI_SUCCESS_WITH_INFO) 
		{	

			Format_output(dnamefrom_null,dnamefrom_temp,  logmessage,'?');
			Format_output(dnameto_null,dnameto_temp, logmessage,'?');
			Format_output(portnbfrom_null,portnbfrom_temp,  logmessage,'?');
			Format_output(portnbto_null,portnbto_temp, logmessage,'?');
			Format_output(lkname_null,lkname_temp,  logmessage,'?');
			Format_output(lkinfo_null,lkinfo_temp,  logmessage,'?');
			Format_output(porttypefrom_null,porttypefrom_temp,  logmessage,'?');
			Format_output(porttypeto_null,porttypeto_temp, logmessage,'?');
			pos1=strcspn(dnamefrom_temp,"?");
			pos2=strcspn(dnameto_temp,"?");
			dnamefrom_temp[pos1]='\0';
			dnameto_temp[pos2]='\0';
			pos1=strcspn(portnbfrom_temp,"?");
			pos2=strcspn(portnbto_temp,"?");
			portnbfrom_temp[pos1]='\0';
			portnbto_temp[pos2]='\0';
			pos1=strcspn(porttypefrom_temp,"?");
			pos2=strcspn(porttypeto_temp,"?");
			porttypefrom_temp[pos1]='\0';
			porttypeto_temp[pos2]='\0';

			pos1=strcspn(lkname_temp,"?");
			lkname_temp[pos1]='\0';
			pos2=strcspn(lkinfo_temp,"?");
			lkinfo_temp[pos2]='\0';
			len=strlen(lkinfo_temp)+strlen(lkname_temp)+strlen(dnamefrom_temp)+strlen(dnameto_temp)+strlen(portnbfrom_temp)+strlen(portnbto_temp)+strlen(porttypefrom_temp)+strlen(porttypeto_temp)+strlen("|linkid (I): |node_from (C): |node_to (C): |port_nbrfrom (C): |port_typefrom (C): |port_nbrto (C): |port_typeto (C): |link_type (C): |bidirectional_link_used (I):1 |lkused (I):1 |link_info (C): |");
			sprintf(buffer,"%d",linkid);
			len+=strlen(buffer);

			if(len>len_conn)
			{
				len++;
				len_conn=len;
				rescode=-1;
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
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);

				if(lkinfo_temp!=NULL)
					free(lkinfo_temp);
				GetErrorMess(appliName,"BUFFER_TOO_SMALL",ErrorMessage,1);
				return rescode;

			}
			else
			{
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
		int i = 0;
		int len=0;
		int j=0;
		int pos1=0;
		int pos2=0;
		sword status;
		int rescode=0;
		char logmessage[100];
		sb4  porttypefromlen=0;
		sb4 porttypetolen=0;
		OCIStmt *stmthp;
		OCIBind  *bnd1p[3] ; 
		OCIDefine* def[11];
		OCIParam *parmdp;
		char buffer[20];
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
		int dnamefrom_null=0;
		int dnameto_null=0;
		int portnbfrom_null=0;
		int portnbto_null=0;
		int porttypefrom_null=0;
		int porttypeto_null=0;
		int lkinfo_null=0;
		int lkname_null=0;
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
			if(port_way==1) //port nb is an input so we have info about the node_to part of a link
				sprintf(selectconn,"select t.linkid,e.devicename||'?',f.devicename||'?',p.port_nbr||'?',nvl(p.port_type,'none')||'?',g.port_nbr||'?',nvl(g.port_type,'none')||'?',l.link_name||'?',t.bidirectional_link_used, t.lkused,nvl(t.link_info,'none')||'?' from %s t,%s p,%s g,%s l,%s e,%s f where nvl(g.port_type,'none')=:portype and g.port_nbr=:portnb and g.port_way=1  and t.link_type=l.linktypeID and p.portid=t.portidfrom and g.portid=t.portidto and g.deviceid=f.deviceid and p.deviceid=e.deviceid and f.devicename=:dname",MACRO_CONNECTIVITY_TABLE,PORT_TABLE,PORT_TABLE,LINKTYPE_TABLE,LOGICAL_DEVICE_TABLE,LOGICAL_DEVICE_TABLE);
			else // info about the node_from part of a link
				sprintf(selectconn,"select t.linkid,e.devicename||'?',f.devicename||'?',p.port_nbr||'?',nvl(p.port_type,'none')||'?',g.port_nbr||'?',nvl(g.port_type,'none')||'?',l.link_name||'?',t.bidirectional_link_used, t.lkused,nvl(t.link_info,'none')||'?' from %s t,%s p,%s g,%s l,%s e,%s f where nvl(p.port_type,'none')=:portype and p.port_nbr=:portnb and p.port_way=2 and t.link_type=l.linktypeID and p.portid=t.portidfrom and g.portid=t.portidto and g.deviceid=f.deviceid and p.deviceid=e.deviceid and e.devicename=:dname",MACRO_CONNECTIVITY_TABLE,PORT_TABLE,PORT_TABLE,LINKTYPE_TABLE,LOGICAL_DEVICE_TABLE,LOGICAL_DEVICE_TABLE);

			status=OCIStmtPrepare(stmthp, ociError, (text*) selectconn,(ub4) strlen(selectconn),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bnd1p[0], ociError,(text*)":dname",-1,(dvoid*) node_name,strlen(node_name)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bnd1p[1], ociError,(text*)":portnb",-1,(dvoid*) port_nb,strlen(port_nb)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bnd1p[2], ociError,(text*)":portype",-1,(dvoid*) port_type1,strlen(port_type1)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else
			status=OCIStmtExecute(ociHdbc, stmthp, ociError, 0, 0,(OCISnapshot *) 0, (OCISnapshot *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
		}
		else
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 2);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet unsuccessful");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &dnamefromlen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);


		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
		}
		else
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 3);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet unsuccessful");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &dnametolen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);


		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
		}
		else
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 4);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet unsuccessful");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &portnbfromlen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
		}
		else
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 5);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet unsuccessful");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &porttypefromlen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
		}
		else
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 6);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet unsuccessful");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &portnbtolen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
		}
		else
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 7);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet unsuccessful");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &porttypetolen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
		}
		else
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 8);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet unsuccessful");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &lknamelen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
		}
		else
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 11);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet unsuccessful");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &lkinfolen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);




		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIAttrGet unsuccessful");
		}
		else
		{
			MinStringLength(lknamelen);
			MinStringLength(dnamefromlen);
			MinStringLength(dnametolen);
			MinStringLength(portnbfromlen);
			MinStringLength(portnbtolen);
			MinStringLength(porttypefromlen);
			MinStringLength(porttypetolen);
			MinStringLength(lkinfolen);


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
			else
				status=OCIDefineByPos (stmthp, &def[0], ociError, 1,&linkid,sizeof(linkid), SQLT_INT, 0, 0, 0, OCI_DEFAULT);
		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
		}
		else
			status=OCIDefineByPos (stmthp, &def[1], ociError,2,dnamefrom_temp,dnamefromlen+1, SQLT_STR, &dnamefrom_null, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
		}
		else
			status=OCIDefineByPos (stmthp, &def[2], ociError,3,dnameto_temp,dnametolen+1, SQLT_STR, &dnameto_null, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
		}
		else
			status=OCIDefineByPos (stmthp, &def[3], ociError, 4,portnbfrom_temp,portnbfromlen+1, SQLT_STR, &portnbfrom_null, 0, 0,  OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
		}
		else
			status=OCIDefineByPos (stmthp, &def[4], ociError, 5,porttypefrom_temp,porttypefromlen+1, SQLT_STR, &porttypefrom_null, 0, 0,  OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
		}
		else
			status=OCIDefineByPos (stmthp, &def[5], ociError, 6,portnbto_temp,portnbtolen+1, SQLT_STR, &portnbto_null, 0, 0,  OCI_DEFAULT);
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
		}
		else
			status=OCIDefineByPos (stmthp, &def[6], ociError, 7,porttypeto_temp,porttypetolen+1, SQLT_STR, &porttypeto_null, 0, 0,  OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
		}
		else
			status=OCIDefineByPos (stmthp, &def[7], ociError, 8,lkname_temp,lknamelen, SQLT_STR, &lkname_null, 0, 0, OCI_DEFAULT);


		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
		}
		else
			status=OCIDefineByPos (stmthp, &def[8], ociError,9,&bidirectional_link_used,sizeof(bidirectional_link_used), SQLT_INT, 0, 0, 0, OCI_DEFAULT);


		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
		}
		else
			status=OCIDefineByPos (stmthp, &def[9], ociError,10,&lkused,sizeof(lkused), SQLT_INT, 0, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
		}
		else
			status=OCIDefineByPos (stmthp, &def[10], ociError,11,lkinfo_temp,lkinfolen+1, SQLT_STR, &lkinfo_null, 0, 0, OCI_DEFAULT);
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
		}
		else
			status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT,1, OCI_DEFAULT);
		if (status==OCI_SUCCESS || status==OCI_SUCCESS_WITH_INFO) 
		{	

			Format_output(dnamefrom_null,dnamefrom_temp,  logmessage,'?');
			Format_output(dnameto_null,dnameto_temp, logmessage,'?');
			Format_output(portnbfrom_null,portnbfrom_temp,  logmessage,'?');
			Format_output(portnbto_null,portnbto_temp, logmessage,'?');
			Format_output(lkname_null,lkname_temp,  logmessage,'?');
			Format_output(lkinfo_null,lkinfo_temp,  logmessage,'?');
			Format_output(porttypefrom_null,porttypefrom_temp,  logmessage,'?');
			Format_output(porttypeto_null,porttypeto_temp, logmessage,'?');
			pos1=strcspn(dnamefrom_temp,"?");
			pos2=strcspn(dnameto_temp,"?");
			dnamefrom_temp[pos1]='\0';
			dnameto_temp[pos2]='\0';
			pos1=strcspn(portnbfrom_temp,"?");
			pos2=strcspn(portnbto_temp,"?");
			portnbfrom_temp[pos1]='\0';
			portnbto_temp[pos2]='\0';
			pos1=strcspn(porttypefrom_temp,"?");
			pos2=strcspn(porttypeto_temp,"?");
			porttypefrom_temp[pos1]='\0';
			porttypeto_temp[pos2]='\0';
			pos1=strcspn(lkname_temp,"?");
			lkname_temp[pos1]='\0';
			pos2=strcspn(lkinfo_temp,"?");
			lkinfo_temp[pos2]='\0';
			len=strlen(lkinfo_temp)+strlen(lkname_temp)+strlen(dnamefrom_temp)+strlen(dnameto_temp)+strlen(portnbfrom_temp)+strlen(portnbto_temp)+strlen(porttypefrom_temp)+strlen(porttypeto_temp)+strlen("|linkid (I): |node_from (C): |node_to (C): |port_nbrfrom (C): |port_typefrom (C): |port_nbrto (C): |port_typeto (C): |link_type (C): |bidirectional_link_used (I):1 |lkused (I):1 |link_info (C): |");
			sprintf(buffer,"%d",linkid);
			len+=strlen(buffer);
			//std:://cout<<"value of lkinfo_temp"<<lkinfo_temp<<std:endl;
			if(len>len_conn)
			{
				len++;
				len_conn=len;
				rescode=-1;
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
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);


				GetErrorMess(appliName,"BUFFER_TOO_SMALL",ErrorMessage,1);
				return rescode;

			}
			else
			{
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
		int i = 0;
		int j=0;
		int pos1=0;
		int pos2=0;
		int pos3=0;
		int rescode=0;
		int free_mem=0;
		ub4 numcols = 0;
		ub4 collen=0;
		ub2 type = 0;
		sb4 ipnamelen=0;
		sb4 ipaddlen=0;

		sb4 subnetlen=0;
		int len=0;
		//char buffer[20];
		sb4 sublen=0;
		sword status;
		OCIStmt *stmthp;
		OCIBind  *bnd1p = (OCIBind *) 0; 
		OCIDefine* def[3];
		OCIParam *parmdp;
		char * ipadd_temp=NULL;
		char * ipname_temp=NULL;
		char * subnet_temp=NULL;
		char selectIP[1000];
		int ipadd_null=0;
		int ipname_null=0;
		int subnet_null=0;
		char logmessage[100];
		status =OCIHandleAlloc (ociEnv, (void**)&stmthp, OCI_HTYPE_STMT , 0, 0);
		if(ip_address==NULL)
		{
			GetErrorMess(appliName," IP add MUST be given",ErrorMessage,1);
			return -1;
		}
		if(status!=OCI_SUCCESS)
		{	
			rescode=ShowErrors (status, ociError, "OCIHandleAlloc unsuccessful");
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			else
				GetErrorMess(appliName,"NOT CONNECTED TO ANY DB",ErrorMessage,1);
			return -1;
		}
		else
		{
			sprintf(selectIP,"select ipaddress||'?',subnet_info||'?', ipname||'?' from %s where ipaddress=:IPadd",IPINFO_TABLE);
			status=OCIStmtPrepare(stmthp, ociError, (text*) selectIP,(ub4) strlen(selectIP),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bnd1p, ociError,(text*)":IPadd",-1,(dvoid*) ip_address,strlen(ip_address)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else	
			status=OCIStmtExecute(ociHdbc, stmthp, ociError, 0, 0,(OCISnapshot *) 0, (OCISnapshot *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
		}
		else
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 1);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIParamGet unsuccessful");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &ipaddlen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIAttrGet unsuccessful");
		}
		else
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 2);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIParamGet unsuccessful");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &subnetlen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIAttrGet unsuccessful");
		}
		else
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 3);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIParamGet unsuccessful");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &ipnamelen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);


		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIParamGet unsuccessful");
		}
		else
		{
			MinStringLength(ipaddlen);
			MinStringLength(ipnamelen);
			MinStringLength(subnetlen);

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
			else
				status =OCIDefineByPos(stmthp, &def[0], ociError,1, (ub1 *) (ipadd_temp), ipaddlen + 1,SQLT_STR, (dvoid *) &ipadd_null,(ub2 *) 0,0, OCI_DEFAULT);
		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
			}
		}
		else
			status =OCIDefineByPos(stmthp, &def[1], ociError,2, (ub1 *) (subnet_temp), subnetlen + 1,SQLT_STR, (dvoid *) &subnet_null,(ub2 *) 0,0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
			}
		}
		else
			status =OCIDefineByPos(stmthp, &def[2], ociError,3, (ub1 *) (ipname_temp),ipnamelen + 1,SQLT_STR, (dvoid *) &ipname_null,(ub2 *) 0,0, OCI_DEFAULT);


		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");

			}
		}
		else
			status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT,1,OCI_DEFAULT);

		if (status == OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO) 
		{	

			Format_output(ipname_null,ipname_temp,logmessage,'?');
			Format_output(ipadd_null,ipadd_temp,logmessage,'?');
			Format_output(subnet_null,subnet_temp,logmessage,'?');
			pos1=strcspn(subnet_temp,"?");
			pos2=strcspn(ipadd_temp,"?");
			pos3=strcspn(ipname_temp,"?");
			subnet_temp[pos1]='\0';
			ipadd_temp[pos2]='\0';
			ipname_temp[pos3]='\0';
			len=strlen(ipadd_temp)+strlen(ipname_temp)+strlen(subnet_temp)+strlen("|ipaddress (C):|subnet_info (C): |ipname (C): |");

			if(len>len_ip)
			{
				len++;
				len_ip=len;
				rescode= -1;
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				if(ipadd_temp!=NULL)
					free(ipadd_temp);

				if(ipname_temp!=NULL)
					free(ipname_temp);

				if(subnet_temp!=NULL)
					free(subnet_temp);

				GetErrorMess(appliName,"BUFFER_TOO_SMALL",ErrorMessage,1);
				return rescode;
			}
			else
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
			}}
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
		int i = 0;
		int j=0;
		int len=0;
		char buffer[20];
		int pos1=0;
		int link_nbr=0;
		int rescode=0;
		int free_mem=0;
		int pos2=0;
		int linktypeID=0;
		ub4 numcols = 0;
		ub4 collen=0;
		ub2 type = 0;
		sword status;
		OCIStmt *stmthp;
		OCIBind  *bnd1p = (OCIBind *) 0; 
		OCIDefine* def[3];
		sb4 lktypelen=0;
		OCIParam *parmdp;
		char * lktype_temp=NULL;
		if(lktype_name==NULL)
		{
			GetErrorMess(appliName," lktype_name MUST be given",ErrorMessage,1);
			return -1;
		}
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
			char selectlktype[1000];
			sprintf(selectlktype,"select  link_name||'?',link_nbr,linktypeID from %s where link_name=:ltype",LINKTYPE_TABLE);
			status=OCIStmtPrepare(stmthp, ociError, (text*) selectlktype,(ub4) strlen(selectlktype),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bnd1p, ociError,(text*)":ltype",-1,(dvoid*) lktype_name,strlen(lktype_name)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else	
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
				rescode=ShowErrors (status, ociError, "OCIParamGet unsuccessful");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &lktypelen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIAttrGet unsuccessful");
		}
		else	
		{
			lktype_temp = (char *) realloc(lktype_temp,(lktypelen + 1)*sizeof(char));
			free_mem=1;
			if(lktype_temp==NULL)
			{
				status=-10;
				rescode=ShowErrors (status, ociError, "  pointer allocation unsuccessful");
				rescode=-1;
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				GetErrorMess(appliName,"REALLOC UNSUCCESSFUL",ErrorMessage,1);
				return rescode;
			}
			else
				status =OCIDefineByPos(stmthp, &def[0], ociError,1, (ub1 *) (lktype_temp), lktypelen + 1,SQLT_STR, (dvoid *) 0,(ub2 *) 0,0, OCI_DEFAULT);
		}

		if(status!=OCI_SUCCESS)
		{	
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
			}
		}
		else
			status=OCIDefineByPos (stmthp, &def[1], ociError, 2,&link_nbr,sizeof(link_nbr), SQLT_INT, 0, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{	
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
			}
		}
		else
			status=OCIDefineByPos (stmthp, &def[2], ociError, 3,&linktypeID,sizeof(linktypeID), SQLT_INT, 0, 0, 0, OCI_DEFAULT);


		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
			}
		}
		else
			status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT,1,OCI_DEFAULT);

		if (status == OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO) 
		{	

			pos2=strcspn(lktype_temp,"?");
			lktype_temp[pos2]='\0';
			len=strlen("|link_name (C): |link_nbr (I): |linktypeID (I) : |")+strlen(lktype_temp);
			sprintf(buffer,"%d",link_nbr);
			len+=strlen(buffer);
			sprintf(buffer,"%d",linktypeID);
			len+=strlen(buffer);
			if(len>len_lktype)
			{
				len++;
				len_lktype=len;
				rescode=-1;
				if(lktype_temp!=NULL)
					lktype_temp = (char *) realloc(lktype_temp,(0)*sizeof(char));
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				GetErrorMess(appliName,"BUFFER_TOO_SMALL",ErrorMessage,1);
				return rescode;
			}
			else
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
			lktype_temp = (char *) realloc(lktype_temp,(0)*sizeof(char));
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
		int i = 0;
		int j=0;
		int pos1=0;
		int pos2=0;
		ub4 numcols = 0;
		ub4 collen=0;
		ub2 type = 0;
		int len=0;
		char buffer[20];
		sword status;
		OCIStmt *stmthp;
		OCIBind  *bnd1p = (OCIBind *) 0; 
		OCIDefine* def[3];
		ub4 lktypelen=0;
		int linktypeID=0;
		int rescode=0;
		int free_mem=0;
		OCIParam *parmdp;
		char * lktype_temp=NULL;
		int link_nbr=0;
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
			char selectlktype[1000];
			sprintf(selectlktype,"select link_name||'?',link_nbr,linktypeID from %s where linktypeID=:ltype",LINKTYPE_TABLE);
			status=OCIStmtPrepare(stmthp, ociError, (text*) selectlktype,(ub4) strlen(selectlktype),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful\n");
		}
		else
			status=OCIBindByName(stmthp, &bnd1p, ociError,(text*)":ltype",-1,(dvoid*) &lktype_nbr,sizeof(lktype_nbr), SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful\n");
		}
		else
			status=OCIStmtExecute(ociHdbc, stmthp, ociError, 0, 0,(OCISnapshot *) 0, (OCISnapshot *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful\n");
		}
		else
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 1);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet unsuccessful\n");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &lktypelen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIAttrGet unsuccessful\n");
		}
		else
		{	
			lktype_temp=(char *) realloc(lktype_temp,(lktypelen + 1)*sizeof(char));
			free_mem=1;
			if(lktype_temp==NULL)
			{
				status=-10;
				rescode=ShowErrors (status, ociError, "  pointer allocation unsuccessful");
				rescode=-1;
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				GetErrorMess(appliName,"REALLOC UNSUCCESSFUL",ErrorMessage,1);
				return rescode;

			}
			else
				status =OCIDefineByPos(stmthp, &def[0], ociError,1, (ub1 *) (lktype_temp), lktypelen + 1,SQLT_STR, (dvoid *) 0,(ub2 *) 0,0, OCI_DEFAULT);
		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful\n");

			}
		}
		else
			status=OCIDefineByPos (stmthp, &def[1], ociError, 2,&link_nbr,sizeof(link_nbr), SQLT_INT, 0, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful\n");

			}
		}
		else
			status=OCIDefineByPos (stmthp, &def[2], ociError, 3,&linktypeID,sizeof(linktypeID), SQLT_INT, 0, 0, 0, OCI_DEFAULT);


		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful\n");
			}
		}
		else
			status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT,1,OCI_DEFAULT);

		if (status == OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO) 
		{	

			pos2=strcspn(lktype_temp,"?");

			lktype_temp[pos2]='\0';
			len=strlen("|link_name (C): |link_nbr (I): |linktypeID (I) : |")+strlen(lktype_temp);
			sprintf(buffer,"%d",link_nbr);
			len+=strlen(buffer);
			sprintf(buffer,"%d",linktypeID);
			len+=strlen(buffer);
			if(len>len_lktype)
			{
				len++;
				len_lktype=len;
				rescode=-1;
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				if(lktype_temp!=NULL)
					lktype_temp = (char *) realloc(lktype_temp,(0)*sizeof(char));
				GetErrorMess(appliName,"REALLOC UNSUCCESSFUL",ErrorMessage,1);
				return rescode;
			}
			else
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
			lktype_temp = (char *) realloc(lktype_temp,(0)*sizeof(char));
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
	/**************************************************************/
	//
	/*****************************************************************************************/
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
		int j=0;
		int pos1=0;
		int pos2=0;
		ub4 numcols = 0;
		ub4 collen=0;
		ub2 type = 0;
		int len=0;
		int pos3=0;
		int lkname_null=0;
		char logmessage[100];
		int prefetch_rows=50;
		sword status;
		OCIStmt *stmthp;
		OCIBind  *bnd1p = (OCIBind *) 0; 
		OCIDefine* def[2];
		ub4 lktypelen=0;
		int rescode=0;
		int free_mem=0;
		OCIParam *parmdp;
		char * lktype_temp=NULL;
		char * lkTypeslist_temp=NULL;
		int link_nbr=0;

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
			char selectlktype[1000];
			sprintf(selectlktype,"select e.link_name||'|'||e.linktypeid||'?' from %s t, %s e,%s f where mod(t.link_nbr,e.link_nbr)=0 and e.link_nbr!=t.link_nbr and e.link_nbr=f.prime_nb and t.linktypeID=:ltype",LINKTYPE_TABLE,LINKTYPE_TABLE,PRIME_NUMBER_TABLE);
			status=OCIStmtPrepare(stmthp, ociError, (text*) selectlktype,(ub4) strlen(selectlktype),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful\n");
		}
		else
			status=OCIBindByName(stmthp, &bnd1p, ociError,(text*)":ltype",-1,(dvoid*) &lktype_nbr,sizeof(lktype_nbr), SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else
			status=	OCIAttrSet (stmthp,OCI_HTYPE_STMT,&prefetch_rows,0,OCI_ATTR_PREFETCH_ROWS,ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIAttrSet unsuccessful\n");
		}
		else
			status=OCIStmtExecute(ociHdbc, stmthp, ociError, 0, 0,(OCISnapshot *) 0, (OCISnapshot *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful\n");
		}
		else
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 1);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet unsuccessful\n");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &lktypelen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIAttrGet unsuccessful\n");
		}
		else
		{	
			lktype_temp=(char *) realloc(lktype_temp,(lktypelen + 1)*sizeof(char));
			free_mem=1;
			if(lktype_temp==NULL)
			{
				status=-10;
				rescode=ShowErrors (status, ociError, "  pointer allocation unsuccessful");
				rescode=-1;
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				GetErrorMess(appliName,"REALLOC UNSUCCESSFUL",ErrorMessage,1);
				return rescode;

			}
			else
				status =OCIDefineByPos(stmthp, &def[0], ociError,1, (ub1 *) (lktype_temp), lktypelen + 1,SQLT_STR, (dvoid *) 0,(ub2 *) 0,0, OCI_DEFAULT);
		}


		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful\n");
			}
		}
		else
		{

			pos2=0;
			i=0;
			while(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
			{
				status =OCIStmtFetch2 (stmthp, ociError, 1, OCI_FETCH_NEXT,1, OCI_DEFAULT);
				if(status==OCI_SUCCESS)
				{
					Format_output(lkname_null, lktype_temp, logmessage, '?');
					pos1=strcspn(lktype_temp,"?");
					lktype_temp[pos1]='\0';
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
				lkTypeslist_temp=(char*) realloc(lkTypeslist_temp,0*sizeof(char));
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
		int j=0;
		int rescode=0;
		ub4 numcols = 0;
		ub4 collen=0;
		ub2 type = 0;
		int pos1=0;
		int pos3=0;
		int actual_len=0;
		int pos2=0;
		int len_devname=0;
		char* devName=NULL;
		char* devIDs_temp=NULL;
		sword status;
		int nline=0;
		int nb_rdtrip=1;
		OCIParam *parmdp;
		int prefetch_rows=2000;
		OCIStmt *stmthp;
		OCIBind  *bnd1p = (OCIBind *) 0; 
		OCIDefine* def;
		char selectdevID[1000];
		int dname_null=0;
		char logmessage[100];
		status =OCIHandleAlloc (ociEnv, (void**)&stmthp, OCI_HTYPE_STMT , 0, 0);
		if(devitype==NULL)
		{
			GetErrorMess(appliName," device type MUST be given",ErrorMessage,1);
			return -1;
		}
		if(status!=OCI_SUCCESS)
		{	
			rescode=ShowErrors (status, ociError, "OCIStmtHAndleAlloc unsuccessful");
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			else
				GetErrorMess(appliName,"NOT CONNECTED TO ANY DB",ErrorMessage,1);
			return -1;
		}
		else
		{
			sprintf(selectdevID,"select t.devicename||'|'||t.deviceid||'?' from %s t,%s f where t.devicetypeID=f.devicetypeID and f.devicetype=:dtype",LOGICAL_DEVICE_TABLE,DEVICETYPE_TABLE);
			status=OCIStmtPrepare(stmthp, ociError, (text*) selectdevID,(ub4) strlen(selectdevID),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
		}
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bnd1p, ociError,(text*)":dtype",-1,(dvoid*) devitype,strlen(devitype)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else
			status=	OCIAttrSet (stmthp,OCI_HTYPE_STMT,&prefetch_rows,0,OCI_ATTR_PREFETCH_ROWS,ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIAttrSet unsuccessful");
		}
		else
			status=OCIStmtExecute(ociHdbc, stmthp, ociError, 0, 0,(OCISnapshot *) 0, (OCISnapshot *) 0, OCI_DEFAULT);
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful\n");
		}
		else
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 1);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet unsuccessful\n");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &len_devname, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors(status, ociError, "OCIStmtExecute unsuccessful");
		}
		else
		{
			devName=(char*)malloc((len_devname+1)*sizeof(char));
			if(devName!=NULL)
				status=OCIDefineByPos (stmthp, &def, ociError, 1,(ub1*) devName,len_devname+1, SQLT_STR, &dname_null, 0, 0, OCI_DEFAULT);
			else
			{
				status=-10;
				rescode=ShowErrors (status, ociError, "pointer allocation unsuccessful");
				rescode=-1;
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);

				GetErrorMess(appliName,"REALLOC UNSUCCESSFUL",ErrorMessage,1);
				return rescode;
			}
		}
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
		}
		else
		{


			pos2=0;
			i=0;
			while(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
			{
				status =OCIStmtFetch2 (stmthp, ociError, 1, OCI_FETCH_NEXT,1, OCI_DEFAULT);
				if(status==OCI_SUCCESS)
				{
					Format_output(dname_null, devName, logmessage, '?');
					pos1=strcspn(devName,"?");
					devName[pos1]='\0';
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
				devIDs_temp=(char*) realloc(devIDs_temp,0*sizeof(char));
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
		ub4 numcols = 0;
		ub4 collen=0;
		ub2 type = 0;
		int devID=0;
		int* devIDs_temp=NULL;
		sword status;
		int nline=0;
		int nb_rdtrip=1;
		int prefetch_rows=2000;
		OCIStmt *stmthp;
		OCIBind  *bnd1p = (OCIBind *) 0; 
		OCIDefine* def;
		char selectdevID[1000];
		status =OCIHandleAlloc (ociEnv, (void**)&stmthp, OCI_HTYPE_STMT , 0, 0);
		if(devitype==NULL)
		{
			GetErrorMess(appliName," device type MUST be given",ErrorMessage,1);
			return -1;
		}
		if(status!=OCI_SUCCESS)
		{	
			rescode=ShowErrors (status, ociError, "OCIStmtHAndleAlloc unsuccessful");
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			else
				GetErrorMess(appliName,"NOT CONNECTED TO ANY DB",ErrorMessage,1);
			return -1;
		}
		else
		{
			sprintf(selectdevID,"select t.deviceid from %s t, %s f where t.devicetypeID=f.devicetypeID and f.devicetype=:dtype",LOGICAL_DEVICE_TABLE,DEVICETYPE_TABLE);
			status=OCIStmtPrepare(stmthp, ociError, (text*) selectdevID,(ub4) strlen(selectdevID),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
		}
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bnd1p, ociError,(text*)":dtype",-1,(dvoid*) devitype,strlen(devitype)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else
			status=	OCIAttrSet (stmthp,OCI_HTYPE_STMT,&prefetch_rows,0,OCI_ATTR_PREFETCH_ROWS,ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIAttrSet unsuccessful");
		}
		else
			status=OCIStmtExecute(ociHdbc, stmthp, ociError, 0, 0,(OCISnapshot *) 0, (OCISnapshot *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
		}
		else
			status=OCIDefineByPos (stmthp, &def, ociError, 1,&devID,sizeof(devID), SQLT_INT, 0, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
		}
		else
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
						//means NO_ROWS_SELECTED
						devIDs_list[0]=-1;
						devIDs_list[1]=-1;
						GetErrorMess(appliName,"NO_ROWS_SELECTED",ErrorMessage,1);
					}
				}
				devIDs_temp=(int*) realloc(devIDs_temp,0*sizeof(int));
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
		ub4 numcols = 0;
		ub4 collen=0;
		ub2 type = 0;
		int lkID=0;
		int* lkIDs_temp=NULL;
		sword status;
		int nline=0;
		int nb_rdtrip=1;
		int prefetch_rows=2000;
		OCIStmt *stmthp;
		OCIBind  *bnd1p = (OCIBind *) 0; 
		OCIDefine* def;
		char select_query[1000];
		status =OCIHandleAlloc (ociEnv, (void**)&stmthp, OCI_HTYPE_STMT , 0, 0);
		if(status!=OCI_SUCCESS)
		{	
			rescode=ShowErrors (status, ociError, "OCIStmtHAndleAlloc unsuccessful");
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			else
				GetErrorMess(appliName,"NOT CONNECTED TO ANY DB",ErrorMessage,1);
			return -1;
		}
		else
		{
			sprintf(select_query,"select t.linkid from %s t,%s e where t.portIDfrom=e.portid and e.deviceid=:nfrom and e.port_way=2",MACRO_CONNECTIVITY_TABLE,PORT_TABLE);
			status=OCIStmtPrepare(stmthp, ociError, (text*) select_query,(ub4) strlen(select_query),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
		}
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bnd1p, ociError,(text*)":nfrom",-1,(dvoid*)&node_from,sizeof(node_from), SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else	
			status=OCIAttrSet (stmthp,OCI_HTYPE_STMT,&prefetch_rows,0,OCI_ATTR_PREFETCH_ROWS,ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIAttrSet  unsuccessful");
		}
		else
			status=OCIStmtExecute(ociHdbc, stmthp, ociError, 0, 0,(OCISnapshot *) 0, (OCISnapshot *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtExecute  unsuccessful");
		}
		else
			status=OCIDefineByPos (stmthp, &def, ociError, 1,&lkID,sizeof(lkID), SQLT_INT, 0, 0, 0, OCI_DEFAULT);


		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIDefineByPos  unsuccessful");
		}
		else
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
				lkIDs_temp=(int*) realloc(lkIDs_temp,0*sizeof(int));
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
		ub4 numcols = 0;
		ub4 collen=0;
		ub2 type = 0;
		int lkID=0;
		int* lkIDs_temp=NULL;
		sword status;
		int nline=0;
		int nb_rdtrip=1;
		int prefetch_rows=2000;
		OCIStmt *stmthp;
		OCIBind  *bnd1p = (OCIBind *) 0; 
		OCIDefine* def;
		char select_query[1000];
		status =OCIHandleAlloc (ociEnv, (void**)&stmthp, OCI_HTYPE_STMT , 0, 0);
		if(status!=OCI_SUCCESS)
		{	
			rescode=ShowErrors (status, ociError, "OCIStmtHAndleAlloc unsuccessful");
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			else
				GetErrorMess(appliName,"NOT CONNECTED TO ANY DB",ErrorMessage,1);
			return -1;
		}
		else
		{
			sprintf(select_query,"select t.linkid from %s t,%s e where t.portIDTO=e.portid and e.deviceid=:nTO and e.port_way=1",MACRO_CONNECTIVITY_TABLE,PORT_TABLE);
			status=OCIStmtPrepare(stmthp, ociError, (text*) select_query,(ub4) strlen(select_query),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
		}
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bnd1p, ociError,(text*)":nto",-1,(dvoid*)&node_to,sizeof(node_to), SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else	
			status=OCIAttrSet(stmthp,OCI_HTYPE_STMT,&prefetch_rows,0,OCI_ATTR_PREFETCH_ROWS,ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIAttrSet unsuccessful");
		}
		else
			status=OCIStmtExecute(ociHdbc, stmthp, ociError, 0, 0,(OCISnapshot *) 0, (OCISnapshot *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
		}
		else
			status=OCIDefineByPos (stmthp, &def, ociError, 1,&lkID,sizeof(lkID), SQLT_INT, 0, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
		}
		else
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
				lkIDs_temp=(int*) realloc(lkIDs_temp,0*sizeof(int));
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
		ub4 numcols = 0;
		ub4 collen=0;
		ub2 type = 0;
		int portID=0;
		int* portIDs_temp=NULL;
		sword status;
		int nline=0;
		int nb_rdtrip=1;
		int prefetch_rows=2000;
		OCIStmt *stmthp;
		OCIBind  *bnd1p = (OCIBind *) 0; 
		OCIDefine* def;
		char select_query[1000];
		status =OCIHandleAlloc (ociEnv, (void**)&stmthp, OCI_HTYPE_STMT , 0, 0);
		if(status!=OCI_SUCCESS)
		{	
			rescode=ShowErrors (status, ociError, "OCIStmtHAndleAlloc unsuccessful");
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			else
				GetErrorMess(appliName,"NOT CONNECTED TO ANY DB",ErrorMessage,1);
			return -1;
		}
		else
		{
			sprintf(select_query,"select portid from %s where deviceid=:dID",PORT_TABLE);
			status=OCIStmtPrepare(stmthp, ociError, (text*) select_query,(ub4) strlen(select_query),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
		}
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bnd1p, ociError,(text*)":dID",-1,(dvoid*)&devID,sizeof(devID), SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else 	
			status=OCIAttrSet(stmthp,OCI_HTYPE_STMT, &prefetch_rows,0,OCI_ATTR_PREFETCH_ROWS,ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIAttrSet unsuccessful");
		}
		else
			status=OCIStmtExecute(ociHdbc, stmthp, ociError, 0, 0,(OCISnapshot *) 0, (OCISnapshot *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
		}
		else 
			status=OCIDefineByPos (stmthp, &def, ociError, 1,&portID,sizeof(portID), SQLT_INT, 0, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
		}
		else
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
				portIDs_temp=(int*) realloc(portIDs_temp,0*sizeof(int));
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
		int j=0;
		int rescode=0;
		int len_port=0;
		ub4 numcols = 0;
		ub4 collen=0;
		int pos1=0;
		int pos2=0;
		int pos3=0;
		ub2 type = 0;
		char* port_info=NULL;
		char* portInfos_temp=NULL;
		sword status;
		int nline=0;
		int nb_rdtrip=1;
		int prefetch_rows=2000;
		OCIStmt *stmthp;
		OCIBind  *bnd1p = (OCIBind *) 0; 
		OCIDefine* def;
		OCIParam *parmdp;
		char select_query[1000];
		status =OCIHandleAlloc (ociEnv, (void**)&stmthp, OCI_HTYPE_STMT , 0, 0);
		if(status!=OCI_SUCCESS)
		{	
			rescode=ShowErrors (status, ociError, "OCIStmtHAndleAlloc unsuccessful");
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			else
				GetErrorMess(appliName,"NOT CONNECTED TO ANY DB",ErrorMessage,1);
			return -1;
		}
		else
		{
			sprintf(select_query,"select port_nbr||'|'||nvl(port_type,'none')||'|'||port_way||'|'||nvl(macaddress,'none')||'|'||nvl(bia, 'none')||'?' from %s where serialnb=:snb",HWPORT_TABLE);
			status=OCIStmtPrepare(stmthp, ociError, (text*) select_query,(ub4) strlen(select_query),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
		}
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bnd1p, ociError,(text*)":snb",-1,(dvoid*)serialnb,strlen(serialnb)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else 	
			status=OCIAttrSet(stmthp,OCI_HTYPE_STMT, &prefetch_rows,0,OCI_ATTR_PREFETCH_ROWS,ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIAttrSet unsuccessful");
		}
		else
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
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &len_port, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
		}
		else 
		{
			port_info=(char*) malloc((len_port+1)*sizeof(char));
			if(port_info!=NULL)
				status=OCIDefineByPos (stmthp, &def, ociError, 1,port_info,len_port+1, SQLT_STR, 0, 0, 0, OCI_DEFAULT);
			else
			{
				status=-10;
				rescode=ShowErrors (status, ociError, "  pointer allocation unsuccessful");
				rescode=-1;
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				GetErrorMess(appliName,"MALLOC UNSUCCESSFUL",ErrorMessage,1);
				return rescode;
			}
		}
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
		}
		else
		{

			while(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
			{
				status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT, 1,OCI_DEFAULT);
				if(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
				{
					pos1=strcspn(port_info,"?");
					port_info[pos1]='\0';
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
	/***************************************************************************************/
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
		int j=0;
		int rescode=0;
		int pos1=0;
		int actual_len=0;
		int pos2=0;
		int pos3=0;
		int etherlen=0;
		ub4 numcols = 0;
		ub4 collen=0;
		ub2 type = 0;
		char* etheradd=NULL;
		char* MacAdds_temp=NULL;
		sword status;
		int nline=0;
		OCIParam *parmdp;
		int nb_rdtrip=1;
		int prefetch_rows=2000;
		OCIStmt *stmthp;
		OCIBind  *bnd1p = (OCIBind *) 0; 
		OCIDefine* def;

		char select_query[1000];
		status =OCIHandleAlloc (ociEnv, (void**)&stmthp, OCI_HTYPE_STMT , 0, 0);
		if(status!=OCI_SUCCESS)
		{	
			rescode=ShowErrors (status, ociError, "OCIStmtHAndleAlloc unsuccessful");
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			else
				GetErrorMess(appliName,"NOT CONNECTED TO ANY DB",ErrorMessage,1);
			return -1;
		}
		else
		{
			sprintf(select_query,"select t.port_nbr||'|'||t.port_way||'|'||nvl(t.port_type,'none')||'|'||f.macaddress||'|'||t.ipaddress||'?' from %s t,%s f,%s d where d.deviceid=:dID and d.serialnb=f.serialnb and d.deviceid=t.deviceid and f.port_nbr=t.port_nbr and nvl(f.port_type,'none')=nvl(t.port_type,'none') and f.port_way=t.port_way and t.ipaddress is not null ",PORT_TABLE,HWPORT_TABLE,LOGICAL_DEVICE_TABLE);
			status=OCIStmtPrepare(stmthp, ociError, (text*) select_query,(ub4) strlen(select_query),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bnd1p, ociError,(text*)":dID",-1,(dvoid*)&devID,sizeof(devID), SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else
			status=OCIAttrSet (stmthp,OCI_HTYPE_STMT,&prefetch_rows,0,OCI_ATTR_PREFETCH_ROWS,ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIAttrSet  unsuccessful");
		}
		else
			status=OCIStmtExecute(ociHdbc, stmthp, ociError, 0, 0,(OCISnapshot *) 0, (OCISnapshot *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtExecute  unsuccessful");
		}
		else
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 1);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet  unsuccessful");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &etherlen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIAttrGet  unsuccessful");
		}
		else
		{
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
			{
				//add 2 because 1 for \0 and 1 for ?
				status=OCIDefineByPos(stmthp, &def, ociError, 1,(ub1*) etheradd,etherlen+1, SQLT_STR, 0, 0, 0, OCI_DEFAULT);
			}
		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "OCIDefineByPos  unsuccessful");
				etheradd=(char*)realloc(etheradd,0*sizeof(char));
			}
		}
		else
		{

			pos2=0;
			i=0;
			while(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
			{
				status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT,1, OCI_DEFAULT);
				if(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
				{
					pos1=strcspn(etheradd,"?");
					etheradd[pos1]='\0';
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
						i++;
					}
				}

			}
			if(rescode==0)
			{
				if(pos2>len_mac)
				{
					len_mac=pos2;
					rescode=-1;
					etheradd=(char*)realloc(etheradd,0*sizeof(char));
					if(MacAdds_temp!=NULL)
						MacAdds_temp=(char*) realloc(MacAdds_temp,0*sizeof(char));
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
			etheradd=(char*)realloc(etheradd,0*sizeof(char));
		if(MacAdds_temp!=NULL)
			MacAdds_temp=(char*) realloc(MacAdds_temp,0*sizeof(char));

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
		int j=0;
		int rescode=0;
		int pos1=0;
		int actual_len=0;
		int pos2=0;
		int pos3=0;
		int devlen=0;
		ub4 numcols = 0;
		ub4 collen=0;
		ub2 type = 0;
		char* destin=NULL;
		char* destin_temp=NULL;
		sword status;
		int nline=0;
		OCIParam *parmdp;
		int nb_rdtrip=1;
		int prefetch_rows=5000;
		OCIStmt *stmthp;
		OCIBind  *bnd1p = (OCIBind *) 0; 
		OCIBind  *bnd2p = (OCIBind *) 0; 
		OCIBind  *bnd3p = (OCIBind *) 0; 

		OCIDefine* def;
		char select_query[1000];
		status =OCIHandleAlloc (ociEnv, (void**)&stmthp, OCI_HTYPE_STMT , 0, 0);
		if(devname==NULL)
		{
			GetErrorMess(appliName," devname MUST be given",ErrorMessage,1);
			return -1;
		}
		if(status!=OCI_SUCCESS)
		{	
			rescode=ShowErrors (status, ociError, "OCIStmtHAndleAlloc unsuccessful");
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			else
				GetErrorMess(appliName,"NOT CONNECTED TO ANY DB",ErrorMessage,1);
			return -1;
		}
		else
		{
			sprintf(select_query,"select distinct s.devicename||'|' from %s t, %s s, %s l, %s g ,%s d,%s f where t.devicename=:dname and l.node1=t.deviceid and l.node11=s.deviceid and  g.deviceid=t.deviceid  and g.port_nbr=:ptnb and g.port_way=2 and g.portid=d.portidfrom and nvl(g.port_type,'none')=:ptype and d.portidto=f.portid and f.deviceid=l.node2", LOGICAL_DEVICE_TABLE,LOGICAL_DEVICE_TABLE,PATHDETAILS_TABLE,PORT_TABLE,MACRO_CONNECTIVITY_TABLE,PORT_TABLE);
			status=OCIStmtPrepare(stmthp, ociError, (text*) select_query,(ub4) strlen(select_query),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bnd1p, ociError,(text*)":ptnb",-1,(dvoid*)port_nb,strlen(port_nb)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bnd2p, ociError,(text*)":ptype",-1,(dvoid*)port_type,strlen(port_type)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bnd3p, ociError,(text*)":dname",-1,(dvoid*)devname,strlen(devname)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);


		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else
			status=OCIAttrSet (stmthp,OCI_HTYPE_STMT,&prefetch_rows,0,OCI_ATTR_PREFETCH_ROWS,ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIAttrSet  unsuccessful");
		}
		else
			status=OCIStmtExecute(ociHdbc, stmthp, ociError, 0, 0,(OCISnapshot *) 0, (OCISnapshot *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtExecute  unsuccessful");
		}
		else
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 1);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet  unsuccessful");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &devlen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIAttrGet  unsuccessful");
		}
		else
		{
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
			{
				//add 2 because 1 for \0 and 1 for ?
				status=OCIDefineByPos(stmthp, &def, ociError, 1,(ub1*) destin,devlen+1, SQLT_STR, 0, 0, 0, OCI_DEFAULT);
			}
		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "OCIDefineByPos  unsuccessful");
			}
		}
		else
		{

			pos2=0;
			i=0;
			while(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
			{
				status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT,1,OCI_DEFAULT);
				if(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
				{
					pos1=strcspn(destin,"|");
					destin[pos1]='\0';
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
					rescode=0;
				}
			}

			if(rescode==0 && i>0)
			{
				if(pos2>len_Destin_list)
				{
					len_Destin_list=pos2;
					destin=(char*)realloc(destin,(0)*sizeof(char));
					status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
					rescode=-1;
					if(destin_temp!=NULL)
						destin_temp=(char*) realloc(destin_temp,0*sizeof(char));
					GetErrorMess(appliName,"BUFFER_TOO_SMALL",ErrorMessage,1);
					return rescode;	
				}
				else
				{
					memcpy(Destin_list,destin_temp,pos2);
					len_Destin_list=pos2;
				}


			}

			//std:://cout<<"before last free"<<std::endl;
		}
		if(destin!=NULL)
			destin=(char*)realloc(destin,0*sizeof(char));


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
		int j=0;
		int rescode=0;
		ub4 numcols = 0;
		ub4 collen=0;
		ub2 type = 0;
		int pos1=0;
		int pos3=0;
		int actual_len=0;
		int pos2=0;
		int len_devname=0;
		char* devName=NULL;
		char* devIDs_temp=NULL;
		sword status;
		int nline=0;
		int nb_rdtrip=1;
		int prefetch_rows=2000;
		OCIStmt *stmthp;
		OCIBind  *bnd1p = (OCIBind *) 0; 
		OCIDefine* def;
		OCIParam *parmdp;
		char selectdevID[1000];
		status =OCIHandleAlloc (ociEnv, (void**)&stmthp, OCI_HTYPE_STMT , 0, 0);
		if(devitype==NULL)
		{
			GetErrorMess(appliName," device type MUST be given",ErrorMessage,1);
			return -1;
		}
		if(status!=OCI_SUCCESS)
		{	
			rescode=ShowErrors (status, ociError, "OCIStmtHAndleAlloc unsuccessful");
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			else
				GetErrorMess(appliName,"NOT CONNECTED TO ANY DB",ErrorMessage,1);
			return -1;
		}
		else
		{
			sprintf(selectdevID,"select t.devicename||'|' from %s t, %s f where t.devicetypeID=f.devicetypeID and f.devicetype=:dtype and t.active=0 and t.nodeused=1",LOGICAL_DEVICE_TABLE,DEVICETYPE_TABLE);
			status=OCIStmtPrepare(stmthp, ociError, (text*) selectdevID,(ub4) strlen(selectdevID),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
			//std:://cout<<"stmt"<<selectdevID<<std::endl;
		}
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bnd1p, ociError,(text*)":dtype",-1,(dvoid*) devitype,strlen(devitype)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else
			status=	OCIAttrSet (stmthp,OCI_HTYPE_STMT,&prefetch_rows,0,OCI_ATTR_PREFETCH_ROWS,ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIAttrSet unsuccessful");
		}
		else
			status=OCIStmtExecute(ociHdbc, stmthp, ociError, 0, 0,(OCISnapshot *) 0, (OCISnapshot *) 0, OCI_DEFAULT);
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtExecute  unsuccessful");
		}
		else
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 1);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet  unsuccessful");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &len_devname, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
		}
		else
		{
			devName=(char*)malloc((len_devname+1)*sizeof(char));
			if(devName!=NULL)
				status=OCIDefineByPos (stmthp, &def, ociError, 1,(ub1*) devName,len_devname+1, SQLT_STR, 0, 0, 0, OCI_DEFAULT);
			else
			{
				status=-10;
				rescode=ShowErrors (status, ociError, "pointer allocation unsuccessful");
				rescode=-1;
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				GetErrorMess(appliName,"REALLOC UNSUCCESSFUL",ErrorMessage,1);
				return rescode;	
			}
		}
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
		}
		else
		{


			pos2=0;
			i=0;
			while(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
			{
				status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT,1,OCI_DEFAULT);
				if(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
				{
					pos1=strcspn(devName,"|");
					devName[pos1]='\0';
					pos3=pos2;
					pos2+=pos1+1;
					actual_len+=pos1+1;
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
					devIDs_temp=(char*) realloc(devIDs_temp,0*sizeof(char));
					rescode=-1;
					free(devName);
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
				devIDs_temp=(char*) realloc(devIDs_temp,0*sizeof(char));
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
		int i = 0;
		int j=0;
		int pos1=0;
		int pos2=0;
		int pos3=0;
		int pos4=0;
		int free_mem=0;
		ub4 numcols = 0;
		ub4 collen=0;
		ub2 type = 0;
		sword status;
		int rescode=0;
		OCIStmt *stmthp;
		OCIBind  *bnd1p = (OCIBind *) 0; 
		OCIDefine* def[1];
		ub4  devtypelen=0;
		ub4 devlen=0;
		ub4  uuidlen=0;
		ub4 barcodelen=0;

		int node=0;
		int active=0;
		int deviceid=0;
		int promiscuous_mode=0;
		int status1=0;
		int nodeused=0;
		int len=0;
		char selectdev[1000];
		status =OCIHandleAlloc (ociEnv, (void**)&stmthp, OCI_HTYPE_STMT , 0, 0);
		if(deviname==NULL)
		{
			GetErrorMess(appliName," device name MUST be given",ErrorMessage,1);
			return -1;
		}
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
			sprintf(selectdev,"select deviceid from %s where devicename=:dname",LOGICAL_DEVICE_TABLE);
			status=OCIStmtPrepare(stmthp, ociError, (text*) selectdev,(ub4) strlen(selectdev),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else	
			status=OCIBindByName(stmthp, &bnd1p, ociError,(text*)":dname",-1,(dvoid*) deviname,strlen(deviname)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIBindByPos unsuccessful");
		}
		else	
			status=OCIStmtExecute(ociHdbc, stmthp, ociError, 0, 0,(OCISnapshot *) 0, (OCISnapshot *) 0, OCI_DEFAULT);


		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "Execute unsuccessful");
			}
		}
		else
			status=OCIDefineByPos (stmthp, &def[0], ociError, 1,&deviceid,sizeof(deviceid), SQLT_INT, 0, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "DefineByPos1 unsuccessful");

			}
		}
		else
			status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT,1, OCI_DEFAULT);

		if (status==OCI_SUCCESS || status==OCI_SUCCESS_WITH_INFO) 
		{	
			deviceID=deviceid;
		} 
		else 
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "Fetch unsuccessful");
				strcpy(ErrorMessage,"NO_ROWS_SELECTED");
				GetErrorMess(appliName,"NO_ROWS_SELECTED",ErrorMessage,1);
				rescode=0; //means no data found
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

	/*****************************************************************************************/

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
		int i = 0;
		int j=0;
		int pos1=0;
		int pos2=0;
		int pos3=0;
		int pos4=0;
		int free_mem=0;
		ub4 numcols = 0;
		ub4 collen=0;
		ub2 type = 0;
		sword status;
		int rescode=0;
		OCIStmt *stmthp;
		OCIBind  *bnd1p[4]; 
		OCIDefine* def[1];
		ub4  devtypelen=0;
		ub4 devlen=0;
		ub4  uuidlen=0;
		ub4 barcodelen=0;

		int portid=0;
		int status1=0;
		int nodeused=0;
		int len=0;
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
			sprintf(selectdev,"select portid from %s where deviceid=:devid and port_nbr=:pnb and nvl(port_type,'none')=:ptype and port_way=:pway",PORT_TABLE);
			//printf("Select statement: %s\n     with deviceid=%d, Port Number=%s porttype=%s PortWay=%d\n",selectdev, deviceID,port_nb,port_type, port_way);
			status=OCIStmtPrepare(stmthp, ociError, (text*) selectdev,(ub4) strlen(selectdev),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful 1");
		}
		else	
			status=OCIBindByName(stmthp, &bnd1p[0], ociError,(text*)":devid",-1,(dvoid*) &deviceID,sizeof(int), SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful 2");
		}
		else	
			status=OCIBindByName(stmthp, &bnd1p[1], ociError,(text*)":pnb",-1,(dvoid*) port_nb,strlen(port_nb)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful 3");
		}
		else	
			status=OCIBindByName(stmthp, &bnd1p[2], ociError,(text*)":ptype",-1,(dvoid*) port_type,strlen(port_type)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful 4");
		}
		else	
			status=OCIBindByName(stmthp, &bnd1p[3], ociError,(text*)":pway",-1,(dvoid*) &port_way,sizeof(int), SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIBindByPos unsuccessful 5");
		}
		else	
			status=OCIStmtExecute(ociHdbc, stmthp, ociError, 0, 0,(OCISnapshot *) 0, (OCISnapshot *) 0, OCI_DEFAULT);


		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "Execute unsuccessful");
			}
		}
		else
			status=OCIDefineByPos (stmthp, &def[0], ociError, 1,&portid,sizeof(portid), SQLT_INT, 0, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "DefineByPos1 unsuccessful");

			}
		}
		else
			status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT,1, OCI_DEFAULT);

		if (status==OCI_SUCCESS || status==OCI_SUCCESS_WITH_INFO) 
		{	
			portID=portid;
		} 
		else 
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "Fetch unsuccessful");
				strcpy(ErrorMessage,"NO_ROWS_SELECTED");
				GetErrorMess(appliName,"NO_ROWS_SELECTED",ErrorMessage,1);
				rescode=0; //means no data found
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
		int i = 0;
		int j=0;
		int pos1=0;
		int pos2=0;
		int pos3=0;
		int pos4=0;
		int free_mem=0;
		ub4 numcols = 0;
		ub4 collen=0;
		ub2 type = 0;
		sword status;
		int rescode=0;
		OCIStmt *stmthp;
		OCIBind  *bnd1p = (OCIBind *) 0; 
		OCIDefine* def[1];
		ub4  devtypelen=0;
		ub4 devlen=0;
		ub4  uuidlen=0;
		ub4 barcodelen=0;
		OCIParam *parmdp;
		int status1=0;
		int len=0;
		char * devicename_temp=NULL;

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
			sprintf(selectdev,"select devicename||'?' from %s where deviceid=:devid",LOGICAL_DEVICE_TABLE);
			status=OCIStmtPrepare(stmthp, ociError, (text*) selectdev,(ub4) strlen(selectdev),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else	
			status=OCIBindByName(stmthp, &bnd1p, ociError,(text*)":devid",-1,(dvoid*)&deviceID,sizeof(deviceID), SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIBindByPos unsuccessful");
		}
		else	
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
			{
				rescode=ShowErrors (status, ociError, "Execute unsuccessful");
			}
		}
		else
		{	
			devicename_temp=(char*)realloc(devicename_temp,(devlen+1)*sizeof(char));
			if(devicename_temp==NULL)
			{
				rescode=ShowErrors (-10, ociError, "Memory allocation failed ");
				status=-10;
				rescode=-1;

				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				GetErrorMess(appliName,"REALLOC UNSUCCESSFUL",ErrorMessage,1);
				return rescode;
			}
			else
				status=OCIDefineByPos (stmthp, &def[0], ociError, 1,devicename_temp,devlen+1, SQLT_STR, 0, 0, 0, OCI_DEFAULT);
		}
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "DefineByPos1 unsuccessful");

			}
		}
		else
			status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT, 1,OCI_DEFAULT);

		if (status==OCI_SUCCESS || status==OCI_SUCCESS_WITH_INFO) 
		{	

			pos1=strcspn(devicename_temp,"?");
			devicename_temp[pos1]='\0';
			strcpy(devicename,devicename_temp);

		} 
		else 
		{
			if(rescode==0)
			{
				rescode=0;
				strcpy(devicename,"NO_ROWS_SELECTED");
				GetErrorMess(appliName,"NO_ROWS_SELECTED",ErrorMessage,1);

			}
		}
		if(devicename_temp!=NULL)
			devicename_temp=(char*)realloc(devicename_temp,0*sizeof(char));

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
		int j=0;
		int rescode=0;
		ub4 numcols = 0;
		ub4 collen=0;
		ub2 type = 0;
		int pos1=0;
		int pos3=0;
		int actual_len=0;
		int pos2=0;
		int len_devname=0;
		char* devName=NULL;
		char* devIDs_temp=NULL;
		sword status;
		int nline=0;
		int nb_rdtrip=1;
		int prefetch_rows=2000;
		OCIStmt *stmthp;
		OCIBind  *bnd1p = (OCIBind *) 0; 
		OCIDefine* def;
		OCIParam *parmdp;
		char selectdevID[1000];
		status =OCIHandleAlloc (ociEnv, (void**)&stmthp, OCI_HTYPE_STMT , 0, 0);
		if(location==NULL)
		{
			GetErrorMess(appliName," location MUST be given",ErrorMessage,1);
			return -1;
		}
		if(status!=OCI_SUCCESS)
		{	
			rescode=ShowErrors (status, ociError, "OCIStmtHAndleAlloc unsuccessful");
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			else
				GetErrorMess(appliName,"NOT CONNECTED TO ANY DB",ErrorMessage,1);
			return -1;
		}
		else
		{
			sprintf(selectdevID,"select devicename||'|' from %s where instr(nvl(location,'none'),:location)>0",LOGICAL_DEVICE_TABLE);
			status=OCIStmtPrepare(stmthp, ociError, (text*) selectdevID,(ub4) strlen(selectdevID),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
			//std:://cout<<"stmt"<<selectdevID<<std::endl;
		}
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bnd1p, ociError,(text*)":location",-1,(dvoid*) location,strlen(location)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else
			status=	OCIAttrSet (stmthp,OCI_HTYPE_STMT,&prefetch_rows,0,OCI_ATTR_PREFETCH_ROWS,ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIAttrSet unsuccessful");
		}
		else
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
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &len_devname, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);



		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
		}
		else
		{
			devName=(char*)malloc((len_devname+1)*sizeof(char));
			if(devName!=NULL)
				status=OCIDefineByPos (stmthp, &def, ociError, 1,(ub1*) devName,len_devname+1, SQLT_STR, 0, 0, 0, OCI_DEFAULT);
			else
			{
				status=-10;
				rescode=ShowErrors (status, ociError, "pointer allocation unsuccessful");
				rescode=-1;
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				GetErrorMess(appliName,"REALLOC UNSUCCESSFUL",ErrorMessage,1);
				return rescode;	
			}
		}
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
		}
		else
		{


			pos2=0;
			i=0;
			while(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
			{
				status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT,1,OCI_DEFAULT);
				if(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
				{
					pos1=strcspn(devName,"|");
					devName[pos1]='\0';
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
					devIDs_temp=(char*) realloc(devIDs_temp,0*sizeof(char));
					rescode=-1;
					free(devName);
					status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
					GetErrorMess(appliName,"BUFFER_TOO_SMALL",ErrorMessage,1);
					return rescode;
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
				devIDs_temp=(char*) realloc(devIDs_temp,0*sizeof(char));
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
		int j=0;
		int rescode=0;
		ub4 numcols = 0;
		ub4 collen=0;
		ub2 type = 0;
		int pos1=0;
		int pos3=0;
		int actual_len=0;
		int pos2=0;
		int len_devname=0;
		char* devName=NULL;
		char* devIDs_temp=NULL;
		sword status;
		int nline=0;
		int nb_rdtrip=1;
		int prefetch_rows=2000;
		OCIStmt *stmthp;
		OCIBind  *bnd1p = (OCIBind *) 0; 
		OCIDefine* def;
		OCIParam *parmdp;
		char selectdevID[1000];
		status =OCIHandleAlloc (ociEnv, (void**)&stmthp, OCI_HTYPE_STMT , 0, 0);
		if(function==NULL)
		{
			GetErrorMess(appliName," function MUST be given",ErrorMessage,1);
			return -1;
		}
		if(status!=OCI_SUCCESS)
		{	
			rescode=ShowErrors (status, ociError, "OCIStmtHAndleAlloc unsuccessful");
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			else
				GetErrorMess(appliName,"NOT CONNECTED TO ANY DB",ErrorMessage,1);
			return -1;
		}
		else
		{
			if(strstr(function,"none")==NULL)
				sprintf(selectdevID,"select t.devicename||'|' from %s t , %s e where mod(t.functionid,e.functionid)=0 and e.function_name=:fct and t.functionID!=0 ",LOGICAL_DEVICE_TABLE,DEVICE_FUNCTION_TABLE);
			else
				sprintf(selectdevID," select t.devicename||'|' from %s t where functionid=0",LOGICAL_DEVICE_TABLE);
			status=OCIStmtPrepare(stmthp, ociError, (text*) selectdevID,(ub4) strlen(selectdevID),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
			//std:://cout<<"stmt"<<selectdevID<<std::endl;
		}
		if(strstr(function,"none")==NULL)
		{
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)
					rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
			}
			else
				status=OCIBindByName(stmthp, &bnd1p, ociError,(text*)":fct",-1,(dvoid*) function,strlen(function)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);
		}
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else
			status=	OCIAttrSet (stmthp,OCI_HTYPE_STMT,&prefetch_rows,0,OCI_ATTR_PREFETCH_ROWS,ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIAttrSet unsuccessful");
		}
		else
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
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &len_devname, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);



		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
		}
		else
		{
			devName=(char*)malloc((len_devname+1)*sizeof(char));
			if(devName!=NULL)
				status=OCIDefineByPos (stmthp, &def, ociError, 1,(ub1*) devName,len_devname+1, SQLT_STR, 0, 0, 0, OCI_DEFAULT);
			else
			{
				status=-10;
				rescode=ShowErrors (status, ociError, "pointer allocation unsuccessful");
				rescode=-1;
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				GetErrorMess(appliName,"REALLOC UNSUCCESSFUL",ErrorMessage,1);
				return rescode;	
			}
		}
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
		}
		else
		{


			pos2=0;
			i=0;
			while(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
			{
				status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT,1,OCI_DEFAULT);
				if(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
				{
					pos1=strcspn(devName,"|");
					devName[pos1]='\0';
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
			//std:://cout<<"devName="<<devName<<std::endl;
			//std:://cout<<"pos2="<<pos2<<std::endl;
			//std:://cout<<"i="<<i<<std::endl;
			//std:://cout<<"len_array="<<len_array<<std::endl;
			if(rescode==0)
			{	

				if(pos2>len_array)
				{
					len_array=pos2;
					devIDs_temp=(char*) realloc(devIDs_temp,0*sizeof(char));
					rescode=-1;
					free(devName);
					status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
					GetErrorMess(appliName,"BUFFER_TOO_SMALL",ErrorMessage,1);
					return rescode;
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
				devIDs_temp=(char*) realloc(devIDs_temp,0*sizeof(char));
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
		int j=0;
		int rescode=0;
		ub4 numcols = 0;
		ub4 collen=0;
		ub2 type = 0;
		int pos1=0;
		int pos3=0;
		int actual_len=0;
		int pos2=0;
		int len_devname=0;
		char* devName=NULL;
		char* devIDs_temp=NULL;
		sword status;
		int nline=0;
		int nb_rdtrip=1;
		int prefetch_rows=2000;
		OCIStmt *stmthp;
		OCIBind  *bnd1p = (OCIBind *) 0; 
		OCIDefine* def;
		OCIParam *parmdp;
		char selectdevID[1000];
		status =OCIHandleAlloc (ociEnv, (void**)&stmthp, OCI_HTYPE_STMT , 0, 0);
		if(location==NULL)
		{
			GetErrorMess(appliName," location MUST be given",ErrorMessage,1);
			return -1;
		}
		if(status!=OCI_SUCCESS)
		{	
			rescode=ShowErrors (status, ociError, "OCIStmtHAndleAlloc unsuccessful");
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			else
				GetErrorMess(appliName,"NOT CONNECTED TO ANY DB",ErrorMessage,1);
			return -1;
		}
		else
		{
			sprintf(selectdevID,"select serialnb||'|'||nvl(hwname,'none')||'?' from %s where nvl(location,'none')=:location and device_status=%d",HW_DEVICE_TABLE,SPARE);
			status=OCIStmtPrepare(stmthp, ociError, (text*) selectdevID,(ub4) strlen(selectdevID),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
			//std:://cout<<"stmt"<<selectdevID<<std::endl;
		}
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bnd1p, ociError,(text*)":location",-1,(dvoid*) location,strlen(location)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else
			status=	OCIAttrSet (stmthp,OCI_HTYPE_STMT,&prefetch_rows,0,OCI_ATTR_PREFETCH_ROWS,ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIAttrSet unsuccessful");
		}
		else
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
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &len_devname, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
		}
		else
		{
			devName=(char*)malloc((len_devname+1)*sizeof(char));
			if(devName!=NULL)
				status=OCIDefineByPos (stmthp, &def, ociError, 1,(ub1*) devName,len_devname+1, SQLT_STR, 0, 0, 0, OCI_DEFAULT);
			else
			{
				status=-10;
				rescode=ShowErrors (status, ociError, "pointer allocation unsuccessful");
				rescode=-1;
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				GetErrorMess(appliName,"REALLOC UNSUCCESSFUL",ErrorMessage,1);
				return rescode;	
			}
		}
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
		}
		else
		{


			pos2=0;
			i=0;
			while(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
			{
				status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT,1,OCI_DEFAULT);
				if(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
				{
					pos1=strcspn(devName,"?");
					devName[pos1]='\0';
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
					devIDs_temp=(char*) realloc(devIDs_temp,0*sizeof(char));
					rescode=-1;
					free(devName);
					status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
					GetErrorMess(appliName,"BUFFER_TOO_SMALL",ErrorMessage,1);
					return rescode;
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
				devIDs_temp=(char*) realloc(devIDs_temp,0*sizeof(char));
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
		int j=0;
		int rescode=0;
		ub4 numcols = 0;
		ub4 collen=0;
		ub2 type = 0;
		int pos1=0;
		int pos3=0;
		int actual_len=0;
		int pos2=0;
		int len_devname=0;
		char* devName=NULL;
		char* devIDs_temp=NULL;
		sword status;
		int nline=0;
		int nb_rdtrip=1;
		int prefetch_rows=2000;
		OCIStmt *stmthp;
		OCIBind  *bnd1p = (OCIBind *) 0; 
		OCIDefine* def;
		OCIParam *parmdp;
		char selectdevID[1000];
		status =OCIHandleAlloc (ociEnv, (void**)&stmthp, OCI_HTYPE_STMT , 0, 0);
		if(hwtype==NULL)
		{
			GetErrorMess(appliName," hwtype MUST be given",ErrorMessage,1);
			return -1;
		}
		if(status!=OCI_SUCCESS)
		{	
			rescode=ShowErrors (status, ociError, "OCIStmtHAndleAlloc unsuccessful");
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			else
				GetErrorMess(appliName,"NOT CONNECTED TO ANY DB",ErrorMessage,1);
			return -1;
		}
		else
		{
			sprintf(selectdevID,"select serialnb||'|'||nvl(hwname,'none')||'?' from %s where nvl(hwtype,'none')=:hwtype and device_status=%d",HW_DEVICE_TABLE, SPARE);
			status=OCIStmtPrepare(stmthp, ociError, (text*) selectdevID,(ub4) strlen(selectdevID),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
			//std:://cout<<"stmt"<<selectdevID<<std::endl;
		}
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bnd1p, ociError,(text*)":hwtype",-1,(dvoid*) hwtype,strlen(hwtype)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else
			status=	OCIAttrSet (stmthp,OCI_HTYPE_STMT,&prefetch_rows,0,OCI_ATTR_PREFETCH_ROWS,ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIAttrSet unsuccessful");
		}
		else
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
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &len_devname, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
		}
		else
		{
			devName=(char*)malloc((len_devname+1)*sizeof(char));
			if(devName!=NULL)
				status=OCIDefineByPos (stmthp, &def, ociError, 1,(ub1*) devName,len_devname+1, SQLT_STR, 0, 0, 0, OCI_DEFAULT);
			else
			{
				status=-10;
				rescode=ShowErrors (status, ociError, "pointer allocation unsuccessful");
				rescode=-1;
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				GetErrorMess(appliName,"REALLOC UNSUCCESSFUL",ErrorMessage,1);
				return rescode;	
			}
		}
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
		}
		else
		{


			pos2=0;
			i=0;
			while(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
			{
				status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT,1,OCI_DEFAULT);
				if(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
				{
					pos1=strcspn(devName,"?");
					devName[pos1]='\0';
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
					devIDs_temp=(char*) realloc(devIDs_temp,0*sizeof(char));
					rescode=-1;
					free(devName);
					status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
					GetErrorMess(appliName,"BUFFER_TOO_SMALL",ErrorMessage,1);
					return rescode;
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
				devIDs_temp=(char*) realloc(devIDs_temp,0*sizeof(char));
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
		int i = 0;
		int j=0;
		int pos1=0;
		int pos2=0;
		int pos3=0;
		int pos4=0;
		int pos5=0;
		int pos6=0;
		int pos7=0;
		int rescode1=0;
		int free_mem=0;
		ub4 numcols = 0;
		ub4 collen=0;
		ub2 type = 0;
		sword status;
		int rescode=0;
		OCIStmt *stmthp;
		OCIBind  *bnd1p = (OCIBind *) 0; 
		OCIBind  *bnd2p = (OCIBind *) 0;
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
		int len=0;
		char * hwname_temp=NULL;
		char * hwtype_temp=NULL;
		char * location_temp=NULL;
		char * serialnb_temp=NULL;
		char * responsible_temp=NULL;
		char * comments_temp=NULL;
		char * dname_temp=NULL;
		int dname_null=0;
		int dtype_null=0;
		int serialnb_null=0;
		int location_null=0;
		int dcomments_null=0;
		int dresponsible_null=0;
		int hwname_null=0;
		char logmessage[100];
		char device_status[20];
		if(serialnb==NULL)
		{
			GetErrorMess(appliName,"serialnb MUST be given",ErrorMessage,1);
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

			sprintf(selectdev," select nvl(t.hwname,'none')||'?',nvl(t.hwtype,'none')||'?',t.device_status,nvl(t.responsible,'none')||'?',nvl(t.user_comments,'none')||'?',nvl(t.location,'none')||'?',t.serialnb||'?',d.devicename||'?' from %s t,%s d where t.serialnb=:dname and t.serialnb=d.serialnb",HW_DEVICE_TABLE,LOGICAL_DEVICE_TABLE);
			status=OCIStmtPrepare(stmthp, ociError, (text*) selectdev,(ub4) strlen(selectdev),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
			//std:://cout<<" select stmt "<< selectdev <<std::endl;
			//std:://cout<<" serialnb "<< serialnb <<std::endl;
		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else	
			status=OCIBindByName(stmthp, &bnd1p, ociError,(text*)":dname",-1,(dvoid*)serialnb,strlen(serialnb)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);


		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIBindByPos unsuccessful");
		}
		else	
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

		//std:://cout<<" param " <<std::endl;
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
				rescode=ShowErrors (status, ociError, "OCIParamGet1 unsuccessful");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &devtypelen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);


		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIAttrGet1 unsuccessful");
		}
		else
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 4);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet2 unsuccessful");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &responlen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIAttrGet1 unsuccessful");
		}
		else
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 5);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet2 unsuccessful");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &commentslen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIAttrGet1 unsuccessful");
		}
		else
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 6);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet2 unsuccessful");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &serialnblen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);

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
				rescode=ShowErrors (status, ociError, "OCIAttrGet2 unsuccessful");
		}
		else
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 8);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet3 unsuccessful");
		}
		else 
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &dnamelen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);


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
			MinStringLength(responlen);
			MinStringLength(commentslen);
			MinStringLength(dnamelen);
			//std:://cout << "devtype_len" << devtypelen<< std::endl;
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
				rescode=-1;
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
				return rescode;

			}
			else
				status =OCIDefineByPos(stmthp, &def[0], ociError,1, (ub1 *) (hwname_temp), devlen + 1,SQLT_STR, (dvoid *) &hwname_null,(ub2 *) 0,0, OCI_DEFAULT);
		}
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "DefineByPos0 unsuccessful");

			}
		}
		else
			status=OCIDefineByPos (stmthp, &def[1], ociError, 2,hwtype_temp,devtypelen+1, SQLT_STR, (dvoid *) &dtype_null,0, 0,OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "DefineByPos1 unsuccessful");

			}
		}
		else
			status=OCIDefineByPos (stmthp, &def[2], ociError, 3,&status1,sizeof(status1), SQLT_INT, 0, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "DefineByPos2 unsuccessful");
			}
		}
		else
			status=OCIDefineByPos (stmthp, &def[3], ociError, 4,responsible_temp,responlen+1, SQLT_STR, (dvoid *) &dresponsible_null, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "DefineByPos3 unsuccessful");
			}
		}
		else
			status=OCIDefineByPos (stmthp, &def[4], ociError, 5,comments_temp,commentslen+1, SQLT_STR, (dvoid *) &dcomments_null, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "DefineByPos4 unsuccessful");
			}
		}
		else
			status=OCIDefineByPos (stmthp, &def[5], ociError,6,location_temp,locationlen+1, SQLT_STR, (dvoid *) &location_null, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "DefineByPos5 unsuccessful");
			}
		}
		else
			status=OCIDefineByPos (stmthp, &def[6], ociError, 7,serialnb_temp,serialnblen+1, SQLT_STR, (dvoid *) &serialnb_null,0, 0,OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "DefineByPos6 unsuccessful");

			}
		}
		else	
			status=OCIDefineByPos (stmthp, &def[7], ociError,8,dname_temp,dnamelen+1, SQLT_STR, (dvoid *) &dname_null, 0, 0, OCI_DEFAULT);



		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "DefineByPos8 unsuccessful");

			}
		}
		else
			status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT,1,OCI_DEFAULT);

		if (status==OCI_SUCCESS || status==OCI_SUCCESS_WITH_INFO) 
		{	
			Format_output(serialnb_null,serialnb_temp, logmessage,'?');
			Format_output(dtype_null,hwtype_temp,logmessage,'?');
			Format_output(location_null,location_temp,logmessage,'?');
			Format_output(hwname_null,hwname_temp,logmessage,'?');
			Format_output(dname_null,dname_temp,logmessage,'?');
			Format_output(dresponsible_null,responsible_temp,logmessage,'?');
			Format_output(dcomments_null,comments_temp,logmessage,'?');
			pos1=strcspn(dname_temp,"?");
			pos2=strcspn(hwtype_temp,"?");
			pos3=strcspn(location_temp,"?");
			pos4=strcspn(serialnb_temp,"?");
			pos5=strcspn(hwname_temp,"?");
			pos6=strcspn(comments_temp,"?");
			pos7=strcspn(responsible_temp,"?");
			dname_temp[pos1]='\0';
			hwtype_temp[pos2]='\0';
			location_temp[pos3]='\0';
			serialnb_temp[pos4]='\0';
			hwname_temp[pos5]='\0';
			comments_temp[pos6]='\0';
			responsible_temp[pos7]='\0';
			rescode1=GetDeviceStatus(status1, device_status);
			len=strlen(device_status)+strlen(responsible_temp)+strlen(hwtype_temp)+strlen(dname_temp)+strlen(hwname_temp)+strlen(location_temp)+strlen(serialnb_temp)+strlen(comments_temp)+strlen("|hwname (C): |hwtype (C): |device_status (C): |location (C): |user_comments  (C): |responsible (C): |devicename (C): |serialnb  (C): |");


			if(len_device<len)
			{
				len++;
				len_device=len;
				GetErrorMess(appliName, "BUFFER_TOO_SMALL",ErrorMessage,1);
				if(hwtype_temp!=NULL)
					hwtype_temp = (char *) realloc(hwtype_temp,(0)*sizeof(char));
				if(hwname_temp!=NULL)
					hwname_temp= (char *) realloc(hwname_temp,(0)*sizeof(char));
				if(dname_temp!=NULL)
					dname_temp= (char *) realloc(dname_temp,(0)*sizeof(char));
				if(serialnb_temp!=NULL)
					serialnb_temp = (char *) realloc(serialnb_temp,(0)*sizeof(char));
				if(location_temp!=NULL)
					location_temp= (char *) realloc(location_temp,(0)*sizeof(char));
				if(responsible_temp!=NULL)
					responsible_temp= (char *) realloc(responsible_temp,(0)*sizeof(char));
				if(comments_temp!=NULL)
					comments_temp= (char *) realloc(comments_temp,(0)*sizeof(char));
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				return -1;

			}
			else
				sprintf(device_result,"|hwname (C):%s |hwtype (C):%s |device_status (C):%s |location (C):%s |responsible  (C):%s |user_comments (C):%s |devicename (C):%s |serialnb  (C):%s |" ,hwname_temp,hwtype_temp,device_status,location_temp,responsible_temp,comments_temp,dname_temp,serialnb_temp);


		} 
		else 
		{
			if(rescode==0)
			{
				sprintf(selectdev," select nvl(hwname,'none')||'?',nvl(hwtype,'none')||'?',device_status,nvl(responsible,'none')||'?',nvl(user_comments,'none')||'?',nvl(location,'none')||'?',serialnb||'?' from %s where serialnb=:dname",HW_DEVICE_TABLE);
				status=OCIStmtPrepare(stmthp, ociError, (text*) selectdev,(ub4) strlen(selectdev),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
				//std:://cout<<"selectdev "<<selectdev <<std::endl;
				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)
						rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
				}
				else	
					status=OCIBindByName(stmthp, &bnd1p, ociError,(text*)":dname",-1,(dvoid*)serialnb,strlen(serialnb)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);


				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)
						rescode=ShowErrors (status, ociError, "OCIPrepare unsuccessful");
				}
				else	
					status=OCIStmtExecute(ociHdbc, stmthp, ociError, 0, 0,(OCISnapshot *) 0, (OCISnapshot *) 0, OCI_DEFAULT);
				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)
						rescode=ShowErrors (status, ociError, "OCIExecute2 unsuccessful");
				}
				else	
					status =OCIDefineByPos(stmthp, &def[0], ociError,1, (ub1 *) (hwname_temp), devlen + 1,SQLT_STR, (dvoid *) &hwname_null,(ub2 *) 0,0, OCI_DEFAULT);
				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)
					{
						rescode=ShowErrors (status, ociError, "DefineByPos0 unsuccessful");

					}
				}
				else
					status=OCIDefineByPos (stmthp, &def[1], ociError, 2,hwtype_temp,devtypelen+1, SQLT_STR, (dvoid *) &dtype_null,0, 0,OCI_DEFAULT);

				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)
					{
						rescode=ShowErrors (status, ociError, "DefineByPos1 unsuccessful");

					}
				}
				else
					status=OCIDefineByPos (stmthp, &def[2], ociError, 3,&status1,sizeof(status1), SQLT_INT, 0, 0, 0, OCI_DEFAULT);

				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)
					{
						rescode=ShowErrors (status, ociError, "DefineByPos2 unsuccessful");
					}
				}
				else
					status=OCIDefineByPos (stmthp, &def[3], ociError, 4,responsible_temp,responlen+1, SQLT_STR, (dvoid *) &dresponsible_null, 0, 0, OCI_DEFAULT);

				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)
					{
						rescode=ShowErrors (status, ociError, "DefineByPos3 unsuccessful");
					}
				}
				else
					status=OCIDefineByPos (stmthp, &def[4], ociError, 5,comments_temp,commentslen+1, SQLT_STR, (dvoid *) &dcomments_null, 0, 0, OCI_DEFAULT);

				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)
					{
						rescode=ShowErrors (status, ociError, "DefineByPos4 unsuccessful");
					}
				}
				else
					status=OCIDefineByPos (stmthp, &def[5], ociError,6,location_temp,locationlen+1, SQLT_STR, (dvoid *) &location_null, 0, 0, OCI_DEFAULT);

				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)
					{
						rescode=ShowErrors (status, ociError, "DefineByPos5 unsuccessful");
					}
				}
				else
					status=OCIDefineByPos (stmthp, &def[6], ociError, 7,serialnb_temp,serialnblen+1, SQLT_STR, (dvoid *) &serialnb_null,0, 0,OCI_DEFAULT);

				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)
					{
						rescode=ShowErrors (status, ociError, "DefineByPos8 unsuccessful");
					}
				}
				else
					status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT,1,OCI_DEFAULT);

				if (status==OCI_SUCCESS || status==OCI_SUCCESS_WITH_INFO) 
				{	
					Format_output(serialnb_null,serialnb_temp, logmessage,'?');
					Format_output(dtype_null,hwtype_temp,logmessage,'?');
					Format_output(location_null,location_temp,logmessage,'?');
					Format_output(hwname_null,hwname_temp,logmessage,'?');
					Format_output(dresponsible_null,responsible_temp,logmessage,'?');
					Format_output(dcomments_null,comments_temp,logmessage,'?');

					pos2=strcspn(hwtype_temp,"?");
					pos3=strcspn(location_temp,"?");
					pos4=strcspn(serialnb_temp,"?");
					pos5=strcspn(hwname_temp,"?");
					pos6=strcspn(comments_temp,"?");
					pos7=strcspn(responsible_temp,"?");

					hwtype_temp[pos2]='\0';
					location_temp[pos3]='\0';
					serialnb_temp[pos4]='\0';
					hwname_temp[pos5]='\0';
					comments_temp[pos6]='\0';
					responsible_temp[pos7]='\0';
					rescode1=GetDeviceStatus(status1, device_status);
					len=strlen(device_status)+strlen(responsible_temp)+strlen(hwtype_temp)+strlen(hwname_temp)+strlen(location_temp)+strlen(serialnb_temp)+strlen(comments_temp)+strlen("|hwname (C): |hwtype (C): |device_status (C): |location (C): |user_comments  (C): |responsible (C): |devicename (C): none |serialnb  (C): |");


					if(len_device<len)
					{
						len++;
						len_device=len;
						GetErrorMess(appliName, "BUFFER_TOO_SMALL",ErrorMessage,1);
						if(hwtype_temp!=NULL)
							hwtype_temp = (char *) realloc(hwtype_temp,(0)*sizeof(char));
						if(hwname_temp!=NULL)
							hwname_temp= (char *) realloc(hwname_temp,(0)*sizeof(char));

						if(serialnb_temp!=NULL)
							serialnb_temp = (char *) realloc(serialnb_temp,(0)*sizeof(char));
						if(location_temp!=NULL)
							location_temp= (char *) realloc(location_temp,(0)*sizeof(char));
						if(responsible_temp!=NULL)
							responsible_temp= (char *) realloc(responsible_temp,(0)*sizeof(char));
						if(comments_temp!=NULL)
							comments_temp= (char *) realloc(comments_temp,(0)*sizeof(char));
						status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
						return -1;

					}
					else
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
			hwtype_temp = (char *) realloc(hwtype_temp,(0)*sizeof(char));
		if(hwname_temp!=NULL)
			hwname_temp= (char *) realloc(hwname_temp,(0)*sizeof(char));
		if(dname_temp!=NULL)
			dname_temp= (char *) realloc(dname_temp,(0)*sizeof(char));
		if(serialnb_temp!=NULL)
			serialnb_temp = (char *) realloc(serialnb_temp,(0)*sizeof(char));
		if(location_temp!=NULL)
			location_temp= (char *) realloc(location_temp,(0)*sizeof(char));
		if(responsible_temp!=NULL)
			responsible_temp= (char *) realloc(responsible_temp,(0)*sizeof(char));
		if(comments_temp!=NULL)
			comments_temp= (char *) realloc(comments_temp,(0)*sizeof(char));

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
		int j=0;
		int rescode=0;
		ub4 numcols = 0;
		ub4 collen=0;
		ub2 type = 0;
		int pos1=0;
		int pos3=0;
		int actual_len=0;
		int pos2=0;
		int len_devname=0;
		char* devName=NULL;
		char* devIDs_temp=NULL;
		sword status;
		int nline=0;
		int nb_rdtrip=1;
		int prefetch_rows=2000;
		OCIStmt *stmthp;
		OCIBind  *bnd1p = (OCIBind *) 0; 
		OCIDefine* def;
		OCIParam *parmdp;
		char selectdevID[1000];
		status =OCIHandleAlloc (ociEnv, (void**)&stmthp, OCI_HTYPE_STMT , 0, 0);
		if(ipname==NULL)
		{
			GetErrorMess(appliName," location MUST be given",ErrorMessage,1);
			return -1;
		}
		if(status!=OCI_SUCCESS)
		{	
			rescode=ShowErrors (status, ociError, "OCIStmtHAndleAlloc unsuccessful");
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			else
				GetErrorMess(appliName,"NOT CONNECTED TO ANY DB",ErrorMessage,1);
			return -1;
		}
		else
		{
			sprintf(selectdevID,"select t.ipalias||'|' from %s t , %s d where t.ipaddress=d.ipaddress and d.ipname=:ipname",IPALIAS_TABLE,IPINFO_TABLE);
			status=OCIStmtPrepare(stmthp, ociError, (text*) selectdevID,(ub4) strlen(selectdevID),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
			//std:://cout<<"stmt"<<selectdevID<<std::endl;
		}
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bnd1p, ociError,(text*)":ipname",-1,(dvoid*) ipname,strlen(ipname)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else
			status=	OCIAttrSet (stmthp,OCI_HTYPE_STMT,&prefetch_rows,0,OCI_ATTR_PREFETCH_ROWS,ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIAttrSet unsuccessful");
		}
		else
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
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &len_devname, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);



		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
		}
		else
		{
			devName=(char*)malloc((len_devname+1)*sizeof(char));
			if(devName!=NULL)
				status=OCIDefineByPos (stmthp, &def, ociError, 1,(ub1*) devName,len_devname+1, SQLT_STR, 0, 0, 0, OCI_DEFAULT);
			else
			{
				status=-10;
				rescode=ShowErrors (status, ociError, "pointer allocation unsuccessful");
				rescode=-1;
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				GetErrorMess(appliName,"REALLOC UNSUCCESSFUL",ErrorMessage,1);
				return rescode;	
			}
		}
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
		}
		else
		{


			pos2=0;
			i=0;
			while(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
			{
				status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT,1,OCI_DEFAULT);
				if(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
				{
					pos1=strcspn(devName,"|");
					devName[pos1]='\0';
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
					devIDs_temp=(char*) realloc(devIDs_temp,0*sizeof(char));
					rescode=-1;
					free(devName);
					status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
					GetErrorMess(appliName,"BUFFER_TOO_SMALL",ErrorMessage,1);
					return rescode;
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
				devIDs_temp=(char*) realloc(devIDs_temp,0*sizeof(char));
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
		int j=0;
		int rescode=0;
		ub4 numcols = 0;
		ub4 collen=0;
		ub2 type = 0;
		int pos1=0;
		int pos3=0;
		int actual_len=0;
		int pos2=0;
		int len_devname=0;
		char* devName=NULL;
		char* devIDs_temp=NULL;
		sword status;
		int nline=0;
		int nb_rdtrip=1;
		int prefetch_rows=2000;
		OCIStmt *stmthp;
		OCIDefine* def;
		OCIParam *parmdp;
		char selectdevID[1000];
		status =OCIHandleAlloc (ociEnv, (void**)&stmthp, OCI_HTYPE_STMT , 0, 0);

		if(status!=OCI_SUCCESS)
		{	
			rescode=ShowErrors (status, ociError, "OCIStmtHAndleAlloc unsuccessful");
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			else
				GetErrorMess(appliName,"NOT CONNECTED TO ANY DB",ErrorMessage,1);
			return -1;
		}
		else
		{
			sprintf(selectdevID,"select function_name||'|' from %s",DEVICE_FUNCTION_TABLE);
			status=OCIStmtPrepare(stmthp, ociError, (text*) selectdevID,(ub4) strlen(selectdevID),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
			//std:://cout<<"stmt"<<selectdevID<<std::endl;
		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else
			status=	OCIAttrSet (stmthp,OCI_HTYPE_STMT,&prefetch_rows,0,OCI_ATTR_PREFETCH_ROWS,ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIAttrSet unsuccessful");
		}
		else
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
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &len_devname, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);



		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
		}
		else
		{
			devName=(char*)malloc((len_devname+1)*sizeof(char));
			if(devName!=NULL)
				status=OCIDefineByPos (stmthp, &def, ociError, 1,(ub1*) devName,len_devname+1, SQLT_STR, 0, 0, 0, OCI_DEFAULT);
			else
			{
				status=-10;
				rescode=ShowErrors (status, ociError, "pointer allocation unsuccessful");
				rescode=-1;
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				GetErrorMess(appliName,"REALLOC UNSUCCESSFUL",ErrorMessage,1);
				return rescode;	
			}
		}
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
		}
		else
		{


			pos2=0;
			i=0;
			while(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
			{
				status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT,1,OCI_DEFAULT);
				if(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
				{
					pos1=strcspn(devName,"|");
					devName[pos1]='\0';
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
					devIDs_temp=(char*) realloc(devIDs_temp,0*sizeof(char));
					rescode=-1;
					free(devName);
					status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
					GetErrorMess(appliName,"BUFFER_TOO_SMALL",ErrorMessage,1);
					return rescode;
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
				devIDs_temp=(char*) realloc(devIDs_temp,0*sizeof(char));
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
		int i = 0;
		int j=0;
		int pos1=0;
		int pos2=0;
		int len1=0;
		int pos3=0;
		int pos4=0;
		int rescode=0;
		int free_mem=0;
		ub4 numcols = 0;
		ub4 collen=0;
		ub2 type = 0;
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
		int ipalias_null=0;
		int ipaddress_null=0;
		int ipname_null=0;
		int subnet_null=0;

		char logmessage[100];
		int res1=0;

		if(ipalias==NULL)
		{
			GetErrorMess(appliName," ipalias MUST be given",ErrorMessage,1);
			return -1;
		}
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
			char selectdevtype[1000];
			sprintf(selectdevtype,"select t.ipaddress||'?',t.ipalias||'?',e.ipname||'?',e.subnet_info||'?' from %s t,%s e where t.ipalias=:ipalias and t.ipaddress=e.ipaddress ",IPALIAS_TABLE,IPINFO_TABLE);
			status=OCIStmtPrepare(stmthp, ociError, (text*) selectdevtype,(ub4) strlen(selectdevtype),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
		}

		if(status!=OCI_SUCCESS )
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
			}
		}
		else
			status=OCIBindByName(stmthp, &bnd1p, ociError,(text*)":ipalias",-1,(dvoid*) ipalias,strlen(ipalias)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "OCIBindByPos unsuccessful");
			}
		}
		else
			status=OCIStmtExecute(ociHdbc, stmthp, ociError, 0, 0,(OCISnapshot *) 0, (OCISnapshot *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
			}
		}
		else
		{
			/* The next two statements describe the select-list item, dept, and return its length to allocate the memory*/
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 2);
		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "OCIParamGet1 unsuccessful");
			}
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &ipalias_len, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);
		//std:://cout<<"devtypelen="<<devtypelen<<std::endl;	

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "OCIAttrGet1 unsuccessful");
			}
		}
		else 
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT,ociError,(dvoid **) &parmdp, (ub4) 3);
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "OCIParamGet2 unsuccessful");
			}
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &ipname_len, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);
		//std:://cout<<"descriptionlen="<<descriptionlen<<std::endl;	
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{

				rescode=ShowErrors (status, ociError, "OCIAttrGet2 unsuccessful");
			}
		}
		else
		{
			/* Use the retrieved length of dept to allocate an output buffer, and then define the output variable. If the define call returns an error,exit the application */
			MinStringLength(ipalias_len); //case it will be a null value need a buffer bigger than 5
			MinStringLength(ipname_len);

			ipname_temp = (char *) realloc(ipname_temp,(ipname_len + 1)*sizeof(char));
			ipalias_temp= (char *) realloc(ipalias_temp,(ipalias_len + 1)*sizeof(char));
			ipaddress_temp= (char *) realloc(ipaddress_temp,(ipadd_len + 1)*sizeof(char));
			subnet_temp= (char *) realloc(subnet_temp,(ipadd_len + 1)*sizeof(char));
			if(subnet_temp==NULL || ipaddress_temp==NULL ||ipalias_temp==NULL || ipname_temp==NULL)
			{
				status=-10;
				rescode=ShowErrors (status, ociError, "  pointer allocation unsuccessful");
				rescode=-1;
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
				return rescode;
			}
			else
				status =OCIDefineByPos(stmthp, &def[0], ociError,1, (ub1 *) (ipaddress_temp), ipadd_len + 1,SQLT_STR, (dvoid *)&ipaddress_null,(ub2 *) 0,0, OCI_DEFAULT);
		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{

				rescode=ShowErrors (status, ociError, "OCIDefineByPos0 unsuccessful");		
			}
		}
		else
			status=OCIDefineByPos (stmthp, &def[1],ociError, 2,ipalias_temp,ipalias_len, SQLT_STR, &ipalias_null, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{

				rescode=ShowErrors (status, ociError, "OCIDefineByPos0 unsuccessful");		
			}
		}
		else
			status=OCIDefineByPos (stmthp, &def[2],ociError, 3,ipname_temp,ipname_len, SQLT_STR, &ipname_null, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "OCIDefineByPos1 unsuccessful");

			}
		}
		else
			status=OCIDefineByPos (stmthp, &def[3], ociError, 4,subnet_temp,ipadd_len, SQLT_STR, &subnet_null, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "OCIDefineByPos4 unsuccessful");
			}
		}
		else
			status =OCIStmtFetch2(stmthp,ociError, 1, OCI_FETCH_NEXT,1, OCI_DEFAULT);

		if (status == OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO) 
		{	
			Format_output(ipaddress_null,ipaddress_temp,  logmessage,'?');
			Format_output(ipname_null,ipname_temp, logmessage,'?');
			Format_output(ipalias_null,ipalias_temp, logmessage,'?');
			Format_output(subnet_null,subnet_temp, logmessage,'?');

			//case where value has been truncated
			pos1=strcspn(ipaddress_temp,"?");
			pos2=strcspn(ipname_temp,"?");
			pos3=strcspn(ipalias_temp,"?");
			pos4=strcspn(subnet_temp,"?");
			ipaddress_temp[pos1]='\0';
			ipname_temp[pos2]='\0';
			ipalias_temp[pos3]='\0';
			subnet_temp[pos4]='\0';
			len1=strlen("|ipalias (C): |ipaddress (C): |ipname (C): |subnet_mask (C): |")+strlen(ipname_temp)+strlen(ipalias_temp)+strlen(ipaddress_temp)+strlen(subnet_temp);

			if(len_ipalias<len1)
			{	
				len1++;
				len_ipalias=len1;
				GetErrorMess(appliName, "BUFFER_TOO_SMALL",ErrorMessage,1);
				if(ipalias_temp!=NULL)
					ipalias_temp = (char *) realloc(ipalias_temp,(0)*sizeof(char));
				if(subnet_temp!=NULL)
					subnet_temp= (char *) realloc(subnet_temp,(0)*sizeof(char));
				if(ipname_temp!=NULL)
					ipname_temp= (char *) realloc(ipname_temp,(0)*sizeof(char));
				if(ipaddress_temp!=NULL)
					ipaddress_temp= (char *) realloc(ipaddress_temp,(0)*sizeof(char));
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				return -1;


			}
			else
			{
				sprintf(ipalias_result,"|ipalias (C):%s |ipaddress (C):%s |ipname (C):%s |subnet_mask (C):%s |",ipalias_temp,ipaddress_temp,ipname_temp,subnet_temp);
			}

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
			ipalias_temp = (char *) realloc(ipalias_temp,(0)*sizeof(char));
		if(subnet_temp!=NULL)
			subnet_temp= (char *) realloc(subnet_temp,(0)*sizeof(char));
		if(ipname_temp!=NULL)
			ipname_temp= (char *) realloc(ipname_temp,(0)*sizeof(char));
		if(ipaddress_temp!=NULL)
			ipaddress_temp= (char *) realloc(ipaddress_temp,(0)*sizeof(char));
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
		ub4 numcols = 0;
		ub4 collen=0;
		ub2 type = 0;
		int lkID=0;
		int* lkIDs_temp=NULL;
		sword status;
		int nline=0;
		int nb_rdtrip=1;
		int prefetch_rows=2000;
		OCIStmt *stmthp;
		OCIBind  *bnd1p = (OCIBind *) 0; 
		OCIDefine* def;
		char selectdevID[1000];
		status =OCIHandleAlloc (ociEnv, (void**)&stmthp, OCI_HTYPE_STMT , 0, 0);
		if(lkinfo==NULL)
		{
			GetErrorMess(appliName," lkinfo type MUST be given",ErrorMessage,1);
			return -1;
		}
		if(status!=OCI_SUCCESS)
		{	
			rescode=ShowErrors (status, ociError, "OCIStmtHAndleAlloc unsuccessful");
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			else
				GetErrorMess(appliName,"NOT CONNECTED TO ANY DB",ErrorMessage,1);
			return -1;
		}
		else
		{
			sprintf(selectdevID,"select t.linkid from %s t where instr(nvl(t.link_info,'none'),:lkinfo)>0",MACRO_CONNECTIVITY_TABLE);
			status=OCIStmtPrepare(stmthp, ociError, (text*) selectdevID,(ub4) strlen(selectdevID),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
		}
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bnd1p, ociError,(text*)":lkinfo",-1,(dvoid*) lkinfo,strlen(lkinfo)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else
			status=	OCIAttrSet (stmthp,OCI_HTYPE_STMT,&prefetch_rows,0,OCI_ATTR_PREFETCH_ROWS,ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIAttrSet unsuccessful");
		}
		else
			status=OCIStmtExecute(ociHdbc, stmthp, ociError, 0, 0,(OCISnapshot *) 0, (OCISnapshot *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
		}
		else
			status=OCIDefineByPos (stmthp, &def, ociError, 1,&lkID,sizeof(lkID), SQLT_INT, 0, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
		}
		else
		{
			lkIDs_temp=(int*) realloc(lkIDs_temp,prefetch_rows*sizeof(int));
			if(lkIDs_temp==NULL)
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
					lkIDs_temp=(int*) realloc(lkIDs_temp,0*sizeof(int));
					GetErrorMess(appliName,"BUFFER_TOO_SMALL",ErrorMessage,1);
					return rescode;	
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
				lkIDs_temp=(int*) realloc(lkIDs_temp,0*sizeof(int));
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
		int j=0;
		int rescode=0;
		ub4 numcols = 0;
		ub4 collen=0;
		ub2 type = 0;
		int pos1=0;
		int pos3=0;
		int actual_len=0;
		int pos2=0;
		int len_devname=0;
		char* devName=NULL;
		char* devIDs_temp=NULL;
		sword status;
		int nline=0;
		int nb_rdtrip=1;
		int prefetch_rows=2000;
		OCIStmt *stmthp;
		OCIDefine* def;
		OCIParam *parmdp;
		char selectdevID[1000];
		status =OCIHandleAlloc (ociEnv, (void**)&stmthp, OCI_HTYPE_STMT , 0, 0);

		if(status!=OCI_SUCCESS)
		{	
			rescode=ShowErrors (status, ociError, "OCIStmtHAndleAlloc unsuccessful");
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			else
				GetErrorMess(appliName,"NOT CONNECTED TO ANY DB",ErrorMessage,1);
			return -1;
		}
		else
		{
			sprintf(selectdevID,"select system_name||'|' from %s",SUBSYSTEM_TABLE);
			status=OCIStmtPrepare(stmthp, ociError, (text*) selectdevID,(ub4) strlen(selectdevID),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
			//std:://cout<<"stmt"<<selectdevID<<std::endl;
		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else
			status=	OCIAttrSet (stmthp,OCI_HTYPE_STMT,&prefetch_rows,0,OCI_ATTR_PREFETCH_ROWS,ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIAttrSet unsuccessful");
		}
		else
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
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &len_devname, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);



		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
		}
		else
		{
			devName=(char*)malloc((len_devname+1)*sizeof(char));
			if(devName!=NULL)
				status=OCIDefineByPos (stmthp, &def, ociError, 1,(ub1*) devName,len_devname+1, SQLT_STR, 0, 0, 0, OCI_DEFAULT);
			else
			{
				status=-10;
				rescode=ShowErrors (status, ociError, "pointer allocation unsuccessful");
				rescode=-1;
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				GetErrorMess(appliName,"REALLOC UNSUCCESSFUL",ErrorMessage,1);
				return rescode;	
			}
		}
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
		}
		else
		{


			pos2=0;
			i=0;
			while(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
			{
				status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT,1,OCI_DEFAULT);
				if(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
				{
					pos1=strcspn(devName,"|");
					devName[pos1]='\0';
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
					devIDs_temp=(char*) realloc(devIDs_temp,0*sizeof(char));
					rescode=-1;
					free(devName);
					status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
					GetErrorMess(appliName,"BUFFER_TOO_SMALL",ErrorMessage,1);
					return rescode;
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
				devIDs_temp=(char*) realloc(devIDs_temp,0*sizeof(char));
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
		int j=0;
		int rescode=0;
		ub4 numcols = 0;
		ub4 collen=0;
		ub2 type = 0;
		int pos1=0;
		int pos3=0;
		int actual_len=0;
		int pos2=0;
		int len_devname=0;
		char* devName=NULL;
		char* devIDs_temp=NULL;
		sword status;
		int nline=0;
		int nb_rdtrip=1;
		int prefetch_rows=2000;
		OCIStmt *stmthp;
		OCIBind  *bnd1p = (OCIBind *) 0; 
		OCIDefine* def;
		OCIParam *parmdp;
		char selectdevID[1000];
		status =OCIHandleAlloc (ociEnv, (void**)&stmthp, OCI_HTYPE_STMT , 0, 0);

		if(status!=OCI_SUCCESS)
		{	
			rescode=ShowErrors (status, ociError, "OCIStmtHAndleAlloc unsuccessful");
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			else
				GetErrorMess(appliName,"NOT CONNECTED TO ANY DB",ErrorMessage,1);
			return -1;
		}
		else
		{
			sprintf(selectdevID,"select distinct nvl(hwtype,'none')||'?' from %s where device_status=%d",HW_DEVICE_TABLE,SPARE);
			status=OCIStmtPrepare(stmthp, ociError, (text*) selectdevID,(ub4) strlen(selectdevID),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
			//std:://cout<<"stmt"<<selectdevID<<std::endl;
		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else
			status=	OCIAttrSet (stmthp,OCI_HTYPE_STMT,&prefetch_rows,0,OCI_ATTR_PREFETCH_ROWS,ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIAttrSet unsuccessful");
		}
		else
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
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &len_devname, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
		}
		else
		{
			devName=(char*)malloc((len_devname+1)*sizeof(char));
			if(devName!=NULL)
				status=OCIDefineByPos (stmthp, &def, ociError, 1,(ub1*) devName,len_devname+1, SQLT_STR, 0, 0, 0, OCI_DEFAULT);
			else
			{
				status=-10;
				rescode=ShowErrors (status, ociError, "pointer allocation unsuccessful");
				rescode=-1;
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				GetErrorMess(appliName,"REALLOC UNSUCCESSFUL",ErrorMessage,1);
				return rescode;	
			}
		}
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
		}
		else
		{


			pos2=0;
			i=0;
			while(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
			{
				status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT,1,OCI_DEFAULT);
				if(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
				{
					pos1=strcspn(devName,"?");
					devName[pos1]='\0';
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
					devIDs_temp=(char*) realloc(devIDs_temp,0*sizeof(char));
					rescode=-1;
					free(devName);
					status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
					GetErrorMess(appliName,"BUFFER_TOO_SMALL",ErrorMessage,1);
					return rescode;
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
				devIDs_temp=(char*) realloc(devIDs_temp,0*sizeof(char));
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
		int i = 0;
		int j=0;
		int pos1=0;
		int pos2=0;
		int rescode=0;
		ub4 numcols = 0;
		ub4 collen=0;
		ub2 type = 0;
		sword status;
		OCIStmt *stmthp;
		OCIBind  *bnd1p[4] ; 
		OCIDefine* def[6];
		sb4  phylen=0;
		sb4 iplen=0;
		sb4  dnamelen=0;
		sb4 portypelen=0;
		sb4  portnblen=0;
		sb4 maclen=0;
		sb4 bialen=0;
		OCIParam *parmdp;
		int len=0;
		int free_mem=0;
		//char buffer[20];
		int portid=0;
		int admin_status=0; 
		int pxi_booting=0;
		int speed=0;
		int portway=0;
		char * mac_temp=NULL;
		char * bia_temp=NULL;
		char * snb_temp=NULL;
		char * portype_temp=NULL;
		char * portnb_temp=NULL;

		int mac_null=0;
		int bia_null=0;
		int snb_null=0;
		int portype_null=0;
		int portnb_null=0;
		char logmessage[100];
		char selectport[1000];
		char port_type1[100];
		status =OCIHandleAlloc (ociEnv, (void**)&stmthp, OCI_HTYPE_STMT , 0, 0);
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
		if(status!=OCI_SUCCESS)
		{		
			rescode=ShowErrors (status, ociError, "OCIHandleAlloc unsuccessful");
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			else
				GetErrorMess(appliName,"NOT CONNECTED TO ANY DB",ErrorMessage,1);
			return -1;	
		}
		else
		{
			sprintf(selectport,"select e.serialnb||'?',e.port_nbr||'?',e.port_way,nvl(e.port_type,'none')||'?',nvl(e.bia,'none')||'?',nvl(e.macaddress,'none')||'?' from %s e where e.serialnb=:snb and e.port_nbr=:portnb and e.port_way=:portway and nvl(e.port_type,'none')=:portype",HWPORT_TABLE);
			status=OCIStmtPrepare(stmthp, ociError, (text*) selectport,(ub4) strlen(selectport),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bnd1p[0], ociError,(text*)":snb",-1,(dvoid*) serialnb,strlen(serialnb)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bnd1p[1], ociError,(text*)":portnb",-1,(dvoid*) port_nb,strlen(port_nb)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bnd1p[2], ociError,(text*)":portway",-1,(dvoid*) &port_way,sizeof(port_way), SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bnd1p[3], ociError,(text*)":portype",-1,(dvoid*) port_type1,strlen(port_type1)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else	
			status=OCIStmtExecute(ociHdbc, stmthp, ociError, 0, 0,(OCISnapshot *) 0, (OCISnapshot *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
		}
		else
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 1);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet unsuccessful");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &dnamelen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIAttrGet unsuccessful");
		}
		else
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 2);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet unsuccessful");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &portnblen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIAttrGet unsuccessful");
		}
		else
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 4);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet unsuccessful");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &portypelen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);


		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIAttrGet unsuccessful");
		}
		else
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 5);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet unsuccessful");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &bialen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIAttrGet unsuccessful");
		}
		else
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 6);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet unsuccessful");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &maclen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);


		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIAttrGet unsuccessful");
		}
		else
		{
			MinStringLength(portnblen);

			MinStringLength(portypelen);
			MinStringLength(bialen);
			MinStringLength(maclen);

			MinStringLength(dnamelen);

			portype_temp = (char *) realloc(portype_temp,(portypelen + 1)*sizeof(char));

			portnb_temp = (char *) realloc(portnb_temp,(portnblen + 1)*sizeof(char));
			bia_temp= (char *) realloc(bia_temp,(bialen + 1)*sizeof(char));
			mac_temp = (char *) realloc(mac_temp,(maclen + 1)*sizeof(char));

			snb_temp= (char *) realloc(snb_temp,(dnamelen + 1)*sizeof(char));

			if(snb_temp==NULL || bia_temp==NULL||mac_temp==NULL|| portnb_temp==NULL||portype_temp==NULL)
			{
				status=-10;
				rescode=ShowErrors (status, ociError, "  pointer allocation unsuccessful");
				rescode=-1;
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
				return rescode;
			}
			else
				status=OCIDefineByPos (stmthp, &def[0], ociError, 1,snb_temp,dnamelen+1, SQLT_STR,&snb_null,  0, 0, OCI_DEFAULT);
		}


		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
			}
		}
		else
			status=OCIDefineByPos (stmthp, &def[1], ociError,2,portnb_temp,portnblen+1, SQLT_STR, &portnb_null, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
			}
		}
		else
			status=OCIDefineByPos (stmthp, &def[2], ociError, 3,&portway,sizeof(portway), SQLT_INT, 0, 0, 0, OCI_DEFAULT);


		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
			}
		}
		else
			status=OCIDefineByPos (stmthp, &def[3], ociError, 4,portype_temp,portypelen+1, SQLT_STR, &portype_null, 0, 0, OCI_DEFAULT);



		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
			}
		}
		else
			status =OCIDefineByPos(stmthp, &def[4], ociError,5, (ub1 *) (bia_temp), bialen + 1,SQLT_STR, (dvoid *) &bia_null,(ub2 *) 0,0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
			}
		}
		else
			status=OCIDefineByPos (stmthp, &def[5], ociError, 6,mac_temp,maclen+1, SQLT_STR, (dvoid *) &mac_null,0,0, OCI_DEFAULT);




		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
			}
		}
		else
			status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT,1,OCI_DEFAULT);

		if (status==OCI_SUCCESS || status==OCI_SUCCESS_WITH_INFO) 
		{	


			Format_output(mac_null,mac_temp,  logmessage,'?');
			Format_output(portnb_null,portnb_temp, logmessage,'?');
			Format_output(snb_null,snb_temp,  logmessage,'?');
			Format_output(bia_null,bia_temp, logmessage,'?');
			Format_output(portype_null,portype_temp,  logmessage,'?');

			pos1=strcspn(portype_temp,"?");

			portype_temp[pos1]='\0';

			pos1=strcspn(portnb_temp,"?");
			pos2=strcspn(bia_temp,"?");
			portnb_temp[pos1]='\0';
			bia_temp[pos2]='\0';
			pos1=strcspn(mac_temp,"?");

			mac_temp[pos1]='\0';

			pos1=strcspn(snb_temp,"?");
			snb_temp[pos1]='\0';


			len=strlen(mac_temp)+strlen(bia_temp)+strlen(snb_temp)+strlen(portype_temp)+strlen(portnb_temp)+strlen("|serialnb (C): |port_nbr (C): |port_way (I):1 |port_type (C): |bia (C): |macaddress (C): |");



			if(len_port<len)
			{
				len++;
				len_port=len;
				rescode=-1;
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);

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


				GetErrorMess(appliName,"BUFFER_TOO_SMALL",ErrorMessage,1);
				return rescode;
			}
			else
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