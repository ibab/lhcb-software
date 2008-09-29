/********************************************************************************/
//	Author: L.Abadie
// version:v3.5
// changes: handling error such that can be extended to C++: (create a class SQLException) (strlen ck + update stmt)
// file which performs bulk collect insert for the port table
// N.B : Memory management (allocation+release) should be handled by the user application thus, for all the fct provided by this lib
/********************************************************************************/

#include "list_structures.h"
#include "system_info.h"
#include "CDB.h"
#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

#if defined( _WIN32) || defined(WIN32)
#define EXTERN_CONFDB _declspec(dllexport)
#else
#define EXTERN_CONFDB extern
#endif

	extern   OCIError* ociError ;
	extern   OCIEnv* ociEnv ;
	extern   OCISvcCtx* ociHdbc ;
	extern   OCIServer *mysrvhp;
	extern   OCISession *myusrhp; 

/**
* Upadte Multiple Ports, returning an integer value.
* The user should manage the memory : there is no memory allocation.
* @param devicename : name of the device which will be updated
* @param port_nb   : port nb of the device to update
* @param port_way   : specify the way of the port to update :1 if the port is an input and 2 if it's an output 
* @param port_type   : specify the type of the port of the device to update. Put "" if there is none.
* @param speed   : new value of speed of the port: PUT -1 to keep the same value
* @param pxi_booting : new value of pxi_booting 1 if the port is the pxi_booting one, 0 otherwise. Put a value different from 1 or 0 to keep the same value.
* @param phy: new value of phy possible values :"SX","T","SL". if no changes put "none"
* @param first_time1 : 1 if it's your first Port insert (save it into the database), 0 otherwise   
* @param last_rows1 : 1 if it's your last Port insert (save it into the database), 0 otherwise
* @param ErrMess : error message in case of failure
* @return 0 if it is successful
*/ 
EXTERN_CONFDB
	int UpdateMultiplePorts(char* devicename,char* port_nb,int port_way,char* port_type,int speed,char* phy,int pxi_booting,int first_time,int last_rows,char* ErrMess)
	{
		char appliName[100]="UpdateMultiplePorts";
		char sqlstmt[1000];
		OCIStmt *hstmt;
		OCIBind  *bndp[8]; 
		int rescode=0;
		int res_query=0;
		sword status=0;
		int i=0;

		char* devname=NULL;
		char* dphy=NULL;
		char* portype=NULL;
		char* portnb=NULL;
		int* portway=NULL;
		int* pspeed=NULL;
		int* pxibooting=NULL;

		int* devname_nullList=NULL;
		int* dphy_nullList=NULL;
		int* portype_nullList=NULL;
		int* portnb_nullList=NULL;

		static int FIRST_TIME=0;
		int force_insert=0;
		int* numrows_inserted=NULL;
		int numrows=0;

		static int NbElement;
		UpdtPort** portList;
		int max_devnamelen=0;
		int max_dphylen=0;
		int max_portypelen=0;
		int max_portnblen=0;

		if (first_time==1)
		{
			FIRST_TIME=1;
			NbElement=1;
			portList=(UpdtPort**)malloc( NbElement*sizeof(UpdtPort));			
		}
		else 
		{
			if (FIRST_TIME==0&&force_insert==0)
			{
				GetErrorMess(appliName, "CACHE HAS NOT BEEN INITIALIZED",ErrMess,1);
				return -1;
			}
			NbElement++;
			portList=(UpdtPort**)realloc(portList, NbElement*sizeof(UpdtPort));
		}
		if(port_type==NULL || strlen(port_type)==0)
			port_type="none";
		
		portList[NbElement-1]=new UpdtPort(devicename,port_nb,port_way,port_type,speed,phy, pxi_booting);

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

				if(devname!=NULL)
					free(devname);
				if(dphy!=NULL)
					free(dphy);
				if(portype!=NULL)
					free(portype);
				if(portnb!=NULL)
					free(portnb);
				if(portway!=NULL)
					free(portway);
				if(pspeed!=NULL)
					free(pspeed);
				if(pxibooting!=NULL)
					free(pxibooting);
				if(numrows_inserted!=NULL)
					free(numrows_inserted);
				if(devname_nullList!=NULL)
					free(devname_nullList);
				if(dphy_nullList!=NULL)
					free(dphy_nullList);
				if(portype_nullList!=NULL)
					free(portype_nullList);
				if(portnb_nullList!=NULL)
					free(portnb_nullList);

				for(i=0;i<NbElement;i++)
					delete portList[i];
				if(portList!=NULL)
					free(portList);
				
				free(errmessg1);
				free(errmessg2);
				return -1;				
			}

			for(i=0;i<NbElement;i++){
				if (portList[i]->devnamelen > max_devnamelen)
					max_devnamelen=portList[i]->devnamelen;
				if (portList[i]->dphylen > max_dphylen)
					max_dphylen=portList[i]->dphylen;
				if (portList[i]->portypelen > max_portypelen)
					max_portypelen=portList[i]->portypelen;
				if (portList[i]->portnblen > max_portnblen)
					max_portnblen=portList[i]->portnblen;				
			}

			devname=(char*)malloc(NbElement*max_devnamelen*sizeof(char));
			dphy=(char*)malloc(NbElement*max_dphylen*sizeof(char));
			portype=(char*)malloc(NbElement*max_portypelen*sizeof(char));
			portnb=(char*)malloc(NbElement*max_portnblen*sizeof(char));

			portway=(int*)malloc(sizeof(int)*NbElement);
			pspeed=(int*)malloc(sizeof(int)*NbElement);
			pxibooting=(int*)malloc(sizeof(int)*NbElement);

			devname_nullList=(int*)malloc(NbElement*sizeof(int));
			dphy_nullList=(int*)malloc(NbElement*sizeof(int));
			portype_nullList=(int*)malloc(NbElement*sizeof(int));
			portnb_nullList=(int*)malloc(NbElement*sizeof(int));
			
			if(devname!=NULL && dphy!=NULL && portype!=NULL && portnb!=NULL && portway!=NULL && pspeed!=NULL && pxibooting!=NULL 
				&& devname_nullList!=NULL && dphy_nullList!=NULL && portype_nullList!=NULL && portnb_nullList!=NULL)
			for(i=0;i<NbElement;i++)
			{
				memcpy(devname+i*max_devnamelen,portList[i]->devname,portList[i]->devnamelen);
				devname_nullList[i]=portList[i]->devname_null;				
				memcpy(dphy+i*max_dphylen,portList[i]->dphy,portList[i]->dphylen);
				dphy_nullList[i]=portList[i]->dphy_null;
				memcpy(portype+i*max_portypelen,portList[i]->portype,portList[i]->portypelen);
				portype_nullList[i]=portList[i]->portype_null;
				memcpy(portnb+i*max_portnblen,portList[i]->portnb,portList[i]->portnblen);
				portnb_nullList[i]=portList[i]->portnb_null;

				portway[i]=portList[i]->portway;
				pspeed[i]=portList[i]->pspeed;
				pxibooting[i]=portList[i]->pxibooting;
			}

			numrows_inserted=(int*)malloc(sizeof(int)*NbElement);

			if(devname==NULL || dphy==NULL || portype==NULL || portnb==NULL || portway==NULL || pspeed==NULL || pxibooting==NULL
				|| devname_nullList==NULL || dphy_nullList==NULL || portype_nullList==NULL || portnb_nullList==NULL || numrows_inserted==NULL)
			{
				rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");
				GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
				
				FIRST_TIME=0;
				if(devname!=NULL)
					free(devname);
				if(dphy!=NULL)
					free(dphy);
				if(portype!=NULL)
					free(portype);
				if(portnb!=NULL)
					free(portnb);
				if(portway!=NULL)
					free(portway);
				if(pspeed!=NULL)
					free(pspeed);
				if(pxibooting!=NULL)
					free(pxibooting);
				if(numrows_inserted!=NULL)
					free(numrows_inserted);
				if(devname_nullList!=NULL)
					free(devname_nullList);
				if(dphy_nullList!=NULL)
					free(dphy_nullList);
				if(portype_nullList!=NULL)
					free(portype_nullList);
				if(portnb_nullList!=NULL)
					free(portnb_nullList);
				
				for(i=0;i<NbElement;i++)
					delete portList[i];
				if(portList!=NULL)
                    free(portList);
				
				NbElement=0;				
				return -1;
			}
			else 
				for(i=0;i<NbElement;i++)
					numrows_inserted[i]=0;

			try{
                HandleAlloc(ociEnv,(dvoid**)&hstmt,OCI_HTYPE_STMT,&status);
				sprintf(sqlstmt,"BEGIN update %s set speed=decode(nvl(:speed,-1),-1,speed,:speed),phy=decode(nvl(:phy,'none'),'none',phy,:phy),pxi_booting=decode(:pxi_booting,1,1,0,0,pxi_booting),last_update=sysdate,user_update='%s' where deviceid=(select deviceid from %s where devicename=:dname) and port_nbr=:port_nbr and port_way=:port_way and nvl(port_type,'none')=:port_type; :numrows:=%s; END;",PORT_TABLE,login,LOGICAL_DEVICE_TABLE,SQLROWCOUNT);
				StmtPrepare(hstmt,ociError, sqlstmt, &status);

				BindByName(hstmt,&bndp[0],ociError,":port_nbr",portnb,max_portnblen,SQLT_STR,&portnb_nullList[0],&status);
				BindByName(hstmt,&bndp[1],ociError,":speed",&pspeed[0],sizeof(&portList[0]->pspeed),SQLT_INT,0,&status);
				BindByName(hstmt,&bndp[2],ociError,":port_way",&portway[0],sizeof(&portList[0]->portway),SQLT_INT,0,&status);
				BindByName(hstmt,&bndp[3],ociError,":port_type",portype,max_portypelen,SQLT_STR,&portype_nullList[0],&status);
				BindByName(hstmt,&bndp[4],ociError,":phy",dphy,max_dphylen,SQLT_STR,&dphy_nullList[0],&status);
				BindByName(hstmt,&bndp[5],ociError,":pxi_booting",&pxibooting[0],sizeof(&portList[0]->pxibooting),SQLT_INT,0,&status);
				BindByName(hstmt,&bndp[6],ociError,":dname",devname,max_devnamelen,SQLT_STR,&devname_nullList[0],&status);
				BindByName(hstmt,&bndp[7],ociError,":numrows",&numrows_inserted[0],sizeof(int),SQLT_INT,0,&status);

				BindArrayOfStruct(bndp[0],ociError,max_portnblen,sizeof(int),&status);
				BindArrayOfStruct(bndp[1],ociError,sizeof(int),0,&status);
				BindArrayOfStruct(bndp[2],ociError,sizeof(int),0,&status);
				BindArrayOfStruct(bndp[3],ociError,max_portypelen,sizeof(int),&status);
				BindArrayOfStruct(bndp[4],ociError,max_dphylen,sizeof(int),&status);
				BindArrayOfStruct(bndp[5],ociError,sizeof(int),0,&status);
				BindArrayOfStruct(bndp[6],ociError,max_devnamelen,sizeof(int),&status);
				BindArrayOfStruct(bndp[7],ociError,sizeof(int),0,&status);

				StmtExecute(ociHdbc, hstmt, ociError,NbElement, &status);
			
			}catch(Error err){
				sprintf(appliName,"UpdateMultiplePorts");	///////
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
				
				if(devname!=NULL)
					free(devname);
				if(dphy!=NULL)
					free(dphy);
				if(portype!=NULL)
					free(portype);
				if(portnb!=NULL)
					free(portnb);
				if(portway!=NULL)
					free(portway);
				if(pspeed!=NULL)
					free(pspeed);
				if(pxibooting!=NULL)
					free(pxibooting);
				if(numrows_inserted!=NULL)
					free(numrows_inserted);
				if(devname_nullList!=NULL)
					free(devname_nullList);
				if(dphy_nullList!=NULL)
					free(dphy_nullList);
				if(portype_nullList!=NULL)
					free(portype_nullList);
				if(portnb_nullList!=NULL)
					free(portnb_nullList);
				
				for(i=0;i<NbElement;i++)
					delete portList[i];
				if(force_insert==1)
				{
                    FIRST_TIME=1;
					force_insert=0;
					portList=(UpdtPort**)realloc(portList, 0*sizeof(UpdtPort));
				}
				else if(portList!=NULL)
                    free(portList);

				NbElement=0;

				GetErrorMess(appliName, "COULDNOT UPDATE ALL ROWS",ErrMess,1);
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
				delete portList[i];

			if(force_insert==1)
			{
				FIRST_TIME=1;
				force_insert=0;
				portList=(UpdtPort**)realloc(portList, 0*sizeof(UpdtPort));
			}
			else if(portList!=NULL)
                free(portList);
			NbElement=0;
		}
		else
		{
			if(res_query!=0)
			{
				FIRST_TIME=0;
				GetErrorMess(appliName, "Cache Problem",ErrMess,1);
				for(i=0;i<NbElement;i++)
					delete portList[i];
				if(portList!=NULL)
                    free(portList);
			}
			if(ociEnv==0)
			{
				FIRST_TIME=0;
				res_query=-1;
				GetErrorMess(appliName, "NOT CONNECTED TO ANY DB",ErrMess,1);
				for(i=0;i<NbElement;i++)
					delete portList[i];
				if(portList!=NULL)
                    free(portList);
			}
			status+=res_query;
			if(ociEnv!=0 && FIRST_TIME==1 && res_query==0)
			{
				status=0;
				GetErrorMess(appliName, "NO_ERROR",ErrMess,0);
			}	
		}
		if(devname!=NULL)
			free(devname);
		if(dphy!=NULL)
			free(dphy);
		if(portype!=NULL)
			free(portype);
		if(portnb!=NULL)
			free(portnb);
		if(portway!=NULL)
			free(portway);
		if(pspeed!=NULL)
			free(pspeed);
		if(pxibooting!=NULL)
			free(pxibooting);
		if(numrows_inserted!=NULL)
			free(numrows_inserted);
		if(devname_nullList!=NULL)
			free(devname_nullList);
		if(dphy_nullList!=NULL)
			free(dphy_nullList);
		if(portype_nullList!=NULL)
			free(portype_nullList);
		if(portnb_nullList!=NULL)
			free(portnb_nullList);

		return (rescode+status);
	}

