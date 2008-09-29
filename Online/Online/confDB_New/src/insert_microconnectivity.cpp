/********************************************************************************/
//	Author: L.Abadie
// version:v3.5
// changes: handling error such that can be extended to C++: (create a class SQLException) (strlen ck + update stmt)
// file which performs bulk collect insert for the connectivity table
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
* Insert Multiple MicroLink  (i.e. these are physical cables)returning an integer value.
* The user should manage the memory : there is no memory allocation.
* @param node_from : name of the board cpnt starting the link : put "motherboard", if it's the board name in which the board cpnt sits.
* @param node_to : name of the board cpnt ending the link : put "motherboard", if it's the board name in which the board cpnt sits.
* @param port_nbfrom : port nb of the cpnt from. Put the portid if node_from="motherboard"
* @param port_nbto : port nb of the cpnt to. Put the portid if node_to="motherboard"
* @param link_type : name of the link type
* @param bidirectional_link_used :  1 if it's a bidirectional link, 0 otherwise
* @param first_time1 : 1 if it's your MultipleLink first insert (save it into the database), 0 otherwise
* @param last_rows1 : 1 if it's your MultipleLink last insert (save it into the database), 0 otherwise. Maximum in one go: 10000 rows. (we force the insert)
* @param ErrMess : error message in case of failure
* @return 0 if it is successful
*/ 
EXTERN_CONFDB
	int InsertMultipleMicroLinks(char* node_from,char* node_to,int port_nbfrom,int port_nbto,char* link_type,int bidirectional_link_used,int first_time, int last_rows,char* ErrMess)
	{
		char appliName[100]="InsertMultipleMicroLinks";
		char sqlstmt[2000];
		OCIStmt *hstmt;
		OCIBind  *bndp[7]; 
		
		int rescode=0;
		int res_query=0;
		sword status=0;
		int i=0;

		char* nodefrom=NULL;
		char* nodeto=NULL;
		char* linktype=NULL;
		int* bidirectional_lkused=0;
		int* portnbfrom=NULL;
		int* portnbto=NULL;

		int* nodefrom_nullList=NULL;
		int* nodeto_nullList=NULL;
		int* linktype_nullList=NULL;

		static int FIRST_TIME=0;
		int force_insert=0;
		char seqname[100]="lhcb_microconnseq.nextval";
		int* numrows_inserted=NULL;
		int numrows=0;
		
		static int NbElement;
		MicroLink** linkList;
		int max_nodefromlen=0;
		int max_nodetolen=0;
		int max_linktypelen=0;

		if (first_time==1)
		{
			FIRST_TIME=1;
			NbElement=1;
			linkList=(MicroLink**)malloc( NbElement*sizeof(MicroLink));			
		}
		else 
		{
			if (FIRST_TIME==0&&force_insert==0)
			{
				GetErrorMess(appliName, "CACHE HAS NOT BEEN INITIALIZED",ErrMess,1);
				return -1;
			}
			NbElement++;
			linkList=(MicroLink**)realloc(linkList, NbElement*sizeof(MicroLink));
		}

		linkList[NbElement-1]=new MicroLink(node_from,node_to,port_nbfrom,port_nbto,link_type,bidirectional_link_used);
	
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
				
				if(nodefrom!=NULL)
					free(nodefrom);
				if(nodeto!=NULL)
					free(nodeto);
				if(linktype!=NULL)
					free(linktype);
				if(portnbfrom!=NULL)
					free(portnbfrom);
				if(portnbto!=NULL)
					free(portnbto);
				if(bidirectional_lkused!=NULL)
					free(bidirectional_lkused);
				if(nodefrom_nullList!=NULL)
					free(nodefrom_nullList);
				if(nodeto_nullList!=NULL)
					free(nodeto_nullList);
				if(linktype_nullList!=NULL)
					free(linktype_nullList);	

				for(i=0;i<NbElement;i++)
					delete linkList[i];
				if(linkList!=NULL)
					free(linkList);
				
				free(errmessg1);
				free(errmessg2);
				return -1;
			}
			for(i=0;i<NbElement;i++){
				if (linkList[i]->nodefromlen > max_nodefromlen)
					max_nodefromlen=linkList[i]->nodefromlen;
				if (linkList[i]->nodetolen > max_nodetolen)
					max_nodetolen=linkList[i]->nodetolen;
				if (linkList[i]->linktypelen > max_linktypelen)
					max_linktypelen=linkList[i]->linktypelen;
			}
			nodefrom=(char*)malloc(NbElement*max_nodefromlen*sizeof(char));
			nodeto=(char*)malloc(NbElement*max_nodetolen*sizeof(char));
			linktype=(char*)malloc(NbElement*max_linktypelen*sizeof(char));
			portnbfrom=(int*)malloc(sizeof(int)*NbElement);			
			portnbto=(int*)malloc(sizeof(int)*NbElement);			
			bidirectional_lkused=(int*)malloc(sizeof(int)*NbElement);			
			nodefrom_nullList=(int*)malloc(NbElement*sizeof(int));
			nodeto_nullList=(int*)malloc(NbElement*sizeof(int));
			linktype_nullList=(int*)malloc(NbElement*sizeof(int));
		
			if(nodefrom!=NULL && nodeto!=NULL && portnbfrom!=NULL && portnbto!=NULL && linktype!=NULL 
				&& bidirectional_lkused!=NULL && nodefrom_nullList!=NULL && nodeto_nullList!=NULL && linktype_nullList!=NULL)

			for(i=0;i<NbElement;i++)
			{
				memcpy(nodefrom+i*max_nodefromlen,linkList[i]->nodefrom,linkList[i]->nodefromlen);
				nodefrom_nullList[i]=linkList[i]->nodefrom_null;
				memcpy(nodeto+i*max_nodetolen,linkList[i]->nodeto,linkList[i]->nodetolen);
				nodeto_nullList[i]=linkList[i]->nodeto_null;
				memcpy(linktype+i*max_linktypelen,linkList[i]->linktype,linkList[i]->linktypelen);
				linktype_nullList[i]=linkList[i]->linktype_null;				
				portnbfrom[i]=linkList[i]->portnbfrom;
				portnbto[i]=linkList[i]->portnbto;
				bidirectional_lkused[i]=linkList[i]->bidirectional_lkused;
			}
			numrows_inserted=(int*)malloc(sizeof(int)*NbElement);

			if( nodefrom==NULL || nodeto==NULL || portnbfrom==NULL || portnbto==NULL || linktype==NULL || bidirectional_lkused==0 
				|| nodefrom_nullList==NULL || nodeto_nullList==NULL || linktype_nullList==NULL || numrows_inserted==NULL )
			{
				rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");
				GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
				
				FIRST_TIME=0;
				if(nodefrom!=NULL)
					free(nodefrom);
				if(nodeto!=NULL)
					free(nodeto);
				if(linktype!=NULL)
					free(linktype);
				if(portnbfrom!=NULL)
					free(portnbfrom);
				if(portnbto!=NULL)
					free(portnbto);
				if(numrows_inserted!=NULL)
					free(numrows_inserted);
				if(bidirectional_lkused!=NULL)
					free(bidirectional_lkused);
				if(nodefrom_nullList!=NULL)
					free(nodefrom_nullList);
				if(nodeto_nullList!=NULL)
					free(nodeto_nullList);
				if(linktype_nullList!=NULL)
					free(linktype_nullList);	
				
				for(i=0;i<NbElement;i++)
					delete linkList[i];
				if(linkList!=NULL)
                    free(linkList);
				
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
				sprintf(sqlstmt,"BEGIN if :nfrom='motherboard' then insert ALL into %s(linkid,cpntidfrom,portidfrom,cpntidto,portidto,link_type,bidirectional_link_used,link_weight,lkused,author,created,terminal_name) values (%s,-1,:portnbfrom,cpntidto,:portnbto,lknb,:biused,lkweight,1,'%s',sysdate,'%s') select s.linktypeid as lknb,f.cpntid as cpntidto,decode(f.cpntid,-1,3,1) as lkweight from %s f,%s s where f.cpntname=:nto and s.link_name=:lktype ; else if :nto='motherboard' then insert ALL into %s(linkid,cpntidfrom,portidfrom,cpntidto,portidto,link_type,bidirectional_link_used,link_weight,lkused,author,created,terminal_name) values (%s,cpntidfrom,:portnbfrom,-1,:portnbto,lknb,:biused,lkweight,1,'%s',sysdate,'%s') select s.linktypeid as lknb,f.cpntid as cpntidfrom, decode(f.cpntid,-1,3,2) as lkweight from %s f,%s s where f.cpntname=:nfrom and s.link_name=:lktype; else insert ALL into %s(linkid,cpntidfrom,portidfrom,cpntidto,portidto,link_type,bidirectional_link_used,link_weight,lkused,author,created,terminal_name) values (%s,cpntidfrom,:portnbfrom,cpntidto,:portnbto,lknb,:biused,0,1,'%s',sysdate,'%s') select s.linktypeid as lknb,t.cpntid as cpntidfrom,f.cpntid as cpntidto from %s t,%s f,%s s where t.cpntname=:nfrom and f.cpntname=:nto and s.link_name=:lktype and f.motherboardid=t.motherboardid; end if;end if;:numrows:=%s;end;",MICRO_CONNECTIVITY_TABLE,seqname,login,host,LG_CPNTS_TABLE,LINKTYPE_TABLE,MICRO_CONNECTIVITY_TABLE,seqname,login,host,LG_CPNTS_TABLE,LINKTYPE_TABLE,MICRO_CONNECTIVITY_TABLE,seqname,login,host,LG_CPNTS_TABLE,LG_CPNTS_TABLE,LINKTYPE_TABLE,SQLROWCOUNT);
				StmtPrepare(hstmt,ociError, sqlstmt, &status);
				
				BindByName(hstmt,&bndp[0],ociError,":portnbfrom",&portnbfrom[0],sizeof(int),SQLT_INT,0,&status);
				BindByName(hstmt,&bndp[1],ociError,":portnbto",&portnbto[0],sizeof(int),SQLT_INT,0,&status);
				BindByName(hstmt,&bndp[2],ociError,":biused",&bidirectional_lkused[0],sizeof(&linkList[0]->bidirectional_lkused),SQLT_INT,0,&status);
				BindByName(hstmt,&bndp[3],ociError,":nfrom",nodefrom,max_nodefromlen,SQLT_STR,&nodefrom_nullList[0],&status);
				BindByName(hstmt,&bndp[4],ociError,":nto",nodeto,max_nodetolen,SQLT_STR,&nodeto_nullList[0],&status);
				BindByName(hstmt,&bndp[5],ociError,":lktype",linktype,max_linktypelen,SQLT_STR,&linktype_nullList[0],&status);
				BindByName(hstmt,&bndp[6],ociError,":numrows",&numrows_inserted[0],sizeof(int),SQLT_INT,0,&status);

				BindArrayOfStruct(bndp[0],ociError,sizeof(int),0,&status);
				BindArrayOfStruct(bndp[1],ociError,sizeof(int),0,&status);
				BindArrayOfStruct(bndp[2],ociError,sizeof(int),0,&status);
				BindArrayOfStruct(bndp[3],ociError,max_nodefromlen,sizeof(int),&status);
				BindArrayOfStruct(bndp[4],ociError,max_nodetolen,sizeof(int),&status);
				BindArrayOfStruct(bndp[5],ociError,max_linktypelen,sizeof(int),&status);
				BindArrayOfStruct(bndp[6],ociError,sizeof(int),0,&status);

				StmtExecute(ociHdbc, hstmt, ociError,NbElement, &status);
				
			}catch(Error err){
				sprintf(appliName,"InsertMultipleMacroLinks");	///////
				rescode=ShowErrors (status, err.ociError, err.log);
				
				if(ociError!=0)
					OCIReportError(ociError,appliName,ErrMess,1); 
				else
					GetErrorMess(appliName,err.msg,ErrMess,1);
			}
			
			//check if all rows has been inserted
			//if not we do a rollback

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
				
				if(nodefrom!=NULL)
					free(nodefrom);
				if(nodeto!=NULL)
					free(nodeto);
				if(linktype!=NULL)
					free(linktype);
				if(portnbfrom!=NULL)
					free(portnbfrom);
				if(portnbto!=NULL)
					free(portnbto);
				if(numrows_inserted!=NULL)
					free(numrows_inserted);
				if(bidirectional_lkused!=NULL)
					free(bidirectional_lkused);
				if(nodefrom_nullList!=NULL)
					free(nodefrom_nullList);
				if(nodeto_nullList!=NULL)
					free(nodeto_nullList);
				if(linktype_nullList!=NULL)
					free(linktype_nullList);	
				
				for(i=0;i<NbElement;i++)
					delete linkList[i];
				if(force_insert==1)
				{
                    FIRST_TIME=1;
					force_insert=0;
					linkList=(MicroLink**)realloc(linkList, 0*sizeof(MicroLink));
				}
				else if(linkList!=NULL)
                    free(linkList);

				NbElement=0;
				GetErrorMess(appliName, "COULDNOT_INSERT_ALL_ROWS",ErrMess,1);
				return -1;
			}

			else
            {
				status = OCITransCommit(ociHdbc, ociError, 0);
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
				delete linkList[i];

			if(force_insert==1)
			{
				FIRST_TIME=1;
				force_insert=0;
				linkList=(MicroLink**)realloc(linkList, 0*sizeof(MicroLink));
			}
			else if(linkList!=NULL)
                free(linkList);
			NbElement=0;
		}

		else
		{
			if(res_query!=0)
			{
				FIRST_TIME=0;
				GetErrorMess(appliName, "Cache Problem",ErrMess,1);
				for(i=0;i<NbElement;i++)
					delete linkList[i];
				if(linkList!=NULL)
                    free(linkList);
			}
			if(ociEnv==0)
			{
				FIRST_TIME=0;
				res_query=-1;
				GetErrorMess(appliName, "NOT CONNECTED TO ANY DB",ErrMess,1);
				for(i=0;i<NbElement;i++)
					delete linkList[i];
				if(linkList!=NULL)
                    free(linkList);
			}
			status+=res_query;
			if(ociEnv!=0 && FIRST_TIME==1 && res_query==0)
			{
				status=0;
				GetErrorMess(appliName, "NO_ERROR",ErrMess,0);
			}	
		}

		if(nodefrom!=NULL)
			free(nodefrom);
		if(nodeto!=NULL)
			free(nodeto);
		if(linktype!=NULL)
			free(linktype);
		if(portnbfrom!=NULL)
			free(portnbfrom);
		if(portnbto!=NULL)
			free(portnbto);
		if(numrows_inserted!=NULL)
			free(numrows_inserted);
		if(bidirectional_lkused!=NULL)
			free(bidirectional_lkused);
		if(nodefrom_nullList!=NULL)
			free(nodefrom_nullList);
		if(nodeto_nullList!=NULL)
			free(nodeto_nullList);
		if(linktype_nullList!=NULL)
			free(linktype_nullList);	

		return (status+rescode);
	}

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif
