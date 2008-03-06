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

	/*************************************************************************************/
	//return the cpnt row in cpnt_result
	/*************************************************************************************/
	/**
	* Get the row of the given board cpnt name (a chip which is used in the microsc. connect., not board )returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* @param functionalcpntname : functional cpnt name (corresponds to its function in the system):should be unique (like the lg devicename) it's  case sensitive.
	* @param len_cpnt : length of the devtype_result. If the buffer is too small, we put the size needed here.
	* @param cpnt_result :char* buffer where to put the result
	* @param cpnt_result : formatted as follows |db_column_name (db_column_type):db_column_value|
	* @param cpnt_result : db_column_type is I for int and C for char*
	* @param cpnt_result : equals to "NO_ROWS_SELECTED" if no rows selected
	* @param ErrorMessage : error message in case of failure
	* @return 0 if it is successful
	*/

	EXTERN_CONFDB
		int GetBoardCpntRow_cpntname(char* functionalcpntname,int &len_cpnt, char* cpnt_result,char* ErrorMessage)
	{
		char appliName[100]="GetBoardCpntRow_cpntname";
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
		OCIDefine* def[7];
		sb4  devtypelen=0;
		sb4 devlen=0;
		sb4  locationlen=0;
		sb4 serialnblen=0;
		sb4 positionlen=0;

		OCIParam *parmdp;
		int node=0;
		int active=0;
		int cpntid=0;
		int promiscuous_mode=0;
		int status1=0;
		int nodeused=0;

		int len=0;
		char * cpntname_temp=NULL;
		char * cpnttype_temp=NULL;
		char * location_temp=NULL;
		char * serialnb_temp=NULL;
		char* position_temp=NULL; 
		int dname_null=0;
		int dtype_null=0;
		int serialnb_null=0;
		int location_null=0;
		int position_null=0;
		char buffer[20];
		char logmessage[100];
		if(functionalcpntname==NULL)
		{
			GetErrorMess(appliName,"cpntname MUST be given",ErrorMessage,1);
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
			sprintf(selectdev,"select t.cpntname||'?',t.cpnttype||'?',t.cpntid,t.snbid,f.devicename||'?',nvl(g.serialnb,'none')||'?',nvl(t.location,'none')||'?' from %s t,%s f,%s g  where t.cpntname=:dname and t.motherboardid=f.deviceid and g.snbid=t.snbid",LG_CPNTS_TABLE,LOGICAL_DEVICE_TABLE,HW_CPNTS_TABLE);
			status=OCIStmtPrepare(stmthp, ociError, (text*) selectdev,(ub4) strlen(selectdev),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else	
			status=OCIBindByName(stmthp, &bnd1p, ociError,(text*)":dname",-1,(dvoid*) functionalcpntname,strlen(functionalcpntname)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);


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
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 5);

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
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 6);

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
				rescode=ShowErrors (status, ociError, "OCIAttrGet3 unsuccessful");
		}
		else
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 7);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet4  unsuccessful");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &positionlen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);


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
			MinStringLength(positionlen);

			//std::cout << "devtype_len" << devtypelen<< std::endl;
			cpnttype_temp = (char *) realloc(cpnttype_temp,(devtypelen + 1)*sizeof(char));
			cpntname_temp= (char *) realloc(cpntname_temp,(devlen + 1)*sizeof(char));
			location_temp = (char *) realloc(location_temp,(locationlen + 1)*sizeof(char));
			serialnb_temp= (char *) realloc(serialnb_temp,(serialnblen + 1)*sizeof(char));
			position_temp= (char *) realloc(position_temp,(positionlen + 1)*sizeof(char));

			if(cpnttype_temp==NULL || cpntname_temp==NULL || location_temp==NULL || serialnb_temp==NULL||position_temp==NULL)
			{

				rescode=ShowErrors (status, ociError, "  pointer allocation unsuccessful");
				rescode=-1;
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				if(cpnttype_temp!=NULL)
					free(cpnttype_temp);
				if(cpntname_temp!=NULL)
					free(cpntname_temp);
				if(location_temp!=NULL)
					free(location_temp);
				if(serialnb_temp!=NULL)
					free(serialnb_temp);
				if(position_temp!=NULL)
					free(position_temp);

				GetErrorMess(appliName,"REALLOC UNSUCCESSFUL",ErrorMessage,1);
				return rescode;

			}
			else
				status =OCIDefineByPos(stmthp, &def[0], ociError,1, (ub1 *) (cpntname_temp), devlen + 1,SQLT_STR, (dvoid *) &dname_null,(ub2 *) 0,0, OCI_DEFAULT);
		}
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "DefineByPos0 unsuccessful");

			}
		}
		else
			status=OCIDefineByPos (stmthp, &def[1], ociError, 2,cpnttype_temp,devtypelen+1, SQLT_STR, (dvoid *) &dtype_null,0, 0,OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "DefineByPos3 unsuccessful");
			}
		}
		else
			status=OCIDefineByPos (stmthp, &def[2], ociError, 3,&cpntid,sizeof(cpntid), SQLT_INT, 0, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "DefineByPos5 unsuccessful");
			}
		}
		else
			status=OCIDefineByPos (stmthp, &def[4], ociError, 5,location_temp,locationlen+1, SQLT_STR, (dvoid *) &location_null,0, 0,OCI_DEFAULT);


		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "DefineByPos7 unsuccessful");

			}
		}
		else
			status =OCIDefineByPos(stmthp, &def[5], ociError,6, (ub1 *)(serialnb_temp), serialnblen + 1,SQLT_STR, (dvoid *) &serialnb_null,(ub2 *) 0,0, OCI_DEFAULT);
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "DefineByPos3 unsuccessful");
			}
		}
		else
			status=OCIDefineByPos (stmthp, &def[3], ociError,4,&nodeused,sizeof(nodeused), SQLT_INT, 0, 0, 0, OCI_DEFAULT);
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "DefineByPos7 unsuccessful");

			}
		}
		else
			status =OCIDefineByPos(stmthp, &def[6], ociError,7, (ub1 *)(position_temp),positionlen + 1,SQLT_STR, (dvoid *) &position_null,(ub2 *) 0,0, OCI_DEFAULT);

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
			Format_output(dtype_null,cpnttype_temp,logmessage,'?');
			Format_output(location_null,location_temp,logmessage,'?');
			Format_output(dname_null,cpntname_temp,logmessage,'?');
			Format_output(position_null,position_temp,logmessage,'?');

			pos1=strcspn(cpntname_temp,"?");
			pos2=strcspn(cpnttype_temp,"?");
			pos3=strcspn(location_temp,"?");
			pos4=strcspn(serialnb_temp,"?");
			pos5=strcspn(position_temp,"?");

			cpntname_temp[pos1]='\0';
			cpnttype_temp[pos2]='\0';
			location_temp[pos3]='\0';
			serialnb_temp[pos4]='\0';
			position_temp[pos5]='\0';

			len=strlen(position_temp)+strlen(cpnttype_temp)+strlen(cpntname_temp)+strlen(location_temp)+strlen(serialnb_temp)+strlen("|cpntname (C): |cpnttype (C): |cpntid  (I): |snbid (I): |motherboard (C): |serialnb  (C): |location (C): |");
			sprintf(buffer,"%d",cpntid);
			len+=strlen(buffer);
			sprintf(buffer,"%d",nodeused);
			len+=strlen(buffer);

			if(len_cpnt<len)
			{
				len++;
				len_cpnt=len;
				GetErrorMess(appliName, "BUFFER_TOO_SMALL",ErrorMessage,1);
				if(cpnttype_temp!=NULL)
					cpnttype_temp = (char *) realloc(cpnttype_temp,(0)*sizeof(char));
				if(cpntname_temp!=NULL)
					cpntname_temp= (char *) realloc(cpntname_temp,(0)*sizeof(char));
				if(serialnb_temp!=NULL)
					serialnb_temp = (char *) realloc(serialnb_temp,(0)*sizeof(char));
				if(location_temp!=NULL)
					location_temp= (char *) realloc(location_temp,(0)*sizeof(char));
				if(position_temp!=NULL)
					position_temp= (char *) realloc(position_temp,(0)*sizeof(char));
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				return -1;

			}
			else
				sprintf(cpnt_result,"|cpntname (C):%s |cpnttype (C):%s |cpntid  (I):%d |snbid (I):%d |motherboard (C):%s |serialnb  (C):%s |location (C):%s |" ,cpntname_temp,cpnttype_temp,cpntid,nodeused,location_temp,serialnb_temp,position_temp);


		} 
		else 
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "OCIFETCH unsuccessful");
				strcpy(cpnt_result,"NO_ROWS_SELECTED");	
			}
		}

		if(cpnttype_temp!=NULL)
			cpnttype_temp = (char *) realloc(cpnttype_temp,(0)*sizeof(char));
		if(cpntname_temp!=NULL)
			cpntname_temp= (char *) realloc(cpntname_temp,(0)*sizeof(char));
		if(location_temp!=NULL)
			location_temp = (char *) realloc(location_temp,(0)*sizeof(char));
		if(serialnb_temp!=NULL)
			serialnb_temp= (char *) realloc(serialnb_temp,(0)*sizeof(char));
		if(position_temp!=NULL)
			position_temp= (char *) realloc(position_temp,(0)*sizeof(char));
		status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
		rescode+=status;
		if(rescode==0)
		{
			OCIReportError(ociError,appliName,ErrorMessage,0); 
		}
		else
		{
			OCIReportError(ociError,appliName,ErrorMessage,1); 
		}
		return rescode;
	}

	/***************************************************************************************/
	/**
	* Get the row of the given cpntID of the board cpnt in cpnt_result ((a cpnt which is used in the microsc. connect.),  returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* @param cpntID : cpntID of the functional board cpnt
	* @param len_cpnt : length of the cpnt_result. If the buffer is too small, we put the size needed here.
	* @param cpnt_result :char* buffer where to put the result
	* @param cpnt_result : formatted as follows |db_column_name (db_column_type):db_column_value|
	* @param cpnt_result : db_column_type is I for int and C for char*
	* @param cpnt_result : equals to "NO_ROWS_SELECTED" if no rows selected
	* @param ErrorMessage : error message in case of failure
	* @return 0 if it is successful
	*/
	EXTERN_CONFDB
		int GetBoardCpntRow_cpntid(int cpntID,int &len_cpnt, char* cpnt_result,char* ErrorMessage)
	{
		char appliName[100]="GetBoardCpntRow_cpntid";
		int i = 0;
		int j=0;
		int pos1=0;
		int pos2=0;
		int pos3=0;
		int pos4=0;
		int pos5=0;
		int node=0;
		int active=0;
		int cpntid=0;
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
		OCIDefine* def[6];
		sb4  devtypelen=0;
		sb4 devlen=0;
		sb4  locationlen=0;
		sb4 serialnblen=0;
		sb4 positionlen=0;

		OCIParam *parmdp;
		int len=0;
		char * cpntname_temp=NULL;
		char * cpnttype_temp=NULL;
		char * serialnb_temp=NULL;
		char * location_temp=NULL;
		char selectdev[1000];
		char * position_temp=NULL;
		int dname_null=0;
		int dtype_null=0;
		int location_null=0;
		int serialnb_null=0;
		int position_null=0;
		char buffer[20];
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
			sprintf(selectdev,"select t.cpntname||'?',t.cpnttype||'?',t.cpntid,t.snbid,f.devicename||'?',nvl(g.serialnb,'none')||'?',nvl(t.location,'none')||'?' from %s t,%s f,%s g  where t.cpntid=:dID and t.motherboardid=f.deviceid and g.snbid=t.snbid",LG_CPNTS_TABLE,LOGICAL_DEVICE_TABLE,HW_CPNTS_TABLE);
			status=OCIStmtPrepare(stmthp, ociError, (text*) selectdev,(ub4) strlen(selectdev),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bnd1p, ociError,(text*)":dID",-1,(dvoid*) &cpntID,sizeof(cpntID), SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

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
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 5);

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
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 6);

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
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 7);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet4  unsuccessful");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &positionlen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);

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
			MinStringLength(positionlen);

			cpnttype_temp = (char *) realloc(cpnttype_temp,(devtypelen + 1)*sizeof(char));
			cpntname_temp= (char *) realloc(cpntname_temp,(devlen + 1)*sizeof(char));
			location_temp = (char *) realloc(location_temp,(locationlen + 1)*sizeof(char));
			serialnb_temp= (char *) realloc(serialnb_temp,(serialnblen + 1)*sizeof(char));
			position_temp= (char *) realloc(position_temp,(positionlen + 1)*sizeof(char));

			if(cpnttype_temp==NULL || cpntname_temp==NULL || location_temp==NULL || serialnb_temp==NULL||position_temp==NULL)
			{
				status=-10;
				rescode=ShowErrors (status, ociError, "  pointer allocation unsuccessful");
				rescode=-1;
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				if(cpnttype_temp!=NULL)
					free(cpnttype_temp);
				if(cpntname_temp!=NULL)
					free(cpntname_temp);
				if(location_temp!=NULL)
					free(location_temp);
				if(serialnb_temp!=NULL)
					free(serialnb_temp);
				if(position_temp!=NULL)
					free(position_temp);
				GetErrorMess(appliName,"REALLOC UNSUCCESSFUL",ErrorMessage,1);
				return rescode;
			}
			else
				status =OCIDefineByPos(stmthp, &def[0], ociError,1, (ub1 *) (cpntname_temp), devlen + 1,SQLT_STR, (dvoid *) &dname_null,(ub2 *) 0,0, OCI_DEFAULT);
		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "DefineByPos0  unsuccessful");
			}
		}
		else
			status=OCIDefineByPos (stmthp, &def[1], ociError, 2,cpnttype_temp,devtypelen+1, SQLT_STR, (dvoid *)&dtype_null,(ub2 *) 0,0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "DefineByPos3  unsuccessful");
			}
		}
		else
			status=OCIDefineByPos (stmthp, &def[2], ociError, 3,&cpntid,sizeof(cpntid), SQLT_INT, 0, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "DefineByPos5  unsuccessful");
			}
		}
		else
			status=OCIDefineByPos (stmthp, &def[4], ociError,5,location_temp,locationlen+1, SQLT_STR, (dvoid *) &location_null, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "DefineByPos7  unsuccessful");

			}
		}
		else
			status =OCIDefineByPos(stmthp, &def[5], ociError,6, (ub1 *)(serialnb_temp), serialnblen + 1,SQLT_STR, (dvoid *) &serialnb_null,(ub2 *) 0,0, OCI_DEFAULT);
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "DefineByPos7  unsuccessful");

			}
		}
		else
			status =OCIDefineByPos(stmthp, &def[3], ociError,4, (ub1 *)&nodeused,sizeof(int),SQLT_INT, (dvoid *) 0,(ub2 *) 0,0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "DefineByPos7  unsuccessful");

			}
		}
		else
			status =OCIDefineByPos(stmthp, &def[6], ociError,7, (ub1 *)(position_temp), positionlen + 1,SQLT_STR, (dvoid *) &position_null,(ub2 *) 0,0, OCI_DEFAULT);

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
			//std::cout <<"logmessage bar "<<logmessage << "oracle "<<bar_null <<std::endl;
			Format_output(dtype_null,cpnttype_temp, logmessage,'?');
			//std::cout <<"logmessage dtype "<<logmessage <<"oracle "<<dtype_null <<std::endl;
			Format_output(location_null,location_temp, logmessage,'?');
			//std::cout <<"logmessage uuid "<<logmessage <<"oracle "<<uuid_null <<std::endl;
			Format_output(dname_null,cpntname_temp, logmessage,'?');
			//std::cout0 <<"logmessage dname "<<logmessage <<"oracle "<<dname_null <<std::endl;
			Format_output(position_null,position_temp, logmessage,'?');

			pos1=strcspn(cpntname_temp,"?");
			pos2=strcspn(cpnttype_temp,"?");
			pos3=strcspn(location_temp,"?");
			pos4=strcspn(serialnb_temp,"?");
			pos5=strcspn(position_temp,"?");
			cpntname_temp[pos1]='\0';
			cpnttype_temp[pos2]='\0';
			serialnb_temp[pos4]='\0';
			location_temp[pos3]='\0';
			position_temp[pos5]='\0';

			len=strlen(position_temp)+strlen(cpnttype_temp)+strlen(cpntname_temp)+strlen(location_temp)+strlen(serialnb_temp)+strlen("|cpntname (C): |cpnttype (C): |cpntid  (I): |snbid (I): |motherboard (C): |serialnb  (C): |location (C) : |");
			sprintf(buffer,"%d",cpntid);
			len+=strlen(buffer);
			sprintf(buffer,"%d",nodeused);
			len+=strlen(buffer);
			if(len_cpnt<len)
			{
				len++;
				len_cpnt=len;
				rescode=-1;
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				if(cpnttype_temp!=NULL)
					free(cpnttype_temp);
				if(cpntname_temp!=NULL)
					free(cpntname_temp);
				if(location_temp!=NULL)
					free(location_temp);
				if(serialnb_temp!=NULL)
					free(serialnb_temp);
				if(position_temp!=NULL)
					free(position_temp);
				GetErrorMess(appliName,"BUFFER_TOO_SMALL",ErrorMessage,1);
				return rescode;
			}
			else
				sprintf(cpnt_result,"|cpntname (C):%s |cpnttype (C):%s |cpntid  (I):%d |snbid (I):%d |motherboard (C):%s |serialnb  (C):%s |location (c):%s |",cpntname_temp,cpnttype_temp,cpntid,nodeused,location_temp,serialnb_temp,position_temp);


		} 
		else 
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "OCIFETCH  unsuccessful");
				strcpy(cpnt_result,"NO_ROWS_SELECTED");

			}
		}

		if(cpnttype_temp!=NULL)
			cpnttype_temp = (char *) realloc(cpnttype_temp,(0)*sizeof(char));
		if(cpntname_temp!=NULL)
			cpntname_temp= (char *) realloc(cpntname_temp,(0)*sizeof(char));
		if(serialnb_temp!=NULL)
			serialnb_temp = (char *) realloc(serialnb_temp,(0)*sizeof(char));
		if(location_temp!=NULL)
			location_temp= (char *) realloc(location_temp,(0)*sizeof(char));
		if(position_temp!=NULL)
			free(position_temp);
		status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
		rescode+=status;
		if(rescode==0)
		{
			OCIReportError(ociError,appliName,ErrorMessage,0); 
		}
		else
		{
			OCIReportError(ociError,"GetDeviceRow_devid ",ErrorMessage,1); 
		}
		return rescode;
	}


	/**************************************************************************************/
	//return link row in  Conn_row
	/**************************************************************************************/
	/**
	* Get the microscopic link row in  Conn_row, returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* @param lkID : lkID of the microscopic link
	* @param len_conn : length of the Conn_row. If the buffer is too small, we put the size needed here.
	* @param Conn_row :char* buffer where to put the result
	* @param Conn_row : formatted as follows |db_column_name (db_column_type):db_column_value|
	* @param Conn_row : db_column_type is I for int and C for char*
	* @param Conn_row : equals to "NO_ROWS_SELECTED" if no rows selected
	* @param Conn_row : if the cpntfrom or the cpntto are equal to the a functional cpnt (board name); it measn that resp. port_nbrfrom or port_nbrto corrsponds to the portid.
	* @param ErrorMessage : error message in case of failure
	* @return 0 if it is successful
	*/
	EXTERN_CONFDB
		int GetMicroConnectivityRow_lkid(int lkID, int &len_conn,char* Conn_row,char* ErrorMessage)
	{
		char appliName[100]="GetMicroConnectivityRow_lkid";
		int i = 0;
		int len=0;
		int j=0;
		int pos1=0;
		int pos2=0;
		sword status;
		int rescode=0;
		OCIStmt *stmthp;
		OCIBind  *bnd1p = (OCIBind *) 0; 
		OCIDefine* def[8];
		OCIParam *parmdp;
		char buffer[20];
		char logmessage[100];

		sb4  dnamefromlen=0;
		sb4 dnametolen=0;
		sb4  portnbfromlen=0;
		sb4 portnbtolen=0;
		sb4  lknamelen=0;
		char * dnamefrom_temp=NULL;
		char * dnameto_temp=NULL;
		int portnbfrom_temp=0;
		int portnbto_temp=0;
		char * lkname_temp=NULL;
		int dnamefrom_null=0;
		int dnameto_null=0;
		int lkname_null=0;
		int linkid=0;

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
			sprintf(selectconn," select t.linkid,decode(t.cpntidfrom,-1,g.devicename,e.cpntname)||'?',decode(t.cpntidto,-1,h.devicename,f.cpntname)||'?', t.portidfrom,t.portidto,l.link_name||'?',t.bidirectional_link_used,t.lkused from %s t,%s l,%s e,%s f,%s h,%s g where t.linkid=:lkid and t.link_type=l.linktypeID  and t.cpntidfrom=e.cpntid and t.cpntidto=f.cpntid and decode(nvl(e.motherboardid,-1),-1,f.motherboardid,e.motherboardid)=g.deviceid and decode(nvl(f.motherboardid,-1),-1,e.motherboardid,f.motherboardid)=h.deviceid ",MICRO_CONNECTIVITY_TABLE,LINKTYPE_TABLE,LG_CPNTS_TABLE,LG_CPNTS_TABLE,LOGICAL_DEVICE_TABLE,LOGICAL_DEVICE_TABLE);
			status=OCIStmtPrepare(stmthp, ociError, (text*) selectconn,(ub4) strlen(selectconn),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
			//std::cout<<"value of selectconn "<<selectconn<<std::endl;
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
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 6);

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
				rescode=ShowErrors (status, ociError, "OCIAttrGet unsuccessful");
		}
		else
		{
			MinStringLength(lknamelen);
			MinStringLength(dnamefromlen);
			MinStringLength(dnametolen);


			dnameto_temp = (char *) realloc(dnameto_temp,(dnametolen + 1)*sizeof(char));
			dnamefrom_temp= (char *) realloc(dnamefrom_temp,(dnamefromlen + 1)*sizeof(char));
			lkname_temp= (char *) realloc(lkname_temp,(lknamelen + 1)*sizeof(char));

			if(dnamefrom_temp==NULL || dnameto_temp==NULL||lkname_temp==NULL)
			{
				status=-10;
				rescode=ShowErrors (status, ociError, "  pointer allocation unsuccessful");
				rescode=-1;
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);

				if(dnamefrom_temp!=NULL)
					free(dnamefrom_temp);
				if(dnameto_temp!=NULL)
					free(dnameto_temp);
				if(lkname_temp!=NULL)
					free(lkname_temp);



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
			status=OCIDefineByPos (stmthp, &def[2], ociError,3,dnameto_temp,dnametolen+1, SQLT_STR,  0, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
		}
		else
			status=OCIDefineByPos (stmthp, &def[3], ociError, 4,&portnbfrom_temp,sizeof(int), SQLT_INT, 0, 0, 0,OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
		}
		else
			status=OCIDefineByPos (stmthp, &def[4], ociError, 5,&portnbto_temp,sizeof(int), SQLT_INT,0, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
		}
		else
			status=OCIDefineByPos (stmthp, &def[5], ociError, 6,lkname_temp,lknamelen, SQLT_STR, &lkname_null, 0, 0, OCI_DEFAULT);


		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
		}
		else
			status=OCIDefineByPos (stmthp, &def[6], ociError,7,&bidirectional_link_used,sizeof(bidirectional_link_used), SQLT_INT, 0, 0, 0, OCI_DEFAULT);


		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
		}
		else
			status=OCIDefineByPos (stmthp, &def[7], ociError,8,&lkused,sizeof(lkused), SQLT_INT, 0, 0, 0, OCI_DEFAULT);


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
			Format_output(lkname_null,lkname_temp,  logmessage,'?');

			pos1=strcspn(dnamefrom_temp,"?");
			pos2=strcspn(dnameto_temp,"?");
			dnamefrom_temp[pos1]='\0';
			dnameto_temp[pos2]='\0';
			//std::cout<<"value of lkname_temp "<<portnbfrom_temp<<" pp " <<portnbto_temp<<std::endl;


			pos1=strcspn(lkname_temp,"?");

			lkname_temp[pos1]='\0';
			//std::cout<<"value of lkname_temp "<<lkname_temp<<std::endl;
			//std::cout<<"value of linkid "<<linkid<<std::endl;
			len=strlen(lkname_temp)+strlen(dnamefrom_temp)+strlen(dnameto_temp)+strlen("|linkid (I): |cpntfrom (C): |cpntto (C): |port_nbrfrom (I): |port_nbrto (I): |link_type (C): |bidirectional_link_used (I):1 |lkused (I):1 |");
			sprintf(buffer,"%d",linkid);
			//std::cout<<"value of buffer "<<buffer<<std::endl;
			len+=strlen(buffer);
			sprintf(buffer,"%d",portnbfrom_temp);
			len+=strlen(buffer);
			sprintf(buffer,"%d",portnbto_temp);
			len+=strlen(buffer);
			//std::cout<<"value of lkname_temp "<<lkname_temp<<std::endl;
			//std::cout<<"value of len "<<len<<std::endl;
			if(len>len_conn)
			{
				len++;
				len_conn=len;
				rescode=-1;
				if(dnamefrom_temp!=NULL)
					free(dnamefrom_temp);
				if(dnameto_temp!=NULL)
					free(dnameto_temp);
				if(lkname_temp!=NULL)
					free(lkname_temp);

				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);


				GetErrorMess(appliName,"NOT ENOUGH MEMORY",ErrorMessage,1);
				return rescode;

			}
			else
			{
				//std::cout<<"before sprintf connrow "<<std::endl;
				sprintf(Conn_row,"|linkid (I):%d |cpntfrom (C):%s |cpntto (C):%s |port_nbrfrom (C):%d |port_nbrto (C):%d |link_type (C):%s |bidirectional_link_used (I):%d |lkused (I):%d |",linkid,dnamefrom_temp,dnameto_temp,portnbfrom_temp,portnbto_temp,lkname_temp,bidirectional_link_used,lkused);

				// std::cout<<"after sprintf connrow "<<Conn_row<<std::endl;
				if(dnamefrom_temp!=NULL)
					free(dnamefrom_temp);
				if(dnameto_temp!=NULL)
					free(dnameto_temp);
				if(lkname_temp!=NULL)
					free(lkname_temp);


			}
		} 
		else 
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "OCIFETCH unsuccessful");
				strcpy(Conn_row,"NO_ROWS_SELECTED");

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
		return rescode;

	}

	/**************************************************************************************/
	//return link row in  Conn_row
	/**************************************************************************************/
	/**
	* Get the micro link row in  Conn_row,  returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* @param cpntID : cpntid of the functional board cpnt. Put -1 if it's the motherboard
	* @param port_nb : port_nb of the cpnt. Put the portid if cpntID=-1.
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
		int GetMicroConnectivityRow_node(int cpntID, int port_nb, int port_way,int &len_conn, char* Conn_row,char* ErrorMessage)
	{
		char appliName[100]="GetMicroConnectivityRow_node";
		int i = 0;
		int len=0;
		int j=0;
		int pos1=0;
		int pos2=0;
		sword status;
		int rescode=0;
		char logmessage[100];

		OCIStmt *stmthp;
		OCIBind  *bnd1p[2] ; 
		OCIDefine* def[8];
		OCIParam *parmdp;
		char buffer[20];
		sb4  dnamefromlen=0;
		sb4 dnametolen=0;
		sb4  lknamelen=0;
		char * dnamefrom_temp=NULL;
		char * dnameto_temp=NULL;
		int portnbfrom_temp=0;
		int  portnbto_temp=0;
		char * lkname_temp=NULL;
		int dnamefrom_null=0;
		int dnameto_null=0;
		int lkname_null=0;
		int linkid=0;

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
				sprintf(selectconn," select t.linkid,decode(t.cpntidfrom,-1,g.devicename,e.cpntname)||'?',decode(t.cpntidto,-1,h.devicename,f.cpntname)||'?', t.portidfrom,t.portidto,l.link_name||'?',t.bidirectional_link_used,t.lkused from %s t,%s l,%s e,%s f,%s h,%s g where t.cpntidto=:cpntid and t.portidto=:portnb and t.link_type=l.linktypeID  and t.cpntidfrom=e.cpntid and t.cpntidto=f.cpntid and decode(nvl(e.motherboardid,-1),-1,f.motherboardid,e.motherboardid)=g.deviceid and decode(nvl(f.motherboardid,-1),-1,e.motherboardid,f.motherboardid)=h.deviceid",MICRO_CONNECTIVITY_TABLE,LINKTYPE_TABLE,LG_CPNTS_TABLE,LG_CPNTS_TABLE,LOGICAL_DEVICE_TABLE,LOGICAL_DEVICE_TABLE);
			else // info about the node_from part of a link
				sprintf(selectconn," select t.linkid,decode(t.cpntidfrom,-1,g.devicename,e.cpntname)||'?',decode(t.cpntidto,-1,h.devicename,f.cpntname)||'?', t.portidfrom,t.portidto,l.link_name||'?',t.bidirectional_link_used,t.lkused from %s t,%s l,%s e,%s f,%s h,%s g where t.cpntidfrom=:cpntid and t.portidfrom=:portnb and t.link_type=l.linktypeID  and t.cpntidfrom=e.cpntid and t.cpntidto=f.cpntid and decode(nvl(e.motherboardid,-1),-1,f.motherboardid,e.motherboardid)=g.deviceid and decode(nvl(f.motherboardid,-1),-1,e.motherboardid,f.motherboardid)=h.deviceid",MICRO_CONNECTIVITY_TABLE,LINKTYPE_TABLE,LG_CPNTS_TABLE,LG_CPNTS_TABLE,LOGICAL_DEVICE_TABLE,LOGICAL_DEVICE_TABLE);
			//std::cout<<"value of selectconn "<<selectconn<<" and len="<<strlen(selectconn)<<std::endl;
			status=OCIStmtPrepare(stmthp, ociError, (text*) selectconn,(ub4) strlen(selectconn),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bnd1p[0], ociError,(text*)":cpntid",-1,(dvoid*) &cpntID,sizeof(cpntID), SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bnd1p[1], ociError,(text*)":portnb",-1,(dvoid*)&port_nb,sizeof(port_nb), SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		//std::cout<<"after binding "<<std::endl;
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else
			status=OCIStmtExecute(ociHdbc, stmthp, ociError, 0, 0,(OCISnapshot *) 0, (OCISnapshot *) 0, OCI_DEFAULT);
		//std::cout<<"after execute "<<std::endl;
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

		//std::cout<<"after dnamefromlen "<<std::endl;
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

		//std::cout<<"after dnametolen "<<std::endl;

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
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &lknamelen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);


		//std::cout<<"after attr get "<<std::endl;


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



			dnameto_temp = (char *) realloc(dnameto_temp,(dnametolen + 1)*sizeof(char));
			dnamefrom_temp= (char *) realloc(dnamefrom_temp,(dnamefromlen + 1)*sizeof(char));
			lkname_temp= (char *) realloc(lkname_temp,(lknamelen + 1)*sizeof(char));

			if(dnamefrom_temp==NULL || dnameto_temp==NULL ||lkname_temp==NULL)
			{
				status=-10;
				rescode=ShowErrors (status, ociError, "  pointer allocation unsuccessful");
				rescode=-1;
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);

				if(dnamefrom_temp!=NULL)
					free(dnamefrom_temp);
				if(dnameto_temp!=NULL)
					free(dnameto_temp);
				if(lkname_temp!=NULL)
					free(lkname_temp);



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
			status=OCIDefineByPos (stmthp, &def[3], ociError, 4,&portnbfrom_temp,sizeof(int), SQLT_INT, 0, 0, 0,  OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
		}
		else
			status=OCIDefineByPos (stmthp, &def[4], ociError, 5,&portnbto_temp,sizeof(int), SQLT_INT, 0, 0, 0,  OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
		}
		else
			status=OCIDefineByPos (stmthp, &def[5], ociError, 6,lkname_temp,lknamelen, SQLT_STR, &lkname_null, 0, 0, OCI_DEFAULT);


		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
		}
		else
			status=OCIDefineByPos (stmthp, &def[6], ociError,7,&bidirectional_link_used,sizeof(bidirectional_link_used), SQLT_INT, 0, 0, 0, OCI_DEFAULT);


		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
		}
		else
			status=OCIDefineByPos (stmthp, &def[7], ociError,8,&lkused,sizeof(lkused), SQLT_INT, 0, 0, 0, OCI_DEFAULT);

		//std::cout<<"after defining "<<std::endl;


		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
		}
		else
			status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT,1, OCI_DEFAULT);
		//std::cout<<"after fetching "<<std::endl;
		if (status==OCI_SUCCESS || status==OCI_SUCCESS_WITH_INFO) 
		{	

			Format_output(dnamefrom_null,dnamefrom_temp,  logmessage,'?');
			Format_output(dnameto_null,dnameto_temp, logmessage,'?');
			Format_output(lkname_null,lkname_temp,  logmessage,'?');

			pos1=strcspn(dnamefrom_temp,"?");
			pos2=strcspn(dnameto_temp,"?");
			dnamefrom_temp[pos1]='\0';
			dnameto_temp[pos2]='\0';
			pos1=strcspn(lkname_temp,"?");
			lkname_temp[pos1]='\0';
			len=strlen(lkname_temp)+strlen(dnamefrom_temp)+strlen(dnameto_temp)+strlen("|linkid (I): |cpnt_from (C): |cpnt_to (C): |port_nbrfrom (I): |port_nbrto (I): |link_type (C): |bidirectional_link_used (I):1 |lkused (I): 1|");
			sprintf(buffer,"%d",linkid);
			len+=strlen(buffer);
			//std::cout<<"after fetching 2"<<std::endl;
			if(len>len_conn)
			{
				len++;
				len_conn=len;
				rescode=-1;

				if(dnamefrom_temp!=NULL)
					free(dnamefrom_temp);
				if(dnameto_temp!=NULL)
					free(dnameto_temp);
				if(lkname_temp!=NULL)
					free(lkname_temp);

				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);


				GetErrorMess(appliName,"NOT ENOUGH SPACE",ErrorMessage,1);
				return rescode;

			}
			else
			{
				sprintf(Conn_row,"|linkid (I):%d |cpnt_from (C):%s |cpnt_to (C):%s |port_nbrfrom (C):%d |port_nbrto (C):%d |link_type (C):%s |bidirectional_link_used (I):%d |lkused (I):%d |",linkid,dnamefrom_temp,dnameto_temp,portnbfrom_temp,portnbto_temp,lkname_temp,bidirectional_link_used, lkused);



				if(dnamefrom_temp!=NULL)
					free(dnamefrom_temp);
				if(dnameto_temp!=NULL)
					free(dnameto_temp);
				if(lkname_temp!=NULL)
					free(lkname_temp);
			}
		} 
		else 
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "OCIFETCH unsuccessful");
				strcpy(Conn_row,"NO_ROWS_SELECTED");

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
		return rescode;
	}
	/**************************************************************************************/
	//return link row in  Conn_row
	/**************************************************************************************/
	/**
	* Get the link row in  Conn_row, 
	* The user should manage the memory : there is no memory allocation.
	* @param cpnt_name : cpnt_name of the functional board cpnt. Put "motherboard" in lower case if it's the board name
	* @param port_nb : port_nb of the cpnt. Put the portid if cpnt_name="motherboard"
	* @param port_way : 1 if the port_nb is an input (so it means that the link ends up to this port),2 if the port nb is an output (so the link starts from this port)
	* @param len_conn : length of the Conn_row. If the buffer is too small, we put the size needed here.
	* @param Conn_row :char* buffer where to put the result
	* @param Conn_row : formatted as follows |db_column_name (db_column_type):db_column_value|
	* @param Conn_row :attribute cpntname_nodefrom and cpntname_nodeto are not part of the row, it's just to get the cpntname of nodefrom and nodeto
	* @param Conn_row : db_column_type is I for int and C for char*
	* @param Conn_row : equals to "NO_ROWS_SELECTED" if no rows selected
	* @param ErrorMessage : error message in case of failure
	* @return 0 if it is successful
	*/
	EXTERN_CONFDB
		int GetMicroConnectivityRow_cpntname(char* cpnt_name, int port_nb, int port_way,int &len_conn, char* Conn_row,char* ErrorMessage)
	{
		char appliName[100]="GetMicroConnectivityRow_cpntname";
		int i = 0;
		int len=0;
		int j=0;
		int pos1=0;
		int pos2=0;
		sword status;
		int rescode=0;
		char logmessage[100];

		OCIStmt *stmthp;
		OCIBind  *bnd1p[3] ; 
		OCIDefine* def[8];
		OCIParam *parmdp;
		char buffer[20];
		sb4  dnamefromlen=0;
		sb4 dnametolen=0;
		sb4  lknamelen=0;
		char * dnamefrom_temp=NULL;
		char * dnameto_temp=NULL;
		int portnbfrom_temp=0;
		int  portnbto_temp=0;
		char * lkname_temp=NULL;
		int dnamefrom_null=0;
		int dnameto_null=0;
		int lkname_null=0;
		int linkid=0;

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
				sprintf(selectconn," select t.linkid,decode(t.cpntidfrom,-1,g.devicename,e.cpntname)||'?',decode(t.cpntidto,-1,h.devicename,f.cpntname)||'?', t.portidfrom,t.portidto,l.link_name||'?',t.bidirectional_link_used,t.lkused from %s t,%s l,%s e,%s f,%s h,%s g where f.cpntname=:cpntname and t.portidto=:portnb and t.link_type=l.linktypeID  and t.cpntidfrom=e.cpntid and t.cpntidto=f.cpntid and decode(nvl(e.motherboardid,-1),-1,h.deviceid,e.motherboardid)=g.deviceid and decode(nvl(f.motherboardid,-1),-1,g.deviceid,f.motherboardid)=h.deviceid",MICRO_CONNECTIVITY_TABLE,LINKTYPE_TABLE,LG_CPNTS_TABLE,LG_CPNTS_TABLE,LOGICAL_DEVICE_TABLE,LOGICAL_DEVICE_TABLE);
			else // info about the node_from part of a link
				sprintf(selectconn," select t.linkid,decode(t.cpntidfrom,-1,g.devicename,e.cpntname)||'?',decode(t.cpntidto,-1,h.devicename,f.cpntname)||'?', t.portidfrom,t.portidto,l.link_name||'?',t.bidirectional_link_used,t.lkused from %s t,%s l,%s e,%s f,%s h,%s g where e.cpntname=:cpntname and t.portidfrom=:portnb and t.link_type=l.linktypeID  and t.cpntidfrom=e.cpntid and t.cpntidto=f.cpntid and decode(nvl(e.motherboardid,-1),-1,h.deviceid,e.motherboardid)=g.deviceid and decode(nvl(f.motherboardid,-1),-1,g.deviceid,f.motherboardid)=h.deviceid",MICRO_CONNECTIVITY_TABLE,LINKTYPE_TABLE,LG_CPNTS_TABLE,LG_CPNTS_TABLE,LOGICAL_DEVICE_TABLE,LOGICAL_DEVICE_TABLE);

			status=OCIStmtPrepare(stmthp, ociError, (text*) selectconn,(ub4) strlen(selectconn),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bnd1p[0], ociError,(text*)":cpntname",-1,(dvoid*) cpnt_name,strlen(cpnt_name)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bnd1p[1], ociError,(text*)":portnb",-1,(dvoid*) &port_nb,sizeof(int), SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

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
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 6);

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
				rescode=ShowErrors (status, ociError, "OCIAttrGet unsuccessful");
		}
		else
		{
			MinStringLength(lknamelen);
			MinStringLength(dnamefromlen);
			MinStringLength(dnametolen);



			dnameto_temp = (char *) realloc(dnameto_temp,(dnametolen + 1)*sizeof(char));
			dnamefrom_temp= (char *) realloc(dnamefrom_temp,(dnamefromlen + 1)*sizeof(char));
			lkname_temp= (char *) realloc(lkname_temp,(lknamelen + 1)*sizeof(char));

			if(dnamefrom_temp==NULL || dnameto_temp==NULL ||lkname_temp==NULL)
			{
				status=-10;
				rescode=ShowErrors (status, ociError, "  pointer allocation unsuccessful");
				rescode=-1;
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);

				if(dnamefrom_temp!=NULL)
					free(dnamefrom_temp);
				if(dnameto_temp!=NULL)
					free(dnameto_temp);
				if(lkname_temp!=NULL)
					free(lkname_temp);



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
			status=OCIDefineByPos (stmthp, &def[3], ociError, 4,&portnbfrom_temp,sizeof(int), SQLT_INT, 0, 0, 0,  OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
		}
		else
			status=OCIDefineByPos (stmthp, &def[4], ociError, 5,&portnbto_temp,sizeof(int), SQLT_INT, 0, 0, 0,  OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
		}
		else
			status=OCIDefineByPos (stmthp, &def[5], ociError, 6,lkname_temp,lknamelen, SQLT_STR, &lkname_null, 0, 0, OCI_DEFAULT);


		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
		}
		else
			status=OCIDefineByPos (stmthp, &def[6], ociError,7,&bidirectional_link_used,sizeof(bidirectional_link_used), SQLT_INT, 0, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
		}
		else
			status=OCIDefineByPos (stmthp, &def[7], ociError,8,&lkused,sizeof(lkused), SQLT_INT, 0, 0, 0, OCI_DEFAULT);



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
			Format_output(lkname_null,lkname_temp,  logmessage,'?');

			pos1=strcspn(dnamefrom_temp,"?");
			pos2=strcspn(dnameto_temp,"?");
			dnamefrom_temp[pos1]='\0';
			dnameto_temp[pos2]='\0';
			pos1=strcspn(lkname_temp,"?");
			lkname_temp[pos1]='\0';
			len=strlen(lkname_temp)+strlen(dnamefrom_temp)+strlen(dnameto_temp)+strlen("|linkid (I): |cpnt_from (C): |cpnt_to (C): |port_nbrfrom (I): |port_nbrto (I): |link_type (C): |bidirectional_link_used (I):1 |lkused (I):1 |");
			sprintf(buffer,"%d",linkid);
			len+=strlen(buffer);

			if(len>len_conn)
			{
				len++;
				len_conn=len;
				rescode=-1;

				if(dnamefrom_temp!=NULL)
					free(dnamefrom_temp);
				if(dnameto_temp!=NULL)
					free(dnameto_temp);
				if(lkname_temp!=NULL)
					free(lkname_temp);
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				GetErrorMess(appliName,"GetConnectivityRow_lkid",ErrorMessage,1);
				return rescode;
			}
			else
			{
				sprintf(Conn_row,"|linkid (I):%d |cpnt_from (C):%s |cpnt_to (C):%s |port_nbrfrom (I):%d |port_nbrto (I):%d |link_type (C):%s |bidirectional_link_used (I):%d |lkused (I):%d |",linkid,dnamefrom_temp,dnameto_temp,portnbfrom_temp,portnbto_temp,lkname_temp,bidirectional_link_used, lkused);
				if(dnamefrom_temp!=NULL)
					free(dnamefrom_temp);
				if(dnameto_temp!=NULL)
					free(dnameto_temp);
				if(lkname_temp!=NULL)
					free(lkname_temp);

			}
		} 
		else 
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "OCIFETCH unsuccessful");
				strcpy(Conn_row,"NO_ROWS_SELECTED");

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
		return rescode;

	}




	/*****************************************************************************************/
	/**
	* Get the list of cpntname, cpntid  of the given cpnt type, and returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* @param cpnttype : board cpnt type name (functional)
	* @param len_array : length of the devIDs_list. If the buffer is too small, we put the size needed here.
	* @param cpntIDs_list :  char* buffer where to put the result (list of functional cpntNames and cpntid, presented  as follows cpntname|cpntid); element separated by a '\\0'.
	* @param cpntIDs_list : equals to "NO_ROWS_SELECTED" if no rows selected
	* @param ErrorMessage : error message in case of failure
	* @return 0 if it is successful
	*/
	EXTERN_CONFDB
		int GetBoardCpntPerType(char* cpnttype, int &len_array, char* cpntIDs_list,char* ErrorMessage)
	{
		char appliName[100]="GetBoardCpntPerType";
		int i = 0;
		int j=0;
		int rescode=0;
		ub4 numcols = 0;
		ub4 collen=0;
		ub2 type = 0;
		int pos1=0;
		int pos3=0;
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
		char selectdevID[1000];
		int dname_null=0;
		char logmessage[100];
		OCIParam *parmdp;

		status =OCIHandleAlloc (ociEnv, (void**)&stmthp, OCI_HTYPE_STMT , 0, 0);
		if(cpnttype==NULL)
		{
			GetErrorMess(appliName," cpnt type MUST be given",ErrorMessage,1);
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
			sprintf(selectdevID,"select cpntname||'|'||cpntid||'?' from %s t where cpnttype=:dtype",LG_CPNTS_TABLE);
			status=OCIStmtPrepare(stmthp, ociError, (text*) selectdevID,(ub4) strlen(selectdevID),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
		}
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bnd1p, ociError,(text*)":dtype",-1,(dvoid*) cpnttype,strlen(cpnttype)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

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
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 1);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet unsuccessful");
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
						memcpy(cpntIDs_list,devIDs_temp,pos2);
						len_array=pos2;
					}
					else
					{
						rescode=ShowErrors (status, ociError, "OCIFETCH unsuccessful");
						strcpy(cpntIDs_list,"NO_ROWS_SELECTED");

					}
				}
				devIDs_temp=(char*) realloc(devIDs_temp,0*sizeof(char));
				free(devName);
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
		return rescode;
	}

	/*****************************************************************************************/
	/**
	* Get the list of micro LkID  which start from the given cpntID, returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* @param cpntid_from : cpntID which starts the link. Put -1 if you want to get the list of lkids whcih start from your motherboard
	* @param motherboardID : motherboardid (=deviceid of a functional device). Put -1 if you want to get the list of lkids whcih start from a cpntboard
	* @param len_array : length of the lkfrom_list. If the buffer is too small, we put the size needed here.
	* @param lkfrom_list :  int* buffer where to put the result (list of cpntIDs)
	* @param lkfrom_list : equals to (-1,-1) if no rows selected
	* @param ErrorMessage : error message in case of failure
	* @return 0 if it is successful
	*/
	EXTERN_CONFDB
		int GetMicroLkFromCpntID(int cpntid_from,int motherboardID, int &len_array, int* lkfrom_list,char* ErrorMessage)
	{
		char appliName[100]="GetMicroLkFromCpntID";
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
			if(motherboardID==-1)
				sprintf(select_query,"select linkid from %s where cpntidfrom=:nfrom ",MICRO_CONNECTIVITY_TABLE);
			else
				sprintf(select_query,"select t.linkid from %s t,%s d where d.motherboardid=:nfrom and d.cpntid=t.cpntidto and t.cpntidfrom=-1 ",MICRO_CONNECTIVITY_TABLE,LG_CPNTS_TABLE);
			status=OCIStmtPrepare(stmthp, ociError, (text*) select_query,(ub4) strlen(select_query),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
		}
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else
		{
			if(motherboardID==-1)
				status=OCIBindByName(stmthp, &bnd1p, ociError,(text*)":nfrom",-1,(dvoid*)&cpntid_from,sizeof(cpntid_from), SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);
			else
				status=OCIBindByName(stmthp, &bnd1p, ociError,(text*)":nfrom",-1,(dvoid*)&motherboardID,sizeof(motherboardID), SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);
		}	
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
						rescode=ShowErrors (status, ociError, "OCIFETCH  unsuccessful");
						lkfrom_list[0]=-1;
						lkfrom_list[1]=-1;
					}
				}
				lkIDs_temp=(int*) realloc(lkIDs_temp,0*sizeof(int));
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
		return rescode;
	}

	/***************************************************************************************/
	//returns all the links which end to that node
	/*****************************************************************************************/
	/**
	* Get the list of micro LkID  which end at the given cpntID, returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* @param cpnt_to : cpntID (end of the micro link).Put -1 if you want to get the list of lkids which end at your motherboard
	* @param motherboardID : motherboardid (=deviceid of a functional device). Put -1 if you want to get the list of lkids whcih ends at a cpntboard
	* @param len_array : length of the lkfrom_list. If the buffer is too small, we put the size needed here.
	* @param lkfrom_list :  int* buffer where to put the result (list of deviceIDs)
	* @param lkfrom_list : equals to (-1,-1) if no rows selected
	* @param ErrorMessage : error message in case of failure
	* @return 0 if it is successful
	*/

	EXTERN_CONFDB
		int GetMicroLkToCpntID(int cpnt_to,int motherboardID, int &len_array, int* lkfrom_list,char* ErrorMessage)
	{
		char appliName[100]="GetMicroLkToCpntID";
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
			if(motherboardID==-1)
				sprintf(select_query,"select linkid from %s where cpntidto=:nto ",MICRO_CONNECTIVITY_TABLE);
			else
				sprintf(select_query,"select t.linkid from %s t,%s d where d.motherboardid=:nto and d.cpntid=t.cpntidfrom and t.cpntidto=-1",MICRO_CONNECTIVITY_TABLE,LG_CPNTS_TABLE);
			status=OCIStmtPrepare(stmthp, ociError, (text*) select_query,(ub4) strlen(select_query),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
		}
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else
		{
			if(motherboardID==-1)
				status=OCIBindByName(stmthp, &bnd1p, ociError,(text*)":nto",-1,(dvoid*)&cpnt_to,sizeof(cpnt_to), SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);
			else
				status=OCIBindByName(stmthp, &bnd1p, ociError,(text*)":nto",-1,(dvoid*)&motherboardID,sizeof(motherboardID), SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		}	
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
						rescode=ShowErrors (status, ociError, "OCIFETCH unsuccessful");
						lkfrom_list[0]=-1;
						lkfrom_list[1]=-1;
					}
				}
				lkIDs_temp=(int*) realloc(lkIDs_temp,0*sizeof(int));
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
		return rescode;
	}






	/*************************************************************************************/
	//return the cpntid 
	/*************************************************************************************/

	/**
	* Get the cpntid of the given board cpnt name in cpntID,  returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* @param cpntname : board cpnt name: it's case sensitive.
	* @param cpntID : cpntID (to use if return value is 0) always>0
	* @param cpntID is set -1 if no matching cpnt is found
	* @param ErrorMessage : error message in case of failure
	* @return 0 if it is successful
	*/

	EXTERN_CONFDB
		int GetCpntID_cpntname(char* cpntname,int &cpntID,char* ErrorMessage)
	{
		char appliName[100]="GetCpntID_cpntname";
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
		int cpntid=0;
		int promiscuous_mode=0;
		int status1=0;
		int nodeused=0;
		int len=0;
		char selectdev[1000];
		status =OCIHandleAlloc (ociEnv, (void**)&stmthp, OCI_HTYPE_STMT , 0, 0);
		if(cpntname==NULL)
		{
			GetErrorMess(appliName," cpnt name MUST be given",ErrorMessage,1);
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
			sprintf(selectdev,"select cpntid from %s where cpntname=:dname",LG_CPNTS_TABLE);
			status=OCIStmtPrepare(stmthp, ociError, (text*) selectdev,(ub4) strlen(selectdev),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else	
			status=OCIBindByName(stmthp, &bnd1p, ociError,(text*)":dname",-1,(dvoid*) cpntname,strlen(cpntname)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

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
			status=OCIDefineByPos (stmthp, &def[0], ociError, 1,&cpntid,sizeof(cpntid), SQLT_INT, 0, 0, 0, OCI_DEFAULT);

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
			cpntID=cpntid;
		} 
		else 
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "Fetch unsuccessful");
				cpntID=-1; //means no data found
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
	//return the cpntname
	/*************************************************************************************/
	/**
	* Get the cpntname corresponding to the given cpntID in cpntname, returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* @param cpntID : cpntID always>0
	* @param cpntname : boadr cpnt name (return); Error if failed
	* @param cpntname : if nothing is found, cpntname is set to NO_ROWS_SELECTED
	* @param ErrorMessage : error message in case of failure
	* @return 0 if it is successful
	*/

	EXTERN_CONFDB
		int GetCpntName_cpntid(int cpntID,char* cpntname,char* ErrorMessage)
	{
		char appliName[100]="GetCpntName_cpntid";
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
		char * cpntname_temp=NULL;

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
			sprintf(selectdev,"select cpntname||'?' from %s where cpntid=:devid",LG_CPNTS_TABLE);
			status=OCIStmtPrepare(stmthp, ociError, (text*) selectdev,(ub4) strlen(selectdev),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else	
			status=OCIBindByName(stmthp, &bnd1p, ociError,(text*)":devid",-1,(dvoid*)&cpntID,sizeof(cpntID), SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

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
			cpntname_temp=(char*)realloc(cpntname_temp,(devlen+1)*sizeof(char));
			if(cpntname_temp==NULL)
			{
				rescode=ShowErrors (-10, ociError, "Memory allocation failed ");
				status=-10;
				rescode=-1;

				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				GetErrorMess(appliName,"REALLOC UNSUCCESSFUL",ErrorMessage,1);
				return rescode;
			}
			else
				status=OCIDefineByPos (stmthp, &def[0], ociError, 1,cpntname_temp,devlen+1, SQLT_STR, 0, 0, 0, OCI_DEFAULT);
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

			pos1=strcspn(cpntname_temp,"?");
			cpntname_temp[pos1]='\0';
			strcpy(cpntname,cpntname_temp);

		} 
		else 
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "Fetch unsuccessful");
				strcpy(cpntname,"NO_ROWS_SELECTED");
			}
		}
		if(cpntname_temp!=NULL)
			cpntname_temp=(char*)realloc(cpntname_temp,0*sizeof(char));

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
	/**
	* Get the list of functional cpnt board names  which are located on the given motherboard, returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* @param motherboardname : name of the motherboard (functional name) 
	* @param len_array : length of the devIDs_list. If the buffer is too small, we put the size needed here.
	* @param devnames_list :  char* buffer where to put the result (list of cpntIDs), elements separated by '\\0';
	* @param devnames_list : NO_ROWS_SELECTED is set in case there is no free cpnts
	* @param ErrorMessage : error message in case of failure
	* @return 0 if it is successful
	*/
	EXTERN_CONFDB
		int GetCpntNamesPerBoard(char* motherboardname, int &len_array, char* devnames_list,char* ErrorMessage)
	{
		char appliName[100]="GetCpntNamesPerBoard";
		int i = 0;
		int j=0;
		int rescode=0;
		ub4 numcols = 0;
		ub4 collen=0;
		ub2 type = 0;
		int pos1=0;
		int pos3=0;
		int pos2=0;
		int len_devname=0;
		char* devName=NULL;
		char* devIDs_temp=NULL;
		sword status;
		int nline=0;
		int nb_rdtrip=1;
		int prefetch_rows=2000;
		OCIStmt *stmthp;
		OCIParam *parmdp;
		OCIBind  *bnd1p = (OCIBind *) 0; 
		OCIDefine* def;
		char selectdevID[1000];
		status =OCIHandleAlloc (ociEnv, (void**)&stmthp, OCI_HTYPE_STMT , 0, 0);
		if(motherboardname==NULL)
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
			sprintf(selectdevID,"select cpntname||'|' from %s,%s  where motherboardid=deviceid and devicename=:location",LG_CPNTS_TABLE,LOGICAL_DEVICE_TABLE);
			status=OCIStmtPrepare(stmthp, ociError, (text*) selectdevID,(ub4) strlen(selectdevID),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
			//std::cout<<"stmt"<<selectdevID<<std::endl;
		}
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bnd1p, ociError,(text*)":location",-1,(dvoid*) motherboardname,strlen(motherboardname)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

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
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 1);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet unsuccessful");
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
						rescode=ShowErrors (status, ociError, "OCIFETCH unsuccessful");
						strcpy(devnames_list,"NO_ROWS_SELECTED");
					}
				}
				devIDs_temp=(char*) realloc(devIDs_temp,0*sizeof(char));
				free(devName);
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
		return rescode;
	}

	/*****************************************************************************************/
	/**
	* Get the list of spare hw board cpnt snbid concatenated with serialnb concatenated with the hw name  if it exists  which are at the given location, returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* @param location : location 
	* @param len_array : length of the devIDs_list. If the buffer is too small, we put the size needed here.
	* @param devnames_list :  char* buffer where to put the result (list of cpntIDs). one line is rpresneted as  follows snbid|serialnb|hwname. the lines are separated with '\\0'
	* @param devnames_list : NO_ROWS_SELECTED is set in case there is no free cpnts
	* @param ErrorMessage : error message in case of failure
	* @return 0 if it is successful
	*/
	EXTERN_CONFDB
		int GetSpareHWCpntPerLocation(char* location, int &len_array, char* devnames_list,char* ErrorMessage)
	{
		char appliName[100]="GetSpareHWCpntPerLocation";
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
			sprintf(selectdevID,"select snbid||'|'||nvl(serialnb,'none')||'|'||nvl(hwname,'none')||'?' from %s where nvl(location,'none')=:location",HW_CPNTS_TABLE);
			status=OCIStmtPrepare(stmthp, ociError, (text*) selectdevID,(ub4) strlen(selectdevID),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
			//std::cout<<"stmt"<<selectdevID<<std::endl;
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
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 1);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet unsuccessful");
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
						memcpy(devnames_list,devIDs_temp,pos2);
						len_array=pos2;
					}
					else
					{
						rescode=ShowErrors (status, ociError, "OCIFETCH unsuccessful");
						strcpy(devnames_list,"NO_ROWS_SELECTED");
					}
				}
				devIDs_temp=(char*) realloc(devIDs_temp,0*sizeof(char));
				free(devName);
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
		return rescode;
	}
	/*****************************************************************************************/
	/**
	* Get the list of spare snbd, serialnb + names (if it exists)  which are of the given hwtype, returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* @param hwtype : hwtype
	* @param len_array : length of the devIDs_list. If the buffer is too small, we put the size needed here.
	* @param devnames_list :  char* buffer where to put the result (list of cpntIDs). one line is rpresneted as  follows snbid|serialnb|hwname. the lines are separated with '\\0'
	* @param devnames_list : NO_ROWS_SELECTED is set in case there is no free cpnts
	* @param ErrorMessage : error message in case of failure
	* @return 0 if it is successful
	*/
	EXTERN_CONFDB
		int GetSpareHWCpntPerType(char* hwtype, int &len_array, char* devnames_list,char* ErrorMessage)
	{
		char appliName[100]="GetSpareHWCpntPerType";
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
		int len_devname=300;
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
			sprintf(selectdevID,"select snbid||'|'||nvl(serialnb,'none')||'|'||nvl(hwname,'none')||'?' from %s where nvl(hwtype,'none')=:hwtype",HW_CPNTS_TABLE);
			status=OCIStmtPrepare(stmthp, ociError, (text*) selectdevID,(ub4) strlen(selectdevID),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
			//std::cout<<"stmt"<<selectdevID<<std::endl;
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
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 1);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet unsuccessful");
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
						memcpy(devnames_list,devIDs_temp,pos2);
						len_array=pos2;
					}
					else
					{
						rescode=ShowErrors (status, ociError, "OCIFETCH unsuccessful");
						strcpy(devnames_list,"NO_ROWS_SELECTED");
					}
				}
				devIDs_temp=(char*) realloc(devIDs_temp,0*sizeof(char));
				free(devName);
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
		return rescode;
	}
	/*************************************************************************************/
	//return the cpnt row in cpnt_result
	/*************************************************************************************/

	/**
	* Get the row of the given hw board cpnt name (a hw board cpnt which is used in the microsc. connect)  returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* @param serialnb : serialnb  of the hw: it's  case sensitive.
	* @param len_cpnt : length of the devtype_result. If the buffer is too small, we put the size needed here.
	* @param cpnt_result :char* buffer where to put the result
	* @param cpnt_result : formatted as follows |db_column_name (db_column_type):db_column_value|
	* @param cpnt_result : db_column_type is I for int and C for char*
	* @param cpnt_result : equals to "NO_ROWS_SELECTED" if no rows selected
	* @param ErrorMessage : error message in case of failure
	* @return 0 if it is successful
	*/

	EXTERN_CONFDB
		int GetHWCpntRow_serialnb(char* serialnb,int &len_cpnt, char* cpnt_result,char* ErrorMessage)
	{
		char appliName[100]="GetHWCpntRow_serialnb";
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
		OCIDefine* def[10];
		sb4  devtypelen=0;
		sb4 devlen=0;
		sb4  locationlen=0;
		sb4 serialnblen=0;
		sb4 responlen=0;
		sb4 commentslen=0;
		sb4 dnamelen=0;
		OCIParam *parmdp;
		int status1=0;
		int snbid=0;
		int len=0;
		int replacable=0;
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
		char cpnt_status[20];
		char buffer[20];
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

			sprintf(selectdev," select nvl(t.hwname,'none')||'?',nvl(t.hwtype,'none')||'?',t.cpnt_status,nvl(t.responsible,'none')||'?',nvl(t.user_comment,'none')||'?',nvl(t.location,'none')||'?',t.serialnb||'?',d.cpntname||'?' ,t.snbid,t.replacable from %s t,%s d where t.serialnb=:dname and t.snbid=d.snbid",HW_CPNTS_TABLE,LG_CPNTS_TABLE);
			status=OCIStmtPrepare(stmthp, ociError, (text*) selectdev,(ub4) strlen(selectdev),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
			//std::cout<<" select stmt "<< selectdev <<std::endl;
			//std::cout<<" serialnb "<< serialnb <<std::endl;
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

		//std::cout<<" param " <<std::endl;
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
			//std::cout << "devtype_len" << devtypelen<< std::endl;
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
				rescode=ShowErrors (status, ociError, "DefineByPos6 unsuccessful");

			}
		}
		else	
			status=OCIDefineByPos (stmthp, &def[8], ociError,9,&snbid,sizeof(int), SQLT_INT, (dvoid *) 0, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "DefineByPos6 unsuccessful");

			}
		}
		else	
			status=OCIDefineByPos (stmthp, &def[9], ociError,10,&replacable,sizeof(int), SQLT_INT, (dvoid *) 0, 0, 0, OCI_DEFAULT);


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
			rescode1=GetDeviceStatus(status1, cpnt_status);
			len=strlen(cpnt_status)+strlen(responsible_temp)+strlen(hwtype_temp)+strlen(dname_temp)+strlen(hwname_temp)+strlen(location_temp)+strlen(serialnb_temp)+strlen(comments_temp)+strlen("|snbid (I): |hwname (C): |hwtype (C): |cpnt_status (C): |replacable (I): 1|location (C): |user_comment  (C): |responsible (C): |cpntname (C): |serialnb  (C): |");
			sprintf(buffer,"%d",snbid);
			len+=strlen(buffer);


			if(len_cpnt<len)
			{
				len++;
				len_cpnt=len;
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
				sprintf(cpnt_result,"|snbid (I):%d |hwname (C):%s |hwtype (C):%s |cpnt_status (C):%s |replacable (I):%d |location (C):%s |responsible  (C):%s |user_comment (C):%s |cpntname (C):%s |serialnb  (C):%s |" ,snbid,hwname_temp,hwtype_temp,cpnt_status,replacable,location_temp,responsible_temp,comments_temp,dname_temp,serialnb_temp);


		} 
		else 
		{
			if(rescode==0)
			{
				sprintf(selectdev," select nvl(hwname,'none')||'?',nvl(hwtype,'none')||'?',cpnt_status,nvl(responsible,'none')||'?',nvl(user_comment,'none')||'?',nvl(location,'none')||'?',serialnb||'?',snbid,replacable from %s where serialnb=:dname",HW_CPNTS_TABLE);
				status=OCIStmtPrepare(stmthp, ociError, (text*) selectdev,(ub4) strlen(selectdev),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
				//std::cout<<"selectdev "<<selectdev <<std::endl;
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
						rescode=ShowErrors (status, ociError, "DefineByPos5 unsuccessful");
					}
				}
				else
					status=OCIDefineByPos (stmthp, &def[7], ociError,8,&snbid,sizeof(int), SQLT_INT, (dvoid *) 0,0, 0,OCI_DEFAULT);

				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)
					{
						rescode=ShowErrors (status, ociError, "DefineByPos5 unsuccessful");
					}
				}
				else
					status=OCIDefineByPos (stmthp, &def[8], ociError,9,&replacable,sizeof(int), SQLT_INT, (dvoid *) 0,0, 0,OCI_DEFAULT);

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
					rescode1=GetDeviceStatus(status1, cpnt_status);
					len=strlen(cpnt_status)+strlen(responsible_temp)+strlen(hwtype_temp)+strlen(hwname_temp)+strlen(location_temp)+strlen(serialnb_temp)+strlen(comments_temp)+strlen("|hwname (C): |hwtype (C): |cpnt_status (C): |replacable (I): 1|location (C): |user_comment  (C): |responsible (C): |cpntname (C): none |serialnb  (C): |");
					sprintf(buffer,"%d",snbid);
					len+=strlen(buffer);

					if(len_cpnt<len)
					{
						len++;
						len_cpnt=len;
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
						sprintf(cpnt_result,"|hwname (C):%s |hwtype (C):%s |cpnt_status (C):%s |replacable (I):%d |location (C):%s |responsible  (C):%s |user_comment (C):%s |cpntname (C):none |serialnb  (C):%s |" ,hwname_temp,hwtype_temp,cpnt_status,replacable,location_temp,responsible_temp,comments_temp,serialnb_temp);
				}
				else
				{

					if(rescode==0)
					{
						rescode=ShowErrors (status, ociError, "OCIFETCH unsuccessful");
						strcpy(cpnt_result,"NO_ROWS_SELECTED");	
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
		if(rescode==0)
		{
			OCIReportError(ociError,appliName,ErrorMessage,0); 
		}
		else
		{
			OCIReportError(ociError,appliName,ErrorMessage,1); 
		}
		return rescode;
	}
	/*************************************************************************************/
	//return the cpnt row in cpnt_result
	/*************************************************************************************/

	/**
	* Get the row of the given hw snbid (a hw board cpnt which is used in the microsc. connect)  returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* @param snbid : snbid  of the hw: it's  case sensitive.
	* @param len_cpnt : length of the devtype_result. If the buffer is too small, we put the size needed here.
	* @param cpnt_result :char* buffer where to put the result
	* @param cpnt_result : formatted as follows |db_column_name (db_column_type):db_column_value|
	* @param cpnt_result : db_column_type is I for int and C for char*
	* @param cpnt_result : equals to "NO_ROWS_SELECTED" if no rows selected
	* @param ErrorMessage : error message in case of failure
	* @return 0 if it is successful
	*/

	EXTERN_CONFDB
		int GetHWCpntRow_snbid(int snbid,int &len_cpnt, char* cpnt_result,char* ErrorMessage)
	{
		char appliName[100]="GetHWCpntRow_snbid";
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
		OCIDefine* def[10];
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
		int replacable=0;
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
		char cpnt_status[20];
		char buffer[20];

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

			sprintf(selectdev," select nvl(t.hwname,'none')||'?',nvl(t.hwtype,'none')||'?',t.cpnt_status,nvl(t.responsible,'none')||'?',nvl(t.user_comment,'none')||'?',nvl(t.location,'none')||'?',nvl(t.serialnb,'none')||'?',nvl(d.cpntname,'none')||'?' ,t.snbid,t.replacable from %s t,%s d where t.snbid=:did and t.snbid=d.snbid",HW_CPNTS_TABLE,LG_CPNTS_TABLE);
			status=OCIStmtPrepare(stmthp, ociError, (text*) selectdev,(ub4) strlen(selectdev),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
			//std::cout<<" select stmt "<< selectdev <<std::endl;
			//std::cout<<" serialnb "<< serialnb <<std::endl;
		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else	
			status=OCIBindByName(stmthp, &bnd1p, ociError,(text*)":did",-1,(dvoid*)&snbid,sizeof(int), SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);


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

		//std::cout<<" param " <<std::endl;
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
			//std::cout << "devtype_len" << devtypelen<< std::endl;
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
				rescode=ShowErrors (status, ociError, "DefineByPos6 unsuccessful");

			}
		}
		else	
			status=OCIDefineByPos (stmthp, &def[8], ociError,9,&snbid,sizeof(int), SQLT_INT, (dvoid *) 0, 0, 0, OCI_DEFAULT);


		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "DefineByPos6 unsuccessful");

			}
		}
		else	
			status=OCIDefineByPos (stmthp, &def[9], ociError,10,&replacable,sizeof(int), SQLT_INT, (dvoid *) 0, 0, 0, OCI_DEFAULT);

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
			rescode1=GetDeviceStatus(status1, cpnt_status);
			len=strlen(cpnt_status)+strlen(responsible_temp)+strlen(hwtype_temp)+strlen(dname_temp)+strlen(hwname_temp)+strlen(location_temp)+strlen(serialnb_temp)+strlen(comments_temp)+strlen("|snbid (I): |hwname (C): |hwtype (C): |replacable (I): 1|cpnt_status (C): |location (C): |user_comment  (C): |responsible (C): |cpntname (C): |serialnb  (C): |");
			sprintf(buffer,"%d",snbid);
			len+=strlen(buffer);


			if(len_cpnt<len)
			{
				len++;
				len_cpnt=len;
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
				sprintf(cpnt_result,"|snbid (I):%d |hwname (C):%s |hwtype (C):%s |cpnt_status (C):%s |replacable (I):%d |location (C):%s |responsible  (C):%s |user_comment (C):%s |cpntname (C):%s |serialnb  (C):%s |" ,snbid,hwname_temp,hwtype_temp,cpnt_status,replacable,location_temp,responsible_temp,comments_temp,dname_temp,serialnb_temp);


		} 
		else 
		{
			if(rescode==0)
			{
				sprintf(selectdev," select nvl(hwname,'none')||'?',nvl(hwtype,'none')||'?',cpnt_status,nvl(responsible,'none')||'?',nvl(user_comment,'none')||'?',nvl(location,'none')||'?',serialnb||'?',snbid,replacable from %s where snbid=:did",HW_CPNTS_TABLE);
				status=OCIStmtPrepare(stmthp, ociError, (text*) selectdev,(ub4) strlen(selectdev),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
				//std::cout<<"selectdev "<<selectdev <<std::endl;
				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)
						rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
				}
				else	
					status=OCIBindByName(stmthp, &bnd1p, ociError,(text*)":did",-1,(dvoid*)&snbid,sizeof(int), SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);


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
						rescode=ShowErrors (status, ociError, "DefineByPos5 unsuccessful");
					}
				}
				else
					status=OCIDefineByPos (stmthp, &def[7], ociError,8,&snbid,sizeof(int), SQLT_INT, (dvoid *) 0,0, 0,OCI_DEFAULT);

				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)
					{
						rescode=ShowErrors (status, ociError, "DefineByPos5 unsuccessful");
					}
				}
				else
					status=OCIDefineByPos (stmthp, &def[8], ociError,9,&replacable,sizeof(int), SQLT_INT, (dvoid *) 0,0, 0,OCI_DEFAULT);


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
					rescode1=GetDeviceStatus(status1, cpnt_status);
					len=strlen(cpnt_status)+strlen(responsible_temp)+strlen(hwtype_temp)+strlen(hwname_temp)+strlen(location_temp)+strlen(serialnb_temp)+strlen(comments_temp)+strlen("|hwname (C): |hwtype (C): |cpnt_status (C): |replacable (I): 1|location (C): |user_comment  (C): |responsible (C): |cpntname (C): none |serialnb  (C): |");
					sprintf(buffer,"%d",snbid);
					len+=strlen(buffer);

					if(len_cpnt<len)
					{
						len++;
						len_cpnt=len;
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
						sprintf(cpnt_result,"|hwname (C):%s |hwtype (C):%s |cpnt_status (C):%s |replacable (I):%d |location (C):%s |responsible  (C):%s |user_comment (C):%s |cpntname (C):none |serialnb  (C):%s |" ,hwname_temp,hwtype_temp,cpnt_status,replacable,location_temp,responsible_temp,comments_temp,serialnb_temp);
				}
				else
				{

					if(rescode==0)
					{
						rescode=ShowErrors (status, ociError, "OCIFETCH unsuccessful");
						strcpy(cpnt_result,"NO_ROWS_SELECTED");	
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
		if(rescode==0)
		{
			OCIReportError(ociError,appliName,ErrorMessage,0); 
		}
		else
		{
			OCIReportError(ociError,appliName,ErrorMessage,1); 
		}
		return rescode;
	}
#if defined(__cplusplus) || defined(c_plusplus)
}
#endif