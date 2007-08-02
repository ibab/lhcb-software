/********************************************************************************/
//	Author: L.Abadie
// version:v3.5
// changes: handling error such that can be extended to C++: (create a class SQLException) (strlen ck + update stmt)
// file which performs bulk collect insert for the connectivity table
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
	/************************global variables for the connectivity***********************/
	int FIRST_TIME_MICROCONNECTIVITY=0;
	char* _cpntNodeFromList;
	int _max_cpntNodeFromList_len;
	int _cpntNodeFromList_len;
	int _cpntNodeFromNbElement;

	int* _cpntNodeFrom_nullvalue;
	int _cpntNodeFromNbEl;

	int* _cpntportnbfromList;
	int _cpntportnbfromNbElement;


	char* _cpntNodeToList;
	int _max_cpntNodeToList_len;
	int _cpntNodeToList_len;
	int _cpntNodeToNbElement;

	int* _cpntNodeTo_nullvalue;
	int _cpntNodeToNbEl;

	int* _cpntportnbtoList;
	int _cpntportnbtoNbElement;








	char* _cpntLinkTypeList2;
	int _max_cpntLinkTypeList_len2;
	int _cpntLinkTypeList_len2;
	int _cpntLinkTypeNbElement2;

	int* _cpntLinkType_nullvalue;
	int _cpntLinkTypeNbEl;



	int* _cpntBidirectionalList;
	int _cpntBidirectionalNbElement;


	/******************************************************/
	int freeMicroConnectivity()
	{
		int status=0;
		_cpntLinkTypeList2=(char*)realloc(_cpntLinkTypeList2,0*sizeof(char));
		_cpntLinkTypeList2=NULL;

		_cpntNodeToList=(char*)realloc(_cpntNodeToList,0*sizeof(char));
		_cpntNodeToList=NULL;

		_cpntportnbtoList=(int*)realloc(_cpntportnbtoList,0*sizeof(int));
		_cpntportnbtoList=NULL;


		_cpntNodeFromList=(char*)realloc(_cpntNodeFromList,0*sizeof(char));
		_cpntNodeFromList=NULL;

		_cpntportnbfromList=(int*)realloc(_cpntportnbfromList,0*sizeof(int));
		_cpntportnbfromList=NULL;


		_cpntBidirectionalList=(int*)realloc(_cpntBidirectionalList,0*sizeof(int));
		_cpntBidirectionalList=NULL;



		_cpntNodeTo_nullvalue=(int*)realloc(_cpntNodeTo_nullvalue,0*sizeof(int));
		_cpntNodeTo_nullvalue=NULL;

		_cpntNodeFrom_nullvalue=(int*)realloc(_cpntNodeFrom_nullvalue,0*sizeof(int));
		_cpntNodeFrom_nullvalue=NULL;


		_cpntLinkType_nullvalue=(int*)realloc(_cpntLinkType_nullvalue,0*sizeof(int));
		_cpntLinkType_nullvalue=NULL;
		FIRST_TIME_MICROCONNECTIVITY=0;


		return status;
	}

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
		int InsertMultipleMicroLinks(char* node_from,char* node_to,int port_nbfrom,int port_nbto,char* link_type,int bidirectional_link_used,int first_time1, int last_rows1,char* ErrMess)
	{
		int numrows=0;
		int num_errs1=0;
		char sqlstmt[2000];
		int i=0;
		char appliName[100]="InsertMultipleMicroLinks";
		OCIStmt *hstmt;
		OCIBind  *bndp2[8];
		OCIBind  *bndp3 = (OCIBind *) 0; 
		int res_query=0;
		sword status=0;
		char* nodefrom=NULL;
		char* nodeto=NULL;
		char* portnbfrom=NULL;
		char* portypefrom=NULL;
		char* portnbto=NULL;
		char* portypeto=NULL;

		char* linktype=NULL;
		int rescode=0;
		char res_query1[100]="lll";
		char seqname[100]="lhcb_microconnseq.nextval";
		int last_rows=last_rows1;

		OCIError* ociError1=0 ;
		int* numrows_inserted=NULL;

		int nodefrom_null=null_charptr(node_from);
		int nodeto_null=null_charptr(node_to);

		int linktype_null=null_charptr(link_type);
		int force_insert=0;
		int first_time=first_time1;

		//std::cout<<"value of port_typeto1 "<<port_typeto1<<"value of port_typeto"<<port_typeto <<std::endl;
		if(FIRST_TIME_MICROCONNECTIVITY==1 && _cpntNodeFromList==NULL) //means new loop in the mass insertion (by 10000)
		{
			first_time=1; //need to put to 1, as if new cache

		}
		if(first_time==1)
		{

			status+=freeMicroConnectivity();
			FIRST_TIME_MICROCONNECTIVITY=1;

		}
		res_query=AppendString(node_from,_cpntNodeFromList, _cpntNodeFromList_len,_cpntNodeFromNbElement,_max_cpntNodeFromList_len,first_time);
		res_query+=AppendString(node_to,_cpntNodeToList,_cpntNodeToList_len,_cpntNodeToNbElement,_max_cpntNodeToList_len,first_time);
		res_query+=AppendString(link_type,_cpntLinkTypeList2,_cpntLinkTypeList_len2,_cpntLinkTypeNbElement2,_max_cpntLinkTypeList_len2,first_time);

		res_query=AppendInt(port_nbfrom,_cpntportnbfromList, _cpntportnbfromNbElement,first_time);
		res_query+=AppendInt(port_nbto,_cpntportnbtoList,_cpntportnbtoNbElement,first_time);

		res_query+=AppendInt(bidirectional_link_used,_cpntBidirectionalList,_cpntBidirectionalNbElement,first_time);


		res_query+=AppendInt(nodeto_null,_cpntNodeTo_nullvalue,_cpntNodeToNbEl,first_time);
		res_query+=AppendInt(linktype_null,_cpntLinkType_nullvalue,_cpntLinkTypeNbEl,first_time);
		res_query+=AppendInt(nodefrom_null,_cpntNodeFrom_nullvalue,_cpntNodeFromNbEl,first_time);


		if(_cpntNodeFromNbElement==MAXROWS && last_rows!=1)
		{
			last_rows=1;
			force_insert=1;
		}
		status=res_query;
		if(last_rows==1 && res_query==0 && ociEnv!=0 && FIRST_TIME_MICROCONNECTIVITY==1)
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
				status+=freeMicroConnectivity();
				if(nodefrom!=NULL) //means not null otehriwse it's -1
					free(nodefrom);
				if(nodeto!=NULL)
					free(nodeto);
				if(linktype!=NULL)
					free(linktype);


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
				sprintf(sqlstmt,"BEGIN if :nfrom='motherboard' then insert ALL into %s(linkid,cpntidfrom,portidfrom,cpntidto,portidto,link_type,bidirectional_link_used,link_weight,lkused,author,created,terminal_name) values (%s,-1,:portnbfrom,cpntidto,:portnbto,lknb,:biused,lkweight,1,'%s',sysdate,'%s') select s.linktypeid as lknb,f.cpntid as cpntidto,decode(f.cpntid,-1,3,1) as lkweight from %s f,%s s where f.cpntname=:nto and s.link_name=:lktype ; else if :nto='motherboard' then insert ALL into %s(linkid,cpntidfrom,portidfrom,cpntidto,portidto,link_type,bidirectional_link_used,link_weight,lkused,author,created,terminal_name) values (%s,cpntidfrom,:portnbfrom,-1,:portnbto,lknb,:biused,lkweight,1,'%s',sysdate,'%s') select s.linktypeid as lknb,f.cpntid as cpntidfrom, decode(f.cpntid,-1,3,2) as lkweight from %s f,%s s where f.cpntname=:nfrom and s.link_name=:lktype; else insert ALL into %s(linkid,cpntidfrom,portidfrom,cpntidto,portidto,link_type,bidirectional_link_used,link_weight,lkused,author,created,terminal_name) values (%s,cpntidfrom,:portnbfrom,cpntidto,:portnbto,lknb,:biused,0,1,'%s',sysdate,'%s') select s.linktypeid as lknb,t.cpntid as cpntidfrom,f.cpntid as cpntidto from %s t,%s f,%s s where t.cpntname=:nfrom and f.cpntname=:nto and s.link_name=:lktype and f.motherboardid=t.motherboardid; end if;end if;:numrows:=%s;end;",MICRO_CONNECTIVITY_TABLE,seqname,login,host,LG_CPNTS_TABLE,LINKTYPE_TABLE,MICRO_CONNECTIVITY_TABLE,seqname,login,host,LG_CPNTS_TABLE,LINKTYPE_TABLE,MICRO_CONNECTIVITY_TABLE,seqname,login,host,LG_CPNTS_TABLE,LG_CPNTS_TABLE,LINKTYPE_TABLE,SQLROWCOUNT);
				status=OCIStmtPrepare(hstmt, ociError, (text*)sqlstmt, (ub4)strlen((char *)sqlstmt), (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
				//std::cout<<"value of stmt "<<sqlstmt <<" and length="<<strlen(sqlstmt)<<std::endl;
			}
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
			}
			else
			{
				nodefrom=(char*)malloc(_cpntNodeFromNbElement*_max_cpntNodeFromList_len*sizeof(char));
				if(nodefrom!=NULL)
					status+=NormalizeVector(_cpntNodeFromList, _cpntNodeFromNbElement,_max_cpntNodeFromList_len,nodefrom);

				nodeto=(char*)malloc(_cpntNodeToNbElement*_max_cpntNodeToList_len*sizeof(char));
				if(nodeto!=NULL)
					status+=NormalizeVector(_cpntNodeToList, _cpntNodeToNbElement,_max_cpntNodeToList_len,nodeto);

				linktype=(char*)malloc(_cpntLinkTypeNbElement2*_max_cpntLinkTypeList_len2*sizeof(char));
				if(linktype!=NULL)
					status+=NormalizeVector(_cpntLinkTypeList2, _cpntLinkTypeNbElement2,_max_cpntLinkTypeList_len2,linktype);


				numrows_inserted=(int*)malloc(sizeof(int)*_cpntNodeFromNbElement);
				if(nodefrom==NULL || nodeto==NULL ||linktype==NULL ||numrows_inserted==NULL)
				{

					status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
					rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");
					GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
					status+=freeMicroConnectivity();
					if(nodefrom!=NULL) //means not null otehriwse it's -1
						free(nodefrom);
					if(nodeto!=NULL)
						free(nodeto);
					if(linktype!=NULL)
						free(linktype);


					if(numrows_inserted!=NULL)
						free(numrows_inserted);
					return -1;
				}
				else
				{
					for(i=0;i<_cpntNodeFromNbElement;i++)
						numrows_inserted[i]=0;

					status =OCIBindByName(hstmt, &bndp2[0], ociError,(text*) ":portnbfrom", -1,(dvoid*)&_cpntportnbfromList[0], sizeof(int),  SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
				}
			}
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp2[1], ociError,(text*) ":portnbto", -1,(dvoid*) &_cpntportnbtoList[0],  sizeof(int),SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp2[2], ociError,(text*) ":biused", -1,(dvoid*)&_cpntBidirectionalList[0], sizeof(bidirectional_link_used),  SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp2[3], ociError,(text*) ":nfrom", -1,(dvoid*)nodefrom, _max_cpntNodeFromList_len,  SQLT_STR, (dvoid *) & _cpntNodeFrom_nullvalue[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);


			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp2[4], ociError,(text*) ":nto", -1,(dvoid*)nodeto, _max_cpntNodeToList_len,  SQLT_STR,  (dvoid *) & _cpntNodeTo_nullvalue[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp2[5], ociError,(text*) ":lktype", -1,(dvoid*)linktype, _max_cpntLinkTypeList_len2,SQLT_STR, (dvoid *) & _cpntLinkType_nullvalue[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);






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
				status=OCIBindArrayOfStruct(bndp2[0], ociError,   sizeof(int),0, 0, 0);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp2[1], ociError,  sizeof(int),0, 0, 0);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp2[2], ociError, sizeof(int), 0, 0, 0);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp2[3], ociError,_max_cpntNodeFromList_len, sizeof(int), 0, 0);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp2[4], ociError, _max_cpntNodeToList_len, sizeof(int), 0, 0);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp2[5], ociError,_max_cpntLinkTypeList_len2, sizeof(int), 0, 0);

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
				status= OCIStmtExecute(ociHdbc, hstmt, ociError, _cpntNodeFromNbElement, 0, 0, 0, OCI_DEFAULT );

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
			}
			else
			{
				//check if all rows has been inserted
				//if not we do a rollback
				numrows=-1;
				for(i=0;i<_cpntNodeFromNbElement;i++)
				{
					if(numrows_inserted[i]==0)
					{
						numrows=0;
						i=_cpntNodeFromNbElement+5;
					}
					else
						numrows=numrows_inserted[i];
				}
				if(numrows==0)
				{
					status = OCITransCommit(ociHdbc, ociError, 0);
					status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
					status+=freeMicroConnectivity();


					if(nodefrom!=NULL) //means not null otehriwse it's -1
						free(nodefrom);
					if(nodeto!=NULL)
						free(nodeto);
					if(linktype!=NULL)
						free(linktype);

					free(numrows_inserted);
					GetErrorMess(appliName, "COULDNOT_INSERT_ALL_ROWS",ErrMess,1);

					return -1;
				}
				else
				{
					status = OCITransCommit(ociHdbc, ociError, 0);
					//sprintf(sqlstmt,"BEGIN  :rescode:=%s(1); :numrows:=%s; END; ",MACRO_CONNECTIVITY_TABLE,_UpdateDynamicTables,SQLROWCOUNT);
					//std::cout<<"value of stmt 3 "<<sqlstmt <<std::endl;

				}

			}
			status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
			status+=freeMicroConnectivity();
			//std::cout<<"after freeconnect"<<std::endl;
			if(nodefrom!=NULL) //means not null otehriwse it's -1
				free(nodefrom);
			if(nodeto!=NULL)
				free(nodeto);
			if(linktype!=NULL)
				free(linktype);

			if(numrows_inserted!=NULL)
				free(numrows_inserted);
			if(rescode!=0)
				OCIReportError(ociError,appliName, ErrMess,1); 
			else
				OCIReportError(ociError,appliName, ErrMess,0);  
			if(force_insert==1 && rescode==0)
			{
				FIRST_TIME_MICROCONNECTIVITY=1;
				force_insert=0;
			}


		}
		else
		{
			if(res_query!=0)
			{
				status=freeMicroConnectivity();
				status+=res_query;
				GetErrorMess(appliName, "Cache pb",ErrMess,1);
			}
			if(FIRST_TIME_MICROCONNECTIVITY!=1)
			{
				status=freeMicroConnectivity();
				status+=res_query;
				GetErrorMess(appliName, "CACHE HAS NOT BEEN INITIALIZED",ErrMess,1);
			}
			if(ociEnv==0)
			{
				status=freeMicroConnectivity();
				status+=res_query;
				GetErrorMess(appliName, "NOT CONNECTED TO ANY DB",ErrMess,1);
			}
			if(ociEnv!=0 && FIRST_TIME_MICROCONNECTIVITY==1 && res_query==0)
			{
				status=0;
				GetErrorMess(appliName, "NO_ERROR",ErrMess,0);
			}

		}
		return (status+rescode);
	}

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif





