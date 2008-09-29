/********************************************************************************/
//	Author: L.Abadie
// version:v3.5
// changes: handling error such that can be extended to C++: (create a class SQLException) (strlen ck + update stmt)
// file which performs bulk collect insert for the cpnt table
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
	* Insert a cpnt into the functional_cpnt table and the hw table :it means that the hw cpnt is currently IN_USE and it's having the given functional fct returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* @param cpntname : functional name of the cpnt (even if it's a spare cpnt, put its function name in the board)
	* @param cpnttype : name of the cpnttype
	* @param replacable : 1 if it's replacable on its own , 0 otherwise.
	* @param motherboardname : Put "none", if you insert a spare board cpnt. otherwise the name (functional device name) where the board cpnt sits
	* @param serial_nb :  serial_nb of the cpnt
	* @param hwtype :hardware type : this one should be the same as you declared for your spare (to manage spare cpnt)
	* @param responsible :guy responsible for this hw
	* @param comments : some comments about the hw (max.1000 characters)
	* @param location :  Position of your cpnt on the board if it's not a spare, if it's a spare its location.
	* @param first_time1 : 1 if it's your first MultipleCpnt insert (save it into the database), 0 otherwise
	* @param last_rows1 : 1 if it's your MultipleCpnt last insert (save it into the database), 0 otherwise  Maximum in one go: 10000 rows.( we force the insert)
	* @param ErrMess : error message in case of failure
	* @return 0 if it is successful
	*/

	EXTERN_CONFDB
		int InsertMultipleBoardCpnts(char* cpntname,char* cpnttype,int replacable,char* motherboardname,char* serial_nb,char* hwtype,char* responsible,char* comments,char* location,int first_time,int last_rows,char* ErrMess)
	{
		char appliName[100]="InsertMultipleBoardCpnts";
		char sqlstmt[2000];
		OCIStmt *hstmt;
		OCIBind  *bndp[11]; 
		int rescode=0;		
		int res_query=0;
		sword status=0;
		int i=0;

		char* dname=NULL;		
		char* dlocation=NULL;
		char* dmotherboard=NULL;
		char* dserialnb=NULL;
		char* dtype=NULL;
		char* dresponsible=NULL;
		char* dcomments=NULL;
		char* dhwtype=NULL;
		int* dreplacable=NULL;

		int* name_nullList=NULL;
		int* location_nullList=NULL;
		int* motherboard_nullList=NULL;
		int* serialnb_nullList=NULL;
		int* type_nullList=NULL;
		int* responsible_nullList=NULL;
		int* comments_nullList=NULL;
		int* hwtype_nullList=NULL;

		static int FIRST_TIME=0;

		int force_insert=0;
		char hist_seq[100]="LHCB_CPNT_HISTORY_COMPTID_SEQ.nextval";
		char hw_seq[100]="LHCB_HW_CPNTS_SNBID_SEQ.nextval";
		char lg_seq[100]="LHCB_LG_CPNTS_DEVICEID_SEQ.nextval";
		int* numrows_inserted=NULL;
		int numrows=0;
		int* snbidList=NULL;

		static int NbElement=0;
		Cpnt** CpntList;
		int max_namelen=0;
		int max_locationlen=0;
		int max_motherboardlen=0;
		int max_serialnblen=0;
		int max_typelen=0;
		int max_responsiblelen=0;
		int max_commentslen=0;
		int max_hwtypelen=0;
	
		int dname1=null_charptr(cpntname);
		int dserialnb1=null_charptr(serial_nb);
		int dtype1=null_charptr(cpnttype);
		int dlocation1=null_charptr(location);
		int dlocation2=null_charptr(motherboardname);
		int dresponsible1=null_charptr(responsible);
		int dhwtype1=null_charptr(hwtype);
		int dcomments1=null_charptr(comments);

		if(replacable==1)
		{
			if(serial_nb!=NULL)
			{
				if(strncmp(serial_nb,"none",4)==0||strlen(serial_nb)<2)
				{
					GetErrorMess(appliName, "If the cpnt is replacable,a serialnb is mandatory the",ErrMess,1);
					return -1;
				}
			}
			else
			{
				GetErrorMess(appliName, "If the cpnt is replacable,a serialnb is mandatory the",ErrMess,1);
				return -1;
			}

		}

		if (first_time==1)
		{
			FIRST_TIME=1;
			NbElement=1;
			CpntList=(Cpnt**)malloc( NbElement*sizeof(Cpnt));			
		}
		else 
		{
			if (FIRST_TIME==0&&force_insert==0)
			{
				GetErrorMess(appliName, "CACHE HAS NOT BEEN INITIALIZED",ErrMess,1);
				return -1;
			}
			NbElement++;
			CpntList=(Cpnt**)realloc(CpntList, NbElement*sizeof(Cpnt));
		}

		CpntList[NbElement-1]=new Cpnt(cpntname,cpnttype,replacable,motherboardname,serial_nb,hwtype,responsible,comments,location);

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

				if(dname!=NULL)	
					free(dname);
				if(dtype!=NULL)
					free(dtype);
				if(dlocation!=NULL)
					free(dlocation);
				if(dserialnb!=NULL)
					free(dserialnb);
				if(dresponsible!=NULL)
					free(dresponsible);
				if(dcomments!=NULL)
					free(dcomments);
				if(dhwtype!=NULL)
					free(dhwtype);
				if(dmotherboard!=NULL)
					free(dmotherboard);				
				if(numrows_inserted!=NULL)
					free(numrows_inserted);
				if(name_nullList!=NULL)
					free(name_nullList);
				if(serialnb_nullList!=NULL)
					free(serialnb_nullList);
				if(location_nullList!=NULL)
					free(location_nullList);
				if(type_nullList!=NULL)
					free(type_nullList);
				if(hwtype_nullList!=NULL)
					free(hwtype_nullList);
				if(comments_nullList!=NULL)
					free(comments_nullList);
				if(responsible_nullList!=NULL)
					free(responsible_nullList);
				if(motherboard_nullList!=NULL)
					free(motherboard_nullList);
				if(dreplacable!=NULL)
					free(dreplacable);
				if(snbidList!=NULL)
					free(snbidList);
				
				for(i=0;i<NbElement;i++)
					delete CpntList[i];
				if(CpntList!=NULL)
					free(CpntList);
				
				free(errmessg1);
				free(errmessg2);
				return -1;
			}

			for(i=0;i<NbElement;i++){
				if (CpntList[i]->namelen > max_namelen)
					max_namelen=CpntList[i]->namelen;
				if (CpntList[i]->serialnblen > max_serialnblen)
					max_serialnblen=CpntList[i]->serialnblen;
				if (CpntList[i]->locationlen > max_locationlen)
					max_locationlen=CpntList[i]->locationlen;
				if (CpntList[i]->typelen > max_typelen)
					max_typelen=CpntList[i]->typelen;				
				if (CpntList[i]->hwtypelen > max_hwtypelen)
					max_hwtypelen=CpntList[i]->hwtypelen;
				if (CpntList[i]->commentslen > max_commentslen)
					max_commentslen=CpntList[i]->commentslen;
				if (CpntList[i]->responsiblelen > max_responsiblelen)
					max_responsiblelen=CpntList[i]->responsiblelen;
				if (CpntList[i]->motherboardlen > max_motherboardlen)
					max_motherboardlen=CpntList[i]->motherboardlen;
			}

			dname=(char*)malloc( NbElement*max_namelen*sizeof(char));
			dserialnb=(char*)malloc(NbElement*max_serialnblen*sizeof(char));
			dlocation=(char*)malloc(NbElement*max_locationlen*sizeof(char));
			dtype=(char*)malloc(NbElement*max_typelen*sizeof(char));
			dhwtype=(char*)malloc(NbElement*max_hwtypelen*sizeof(char));
			dcomments=(char*)malloc(NbElement*max_commentslen*sizeof(char));
			dresponsible=(char*)malloc(NbElement*max_responsiblelen*sizeof(char));
			dmotherboard=(char*)malloc(NbElement*max_motherboardlen*sizeof(char));
			dreplacable=(int*)malloc(sizeof(int)*NbElement);
			snbidList=(int*)malloc(sizeof(int)*NbElement);
			
			name_nullList=(int*)malloc(NbElement*sizeof(int));
			serialnb_nullList=(int*)malloc(NbElement*sizeof(int));
			location_nullList=(int*)malloc(NbElement*sizeof(int));
			type_nullList=(int*)malloc(NbElement*sizeof(int));
			hwtype_nullList=(int*)malloc(NbElement*sizeof(int));
			comments_nullList=(int*)malloc(NbElement*sizeof(int));
			responsible_nullList=(int*)malloc(NbElement*sizeof(int));
			motherboard_nullList=(int*)malloc(NbElement*sizeof(int));

			if(name_nullList!=NULL && serialnb_nullList!=NULL && location_nullList!=NULL && type_nullList!=NULL && hwtype_nullList!=NULL && comments_nullList!=NULL && responsible_nullList!=NULL && motherboard_nullList!=NULL 
				&& dname!=NULL && dserialnb!=NULL && dlocation!=NULL && dtype!=NULL && dhwtype!=NULL && dcomments!=NULL && dresponsible!=NULL && dmotherboard!=NULL&&dreplacable!=NULL&&snbidList!=NULL)
			for(i=0;i<NbElement;i++)
			{
				memcpy(dname+i*max_namelen,CpntList[i]->name,CpntList[i]->namelen);
				name_nullList[i]=CpntList[i]->name_null;
				memcpy(dserialnb+i*max_serialnblen,CpntList[i]->serialnb,CpntList[i]->serialnblen);
				serialnb_nullList[i]=CpntList[i]->serialnb_null;
				memcpy(dlocation+i*max_locationlen,CpntList[i]->location,CpntList[i]->locationlen);
				location_nullList[i]=CpntList[i]->location_null;
				memcpy(dtype+i*max_typelen,CpntList[i]->type,CpntList[i]->typelen);
				type_nullList[i]=CpntList[i]->type_null;
				memcpy(dhwtype+i*max_hwtypelen,CpntList[i]->hwtype,CpntList[i]->hwtypelen);					
				hwtype_nullList[i]=CpntList[i]->hwtype_null;
				memcpy(dcomments+i*max_commentslen,CpntList[i]->comments,CpntList[i]->commentslen);					
				comments_nullList[i]=CpntList[i]->comments_null;
				memcpy(dresponsible+i*max_responsiblelen,CpntList[i]->responsible,CpntList[i]->responsiblelen);					
				responsible_nullList[i]=CpntList[i]->responsible_null;
				memcpy(dmotherboard+i*max_motherboardlen,CpntList[i]->motherboard,CpntList[i]->motherboardlen);
				motherboard_nullList[i]=CpntList[i]->motherboard_null;
				dreplacable[i]=CpntList[i]->replacable;
			}

			numrows_inserted=(int*)malloc(sizeof(int)*NbElement);

			if( (dtype==NULL) || (dname==NULL) || dmotherboard==NULL || dlocation==NULL || (dserialnb==NULL) || (dresponsible==NULL)||dcomments==NULL||dhwtype==NULL||numrows_inserted==NULL ||dreplacable==NULL || snbidList==NULL
				||name_nullList==NULL || serialnb_nullList==NULL || location_nullList==NULL || type_nullList==NULL || hwtype_nullList==NULL || comments_nullList==NULL || responsible_nullList==NULL || motherboard_nullList==NULL)

			{
				rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");
				GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
				
				FIRST_TIME=0;
				
				if(dname!=NULL)
					free(dname);
				if(dtype!=NULL)
					free(dtype);
				if(dlocation!=NULL)
					free(dlocation);
				if(dserialnb!=NULL)
					free(dserialnb);
				if(dmotherboard!=NULL)
					free(dmotherboard);
				if(dresponsible!=NULL)
					free(dresponsible);
				if(dcomments!=NULL)
					free(dcomments);
				if(dhwtype!=NULL)
					free(dhwtype);
				if(numrows_inserted!=NULL)
					free(numrows_inserted);
				if(dreplacable!=NULL)
					free(dreplacable);
				if(snbidList!=NULL)
					free(snbidList);
				if(name_nullList!=NULL)
					free(name_nullList);
				if(serialnb_nullList!=NULL)
					free(serialnb_nullList);
				if(location_nullList!=NULL)
					free(location_nullList);
				if(type_nullList!=NULL)
					free(type_nullList);
				if(hwtype_nullList!=NULL)
					free(hwtype_nullList);
				if(comments_nullList!=NULL)
					free(comments_nullList);
				if(responsible_nullList!=NULL)
					free(responsible_nullList);
				if(motherboard_nullList!=NULL)
					free(motherboard_nullList);
				
				for(i=0;i<NbElement;i++)
					delete CpntList[i];
				if(CpntList!=NULL)
                    free(CpntList);
				
				NbElement=0;
				
				return -1;
			}
			else
			{
				for(i=0;i<NbElement;i++)
				{
                    numrows_inserted[i]=0;
                    snbidList[i]=0;
				}
			}

			try{
                HandleAlloc(ociEnv,(dvoid**)&hstmt,OCI_HTYPE_STMT,&status);
				sprintf(sqlstmt,"BEGIN if nvl(:motherboard,'none') not like 'none' then insert ALL into %s(snbid,serialnb,hwtype,responsible,user_comment,cpnt_status,hwname,replacable,location) values(%s,:serialnb,:hwtype,:responsible,:comments,%d,:dname,:replacable,location) select serialnb as location from %s where devicename=:motherboard and serialnb is not null; insert all into %s(cpntname,cpnttype,snbid,cpntid,motherboardid,location,author,created,terminal_name) values (:dname,:cpnttype,snbid,%s,motherboardid,:location,'%s',sysdate,'%s') select t.snbid as snbid, e.deviceid as motherboardid from %s t,%s  e where t.serialnb=:serialnb and e.devicename=:motherboard;if %s=1 then insert all into %s(historycpntID,snbid,cpntid,cpnt_status,user_comment,location,status_change,created,author,terminal_name) values(%s,snbid,cpntid,'IN_USE',:comments,:motherboard,statuschange,sysdate,'%s','%s') select snbid,cpntid,created as statuschange from %s where  cpntname=:dname; end if; else insert into %s(snbid,serialnb,hwtype,responsible,user_comment,cpnt_status,hwname,replacable,location) values(%s,:serialnb,:hwtype,:responsible,:comments,%d,:dname,:replacable,:location) returning snbid into :snbid; if %s=1 then insert into %s(historycpntID,snbid,cpnt_status,user_comment,location,status_change,created,author,terminal_name) values(%s,:snbid,'SPARE',:comments,:location,sysdate,sysdate,'%s','%s'); end if; end if;:numrows:=%s; END;",HW_CPNTS_TABLE,hw_seq,IN_USE,LOGICAL_DEVICE_TABLE,LG_CPNTS_TABLE,lg_seq,login,host,HW_CPNTS_TABLE,LOGICAL_DEVICE_TABLE,SQLROWCOUNT,HISTORY_CPNT_TABLE,hist_seq,login,host,LG_CPNTS_TABLE,HW_CPNTS_TABLE,hw_seq,SPARE,SQLROWCOUNT,HISTORY_CPNT_TABLE,hist_seq,login,host,SQLROWCOUNT);
				StmtPrepare(hstmt,ociError, sqlstmt, &status);
				BindByName(hstmt,&bndp[0],ociError,":dname",dname, max_namelen,SQLT_STR,&name_nullList[0],&status);
				BindByName(hstmt,&bndp[1],ociError,":cpnttype",dtype,max_typelen,SQLT_STR,&type_nullList[0],&status);
				BindByName(hstmt,&bndp[2],ociError,":replacable",&dreplacable[0],sizeof(int),SQLT_INT,0,&status);				
				BindByName(hstmt,&bndp[3],ociError,":snbid",&snbidList[0],sizeof(int),SQLT_INT,0,&status);				
				BindByName(hstmt,&bndp[4],ociError,":motherboard",dmotherboard,max_motherboardlen,SQLT_STR,&motherboard_nullList[0],&status);
				BindByName(hstmt,&bndp[5],ociError,":serialnb",dserialnb,max_serialnblen,SQLT_STR,&serialnb_nullList[0],&status);
				BindByName(hstmt,&bndp[6],ociError,":location",dlocation,max_locationlen,SQLT_STR,&location_nullList[0],&status);
				BindByName(hstmt,&bndp[7],ociError,":hwtype",dhwtype,max_hwtypelen,SQLT_STR,&hwtype_nullList[0],&status);
				BindByName(hstmt,&bndp[8],ociError,":responsible",dresponsible,max_responsiblelen,SQLT_STR,&responsible_nullList[0],&status);
				BindByName(hstmt,&bndp[9],ociError,":comments",dcomments,max_commentslen,SQLT_STR,&comments_nullList[0],&status);
				BindByName(hstmt,&bndp[10],ociError,":numrows",&numrows_inserted[0],sizeof(int),SQLT_INT,0,&status);

				BindArrayOfStruct(bndp[0],ociError,max_namelen,sizeof(int),&status);
				BindArrayOfStruct(bndp[1],ociError,max_typelen,sizeof(int),&status);
				BindArrayOfStruct(bndp[2],ociError,sizeof(int),0,&status);
				BindArrayOfStruct(bndp[3],ociError,sizeof(int),0,&status);
				BindArrayOfStruct(bndp[4],ociError,max_motherboardlen,sizeof(int),&status);
				BindArrayOfStruct(bndp[5],ociError,max_serialnblen,sizeof(int),&status);
				BindArrayOfStruct(bndp[6],ociError,max_locationlen,sizeof(int),&status);
				BindArrayOfStruct(bndp[7],ociError,max_hwtypelen,sizeof(int),&status);
				BindArrayOfStruct(bndp[8],ociError,max_responsiblelen,sizeof(int),&status);
				BindArrayOfStruct(bndp[9],ociError,max_commentslen,sizeof(int),&status);
				BindArrayOfStruct(bndp[10],ociError,sizeof(int),0,&status);

				StmtExecute(ociHdbc, hstmt, ociError,NbElement, &status);

			}catch(Error err){
				sprintf(appliName,"InsertMultipleBoardCpnts");	///////
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

				if(dname!=NULL)
					free(dname);
				if(dtype!=NULL)
					free(dtype);
				if(dlocation!=NULL)
					free(dlocation);
				if(dserialnb!=NULL)
					free(dserialnb);
				if(dresponsible!=NULL)
					free(dresponsible);
				if(dcomments!=NULL)
					free(dcomments);
				if(dhwtype!=NULL)
					free(dhwtype);
				if(dmotherboard!=NULL)
					free(dmotherboard);
				if(numrows_inserted!=NULL)
					free(numrows_inserted);
				if(dreplacable!=NULL)
					free(dreplacable);
				if(snbidList!=NULL)
					free(snbidList);
				if(name_nullList!=NULL)
					free(name_nullList);
				if(serialnb_nullList!=NULL)
					free(serialnb_nullList);
				if(location_nullList!=NULL)
					free(location_nullList);
				if(type_nullList!=NULL)
					free(type_nullList);
				if(hwtype_nullList!=NULL)
					free(hwtype_nullList);
				if(comments_nullList!=NULL)
					free(comments_nullList);
				if(responsible_nullList!=NULL)
					free(responsible_nullList);
				if(motherboard_nullList!=NULL)
					free(motherboard_nullList);

				for(i=0;i<NbElement;i++)
					delete CpntList[i];
				if(force_insert==1)
				{
					FIRST_TIME=1;
					force_insert=0;
					CpntList=(Cpnt**)realloc(CpntList, 0*sizeof(Cpnt));
				}
				else if(CpntList!=NULL)
					free(CpntList);

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
				delete CpntList[i];
			if(force_insert==1)
			{
				FIRST_TIME=1;
				force_insert=0;
				CpntList=(Cpnt**)realloc(CpntList, 0*sizeof(Cpnt));
			}
			else if(CpntList!=NULL)
				free(CpntList);
			NbElement=0;
		}	
		else
		{		
			if(res_query!=0)
			{
				FIRST_TIME=0;
				GetErrorMess(appliName, "Cache Problem",ErrMess,1);
				for(i=0;i<NbElement;i++)
					delete CpntList[i];
				if(CpntList!=NULL)
                    free(CpntList);
			}
			if(ociEnv==0)
			{
				FIRST_TIME=0;
				res_query=-1;
				GetErrorMess(appliName, "NOT CONNECTED TO ANY DB",ErrMess,1);
				for(i=0;i<NbElement;i++)
					delete CpntList[i];
				if(CpntList!=NULL)
                    free(CpntList);
			}
		
			status+=res_query;
			if(ociEnv!=0 && FIRST_TIME==1 && res_query==0)
			{
				status=0;
				GetErrorMess(appliName, "NO_ERROR",ErrMess,0);
			}
		}
		if(dname!=NULL)
			free(dname);
		if(dtype!=NULL)
			free(dtype);
		if(dlocation!=NULL)
			free(dlocation);
		if(dserialnb!=NULL)
			free(dserialnb);
		if(dresponsible!=NULL)
			free(dresponsible);
		if(dcomments!=NULL)
			free(dcomments);
		if(dhwtype!=NULL)
			free(dhwtype);
		if(dmotherboard!=NULL)
			free(dmotherboard);
		if(numrows_inserted!=NULL)
			free(numrows_inserted);
		if(dreplacable!=NULL)
			free(dreplacable);
		if(snbidList!=NULL)
			free(snbidList);
		if(name_nullList!=NULL)
			free(name_nullList);
		if(serialnb_nullList!=NULL)
			free(serialnb_nullList);
		if(location_nullList!=NULL)
			free(location_nullList);
		if(type_nullList!=NULL)
			free(type_nullList);
		if(hwtype_nullList!=NULL)
			free(hwtype_nullList);
		if(comments_nullList!=NULL)
			free(comments_nullList);
		if(responsible_nullList!=NULL)
			free(responsible_nullList);
		if(motherboard_nullList!=NULL)
			free(motherboard_nullList);

		return (status+rescode);
	}


#if defined(__cplusplus) || defined(c_plusplus)
}
#endif