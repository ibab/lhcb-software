/********************************************************************************/
//	Author: L.Abadie
// version:v3.5
// changes: handling error such that can be extended to C++: (create a class SQLException) (strlen ck + update stmt)
// file which performs bulk collect insert for the linktype table
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
* Update a  Link Type Name (due to mistyping error), returning an integer value.
* The user should manage the memory : there is no memory allocation.
* @param old_link_name : value  of the  link type name you want to modify
* @param link_name : value  of the new link type
* @param first_time1 : 1 if it's your SimpleLinkType first insert (save it into the database), 0 otherwise
* @param last_rows1 : 1 if it's your SimpleLinkType last insert (save it into the database), 0 otherwise
* @param ErrMess : error message in case of failure
* @return 0 if it is successful
*/ 

EXTERN_CONFDB
	int UpdateMultipleLinkTypeNames(char *old_link_name, char *link_name,int first_time,int last_rows,char* ErrMess)
	{
		char appliName[100]="UpdateMultipleLinkTypenames";
		char sqlstmt[1000];
		OCIStmt *hstmt;
		OCIBind  *bndp[3]; 
		int rescode=0;
		int res_query=0;
		sword status=0;
		int i=0;

		char* ltype=NULL;
		int* ltype_nullList=NULL;
		char* typeold=NULL;
		int* typeold_nullList=NULL;
		
		static int FIRST_TIME=0;
		int force_insert=0;
		int* numrows_inserted=NULL;
		int numrows=0;

		static int NbElement;
		UpdtLinkType** ltypeList;
		int max_ltypelen=0;
		int max_typeoldlen=0;
				
		if (first_time==1)
		{
			FIRST_TIME=1;
			NbElement=1;
			ltypeList=(UpdtLinkType**)malloc( NbElement*sizeof(UpdtLinkType));			
		}
		else 
		{
			if (FIRST_TIME==0&&force_insert==0)
			{
				GetErrorMess(appliName, "CACHE HAS NOT BEEN INITIALIZED",ErrMess,1);
				return -1;
			}
			NbElement++;
			ltypeList=(UpdtLinkType**)realloc(ltypeList, NbElement*sizeof(UpdtLinkType));
		}

		ltypeList[NbElement-1]=new UpdtLinkType(old_link_name,link_name);
		
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
				if(ltype!=NULL)
					free(ltype);
				if(typeold!=NULL)
					free(typeold);
				if(numrows_inserted!=NULL)
					free(numrows_inserted);
				if(ltype_nullList!=NULL)
					free(ltype_nullList);
				if(typeold_nullList!=NULL)
					free(typeold_nullList);
				
				for(i=0;i<NbElement;i++)
					delete ltypeList[i];
				if(ltypeList!=NULL)
					free(ltypeList);
				
				free(errmessg1);
				free(errmessg2);
				return -1;
			}
			for(i=0;i<NbElement;i++){
				if (ltypeList[i]->ltypelen > max_ltypelen)
					max_ltypelen=ltypeList[i]->ltypelen;
				if (ltypeList[i]->typeoldlen > max_typeoldlen)
					max_typeoldlen=ltypeList[i]->typeoldlen;
			}

			ltype=(char*)malloc(NbElement*max_ltypelen*sizeof(char));
			ltype_nullList=(int*)malloc(NbElement*sizeof(int));
			typeold=(char*)malloc(NbElement*max_typeoldlen*sizeof(char));
			typeold_nullList=(int*)malloc(NbElement*sizeof(int));

			if(ltype!=NULL && ltype_nullList!=NULL && typeold!=NULL && typeold_nullList!=NULL)
			for(i=0;i<NbElement;i++)
			{
				memcpy(ltype+i*max_ltypelen,ltypeList[i]->ltype,ltypeList[i]->ltypelen);
				ltype_nullList[i]=ltypeList[i]->ltype_null;
				memcpy(typeold+i*max_typeoldlen,ltypeList[i]->typeold,ltypeList[i]->typeoldlen);
				typeold_nullList[i]=ltypeList[i]->typeold_null;
			}
			numrows_inserted=(int*)malloc(sizeof(int)*NbElement);

			if( ltype==NULL || ltype_nullList==NULL || numrows_inserted==NULL || typeold==NULL || typeold_nullList==NULL)
			{
				rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");
				GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
				
				FIRST_TIME=0;
				if(ltype!=NULL)
					free(ltype);
				if(typeold!=NULL)
					free(typeold);
				if(numrows_inserted!=NULL)
					free(numrows_inserted);
				if(ltype_nullList!=NULL)
					free(ltype_nullList);
				if(typeold_nullList!=NULL)
					free(typeold_nullList);
				
				for(i=0;i<NbElement;i++)
					delete ltypeList[i];
				if(ltypeList!=NULL)
                    free(ltypeList);
				
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
				sprintf(sqlstmt,"BEGIN update %s set link_name=:lkname,last_update=sysdate,user_update='%s' where link_name=:oldlkname; :numrows:=%s; end;",LINKTYPE_TABLE,login,SQLROWCOUNT);
				StmtPrepare(hstmt,ociError, sqlstmt, &status);
				BindByName(hstmt,&bndp[0],ociError,":lkname",ltype,max_ltypelen,SQLT_STR,&ltype_nullList[0],&status);
				BindByName(hstmt,&bndp[1],ociError,":oldlkname",typeold,max_typeoldlen,SQLT_STR,&typeold_nullList[0],&status);
				BindByName(hstmt,&bndp[2],ociError,":numrows",&numrows_inserted[0],sizeof(int),SQLT_INT,0,&status);
				BindArrayOfStruct(bndp[0],ociError,max_ltypelen,sizeof(int),&status);
				BindArrayOfStruct(bndp[1],ociError,max_typeoldlen,sizeof(int),&status);
				BindArrayOfStruct(bndp[2],ociError,sizeof(int),0,&status);
				StmtExecute(ociHdbc, hstmt, ociError,NbElement, &status);
			}catch(Error err){
				sprintf(appliName,"UpdateMultipleLinkTypeNames");	///////
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
				
				if(ltype!=NULL)
					free(ltype);
				if(typeold!=NULL)
					free(typeold);
				if(numrows_inserted!=NULL)
					free(numrows_inserted);
				if(ltype_nullList!=NULL)
					free(ltype_nullList);
				if(typeold_nullList!=NULL)
					free(typeold_nullList);
								
				for(i=0;i<NbElement;i++)
					delete ltypeList[i];
				if(force_insert==1)
				{
                    FIRST_TIME=1;
					force_insert=0;
					ltypeList=(UpdtLinkType**)realloc(ltypeList, 0*sizeof(UpdtLinkType));
				}
				else if(ltypeList!=NULL)
                    free(ltypeList);

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
				delete ltypeList[i];

			if(force_insert==1)
			{
				FIRST_TIME=1;
				force_insert=0;
				ltypeList=(UpdtLinkType**)realloc(ltypeList, 0*sizeof(UpdtLinkType));
			}
			else if(ltypeList!=NULL)
                free(ltypeList);
			NbElement=0;
		}
		else
		{
			if(res_query!=0)
			{
				FIRST_TIME=0;
				GetErrorMess(appliName, "Cache Problem",ErrMess,1);
				for(i=0;i<NbElement;i++)
					delete ltypeList[i];
				if(ltypeList!=NULL)
                    free(ltypeList);
			}
			if(ociEnv==0)
			{
				FIRST_TIME=0;
				res_query=-1;
				GetErrorMess(appliName, "NOT CONNECTED TO ANY DB",ErrMess,1);
				for(i=0;i<NbElement;i++)
					delete ltypeList[i];
				if(ltypeList!=NULL)
                    free(ltypeList);
			}
			status+=res_query;
			if(ociEnv!=0 && FIRST_TIME==1 && res_query==0)
			{
				status=0;
				GetErrorMess(appliName, "NO_ERROR",ErrMess,0);
			}	
		}
		if(ltype!=NULL)
			free(ltype);
		if(typeold!=NULL)
			free(typeold);
		if(numrows_inserted!=NULL)
			free(numrows_inserted);
		if(ltype_nullList!=NULL)
			free(ltype_nullList);
		if(typeold_nullList!=NULL)
			free(typeold_nullList);

		return (rescode+status);
	}
