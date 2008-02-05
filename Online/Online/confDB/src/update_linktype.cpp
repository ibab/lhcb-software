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


	extern   OCIError* ociError ;
	extern   OCIEnv* ociEnv ;
	extern   OCISvcCtx* ociHdbc ;
	extern   OCIServer *mysrvhp;
	extern   OCISession *myusrhp; 
	/************global variables for the link type*******************************/ 
	int FIRST_TIME_SLKUPDATE=0;
	int FIRST_TIME_SLK1UPDATE=0;
	char* _LinkTypeListUpdate;  //the list of the devicetype which will be stored
	int _linkTypeListLengthUpdate; //the total length of the char*
	int _max_linkTypeLengthUpdate; //the maximum length of the devicetype as a string
	int _linkTypeNbElementUpdate;

	char* _oldLinkTypeListUpdate;  //the list of the devicetype which will be stored
	int _oldlinkTypeListLengthUpdate; //the total length of the char*
	int _max_oldlinkTypeLengthUpdate; //the maximum length of the devicetype as a string
	int _oldlinkTypeNbElementUpdate;

	int* _lktype_nullvalueUpdate;
	int  _lktypeNbElUpdate;

	int* _oldlktype_nullvalueUpdate;
	int  _oldlktypeNbElUpdate;

	char* _LinkTypeList1Update;  //the list of the devicetype which will be stored
	int _linkTypeListLength1Update; //the total length of the char*
	int _max_linkTypeLength1Update; //the maximum length of the devicetype as a string
	int _linkTypeNbElement1Update;

	char* _oldLinkTypeList1Update;  //the list of the devicetype which will be stored
	int _oldlinkTypeListLength1Update; //the total length of the char*
	int _oldmax_linkTypeLength1Update; //the maximum length of the devicetype as a string
	int _oldlinkTypeNbElement1Update;

	int* _lktype1_nullvalueUpdate;
	int  _lktype1NbElUpdate;

	int* _oldlktype1_nullvalueUpdate;
	int  _oldlktype1NbElUpdate;

	char* _simple_lkListUpdate;
	int _simple_lkListLengthUpdate;
	int _simple_lkNbElementUpdate;
	int _max_simple_lkLengthUpdate;

	int* _simplelktype_nullvalueUpdate;
	int  _simplelktypeNbElUpdate;

	int freeLinkTypeUpdate()
	{
		int status=0;
		_LinkTypeListUpdate=(char*)realloc(_LinkTypeListUpdate,0*sizeof(char));
		_LinkTypeListUpdate=NULL;
		_lktype_nullvalueUpdate=(int*)realloc(_lktype_nullvalueUpdate,0*sizeof(int));
		_lktype_nullvalueUpdate=NULL;
		_oldLinkTypeListUpdate=(char*)realloc(_oldLinkTypeListUpdate,0*sizeof(char));
		_oldLinkTypeListUpdate=NULL;
		_oldlktype_nullvalueUpdate=(int*)realloc(_oldlktype_nullvalueUpdate,0*sizeof(int));
		_oldlktype_nullvalueUpdate=NULL;
		FIRST_TIME_SLKUPDATE=0;
		return status;
	}

	int freeLinkType1Update()
	{
		int status=0;
		_LinkTypeList1Update=(char*)realloc(_LinkTypeList1Update,0*sizeof(char));
		_LinkTypeList1Update=NULL;
		_oldLinkTypeList1Update=(char*)realloc(_oldLinkTypeList1Update,0*sizeof(char));
		_oldLinkTypeList1Update=NULL;


		_simple_lkListUpdate=(char*)realloc(_simple_lkListUpdate,0*sizeof(char));
		_simple_lkListUpdate=NULL;

		_simplelktype_nullvalueUpdate=(int*)realloc(_simple_lkListUpdate,0*sizeof(int));
		_simplelktype_nullvalueUpdate=NULL;

		_lktype1_nullvalueUpdate=(int*)realloc(_lktype_nullvalueUpdate,0*sizeof(int));
		_lktype1_nullvalueUpdate=NULL;
		_oldlktype1_nullvalueUpdate=(int*)realloc(_oldlktype_nullvalueUpdate,0*sizeof(int));
		_oldlktype1_nullvalueUpdate=NULL;
		FIRST_TIME_SLK1UPDATE=0;
		return status;
	}
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
		int UpdateMultipleLinkTypeNames(char *old_link_name, char *link_name,int first_time1,int last_rows1,char* ErrMess)
	{
		char appliName[100]="UpdateMultipleLinkTypenames";
		int* numrows_inserted=NULL;
		char sqlstmt[1000];
		OCIStmt *hstmt;
		OCIBind  *bndp1 = (OCIBind *) 0; 
		int res_query=0;
		int i=0;
		int free_mem=0;
		char* ltype=NULL;
		char* ltype_old=NULL;
		int force_insert=0;
		int first_time=first_time1;
		int last_rows=last_rows1;
		int rescode=0;
		int ltype1=null_charptr(link_name);
		int ltypeold=null_charptr(old_link_name);

		sword status=0;
		int numrows=0;
		OCIBind  *bndp3 = (OCIBind *) 0;
		OCIBind  *bndp2 = (OCIBind *) 0;

		if(FIRST_TIME_SLKUPDATE==1 && _oldLinkTypeListUpdate==NULL)
			first_time=1;
		res_query=AppendString(old_link_name,_oldLinkTypeListUpdate,_oldlinkTypeListLengthUpdate,_oldlinkTypeNbElementUpdate,_max_oldlinkTypeLengthUpdate,first_time);

		res_query=AppendString(link_name,_LinkTypeListUpdate,_linkTypeListLengthUpdate,_linkTypeNbElementUpdate,_max_linkTypeLengthUpdate,first_time);
		res_query+=AppendInt(ltype1,_lktype_nullvalueUpdate,_lktypeNbElUpdate,first_time);

		res_query+=AppendInt(ltypeold,_oldlktype_nullvalueUpdate,_oldlktypeNbElUpdate,first_time);


		if(first_time==1)
			FIRST_TIME_SLKUPDATE=1;

		if(last_rows!=1 && _oldlinkTypeNbElementUpdate==MAXROWS)
		{
			force_insert=1;
			last_rows=1;
		}
		if(last_rows==1 && res_query==0 && ociEnv!=0 && FIRST_TIME_SLKUPDATE==1)
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
				status+=freeLinkTypeUpdate();
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
				sprintf(sqlstmt,"BEGIN update %s set link_name=:lkname,last_update=sysdate,user_update='%s' where link_name=:oldlkname; :numrows:=%s; end;",LINKTYPE_TABLE,login,SQLROWCOUNT);
				//std:://cout<<"sqlstmt "<< sqlstmt<<std::endl;

				status=OCIStmtPrepare(hstmt, ociError, (text*)sqlstmt, (ub4)strlen((char *)sqlstmt), (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
			}

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
			}
			else
			{
				ltype=(char*)malloc( _linkTypeNbElementUpdate*_max_linkTypeLengthUpdate*sizeof(char));
				if(ltype!=NULL)
					status=NormalizeVector(_LinkTypeListUpdate, _linkTypeNbElementUpdate,_max_linkTypeLengthUpdate,ltype);
				ltype_old=(char*)malloc( _oldlinkTypeNbElementUpdate*_max_oldlinkTypeLengthUpdate*sizeof(char));
				if(ltype_old!=NULL)
					status=NormalizeVector(_oldLinkTypeListUpdate, _oldlinkTypeNbElementUpdate,_max_oldlinkTypeLengthUpdate,ltype_old);

				numrows_inserted=(int*)malloc(sizeof(int)* _oldlinkTypeNbElementUpdate);

				if(ltype==NULL ||numrows_inserted==NULL ||ltype_old==NULL)
				{

					rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");
					status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
					rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");
					GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
					status+=freeLinkTypeUpdate();
					if(ltype!=NULL)
						free(ltype);
					if(ltype_old!=NULL)
						free(ltype_old);
					if(numrows_inserted!=NULL)
						free(numrows_inserted);
					return -1;
				}
				else
				{
					for(i=0;i< _oldlinkTypeNbElementUpdate;i++)
						numrows_inserted[i]=0;
					status =OCIBindByName(hstmt, &bndp1 , ociError,(text*) ":lkname", -1,(dvoid*)ltype, _max_linkTypeLengthUpdate,  SQLT_STR, (dvoid *) &_lktype_nullvalueUpdate[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
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
				status =OCIBindByName(hstmt, &bndp2, ociError,(text*) ":oldlkname", -1,(dvoid*)ltype_old, _max_oldlinkTypeLengthUpdate,  SQLT_STR, (dvoid *) &_oldlktype_nullvalueUpdate[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp1, ociError,_max_linkTypeLengthUpdate, sizeof(int),0, 0);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp2, ociError,_max_oldlinkTypeLengthUpdate, sizeof(int),0, 0);


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
				status= OCIStmtExecute(ociHdbc, hstmt, ociError, _oldlinkTypeNbElementUpdate, 0, 0, 0, OCI_DEFAULT );

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
			}
			else
			{
				numrows=-1;
				//std:://cout<<"ltype_old  "<< ltype_old<<" ltype "<<ltype<< std::endl;
				//std:://cout<<"numrows_inserted[0] "<< numrows_inserted[0]<<"_oldlinkTypeNbElementUpdate"<<_oldlinkTypeNbElementUpdate<<std::endl;
				for(i=0;i< _oldlinkTypeNbElementUpdate;i++)
				{
					if(numrows_inserted[i]==0)
					{
						numrows=0;
						i=  _oldlinkTypeNbElementUpdate+5;
					}
					else
						numrows=numrows_inserted[i];
				}
				if(numrows==0)
				{
					status = OCITransCommit(ociHdbc, ociError, 0);
					status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
					status+=freeLinkTypeUpdate();
					if(ltype!=NULL)
						free(ltype);
					if(ltype_old!=NULL)
						free(ltype_old);
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
			status+=freeLinkTypeUpdate();
			if(rescode!=0)
				if(ociError!=0)
					OCIReportError(ociError,appliName, ErrMess,1); 
				else
					GetErrorMess(appliName,  "NOT CONNECTED TO ANY DB",ErrMess,1); 
			else
				OCIReportError(ociError,appliName, ErrMess,0); 

			if(ltype!=NULL)
				free(ltype);
			if(ltype_old!=NULL)
				free(ltype_old);
			if(numrows_inserted!=NULL)
				free(numrows_inserted);

			if(force_insert==1 && rescode==0)
				FIRST_TIME_SLKUPDATE=1;
		}
		else
		{
			if(res_query!=0)
			{
				status=freeLinkTypeUpdate();
				GetErrorMess(appliName, "Cache Problem",ErrMess,1);
			}
			if(FIRST_TIME_SLKUPDATE!=1)
			{
				status=freeLinkTypeUpdate();
				res_query=-1;
				GetErrorMess(appliName, "CACHE HAS NOT BEEN INITIALIZED",ErrMess,1);
			}
			if(ociEnv==0)
			{
				status=freeLinkTypeUpdate();
				res_query=-1;
				GetErrorMess(appliName, "NOT CONNECTED TO ANY DB",ErrMess,1);
			}
			if(ociEnv!=0 && FIRST_TIME_SLKUPDATE==1 && res_query==0)
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
	* Update a Composite Link Type,  returning an integer value.Not to used...
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
		int UpdateMultipleCompositeLinkTypes(char* link_name,char * simple_lk_list,int first_time1,int last_rows1,char* ErrMess)
	{
		char appliName[100]="UpdateMultipleCompositeLinkTypes";
		int* numrows_inserted=NULL;
		int numrows=0;
		char sqlstmt[1000];
		OCIStmt *hstmt;
		OCIBind  *bndp[2]; 
		int res_query=0;
		int rescode=0;
		int free_mem=0;
		sword status=0;
		char* ltype=NULL;
		int i=0;
		char* lklist=NULL;
		OCIBind  *bndp3 = (OCIBind *) 0;
		int lktype1=null_charptr(link_name);
		int force_insert=0;
		int first_time=first_time1;
		int last_rows=last_rows1;
		int lklist1=null_charptr(simple_lk_list);
		char seqname[100]="lhcb_lktypeseq.nextval";
		if(FIRST_TIME_SLK1UPDATE==1 && _LinkTypeList1Update==NULL)
			first_time=1;

		res_query=AppendString(link_name,_LinkTypeList1Update,_linkTypeListLength1Update,_linkTypeNbElement1Update,_max_linkTypeLength1Update,first_time);
		res_query+=AppendString(simple_lk_list,_simple_lkListUpdate,_simple_lkListLengthUpdate,_simple_lkNbElementUpdate,_max_simple_lkLengthUpdate,first_time);

		res_query+=AppendInt(lktype1,_lktype1_nullvalueUpdate,_lktype1NbElUpdate,first_time);
		res_query+=AppendInt(lklist1,_simplelktype_nullvalueUpdate,_simplelktypeNbElUpdate,first_time);
		if(first_time==1)
			FIRST_TIME_SLK1UPDATE=1;					 


		if(last_rows!=1 && _linkTypeNbElement1Update==MAXROWS)
		{
			last_rows=1;
			force_insert=1;
		}

		if(last_rows==1 && res_query==0 && ociEnv!=0 && FIRST_TIME_SLK1UPDATE==1)
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
				status+=freeLinkType1Update();
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
				sprintf(sqlstmt,"BEGIN if :lklist='none' then update %s set last_update=sysdate, user_update='%s', link_nbr=(select p.prime_nb from %s  p where p.prime_nb_position=(select nvl(count(t.link_nbr),0)+1 from %s t,%s g where t.link_nbr=g.prime_nb )) where link_name=:lkname; else update %s set last_update=sysdate,user_update='%s',link_nbr=(select case when ceil(power(10,sum(log(10,link_nbr))))-power(10,sum(log(10,link_nbr)))>0.5  then floor(power(10,sum(log(10,link_nbr)))) else ceil(power(10,sum(log(10,link_nbr)))) end from %s t where instr(:lklist,t.link_name)>0) where link_name=:lkname; end if; :numrows:=%s; END;",LINKTYPE_TABLE,login,PRIME_NUMBER_TABLE,LINKTYPE_TABLE,PRIME_NUMBER_TABLE,LINKTYPE_TABLE,login,LINKTYPE_TABLE,SQLROWCOUNT);
				status=OCIStmtPrepare(hstmt, ociError, (text*)sqlstmt, (ub4)strlen((char *)sqlstmt), (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);

			}	
			if (status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
			}
			else
			{
				ltype=(char*)malloc( _linkTypeNbElement1Update*_max_linkTypeLength1Update*sizeof(char));
				if(ltype!=NULL)
					status=NormalizeVector(_LinkTypeList1Update, _linkTypeNbElement1Update,_max_linkTypeLength1Update,ltype);

				lklist=(char*)malloc( _simple_lkNbElementUpdate*_max_simple_lkLengthUpdate*sizeof(char));
				if(lklist!=NULL)
					status+=NormalizeVector(_simple_lkListUpdate,_simple_lkNbElementUpdate,_max_simple_lkLengthUpdate,lklist);	
				free_mem=1;
				numrows_inserted=(int*)malloc(sizeof(int)* _linkTypeNbElement1Update);
				for(i=0;i< _linkTypeNbElement1Update;i++)
					numrows_inserted[i]=0;
				if(ltype==NULL || lklist==NULL||numrows_inserted==NULL)
				{
					status=-10;
					rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");

					status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
					rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");
					GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
					status+=freeLinkType1Update();
					if(ltype!=NULL)
						free(ltype);
					if(lklist!=NULL)
						free(lklist);
					if(numrows_inserted!=NULL)
						free(numrows_inserted);
					return -1;
				}
				else
					status =OCIBindByName(hstmt, &bndp[0] , ociError,(text*) ":lkname", -1,(dvoid*)ltype, _max_linkTypeLength1Update,  SQLT_STR, (dvoid *) &_lktype1_nullvalueUpdate[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
			}
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp[1] , ociError,(text*) ":lklist", -1,(dvoid*)lklist, _max_simple_lkLengthUpdate,  SQLT_STR, (dvoid *) &_simplelktype_nullvalueUpdate[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);

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
				status=OCIBindArrayOfStruct(bndp[0], ociError,_max_linkTypeLength1Update, sizeof(int),0, 0);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}     
			else
				status=OCIBindArrayOfStruct(bndp[1], ociError,_max_simple_lkLengthUpdate, sizeof(int),0, 0);

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
				status= OCIStmtExecute(ociHdbc, hstmt, ociError,_linkTypeNbElement1Update, 0, 0, 0, OCI_DEFAULT );
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
			}		
			else
			{
				numrows=-1;
				for(i=0;i< _linkTypeNbElement1Update;i++)
				{
					if(numrows_inserted[i]==0)
					{
						numrows=0;
						i=  _linkTypeNbElement1Update+5;
					}
					else
						numrows=numrows_inserted[i];
				}
				if(numrows==0)
				{
					status = OCITransCommit(ociHdbc, ociError, 0);
					status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
					status+=freeLinkType1Update();
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

			status+=freeLinkType1Update();
			if(ltype!=NULL)
				free(ltype);
			if(lklist!=NULL)
				free(lklist);
			if(numrows_inserted!=NULL)
				free(numrows_inserted);

			if(force_insert==1 && rescode==0)
				FIRST_TIME_SLK1UPDATE=1;

		}
		else
		{
			if(res_query!=0)
			{
				status=freeLinkType1Update();
				GetErrorMess(appliName, "Cache Problem",ErrMess,1);
			}
			if(FIRST_TIME_SLK1UPDATE!=1)
			{
				status=freeLinkType1Update();
				res_query=-1;
				GetErrorMess(appliName, "CAHCE HAS NOT BEEN INITIALIZED",ErrMess,1);
			}
			if(ociEnv==0)
			{
				status=freeLinkType1Update();
				res_query=-1;
				GetErrorMess(appliName, "NOT CONNECTED TO ANY DB",ErrMess,1);
			}
			if(ociEnv!=0 && FIRST_TIME_SLK1UPDATE==1 && res_query==0)
			{
				status=0;
				GetErrorMess(appliName, "NO_ERROR",ErrMess,0);
			}
			status+=res_query;
		}
		return (rescode+status);
	}


#if defined(__cplusplus) || defined(c_plusplus)
}
#endif