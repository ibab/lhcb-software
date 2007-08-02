/********************************************************************************/
//	Author: L.Abadie
// version:v3.5
// changes: handling error such that can be extended to C++: (create a class SQLException) (strlen ck + update stmt)
// file which performs bulk collect update for the device type table
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

	/************global variables for the devicetype*******************************/ 
	char* _DeviceTypeListUpdate;  //the list of the devicetype which will be stored
	int _devicetypeListLengthUpdate; //the total length of the char*
	int _max_devicetypeLengthUpdate; //the maximum length of the devicetype as a string

	char* _newDeviceTypeListUpdate;  //the list of the devicetype which will be stored
	int _newdevicetypeListLengthUpdate; //the total length of the char*
	int _max_newdevicetypeLengthUpdate; //the maximum length of the devicetype as a string

	char* _oldDeviceTypeListUpdate;  //the list of the devicetype which will be stored
	int _olddevicetypeListLengthUpdate; //the total length of the char*
	int _max_olddevicetypeLengthUpdate; //the maximum length of the devicetype as a string

	char* _DescriptionListUpdate;
	int _descriptionListLengthUpdate;
	int _max_descriptionLengthUpdate;
	int* _NbrofinputListUpdate;
	int* _NbrofoutputListUpdate;
	int _devicetypeNbElementUpdate;// total nb of devicetype to update
	int _newdevicetypeNbElementUpdate;// total nb of devicetype to insert
	int _olddevicetypeNbElementUpdate;// total nb of devicetype to insert

	int _descNbElementUpdate;
	int _rgbcolorNbElementUpdate;
	int _nbinElementUpdate;
	int _nboutElementUpdate;
	char* _rgbcolorListUpdate;
	int _rgbcolorListLengthUpdate;
	int _max_rgbcolorLengthUpdate;

	int* _devicetype_nullvalueUpdate;
	int _devicetypeNbElUpdate;

	int* _newdevicetype_nullvalueUpdate;
	int _newdevicetypeNbElUpdate;

	int* _olddevicetype_nullvalueUpdate;
	int _olddevicetypeNbElUpdate;


	int* _description_nullvalueUpdate;
	int _descriptionNbElUpdate;

	int* _rgbcolor_nullvalueUpdate;
	int _rgbcolorNbElUpdate;
	int FIRST_TIME_DEVICETYPEUPDATE=0;
	int FIRST_TIME_DEVICETYPEUPDATE1=0;
	int FIRST_TIME_SYSDEVTYPEUPDATE=0;







	char* _SysDeviceTypeListUpdate;
	int* _SysTypeUpdate;
	int* _SysdeviceTypeList_nullvalueUpdate;
	int _SysTypeNbElementUpdate;
	int _SysdeviceTypeListNbElUpdate;
	int _SysdeviceTypeListLengthUpdate;
	int _SysdeviceTypeNbElementUpdate;
	int _max_SysdeviceTypeLengthUpdate;

	int freeSysDeviceTypeUpdate()
	{
		int status=0;
		_SysDeviceTypeListUpdate=(char*)realloc(_SysDeviceTypeListUpdate,0*sizeof(char));
		_SysTypeUpdate=(int*)realloc(_SysTypeUpdate,0*sizeof(int));
		_SysdeviceTypeList_nullvalueUpdate=(int*)realloc(_SysdeviceTypeList_nullvalueUpdate,0*sizeof(int));
		FIRST_TIME_SYSDEVTYPEUPDATE=0;

		return status;
	}
	int freeDeviceTypeUpdate()
	{
		int status=0;
		_DeviceTypeListUpdate=(char*)realloc(_DeviceTypeListUpdate,0*sizeof(char));
		_DeviceTypeListUpdate=NULL;
		_rgbcolorListUpdate=(char*)realloc(_rgbcolorListUpdate,0*sizeof(char));
		_rgbcolorListUpdate=NULL;
		_DescriptionListUpdate=(char*)realloc(_DescriptionListUpdate,0*sizeof(char));
		_DescriptionListUpdate=NULL;
		_NbrofinputListUpdate=(int*)realloc(_NbrofinputListUpdate,0*sizeof(int));
		_NbrofinputListUpdate=NULL;
		_NbrofoutputListUpdate=(int*)realloc(_NbrofoutputListUpdate,0*sizeof(int));
		_NbrofoutputListUpdate=NULL;
		_description_nullvalueUpdate=(int*)realloc(_description_nullvalueUpdate,0*sizeof(int));
		_description_nullvalueUpdate=NULL;
		_devicetype_nullvalueUpdate=(int*)realloc(_devicetype_nullvalueUpdate,0*sizeof(int));
		_devicetype_nullvalueUpdate=NULL;
		_rgbcolor_nullvalueUpdate=(int*)realloc(_rgbcolor_nullvalueUpdate,0*sizeof(int));
		_rgbcolor_nullvalueUpdate=NULL;
		FIRST_TIME_DEVICETYPEUPDATE=0;
		return status;
	}

	int freeDeviceTypeUpdate1()
	{
		int status=0;
		_newDeviceTypeListUpdate=(char*)realloc(_newDeviceTypeListUpdate,0*sizeof(char));
		_newDeviceTypeListUpdate=NULL;
		_newdevicetype_nullvalueUpdate=(int*)realloc(_newdevicetype_nullvalueUpdate,0*sizeof(int));
		_newdevicetype_nullvalueUpdate=NULL;

		_oldDeviceTypeListUpdate=(char*)realloc(_oldDeviceTypeListUpdate,0*sizeof(char));
		_oldDeviceTypeListUpdate=NULL;
		_olddevicetype_nullvalueUpdate=(int*)realloc(_olddevicetype_nullvalueUpdate,0*sizeof(int));
		_olddevicetype_nullvalueUpdate=NULL;

		FIRST_TIME_DEVICETYPEUPDATE1=0;
		return status;
	}


	/**
	* Update the devicetype  returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* @param devicetype : name of the devicetype you want to update
	* @param description : new value of the description, if no change you put  "none" (lower case)
	* @param nbrofinput : new value of nbrofinput: put -1 if you don't want to change it
	* @param nbrofoutput : new value of nbrofoutput: put -1 if you don't want to change it
	* @param rgbcolor : new value of the rgbcolor of the devicetypefor display if no change you put  none (lower case)
	* @param first_time1 : 1 if it's your first MultipleDevice update (save it into the database), 0 otherwise
	* @param last_rows1 : 1 if it's your MultipleDevice last update (save it into the database), 0 otherwise
	* @param ErrMess : error message in case of failure
	* @return 0 if it is successful
	*/

	/******************************************************************/
	EXTERN_CONFDB
		int UpdateMultipleDeviceTypeAttributes(char* devicetype,char* description,int nbrofinput,int nbrofoutput,char* rgbcolor,int first_time1,int last_rows1,char* ErrMess)
	{
		char appliName[100]="UpdateMultipleDeviceTypeAttributes";
		char sqlstmt[1000];
		OCIStmt *hstmt;
		OCIBind  *bndp[5]; 
		int rescode=0;
		int free_mem=0;
		int res_query=0;
		sword status=0;
		int force_insert=0;
		int first_time=first_time1;
		int last_rows=last_rows1;
		char* dtype=NULL;
		int i=0;
		char* descrip=NULL;
		char* rgbcol=NULL;
		int* numrows_inserted=NULL;
		int numrows=0;
		//int devtype_len=0;
		//int descrip_len=0;
		//int rgbcol_len=0;
		//int in_len=0;
		//int out_len=0;
		OCIBind  *bndp3 = (OCIBind *) 0;

		int dtype1=null_charptr(devicetype);
		int descrip1=null_charptr(description);
		int rgbcol1=null_charptr(rgbcolor);
		if(FIRST_TIME_DEVICETYPEUPDATE==1 && _DeviceTypeListUpdate==NULL)
			first_time=1;
		res_query=AppendString(devicetype,_DeviceTypeListUpdate,_devicetypeListLengthUpdate,_devicetypeNbElementUpdate,_max_devicetypeLengthUpdate,first_time);
		res_query+=AppendString(description,_DescriptionListUpdate,_descriptionListLengthUpdate,_descNbElementUpdate,_max_descriptionLengthUpdate,first_time);
		res_query+=AppendString(rgbcolor,_rgbcolorListUpdate,_rgbcolorListLengthUpdate,_rgbcolorNbElementUpdate,_max_rgbcolorLengthUpdate,first_time);

		res_query+=AppendInt(nbrofinput,_NbrofinputListUpdate,_nbinElementUpdate,first_time);
		res_query+=AppendInt(nbrofoutput,_NbrofoutputListUpdate,_nboutElementUpdate,first_time);

		res_query+=AppendInt(descrip1,_description_nullvalueUpdate,_descriptionNbElUpdate,first_time);
		res_query+=AppendInt(dtype1,_devicetype_nullvalueUpdate,_devicetypeNbElUpdate,first_time);
		res_query+=AppendInt(rgbcol1,_rgbcolor_nullvalueUpdate,_rgbcolorNbElUpdate,first_time);

		if(first_time==1)
			FIRST_TIME_DEVICETYPEUPDATE=1;
		status=res_query;
		if(last_rows!=1 && _devicetypeNbElementUpdate==MAXROWS)
		{
			force_insert=1;
			last_rows=1;
		}
		if(last_rows==1 && res_query==0 && ociEnv!=0 && FIRST_TIME_DEVICETYPEUPDATE==1)
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
				status+=freeDeviceTypeUpdate();

				if(dtype!=NULL)
					free(dtype);
				if(descrip!=NULL)
					free(descrip);
				if(rgbcol!=NULL)
					free(rgbcol);
				free(errmessg1);
				free(errmessg2);
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
			{
				sprintf(sqlstmt,
					"BEGIN update %s set description=decode(nvl(:descrip,'none'),'none',description,:descrip),nbrofinput=decode(:nbin,-1,nbrofinput,:nbin),nbrofoutput=decode(:nbout,-1,nbrofoutput,:nbout),rgbcolor=decode(nvl(:rgbcol,'none'),'none',rgbcolor,:rgbcol),last_update=sysdate,user_update='%s' where devicetype=:dtype_old; :numrows:=%s; END;",DEVICETYPE_TABLE,login,SQLROWCOUNT);
				status=OCIStmtPrepare(hstmt, ociError, (text*)sqlstmt, (ub4)strlen((char *)sqlstmt), (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
			}
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
			}
			else
			{
				//devtype_len=strlen(devicetype);
				//descrip_len=strlen(description);
				//rgbcol_len=strlen(rgbcolor);
				//in_len=sizeof(nbrofinput);
				//out_len=sizeof(nbrofoutput);
				dtype=(char*)malloc(_devicetypeNbElementUpdate*_max_devicetypeLengthUpdate*sizeof(char));
				if(dtype!=NULL)
					status+=NormalizeVector(_DeviceTypeListUpdate, _devicetypeNbElementUpdate,_max_devicetypeLengthUpdate,dtype);
				descrip=(char*)malloc(_descNbElementUpdate*_max_descriptionLengthUpdate*sizeof(char));
				if(descrip!=NULL)
					status+=NormalizeVector(_DescriptionListUpdate, _descNbElementUpdate,_max_descriptionLengthUpdate,descrip);
				rgbcol=(char*)malloc(_rgbcolorNbElementUpdate*_max_rgbcolorLengthUpdate*sizeof(char));
				if(rgbcol!=NULL)
					status+=NormalizeVector(_rgbcolorListUpdate, _rgbcolorNbElementUpdate,_max_rgbcolorLengthUpdate,rgbcol);

				free_mem=1;
				numrows_inserted=(int*)malloc(sizeof(int)*_devicetypeNbElementUpdate);

				if(dtype==NULL || descrip==NULL||numrows_inserted==NULL||rgbcol==NULL)
				{

					status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
					rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");
					GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
					status+=freeDeviceTypeUpdate();
					if(dtype!=NULL)
						free(dtype);
					if(descrip!=NULL)
						free(descrip);
					if(rgbcol!=NULL)
						free(rgbcol);
					if(numrows_inserted!=NULL)
						free(numrows_inserted);
					return -1;
				}
				else
				{
					for(i=0;i<_devicetypeNbElementUpdate;i++)
						numrows_inserted[i]=0;
					status =OCIBindByName(hstmt, &bndp[0], ociError,(text*) ":dtype_old", -1,(dvoid*)dtype, _max_devicetypeLengthUpdate,  SQLT_STR, (dvoid *) &_devicetype_nullvalueUpdate[0],(ub2 *)0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
				}
			}
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status = OCIBindByName (hstmt, &bndp[1], ociError,(text*) ":nbin", -1,(dvoid*)&_NbrofinputListUpdate[0],sizeof(nbrofinput), SQLT_INT, 0, 0, 0, 0, 0, OCI_DEFAULT);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status = OCIBindByName (hstmt, &bndp[2], ociError, (text*) ":nbout", -1,(dvoid*)&_NbrofoutputListUpdate[0],sizeof(nbrofoutput),SQLT_INT, 0, 0, 0, 0, 0, OCI_DEFAULT);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status = OCIBindByName(hstmt, &bndp[3], ociError, (text*) ":descrip", -1,(dvoid*)descrip,_max_descriptionLengthUpdate, SQLT_STR,(dvoid *)  &_description_nullvalueUpdate[0], (ub2*)0, 0, 0, 0, OCI_DEFAULT);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status = OCIBindByName(hstmt, &bndp[4], ociError, (text*) ":rgbcol", -1,(dvoid*)rgbcol,_max_rgbcolorLengthUpdate, SQLT_STR,(dvoid *)  &_rgbcolor_nullvalueUpdate[0], (ub2*)0, 0, 0, 0, OCI_DEFAULT);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp3, ociError,(text*) ":numrows", -1,(dvoid*)&numrows_inserted[0], sizeof(nbrofoutput),SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);


			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else	
				status=OCIBindArrayOfStruct(bndp[0], ociError, _max_devicetypeLengthUpdate, sizeof(int),0, 0);

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
				status=OCIBindArrayOfStruct(bndp[2], ociError, sizeof(int), 0, 0, 0);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp[3], ociError, _max_descriptionLengthUpdate, sizeof(int), 0, 0);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp[4], ociError, _max_rgbcolorLengthUpdate, sizeof(int), 0, 0);

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
				status= OCIStmtExecute(ociHdbc, hstmt, ociError, _devicetypeNbElementUpdate, 0, 0, 0, OCI_DEFAULT );

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
			}
			else
			{
				numrows=-1;
				for(i=0;i< _devicetypeNbElementUpdate;i++)
				{
					if(numrows_inserted[i]==0)
					{
						numrows=0;
						i=  _devicetypeNbElementUpdate+5;
					}
					else
						numrows=numrows_inserted[i];
				}
				if(numrows==0)
				{
					status = OCITransCommit(ociHdbc, ociError, 0);
					status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
					status+=freeDeviceTypeUpdate();
					if(dtype!=NULL)
						free(dtype);
					if(rgbcol!=NULL)
						free(rgbcol);
					if(descrip!=NULL)
						free(descrip);
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
			status+=freeDeviceTypeUpdate();
			if(rescode!=0)
				if(ociError!=0)
					OCIReportError(ociError,appliName, ErrMess,1); 
				else
					GetErrorMess(appliName,  "NOT CONNECTED TO ANY DB",ErrMess,1); 
			else
				OCIReportError(ociError,appliName, ErrMess,0);  

			if(dtype!=NULL)
				free(dtype);

			if(descrip!=NULL)
				free(descrip);
			if(rgbcol!=NULL)
				free(rgbcol);
			if(numrows_inserted!=NULL)
				free(numrows_inserted);

			if(force_insert==1 && rescode==0)
				FIRST_TIME_DEVICETYPEUPDATE=1;

		}
		else
		{
			//std::cout<<"before if res_query null "<<res_query<<" rescode "<<rescode<<std::endl;

			if(res_query!=0)
			{
				status=freeDeviceTypeUpdate();
				rescode=res_query;
				GetErrorMess(appliName, "Cache Problem",ErrMess,1);
			}
			if(FIRST_TIME_DEVICETYPEUPDATE!=1)
			{
				status=freeDeviceTypeUpdate();
				rescode=-1;
				GetErrorMess(appliName, "CACHE HAS NOT BEEN INITIALIZED",ErrMess,1);
			}
			if(ociEnv==0)
			{
				status=freeDeviceTypeUpdate();
				rescode=-1;
				GetErrorMess(appliName, "NOT CONNECTED TO ANY DB",ErrMess,1);
			}
			if(ociEnv!=0 && FIRST_TIME_DEVICETYPEUPDATE==1 && res_query==0)
			{
				status=0;
				GetErrorMess(appliName, "NO_ERROR",ErrMess,0);
			}
			//status+=res_query;
			//std::cout<<"res_query null "<<res_query<<" rescode "<<rescode<<std::endl;


		}
		return (rescode+status);
	}

	/**
	* Update the devicetype  returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* @param devicetype_old : name of the devicetype you want to update
	* @param devicetype : new value of the devicetype, 
	* @param first_time1 : 1 if it's your first MultipleDevice update (save it into the database), 0 otherwise
	* @param last_rows1 : 1 if it's your MultipleDevice last update (save it into the database), 0 otherwise
	* @param ErrMess : error message in case of failure
	* @return 0 if it is successful
	*/

	/******************************************************************/
	EXTERN_CONFDB
		int UpdateMultipleDeviceTypes(char* devicetype_old,char* devicetype,int first_time1,int last_rows1,char* ErrMess)
	{
		char appliName[100]="UpdateMultipleDeviceTypes";
		char sqlstmt[1000];
		OCIStmt *hstmt;
		OCIBind  *bndp[2]; 
		int rescode=0;
		int res_query=0;
		sword status=0;
		char* dtype=NULL;
		int force_insert=0;
		int first_time=first_time1;
		int last_rows=last_rows1;
		int i=0;
		char* dtype_old=NULL;
		int* numrows_inserted=NULL;
		int numrows=0;
		OCIBind  *bndp3 = (OCIBind *) 0;

		int dtype1=null_charptr(devicetype);
		int dtype1_old=null_charptr(devicetype_old);
		if(FIRST_TIME_DEVICETYPEUPDATE1==1 && _newDeviceTypeListUpdate==NULL)
			first_time=1;
		res_query=AppendString(devicetype,_newDeviceTypeListUpdate,_newdevicetypeListLengthUpdate,_newdevicetypeNbElementUpdate,_max_newdevicetypeLengthUpdate,first_time);
		res_query+=AppendString(devicetype_old,_oldDeviceTypeListUpdate,_olddevicetypeListLengthUpdate,_olddevicetypeNbElementUpdate,_max_olddevicetypeLengthUpdate,first_time);

		res_query+=AppendInt(dtype1,_newdevicetype_nullvalueUpdate,_newdevicetypeNbElUpdate,first_time);
		res_query+=AppendInt(dtype1_old,_olddevicetype_nullvalueUpdate,_olddevicetypeNbElUpdate,first_time);

		if(first_time==1)
			FIRST_TIME_DEVICETYPEUPDATE1=1;
		status=res_query;

		if(last_rows!=1 && _newdevicetypeNbElementUpdate==MAXROWS)
		{
			last_rows=1;
			force_insert=1;
		}
		if(last_rows==1 && res_query==0 && ociEnv!=0 && FIRST_TIME_DEVICETYPEUPDATE1==1)
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
				status+=freeDeviceTypeUpdate1();

				if(dtype!=NULL)
					free(dtype);
				if(dtype_old!=NULL)
					free(dtype);
				free(errmessg1);
				free(errmessg2);
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
			{
				sprintf(sqlstmt,
					"BEGIN update %s set devicetype=:dtype,last_update=sysdate,user_update='%s' where devicetype=:dtype_old; :numrows:=%s; END;",DEVICETYPE_TABLE,login,SQLROWCOUNT);
				status=OCIStmtPrepare(hstmt, ociError, (text*)sqlstmt, (ub4)strlen((char *)sqlstmt), (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
			}
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
			}
			else
			{
				//devtype_len=strlen(devicetype);
				//descrip_len=strlen(description);
				//rgbcol_len=strlen(rgbcolor);
				//in_len=sizeof(nbrofinput);
				//out_len=sizeof(nbrofoutput);
				dtype=(char*)malloc(_newdevicetypeNbElementUpdate*_max_newdevicetypeLengthUpdate*sizeof(char));
				if(dtype!=NULL)
					status+=NormalizeVector(_newDeviceTypeListUpdate, _newdevicetypeNbElementUpdate,_max_newdevicetypeLengthUpdate,dtype);

				dtype_old=(char*)malloc(_olddevicetypeNbElementUpdate*_max_olddevicetypeLengthUpdate*sizeof(char));
				if(dtype_old!=NULL)
					status+=NormalizeVector(_oldDeviceTypeListUpdate, _olddevicetypeNbElementUpdate,_max_olddevicetypeLengthUpdate,dtype_old);

				numrows_inserted=(int*)malloc(sizeof(int)*_olddevicetypeNbElementUpdate);

				if(dtype==NULL || numrows_inserted==NULL||dtype_old==NULL)
				{

					status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
					rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");
					GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
					status+=freeDeviceTypeUpdate1();
					if(dtype!=NULL)
						free(dtype);
					if(dtype_old!=NULL)
						free(dtype_old);
					if(numrows_inserted!=NULL)
						free(numrows_inserted);
					return -1;
				}
				else
				{
					for(i=0;i<_olddevicetypeNbElementUpdate;i++)
						numrows_inserted[i]=0;
					status =OCIBindByName(hstmt, &bndp[0], ociError,(text*) ":dtype", -1,(dvoid*)dtype, _max_newdevicetypeLengthUpdate,  SQLT_STR, (dvoid *) &_newdevicetype_nullvalueUpdate[0],(ub2 *)0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
				}
			}
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp[1], ociError,(text*) ":dtype_old", -1,(dvoid*)dtype_old, _max_olddevicetypeLengthUpdate,  SQLT_STR, (dvoid *) &_olddevicetype_nullvalueUpdate[0],(ub2 *)0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);


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
				status=OCIBindArrayOfStruct(bndp[0], ociError, _max_newdevicetypeLengthUpdate, sizeof(int),0, 0);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else	
				status=OCIBindArrayOfStruct(bndp[1], ociError, _max_olddevicetypeLengthUpdate, sizeof(int),0, 0);

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
				status= OCIStmtExecute(ociHdbc, hstmt, ociError, _olddevicetypeNbElementUpdate, 0, 0, 0, OCI_DEFAULT );

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
			}
			else
			{
				numrows=-1;
				for(i=0;i< _olddevicetypeNbElementUpdate;i++)
				{
					if(numrows_inserted[i]==0)
					{
						numrows=0;
						i=  _olddevicetypeNbElementUpdate+5;
					}
					else
						numrows=numrows_inserted[i];
				}
				if(numrows==0)
				{
					status = OCITransCommit(ociHdbc, ociError, 0);
					status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
					status+=freeDeviceTypeUpdate1();
					if(dtype!=NULL)
						free(dtype);
					if(dtype_old!=NULL)
						free(dtype_old);


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
			status+=freeDeviceTypeUpdate1();
			if(rescode!=0)
				if(ociError!=0)
					OCIReportError(ociError,appliName, ErrMess,1); 
				else
					GetErrorMess(appliName,  "NOT CONNECTED TO ANY DB",ErrMess,1); 
			else
				OCIReportError(ociError,appliName, ErrMess,0);  

			if(dtype!=NULL)
				free(dtype);
			if(dtype_old!=NULL)
				free(dtype_old);

			if(numrows_inserted!=NULL)
				free(numrows_inserted);

			if(force_insert==1 && rescode==0)
				FIRST_TIME_DEVICETYPEUPDATE1=1;


		}
		else
		{
			//std::cout<<"before if res_query null "<<res_query<<" rescode "<<rescode<<std::endl;

			if(res_query!=0)
			{
				status=freeDeviceTypeUpdate1();
				rescode=res_query;
				GetErrorMess(appliName, "Cache Problem",ErrMess,1);
			}
			if(FIRST_TIME_DEVICETYPEUPDATE1!=1)
			{
				status=freeDeviceTypeUpdate1();
				rescode=-1;
				GetErrorMess(appliName, "CACHE HAS NOT BEEN INITIALIZED",ErrMess,1);
			}
			if(ociEnv==0)
			{
				status=freeDeviceTypeUpdate1();
				rescode=-1;
				GetErrorMess(appliName, "NOT CONNECTED TO ANY DB",ErrMess,1);
			}
			if(ociEnv!=0 && FIRST_TIME_DEVICETYPEUPDATE1==1 && res_query==0)
			{
				status=0;
				GetErrorMess(appliName, "NO_ERROR",ErrMess,0);
			}
			//status+=res_query;
			//std::cout<<"res_query null "<<res_query<<" rescode "<<rescode<<std::endl;


		}
		return (rescode+status);
	}

	/******************************************************/
	/**
	* Update the system_name parameter of a device type  returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* @param devicetype : name of the device
	* @param new_systemList : new list of system, UPPER CASE, separated with a coma
	* @param first_time1 : 1 if it's your first MultipleDeviceNodeUsed update (save it into the database), 0 otherwise
	* @param last_rows1 : 1 if it's your MultipleDeviceNodeUsed last update (save it into the database), 0 otherwise
	* @param ErrMess : error message in case of failure
	* @return 0 if it is successful
	*/
	/*********************************************************************/
	EXTERN_CONFDB int UpdateMultipleDeviceTypeSystemList(char* devicetype,char* new_systemList,int first_time1,int last_rows1,char* ErrMess)
	{

		//extern "C" __declspec(dllexport)
		char appliName[100]="UpdateMultipleDeviceTypeSystemList";
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
		int dname1=null_charptr(devicetype);
		if(FIRST_TIME_SYSDEVTYPEUPDATE==1 && _SysDeviceTypeListUpdate==NULL)
			first_time;
		if(first_time==1)
		{
			freeSysDeviceTypeUpdate();
			FIRST_TIME_SYSDEVTYPEUPDATE=1;

		}
		res_query=AppendString(devicetype,_SysDeviceTypeListUpdate,_SysdeviceTypeListLengthUpdate,_SysdeviceTypeNbElementUpdate,_max_SysdeviceTypeLengthUpdate,first_time);

		res_query+=AppendInt(sysID,_SysTypeUpdate,_SysTypeNbElementUpdate,first_time);

		res_query+=AppendInt(dname1,_SysdeviceTypeList_nullvalueUpdate,_SysdeviceTypeListNbElUpdate,first_time);


		status=res_query;

		if(last_rows!=1 && _SysdeviceTypeNbElementUpdate==MAXROWS)
		{
			force_insert=1;
			last_rows=1;

		}

		if(last_rows==1 && res_query==0 && ociEnv!=0 && FIRST_TIME_SYSDEVTYPEUPDATE==1 )
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
				status+=freeSysDeviceTypeUpdate();
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
				sprintf(sqlstmt,"BEGIN update %s set system_name=:sysid,last_update=sysdate,user_update='%s' where devicetype=:dname; :numrows:=%s; END;",DEVICETYPE_TABLE,login,SQLROWCOUNT);
				status=OCIStmtPrepare(hstmt, ociError, (text*)sqlstmt, (ub4)strlen((char *)sqlstmt), (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
				//std::cout<<"sql stmt "<<sqlstmt<<std::endl;

			}
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
			}
			else
			{
				dname=(char*)malloc(_SysdeviceTypeNbElementUpdate*_max_SysdeviceTypeLengthUpdate*sizeof(char));
				if(dname!=NULL)
					status+=NormalizeVector(_SysDeviceTypeListUpdate, _SysdeviceTypeNbElementUpdate,_max_SysdeviceTypeLengthUpdate,dname);

				numrows_inserted=(int*)malloc(sizeof(int)*_SysdeviceTypeNbElementUpdate);
				numrows_inserted1=(int*)malloc(sizeof(int)*_SysdeviceTypeNbElementUpdate);
				if(dname==NULL ||numrows_inserted==NULL ||numrows_inserted1==NULL)
				{

					rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");
					GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
					status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
					status+=freeSysDeviceTypeUpdate();
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
					for(i=0;i<_SysdeviceTypeNbElementUpdate;i++)
					{
						numrows_inserted[i]=0;
						numrows_inserted1[i]=0;
					}
					numrows_inserted1[_SysdeviceTypeNbElementUpdate-1]=1;
				}
				status =OCIBindByName(hstmt, &bndp[0], ociError,(text*) ":dname", -1,(dvoid*)dname, _max_SysdeviceTypeLengthUpdate,  SQLT_STR, (dvoid *) & _SysdeviceTypeList_nullvalueUpdate[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);

			}
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp[1], ociError,(text*) ":sysid", -1,(dvoid*)&_SysTypeUpdate[0], sizeof(int),  SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);

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
				status=OCIBindArrayOfStruct(bndp[0], ociError,_max_SysdeviceTypeLengthUpdate, sizeof(int),0, 0);

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
				status= OCIStmtExecute(ociHdbc, hstmt, ociError, _SysdeviceTypeNbElementUpdate, 0, 0, 0, OCI_DEFAULT );
			//std::cout<<"res_query "<<res_query<<std::endl;
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
			}
			else
			{
				numrows=-1;
				for(i=0;i< _SysdeviceTypeNbElementUpdate;i++)
				{
					if(numrows_inserted[i]==0)
					{
						numrows=0;
						i= _SysdeviceTypeNbElementUpdate+5;
					}
					else
						numrows=numrows_inserted[i];
				}
				if(res_query!=0)
				{
					status = OCITransRollback(ociHdbc, ociError, 0);
					status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
					status+=freeSysDeviceTypeUpdate();
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
			status+=freeSysDeviceTypeUpdate();
			//std::cout<<"after free device"<<std::endl;
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
				FIRST_TIME_SYSDEVTYPEUPDATE=1;
		}
		else
		{
			if(res_query!=0)
			{
				status=freeSysDeviceTypeUpdate();
				GetErrorMess(appliName, "Cache Problem",ErrMess,1);
			}
			if(FIRST_TIME_SYSDEVTYPEUPDATE!=1)
			{
				status=freeSysDeviceTypeUpdate();
				res_query=-1;
				GetErrorMess(appliName, "CACHE HAS NOT BEEN INITIALIZED",ErrMess,1);
			}
			if(ociEnv==0)
			{
				status=freeSysDeviceTypeUpdate();
				res_query=-1;
				GetErrorMess(appliName, "NOT CONNECTED TO ANY DB",ErrMess,1);
			}
			if(ociEnv!=0 && FIRST_TIME_SYSDEVTYPEUPDATE==1 && res_query==0)
			{
				status=0;
				GetErrorMess(appliName, "NO_ERROR",ErrMess,0);
			}
			status+=res_query;
		}
		//std::cout<<"end of fct "<<std::endl;
		return (status+rescode);
	}


#if defined(__cplusplus) || defined(c_plusplus)
}
#endif