/*****************************************************************/
/* Update a Composite Link Type,  returning an integer value.Not to used...
* The user should manage the memory : there is no memory allocation.
* @param link_name :  the link type name you want to modify. 
* @param simple_lk_list : name of the link types which compose the composite link, separated by "," (all of them)
* @param simple_lk_list :ex. Mixed_Data is composed of HLT_Data and L1_Data, so simple_lk_list="HLT_Data,L1_Data"
* @param simple_lk_list : put none if the link type is no longer a composite link type
* @param first_time1 : 1 if it's your CompositeLinkType first insert (save it into the database), 0 otherwise
* @param last_rows1 : 1 if it's your CompositeLinkType last insert (save it into the database), 0 otherwise
* @param ErrMess : error message in case of failure
* @return 0 if it is successful
*/ 
EXTERN_CONFDB
	int UpdateMultipleCompositeLinkTypes(char* link_name,char * simple_lk_list,int first_time,int last_rows,char* ErrMess)
	{
		char appliName[100]="UpdateMultipleCompositeLinkTypes";
		char sqlstmt[1000];
		OCIStmt *hstmt;
		OCIBind  *bndp[3]; 
		int rescode=0;
		int res_query=0;
		sword status=0;
		int i=0;

		char* ltype=NULL;
		char* lklist=NULL;
		int* ltype_nullList=NULL;
		int* lklist_nullList=NULL;
		
		static int FIRST_TIME=0;
		int force_insert=0;
		int* numrows_inserted=NULL;
		int numrows=0;
		
		static int NbElement;
		LinkType** ltypeList;
		int max_ltypelen=0;
		int max_lklistlen=0;
		
		if (first_time==1)
		{
			FIRST_TIME=1;
			NbElement=1;
			ltypeList=(LinkType**)malloc( NbElement*sizeof(LinkType));			
		}
		else 
		{
			if (FIRST_TIME==0&&force_insert==0)
			{
				GetErrorMess(appliName, "CACHE HAS NOT BEEN INITIALIZED",ErrMess,1);
				return -1;
			}
			NbElement++;
			ltypeList=(LinkType**)realloc(ltypeList, NbElement*sizeof(LinkType));
		}

		ltypeList[NbElement-1]=new LinkType(link_name,simple_lk_list);

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
				if(ltype!=NULL)
					free(ltype);
				if(lklist!=NULL)
					free(lklist);				
				if(numrows_inserted!=NULL)
					free(numrows_inserted);
				if(ltype_nullList!=NULL)
					free(ltype_nullList);
				if(lklist_nullList!=NULL)
					free(lklist_nullList);
				
				for(i=0;i<NbElement;i++)
					delete ltypeList[i];
				if(ltypeList!=NULL)
					free(ltypeList);
				
				free(errmessg1);
				free(errmessg2);
				return -1;
			}
			for(i=0;i<NbElement;i++){
				if (ltypeList[i]->ltypelen > max_ltypelen)
					max_ltypelen=ltypeList[i]->ltypelen;
				if (ltypeList[i]->lklistlen > max_lklistlen)
					max_lklistlen=ltypeList[i]->lklistlen;
			}

			ltype=(char*)malloc(NbElement*max_ltypelen*sizeof(char));
			lklist=(char*)malloc(NbElement*max_lklistlen*sizeof(char));
			ltype_nullList=(int*)malloc(NbElement*sizeof(int));
			lklist_nullList=(int*)malloc(NbElement*sizeof(int));

			if(ltype!=NULL && lklist!=NULL && ltype_nullList!=NULL && lklist_nullList!=NULL)
			for(i=0;i<NbElement;i++)
			{
				memcpy(ltype+i*max_ltypelen,ltypeList[i]->ltype,ltypeList[i]->ltypelen);
				ltype_nullList[i]=ltypeList[i]->ltype_null;
				memcpy(lklist+i*max_lklistlen,ltypeList[i]->lklist,ltypeList[i]->lklistlen);
				lklist_nullList[i]=ltypeList[i]->lklist_null;
			}
			numrows_inserted=(int*)malloc(sizeof(int)*NbElement);

			if( ltype==NULL || lklist==NULL || ltype_nullList==NULL || lklist_nullList==NULL || numrows_inserted==NULL)
			{
				rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");
				GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
				
				FIRST_TIME=0;
				if(ltype!=NULL)
					free(ltype);
				if(lklist!=NULL)
					free(lklist);
				if(numrows_inserted!=NULL)
					free(numrows_inserted);
				if(ltype_nullList!=NULL)
					free(ltype_nullList);
				if(lklist_nullList!=NULL)
					free(lklist_nullList);
				
				for(i=0;i<NbElement;i++)
					delete ltypeList[i];
				if(ltypeList!=NULL)
                    free(ltypeList);
				
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
				sprintf(sqlstmt,"BEGIN if :lklist='none' then update %s set last_update=sysdate, user_update='%s', link_nbr=(select p.prime_nb from %s  p where p.prime_nb_position=(select nvl(count(t.link_nbr),0)+1 from %s t,%s g where t.link_nbr=g.prime_nb )) where link_name=:lkname; else update %s set last_update=sysdate,user_update='%s',link_nbr=(select case when ceil(power(10,sum(log(10,link_nbr))))-power(10,sum(log(10,link_nbr)))>0.5  then floor(power(10,sum(log(10,link_nbr)))) else ceil(power(10,sum(log(10,link_nbr)))) end from %s t where instr(:lklist,t.link_name)>0) where link_name=:lkname; end if; :numrows:=%s; END;",LINKTYPE_TABLE,login,PRIME_NUMBER_TABLE,LINKTYPE_TABLE,PRIME_NUMBER_TABLE,LINKTYPE_TABLE,login,LINKTYPE_TABLE,SQLROWCOUNT);
				StmtPrepare(hstmt,ociError, sqlstmt, &status);
				BindByName(hstmt,&bndp[0],ociError,":lkname",ltype,max_ltypelen,SQLT_STR,&ltype_nullList[0],&status);
				BindByName(hstmt,&bndp[1],ociError,":lklist",lklist,max_lklistlen,SQLT_STR,&lklist_nullList[0],&status);
				BindByName(hstmt,&bndp[2],ociError,":numrows",&numrows_inserted[0],sizeof(int),SQLT_INT,0,&status);
				BindArrayOfStruct(bndp[0],ociError,max_ltypelen,sizeof(int),&status);
				BindArrayOfStruct(bndp[1],ociError,max_lklistlen,sizeof(int),&status);
				BindArrayOfStruct(bndp[2],ociError,sizeof(int),0,&status);
				StmtExecute(ociHdbc, hstmt, ociError,NbElement, &status);
			}catch(Error err){
				sprintf(appliName,"UpdateMultipleCompositeLinkTypes");	///////
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
				
				if(ltype!=NULL)
					free(ltype);
				if(lklist!=NULL)
					free(lklist);
				if(numrows_inserted!=NULL)
					free(numrows_inserted);
				if(ltype_nullList!=NULL)
					free(ltype_nullList);
				if(lklist_nullList!=NULL)
					free(lklist_nullList);
								
				for(i=0;i<NbElement;i++)
					delete ltypeList[i];
				if(force_insert==1)
				{
                    FIRST_TIME=1;
					force_insert=0;
					ltypeList=(LinkType**)realloc(ltypeList, 0*sizeof(LinkType));
				}
				else if(ltypeList!=NULL)
                    free(ltypeList);

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
				delete ltypeList[i];

			if(force_insert==1)
			{
				FIRST_TIME=1;
				force_insert=0;
				ltypeList=(LinkType**)realloc(ltypeList, 0*sizeof(LinkType));
			}
			else if(ltypeList!=NULL)
                free(ltypeList);
			NbElement=0;
		}
		else
		{
			if(res_query!=0)
			{
				FIRST_TIME=0;
				GetErrorMess(appliName, "Cache Problem",ErrMess,1);
				for(i=0;i<NbElement;i++)
					delete ltypeList[i];
				if(ltypeList!=NULL)
                    free(ltypeList);
			}
			if(ociEnv==0)
			{
				FIRST_TIME=0;
				res_query=-1;
				GetErrorMess(appliName, "NOT CONNECTED TO ANY DB",ErrMess,1);
				for(i=0;i<NbElement;i++)
					delete ltypeList[i];
				if(ltypeList!=NULL)
                    free(ltypeList);
			}
			status+=res_query;
			if(ociEnv!=0 && FIRST_TIME==1 && res_query==0)
			{
				status=0;
				GetErrorMess(appliName, "NO_ERROR",ErrMess,0);
			}	
		}
		if(ltype!=NULL)
			free(ltype);
		if(lklist!=NULL)
			free(lklist);
		if(numrows_inserted!=NULL)
			free(numrows_inserted);
		if(ltype_nullList!=NULL)
			free(ltype_nullList);
		if(lklist_nullList!=NULL)
			free(lklist_nullList);

		return (rescode+status);
	}

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif