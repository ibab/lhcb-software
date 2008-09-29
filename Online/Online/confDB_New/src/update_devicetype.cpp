/********************************************************************************/
//	Author: L.Abadie
// version:v3.5
// changes: handling error such that can be extended to C++: (create a class SQLException) (strlen ck + update stmt)
// file which performs bulk collect update for the device type table
// N.B : Memory management (allocation+release) should be handled by the user application thus, for all the fct provided by this lib
/********************************************************************************/
#include "list_structures.h"
#include "CDB.h"
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
******************************************************************/
EXTERN_CONFDB
	int UpdateMultipleDeviceTypeAttributes(char* devicetype,char* description,int nbrofinput,int nbrofoutput,char* rgbcolor,int first_time,int last_rows,char* ErrMess)
	{
		char appliName[100]="UpdateMultipleDeviceTypeAttributes";
		char sqlstmt[1000];
		OCIStmt *hstmt;
		OCIBind  *bndp[6]; 
		int rescode=0;
		int res_query=0;
		sword status=0;
		int i=0;

		char* dtype=NULL;
		char* rgbcol=NULL;
		char* descrip=NULL;
		int* NbrofinputList=NULL;
		int* NbrofoutputList=NULL;
	
		int* type_nullList=NULL;
		int* rgbcol_nullList=NULL;
		int* descrip_nullList=NULL;

		static int FIRST_TIME=0;
		int force_insert=0;
		int* numrows_inserted=NULL;
		int numrows=0;
		
		static int NbElement;
		DevType** typeList;
		int max_typelen=0;
		int max_rgbcollen=0;
		int max_descriplen=0;

		if (first_time==1)
		{
			FIRST_TIME=1;
			NbElement=1;
			typeList=(DevType**)malloc( NbElement*sizeof(DevType));			
		}
		else 
		{
			if (FIRST_TIME==0&&force_insert==0)
			{
				GetErrorMess(appliName, "CACHE HAS NOT BEEN INITIALIZED",ErrMess,1);
				return -1;
			}
			NbElement++;
			typeList=(DevType**)realloc(typeList, NbElement*sizeof(DevType));
		}
		typeList[NbElement-1]=new DevType(NULL, devicetype, nbrofinput, nbrofoutput, description, rgbcolor);
		
		if(NbElement==MAXROWS && last_rows!=1)
		{
			force_insert=1;
			last_rows=1;
		}
	
		if(last_rows==1 && res_query==0 && ociEnv!=0 && FIRST_TIME==1)
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

				FIRST_TIME=0;
				if(dtype!=NULL)
					free(dtype);
				if(descrip!=NULL)
					free(descrip);
				if(rgbcol!=NULL)
					free(rgbcol);
				if(numrows_inserted!=NULL)
					free(numrows_inserted);
				if(type_nullList!=NULL)
					free(type_nullList);
				if(rgbcol_nullList!=NULL)
					free(rgbcol_nullList);
				if(descrip_nullList!=NULL)
					free(descrip_nullList);
				if(NbrofinputList!=NULL)
					free(NbrofinputList);
				if(NbrofoutputList!=NULL)
					free(NbrofoutputList);
				
				for(i=0;i<NbElement;i++)
					delete typeList[i];
				if(typeList!=NULL)
					free(typeList);
				
				free(errmessg1);
				free(errmessg2);
				return -1;
			}
			for(i=0;i<NbElement;i++){
				if (typeList[i]->typelen > max_typelen)
					max_typelen=typeList[i]->typelen;
				if (typeList[i]->rgbcollen > max_rgbcollen)
					max_rgbcollen=typeList[i]->rgbcollen;
				if (typeList[i]->descriplen > max_descriplen)
					max_descriplen=typeList[i]->descriplen;
			}

			dtype=(char*)malloc(NbElement*max_typelen*sizeof(char));
			descrip=(char*)malloc(NbElement*max_descriplen*sizeof(char));
			rgbcol=(char*)malloc(NbElement*max_rgbcollen*sizeof(char));

			NbrofinputList=(int*)malloc(sizeof(int)*NbElement);
			NbrofoutputList=(int*)malloc(sizeof(int)*NbElement);
			
			type_nullList=(int*)malloc(NbElement*sizeof(int));
			rgbcol_nullList=(int*)malloc(NbElement*sizeof(int));
			descrip_nullList=(int*)malloc(NbElement*sizeof(int));
			
			if(type_nullList!=NULL && rgbcol_nullList!=NULL && descrip_nullList!=NULL && dtype!=NULL
				&& rgbcol!=NULL && descrip!=NULL && NbrofinputList!=NULL && NbrofoutputList!=NULL)
			for(i=0;i<NbElement;i++)
			{
				memcpy(dtype+i*max_typelen,typeList[i]->type,typeList[i]->typelen);
				type_nullList[i]=typeList[i]->type_null;
				memcpy(rgbcol+i*max_rgbcollen,typeList[i]->rgbcol,typeList[i]->rgbcollen);
				rgbcol_nullList[i]=typeList[i]->rgbcol_null;
				memcpy(descrip+i*max_descriplen,typeList[i]->descrip,typeList[i]->descriplen);
				descrip_nullList[i]=typeList[i]->descrip_null;

				NbrofinputList[i]=typeList[i]->nbin;
				NbrofoutputList[i]=typeList[i]->nbout;				
			}
	
			numrows_inserted=(int*)malloc(sizeof(int)*NbElement);
			if( (dtype==NULL) || (rgbcol==NULL) || descrip==NULL || numrows_inserted==NULL 
				||type_nullList==NULL || descrip_nullList==NULL || rgbcol_nullList==NULL ||NbrofinputList==NULL || NbrofoutputList==NULL)
			{
				rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");
				GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
				
				FIRST_TIME=0;
				if(dtype!=NULL)
					free(dtype);
				if(descrip!=NULL)
					free(descrip);
				if(rgbcol!=NULL)
					free(rgbcol);
				if(numrows_inserted!=NULL)
					free(numrows_inserted);
				if(type_nullList!=NULL)
					free(type_nullList);
				if(rgbcol_nullList!=NULL)
					free(rgbcol_nullList);
				if(descrip_nullList!=NULL)
					free(descrip_nullList);
				if(NbrofinputList!=NULL)
					free(NbrofinputList);
				if(NbrofoutputList!=NULL)
					free(NbrofoutputList);
				
				for(i=0;i<NbElement;i++)
					delete typeList[i];
				if(typeList!=NULL)
                    free(typeList);
				
				NbElement=0;				
				return -1;
			}
			else
			{
				for(i=0;i<NbElement;i++)
					numrows_inserted[i]=0;
			}

			try{
                HandleAlloc(ociEnv,(dvoid**)&hstmt,OCI_HTYPE_STMT,&status);
				sprintf(sqlstmt,"BEGIN update %s set description=decode(nvl(:descrip,'none'),'none',description,:descrip),nbrofinput=decode(:nbin,-1,nbrofinput,:nbin),nbrofoutput=decode(:nbout,-1,nbrofoutput,:nbout),rgbcolor=decode(nvl(:rgbcol,'none'),'none',rgbcolor,:rgbcol),last_update=sysdate,user_update='%s' where devicetype=:dtype_old; :numrows:=%s; END;",DEVICETYPE_TABLE,login,SQLROWCOUNT);
				StmtPrepare(hstmt,ociError, sqlstmt, &status);

				BindByName(hstmt,&bndp[0],ociError,":dtype_old",dtype,max_typelen,SQLT_STR,&type_nullList[0],&status);
				BindByName(hstmt,&bndp[1],ociError,":nbin",&NbrofinputList[0],sizeof(&typeList[0]->nbin),SQLT_INT,0,&status);
				BindByName(hstmt,&bndp[2],ociError,":nbout",&NbrofoutputList[0],sizeof(&typeList[0]->nbout),SQLT_INT,0,&status);
				BindByName(hstmt,&bndp[3],ociError,":descrip",descrip,max_descriplen,SQLT_STR,&descrip_nullList[0],&status);
				BindByName(hstmt,&bndp[4],ociError,":rgbcol",rgbcol,max_rgbcollen,SQLT_STR,&rgbcol_nullList[0],&status);
				BindByName(hstmt,&bndp[5],ociError,":numrows",&numrows_inserted[0],sizeof(int),SQLT_INT,0,&status);

				BindArrayOfStruct(bndp[0],ociError,max_typelen,sizeof(int),&status);
				BindArrayOfStruct(bndp[1],ociError,sizeof(int),0,&status);
				BindArrayOfStruct(bndp[2],ociError,sizeof(int),0,&status);
				BindArrayOfStruct(bndp[3],ociError,max_descriplen,sizeof(int),&status);
				BindArrayOfStruct(bndp[4],ociError,max_rgbcollen,sizeof(int),&status);
				BindArrayOfStruct(bndp[5],ociError,sizeof(int),0,&status);

				StmtExecute(ociHdbc, hstmt, ociError,NbElement, &status);

			}catch(Error err){
				sprintf(appliName,"UpdateMultipleDeviceTypeAttributes");	///////
				rescode=ShowErrors (status, err.ociError, err.log);
				
				if(ociError!=0)
					OCIReportError(ociError,appliName,ErrMess,1); 
				else
					GetErrorMess(appliName,err.msg,ErrMess,1);
			}

			numrows=-1;
			for(i=0;i< NbElement;i++)
			{
				if(numrows_inserted[i]==0)
				{
					numrows=0;
					i= NbElement+5;
				}
				else
					numrows=numrows_inserted[i];
			}

			if(numrows==0)
			{
				status = OCITransCommit(ociHdbc, ociError, 0);
				status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
				FIRST_TIME=0;
				
				if(dtype!=NULL)
					free(dtype);
				if(descrip!=NULL)
					free(descrip);
				if(rgbcol!=NULL)
					free(rgbcol);
				if(numrows_inserted!=NULL)
					free(numrows_inserted);
				if(type_nullList!=NULL)
					free(type_nullList);
				if(rgbcol_nullList!=NULL)
					free(rgbcol_nullList);
				if(descrip_nullList!=NULL)
					free(descrip_nullList);
				if(NbrofinputList!=NULL)
					free(NbrofinputList);
				if(NbrofoutputList!=NULL)
					free(NbrofoutputList);
				
				for(i=0;i<NbElement;i++)
					delete typeList[i];
				if(force_insert==1)
				{
                    FIRST_TIME=1;
					force_insert=0;
					typeList=(DevType**)realloc(typeList, 0*sizeof(DevType));
				}
				else if(typeList!=NULL)
                    free(typeList);

				NbElement=0;

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

			FIRST_TIME=0;
			status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);

			if(rescode!=0)
				if(ociError!=0)
					OCIReportError(ociError,appliName, ErrMess,1); 
				else
					GetErrorMess(appliName,  "NOT CONNECTED TO ANY DB",ErrMess,1); 
			else
				OCIReportError(ociError,appliName, ErrMess,0); 
			
			for(i=0;i<NbElement;i++)
				delete typeList[i];

			if(force_insert==1)
			{
				FIRST_TIME=1;
				force_insert=0;
				typeList=(DevType**)realloc(typeList, 0*sizeof(DevType));
			}
			else if(typeList!=NULL)
                free(typeList);
			NbElement=0;
		}
		else
		{
			if(res_query!=0)
			{
				FIRST_TIME=0;
				GetErrorMess(appliName, "Cache Problem",ErrMess,1);
				for(i=0;i<NbElement;i++)
					delete typeList[i];
				if(typeList!=NULL)
					free(typeList);
			}
			if(ociEnv==0)
			{
				FIRST_TIME=0;
				res_query=-1;
				GetErrorMess(appliName, "NOT CONNECTED TO ANY DB",ErrMess,1);
				for(i=0;i<NbElement;i++)
					delete typeList[i];
				if(typeList!=NULL)
                    free(typeList);
			}
			status+=res_query;
			if(ociEnv!=0 && FIRST_TIME==1 && res_query==0)
			{
				status=0;
				GetErrorMess(appliName, "NO_ERROR",ErrMess,0);
			}
		}
		if(dtype!=NULL)
			free(dtype);
		if(descrip!=NULL)
			free(descrip);
		if(rgbcol!=NULL)
			free(rgbcol);
		if(numrows_inserted!=NULL)
			free(numrows_inserted);
		if(type_nullList!=NULL)
			free(type_nullList);
		if(rgbcol_nullList!=NULL)
			free(rgbcol_nullList);
		if(descrip_nullList!=NULL)
			free(descrip_nullList);
		if(NbrofinputList!=NULL)
			free(NbrofinputList);
		if(NbrofoutputList!=NULL)
			free(NbrofoutputList);
		
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
******************************************************************/
EXTERN_CONFDB
	int UpdateMultipleDeviceTypes(char* devicetype_old,char* devicetype,int first_time,int last_rows,char* ErrMess)
	{
		char appliName[100]="UpdateMultipleDeviceTypes";
		char sqlstmt[1000];
		OCIStmt *hstmt;
		OCIBind  *bndp[3]; 
		int rescode=0;
		int res_query=0;
		sword status=0;
		int i=0;

		char* dtype=NULL;
		char* dtypeold=NULL;		
		int* type_nullList=NULL;
		int* typeold_nullList=NULL;
		
		static int FIRST_TIME=0;
		int force_insert=0;
		int* numrows_inserted=NULL;
		int numrows=0;

		static int NbElement;
		UpdtDevType** typeList;
		int max_typelen=0;
		int max_typeoldlen=0;

		if (first_time==1)
		{
			FIRST_TIME=1;
			NbElement=1;
			typeList=(UpdtDevType**)malloc(NbElement*sizeof(UpdtDevType));			
		}
		else 
		{
			if (FIRST_TIME==0&&force_insert==0)
			{
				GetErrorMess(appliName, "CACHE HAS NOT BEEN INITIALIZED",ErrMess,1);
				return -1;
			}
			NbElement++;
			typeList=(UpdtDevType**)realloc(typeList, NbElement*sizeof(UpdtDevType));
		}
		typeList[NbElement-1]=new UpdtDevType(devicetype_old, devicetype);
		
		if(NbElement==MAXROWS && last_rows!=1)
		{
			force_insert=1;
			last_rows=1;
		}
		
		if(last_rows==1 && res_query==0 && ociEnv!=0 && FIRST_TIME==1)
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

				FIRST_TIME=0;
				if(dtype!=NULL)
					free(dtype);
				if(dtypeold!=NULL)
					free(dtypeold);
				if(numrows_inserted!=NULL)
					free(numrows_inserted);
				if(type_nullList!=NULL)
					free(type_nullList);
				if(typeold_nullList!=NULL)
					free(typeold_nullList);
				
				for(i=0;i<NbElement;i++)
					delete typeList[i];
				if(typeList!=NULL)
					free(typeList);
				
				free(errmessg1);
				free(errmessg2);
				return -1;
			}
			for(i=0;i<NbElement;i++){
				if (typeList[i]->typelen > max_typelen)
					max_typelen=typeList[i]->typelen;
				if (typeList[i]->typeoldlen > max_typeoldlen)
					max_typeoldlen=typeList[i]->typeoldlen;
			}

			dtype=(char*)malloc(NbElement*max_typelen*sizeof(char));
			dtypeold=(char*)malloc(NbElement*max_typeoldlen*sizeof(char));
			type_nullList=(int*)malloc(NbElement*sizeof(int));
			typeold_nullList=(int*)malloc(NbElement*sizeof(int));

			if(type_nullList!=NULL && typeold_nullList!=NULL && dtype!=NULL && dtypeold!=NULL)
			for(i=0;i<NbElement;i++)
			{
				memcpy(dtype+i*max_typelen,typeList[i]->type,typeList[i]->typelen);
				type_nullList[i]=typeList[i]->type_null;
				memcpy(dtypeold+i*max_typeoldlen,typeList[i]->typeold,typeList[i]->typeoldlen);
				typeold_nullList[i]=typeList[i]->typeold_null;
			}
			numrows_inserted=(int*)malloc(sizeof(int)*NbElement);

			if(dtype==NULL || dtypeold==NULL || numrows_inserted==NULL ||type_nullList==NULL || typeold_nullList==NULL)
			{
				rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");
				GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
				
				FIRST_TIME=0;
				if(dtype!=NULL)
					free(dtype);
				if(dtypeold!=NULL)
					free(dtypeold);
				if(numrows_inserted!=NULL)
					free(numrows_inserted);
				if(type_nullList!=NULL)
					free(type_nullList);
				if(typeold_nullList!=NULL)
					free(typeold_nullList);
				
				for(i=0;i<NbElement;i++)
					delete typeList[i];
				if(typeList!=NULL)
                    free(typeList);
				
				NbElement=0;				
				return -1;
			}
			else
			{
				for(i=0;i<NbElement;i++)
					numrows_inserted[i]=0;
			}			
			
			try{
                HandleAlloc(ociEnv,(dvoid**)&hstmt,OCI_HTYPE_STMT,&status);
				sprintf(sqlstmt,"BEGIN update %s set devicetype=:dtype,last_update=sysdate,user_update='%s' where devicetype=:dtype_old; :numrows:=%s; END;",DEVICETYPE_TABLE,login,SQLROWCOUNT);
				StmtPrepare(hstmt,ociError, sqlstmt, &status);

				BindByName(hstmt,&bndp[0],ociError,":dtype",dtype,max_typelen,SQLT_STR,&type_nullList[0],&status);
				BindByName(hstmt,&bndp[1],ociError,":dtype_old",dtypeold,max_typeoldlen,SQLT_STR,&typeold_nullList[0],&status);
				BindByName(hstmt,&bndp[2],ociError,":numrows",&numrows_inserted[0],sizeof(int),SQLT_INT,0,&status);

				BindArrayOfStruct(bndp[0],ociError,max_typelen,sizeof(int),&status);
				BindArrayOfStruct(bndp[1],ociError,max_typeoldlen,sizeof(int),&status);
				BindArrayOfStruct(bndp[2],ociError,sizeof(int),0,&status);
				
				StmtExecute(ociHdbc, hstmt, ociError,NbElement, &status);
			}catch(Error err){
				sprintf(appliName,"UpdateMultipleDeviceTypes");	///////
				rescode=ShowErrors (status, err.ociError, err.log);
				
				if(ociError!=0)
					OCIReportError(ociError,appliName,ErrMess,1); 
				else
					GetErrorMess(appliName,err.msg,ErrMess,1);
			}

			numrows=-1;
			for(i=0;i< NbElement;i++)
			{
				if(numrows_inserted[i]==0)
				{
					numrows=0;
					i= NbElement+5;
				}
				else
					numrows=numrows_inserted[i];
			}
			if(numrows==0)
			{
				status = OCITransCommit(ociHdbc, ociError, 0);
				status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
				FIRST_TIME=0;
				
				if(dtype!=NULL)
					free(dtype);
				if(dtypeold!=NULL)
					free(dtypeold);
				if(numrows_inserted!=NULL)
					free(numrows_inserted);
				if(type_nullList!=NULL)
					free(type_nullList);
				if(typeold_nullList!=NULL)
					free(typeold_nullList);
				
				for(i=0;i<NbElement;i++)
					delete typeList[i];
				if(force_insert==1)
				{
                    FIRST_TIME=1;
					force_insert=0;
					typeList=(UpdtDevType**)realloc(typeList, 0*sizeof(UpdtDevType));
				}
				else if(typeList!=NULL)
                    free(typeList);

				NbElement=0;

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

			FIRST_TIME=0;
			status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);

			if(rescode!=0)
				if(ociError!=0)
					OCIReportError(ociError,appliName, ErrMess,1); 
				else
					GetErrorMess(appliName,  "NOT CONNECTED TO ANY DB",ErrMess,1); 
			else
				OCIReportError(ociError,appliName, ErrMess,0); 
			
			for(i=0;i<NbElement;i++)
				delete typeList[i];

			if(force_insert==1)
			{
				FIRST_TIME=1;
				force_insert=0;
				typeList=(UpdtDevType**)realloc(typeList, 0*sizeof(UpdtDevType));
			}
			else if(typeList!=NULL)
                free(typeList);
			NbElement=0;
		}
		else
		{
			if(res_query!=0)
			{
				FIRST_TIME=0;
				GetErrorMess(appliName, "Cache Problem",ErrMess,1);
				for(i=0;i<NbElement;i++)
					delete typeList[i];
				if(typeList!=NULL)
					free(typeList);
			}
			if(ociEnv==0)
			{
				FIRST_TIME=0;
				res_query=-1;
				GetErrorMess(appliName, "NOT CONNECTED TO ANY DB",ErrMess,1);
				for(i=0;i<NbElement;i++)
					delete typeList[i];
				if(typeList!=NULL)
                    free(typeList);
			}
			status+=res_query;
			if(ociEnv!=0 && FIRST_TIME==1 && res_query==0)
			{
				status=0;
				GetErrorMess(appliName, "NO_ERROR",ErrMess,0);
			}	
		}
		if(dtype!=NULL)
			free(dtype);
		if(dtypeold!=NULL)
			free(dtypeold);
		if(numrows_inserted!=NULL)
			free(numrows_inserted);
		if(type_nullList!=NULL)
			free(type_nullList);
		if(typeold_nullList!=NULL)
			free(typeold_nullList);
		
		return (rescode+status);
	}

/******************************************************/
/* Update the system_name parameter of a device type  returning an integer value.
* The user should manage the memory : there is no memory allocation.
* @param devicetype : name of the device
* @param new_systemList : new list of system, UPPER CASE, separated with a coma
* @param first_time1 : 1 if it's your first MultipleDeviceNodeUsed update (save it into the database), 0 otherwise
* @param last_rows1 : 1 if it's your MultipleDeviceNodeUsed last update (save it into the database), 0 otherwise
* @param ErrMess : error message in case of failure
* @return 0 if it is successful
*********************************************************************/
EXTERN_CONFDB 
	int UpdateMultipleDeviceTypeSystemList(char* devicetype,char* new_systemList,int first_time,int last_rows,char* ErrMess)
	{
		char appliName[100]="UpdateMultipleDeviceTypeSystemList";
		char sqlstmt[1000];
		OCIStmt *hstmt;
		OCIBind  *bndp[3]; 
		int rescode=0;
		int res_query=0;
		sword status=0;
		int i=0;

		char* dtype=NULL;
		int* sysIDList=NULL;	
		int* type_nullList=NULL;
		
		static int FIRST_TIME=0;
		int force_insert=0;
		int* numrows_inserted=NULL;
		int numrows=0;

		static int NbElement;
		DevType** typeList;
		int max_typelen=0;
		int max_rgbcollen=0;
		int max_descriplen=0;

		if (first_time==1)
		{
			FIRST_TIME=1;
			NbElement=1;
			typeList=(DevType**)malloc( NbElement*sizeof(DevType));			
		}
		else 
		{
			if (FIRST_TIME==0&&force_insert==0)
			{
				GetErrorMess(appliName, "CACHE HAS NOT BEEN INITIALIZED",ErrMess,1);
				return -1;
			}
			NbElement++;
			typeList=(DevType**)realloc(typeList, NbElement*sizeof(DevType));
		}
		int sysID=GetSubsystemID(new_systemList);
		if(sysID==-1)
		{
			GetErrorMess(appliName, "systemnameList is null or check the spelling",ErrMess,1);
			return -1;
		}

		typeList[NbElement-1]=new DevType(NULL, devicetype, NULL, NULL, NULL, NULL);
		typeList[NbElement-1]->sysID=sysID; 

		if(NbElement==MAXROWS && last_rows!=1)
		{
			force_insert=1;
			last_rows=1;
		}
		
		if(last_rows==1 && res_query==0 && ociEnv!=0 && FIRST_TIME==1)
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

				FIRST_TIME=0;
				if(dtype!=NULL)
					free(dtype);
				if(numrows_inserted!=NULL)
					free(numrows_inserted);
				if(type_nullList!=NULL)
					free(type_nullList);
				if(sysIDList!=NULL)
					free(sysIDList);	


				for(i=0;i<NbElement;i++)
					delete typeList[i];
				if(typeList!=NULL)
					free(typeList);
				
				free(errmessg1);
				free(errmessg2);
				return -1;
			}
			for(i=0;i<NbElement;i++){
				if (typeList[i]->typelen > max_typelen)
					max_typelen=typeList[i]->typelen;
			}

			dtype=(char*)malloc(NbElement*max_typelen*sizeof(char));
			sysIDList=(int*)malloc(sizeof(int)*NbElement);
			type_nullList=(int*)malloc(NbElement*sizeof(int));

			if(type_nullList!=NULL && dtype!=NULL && sysIDList!=NULL)
			for(i=0;i<NbElement;i++)
			{
				memcpy(dtype+i*max_typelen,typeList[i]->type,typeList[i]->typelen);
				type_nullList[i]=typeList[i]->type_null;
				sysIDList[i]=typeList[i]->sysID;
			}
	
			numrows_inserted=(int*)malloc(sizeof(int)*NbElement);

			if(dtype==NULL || numrows_inserted==NULL ||type_nullList==NULL || sysIDList==NULL)
			{
				rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");
				GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
				
				FIRST_TIME=0;
				if(dtype!=NULL)
					free(dtype);
				if(numrows_inserted!=NULL)
					free(numrows_inserted);
				if(type_nullList!=NULL)
					free(type_nullList);
				if(sysIDList!=NULL)
					free(sysIDList);
				
				for(i=0;i<NbElement;i++)
					delete typeList[i];
				if(typeList!=NULL)
                    free(typeList);
				
				NbElement=0;				
				return -1;
			}
			else
			{
				for(i=0;i<NbElement;i++)
					numrows_inserted[i]=0;
			}			
			
			try{
                HandleAlloc(ociEnv,(dvoid**)&hstmt,OCI_HTYPE_STMT,&status);
				sprintf(sqlstmt,"BEGIN update %s set system_name=:sysid,last_update=sysdate,user_update='%s' where devicetype=:dname; :numrows:=%s; END;",DEVICETYPE_TABLE,login,SQLROWCOUNT);
				StmtPrepare(hstmt,ociError, sqlstmt, &status);

				BindByName(hstmt,&bndp[0],ociError,":dname",dtype,max_typelen,SQLT_STR,&type_nullList[0],&status);
				BindByName(hstmt,&bndp[1],ociError,":sysid",&sysIDList[0],sizeof(int),SQLT_INT,0,&status);
				BindByName(hstmt,&bndp[2],ociError,":numrows",&numrows_inserted[0],sizeof(int),SQLT_INT,0,&status);
				
				BindArrayOfStruct(bndp[0],ociError,max_typelen,sizeof(int),&status);
				BindArrayOfStruct(bndp[1],ociError,sizeof(int),0,&status);
				BindArrayOfStruct(bndp[2],ociError,sizeof(int),0,&status);
				
				StmtExecute(ociHdbc, hstmt, ociError,NbElement, &status);

			}catch(Error err){
				sprintf(appliName,"UpdateMultipleDeviceTypeSystemList");	///////
				rescode=ShowErrors (status, err.ociError, err.log);
				
				if(ociError!=0)
					OCIReportError(ociError,appliName,ErrMess,1); 
				else
					GetErrorMess(appliName,err.msg,ErrMess,1);
			}

			numrows=-1;
			for(i=0;i< NbElement;i++)
			{
				if(numrows_inserted[i]==0)
				{
					numrows=0;
					i= NbElement+5;
				}
				else
					numrows=numrows_inserted[i];
			}
			if(res_query!=0)
			{
				status = OCITransCommit(ociHdbc, ociError, 0);
				status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
				FIRST_TIME=0;
				
				if(dtype!=NULL)
					free(dtype);
				if(numrows_inserted!=NULL)
					free(numrows_inserted);
				if(type_nullList!=NULL)
					free(type_nullList);
				if(sysIDList!=NULL)
					free(sysIDList);
				
				for(i=0;i<NbElement;i++)
					delete typeList[i];
				if(force_insert==1)
				{
                    FIRST_TIME=1;
					force_insert=0;
					typeList=(DevType**)realloc(typeList, 0*sizeof(DevType));
				}
				else if(typeList!=NULL)
                    free(typeList);

				NbElement=0;

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

			FIRST_TIME=0;
			status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);

			if(rescode!=0)
				if(ociError!=0)
					OCIReportError(ociError,appliName, ErrMess,1); 
				else
					GetErrorMess(appliName,  "NOT CONNECTED TO ANY DB",ErrMess,1); 
			else
				OCIReportError(ociError,appliName, ErrMess,0); 
			
			for(i=0;i<NbElement;i++)
				delete typeList[i];

			if(force_insert==1)
			{
				FIRST_TIME=1;
				force_insert=0;
				typeList=(DevType**)realloc(typeList, 0*sizeof(DevType));
			}
			else if(typeList!=NULL)
                free(typeList);
			NbElement=0;
		}
		else
		{
			if(res_query!=0)
			{
				FIRST_TIME=0;
				GetErrorMess(appliName, "Cache Problem",ErrMess,1);
				for(i=0;i<NbElement;i++)
					delete typeList[i];
				if(typeList!=NULL)
					free(typeList);
			}
			if(ociEnv==0)
			{
				FIRST_TIME=0;
				res_query=-1;
				GetErrorMess(appliName, "NOT CONNECTED TO ANY DB",ErrMess,1);
				for(i=0;i<NbElement;i++)
					delete typeList[i];
				if(typeList!=NULL)
                    free(typeList);
			}
			status+=res_query;
			if(ociEnv!=0 && FIRST_TIME==1 && res_query==0)
			{
				status=0;
				GetErrorMess(appliName, "NO_ERROR",ErrMess,0);
			}	
		}
		if(dtype!=NULL)
			free(dtype);
		if(numrows_inserted!=NULL)
			free(numrows_inserted);
		if(type_nullList!=NULL)
			free(type_nullList);
		if(sysIDList!=NULL)
			free(sysIDList);
			
		return (status+rescode);
	}

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif