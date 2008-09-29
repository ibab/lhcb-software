/********************************************************************************/
//	Author: L.Abadie
// version:v3.5
// changes: handling error such that can be extended to C++: (create a class SQLException) (strlen ck + update stmt)
// file which performs bulk collect insert for the devicetype table
// N.B : Memory management (allocation+release) should be handled by the user application thus, for all the fct provided by this lib
/********************************************************************************/


#include "list_structures.h"
#include "system_info.h"
#include "CDB.h"
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
* Insert a device type , returning an integer value.
* The user should manage the memory : there is no memory allocation.
* @param systemnameList : list of the subsystems (UPPER CASE) by which this device type is used: a VELO_TELL1 board type is used by the VELO, TFC and DAQ. so you type VELO,TFC,DAQ. you separate the different subsystems with a coma.
* @param devicetype : name of the devicetype
* @param nbrofinput : nb of input ports
* @param nbrofoutput : nb of output ports
* @param description :  some description about the component max size 500 characters.
* @param rgbcolor :  color of the devicetype for display in RGB system each field separated with a comma ex.:251,21,3.
* @param first_time1 : 1 if it's your MultipleDeviceType first insert (save it into the database), 0 otherwise
* @param last_rows1 : 1 if it's your MultipleDeviceType last insert (save it into the database), 0 otherwise  Maximum in one go: 10000 rows.(force insertion)
* @param ErrMess : error message in case of failure
* @return 0 if it is successful
*/

EXTERN_CONFDB
	int InsertMultipleDeviceTypes(char* systemnameList,char* devicetype,int nbrofinput,int nbrofoutput, char* description,char* rgbcolor,int first_time, int last_rows,char* ErrMess)
	{
		char appliName[100]="InsertMultipleDeviceTypes";
		char sqlstmt[1000];
		OCIStmt *hstmt;
		OCIBind  *bndp[7]; 
		int rescode=0;
		int res_query=0;
		sword status=0;
		int i=0;

		char* dtype=NULL;
		char* rgbcol=NULL;
		char* descrip=NULL;
		int* NbrofinputList=NULL;
		int* NbrofoutputList=NULL;
		int* sysIDList=NULL;	

		int* type_nullList=NULL;
		int* rgbcol_nullList=NULL;
		int* descrip_nullList=NULL;

		static int FIRST_TIME=0;
		int force_insert=0;
		char seq_name[100]="lhcb_devtype_seq.nextval";
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
		int sysID=GetSubsystemID(systemnameList);
		if(sysID==-1)
		{
			GetErrorMess(appliName, "systemnameList is null or check the spelling",ErrMess,1);
			return -1;
		}

		typeList[NbElement-1]=new DevType(systemnameList, devicetype, nbrofinput, nbrofoutput, description, rgbcolor);
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
			sysIDList=(int*)malloc(sizeof(int)*NbElement);

			type_nullList=(int*)malloc(NbElement*sizeof(int));
			rgbcol_nullList=(int*)malloc(NbElement*sizeof(int));
			descrip_nullList=(int*)malloc(NbElement*sizeof(int));
			
			if(type_nullList!=NULL && rgbcol_nullList!=NULL && descrip_nullList!=NULL && dtype!=NULL
				&& rgbcol!=NULL && descrip!=NULL && NbrofinputList!=NULL && NbrofoutputList!=NULL && sysIDList!=NULL)
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
				sysIDList[i]=typeList[i]->sysID;
			}
	
			numrows_inserted=(int*)malloc(sizeof(int)*NbElement);

			if( (dtype==NULL) || (rgbcol==NULL) || descrip==NULL || numrows_inserted==NULL 
				||type_nullList==NULL || descrip_nullList==NULL || rgbcol_nullList==NULL ||NbrofinputList==NULL || NbrofoutputList==NULL || sysIDList==NULL)
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
				sprintf(sqlstmt,"BEGIN insert into %s(devicetypeID,devicetype,nbrofinput,nbrofoutput,description,rgbcolor,author,user_update,terminal_name,system_name) values(%s,:dtype,:din,:dout,:ddescr,:rgbcol,'%s','%s','%s',:sysID); :numrows:=%s; END;",DEVICETYPE_TABLE,seq_name,login,login,host,SQLROWCOUNT);
				StmtPrepare(hstmt,ociError, sqlstmt, &status);

				BindByName(hstmt,&bndp[0],ociError,":dtype",dtype,max_typelen,SQLT_STR,&type_nullList[0],&status);
				BindByName(hstmt,&bndp[1],ociError,":din",&NbrofinputList[0],sizeof(&typeList[0]->nbin),SQLT_INT,0,&status);
				BindByName(hstmt,&bndp[2],ociError,":dout",&NbrofoutputList[0],sizeof(&typeList[0]->nbout),SQLT_INT,0,&status);
				BindByName(hstmt,&bndp[3],ociError,":ddescr",descrip,max_descriplen,SQLT_STR,&descrip_nullList[0],&status);
				BindByName(hstmt,&bndp[4],ociError,":rgbcol",rgbcol,max_rgbcollen,SQLT_STR,&rgbcol_nullList[0],&status);
				BindByName(hstmt,&bndp[5],ociError,":sysID",&sysIDList[0],sizeof(&typeList[0]->sysID),SQLT_INT,0,&status);
				BindByName(hstmt,&bndp[6],ociError,":numrows",&numrows_inserted[0],sizeof(int),SQLT_INT,0,&status);

				BindArrayOfStruct(bndp[0],ociError,max_typelen,sizeof(int),&status);
				BindArrayOfStruct(bndp[1],ociError,sizeof(int),0,&status);
				BindArrayOfStruct(bndp[2],ociError,sizeof(int),0,&status);
				BindArrayOfStruct(bndp[3],ociError,max_descriplen,sizeof(int),&status);
				BindArrayOfStruct(bndp[4],ociError,max_rgbcollen,sizeof(int),&status);
				BindArrayOfStruct(bndp[5],ociError,sizeof(int),0,&status);
				BindArrayOfStruct(bndp[6],ociError,sizeof(int),0,&status);
				StmtExecute(ociHdbc, hstmt, ociError,NbElement, &status);

			}catch(Error err){
				sprintf(appliName,"InsertMultipleDeviceTypes");	///////
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
		if(sysIDList!=NULL)
			free(sysIDList);
	
		return (rescode+status);
	}



#if defined(__cplusplus) || defined(c_plusplus)
}
#endif