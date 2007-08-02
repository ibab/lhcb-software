/********************************************************************************/
//	Author: L.Abadie
// version:v3.5
// changes: handling error such that can be extended to C++: (create a class SQLException) (strlen ck + update stmt)
// file which performs bulk collect insert for the linktype table
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

	//extern char PRIME_NUMBER[20];
	extern   OCIError* ociError ;
	extern   OCIEnv* ociEnv ;
	extern   OCISvcCtx* ociHdbc ;
	extern   OCIServer *mysrvhp;
	extern   OCISession *myusrhp; 
	/************global variables for the link type*******************************/ 
	int FIRST_TIME_SLK=0;
	int FIRST_TIME_SLK1=0;
	char* _LinkTypeList;  //the list of the devicetype which will be stored
	int _linkTypeListLength; //the total length of the char*
	int _max_linkTypeLength; //the maximum length of the devicetype as a string
	int _linkTypeNbElement;

	int* _lktype_nullvalue;
	int  _lktypeNbEl;

	char* _LinkTypeList1;  //the list of the devicetype which will be stored
	int _linkTypeListLength1; //the total length of the char*
	int _max_linkTypeLength1; //the maximum length of the devicetype as a string
	int _linkTypeNbElement1;

	int* _lktype1_nullvalue;
	int  _lktype1NbEl;

	char* _simple_lkList;
	int _simple_lkListLength;
	int _simple_lkNbElement;
	int _max_simple_lkLength;

	int* _simplelktype_nullvalue;
	int  _simplelktypeNbEl;

	int freeLinkType()
	{
		int status=0;
		_LinkTypeList=(char*)realloc(_LinkTypeList,0*sizeof(char));
		_LinkTypeList=NULL;
		_lktype_nullvalue=(int*)realloc(_lktype_nullvalue,0*sizeof(int));
		_lktype_nullvalue=NULL;
		FIRST_TIME_SLK=0;
		return status;
	}

	int freeLinkType1()
	{
		int status=0;
		_LinkTypeList1=(char*)realloc(_LinkTypeList1,0*sizeof(char));
		_LinkTypeList1=NULL;

		_simple_lkList=(char*)realloc(_simple_lkList,0*sizeof(char));
		_simple_lkList=NULL;

		_simplelktype_nullvalue=(int*)realloc(_simple_lkList,0*sizeof(int));
		_simplelktype_nullvalue=NULL;

		_lktype1_nullvalue=(int*)realloc(_lktype_nullvalue,0*sizeof(int));
		_lktype1_nullvalue=NULL;
		FIRST_TIME_SLK1=0;
		return status;
	}
	/**
	* Insert a Simple Link Type,  returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* @param link_name : name of the link type
	* @param first_time1 : 1 if it's your SimpleLinkType first insert (save it into the database), 0 otherwise
	* @param last_rows1 : 1 if it's your SimpleLinkType last insert (save it into the database), 0 otherwise
	* @param ErrMess : error message in case of failure
	* @return 0 if it is successful
	*/ 


	EXTERN_CONFDB
		int InsertMultipleSimpleLinkTypes( char *link_name,int first_time1,int last_rows1,char* ErrMess)
	{
		char appliName[100]="InsertMultipleSimpleLinkTypes";
		int* numrows_inserted=NULL;
		char sqlstmt[1000];
		OCIStmt *hstmt;
		OCIBind  *bndp1 = (OCIBind *) 0; 
		int res_query=0;
		int i=0;
		int free_mem=0;
		char* ltype=NULL;
		int rescode=0;
		int force_insert=0;
		int first_time=first_time1;;
		int last_rows=last_rows1;
		char seqname[100]="lhcb_lktypeseq.nextval";
		int ltype1=null_charptr(link_name);
		sword status=0;
		int numrows=0;
		OCIBind  *bndp3 = (OCIBind *) 0;

		if(FIRST_TIME_SLK==1 && _LinkTypeList==NULL)
		{
			first_time=1;


		}
		if(first_time==1)
		{

			status+=freeLinkType();
			FIRST_TIME_SLK=1;
		}
		res_query=AppendString(link_name,_LinkTypeList,_linkTypeListLength,_linkTypeNbElement,_max_linkTypeLength,first_time);
		res_query+=AppendInt(ltype1,_lktype_nullvalue,_lktypeNbEl,first_time);




		if(last_rows!=1 && _linkTypeNbElement==MAXROWS)
		{
			last_rows=1;
			force_insert=1;
		}
		if(last_rows==1 && res_query==0 && ociEnv!=0 && FIRST_TIME_SLK==1)
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
				status+=freeLinkType();
				if(ltype!=NULL)
					free(ltype);
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
				sprintf(sqlstmt,"BEGIN insert ALL into %s(linktypeID,link_name,link_nbr,author,user_update,terminal_name) values (%s,:lkname,prime_nb,'%s','%s','%s') select prime_nb from %s  where prime_nb_position=(select nvl(count(link_nbr),0)+1 from %s); :numrows:=%s; end;",LINKTYPE_TABLE,seqname,login,login,host,PRIME_NUMBER_TABLE,LINKTYPE_TABLE,SQLROWCOUNT);
				status=OCIStmtPrepare(hstmt, ociError, (text*)sqlstmt, (ub4)strlen((char *)sqlstmt), (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
			}
			//std::cout<<"value of sqlstmt "<<sqlstmt <<std::endl;
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
			}
			else
			{
				ltype=(char*)malloc( _linkTypeNbElement*_max_linkTypeLength*sizeof(char));
				if(ltype!=NULL)
					status=NormalizeVector(_LinkTypeList, _linkTypeNbElement,_max_linkTypeLength,ltype);
				free_mem=1;
				numrows_inserted=(int*)malloc(sizeof(int)* _linkTypeNbElement);

				if(ltype==NULL ||numrows_inserted==NULL)
				{

					rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");
					status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
					rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");
					GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
					status+=freeLinkType();
					if(ltype!=NULL)
						free(ltype);
					if(numrows_inserted!=NULL)
						free(numrows_inserted);
					return -1;
				}
				else
				{
					for(i=0;i< _linkTypeNbElement;i++)
						numrows_inserted[i]=0;
					status =OCIBindByName(hstmt, &bndp1 , ociError,(text*) ":lkname", -1,(dvoid*)ltype, _max_linkTypeLength,  SQLT_STR, (dvoid *) &_lktype_nullvalue[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
				}

			}
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
				status=OCIBindArrayOfStruct(bndp1, ociError,_max_linkTypeLength, sizeof(int),0, 0);

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
				status= OCIStmtExecute(ociHdbc, hstmt, ociError, _linkTypeNbElement, 0, 0, 0, OCI_DEFAULT );

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
			}
			else
			{
				numrows=-1;
				for(i=0;i< _linkTypeNbElement;i++)
				{
					if(numrows_inserted[i]==0)
					{
						numrows=0;
						i=  _linkTypeNbElement+5;
					}
					else
						numrows=numrows_inserted[i];
				}
				if(numrows==0)
				{
					status = OCITransCommit(ociHdbc, ociError, 0);
					status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
					status+=freeLinkType();
					if(ltype!=NULL)
						free(ltype);
					free(numrows_inserted);
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

			}
			status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
			status+=freeLinkType();
			if(rescode!=0)
				if(ociError!=0)
					OCIReportError(ociError,appliName, ErrMess,1); 
				else
					GetErrorMess(appliName,  "NOT CONNECTED TO ANY DB",ErrMess,1); 
			else
				OCIReportError(ociError,appliName, ErrMess,0); 

			if(ltype!=NULL)
				free(ltype);
			if(numrows_inserted!=NULL)
				free(numrows_inserted);
			if(force_insert==1 && rescode==0)
			{
				FIRST_TIME_SLK=1;
				force_insert=0;
			}
		}
		else
		{
			if(res_query!=0)
			{
				status=freeLinkType();
				GetErrorMess(appliName, "Cache Problem",ErrMess,1);
			}
			if(FIRST_TIME_SLK!=1)
			{
				status=freeLinkType();
				res_query=-1;
				GetErrorMess(appliName, "CACHE HAS NOT BEEN INITIALIZED",ErrMess,1);
			}
			if(ociEnv==0)
			{
				status=freeLinkType();
				res_query=-1;
				GetErrorMess(appliName, "NOT CONNECTED TO ANY DB",ErrMess,1);
			}
			if(ociEnv!=0 && FIRST_TIME_SLK==1 && res_query==0)
			{
				status=0;
				GetErrorMess(appliName, "NO_ERROR",ErrMess,0);
			}
			status+=res_query;


		}
		return (rescode+status);
	}
	/*****************************************************************/
	/**
	* Insert a Composite Link Type,  returning an integer value. Not to used
	* The user should manage the memory : there is no memory allocation.
	* @param link_name : name of the link type (such as "Mixed_Data")		 
	* @param simple_lk_list : name of the link types which compose the composite link, separated by "," 
	* @param simple_lk_list :ex. Mixed_Data is composed of HLT_Data and L1_Data, so simple_lk_list="HLT_Data,L1_Data"
	* @param first_time1 : 1 if it's your CompositeLinkType first insert (save it into the database), 0 otherwise
	* @param last_rows1 : 1 if it's your CompositeLinkType last insert (save it into the database), 0 otherwise
	* @param ErrMess : error message in case of failure
	* @return 0 if it is successful
	*/ 
	EXTERN_CONFDB
		int InsertMultipleCompositeLinkTypes( char* link_name,char* simple_lk_list,int first_time1,int last_rows1,char* ErrMess)
	{
		char appliName[100]="InsertMultipleCompositeLinkTypes";
		int* numrows_inserted=NULL;
		int numrows=0;
		char sqlstmt[1000];
		OCIStmt *hstmt;
		OCIBind  *bndp[2]; 
		int res_query=0;
		int rescode=0;
		int free_mem=0;
		sword status=0;
		int force_insert=0;
		int first_time=first_time1;
		int last_rows=last_rows1;
		char* ltype=NULL;
		int i=0;
		char* lklist=NULL;
		OCIBind  *bndp3 = (OCIBind *) 0;
		char seqname[100]="lhcb_lktypeseq.nextval";
		int lktype1=null_charptr(link_name);
		int lklist1=null_charptr(simple_lk_list);

		if(FIRST_TIME_SLK1==1 && _LinkTypeList1==NULL)
		{
			first_time=1;

		}
		if(first_time==1)
		{
			status+=freeLinkType1();
			FIRST_TIME_SLK1=1;

		}

		res_query=AppendString(link_name,_LinkTypeList1,_linkTypeListLength1,_linkTypeNbElement1,_max_linkTypeLength1,first_time);
		res_query+=AppendString(simple_lk_list,_simple_lkList,_simple_lkListLength,_simple_lkNbElement,_max_simple_lkLength,first_time);

		res_query+=AppendInt(lktype1,_lktype1_nullvalue,_lktype1NbEl,first_time);
		res_query+=AppendInt(lklist1,_simplelktype_nullvalue,_simplelktypeNbEl,first_time);

		if(last_rows!=1 && _linkTypeNbElement1==MAXROWS)
		{
			last_rows=1;
			force_insert=1;

		}
		if(last_rows==1 && res_query==0 && ociEnv!=0 && FIRST_TIME_SLK1==1)
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
				status+=freeLinkType1();
				if(ltype!=NULL)
					free(ltype);
				if(lklist!=NULL)
					free(lklist);
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
				sprintf(sqlstmt,"BEGIN insert ALL into %s (linktypeID,link_name,link_nbr,author,user_update,terminal_name) values (%s,:lkname,lknb,'%s','%s','%s') select case when ceil(power(10,sum(log(10,link_nbr))))-power(10,sum(log(10,link_nbr)))>0.5  then floor(power(10,sum(log(10,link_nbr)))) else ceil(power(10,sum(log(10,link_nbr)))) end as lknb from %s where instr(','||:lklist||',',','||link_name||',')>0; :numrows:=%s; END;",LINKTYPE_TABLE,seqname,login,login,host,LINKTYPE_TABLE,SQLROWCOUNT);
				status=OCIStmtPrepare(hstmt, ociError, (text*)sqlstmt, (ub4)strlen((char *)sqlstmt), (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);

			}	
			if (status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
			}
			else
			{
				ltype=(char*)malloc( _linkTypeNbElement1*_max_linkTypeLength1*sizeof(char));
				if(ltype!=NULL)
					status=NormalizeVector(_LinkTypeList1, _linkTypeNbElement1,_max_linkTypeLength1,ltype);

				lklist=(char*)malloc( _simple_lkNbElement*_max_simple_lkLength*sizeof(char));
				if(lklist!=NULL)
					status+=NormalizeVector(_simple_lkList,_simple_lkNbElement,_max_simple_lkLength,lklist);	
				free_mem=1;
				numrows_inserted=(int*)malloc(sizeof(int)* _linkTypeNbElement1);
				for(i=0;i< _linkTypeNbElement1;i++)
					numrows_inserted[i]=0;
				if(ltype==NULL || lklist==NULL||numrows_inserted==NULL)
				{
					status=-10;
					rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");

					status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
					rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");
					GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
					status+=freeLinkType1();
					if(ltype!=NULL)
						free(ltype);
					if(lklist!=NULL)
						free(lklist);
					if(numrows_inserted!=NULL)
						free(numrows_inserted);
					return -1;
				}
				else
					status =OCIBindByName(hstmt, &bndp[0] , ociError,(text*) ":lkname", -1,(dvoid*)ltype, _max_linkTypeLength1,  SQLT_STR, (dvoid *) &_lktype1_nullvalue[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
			}
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp[1] , ociError,(text*) ":lklist", -1,(dvoid*)lklist, _max_simple_lkLength,  SQLT_STR, (dvoid *) &_simplelktype_nullvalue[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);

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
				status=OCIBindArrayOfStruct(bndp[0], ociError,_max_linkTypeLength1, sizeof(int),0, 0);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}     
			else
				status=OCIBindArrayOfStruct(bndp[1], ociError,_max_simple_lkLength, sizeof(int),0, 0);

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
				status= OCIStmtExecute(ociHdbc, hstmt, ociError,_linkTypeNbElement1, 0, 0, 0, OCI_DEFAULT );
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
			}		
			else
			{
				numrows=-1;
				for(i=0;i< _linkTypeNbElement1;i++)
				{
					if(numrows_inserted[i]==0)
					{
						numrows=0;
						i=  _linkTypeNbElement1+5;
					}
					else
						numrows=numrows_inserted[i];
				}
				if(numrows==0)
				{
					status = OCITransCommit(ociHdbc, ociError, 0);
					status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
					status+=freeLinkType1();
					if(ltype!=NULL)
						free(ltype);
					if(lklist!=NULL)
						free(lklist);
					free(numrows_inserted);
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


			}
			status+=OCIHandleFree (hstmt,OCI_HTYPE_STMT);
			if(rescode!=0)
				if(ociError!=0)
					OCIReportError(ociError,appliName, ErrMess,1); 
				else
					GetErrorMess(appliName,  "NOT CONNECTED TO ANY DB",ErrMess,1); 
			else
				OCIReportError(ociError,appliName, ErrMess,0); 

			status+=freeLinkType1();
			if(ltype!=NULL)
				free(ltype);
			if(lklist!=NULL)
				free(lklist);
			if(numrows_inserted!=NULL)
				free(numrows_inserted);
			if(force_insert==1  && rescode==0)
			{
				FIRST_TIME_SLK1=1;
				force_insert=0;
			}

		}
		else
		{
			if(res_query!=0)
			{
				status=freeLinkType1();
				GetErrorMess(appliName, "Cache Problem",ErrMess,1);
			}
			if(FIRST_TIME_SLK1!=1)
			{
				status=freeLinkType1();
				res_query=-1;
				GetErrorMess(appliName, "CACHE HAS NOT BEEN INITIALIZED",ErrMess,1);
			}
			if(ociEnv==0)
			{
				status=freeLinkType1();
				res_query=-1;
				GetErrorMess(appliName, "NOT CONNECTED TO ANY DB",ErrMess,1);
			}
			status+=res_query;
			if(ociEnv!=0 && FIRST_TIME_SLK1==1 && res_query==0)
			{
				status=0;
				GetErrorMess(appliName, "NO_ERROR",ErrMess,0);
			}
		}
		return (rescode+status);
	}


#if defined(__cplusplus) || defined(c_plusplus)
}
#endif