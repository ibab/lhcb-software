/********************************************************************************/
//	Author: L.Abadie
// version:v3.5
// changes: handling error such that can be extended to C++: (create a class SQLException) (strlen ck + update stmt)
// file which performs bulk collect insert for the devicetype table
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


	/************global variables for the devicetype*******************************/ 
	int FIRST_TIME_DEVICETYPE=0;
	char* _DeviceTypeList;  //the list of the devicetype which will be stored
	int _devicetypeListLength; //the total length of the char*
	int _max_devicetypeLength; //the maximum length of the devicetype as a string
	char* _DescriptionList;
	int _descriptionListLength;
	int _max_descriptionLength;
	int* _NbrofinputList;
	int* _NbrofoutputList;
	int _devicetypeNbElement;// total nb of devicetype to insert
	int _descNbElement;
	int _rgbcolorNbElement;
	int _nbinElement;
	int _nboutElement;
	char* _rgbcolorList;
	int _rgbcolorListLength;
	int _max_rgbcolorLength;

	int _sysIDNbElement;
	int* _sysIDList1;

	int* _devicetype_nullvalue;
	int _devicetypeNbEl;

	int* _description_nullvalue;
	int _descriptionNbEl;

	int* _rgbcolor_nullvalue;
	int _rgbcolorNbEl;


	int freeDeviceType()
	{
		int status=0;
		_DeviceTypeList=(char*)realloc(_DeviceTypeList,0*sizeof(char));
		_DeviceTypeList=NULL;
		_rgbcolorList=(char*)realloc(_rgbcolorList,0*sizeof(char));
		_rgbcolorList=NULL;
		_DescriptionList=(char*)realloc(_DescriptionList,0*sizeof(char));
		_DescriptionList=NULL;
		_NbrofinputList=(int*)realloc(_NbrofinputList,0*sizeof(int));
		_NbrofinputList=NULL;
		_NbrofoutputList=(int*)realloc(_NbrofoutputList,0*sizeof(int));
		_NbrofoutputList=NULL;
		_description_nullvalue=(int*)realloc(_description_nullvalue,0*sizeof(int));
		_description_nullvalue=NULL;
		_devicetype_nullvalue=(int*)realloc(_devicetype_nullvalue,0*sizeof(int));
		_devicetype_nullvalue=NULL;
		_rgbcolor_nullvalue=(int*)realloc(_rgbcolor_nullvalue,0*sizeof(int));
		_rgbcolor_nullvalue=NULL;
		FIRST_TIME_DEVICETYPE=0;
		_sysIDList1=(int*)realloc(_sysIDList1,0*sizeof(int));
		_sysIDList1=NULL;

		return status;
	}


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
		int InsertMultipleDeviceTypes(char* systemnameList,char* devicetype,int nbrofinput,int nbrofoutput, char* description,char* rgbcolor,int first_time1, int last_rows1,char* ErrMess)
	{
		char appliName[100]="InsertMultipleDeviceTypes";
		char sqlstmt[1000];
		OCIStmt *hstmt;
		int numrows=0;
		OCIBind  *bndp3 = (OCIBind *) 0;
		OCIBind  *bndp[6]; 
		int rescode=0;
		int free_mem=0;
		//int descrip_len=0;
		//int devtype_len=0;
		//int rgbcol_len=0;
		//int in_len1=1*4;
		//int out_len1=1*4;
		int force_insert=0;
		int first_time=first_time1;
		int i=0;
		int last_rows=last_rows1;
		int res_query=0;
		sword status=0;
		char* dtype=NULL;
		char* rgbcol=NULL;
		char* descrip=NULL;
		int* numrows_inserted=NULL;
		int dtype1=null_charptr(devicetype);
		int descrip1=null_charptr(description);
		int rgbcol1=null_charptr(rgbcolor);
		char seq_name[100]="lhcb_devtype_seq.nextval";
		//char subsystem_name[10];
		//strncpy(subsystem_name,systemname,9);
		//int pos5=strcspn(subsystem_name,"_");
		//subsystem_name[pos5]='\0';

		if(FIRST_TIME_DEVICETYPE==1 && _DeviceTypeList==NULL)
		{
			first_time=1;

		}
		int sysID=GetSubsystemID(systemnameList);
		if(sysID==-1)
		{
			GetErrorMess(appliName, "systemnameList is null or check the spelling",ErrMess,1);

			return -1;
		}
		if(first_time==1)
		{
			status+=freeDeviceType();
			FIRST_TIME_DEVICETYPE=1;

		}
		res_query=AppendString(devicetype,_DeviceTypeList,_devicetypeListLength,_devicetypeNbElement,_max_devicetypeLength,first_time);
		res_query+=AppendString(description,_DescriptionList,_descriptionListLength,_descNbElement,_max_descriptionLength,first_time);
		res_query+=AppendString(rgbcolor,_rgbcolorList,_rgbcolorListLength,_rgbcolorNbElement,_max_rgbcolorLength,first_time);

		res_query+=AppendInt(nbrofinput,_NbrofinputList,_nbinElement,first_time);
		res_query+=AppendInt(nbrofoutput,_NbrofoutputList,_nboutElement,first_time);
		res_query+=AppendInt(sysID,_sysIDList1,_sysIDNbElement,first_time);


		res_query+=AppendInt(descrip1,_description_nullvalue,_descriptionNbEl,first_time);
		res_query+=AppendInt(dtype1,_devicetype_nullvalue,_devicetypeNbEl,first_time);
		res_query+=AppendInt(rgbcol1,_rgbcolor_nullvalue,_rgbcolorNbEl,first_time);


		if(last_rows!=1 && _devicetypeNbElement==MAXROWS)
		{
			force_insert=1;
			last_rows=1;
		}
		if(last_rows==1 && res_query==0 && ociEnv!=0 && FIRST_TIME_DEVICETYPE==1)
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
			//std::cout<<"end cache before"<<login<<" host before"<<host<<std::endl;
			//std::cout<<" errmessg1 "<<errmessg1<<" errmessg2 "<<errmessg2<<std::endl;

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
				status+=freeDeviceType();
				if(dtype!=NULL)
					free(dtype);
				if(descrip!=NULL)
					free(descrip);
				if(rgbcol!=NULL)
					free(rgbcol);
				free(errmessg1);
				free(errmessg2);
				return -1;
			}
			//need to proceed with messages
			//std::cout<<"end cache "<<login<<" host "<<host<<std::endl;


			status =OCIHandleAlloc (ociEnv, (void**)&hstmt, OCI_HTYPE_STMT , 0, 0);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIHandleAlloc unsuccessful");
			}
			else
			{
				sprintf(sqlstmt,"BEGIN insert into %s(devicetypeID,devicetype,nbrofinput,nbrofoutput,description,rgbcolor,author,user_update,terminal_name,system_name) values(%s,:dtype,:din,:dout,:ddescr,:rgbcol,'%s','%s','%s',:sysID); :numrows:=%s; END;",DEVICETYPE_TABLE,seq_name,login,login,host,SQLROWCOUNT);
				status=OCIStmtPrepare(hstmt, ociError, (text*)sqlstmt, (ub4)strlen((char *)sqlstmt),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
			}

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
			}
			else
			{
				//devtype_len=strlen(devicetype);
				//descrip_len=strlen(description);
				//rgbcol_len=strlen(rgbcolor);
				//in_len=sizeof(nbrofinput);
				//out_len=sizeof(nbrofoutput);
				dtype=(char*)malloc(_devicetypeNbElement*_max_devicetypeLength*sizeof(char));
				if(dtype!=NULL)
					status+=NormalizeVector(_DeviceTypeList, _devicetypeNbElement,_max_devicetypeLength,dtype);
				descrip=(char*)malloc(_descNbElement*_max_descriptionLength*sizeof(char));
				if(descrip!=NULL)
					status+=NormalizeVector(_DescriptionList, _descNbElement,_max_descriptionLength,descrip);
				rgbcol=(char*)malloc(_rgbcolorNbElement*_max_rgbcolorLength*sizeof(char));
				if(rgbcol!=NULL)
					status+=NormalizeVector(_rgbcolorList, _rgbcolorNbElement,_max_rgbcolorLength,rgbcol);

				numrows_inserted=(int*)malloc(sizeof(int)*_devicetypeNbElement);
				//std::cout<<"dtype="<<dtype<<" and"<<std::endl;
				//std::cout<<"value of _DeviceTypeList="<<_DeviceTypeList<<" and"<<std::endl;
				//std::cout<<"value of _max_devicetypeLength="<<_max_devicetypeLength<<" and"<<std::endl;
				//std::cout<<"value of _devicetypeNbElement="<<_devicetypeNbElement<<" and"<<std::endl;
				if(dtype==NULL || descrip==NULL||numrows_inserted==NULL||rgbcol==NULL)
				{

					status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
					rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");
					GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
					status+=freeDeviceType();
					if(dtype!=NULL)
						free(dtype);
					if(descrip!=NULL)
						free(descrip);
					if(rgbcol!=NULL)
						free(rgbcol);
					if(numrows_inserted!=NULL)
						free(numrows_inserted);
					return -1;
				}
				else
				{
					for(i=0;i<_devicetypeNbElement;i++)
						numrows_inserted[i]=0;
					status =OCIBindByName(hstmt, &bndp[0], ociError,(text*) ":dtype", -1,(dvoid*)dtype, _max_devicetypeLength,  SQLT_STR, (dvoid *) &_devicetype_nullvalue[0],(ub2 *)0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
				}
			}
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status = OCIBindByName (hstmt, &bndp[1], ociError,(text*) ":din", -1,(dvoid*)&_NbrofinputList[0],sizeof(nbrofinput), SQLT_INT, 0, 0, 0, 0, 0, OCI_DEFAULT);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status = OCIBindByName (hstmt, &bndp[2], ociError, (text*) ":dout", -1,(dvoid*)&_NbrofoutputList[0],sizeof(nbrofoutput),SQLT_INT, 0, 0, 0, 0, 0, OCI_DEFAULT);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status = OCIBindByName(hstmt, &bndp[3], ociError, (text*) ":ddescr", -1,(dvoid*)descrip,_max_descriptionLength, SQLT_STR,(dvoid *)  &_description_nullvalue[0], (ub2*)0, 0, 0, 0, OCI_DEFAULT);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status = OCIBindByName(hstmt, &bndp[4], ociError, (text*) ":rgbcol", -1,(dvoid*)rgbcol,_max_rgbcolorLength, SQLT_STR,(dvoid *)  &_rgbcolor_nullvalue[0], (ub2*)0, 0, 0, 0, OCI_DEFAULT);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status = OCIBindByName(hstmt, &bndp[5], ociError, (text*) ":sysID", -1,(dvoid*)&_sysIDList1[0],sizeof(int), SQLT_INT,(dvoid *)  0, (ub2*)0, 0, 0, 0, OCI_DEFAULT);


			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp3, ociError,(text*) ":numrows", -1,(dvoid*)&numrows_inserted[0], sizeof(nbrofoutput),SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);


			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else	
				status=OCIBindArrayOfStruct(bndp[0], ociError, _max_devicetypeLength, sizeof(int),0, 0);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp[1], ociError, sizeof(int), 0, 0, 0);

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
				status=OCIBindArrayOfStruct(bndp[3], ociError, _max_descriptionLength, sizeof(int), 0, 0);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp[4], ociError, _max_rgbcolorLength, sizeof(int), 0, 0);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp[5], ociError, sizeof(int), 0,0, 0);

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
				status= OCIStmtExecute(ociHdbc, hstmt, ociError, _devicetypeNbElement, 0, 0, 0, OCI_DEFAULT );

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
			}
			else
			{
				numrows=-1;
				for(i=0;i< _devicetypeNbElement;i++)
				{
					if(numrows_inserted[i]==0)
					{
						numrows=0;
						i=  _devicetypeNbElement+5;
					}
					else
						numrows=numrows_inserted[i];
				}

				if(numrows==0)
				{
					status = OCITransCommit(ociHdbc, ociError, 0);
					status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
					status+=freeDeviceType();
					if(dtype!=NULL)
						free(dtype);
					if(rgbcol!=NULL)
						free(rgbcol);
					if(descrip!=NULL)
						free(descrip);
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
			status+=freeDeviceType();
			if(rescode!=0)
				if(ociError!=0)
					OCIReportError(ociError,appliName, ErrMess,1); 
				else
					GetErrorMess(appliName,  "NOT CONNECTED TO ANY DB",ErrMess,1); 
			else
				OCIReportError(ociError,appliName, ErrMess,0);  

			if(dtype!=NULL)
				free(dtype);

			if(descrip!=NULL)
				free(descrip);
			if(rgbcol!=NULL)
				free(rgbcol);
			if(numrows_inserted!=NULL)
				free(numrows_inserted);
			if(force_insert==1 && rescode==0)
			{
				FIRST_TIME_DEVICETYPE=1;
				force_insert=0;
			}
			//std::cout<<"res_query not null "<<res_query<<" rescode "<<rescode<<std::endl;

		}
		else
		{
			//std::cout<<"before if res_query null "<<res_query<<" rescode "<<rescode<<std::endl;

			if(res_query!=0)
			{
				status=freeDeviceType();
				GetErrorMess(appliName, "Cache Problem",ErrMess,1);
			}
			if(FIRST_TIME_DEVICETYPE!=1)
			{
				status=freeDeviceType();
				res_query=-1;
				GetErrorMess(appliName, "CACHE HAS NOT BEEN INITIALIZED",ErrMess,1);
			}
			if(ociEnv==0)
			{
				status=freeDeviceType();
				res_query=-1;
				GetErrorMess(appliName, "NOT CONNECTED TO ANY DB",ErrMess,1);
			}
			status+=res_query;
			if(ociEnv!=0 && FIRST_TIME_DEVICETYPE==1 && res_query==0)
			{
				status=0;
				GetErrorMess(appliName, "NO_ERROR",ErrMess,0);
			}
			//std::cout<<"res_query null "<<res_query<<" rescode "<<rescode<<std::endl;


		}
		return (rescode+status);
	}

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif
