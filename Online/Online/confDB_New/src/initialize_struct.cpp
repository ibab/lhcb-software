
#if !defined( _INITIALIZE_STRUCTURE_) 
#define  _INITIALIZE_STRUCTURE_
#endif

#include "list_structures.h"
//#include "test33.h"
char  _list_of_subsystemname[50][10];
int _list_of_subsystemID[50];
int _nb_of_subsystem;

int AppendString(char* source_copy,char* &VectorList, int &Vector_currentsize,int &nbofElement,int &max_sourceLength, int first_time)
{
	int status=0;
	int len_source=0;
	//char* p=NULL;
	if(source_copy!=NULL)
		len_source=strlen(source_copy)+1;


	int current_vectorsize=Vector_currentsize;
	if(first_time==1)
	{
		VectorList=NULL;

		nbofElement=0;
		Vector_currentsize=0;
		max_sourceLength=0;


		Vector_currentsize+=len_source;
		if(VectorList=(char*)realloc(VectorList,len_source*sizeof(char)))
		{
			strcpy(VectorList,source_copy);
		}
		else
		{
			status=-1;
			free(VectorList);

		}

		if(len_source>max_sourceLength)
			max_sourceLength=len_source;

		nbofElement++;
	}
	else
	{
		Vector_currentsize+=len_source;
		if(VectorList=(char*)realloc(VectorList,Vector_currentsize*sizeof(char)))
		{
			//memcpy(VectorList,VectorList,current_vectorsize);
			memcpy(VectorList+current_vectorsize,source_copy,len_source);
		}
		else
		{
			status=-1;
			free(VectorList);

		}

		if(len_source>max_sourceLength)
			max_sourceLength=len_source;

		nbofElement++;
	}

	return status;
}



int AppendInt(int source_copy,int* &VectorList, int &Vector_currentsize, int first_time)
{
	int status=0;
	int i=Vector_currentsize;
	if(first_time==1)
	{
		VectorList=0;
		Vector_currentsize=0;
	}

	if(VectorList=(int*)realloc(VectorList,(Vector_currentsize+1)*sizeof(int)))
	{

		VectorList[Vector_currentsize]=source_copy;
		//memcpy(VectorList+Vector_currentsize,source_copy,1);
		Vector_currentsize++;

	}
	else
	{
		status=-1;
	}
	return status;
}

int NormalizeVector(char* VectorList, int nbofElement,int max_sourceLength,char* VectorDestination)
{
	//char* vector_normalize=NULL;
	char* p=VectorList;
	int i=0;
	int len_source=0;
	if(p!=NULL)
		len_source=strlen(p)+1;
	else
		return -1;

	while(i<nbofElement)
	{

		memcpy(VectorDestination+i*max_sourceLength,p,len_source);

		p+=len_source;

		i++;
		if(i!=nbofElement)
			len_source=strlen(p)+1;


	}

	return 0;
}



int ShowErrors (sword status, OCIError *errhp, const char* log) 
{
	text errbuf[512];
	sword errcode;
	switch (status)   
	{
	case OCI_SUCCESS:
		return 0;
	case OCI_SUCCESS_WITH_INFO:
		OCIErrorGet (errhp, 1, 0, &errcode, errbuf, sizeof(errbuf), OCI_HTYPE_ERROR);
		printf("Error - OCI_SUCCESS_WITH_INFO %s and errbuf: %s\n", log,errbuf);
		//return errcode;
		return 0;
	case OCI_NEED_DATA:
		printf("Error - OCI_NEED_DATA %s\n", log);
		return status;
	case OCI_NO_DATA:
		printf("Error - OCI_NODATA %s\n", log);
		return status;
	case OCI_ERROR:
		OCIErrorGet (errhp, 1, 0, &errcode, errbuf, sizeof(errbuf), OCI_HTYPE_ERROR);
		printf("Error - %s and %s\n", errbuf, log);
		return errcode;
	case OCI_INVALID_HANDLE:
		printf("Error - OCI_INVALID_HANDLE %s\n", log);
		return status;
	case OCI_STILL_EXECUTING:
		printf("Error - OCI_STILL_EXECUTE %s\n", log);
		return status;
	default:
		return status;
	}
}


void OCIReportError(OCIError *errhp,const char* ApplicName, char* errMessg, int errnum) 
{
	text errbuf[ ERROR_BUFFER];
	sword errcode;
	if(errnum==0)
	{
		sprintf(errMessg,"NO_ERROR in %s",ApplicName);
	}
	else
	{
		OCIErrorGet (errhp, 1, 0, &errcode, errbuf,  ERROR_BUFFER, OCI_HTYPE_ERROR);
		sprintf(errMessg,"ERROR in %s : %s",ApplicName,errbuf);
	}
}


void GetErrorMess(const char* ApplicName, const char* logMess,char* errMessg, int errnum) 
{
	if(errnum==0)
	{
		sprintf(errMessg,"NO_ERROR in %s",ApplicName);
	}
	else
	{
		sprintf(errMessg,"ERROR in %s : %s",ApplicName,logMess);
	}
}

int null_charptr(char* ptr_name)
{
	int value_ptr=0;
	if(ptr_name==NULL)
		value_ptr=-1;

	return value_ptr;

}
//return vale when doing a select for return values or buffer too small: security issue, len_attributevalue (=length using strlen)
void Format_output(int output_oraclecode, char* attribute_value, char* logmessage, char separator)
{
	//std::cout << "output_oraclecode" << output_oraclecode<< std::endl;
	switch(output_oraclecode)
	{
	case 0:
		strcpy(logmessage,"NO_ERROR");
		return;
	case -2:
		strcpy(logmessage,"ERROR:length of return greater than actual output, value can't put in signed short");
		sprintf(attribute_value,"ERROR_BUFFER_TOOSMALL%c",separator);
		return;
	case -1:
		strcpy(logmessage,"NULL_VALUE");
		sprintf(attribute_value,"NULL%c",separator);
		return;
	default:
		sprintf(logmessage,"ERROR:length of return greater than actual output you need %d",output_oraclecode);
		sprintf(attribute_value,"ERROR_BUFFER_TOOSMALL%c",separator);
		return;
	}



}

void MinStringLength(int &actual_len)
{
	if(actual_len<MINSTRINGLEN)
		actual_len=MINSTRINGLEN;
}


//compute the subsystemIDs ; quicker if it's done at this level
//_list_of_subsystemname
int GetSubsystemID(char * sysnameList)
{
	char sysname[10];
	int pos=0;
	unsigned int pos1=0;
	int i=0;
	int sysnameID=1;
	int stop=0;
	int check_needed=0;
	if(sysnameList==NULL)
		return -1;
	while(pos1<strlen(sysnameList))
	{
		pos=strcspn(sysnameList+pos1,",");
		if(pos==strlen(sysnameList)+1)//means one system
			strcpy(sysname,sysnameList);
		else
		{
			//std::cout<<"value of sysname ="<<sysnameList[pos1]<<std::endl;
			check_needed=1;
			if(sysnameList[pos1]!=' ')
				strncpy(sysname,sysnameList+pos1,pos);
			else
			{
				pos1++;
				strncpy(sysname,sysnameList+pos1,pos);

			}

		}
		sysname[pos]='\0';
		//std::cout<<"value of sysname ="<<sysname<<"and pos= "<<pos<<std::endl;
		std::cout<<"value of sysname ="<<sysname<<std::endl;
		while(stop==0 && i<_nb_of_subsystem)
		{
			if(strcmp(sysname,_list_of_subsystemname[i])==0)
			{
				sysnameID=sysnameID*(_list_of_subsystemID[i]);
				stop=1;
				check_needed=0;
			}
			i++;
		}

		if(sysnameID==1 || check_needed==1) // bad parsing
			return -1;
		pos1+=pos+1;
		stop=0;
		i=0;
	}
	if(sysnameID==1)
		sysnameID=-1;
	return sysnameID;

}

//compute the subsystemIDs ; quicker if it's done at this level
int RetrieveSysIDList(char * sysnameList,char* sysIDlist)
{
	char sysname[10];
	int Idlist_len=0;
	int pos=0;
	unsigned int pos1=0;
	int IDlist[20];
	int i=0;
	int j=0;
	int stop=0;
	char sysIDlist_temp[80];
	if(sysnameList==NULL)
		return -1;
	while(pos1<strlen(sysnameList))
	{
		pos=strcspn(sysnameList+pos1,",");
		if(pos==strlen(sysnameList)+1)//means one system
			strcpy(sysname,sysnameList);
		else
			memcpy(sysname,sysnameList+pos1,pos+1);
		sysname[pos]='\0';
		//std::cout<<"value of sysname="<<sysname<<std::endl;
		while(stop==0 &&  j<_nb_of_subsystem)
		{
			if(strcmp(sysname,_list_of_subsystemname[j])==0)
			{
				IDlist[i]=_list_of_subsystemID[j];
				stop=1;
				i++;
			}
			j++;
		}
		stop=0;
		j=0;


		pos1+=pos+1;

	}
	Idlist_len=i;
	//std::cout<<"value of Idlist_len="<<Idlist_len<<std::endl;
	//std::cout<<"value of sysname="<<sysname<<std::endl;
	//std::cout<<"value of IDlist[0]="<<IDlist[0]<<std::endl;

	for(i=0;i<Idlist_len;i++)
	{
		if(i==0)
		{
			if(Idlist_len!=1)
				sprintf(sysIDlist,"%d,",IDlist[i]);
			else
				sprintf(sysIDlist,"%d",IDlist[i]);
		}
		else
		{
			if(i==(Idlist_len-1))
				sprintf(sysIDlist,"%s%d",sysIDlist_temp,IDlist[i]);
			else
			{
				sprintf(sysIDlist,"%s%d,",sysIDlist_temp,IDlist[i]);
			}
		}
		strcpy(sysIDlist_temp,sysIDlist);
	}
	return 0;

}


//decompose the subsystemID in subsystem_name; quicker if it's done at this level
int DecomposeSysID(int sysID,char* sysIDlist)
{

	int pos=0;
	int pos1=0;
	int i=0;
	char sysIDlist_temp[80]="|";
	char sysIDlist_temp2[80]="|";
	int j=0;
	int value=0;
	if(sysID==-1)
	{
		strcpy(sysIDlist,"none");
		return 0;
	}
	while(j<_nb_of_subsystem)
	{
		value=_list_of_subsystemID[j];
		if(sysID%value==0)
		{
			sprintf(sysIDlist_temp,"%s%s|",sysIDlist_temp2,_list_of_subsystemname[j]);
			strcpy(sysIDlist_temp2,sysIDlist_temp);
		}

		j++;
	}


	if(strlen(sysIDlist_temp)==1)
	{
		strcpy(sysIDlist,"ERROR_INVALID_SYSID");
		return -1;
	}
	else
		strcpy(sysIDlist,sysIDlist_temp);

	return 0;

}
//decompose the subsystemID in subsystem_name; quicker if it's done at this level
int DecomposeSysID_bis(int sysID,char* sysIDlist)
{

	int pos=0;
	int pos1=0;
	int i=0;
	int j=0;
	char sysIDlist_temp[80]=",";
	char sysIDlist_temp2[80]="";
	int len_sys=0;
	int value=0;
	if(sysID==-1||sysID==0)			/////??
	{
		strcpy(sysIDlist,"none");
		return 0;
	}
	while(j<_nb_of_subsystem)
	{
		value=_list_of_subsystemID[j];
		if(sysID%value==0)
		{
			sprintf(sysIDlist_temp,"%s%s,",sysIDlist_temp2,_list_of_subsystemname[j]);
			strcpy(sysIDlist_temp2,sysIDlist_temp);
		}

		j++;
	}
	if(strlen(sysIDlist_temp)==1)
	{
		strcpy(sysIDlist,"ERROR_INVALID_SYSID");
		return -1;
	}
	else
		strcpy(sysIDlist,sysIDlist_temp);
	len_sys=strlen(sysIDlist)-1;
	sysIDlist[len_sys]='\0';
	return 0;

}
int GetDeviceStatusID(char* device_status)
{
	int status_case=-10;
	if(strcmp(device_status,"DESTROYED")==0)
		status_case=DESTROYED;
	if(strcmp(device_status,"IN_REPAIR")==0)
		status_case=IN_REPAIR;
	if(strcmp(device_status,"SPARE")==0)
		status_case=SPARE;
	if(strcmp(device_status,"TEST")==0)
		status_case=TEST;
	if(strcmp(device_status,"EXT_TEST")==0)
		status_case=EXT_TEST;
	if(strcmp(device_status,"IN_USE")==0)
		status_case=IN_USE;

	return status_case;

}

int GetDeviceStatus(int devstatus,char* device_status)
{
	int status_case=0;
	switch(devstatus)
	{
	case DESTROYED:
		strcpy(device_status,"DESTROYED");
		return status_case;
	case SPARE:
		strcpy(device_status,"SPARE");
		return status_case;
	case IN_REPAIR:
		strcpy(device_status,"IN_REPAIR");
		return status_case;
	case TEST:
		strcpy(device_status,"TEST");
		return status_case;
	case EXT_TEST:
		strcpy(device_status,"EXT_TEST");
		return status_case;
	case IN_USE:
		strcpy(device_status,"IN_USE");
		return status_case;
	case NOT_AVAILABLE:
		strcpy(device_status,"NONE");
		return status_case;
	default :
		strcpy(device_status,"INVALID");
		return -1;

	}

}
//////////////
int DefineByPos(OCIStmt* stmthp,OCIDefine** def, OCIError* ociError, int pos,  dvoid* valuep, sb4 value_sz, ub2 dtype, sword* status){
	Error err;
	*status=OCIDefineByPos (stmthp, &def[pos-1],ociError, pos,valuep,value_sz, dtype, 0, 0, 0, OCI_DEFAULT);
	if(*status==OCI_SUCCESS_WITH_INFO){
		ShowErrors (*status, ociError, "") ;
	}

	if(*status != OCI_SUCCESS && *status != OCI_SUCCESS_WITH_INFO){	
		err.ociError=ociError;
		sprintf(err.log,"OCIDefineByPos (pos %i) unsuccessful",pos);
		err.msg=" ";
		throw err;
	}
	return *status;
}
int BindByName(OCIStmt* stmthp,OCIBind** bnd1p, OCIError* ociError, char* placeholder, dvoid* valuep,sb4 value_sz,ub2 dty,int* indp, sword* status){
	Error err;
	*status=OCIBindByName(stmthp, bnd1p, ociError,(text*)placeholder,-1, valuep,value_sz, dty, (dvoid*)indp,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);
	if(*status==OCI_SUCCESS_WITH_INFO){
		ShowErrors (*status, ociError, "") ;
	}

	if(*status != OCI_SUCCESS && *status != OCI_SUCCESS_WITH_INFO){	
		err.ociError=ociError;
		sprintf(err.log,"OCIBindByName unsuccessful");
		err.msg=" ";
		throw err;
	}
	return *status;
}
int HandleAlloc(OCIEnv* ociEnv,dvoid** stmthp,ub4 type, sword* status){
	Error err;
	*status=OCIHandleAlloc (ociEnv, stmthp ,type, 0, 0);

	if(*status != OCI_SUCCESS && *status != OCI_SUCCESS_WITH_INFO){	
		err.ociError=0;
		sprintf(err.log,"OCIHandleAlloc unsuccessful");
		err.msg="NOT CONNECTED TO ANY DB";
		throw err;
	}	
	return *status;
}
int StmtPrepare(OCIStmt* stmthp,OCIError* ociError, char* stmt, sword* status){
	Error err;
	*status=OCIStmtPrepare(stmthp, ociError, (text*) (stmt),(ub4) strlen(stmt),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
	if(*status==OCI_SUCCESS_WITH_INFO){
		ShowErrors (*status, ociError, "") ;
	}

	if(*status != OCI_SUCCESS && *status != OCI_SUCCESS_WITH_INFO){	
		err.ociError=ociError;
		sprintf(err.log,"OCIStmtPrepare unsuccessful");
		err.msg=" ";
		throw err;
	}	
	return *status;
}
int StmtExecute(OCISvcCtx* ociHdbc, OCIStmt* stmthp, OCIError* ociError,ub4 iters, sword* status){
	Error err;
	*status=OCIStmtExecute(ociHdbc, stmthp, ociError, iters, 0,(OCISnapshot *) 0, (OCISnapshot *) 0, OCI_DEFAULT);
	if(*status==OCI_SUCCESS_WITH_INFO){
		ShowErrors (*status, ociError, "") ;
	}

	if(*status != OCI_SUCCESS && *status != OCI_SUCCESS_WITH_INFO){	
		err.ociError=ociError;
		sprintf(err.log,"OCIStmtExecute unsuccessful");
		err.msg=" ";
		throw err;
	}	
	return *status;
}
int TransCommit(OCISvcCtx *svchp,OCIError *errhp, ub4 flags, sword* status ){
	Error err;
	*status=OCITransCommit(svchp, errhp, flags);
	if(*status==OCI_SUCCESS_WITH_INFO){
		ShowErrors (*status, errhp, "") ;
	}

	if(*status != OCI_SUCCESS && *status != OCI_SUCCESS_WITH_INFO){	
		err.ociError=errhp;
		sprintf(err.log,"OCISTransCommit unsuccessful");
		err.msg=" ";
		throw err;
	}	
	return *status;
}
int ParamGet(OCIStmt* stmthp, OCIError* ociError, OCIParam** parmdp, int pos, sword* status){
	Error err;
	*status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **)parmdp, (ub4) pos);
	if(*status==OCI_SUCCESS_WITH_INFO){
		ShowErrors (*status, ociError, "") ;
	}

	if(*status != OCI_SUCCESS && *status != OCI_SUCCESS_WITH_INFO){	
		err.ociError=ociError;
		sprintf(err.log,"OCIParamGet unsuccessful");	
		err.msg=" ";
		throw err;
	}	
	return *status;
}
int AttrGet(OCIParam* parmdp, sb4* attrval, OCIError* ociError, sword* status){
	Error err;
	*status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) attrval, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);
	if(*status==OCI_SUCCESS_WITH_INFO){
		ShowErrors (*status, ociError, "") ;
	}

	if(*status != OCI_SUCCESS && *status != OCI_SUCCESS_WITH_INFO){	
		err.ociError=ociError;
		sprintf(err.log,"OCIAttrGet unsuccessful");
		err.msg=" ";
		throw err;
	}	
	return *status;
}
//////////////////////////////
void RemoveSeparator(char* attribute_value, char* separator)
{
	int pos;
	pos=strcspn(attribute_value,separator);
	attribute_value[pos]='\0';
}
//////////////////////////////
//from connection:
//////////////////////////////
int EnvCreate(OCIEnv*& myenvhp, sword* status){
	Error err;
	*status=OCIEnvCreate(&myenvhp, OCI_DEFAULT, (dvoid *)0,0, 0, 0, (size_t) 0, (dvoid **)0);
	if(*status != OCI_SUCCESS && *status != OCI_SUCCESS_WITH_INFO){	
		sprintf(err.log,"OCIEnvCreate unsuccessful");
		err.msg="";
		throw err;
	}
	return *status;
}
int ServerAttach(OCIServer* srvhp,OCIError* errhp,char* dblink, sword* status){
	Error err;
	*status=OCIServerAttach (srvhp, errhp,(text*) dblink,strlen(dblink), OCI_DEFAULT);
	if(*status==OCI_SUCCESS_WITH_INFO){
		ShowErrors (*status, errhp, "") ;
	}

	if(*status != OCI_SUCCESS && *status != OCI_SUCCESS_WITH_INFO){	
		sprintf(err.log,"OCIServerAttach unsuccessful");
		err.msg="";
		throw err;
	}
	return *status;
}
int AttrSet (dvoid* hndlp,ub4 hndltype,dvoid* attributep, ub4 sizep, ub4 attrtype,OCIError* ociError, sword* status){
	Error err;
	*status=OCIAttrSet (hndlp,hndltype,attributep, sizep, attrtype, ociError);
	if(*status==OCI_SUCCESS_WITH_INFO){
		ShowErrors (*status, ociError, "") ;
	}

	if(*status != OCI_SUCCESS && *status != OCI_SUCCESS_WITH_INFO){	
		err.ociError=ociError;
		sprintf(err.log,"OCIAttrSet unsuccessful");
		err.msg=" ";
		throw err;
	}	
	return *status;
}
int SessionBegin(OCISvcCtx *svchp,OCIError *errhp,OCISession *usrhp,ub4 credt,sword* status){
	Error err;
	*status=OCISessionBegin(svchp, errhp, usrhp,credt, OCI_DEFAULT);
	if(*status==OCI_SUCCESS_WITH_INFO){
			ShowErrors (*status, errhp, "") ;
		}
	if(*status != OCI_SUCCESS && *status != OCI_SUCCESS_WITH_INFO){	
		err.ociError=errhp;
		sprintf(err.log,"OCISessionBegin unsuccessful");
		err.msg=" ";
		throw err;
	}	
	return *status;
}
int BindArrayOfStruct(OCIBind *bindp,OCIError *errhp,ub4 pvskip,ub4 indskip,sword* status ){
	Error err;
	*status=OCIBindArrayOfStruct(bindp,errhp,pvskip,indskip,0,0);
	if(*status==OCI_SUCCESS_WITH_INFO){
			ShowErrors (*status, errhp, "") ;
		}
	if(*status != OCI_SUCCESS && *status != OCI_SUCCESS_WITH_INFO){	
		err.ociError=errhp;
		sprintf(err.log,"OCIBindArrayOfStruct unsuccessful");
		err.msg=" ";
		throw err;
	}	
	return *status;
}
/////////////////////////////
int null_charptr(char* ptr_name, int* len)
{
	*len=0;
	int value_ptr=0;
	if(ptr_name==NULL)
		value_ptr=-1;
	else *len=strlen(ptr_name)+1;

	return value_ptr;

}
