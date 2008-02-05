/********************************************************************************/
//	Author: L.Abadie
// version:v3.5
// changes: handling error such that can be extended to C++: (create a class SQLException) (strlen ck + update stmt)
// file which performs bulk collect insert for the macIP table
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

	/**************************GV for macIP************************************************/






	char* _ip_addListUpdate;
	int _max_ip_add_lenUpdate;
	int _ip_addNbElementUpdate;
	int _ip_addList_lenUpdate;

	int* _ipadd_nullvalueUpdate;
	int _ipaddNbElUpdate;

	char* _ip1_addListUpdate;
	int _max_ip1_add_lenUpdate;
	int _ip1_addNbElementUpdate;
	int _ip1_addList_lenUpdate;

	int* _ip1_nullvalueUpdate;
	int _ip1NbElUpdate;
	char* _subnet_maskListUpdate;
	int _max_subnet_mask_lenUpdate;
	int _subnet_maskNbElementUpdate;
	int _subnet_maskList_lenUpdate;

	int* _subnetmask_nullvalueUpdate;
	int _subnetmaskNbElUpdate;

	int _oldip_addNbElementUpdate;
	int _max_oldip_add_lenUpdate;
	char* _oldip_addListUpdate;
	int* _oldip_nullvalueUpdate;
	int _oldip_addList_lenUpdate;
	int _oldipNbElUpdate;

	char* _ipnameListUpdate;
	int _max_ipname_lenUpdate;
	int _ipnameNbElementUpdate;
	int _ipnameList_lenUpdate;

	int* _ipname_nullvalueUpdate;
	int _ipnameNbElUpdate;



	int FIRST_TIME_MACUPDATE=0;
	int FIRST_TIME_MACUPDATE3=0;
	int FIRST_TIME_MACUPDATE2=0;

	int freeIPEthernetUpdate()
	{
		int status=0;

		_subnet_maskListUpdate=(char*)realloc(_subnet_maskListUpdate,0*sizeof(char));
		_subnet_maskListUpdate=NULL;


		_ipnameListUpdate=(char*)realloc(_ipnameListUpdate,0*sizeof(char));
		_ipnameListUpdate=NULL;

		_ip_addListUpdate=(char*)realloc(_ip_addListUpdate,0*sizeof(char));
		_ip_addListUpdate=NULL;
		_ipname_nullvalueUpdate=(int*)realloc(_ipname_nullvalueUpdate,0*sizeof(int));
		_ipname_nullvalueUpdate=NULL;

		_subnetmask_nullvalueUpdate=(int*)realloc(_subnetmask_nullvalueUpdate,0*sizeof(int));
		_subnetmask_nullvalueUpdate=NULL;


		_ipadd_nullvalueUpdate=(int*)realloc(_ipadd_nullvalueUpdate,0*sizeof(int));
		_ipadd_nullvalueUpdate=NULL;

		FIRST_TIME_MACUPDATE=0;
		return status;
	}

	int freeIPEthernetUpdate3()
	{
		int status=0;


		_ip1_nullvalueUpdate=(int*)realloc(_ip1_nullvalueUpdate,0*sizeof(int));
		_ip1_nullvalueUpdate=NULL;


		_ip1_addListUpdate=(char*)realloc(_ip1_addListUpdate,0*sizeof(char));
		_ip1_addListUpdate=NULL;

		_oldip_nullvalueUpdate=(int*)realloc(_oldip_nullvalueUpdate,0*sizeof(int));
		_oldip_nullvalueUpdate=NULL;


		_oldip_addListUpdate=(char*)realloc(_oldip_addListUpdate,0*sizeof(char));
		_oldip_addListUpdate=NULL;

		FIRST_TIME_MACUPDATE3=0;
		return status;
	}

	/**
	* Update MAC entries,  returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* @param ip_add : key entry : you want to update the info about this IPaddress
	* @param subnet_mask : new value : put  "none" if no changes
	* @param ipname : new value : put  "none" if no changes
	* @param first_time1 : 1 if it's your first MacIP insert (save it into the database), 0 otherwise
	* @param last_rows1 : 1 if it's your last MacIP insert (save it into the database), 0 otherwise
	* @param ErrMess : error message in case of failure
	* @return 0 if it is successful
	*/ 



	EXTERN_CONFDB
		int UpdateMultipleAttributeMacIPs(char* ip_add,char* subnet_mask,char* ipname,int first_time1,int last_rows1,char* ErrMess)
	{
		char appliName[100]="UpdateMultipleAttributeMacIPs";
		int* numrows_inserted=NULL;
		int numrows=0;
		OCIBind  *bndp3 = (OCIBind *) 0;
		char sqlstmt[2000];
		OCIStmt *hstmt;
		OCIBind  *bndp[3]; 
		int res_query=0;
		int free_mem=0;
		int rescode=0;
		sword status=0;
		char* ipadd=NULL;
		char* ethadd=NULL;
		char* iname=NULL;
		char* submask=NULL;
		char* vlanprefix=NULL;
		int i=0;
		int force_insert=0;
		int first_time=first_time1;
		int last_rows=last_rows1;

		int ipadd1=null_charptr(ip_add);
		int submask1=null_charptr(subnet_mask);
		int iname1=null_charptr(ipname);

		if(FIRST_TIME_MACUPDATE==1 && _ip_addListUpdate==NULL)
			first_time=1;

		res_query+=AppendString(ip_add,_ip_addListUpdate,_ip_addList_lenUpdate,_ip_addNbElementUpdate,_max_ip_add_lenUpdate,first_time);
		res_query+=AppendString(subnet_mask,_subnet_maskListUpdate,_subnet_maskList_lenUpdate,_subnet_maskNbElementUpdate,_max_subnet_mask_lenUpdate,first_time);
		res_query+=AppendString(ipname,_ipnameListUpdate,_ipnameList_lenUpdate,_ipnameNbElementUpdate,_max_ipname_lenUpdate,first_time);


		res_query+=AppendInt(iname1,_ipname_nullvalueUpdate,_ipnameNbElUpdate,first_time);
		res_query+=AppendInt(submask1,_subnetmask_nullvalueUpdate,_subnetmaskNbElUpdate,first_time);
		res_query+=AppendInt(ipadd1,_ipadd_nullvalueUpdate,_ipaddNbElUpdate,first_time);


		status=res_query;
		if(first_time==1)
			FIRST_TIME_MACUPDATE=1;

		if(last_rows!=1 && _ip_addNbElementUpdate==MAXROWS)
		{
			force_insert=1;
			last_rows=1;
		}

		if(last_rows==1 && res_query==0 && ociEnv!=0 && FIRST_TIME_MACUPDATE==1)
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


				if(ipadd!=NULL)
					free(ipadd);
				if(iname!=NULL)
					free(iname);
				if(submask!=NULL)
					free(submask);
				status=freeIPEthernetUpdate();
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
				sprintf(sqlstmt,"BEGIN  update %s set subnet_info=decode(nvl(:submask,'none'),'none',subnet_info,:submask),ipname=decode(nvl(:ipname,'none'),'none',ipname,:ipname),LAST_UPDATE=sysdate,author='%s' where ipaddress=:ipadd; :numrows:=%s; end; ",IPINFO_TABLE,login,SQLROWCOUNT);
			if (OCIStmtPrepare(hstmt, ociError, (text*)sqlstmt, (ub4)strlen((char *)sqlstmt), (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT))
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
			}
			else
			{
				submask=(char*)malloc(_subnet_maskNbElementUpdate*_max_subnet_mask_lenUpdate*sizeof(char));
				iname=(char*)malloc(_ipnameNbElementUpdate*_max_ipname_lenUpdate*sizeof(char));
				ipadd=(char*)malloc(_ip_addNbElementUpdate*_max_ip_add_lenUpdate*sizeof(char));
				if(submask!=NULL)
					status+=NormalizeVector(_subnet_maskListUpdate,_subnet_maskNbElementUpdate,_max_subnet_mask_lenUpdate,submask);
				if(iname!=NULL)
					status+=NormalizeVector(_ipnameListUpdate,_ipnameNbElementUpdate,_max_ipname_lenUpdate,iname);
				if(ipadd!=NULL)
					status+=NormalizeVector(_ip_addListUpdate,_ip_addNbElementUpdate,_max_ip_add_lenUpdate,ipadd);
				numrows_inserted=(int*)malloc(sizeof(int)*_ip_addNbElementUpdate);
				if(numrows_inserted!=NULL)
				{
					for(i=0;i<_ip_addNbElementUpdate;i++)
						numrows_inserted[i]=0;
				}
				if( submask==NULL || iname==NULL||ipadd==NULL||numrows_inserted==NULL)
				{
					status=-10;
					rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");

					status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
					rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");
					GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
					status=freeIPEthernetUpdate();

					if(ipadd!=NULL)
						free(ipadd);
					if(iname!=NULL)
						free(iname);
					if(submask!=NULL)
						free(submask);
					if(numrows_inserted!=NULL)
						free(numrows_inserted);
					return -1;
				}	
			}

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp[0], ociError,(text*) ":ipadd", -1,(dvoid*)ipadd, _max_ip_add_lenUpdate,  SQLT_STR, (dvoid *)&_ipadd_nullvalueUpdate[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp[1], ociError,(text*) ":submask", -1,(dvoid*)submask,_max_subnet_mask_lenUpdate,  SQLT_STR, (dvoid *) &_subnetmask_nullvalueUpdate[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);


			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp[2], ociError,(text*) ":ipname", -1,(dvoid*)iname, _max_ipname_lenUpdate,  SQLT_STR, (dvoid *) &_ipname_nullvalueUpdate[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp3, ociError,(text*) ":numrows", -1,(dvoid*)&numrows_inserted[0], sizeof(numrows),SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);


			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp[0], ociError,_max_ip_add_lenUpdate, sizeof(int),0, 0);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp[1], ociError,_max_subnet_mask_lenUpdate, sizeof(int),0, 0);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp[2], ociError,_max_ipname_lenUpdate, sizeof(int),0, 0);

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
				status= OCIStmtExecute(ociHdbc, hstmt, ociError, _ip_addNbElementUpdate, 0, 0, 0, OCI_DEFAULT );
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
			}
			else
			{
				numrows=-1;
				for(i=0;i< _ip_addNbElementUpdate;i++)
				{
					if(numrows_inserted[i]==0)
					{
						numrows=0;
						i=  _ip_addNbElementUpdate+5;
					}
					else
						numrows=numrows_inserted[i];
				}
				if(numrows==0)
				{
					status = OCITransCommit(ociHdbc, ociError, 0);
					status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
					status=freeIPEthernetUpdate();

					if(ipadd!=NULL)
						free(ipadd);
					if(iname!=NULL)
						free(iname);
					if(submask!=NULL)
						free(submask);
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


			status=freeIPEthernetUpdate();
			if(rescode!=0)
				if(ociError!=0)
					OCIReportError(ociError,appliName, ErrMess,1); 
				else
					GetErrorMess(appliName,  "NOT CONNECTED TO ANY DB",ErrMess,1); 
			else
				OCIReportError(ociError,appliName, ErrMess,0); 



			if(ipadd!=NULL)
				free(ipadd);
			if(iname!=NULL)
				free(iname);
			if(submask!=NULL)
				free(submask);
			if(numrows_inserted!=NULL)
				free(numrows_inserted);

			status+=OCIHandleFree (hstmt,OCI_HTYPE_STMT);
			if(rescode==0 && force_insert==1)
				FIRST_TIME_MACUPDATE=1;
		}
		else
		{
			if(res_query!=0)
			{
				status=freeIPEthernetUpdate();
				GetErrorMess(appliName, "Cache Problem",ErrMess,1);
			}
			if(FIRST_TIME_MACUPDATE!=1)
			{
				status=freeIPEthernetUpdate();
				res_query=-1;
				GetErrorMess(appliName, "CACHE HAS NOT BEEN INITIALIZED",ErrMess,1);
			}
			if(ociEnv==0)
			{
				status=freeIPEthernetUpdate();
				res_query=-1;
				GetErrorMess(appliName, "NOT CONNECTED TO ANY DB",ErrMess,1);
			}
			if(ociEnv!=0 && FIRST_TIME_MACUPDATE==1 && res_query==0)
			{
				status=0;
				GetErrorMess(appliName, "NO_ERROR",ErrMess,0);
			}
			rescode=res_query;
		}

		return (rescode+status);
	}



	/**
	* Update MAC entries,  returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* @param oldip_add :  ip address entry you want to modify. 
	* @param ip_add : new value of ip address entry you want to modify. 
	* @param first_time1 : 1 if it's your first MacIP insert (save it into the database), 0 otherwise
	* @param last_rows1 : 1 if it's your last MacIP insert (save it into the database), 0 otherwise
	* @param ErrMess : error message in case of failure
	* @return 0 if it is successful
	*/ 



	EXTERN_CONFDB
		int UpdateMultipleIPAddresses(char* ip_add,char* oldip_add,int first_time1,int last_rows1,char* ErrMess)
	{
		char appliName[100]="UpdateMultipleIPAddresses";
		int* numrows_inserted=NULL;
		int numrows=0;
		OCIBind  *bndp3 = (OCIBind *) 0;
		OCIBind  *bndp4 = (OCIBind *) 0;
		char sqlstmt[2000];
		OCIStmt *hstmt;
		OCIBind  *bndp[3]; 
		int res_query=0;
		int rescode=0;
		sword status=0;
		char* oldipadd=NULL;
		char* ipadd=NULL;
		int i=0;
		int* numrows_inserted1=NULL;

		int force_insert=0;
		int first_time=first_time1;
		int last_rows=last_rows1;

		int oldipadd1=null_charptr(oldip_add);
		int ipadd1=null_charptr(ip_add);

		if(FIRST_TIME_MACUPDATE3==1 && _ip1_addListUpdate==NULL)
			first_time=1;

		res_query+=AppendString(ip_add,_ip1_addListUpdate,_ip1_addList_lenUpdate,_ip1_addNbElementUpdate,_max_ip1_add_lenUpdate,first_time);
		res_query+=AppendString(oldip_add,_oldip_addListUpdate,_oldip_addList_lenUpdate,_oldip_addNbElementUpdate,_max_oldip_add_lenUpdate,first_time);


		res_query+=AppendInt(ipadd1,_ip1_nullvalueUpdate,_ip1NbElUpdate,first_time);
		res_query+=AppendInt(oldipadd1,_oldip_nullvalueUpdate,_oldipNbElUpdate,first_time);


		status=res_query;
		if(first_time==1)
			FIRST_TIME_MACUPDATE3=1;

		if(last_rows!=1 && _ip_addNbElementUpdate==MAXROWS)
		{
			force_insert=1;
			last_rows=1;
		}

		if(last_rows==1 && res_query==0 && ociEnv!=0 && FIRST_TIME_MACUPDATE3==1)
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


				if(ipadd!=NULL)
					free(ipadd);
				if(oldipadd!=NULL)
					free(oldipadd);
				status=freeIPEthernetUpdate3();
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
				sprintf(sqlstmt,"BEGIN :numrows1:=%s(:old_ip,:ipadd,'%s');  :numrows:=%s; end; ",_updateIPaddress,login,SQLROWCOUNT);
			if (OCIStmtPrepare(hstmt, ociError, (text*)sqlstmt, (ub4)strlen((char *)sqlstmt), (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT))
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
			}
			else
			{
				ipadd=(char*)malloc(_ip1_addNbElementUpdate*_max_ip1_add_lenUpdate*sizeof(char));
				if(ipadd!=NULL)
					status+=NormalizeVector(_ip1_addListUpdate,_ip1_addNbElementUpdate,_max_ip1_add_lenUpdate,ipadd);


				oldipadd=(char*)malloc(_oldip_addNbElementUpdate*_max_oldip_add_lenUpdate*sizeof(char));
				if(oldipadd!=NULL)
					status+=NormalizeVector(_oldip_addListUpdate,_oldip_addNbElementUpdate,_max_oldip_add_lenUpdate,oldipadd);

				numrows_inserted=(int*)malloc(sizeof(int)*_oldip_addNbElementUpdate);
				numrows_inserted1=(int*)malloc(sizeof(int)*_oldip_addNbElementUpdate);
				if(numrows_inserted!=NULL && numrows_inserted1!=NULL)
				{
					for(i=0;i<_oldip_addNbElementUpdate;i++)
					{
						numrows_inserted[i]=0;
						numrows_inserted1[i]=0;
					}
				}
				if(oldipadd==NULL ||ipadd==NULL||numrows_inserted==NULL)
				{
					status=-10;
					rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");

					status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
					rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");
					GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
					status=freeIPEthernetUpdate3();
					if(ipadd!=NULL)
						free(ipadd);
					if(oldipadd!=NULL)
						free(oldipadd);
					if(numrows_inserted1!=NULL)
						free(numrows_inserted1);
					if(numrows_inserted!=NULL)
						free(numrows_inserted);
					return -1;
				}
				else
					status =OCIBindByName(hstmt, &bndp[0], ociError,(text*) ":ipadd", -1,(dvoid*)ipadd,_max_ip1_add_lenUpdate,  SQLT_STR, (dvoid *)&_ip1_nullvalueUpdate[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);

			}

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp[1], ociError,(text*) ":old_ip", -1,(dvoid*)oldipadd,_max_oldip_add_lenUpdate,  SQLT_STR, (dvoid *)&_oldip_nullvalueUpdate[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);


			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp3, ociError,(text*) ":numrows", -1,(dvoid*)&numrows_inserted[0], sizeof(numrows),SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp4, ociError,(text*) ":numrows1", -1,(dvoid*)&numrows_inserted1[0], sizeof(numrows),SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);



			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp[0], ociError,_max_ip_add_lenUpdate, sizeof(int),0, 0);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp[1], ociError,_max_oldip_add_lenUpdate, sizeof(int),0, 0);


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
				status= OCIStmtExecute(ociHdbc, hstmt, ociError, _oldip_addNbElementUpdate, 0, 0, 0, OCI_DEFAULT );
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
			}
			else
			{
				numrows=-1;
				for(i=0;i< _oldip_addNbElementUpdate;i++)
				{
					if(numrows_inserted1[i]!=1)
					{
						numrows=0;
						i=  _oldip_addNbElementUpdate+5;
					}
					else
						numrows=numrows_inserted1[i];
				}
				if(numrows==0)
				{
					status = OCITransCommit(ociHdbc, ociError, 0);
					status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
					status=freeIPEthernetUpdate3();

					if(ipadd!=NULL)
						free(ipadd);
					if(oldipadd!=NULL)
						free(oldipadd);
					free(numrows_inserted);
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


			status=freeIPEthernetUpdate3();
			if(rescode!=0)
				if(ociError!=0)
					OCIReportError(ociError,appliName, ErrMess,1); 
				else
					GetErrorMess(appliName,  "NOT CONNECTED TO ANY DB",ErrMess,1); 
			else
				OCIReportError(ociError,appliName, ErrMess,0); 



			if(ipadd!=NULL)
				free(ipadd);
			if(oldipadd!=NULL)
				free(oldipadd);
			if(numrows_inserted1!=NULL)
				free(numrows_inserted1);
			if(numrows_inserted!=NULL)
				free(numrows_inserted);

			status+=OCIHandleFree (hstmt,OCI_HTYPE_STMT);
			if(rescode==0 && force_insert==1)
				FIRST_TIME_MACUPDATE3=1;
		}
		else
		{
			if(res_query!=0)
			{
				status=freeIPEthernetUpdate3();
				GetErrorMess(appliName, "Cache Problem",ErrMess,1);
			}
			if(FIRST_TIME_MACUPDATE3!=1)
			{
				status=freeIPEthernetUpdate3();
				res_query=-1;
				GetErrorMess(appliName, "CACHE HAS NOT BEEN INITIALIZED",ErrMess,1);
			}
			if(ociEnv==0)
			{
				status=freeIPEthernetUpdate3();
				res_query=-1;
				GetErrorMess(appliName, "NOT CONNECTED TO ANY DB",ErrMess,1);
			}
			if(ociEnv!=0 && FIRST_TIME_MACUPDATE3==1 && res_query==0)
			{
				status=0;
				GetErrorMess(appliName, "NO_ERROR",ErrMess,0);
			}
			rescode=res_query;

		}

		return (rescode+status);
	}
	/**
	* Update IP alias entries,  returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* @param old_ipalias :  ip address entry you want to modify. 
	* @param new_ipalias : new value of ip address entry you want to modify. 
	* @param ErrMess : error message in case of failure
	* @return 0 if it is successful
	*/ 
	/*************************************************************************/
	EXTERN_CONFDB int UpdateIPalias(char* old_ipalias,char* new_ipalias,char* ErrMess)
	{

		//extern "C" __declspec(dllexport)
		char appliName[100]="UpdateIPalias";
		char sqlstmt[2000];
		OCIStmt *hstmt;
		OCIBind  *bndp[2]; 
		int rescode=0;
		int free_mem=0;
		char res_query1[100]="blabla";
		int res_query=0;
		int deviceid=0;
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
			sprintf(sqlstmt,"BEGIN update %s set ipalias=:new_ipalias, last_update=sysdate where ipalias=:old_ipalias; :numrows:=%s; END;",IPALIAS_TABLE,SQLROWCOUNT);
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

			status =OCIBindByName(hstmt, &bndp[0], ociError,(text*) ":old_ipalias", -1,(dvoid*)old_ipalias,strlen(old_ipalias)+1,  SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);

		}
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else
			status =OCIBindByName(hstmt, &bndp[1], ociError,(text*) ":new_ipalias", -1,(dvoid*)new_ipalias,strlen(new_ipalias)+1,  SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);

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
			if (numrows_inserted==0)
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

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

