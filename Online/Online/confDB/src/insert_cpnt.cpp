/********************************************************************************/
//	Author: L.Abadie
// version:v3.5
// changes: handling error such that can be extended to C++: (create a class SQLException) (strlen ck + update stmt)
// file which performs bulk collect insert for the cpnt table
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

	/************************global variables for the cpnt***********************/
	int FIRST_TIME_CPNT=0;
	char* _CpntList;
	int _max_cpntLength;
	int _cpntListLength;



	char* _cpnthwtype1List;
	int _max_cpnthwtype1Length;
	int _cpnthwtype1ListLength;

	char* _cpntresponsible1List;
	int _max_cpntresponsible1Length;
	int _cpntresponsible1ListLength;


	char*  _cpntcomments1List;
	int _max_cpntcomments1Length;
	int _cpntcomments1ListLength;



	char* _cpntserialnb1List;
	int _max_cpntserialnb1_length;
	int _cpntserialnb1ListLength;


	char* _cpnttypeList;
	int _max_cpnttype_length;
	int _cpnttypeListLength;




	int * _cpntreplacable;

	int _cpntNbElement;
	int _cpnttypeNbElement;
	int _cpnthwtype1NbElement;

	int _cpntresponsible1NbElement;
	int _cpntcomments1NbElement;
	int _cpntreplacableNbElement;
	int _cpntserialnb1NbElement;
	int _cpntlocationNbElement;
	int _cpntmotherboardNbElement;

	char* _cpntlocationList;
	int _max_cpntlocation_length;
	int _cpntlocationListLength;

	char* _cpntmotherboardList;
	int _max_cpntmotherboard_length;
	int _cpntmotherboardListLength;

	int* _cpntList_nullvalue;
	int _cpntListNbEl;

	int* _cpntserialnb1_nullvalue;
	int _cpntserialnb1NbEl;


	int* _cpnttype_nullvalue;
	int _cpnttypeNbEl;

	int* _cpntresponsible1_nullvalue;
	int _cpntresponsible1NbEl;


	int* _cpnthwtype1_nullvalue;
	int _cpnthwtype1NbEl;

	int* _cpntcomments1_nullvalue;
	int _cpntcomments1NbEl;


	int* _cpntlocationList_nullvalue;
	int _cpntlocationListNbEl;

	int* _cpntmotherboardList_nullvalue;
	int _cpntmotherboardListNbEl;
	int freeCpnt()
	{
		int status=0;
		_CpntList=(char*)realloc(_CpntList,0*sizeof(char));
		_cpntserialnb1List=(char*)realloc(_cpntserialnb1List,0*sizeof(char));
		_cpnttypeList=(char*)realloc(_cpnttypeList,0*sizeof(char));
		_cpntlocationList=(char*)realloc(_cpntlocationList,0*sizeof(char));
		_cpnthwtype1List=(char*)realloc(_cpnthwtype1List,0*sizeof(char));
		_cpntresponsible1List=(char*)realloc(_cpntresponsible1List,0*sizeof(char));
		_cpntcomments1List=(char*)realloc(_cpntcomments1List,0*sizeof(char));

		_cpntreplacable=(int*)realloc(_cpntreplacable,0*sizeof(int));
		_cpntList_nullvalue=(int*)realloc(_cpntList_nullvalue,0*sizeof(int));
		_cpntserialnb1_nullvalue=(int*)realloc(_cpntserialnb1_nullvalue,0*sizeof(int));
		_cpntlocationList_nullvalue=(int*)realloc(_cpntlocationList_nullvalue,0*sizeof(int));
		_cpnthwtype1_nullvalue=(int*)realloc(_cpnthwtype1_nullvalue,0*sizeof(int));
		_cpntcomments1_nullvalue=(int*)realloc(_cpntcomments1_nullvalue,0*sizeof(int));
		_cpntresponsible1_nullvalue=(int*)realloc(_cpntresponsible1_nullvalue,0*sizeof(int));

		_cpnttype_nullvalue=(int*)realloc(_cpnttype_nullvalue,0*sizeof(int));
		_cpnttype_nullvalue=NULL;
		_cpntList_nullvalue=NULL;
		_cpntserialnb1_nullvalue=NULL;
		_cpntmotherboardList=(char*)realloc(_cpntmotherboardList,0*sizeof(char));
		_cpntmotherboardList_nullvalue=(int*)realloc(_cpntmotherboardList_nullvalue,0*sizeof(int));
		_cpntmotherboardList=NULL;

		_CpntList=NULL;
		_cpntserialnb1List=NULL;
		_cpntreplacable=NULL;
		_cpntlocationList=NULL;
		_cpnttypeList=NULL;
		_cpnthwtype1_nullvalue=NULL;
		_cpntcomments1_nullvalue=NULL;
		_cpntresponsible1_nullvalue=NULL;
		_cpntcomments1List=NULL;
		_cpntresponsible1List=NULL;
		_cpnthwtype1List=NULL;
		FIRST_TIME_CPNT=0;
		return status;
	}

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

	//extern "C" __declspec(dllexport)
	EXTERN_CONFDB
		int InsertMultipleBoardCpnts(char* cpntname,char* cpnttype,int replacable,char* motherboardname,char* serial_nb,char* hwtype,char* responsible,char* comments,char* location,int first_time1,int last_rows1,char* ErrMess)
	{
		char appliName[100]="InsertMultipleBoardCpnts";
		char sqlstmt[2000];
		OCIStmt *hstmt;
		OCIBind  *bndp[10]; 
		int rescode=0;
		int free_mem=0;
		int res_query=0;
		sword status=0;
		char* dname=NULL;
		int i=0;
		char* dlocation=NULL;
		char* dlocationbis=NULL;
		char* dserialnb=NULL;
		char* dtype=NULL;
		char* dresponsible=NULL;
		char* dcomments=NULL;
		char* dhwtype=NULL;
		int force_insert=0;
		int first_time=first_time1;
		char hist_seq[100]="LHCB_CPNT_HISTORY_COMPTID_SEQ.nextval";
		char hw_seq[100]="LHCB_HW_CPNTS_SNBID_SEQ.nextval";
		char lg_seq[100]="LHCB_LG_CPNTS_DEVICEID_SEQ.nextval";

		//std::cout<<"value of first_time "<<first_time <<std::endl;
		int last_rows=last_rows1;
		char seq_name[100]="lhcb_lg_cpntseq.nextval";
		int* numrows_inserted=NULL;
		int* snbidList=NULL;
		int numrows=0;
		OCIBind  *bndp3 = (OCIBind *) 0;
		char* errmessg1=(char*)malloc(ERROR_BUFFER*sizeof(char));


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
					status=freeCpnt();
					GetErrorMess(appliName, "If the cpnt is replacable,a serialnb is mandatory the",ErrMess,1);
					return -1;
				}
			}
			else
			{
				status=freeCpnt();
				GetErrorMess(appliName, "If the cpnt is replacable,a serialnb is mandatory the",ErrMess,1);
				return -1;
			}

		}

		if(FIRST_TIME_CPNT==1 && _CpntList==NULL)
		{
			first_time=1;
			//std::cout<<"case of FIRST_TIME_DEVICE==1 && _CpntList==NULL"<<std::endl;

		}
		if(first_time==1)
		{
			status=freeCpnt(); //delete the cache in case it was exiting with error
			FIRST_TIME_CPNT=1;

		}
		//std::cout<<"before appending string "<<std::endl;

		res_query=AppendString(cpntname,_CpntList,_cpntListLength,_cpntNbElement,_max_cpntLength,first_time);
		res_query+=AppendString(serial_nb,_cpntserialnb1List,_cpntserialnb1ListLength,_cpntserialnb1NbElement,_max_cpntserialnb1_length,first_time);
		res_query+=AppendString(cpnttype,_cpnttypeList,_cpnttypeListLength,_cpnttypeNbElement,_max_cpnttype_length,first_time);
		res_query+=AppendString(location,_cpntlocationList,_cpntlocationListLength,_cpntlocationNbElement,_max_cpntlocation_length,first_time);
		res_query+=AppendString(responsible,_cpntresponsible1List,_cpntresponsible1ListLength,_cpntresponsible1NbElement,_max_cpntresponsible1Length,first_time);
		res_query+=AppendString(comments,_cpntcomments1List,_cpntcomments1ListLength,_cpntcomments1NbElement,_max_cpntcomments1Length,first_time);
		res_query+=AppendString(hwtype,_cpnthwtype1List,_cpnthwtype1ListLength,_cpnthwtype1NbElement,_max_cpnthwtype1Length,first_time);
		res_query+=AppendString(motherboardname,_cpntmotherboardList,_cpntmotherboardListLength,_cpntmotherboardNbElement,_max_cpntmotherboard_length,first_time);

		//std::cout<<"before appending int "<<std::endl;

		res_query+=AppendInt(replacable,_cpntreplacable,_cpntreplacableNbElement,first_time);


		res_query+=AppendInt(dname1,_cpntList_nullvalue,_cpntListNbEl,first_time);
		res_query+=AppendInt(dlocation1,_cpntlocationList_nullvalue,_cpntlocationListNbEl,first_time);
		res_query+=AppendInt(dserialnb1,_cpntserialnb1_nullvalue,_cpntserialnb1NbEl,first_time);
		res_query+=AppendInt(dtype1,_cpnttype_nullvalue,_cpnttypeNbEl,first_time);
		res_query+=AppendInt(dhwtype1,_cpnthwtype1_nullvalue,_cpnthwtype1NbEl,first_time);
		res_query+=AppendInt(dcomments1,_cpntcomments1_nullvalue,_cpntcomments1NbEl,first_time);
		res_query+=AppendInt(dresponsible1,_cpntresponsible1_nullvalue,_cpntresponsible1NbEl,first_time);
		res_query+=AppendInt(dlocation2,_cpntmotherboardList_nullvalue,_cpntmotherboardListNbEl,first_time);

		//std::cout<<"value of cpntlist "<<_CpntList<<std::endl;

		//std::cout<<"value of FIRST_TIME_CPNT "<<FIRST_TIME_CPNT <<std::endl;

		if(_cpntNbElement==MAXROWS && last_rows!=1)
		{
			force_insert=1;
			last_rows=1;
		}
		status=res_query;
		if(last_rows==1 && res_query==0 && ociEnv!=0 && FIRST_TIME_CPNT==1)
		{
			int len_host=LOGIN_LEN;
			char login[LOGIN_LEN];
			char host[LOGIN_LEN];
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
				status+=freeCpnt();
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
				if(dlocationbis!=NULL)
					free(dlocationbis);
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
				//sprintf(sqlstmt,"BEGIN if nvl(:motherboard,'none') not like 'none' then insert ALL into %s(snbid,serialnb,hwtype,responsible,user_comment,cpnt_status,hwname,replacable,location) values(%s,:serialnb,:hwtype,:responsible,:comments,%d,:dname,:replacable,location) into %s(cpntname,cpnttype,snbid,cpntid,motherboardid,author,created,terminal_name) values (:dname,:cpnttype,snbid,%s,motherboardid,'%s',sysdate,'%s') into %s(historycpntID,snbid,cpntid,cpnt_status,user_comment,location,status_change,created,author,terminal_name) values(%s,snbid,cpntid,'IN_USE',:comments,:motherboard,statuschange,sysdate,'%s','%s') select t.deviceid as motherboardid,t.serialnb as location,e.snbid,e.cpntid,e.created as statuschange from %s e,%s t where  e.cpntname=:dname and t.devicename=:motherboard and t.serialnb is not null; else insert into %s(snbid,serialnb,hwtype,responsible,user_comment,cpnt_status,hwname,replacable,location) values(%s,:serialnb,:hwtype,:responsible,:comments,%d,:dname,:replacable,:location) returning snbid into :snbid; if %s=1 then insert into %s(historycpntID,snbid,cpnt_status,user_comment,location,status_change,created,author,terminal_name) values(%s,:snbid,'SPARE',:comments,:location,sysdate,sysdate,'%s','%s'); end if; end if;:numrows:=%s; END;",HW_CPNTS_TABLE,hw_seq,IN_USE,LG_CPNTS_TABLE,lg_seq,login,host,HISTORY_CPNT_TABLE,hist_seq,login,host,LG_CPNTS_TABLE,LOGICAL_DEVICE_TABLE, HW_CPNTS_TABLE,hw_seq,SPARE,SQLROWCOUNT,HISTORY_CPNT_TABLE,hist_seq,login,host,SQLROWCOUNT);
				sprintf(sqlstmt,"BEGIN if nvl(:motherboard,'none') not like 'none' then insert ALL into %s(snbid,serialnb,hwtype,responsible,user_comment,cpnt_status,hwname,replacable,location) values(%s,:serialnb,:hwtype,:responsible,:comments,%d,:dname,:replacable,location) select serialnb as location from %s where devicename=:motherboard and serialnb is not null; insert all into %s(cpntname,cpnttype,snbid,cpntid,motherboardid,location,author,created,terminal_name) values (:dname,:cpnttype,snbid,%s,motherboardid,:location,'%s',sysdate,'%s') select t.snbid as snbid, e.deviceid as motherboardid from %s t,%s  e where t.serialnb=:serialnb and e.devicename=:motherboard;if %s=1 then insert all into %s(historycpntID,snbid,cpntid,cpnt_status,user_comment,location,status_change,created,author,terminal_name) values(%s,snbid,cpntid,'IN_USE',:comments,:motherboard,statuschange,sysdate,'%s','%s') select snbid,cpntid,created as statuschange from %s where  cpntname=:dname; end if; else insert into %s(snbid,serialnb,hwtype,responsible,user_comment,cpnt_status,hwname,replacable,location) values(%s,:serialnb,:hwtype,:responsible,:comments,%d,:dname,:replacable,:location) returning snbid into :snbid; if %s=1 then insert into %s(historycpntID,snbid,cpnt_status,user_comment,location,status_change,created,author,terminal_name) values(%s,:snbid,'SPARE',:comments,:location,sysdate,sysdate,'%s','%s'); end if; end if;:numrows:=%s; END;",HW_CPNTS_TABLE,hw_seq,IN_USE,LOGICAL_DEVICE_TABLE,LG_CPNTS_TABLE,lg_seq,login,host,HW_CPNTS_TABLE,LOGICAL_DEVICE_TABLE,SQLROWCOUNT,HISTORY_CPNT_TABLE,hist_seq,login,host,LG_CPNTS_TABLE,HW_CPNTS_TABLE,hw_seq,SPARE,SQLROWCOUNT,HISTORY_CPNT_TABLE,hist_seq,login,host,SQLROWCOUNT);
				status=OCIStmtPrepare(hstmt, ociError, (text*)sqlstmt, (ub4)strlen((char *)sqlstmt), (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
				//std::cout<<"query"<<sqlstmt<<std::endl;
			}

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
			}
			else
			{
				dname=(char*)malloc( _cpntNbElement*_max_cpntLength*sizeof(char));
				if(dname!=NULL)
				{
					status=NormalizeVector(_CpntList, _cpntNbElement,_max_cpntLength,dname);
					//std::cout<<"dname not null "<<_max_cpntLength<<std::endl;
				}
				dserialnb=(char*)malloc( _cpntserialnb1NbElement*_max_cpntserialnb1_length*sizeof(char));
				if(dserialnb!=NULL)
				{
					status+=NormalizeVector(_cpntserialnb1List, _cpntserialnb1NbElement,_max_cpntserialnb1_length,dserialnb);
					//std::cout<<"dserialnb not null "<<_max_cpntserialnb1_length<<std::endl;
				}
				dlocation=(char*)malloc( _cpntlocationNbElement*_max_cpntlocation_length*sizeof(char));
				if(dlocation!=NULL)
				{
					status+=NormalizeVector(_cpntlocationList, _cpntlocationNbElement,_max_cpntlocation_length,dlocation);
					//std::cout<<"dlocation not null "<<_max_cpntlocation_length<<std::endl;
				}
				dlocationbis=(char*)malloc( _cpntmotherboardNbElement*_max_cpntmotherboard_length*sizeof(char));
				if(dlocationbis!=NULL)
				{
					status+=NormalizeVector(_cpntmotherboardList, _cpntmotherboardNbElement,_max_cpntmotherboard_length,dlocationbis);
					//std::cout<<"dlocation not null "<<_max_cpntlocation_length<<std::endl;
				}
				dtype=(char*)malloc(_cpnttypeNbElement*_max_cpnttype_length*sizeof(char));
				if(dtype!=NULL)
				{
					status+=NormalizeVector(_cpnttypeList, _cpnttypeNbElement,_max_cpnttype_length,dtype);
					//std::cout<<"dtype not null "<<_max_cpnttype_length<<std::endl;
				}
				dhwtype=(char*)malloc(_cpnthwtype1NbElement*_max_cpnthwtype1Length*sizeof(char));
				if(dhwtype!=NULL)
					status+=NormalizeVector(_cpnthwtype1List, _cpnthwtype1NbElement,_max_cpnthwtype1Length,dhwtype);
				dcomments=(char*)malloc(_cpntcomments1NbElement*_max_cpntcomments1Length*sizeof(char));
				if(dcomments!=NULL)
					status+=NormalizeVector(_cpntcomments1List, _cpntcomments1NbElement,_max_cpntcomments1Length,dcomments);
				dresponsible=(char*)malloc(_cpntresponsible1NbElement*_max_cpntresponsible1Length*sizeof(char));
				if(dresponsible!=NULL)
				{
					status+=NormalizeVector(_cpntresponsible1List, _cpntresponsible1NbElement,_max_cpntresponsible1Length,dresponsible);
					//std::cout<<"dresponsible not null "<<_max_cpntresponsible1Length<<std::endl;
				}
				numrows_inserted=(int*)malloc(sizeof(int)*_cpntNbElement);
				snbidList=(int*)malloc(sizeof(int)*_cpntNbElement);;
				//std::cout<<"after memory allocation "<<_cpntNbElement<<std::endl;
				if( (dtype==NULL) ||(dlocationbis==NULL)||(snbidList==NULL)|| (dname==NULL) || (dresponsible==NULL) || (dserialnb==NULL) || (dresponsible==NULL)||dcomments==NULL||dhwtype==NULL||numrows_inserted==NULL)
				{
					//std::cout<<"memory pb "<<std::endl;

					rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");
					GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
					status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
					//std::cout<<"memory pb 1"<<std::endl;
					status+=freeCpnt();
					//std::cout<<"memory pb 2"<<std::endl;
					if(dname!=NULL)
						free(dname);
					if(dtype!=NULL)
						free(dtype);
					if(dlocation!=NULL)
						free(dlocation);
					if(dlocationbis!=NULL)
						free(dlocationbis);
					//std::cout<<"memory pb 3"<<std::endl;
					if(dserialnb!=NULL)
						free(dserialnb);
					if(dresponsible!=NULL)
						free(dresponsible);
					if(dcomments!=NULL)
						free(dcomments);
					//std::cout<<"memory pb 4"<<std::endl;
					if(dhwtype!=NULL)
						free(dhwtype);
					if(numrows_inserted!=NULL)
						free(numrows_inserted);
					if(snbidList!=NULL)
						free(snbidList);
					//if(errmessg1!=NULL)
					//free(errmessg1);
					//if(errmessg2!=NULL)
					//free(errmessg2);
					return -1;
				}
				else
				{
					for(i=0;i<_cpntNbElement;i++)
					{
						numrows_inserted[i]=0;
						snbidList[i]=0;
					}
					//std::cout<<"before binding 0"<<std::endl;
					status =OCIBindByName(hstmt, &bndp[0], ociError,(text*) ":dname", -1,(dvoid*)dname, _max_cpntLength,  SQLT_STR, (dvoid *) & _cpntList_nullvalue[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
				}
			}
			//std::cout<<"before binding "<<std::endl;
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp[1], ociError,(text*) ":cpnttype", -1,(dvoid*)dtype, _max_cpnttype_length,  SQLT_STR, (dvoid *) & _cpnttype_nullvalue[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp[2], ociError,(text*) ":replacable", -1,(dvoid*)&_cpntreplacable[0], sizeof(int),  SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp[3], ociError,(text*) ":snbid", -1,(dvoid*)&snbidList[0], sizeof(int),  SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp[4], ociError,(text*) ":motherboard", -1,(dvoid*)dlocationbis, _max_cpntmotherboard_length,  SQLT_STR, (dvoid *)& _cpntmotherboardList_nullvalue[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);


			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp[5], ociError,(text*) ":serialnb", -1,(dvoid*)dserialnb, _max_cpntserialnb1_length,  SQLT_STR, (dvoid *) & _cpntserialnb1_nullvalue[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp[6], ociError,(text*) ":location", -1,(dvoid*)dlocation, _max_cpntlocation_length,  SQLT_STR, (dvoid *)& _cpntlocationList_nullvalue[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp[7], ociError,(text*) ":hwtype", -1,(dvoid*)dhwtype, _max_cpnthwtype1Length,SQLT_STR, (dvoid *)&_cpnthwtype1_nullvalue[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp[8], ociError,(text*) ":responsible", -1,(dvoid*)dresponsible, _max_cpntresponsible1Length,SQLT_STR, (dvoid *) &_cpntresponsible1_nullvalue[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp[9], ociError,(text*) ":comments", -1,(dvoid*)dcomments, _max_cpntcomments1Length,SQLT_STR, (dvoid *) &_cpntcomments1_nullvalue[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);

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
				status=OCIBindArrayOfStruct(bndp[0], ociError,_max_cpntLength, sizeof(int),0, 0);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp[1], ociError,  _max_cpnttype_length, sizeof(int), 0, 0);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp[2], ociError, sizeof(int), 0, 0, 0);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp[3], ociError,  sizeof(int), 0, 0, 0);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp[4], ociError,_max_cpntmotherboard_length, sizeof(int),  0, 0);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp[5], ociError, _max_cpntserialnb1_length, sizeof(int), 0, 0);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp[6], ociError, _max_cpntlocation_length, sizeof(int), 0, 0);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp[7], ociError, _max_cpnthwtype1Length, sizeof(int), 0, 0);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp[8], ociError, _max_cpntresponsible1Length, sizeof(int), 0, 0);


			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp[9], ociError, _max_cpntcomments1Length, sizeof(int), 0, 0);


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
				status= OCIStmtExecute(ociHdbc, hstmt, ociError, _cpntNbElement, 0, 0, 0, OCI_DEFAULT );
			//std::cout<<"after execute "<<std::endl;
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
			}
			else
			{
				numrows=-1;
				for(i=0;i< _cpntNbElement;i++)
				{
					if(numrows_inserted[i]==0)
					{
						numrows=0;
						i= _cpntNbElement+5;
					}
					else
						numrows=numrows_inserted[i];
				}
				if(numrows==0)
				{
					status = OCITransCommit(ociHdbc, ociError, 0);
					status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
					status+=freeCpnt();
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
					if(dlocationbis!=NULL)
						free(dlocationbis);
					if(numrows_inserted!=NULL)
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

			status+=freeCpnt();
			//std::cout<<"after free cpnt final"<<std::endl;
			if(dname!=NULL)
				free(dname);
			if(dtype!=NULL)
				free(dtype);
			if(dlocation!=NULL)
				free(dlocation);
			if(dlocationbis!=NULL)
				free(dlocationbis);
			if(dserialnb!=NULL)
				free(dserialnb);
			if(dresponsible!=NULL)
				free(dresponsible);
			if(dcomments!=NULL)
				free(dcomments);
			if(dhwtype!=NULL)
				free(dhwtype);
			if(numrows_inserted!=NULL)
				free(numrows_inserted);

			status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);

			if(rescode!=0)
				if(ociError!=0)
					OCIReportError(ociError,appliName, ErrMess,1); 
				else
					GetErrorMess(appliName,  "NOT CONNECTED TO ANY DB",ErrMess,1); 
			else
				OCIReportError(ociError,appliName, ErrMess,0); 
			if(force_insert==1 && rescode==0)
			{
				status=freeCpnt();
				FIRST_TIME_CPNT=1;
				force_insert=0;

			}
		}
		else
		{
			if(res_query!=0)
			{
				status=freeCpnt();
				GetErrorMess(appliName, "Cache Problem",ErrMess,1);
			}
			if(FIRST_TIME_CPNT!=1)
			{
				status=freeCpnt();

				res_query=-1;
				GetErrorMess(appliName, "CACHE HAS NOT BEEN INITIALIZED",ErrMess,1);
			}
			if(ociEnv==0)
			{
				status=freeCpnt();
				res_query=-1;
				GetErrorMess(appliName, "NOT CONNECTED TO ANY DB",ErrMess,1);
			}
			if(ociEnv!=0 && FIRST_TIME_CPNT==1 && res_query==0)
			{
				status=0;
				GetErrorMess(appliName, "NO_ERROR",ErrMess,0);
			}
			status+=res_query;

		}
		//std::cout<<"end of fct "<<std::endl;
		return (status+rescode);
	}


#if defined(__cplusplus) || defined(c_plusplus)
}
#endif