/**
* Upadte Multiple Ports status, returning an integer value.
* The user should manage the memory : there is no memory allocation.
* @param devicename : name of the device which will be updated
* @param port_nb   : port nb of the device to update
* @param port_way   : specify the way of the port to update :1 if the port is an input and 2 if it's an output 
* @param port_type   : specify the type of the port of the device to update. Put "" if there is none.
* @param admin_status   : new value of speed of the port: PUT -1 to keep the same value
* @param first_time1 : 1 if it's your first Port insert (save it into the database), 0 otherwise   
* @param last_rows1 : 1 if it's your last Port insert (save it into the database), 0 otherwise
* @param ErrMess : error message in case of failure
* @return 0 if it is successful
*/ 
EXTERN_CONFDB
	int UpdateMultiplePortStatuses(char* devicename,char* port_nb,int port_way,char* port_type,int admin_status,int first_time,int last_rows,char* ErrMess)
	{
		char appliName[100]="UpdateMultiplePortStatuses";
		char sqlstmt[1000];
		OCIStmt *hstmt;
		OCIBind  *bndp[7]; 
		int rescode=0;
		int res_query=0;
		sword status=0;
		int i=0;

		char* devname=NULL;
		char* portype=NULL;
		char* portnb=NULL;
		int* adminstatus=NULL;
		int* portway=NULL;

		int* devname_nullList=NULL;
		int* portype_nullList=NULL;
		int* portnb_nullList=NULL;

		static int FIRST_TIME=0;
		int force_insert=0;
		int* numrows_inserted=NULL;
		int* portid_list=NULL;
		int numrows=0;

		static int NbElement;
		UpdtPort** portList;
		int max_devnamelen=0;
		int max_portypelen=0;
		int max_portnblen=0;

		if (first_time==1)
		{
			FIRST_TIME=1;
			NbElement=1;
			portList=(UpdtPort**)malloc( NbElement*sizeof(UpdtPort));
		}
		else 
		{
			if (FIRST_TIME==0&&force_insert==0)
			{
				GetErrorMess(appliName, "CACHE HAS NOT BEEN INITIALIZED",ErrMess,1);
				return -1;
			}
			NbElement++;
			portList=(UpdtPort**)realloc(portList, NbElement*sizeof(UpdtPort));
		}

		if(port_type==NULL || strlen(port_type)==0)
			port_type="none";

		portList[NbElement-1]=new UpdtPort(devicename,port_nb,port_way,port_type,admin_status);

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

				if(devname!=NULL)
					free(devname);
				if(portype!=NULL)
					free(portype);
				if(portnb!=NULL)
					free(portnb);
				if(adminstatus!=NULL)
					free(adminstatus);
				if(portway!=NULL)
					free(portway);
				if(numrows_inserted!=NULL)
					free(numrows_inserted);
				if(devname_nullList!=NULL)
					free(devname_nullList);
				if(portype_nullList!=NULL)
					free(portype_nullList);
				if(portnb_nullList!=NULL)
					free(portnb_nullList);

				for(i=0;i<NbElement;i++)
					delete portList[i];
				if(portList!=NULL)
					free(portList);
				
				free(errmessg1);
				free(errmessg2);
				return -1;				
			}

			for(i=0;i<NbElement;i++){
				if (portList[i]->devnamelen > max_devnamelen)
					max_devnamelen=portList[i]->devnamelen;
				if (portList[i]->portypelen > max_portypelen)
					max_portypelen=portList[i]->portypelen;
				if (portList[i]->portnblen > max_portnblen)
					max_portnblen=portList[i]->portnblen;				
			}

			devname=(char*)malloc(NbElement*max_devnamelen*sizeof(char));
			portype=(char*)malloc(NbElement*max_portypelen*sizeof(char));
			portnb=(char*)malloc(NbElement*max_portnblen*sizeof(char));
			adminstatus=(int*)malloc(sizeof(int)*NbElement);
			portway=(int*)malloc(sizeof(int)*NbElement);
			
			devname_nullList=(int*)malloc(NbElement*sizeof(int));
			portype_nullList=(int*)malloc(NbElement*sizeof(int));
			portnb_nullList=(int*)malloc(NbElement*sizeof(int));

			if(devname!=NULL && portype!=NULL && portnb!=NULL && adminstatus!=NULL && portway!=NULL 
				&& devname_nullList!=NULL && portype_nullList!=NULL && portnb_nullList!=NULL)
			for(i=0;i<NbElement;i++)
			{
				memcpy(devname+i*max_devnamelen,portList[i]->devname,portList[i]->devnamelen);
				devname_nullList[i]=portList[i]->devname_null;				
				memcpy(portype+i*max_portypelen,portList[i]->portype,portList[i]->portypelen);
				portype_nullList[i]=portList[i]->portype_null;
				memcpy(portnb+i*max_portnblen,portList[i]->portnb,portList[i]->portnblen);
				portnb_nullList[i]=portList[i]->portnb_null;

				adminstatus[i]=portList[i]->adminstatus;
				portway[i]=portList[i]->portway;
			}

			numrows_inserted=(int*)malloc(sizeof(int)*NbElement);
			portid_list=(int*)malloc(sizeof(int)*NbElement);

			if(devname==NULL || portype==NULL || portnb==NULL || adminstatus==NULL || portway==NULL
				|| devname_nullList==NULL || portype_nullList==NULL || portnb_nullList==NULL || numrows_inserted==NULL || portid_list==NULL)
			{
				rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");
				GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
				
				FIRST_TIME=0;
				if(devname!=NULL)
					free(devname);
				if(portype!=NULL)
					free(portype);
				if(portnb!=NULL)
					free(portnb);
				if(adminstatus!=NULL)
					free(adminstatus);
				if(portway!=NULL)
					free(portway);
				if(numrows_inserted!=NULL)
					free(numrows_inserted);
				if(devname_nullList!=NULL)
					free(devname_nullList);
				if(portype_nullList!=NULL)
					free(portype_nullList);
				if(portnb_nullList!=NULL)
					free(portnb_nullList);
				if(portid_list!=NULL)
					free(portid_list);
				
				for(i=0;i<NbElement;i++)
					delete portList[i];
				if(portList!=NULL)
                    free(portList);
				
				NbElement=0;				
				return -1;
			}
			else			
				for(i=0;i<NbElement;i++)
				{
					numrows_inserted[i]=0;
					portid_list[i]=0;
				}

			try{
                HandleAlloc(ociEnv,(dvoid**)&hstmt,OCI_HTYPE_STMT,&status);
				sprintf(sqlstmt,"BEGIN update %s set administrative_status=decode(:admin_status,1,1,0,0,administrative_status),last_update=sysdate,user_update='%s' where deviceid=(select deviceid from %s where devicename=:dname) and port_nbr=:port_nbr and port_way=:port_way and nvl(port_type,'none')=:port_type returning portid into :portid; update %s t set t.last_update=sysdate,t.user_update='%s',t.lkused=(select decode(t.lkused,1,p.administrative_status*g.administrative_status,t.lkused) from %s p,%s g where  t.portidfrom=p.portid and t.portidto=g.portid and (g.portid=:portid or p.portid=:portid)) where t.portidto=:portid or t.portidfrom=:portid;:numrows:=%s; END;",PORT_TABLE,login,LOGICAL_DEVICE_TABLE,MACRO_CONNECTIVITY_TABLE,login,PORT_TABLE,PORT_TABLE,SQLROWCOUNT);
				StmtPrepare(hstmt,ociError, sqlstmt, &status);

				BindByName(hstmt,&bndp[0],ociError,":port_nbr",portnb,max_portnblen,SQLT_STR,&portnb_nullList[0],&status);
				BindByName(hstmt,&bndp[1],ociError,":admin_status",&adminstatus[0],sizeof(&portList[0]->adminstatus),SQLT_INT,0,&status);
				BindByName(hstmt,&bndp[2],ociError,":port_way",&portway[0],sizeof(&portList[0]->portway),SQLT_INT,0,&status);
				BindByName(hstmt,&bndp[3],ociError,":port_type",portype,max_portypelen,SQLT_STR,&portype_nullList[0],&status);
				BindByName(hstmt,&bndp[4],ociError,":dname",devname,max_devnamelen,SQLT_STR,&devname_nullList[0],&status);
				BindByName(hstmt,&bndp[5],ociError,":numrows",&numrows_inserted[0],sizeof(int),SQLT_INT,0,&status);
				BindByName(hstmt,&bndp[6],ociError,":portid",&portid_list[0],sizeof(int),SQLT_INT,0,&status);

				BindArrayOfStruct(bndp[0],ociError,max_portnblen,sizeof(int),&status);
				BindArrayOfStruct(bndp[1],ociError,sizeof(int),0,&status);
				BindArrayOfStruct(bndp[2],ociError,sizeof(int),0,&status);
				BindArrayOfStruct(bndp[3],ociError,max_portypelen,sizeof(int),&status);
				BindArrayOfStruct(bndp[4],ociError,max_devnamelen,sizeof(int),&status);
				BindArrayOfStruct(bndp[5],ociError,sizeof(int),0,&status);
				BindArrayOfStruct(bndp[6],ociError,sizeof(int),0,&status);

				StmtExecute(ociHdbc, hstmt, ociError,NbElement, &status);
			
			}catch(Error err){
				sprintf(appliName,"UpdateMultiplePortStatuses");	///////
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
				
				if(devname!=NULL)
					free(devname);
				if(portype!=NULL)
					free(portype);
				if(portnb!=NULL)
					free(portnb);
				if(adminstatus!=NULL)
					free(adminstatus);
				if(portway!=NULL)
					free(portway);
				if(numrows_inserted!=NULL)
					free(numrows_inserted);
				if(devname_nullList!=NULL)
					free(devname_nullList);
				if(portype_nullList!=NULL)
					free(portype_nullList);
				if(portnb_nullList!=NULL)
					free(portnb_nullList);
				if(portid_list!=NULL)
					free(portid_list);
				
				for(i=0;i<NbElement;i++)
					delete portList[i];
				if(force_insert==1)
				{
                    FIRST_TIME=1;
					force_insert=0;
					portList=(UpdtPort**)realloc(portList, 0*sizeof(UpdtPort));
				}
				else if(portList!=NULL)
                    free(portList);

				NbElement=0;

				GetErrorMess(appliName, "COULDNOT UPDATE ALL ROWS",ErrMess,1);
				return -1;
			}
            else
			{
				status = OCITransCommit(ociHdbc, ociError, 0);
				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)	
						rescode=ShowErrors (status, ociError, "OCITransCommit unsuccessful");
				}

				int res_query;
				sprintf(sqlstmt,"BEGIN :rescode:=%s(1); :numrows:=%s; END; ",_UpdatePathUsed,SQLROWCOUNT);

				try{
					StmtPrepare(hstmt,ociError, sqlstmt, &status);
					BindByName(hstmt,&bndp[0],ociError,":numrows",&numrows,sizeof(int),SQLT_INT,0,&status);
					BindByName(hstmt,&bndp[1],ociError,":rescode",&res_query,sizeof(int),SQLT_INT,0,&status);
					StmtExecute(ociHdbc, hstmt, ociError,1, &status);
				}catch(Error err){
					rescode=ShowErrors (status, err.ociError, err.log);
				}

				status = OCITransCommit(ociHdbc, ociError, 0);
				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)	
						rescode=ShowErrors (status, ociError, "OCITransCommit unsuccessful");
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
				delete portList[i];

			if(force_insert==1)
			{
				FIRST_TIME=1;
				force_insert=0;
				portList=(UpdtPort**)realloc(portList, 0*sizeof(UpdtPort));
			}
			else if(portList!=NULL)
                free(portList);
			NbElement=0;
		}
		else
		{
			if(res_query!=0)
			{
				FIRST_TIME=0;
				GetErrorMess(appliName, "Cache Problem",ErrMess,1);
				for(i=0;i<NbElement;i++)
					delete portList[i];
				if(portList!=NULL)
                    free(portList);
			}
			if(ociEnv==0)
			{
				FIRST_TIME=0;
				res_query=-1;
				GetErrorMess(appliName, "NOT CONNECTED TO ANY DB",ErrMess,1);
				for(i=0;i<NbElement;i++)
					delete portList[i];
				if(portList!=NULL)
                    free(portList);
			}
			status+=res_query;
			if(ociEnv!=0 && FIRST_TIME==1 && res_query==0)
			{
				status=0;
				GetErrorMess(appliName, "NO_ERROR",ErrMess,0);
			}
		}
		if(devname!=NULL)
			free(devname);
		if(portype!=NULL)
			free(portype);
		if(portnb!=NULL)
			free(portnb);
		if(adminstatus!=NULL)
			free(adminstatus);
		if(portway!=NULL)
			free(portway);
		if(numrows_inserted!=NULL)
			free(numrows_inserted);
		if(devname_nullList!=NULL)
			free(devname_nullList);
		if(portype_nullList!=NULL)
			free(portype_nullList);
		if(portnb_nullList!=NULL)
			free(portnb_nullList);
		if(portid_list!=NULL)
			free(portid_list);

		return (rescode+status);
	}

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif