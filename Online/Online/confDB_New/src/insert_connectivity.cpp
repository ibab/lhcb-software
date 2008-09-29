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
* Insert Multiple MacroLink  (i.e. these are physical cables)returning an integer value.
* The user should manage the memory : there is no memory allocation.
* @param node_from : name of the device starting the link
* @param node_to : name of the device ending the link
* @param port_nbfrom : port nb of the device from
* @param port_nbto : port nb of the device to
* @param port_typefrom : port type (used for the DAQ: as one physical port is considered as several functional interfaces). Put "", if none.
* @param port_typeto : port type (used for the DAQ: as one physical port is considered as several functional interfaces). Put "", if none.
* @param link_type : name of the link type
* @param link_information : some comments on the links. Put "" if none.
* @param bidirectional_link_used :  1 if it's a bidirectional link (it means that if it is consider as unidirectional some paths won't be found), 0 otherwise. 
* @param first_time1 : 1 if it's your MultipleLink first insert (save it into the database), 0 otherwise
* @param last_rows1 : 1 if it's your MultipleLink last insert (save it into the database), 0 otherwise. Maximum in one go: 10000 rows. (we force the insert)
* @param ErrMess : error message in case of failure
* @return 0 if it is successful
*/ 
EXTERN_CONFDB
	int InsertMultipleMacroLinks(char* node_from,char* node_to,char* port_nbfrom,char* port_nbto,char* port_typefrom,char* port_typeto,char* link_type,char* link_information,int bidirectional_link_used,int first_time, int last_rows,char* ErrMess)
	{
		char appliName[100]="InsertMultipleMacroLinks";
		char sqlstmt[2500];
		OCIStmt *hstmt;
		OCIBind  *bndp[10]; 
		int rescode=0;
		int res_query=0;
		sword status=0;
		int i=0;

		char* nodefrom=NULL;
		char* nodeto=NULL;
		char* portnbfrom=NULL;
		char* portypefrom=NULL;
		char* portnbto=NULL;
		char* portypeto=NULL;
		char* lkinfo=NULL;
		char* linktype=NULL;
		int* bidirectional_lkused=0;
		
		int* nodefrom_nullList=NULL;
		int* nodeto_nullList=NULL;
		int* portnbfrom_nullList=NULL;
		int* portypefrom_nullList=NULL;
		int* portnbto_nullList=NULL;
		int* portypeto_nullList=NULL;
		int* lkinfo_nullList=NULL;
		int* linktype_nullList=NULL;

		static int FIRST_TIME=0;
		int force_insert=0;
		char seq_name[100]="lhcb_macroconnseq.nextval";
		int* numrows_inserted=NULL;
		int numrows=0;
		
		static int NbElement;
		MacroLink** linkList;
		int max_nodefromlen=0;
		int max_nodetolen=0;
		int max_portnbfromlen=0;
		int max_portypefromlen=0;
		int max_portnbtolen=0;
		int max_portypetolen=0;
		int max_lkinfolen=0;
		int max_linktypelen=0;

		char res_query1[100]="";
		
		if(port_typefrom==NULL || strlen(port_typefrom)<1)
			port_typefrom="none";
		if(port_typeto==NULL || strlen(port_typeto)<1)
			port_typeto="none";

		if (first_time==1)
		{
			FIRST_TIME=1;
			NbElement=1;
			linkList=(MacroLink**)malloc( NbElement*sizeof(MacroLink));			
		}
		else 
		{
			if (FIRST_TIME==0&&force_insert==0)
			{
				GetErrorMess(appliName, "CACHE HAS NOT BEEN INITIALIZED",ErrMess,1);
				return -1;
			}
			NbElement++;
			linkList=(MacroLink**)realloc(linkList, NbElement*sizeof(MacroLink));
		}
	
		linkList[NbElement-1]=new MacroLink(node_from,node_to,port_nbfrom,port_nbto,port_typefrom,port_typeto,link_type,link_information,bidirectional_link_used);
		
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

			int sysID=GetSubsystemID("DAQ,TFC");/////////////////////
			
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
				if(portypefrom!=NULL)
					free(portypefrom);
				if(portypeto!=NULL)
					free(portypeto);
				if(lkinfo!=NULL)
					free(lkinfo);
				if(bidirectional_lkused!=NULL)
					free(bidirectional_lkused);
				if(nodefrom_nullList!=NULL)
					free(nodefrom_nullList);
				if(nodeto_nullList!=NULL)
					free(nodeto_nullList);
				if(portnbfrom_nullList!=NULL)
					free(portnbfrom_nullList);
				if(portypefrom_nullList!=NULL)
					free(portypefrom_nullList);
				if(portnbto_nullList!=NULL)
					free(portnbto_nullList);	
				if(portypeto_nullList!=NULL)
					free(portypeto_nullList);	
				if(lkinfo_nullList!=NULL)
					free(lkinfo_nullList);	
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
				if (linkList[i]->portnbfromlen > max_portnbfromlen)
					max_portnbfromlen=linkList[i]->portnbfromlen;				
				if (linkList[i]->portypefromlen > max_portypefromlen)
					max_portypefromlen=linkList[i]->portypefromlen;
				if (linkList[i]->portnbtolen > max_portnbtolen)
					max_portnbtolen=linkList[i]->portnbtolen;
				if (linkList[i]->portypetolen > max_portypetolen)
					max_portypetolen=linkList[i]->portypetolen;
				if (linkList[i]->lkinfolen > max_lkinfolen)
					max_lkinfolen=linkList[i]->lkinfolen;
				if (linkList[i]->linktypelen > max_linktypelen)
					max_linktypelen=linkList[i]->linktypelen;
			}
			nodefrom=(char*)malloc(NbElement*max_nodefromlen*sizeof(char));
			nodeto=(char*)malloc(NbElement*max_nodetolen*sizeof(char));
			portnbfrom=(char*)malloc(NbElement*max_portnbfromlen*sizeof(char));
			portypefrom=(char*)malloc(NbElement*max_portypefromlen*sizeof(char));
			portnbto=(char*)malloc(NbElement*max_portnbtolen*sizeof(char));
			portypeto=(char*)malloc(NbElement*max_portypetolen*sizeof(char));
			lkinfo=(char*)malloc(NbElement*max_lkinfolen*sizeof(char));
			linktype=(char*)malloc(NbElement*max_linktypelen*sizeof(char));
			bidirectional_lkused=(int*)malloc(sizeof(int)*NbElement);			
			nodefrom_nullList=(int*)malloc(NbElement*sizeof(int));
			nodeto_nullList=(int*)malloc(NbElement*sizeof(int));
			portnbfrom_nullList=(int*)malloc(NbElement*sizeof(int));
			portypefrom_nullList=(int*)malloc(NbElement*sizeof(int));
			portnbto_nullList=(int*)malloc(NbElement*sizeof(int));
			portypeto_nullList=(int*)malloc(NbElement*sizeof(int));
			lkinfo_nullList=(int*)malloc(NbElement*sizeof(int));
			linktype_nullList=(int*)malloc(NbElement*sizeof(int));
		
			if(nodefrom!=NULL && nodeto!=NULL && portnbfrom!=NULL && portypefrom!=NULL && portnbto!=NULL && portypeto!=NULL && lkinfo!=NULL && linktype!=NULL && bidirectional_lkused!=NULL && nodefrom_nullList!=NULL 
				&& nodeto_nullList!=NULL && portnbfrom_nullList!=NULL && portypefrom_nullList!=NULL && portnbto_nullList!=NULL && portypeto_nullList!=NULL && lkinfo_nullList!=NULL && linktype_nullList!=NULL)

			for(i=0;i<NbElement;i++)
			{
				memcpy(nodefrom+i*max_nodefromlen,linkList[i]->nodefrom,linkList[i]->nodefromlen);
				nodefrom_nullList[i]=linkList[i]->nodefrom_null;
				memcpy(nodeto+i*max_nodetolen,linkList[i]->nodeto,linkList[i]->nodetolen);
				nodeto_nullList[i]=linkList[i]->nodeto_null;
				memcpy(portnbfrom+i*max_portnbfromlen,linkList[i]->portnbfrom,linkList[i]->portnbfromlen);
				portnbfrom_nullList[i]=linkList[i]->portnbfrom_null;
				memcpy(portypefrom+i*max_portypefromlen,linkList[i]->portypefrom,linkList[i]->portypefromlen);
				portypefrom_nullList[i]=linkList[i]->portypefrom_null;
				memcpy(portnbto+i*max_portnbtolen,linkList[i]->portnbto,linkList[i]->portnbtolen);
				portnbto_nullList[i]=linkList[i]->portnbto_null;
				memcpy(portypeto+i*max_portypetolen,linkList[i]->portypeto,linkList[i]->portypetolen);
				portypeto_nullList[i]=linkList[i]->portypeto_null;
				memcpy(lkinfo+i*max_lkinfolen,linkList[i]->lkinfo,linkList[i]->lkinfolen);
				lkinfo_nullList[i]=linkList[i]->lkinfo_null;
				memcpy(linktype+i*max_linktypelen,linkList[i]->linktype,linkList[i]->linktypelen);
				linktype_nullList[i]=linkList[i]->linktype_null;				
				bidirectional_lkused[i]=linkList[i]->bidirectional_lkused;
			}
			numrows_inserted=(int*)malloc(sizeof(int)*NbElement);

			if( nodefrom==NULL || nodeto==NULL || portnbfrom==NULL || portypefrom==NULL || portnbto==NULL || portypeto==NULL || lkinfo==NULL || linktype==NULL || bidirectional_lkused==0 || nodefrom_nullList==NULL 
				|| nodeto_nullList==NULL || portnbfrom_nullList==NULL || portypefrom_nullList==NULL || portnbto_nullList==NULL || portypeto_nullList==NULL || lkinfo_nullList==NULL || linktype_nullList==NULL || numrows_inserted==NULL )
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
				if(portypefrom!=NULL)
					free(portypefrom);
				if(portypeto!=NULL)
					free(portypeto);
				if(lkinfo!=NULL)
					free(lkinfo);
				if(numrows_inserted!=NULL)
					free(numrows_inserted);
				if(bidirectional_lkused!=NULL)
					free(bidirectional_lkused);
				if(nodefrom_nullList!=NULL)
					free(nodefrom_nullList);
				if(nodeto_nullList!=NULL)
					free(nodeto_nullList);
				if(portnbfrom_nullList!=NULL)
					free(portnbfrom_nullList);
				if(portypefrom_nullList!=NULL)
					free(portypefrom_nullList);
				if(portnbto_nullList!=NULL)
					free(portnbto_nullList);	
				if(portypeto_nullList!=NULL)
					free(portypeto_nullList);	
				if(lkinfo_nullList!=NULL)
					free(lkinfo_nullList);	
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
				sprintf(sqlstmt,"BEGIN insert ALL into %s (linkid,portidfrom,portidto,link_type,bidirectional_link_used,link_weight,lkused,link_info,author,user_update,terminal_name) values (%s,portidfrom,portidto,lknb,:biused, lkweight,1,:lkinfo,'%s','%s','%s') select f.portid as portidfrom,g.portid as portidto,s.linktypeID as lknb,decode(m.node,1,l.node+m.node+1,l.node+m.node) as lkweight from %s f,%s g,%s l,%s m,%s s ,%s k where f.deviceid=l.deviceid and l.devicename=:nfrom and g.deviceid=m.deviceid and m.devicename=:nto and s.link_name=:lktype and f.port_nbr=:portnbfrom and f.port_way=2 and nvl(f.port_type,'none')=:ptypefrom and g.port_nbr=:portnbto and g.port_way=1 and nvl(g.port_type,'none')=:ptypeto and mod(l.system_name,k.systemID)=0 and mod(m.system_name,k.systemID)=0 and rownum=1; if %s=1 then update %s r set r.system_name=(select case when ceil(power(10,sum(distinct log(10,t.systemID))))-power(10,sum(distinct log(10,t.systemID)))>0.5  then floor(power(10,sum(distinct log(10,t.systemid)))) else ceil(power(10,sum(distinct log(10,t.systemid)))) end from %s t,%s o,%s d,%s p,%s h where t.systemid not in (select q.systemid from %s v, %s q where mod(v.systemid,q.systemid)=0 and v.systemid!=q.systemid) and mod(o.system_name,t.systemid)=0 and mod(d.system_name,t.systemid)=0 and o.deviceid=p.deviceid and p.portid=r.portidfrom and r.portidto=h.portid and h.deviceid=d.deviceid) where r.system_name=-1 and r.portidfrom in (select v.portid from %s v,%s b where v.deviceid=b.deviceid and b.devicename=:nfrom) and r.portidto in (select v.portid from %s v,%s b where v.deviceid=b.deviceid and b.devicename=:nto);  end if;  :numrows:=%s; END;",MACRO_CONNECTIVITY_TABLE,seq_name,login,login,host,PORT_TABLE,PORT_TABLE,LOGICAL_DEVICE_TABLE,LOGICAL_DEVICE_TABLE,LINKTYPE_TABLE,SUBSYSTEM_TABLE,SQLROWCOUNT,MACRO_CONNECTIVITY_TABLE,SUBSYSTEM_TABLE,LOGICAL_DEVICE_TABLE,LOGICAL_DEVICE_TABLE,PORT_TABLE,PORT_TABLE,SUBSYSTEM_TABLE,SUBSYSTEM_TABLE,PORT_TABLE,LOGICAL_DEVICE_TABLE,PORT_TABLE,LOGICAL_DEVICE_TABLE,SQLROWCOUNT);	
				StmtPrepare(hstmt,ociError, sqlstmt, &status);

				BindByName(hstmt,&bndp[0],ociError,":portnbfrom",portnbfrom,max_portnbfromlen,SQLT_STR,&portnbfrom_nullList[0],&status);
				BindByName(hstmt,&bndp[1],ociError,":portnbto",portnbto,max_portnbtolen,SQLT_STR,&portnbto_nullList[0],&status);
				BindByName(hstmt,&bndp[2],ociError,":biused",&bidirectional_lkused[0],sizeof(&linkList[0]->bidirectional_lkused),SQLT_INT,0,&status);
				BindByName(hstmt,&bndp[3],ociError,":nfrom",nodefrom,max_nodefromlen,SQLT_STR,&nodefrom_nullList[0],&status);
				BindByName(hstmt,&bndp[4],ociError,":nto",nodeto,max_nodetolen,SQLT_STR,&nodeto_nullList[0],&status);
				BindByName(hstmt,&bndp[5],ociError,":lktype",linktype,max_linktypelen,SQLT_STR,&linktype_nullList[0],&status);
				BindByName(hstmt,&bndp[6],ociError,":ptypeto",portypeto,max_portypetolen,SQLT_STR,&portypeto_nullList[0],&status);
				BindByName(hstmt,&bndp[7],ociError,":ptypefrom",portypefrom,max_portypefromlen,SQLT_STR,&portypefrom_nullList[0],&status);
				BindByName(hstmt,&bndp[8],ociError,":lkinfo",lkinfo,max_lkinfolen,SQLT_STR,&lkinfo_nullList[0],&status);
				BindByName(hstmt,&bndp[9],ociError,":numrows",&numrows_inserted[0],sizeof(int),SQLT_INT,0,&status);

				BindArrayOfStruct(bndp[0],ociError,max_portnbfromlen,sizeof(int),&status);
				BindArrayOfStruct(bndp[1],ociError,max_portnbtolen,sizeof(int),&status);
				BindArrayOfStruct(bndp[2],ociError,sizeof(int),0,&status);
				BindArrayOfStruct(bndp[3],ociError,max_nodefromlen,sizeof(int),&status);
				BindArrayOfStruct(bndp[4],ociError,max_nodetolen,sizeof(int),&status);
				BindArrayOfStruct(bndp[5],ociError,max_linktypelen,sizeof(int),&status);
				BindArrayOfStruct(bndp[6],ociError,max_portypetolen,sizeof(int),&status);
				BindArrayOfStruct(bndp[7],ociError,max_portypefromlen,sizeof(int),&status);
				BindArrayOfStruct(bndp[8],ociError,max_lkinfolen,sizeof(int),&status);
				BindArrayOfStruct(bndp[9],ociError,sizeof(int),0,&status);

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
				sprintf(sqlstmt,"BEGIN  :rescode:=%s(%d); :numrows:=%s; END; ",_UpdateDynamicTables,sysID,SQLROWCOUNT); 
			
				try{
					StmtPrepare(hstmt,ociError, sqlstmt, &status);
					BindByName(hstmt,&bndp[0],ociError,":numrows",&numrows,sizeof(int),SQLT_INT,0,&status);
					BindByName(hstmt,&bndp[1],ociError,":rescode",res_query1,101,SQLT_STR,0,&status);
					StmtExecute(ociHdbc, hstmt, ociError,1, &status);
					TransCommit(ociHdbc, ociError, 0, &status );
                
				}catch(Error err){
					rescode=ShowErrors (status, err.ociError, err.log);				
				}
				
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
				if(portypefrom!=NULL)
					free(portypefrom);
				if(portypeto!=NULL)
					free(portypeto);
				if(lkinfo!=NULL)
					free(lkinfo);
				if(numrows_inserted!=NULL)
					free(numrows_inserted);
				if(bidirectional_lkused!=NULL)
					free(bidirectional_lkused);
				if(nodefrom_nullList!=NULL)
					free(nodefrom_nullList);
				if(nodeto_nullList!=NULL)
					free(nodeto_nullList);
				if(portnbfrom_nullList!=NULL)
					free(portnbfrom_nullList);
				if(portypefrom_nullList!=NULL)
					free(portypefrom_nullList);
				if(portnbto_nullList!=NULL)
					free(portnbto_nullList);	
				if(portypeto_nullList!=NULL)
					free(portypeto_nullList);	
				if(lkinfo_nullList!=NULL)
					free(lkinfo_nullList);	
				if(linktype_nullList!=NULL)
					free(linktype_nullList);	
				
				for(i=0;i<NbElement;i++)
					delete linkList[i];
				if(force_insert==1)
				{
                    FIRST_TIME=1;
					force_insert=0;
					linkList=(MacroLink**)realloc(linkList, 0*sizeof(MacroLink));
				}
				else if(linkList!=NULL)
                    free(linkList);

				NbElement=0;
				GetErrorMess(appliName, "COULDNOT_INSERT_ALL_ROWS:CHECK SYSTEM COMPATIBILITY",ErrMess,1);
				return -1;
			}
            
            else
            {
				status = OCITransCommit(ociHdbc, ociError, 0);
				sprintf(sqlstmt,"BEGIN  :rescode:=%s(%d); :numrows:=%s; END; ",_UpdateDynamicTables,sysID,SQLROWCOUNT); 

                try{
					StmtPrepare(hstmt,ociError, sqlstmt, &status);
					BindByName(hstmt,&bndp[0],ociError,":numrows",&numrows,sizeof(int),SQLT_INT,0,&status);
					BindByName(hstmt,&bndp[1],ociError,":rescode",res_query1,101,SQLT_STR,0,&status);
					StmtExecute(ociHdbc, hstmt, ociError,1, &status);
					TransCommit(ociHdbc, ociError, 0, &status );
				}catch(Error err){
					rescode=ShowErrors (status, err.ociError, err.log);
				}
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
				linkList=(MacroLink**)realloc(linkList, 0*sizeof(MacroLink));
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
		if(portypefrom!=NULL)
			free(portypefrom);
		if(portypeto!=NULL)
			free(portypeto);
		if(lkinfo!=NULL)
			free(lkinfo);
		if(numrows_inserted!=NULL)
			free(numrows_inserted);
		if(bidirectional_lkused!=NULL)
			free(bidirectional_lkused);
		if(nodefrom_nullList!=NULL)
			free(nodefrom_nullList);
		if(nodeto_nullList!=NULL)
			free(nodeto_nullList);
		if(portnbfrom_nullList!=NULL)
			free(portnbfrom_nullList);
		if(portypefrom_nullList!=NULL)
			free(portypefrom_nullList);
		if(portnbto_nullList!=NULL)
			free(portnbto_nullList);	
		if(portypeto_nullList!=NULL)
			free(portypeto_nullList);	
		if(lkinfo_nullList!=NULL)
			free(lkinfo_nullList);	
		if(linktype_nullList!=NULL)
			free(linktype_nullList);	

		return (status+rescode);
	}

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif