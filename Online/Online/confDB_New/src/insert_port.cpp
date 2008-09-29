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
* Insert Multiple Ports, returning an integer value.
* The user should manage the memory : there is no memory allocation.
* @param devicename : name of the device
* @param port_nb   : port nb
* @param admin_status   : 1 if the status of the port is OK, 0 otherwise
* @param port_way   : 1 if it's an input, 2 if it's an output : the reference is the data coming from the detector.
* @param speed   : speed of the port 
* @param pxi_booting   : 1 if the port is the pxi_booting, 0 otherwise.
* @param port_type   : type of the port interface (control, data,... used for the DAQ). Put "" if none.
* @param bia : burned mac address (used for the DAQ, for addresses). Put "" if none.
* @param ipadd1 : ip address (used for the DAQ, for addresses). Put "" if none.
* @param ipname : ip name (used for the DAQ, for addresses). Put "" if none.
* @param subnet : subnet address (used for the DAQ, for addresses). Put "" if none.
* @param macadd : mac address (used for the DAQ, for addresses). Put "" if none.
* @param phy: possible values :"SX","T","SL"
* @param first_time1 : 1 if it's your first Port insert (save it into the database), 0 otherwise   
* @param last_rows1 : 1 if it's your last Port insert (save it into the database), 0 otherwise
* @param ErrMess : error message in case of failure
* @return 0 if it is successful
*/ 
EXTERN_CONFDB
	int InsertMultiplePorts(char* devicename,char* port_nb,int admin_status,int port_way,int speed,int pxi_booting,char* port_type,char* bia,char* ipadd,char* ipname,char* subnet,char* macadd,char* phy,int first_time,int last_rows,char* ErrMess)
	{
		char appliName[100]="InsertMultiplePorts";
		char sqlstmt[2000];
		OCIStmt *hstmt;
		OCIBind  *bndp[15]; 
		int rescode=0;
		int res_query=0;
		sword status=0;
		int i=0;

		char* devname=NULL;
		char* dphy=NULL;
		char* portype=NULL;
		char* dbia=NULL;
		char* mac=NULL;
		char* ip=NULL;
		char* dipname=NULL;
		char* dsubnet=NULL;
		char* portnb=NULL;
		int* adminstatus=NULL;
		int* portway=NULL;
		int* pspeed=NULL;
		int* pxibooting=NULL;

		int* devname_nullList=NULL;
		int* dphy_nullList=NULL;
		int* portype_nullList=NULL;
		int* dbia_nullList=NULL;
		int* mac_nullList=NULL;
		int* ip_nullList=NULL;
		int* dipname_nullList=NULL;
		int* dsubnet_nullList=NULL;
		int* portnb_nullList=NULL;

		static int FIRST_TIME=0;
		int force_insert=0;
		char seq_name[100]="lhcb_portseq.nextval";
		int* numrows_inserted=NULL;
		int* rescode_inserted=NULL;
		int numrows=0;

		static int NbElement;
		Port** portList;
		int max_devnamelen=0;
		int max_dphylen=0;
		int max_portypelen=0;
		int max_dbialen=0;
		int max_maclen=0;
		int max_iplen=0;
		int max_dipnamelen=0;
		int max_dsubnetlen=0;
		int max_portnblen=0;

		if (first_time==1)
		{
			FIRST_TIME=1;
			NbElement=1;
			portList=(Port**)malloc( NbElement*sizeof(Port));			
		}
		else 
		{
			if (FIRST_TIME==0&&force_insert==0)
			{
				GetErrorMess(appliName, "CACHE HAS NOT BEEN INITIALIZED",ErrMess,1);
				return -1;
			}
			NbElement++;
			portList=(Port**)realloc(portList, NbElement*sizeof(Port));
		}
		
		if(ipadd==NULL || strlen(ipadd)<5 || strcspn(ipadd,".")==strlen(ipadd))
			ipadd="none";
		
		portList[NbElement-1]=new Port(devicename,port_nb,admin_status,port_way,speed,pxi_booting,port_type,bia,ipadd,ipname,subnet,macadd,phy);
		
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
				if(dsubnet!=NULL)
					free(dsubnet);
				if(ip!=NULL)
					free(ip);
				if(dipname!=NULL)
					free(dipname);
				if(dbia!=NULL)
					free(dbia);
				if(mac!=NULL)
					free(mac);
				if(portype!=NULL)
					free(portype);
				if(portnb!=NULL)
					free(portnb);
				if(adminstatus!=NULL)
					free(adminstatus);
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
				if(dbia_nullList!=NULL)
					free(dbia_nullList);
				if(mac_nullList!=NULL)
					free(mac_nullList);
				if(ip_nullList!=NULL)
					free(ip_nullList);
				if(dipname_nullList!=NULL)
					free(dipname_nullList);
				if(dsubnet_nullList!=NULL)
					free(dsubnet_nullList);
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
				if (portList[i]->dbialen > max_dbialen)
					max_dbialen=portList[i]->dbialen;
				if (portList[i]->maclen > max_maclen)
					max_maclen=portList[i]->maclen;
				if (portList[i]->iplen > max_iplen)
					max_iplen=portList[i]->iplen;
				if (portList[i]->dipnamelen > max_dipnamelen)
					max_dipnamelen=portList[i]->dipnamelen;
				if (portList[i]->dsubnetlen > max_dsubnetlen)
					max_dsubnetlen=portList[i]->dsubnetlen;
				if (portList[i]->portnblen > max_portnblen)
					max_portnblen=portList[i]->portnblen;				
			}

			devname=(char*)malloc(NbElement*max_devnamelen*sizeof(char));
			dphy=(char*)malloc(NbElement*max_dphylen*sizeof(char));
			portype=(char*)malloc(NbElement*max_portypelen*sizeof(char));
			dbia=(char*)malloc(NbElement*max_dbialen*sizeof(char));
			mac=(char*)malloc(NbElement*max_maclen*sizeof(char));
			ip=(char*)malloc(NbElement*max_iplen*sizeof(char));
			dipname=(char*)malloc(NbElement*max_dipnamelen*sizeof(char));
			dsubnet=(char*)malloc(NbElement*max_dsubnetlen*sizeof(char));
			portnb=(char*)malloc(NbElement*max_portnblen*sizeof(char));

			adminstatus=(int*)malloc(sizeof(int)*NbElement);
			portway=(int*)malloc(sizeof(int)*NbElement);
			pspeed=(int*)malloc(sizeof(int)*NbElement);
			pxibooting=(int*)malloc(sizeof(int)*NbElement);

			devname_nullList=(int*)malloc(NbElement*sizeof(int));
			dphy_nullList=(int*)malloc(NbElement*sizeof(int));
			portype_nullList=(int*)malloc(NbElement*sizeof(int));
			dbia_nullList=(int*)malloc(NbElement*sizeof(int));
			mac_nullList=(int*)malloc(NbElement*sizeof(int));
			ip_nullList=(int*)malloc(NbElement*sizeof(int));
			dipname_nullList=(int*)malloc(NbElement*sizeof(int));
			dsubnet_nullList=(int*)malloc(NbElement*sizeof(int));
			portnb_nullList=(int*)malloc(NbElement*sizeof(int));
			
			if(devname!=NULL && dphy!=NULL && portype!=NULL && dbia!=NULL && mac!=NULL && ip!=NULL && dipname!=NULL && dsubnet!=NULL && portnb!=NULL && adminstatus!=NULL && portway!=NULL && pspeed!=NULL && pxibooting!=NULL 
				&& devname_nullList!=NULL && dphy_nullList!=NULL && portype_nullList!=NULL && dbia_nullList!=NULL && mac_nullList!=NULL && ip_nullList!=NULL && dipname_nullList!=NULL && dsubnet_nullList!=NULL && portnb_nullList!=NULL)
			for(i=0;i<NbElement;i++)
			{
				memcpy(devname+i*max_devnamelen,portList[i]->devname,portList[i]->devnamelen);
				devname_nullList[i]=portList[i]->devname_null;				
				memcpy(dphy+i*max_dphylen,portList[i]->dphy,portList[i]->dphylen);
				dphy_nullList[i]=portList[i]->dphy_null;
				memcpy(portype+i*max_portypelen,portList[i]->portype,portList[i]->portypelen);
				portype_nullList[i]=portList[i]->portype_null;
				memcpy(dbia+i*max_dbialen,portList[i]->dbia,portList[i]->dbialen);
				dbia_nullList[i]=portList[i]->dbia_null;
				memcpy(mac+i*max_maclen,portList[i]->mac,portList[i]->maclen);
				mac_nullList[i]=portList[i]->mac_null;
				memcpy(ip+i*max_iplen,portList[i]->ip,portList[i]->iplen);
				ip_nullList[i]=portList[i]->ip_null;
				memcpy(dipname+i*max_dipnamelen,portList[i]->dipname,portList[i]->dipnamelen);
				dipname_nullList[i]=portList[i]->dipname_null;
				memcpy(dsubnet+i*max_dsubnetlen,portList[i]->dsubnet,portList[i]->dsubnetlen);
				dsubnet_nullList[i]=portList[i]->dsubnet_null;
				memcpy(portnb+i*max_portnblen,portList[i]->portnb,portList[i]->portnblen);
				portnb_nullList[i]=portList[i]->portnb_null;

				adminstatus[i]=portList[i]->adminstatus;
				portway[i]=portList[i]->portway;
				pspeed[i]=portList[i]->pspeed;
				pxibooting[i]=portList[i]->pxibooting;
			}

			numrows_inserted=(int*)malloc(sizeof(int)*NbElement);
			rescode_inserted=(int*)malloc(sizeof(int)*NbElement);

			if(devname==NULL || dphy==NULL || portype==NULL || dbia==NULL || mac==NULL || ip==NULL || dipname==NULL || dsubnet==NULL || portnb==NULL || adminstatus==NULL || portway==NULL || pspeed==NULL || pxibooting==NULL || rescode_inserted==NULL
				|| devname_nullList==NULL || dphy_nullList==NULL || portype_nullList==NULL || dbia_nullList==NULL || mac_nullList==NULL || ip_nullList==NULL || dipname_nullList==NULL || dsubnet_nullList==NULL || portnb_nullList==NULL || numrows_inserted==NULL)
			{
				rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");
				GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
				
				FIRST_TIME=0;
				if(devname!=NULL)
					free(devname);
				if(dphy!=NULL)
					free(dphy);
				if(dsubnet!=NULL)
					free(dsubnet);
				if(ip!=NULL)
					free(ip);
				if(dipname!=NULL)
					free(dipname);
				if(dbia!=NULL)
					free(dbia);
				if(mac!=NULL)
					free(mac);
				if(portype!=NULL)
					free(portype);
				if(portnb!=NULL)
					free(portnb);
				if(adminstatus!=NULL)
					free(adminstatus);
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
				if(dbia_nullList!=NULL)
					free(dbia_nullList);
				if(mac_nullList!=NULL)
					free(mac_nullList);
				if(ip_nullList!=NULL)
					free(ip_nullList);
				if(dipname_nullList!=NULL)
					free(dipname_nullList);
				if(dsubnet_nullList!=NULL)
					free(dsubnet_nullList);
				if(portnb_nullList!=NULL)
					free(portnb_nullList);
				if(rescode_inserted!=NULL)
					free(rescode_inserted);
		
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
					rescode_inserted[i]=0;
				}

			try{
                HandleAlloc(ociEnv,(dvoid**)&hstmt,OCI_HTYPE_STMT,&status);
				sprintf(sqlstmt,"BEGIN if instr(:ipadd,'none')=0 then :rescode:=%s(:ipadd,:ipname,:subnet,'%s','%s'); end if; insert ALL WHEN (instr(:ipadd,'none')>0) then into %s (portid,deviceid,port_nbr,port_way,administrative_status,speed,port_type,phy,pxi_booting,ipaddress,author,user_update,terminal_name) values (%s,devid,:port_nbr,:portway,:adminstatus,:speed,:portype,:phy,:pxi,NULL,'%s','%s','%s') else into %s (serialnb,port_nbr,port_type,port_way,bia,macaddress,author,created,terminal_name) values (snb,:port_nbr,:portype,:portway,:bia,:mac,'%s',sysdate,'%s') into %s(portid,deviceid,port_nbr,port_way,administrative_status,speed,port_type,phy,pxi_booting,ipaddress,author,user_update,terminal_name) values (%s,devid,:port_nbr,:portway,:adminstatus,:speed,:portype,:phy,:pxi,:ipadd,'%s','%s','%s') select deviceid as devid,serialnb as snb from  %s  where devicename=:dname; :numrows:=%s; end; ",_insertIpaddress,login,host,PORT_TABLE,seq_name,login,login,host,HWPORT_TABLE,login,host,PORT_TABLE,seq_name,login,login,host,LOGICAL_DEVICE_TABLE,SQLROWCOUNT);
				StmtPrepare(hstmt,ociError, sqlstmt, &status);

				BindByName(hstmt,&bndp[0],ociError,":port_nbr",portnb,max_portnblen,SQLT_STR,&portnb_nullList[0],&status);
				BindByName(hstmt,&bndp[1],ociError,":ipadd",ip,max_iplen,SQLT_STR,&ip_nullList[0],&status);
				BindByName(hstmt,&bndp[2],ociError,":ipname",dipname,max_dipnamelen,SQLT_STR,&dipname_nullList[0],&status);
				BindByName(hstmt,&bndp[3],ociError,":subnet",dsubnet,max_dsubnetlen,SQLT_STR,&dsubnet_nullList[0],&status);
				BindByName(hstmt,&bndp[4],ociError,":portway",&portway[0],sizeof(&portList[0]->portway),SQLT_INT,0,&status);
				BindByName(hstmt,&bndp[5],ociError,":speed",&pspeed[0],sizeof(&portList[0]->pspeed),SQLT_INT,0,&status);
				BindByName(hstmt,&bndp[6],ociError,":adminstatus",&adminstatus[0],sizeof(&portList[0]->adminstatus),SQLT_INT,0,&status);
				BindByName(hstmt,&bndp[7],ociError,":portype",portype,max_portypelen,SQLT_STR,&portype_nullList[0],&status);
				BindByName(hstmt,&bndp[8],ociError,":phy",dphy,max_dphylen,SQLT_STR,&dphy_nullList[0],&status);
				BindByName(hstmt,&bndp[9],ociError,":pxi",&pxibooting[0],sizeof(&portList[0]->pxibooting),SQLT_INT,0,&status);
				BindByName(hstmt,&bndp[10],ociError,":bia",dbia,max_dbialen,SQLT_STR,&dbia_nullList[0],&status);
				BindByName(hstmt,&bndp[11],ociError,":mac",mac,max_maclen,SQLT_STR,&mac_nullList[0],&status);
				BindByName(hstmt,&bndp[12],ociError,":dname",devname,max_devnamelen,SQLT_STR,&devname_nullList[0],&status);
				BindByName(hstmt,&bndp[13],ociError,":numrows",&numrows_inserted[0],sizeof(int),SQLT_INT,0,&status);
				BindByName(hstmt,&bndp[14],ociError,":rescode",&rescode_inserted[0],sizeof(int),SQLT_INT,0,&status);

				BindArrayOfStruct(bndp[0],ociError,max_portnblen,sizeof(int),&status);
				BindArrayOfStruct(bndp[1],ociError,max_iplen,sizeof(int),&status);
				BindArrayOfStruct(bndp[2],ociError,max_dipnamelen,sizeof(int),&status);
				BindArrayOfStruct(bndp[3],ociError,max_dsubnetlen,sizeof(int),&status);
				BindArrayOfStruct(bndp[4],ociError,sizeof(int),0,&status);
				BindArrayOfStruct(bndp[5],ociError,sizeof(int),0,&status);
				BindArrayOfStruct(bndp[6],ociError,sizeof(int),0,&status);
				BindArrayOfStruct(bndp[7],ociError,max_portypelen,sizeof(int),&status);
				BindArrayOfStruct(bndp[8],ociError,max_dphylen,sizeof(int),&status);
				BindArrayOfStruct(bndp[9],ociError,sizeof(int),0,&status);
				BindArrayOfStruct(bndp[10],ociError,max_dbialen,sizeof(int),&status);
				BindArrayOfStruct(bndp[11],ociError,max_maclen,sizeof(int),&status);
				BindArrayOfStruct(bndp[12],ociError,max_devnamelen,sizeof(int),&status);
				BindArrayOfStruct(bndp[13],ociError,sizeof(int),0,&status);
				BindArrayOfStruct(bndp[14],ociError,sizeof(int),0,&status);

				StmtExecute(ociHdbc, hstmt, ociError,NbElement, &status);
			
			}catch(Error err){
				sprintf(appliName,"InsertMultiplePorts");	///////
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
				if(dsubnet!=NULL)
					free(dsubnet);
				if(ip!=NULL)
					free(ip);
				if(dipname!=NULL)
					free(dipname);
				if(dbia!=NULL)
					free(dbia);
				if(mac!=NULL)
					free(mac);
				if(portype!=NULL)
					free(portype);
				if(portnb!=NULL)
					free(portnb);
				if(adminstatus!=NULL)
					free(adminstatus);
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
				if(dbia_nullList!=NULL)
					free(dbia_nullList);
				if(mac_nullList!=NULL)
					free(mac_nullList);
				if(ip_nullList!=NULL)
					free(ip_nullList);
				if(dipname_nullList!=NULL)
					free(dipname_nullList);
				if(dsubnet_nullList!=NULL)
					free(dsubnet_nullList);
				if(portnb_nullList!=NULL)
					free(portnb_nullList);
				if(rescode_inserted!=NULL)
					free(rescode_inserted);
				
				for(i=0;i<NbElement;i++)
					delete portList[i];
				if(force_insert==1)
				{
                    FIRST_TIME=1;
					force_insert=0;
					portList=(Port**)realloc(portList, 0*sizeof(Port));
				}
				else if(portList!=NULL)
                    free(portList);

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
				delete portList[i];

			if(force_insert==1)
			{
				FIRST_TIME=1;
				force_insert=0;
				portList=(Port**)realloc(portList, 0*sizeof(Port));
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
		if(dsubnet!=NULL)
			free(dsubnet);
		if(ip!=NULL)
			free(ip);
		if(dipname!=NULL)
			free(dipname);
		if(dbia!=NULL)
			free(dbia);
		if(mac!=NULL)
			free(mac);
		if(portype!=NULL)
			free(portype);
		if(portnb!=NULL)
			free(portnb);
		if(adminstatus!=NULL)
			free(adminstatus);
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
		if(dbia_nullList!=NULL)
			free(dbia_nullList);
		if(mac_nullList!=NULL)
			free(mac_nullList);
		if(ip_nullList!=NULL)
			free(ip_nullList);
		if(dipname_nullList!=NULL)
			free(dipname_nullList);
		if(dsubnet_nullList!=NULL)
			free(dsubnet_nullList);
		if(portnb_nullList!=NULL)
			free(portnb_nullList);
		if(rescode_inserted!=NULL)
			free(rescode_inserted);

		return (rescode+status);
	}

/**
* Insert Multiple Spare Ports (only for the DAQ since they bia nd mac address ppies), returning an integer value.
* The user should manage the memory : there is no memory allocation.
* @param serialnb : serialnb of the hw
* @param port_nb   : port nb
* @param port_way   : 1 if it's an input, 2 if it's an output : the reference is the data coming from the detector.
* @param port_type   : type of the port interface (control, data,... used for the DAQ). Put "" if none.
* @param bia : burned mac address (used for the DAQ, for addresses). Put "" if none.
* @param macadd : mac address (used for the DAQ, for addresses). Put "" if none.
* @param first_time1 : 1 if it's your first Port insert (save it into the database), 0 otherwise   
* @param last_rows1 : 1 if it's your last Port insert (save it into the database), 0 otherwise
* @param ErrMess : error message in case of failure
* @return 0 if it is successful
*/ 
EXTERN_CONFDB
	int InsertMultipleSparePorts(char* serialnb,char* port_nb,int port_way,char* port_type,char* bia,char* macadd,int first_time,int last_rows,char* ErrMess)
	{
		char appliName[100]="InsertMultipleSparePorts";
		char sqlstmt[1000];
		OCIStmt *hstmt;
		OCIBind  *bndp[7]; 
		int rescode=0;
		int res_query=0;
		sword status=0;
		int i=0;

		char* serial=NULL;
		char* portype=NULL;
		char* dbia=NULL;
		char* mac=NULL;
		char* portnb=NULL;
		int* portway;

		int* serial_nullList=NULL;
		int* portype_nullList=NULL;
		int* dbia_nullList=NULL;
		int* mac_nullList=NULL;
		int* portnb_nullList=NULL;

		static int FIRST_TIME=0;
		int force_insert=0;
		int* numrows_inserted=NULL;	
		int numrows=0;

		static int NbElement;
		SparePort** portList;
		int max_seriallen=0;
		int max_portypelen=0;
		int max_dbialen=0;
		int max_maclen=0;
		int max_portnblen=0;

		if (first_time==1)
		{
			FIRST_TIME=1;
			NbElement=1;
			portList=(SparePort**)malloc( NbElement*sizeof(SparePort));			
		}
		else 
		{
			if (FIRST_TIME==0&&force_insert==0)
			{
				GetErrorMess(appliName, "CACHE HAS NOT BEEN INITIALIZED",ErrMess,1);
				return -1;
			}
			NbElement++;
			portList=(SparePort**)realloc(portList, NbElement*sizeof(SparePort));
		}

		portList[NbElement-1]=new SparePort(serialnb, port_nb, port_way, port_type, bia, macadd);
		
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

				if(serial!=NULL)
					free(serial);
				if(dbia!=NULL)
					free(dbia);
				if(mac!=NULL)
					free(mac);
				if(portype!=NULL)
					free(portype);
				if(portnb!=NULL)
					free(portnb);
				if(portway!=NULL)
					free(portway);
				if(numrows_inserted!=NULL)
					free(numrows_inserted);
				if(serial_nullList!=NULL)
					free(serial_nullList);
				if(portype_nullList!=NULL)
					free(portype_nullList);
				if(dbia_nullList!=NULL)
					free(dbia_nullList);
				if(mac_nullList!=NULL)
					free(mac_nullList);
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
				if (portList[i]->seriallen > max_seriallen)
					max_seriallen=portList[i]->seriallen;
				if (portList[i]->portypelen > max_portypelen)
					max_portypelen=portList[i]->portypelen;
				if (portList[i]->dbialen > max_dbialen)
					max_dbialen=portList[i]->dbialen;
				if (portList[i]->maclen > max_maclen)
					max_maclen=portList[i]->maclen;
				if (portList[i]->portnblen > max_portnblen)
					max_portnblen=portList[i]->portnblen;				
			}

			serial=(char*)malloc(NbElement*max_seriallen*sizeof(char));
			portype=(char*)malloc(NbElement*max_portypelen*sizeof(char));
			dbia=(char*)malloc(NbElement*max_dbialen*sizeof(char));
			mac=(char*)malloc(NbElement*max_maclen*sizeof(char));
			portnb=(char*)malloc(NbElement*max_portnblen*sizeof(char));
			portway=(int*)malloc(sizeof(int)*NbElement);

			serial_nullList=(int*)malloc(NbElement*sizeof(int));
			portype_nullList=(int*)malloc(NbElement*sizeof(int));
			dbia_nullList=(int*)malloc(NbElement*sizeof(int));
			mac_nullList=(int*)malloc(NbElement*sizeof(int));
			portnb_nullList=(int*)malloc(NbElement*sizeof(int));

			if(serial!=NULL && portype!=NULL && dbia!=NULL && mac!=NULL && portnb!=NULL && portway!=NULL && serial_nullList!=NULL 
				&& portype_nullList!=NULL && dbia_nullList!=NULL && mac_nullList!=NULL && portnb_nullList!=NULL)
			for(i=0;i<NbElement;i++)
			{
				memcpy(serial+i*max_seriallen,portList[i]->serial,portList[i]->seriallen);
				serial_nullList[i]=portList[i]->serial_null;				
				memcpy(portype+i*max_portypelen,portList[i]->portype,portList[i]->portypelen);
				portype_nullList[i]=portList[i]->portype_null;
				memcpy(dbia+i*max_dbialen,portList[i]->dbia,portList[i]->dbialen);
				dbia_nullList[i]=portList[i]->dbia_null;
				memcpy(mac+i*max_maclen,portList[i]->mac,portList[i]->maclen);
				mac_nullList[i]=portList[i]->mac_null;
				memcpy(portnb+i*max_portnblen,portList[i]->portnb,portList[i]->portnblen);
				portnb_nullList[i]=portList[i]->portnb_null;

				portway[i]=portList[i]->portway;
			}
			numrows_inserted=(int*)malloc(sizeof(int)*NbElement);

			if(serial==NULL || portype==NULL || dbia==NULL || mac==NULL || portnb==NULL || portway==NULL  || serial_nullList==NULL 
				|| portype_nullList==NULL || dbia_nullList==NULL || mac_nullList==NULL || portnb_nullList==NULL || numrows_inserted==NULL)
			{
				rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");
				GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
				
				FIRST_TIME=0;
				if(serial!=NULL)
					free(serial);
				if(dbia!=NULL)
					free(dbia);
				if(mac!=NULL)
					free(mac);
				if(portype!=NULL)
					free(portype);
				if(portnb!=NULL)
					free(portnb);
				if(portway!=NULL)
					free(portway);
				if(numrows_inserted!=NULL)
					free(numrows_inserted);
				if(serial_nullList!=NULL)
					free(serial_nullList);
				if(portype_nullList!=NULL)
					free(portype_nullList);
				if(dbia_nullList!=NULL)
					free(dbia_nullList);
				if(mac_nullList!=NULL)
					free(mac_nullList);
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
				sprintf(sqlstmt,"BEGIN insert into %s (serialnb,port_nbr,port_way,port_type,bia,macaddress,created,author,terminal_name) values (:snb,:port_nbr,:portway,:portype,:bia,:mac,sysdate,'%s','%s'); :numrows:=%s; end; ",HWPORT_TABLE,login,host,SQLROWCOUNT);
				StmtPrepare(hstmt,ociError, sqlstmt, &status);

				BindByName(hstmt,&bndp[0],ociError,":port_nbr",portnb,max_portnblen,SQLT_STR,&portnb_nullList[0],&status);
				BindByName(hstmt,&bndp[1],ociError,":portway",&portway[0],sizeof(&portList[0]->portway),SQLT_INT,0,&status);
				BindByName(hstmt,&bndp[2],ociError,":portype",portype,max_portypelen,SQLT_STR,&portype_nullList[0],&status);
				BindByName(hstmt,&bndp[3],ociError,":bia",dbia,max_dbialen,SQLT_STR,&dbia_nullList[0],&status);
				BindByName(hstmt,&bndp[4],ociError,":mac",mac,max_maclen,SQLT_STR,&mac_nullList[0],&status);
				BindByName(hstmt,&bndp[5],ociError,":snb",serial,max_seriallen,SQLT_STR,&serial_nullList[0],&status);
				BindByName(hstmt,&bndp[6],ociError,":numrows",&numrows_inserted[0],sizeof(int),SQLT_INT,0,&status);
			
				BindArrayOfStruct(bndp[0],ociError,max_portnblen,sizeof(int),&status);
				BindArrayOfStruct(bndp[1],ociError,sizeof(int),0,&status);
				BindArrayOfStruct(bndp[2],ociError,max_portypelen,sizeof(int),&status);
				BindArrayOfStruct(bndp[3],ociError,max_dbialen,sizeof(int),&status);
				BindArrayOfStruct(bndp[4],ociError,max_maclen,sizeof(int),&status);
				BindArrayOfStruct(bndp[5],ociError,max_seriallen,sizeof(int),&status);
				BindArrayOfStruct(bndp[6],ociError,sizeof(int),0,&status);

				StmtExecute(ociHdbc, hstmt, ociError,NbElement, &status);
				
			}catch(Error err){
				sprintf(appliName,"InsertMultipleSparePorts");	///////
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
				
				if(serial!=NULL)
					free(serial);
				if(dbia!=NULL)
					free(dbia);
				if(mac!=NULL)
					free(mac);
				if(portype!=NULL)
					free(portype);
				if(portnb!=NULL)
					free(portnb);
				if(portway!=NULL)
					free(portway);
				if(numrows_inserted!=NULL)
					free(numrows_inserted);
				if(serial_nullList!=NULL)
					free(serial_nullList);
				if(portype_nullList!=NULL)
					free(portype_nullList);
				if(dbia_nullList!=NULL)
					free(dbia_nullList);
				if(mac_nullList!=NULL)
					free(mac_nullList);
				if(portnb_nullList!=NULL)
					free(portnb_nullList);
				
				for(i=0;i<NbElement;i++)
					delete portList[i];
				if(force_insert==1)
				{
                    FIRST_TIME=1;
					force_insert=0;
					portList=(SparePort**)realloc(portList, 0*sizeof(SparePort));
				}
				else if(portList!=NULL)
                    free(portList);

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
				delete portList[i];

			if(force_insert==1)
			{
				FIRST_TIME=1;
				force_insert=0;
				portList=(SparePort**)realloc(portList, 0*sizeof(SparePort));
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

		if(serial!=NULL)
			free(serial);
		if(dbia!=NULL)
			free(dbia);
		if(mac!=NULL)
			free(mac);
		if(portype!=NULL)
			free(portype);
		if(portnb!=NULL)
			free(portnb);
		if(portway!=NULL)
			free(portway);
		if(numrows_inserted!=NULL)
			free(numrows_inserted);
		if(serial_nullList!=NULL)
			free(serial_nullList);
		if(portype_nullList!=NULL)
			free(portype_nullList);
		if(dbia_nullList!=NULL)
			free(dbia_nullList);
		if(mac_nullList!=NULL)
			free(mac_nullList);
		if(portnb_nullList!=NULL)
			free(portnb_nullList);
		
		return (rescode+status);
	}
//insert IP aliases
/**
* Insert Multiple IP aliases (only for the DAQ ), returning an integer value.
* The user should manage the memory : there is no memory allocation. IP address or IP alias should already be inserted using InsertPorts
* @param IP address : IP address for which you want to give an IP alias. Put "none" if you provide an IP name
* @param IP name   : IP name for which you want to give an IP alias. Put "none" if you provide an IP address
* @param IP alias   : 1 if it's an input, 2 if it's an output : the reference is the data coming from the detector.
* @param first_time1 : 1 if it's your first Port insert (save it into the database), 0 otherwise   
* @param last_rows1 : 1 if it's your last Port insert (save it into the database), 0 otherwise
* @param ErrMess : error message in case of failure
* @return 0 if it is successful
*/ 
EXTERN_CONFDB
	int InsertMultipleIPAliases(char* ipaddress,char* ipname,char* ipalias,int first_time,int last_rows,char* ErrMess)
	{
		char appliName[100]="InsertMultipleIPAlias";
		char sqlstmt[1000];
		OCIStmt *hstmt;
		OCIBind  *bndp[4]; 
		int rescode=0;
		int res_query=0;
		sword status=0;
		int i=0;

		char* ip=NULL;
		char* dipname=NULL;
		char* dipalias=NULL;
		
		int* ip_nullList=NULL;
		int* dipname_nullList=NULL;
		int* dipalias_nullList=NULL;

		static int FIRST_TIME=0;
		int force_insert=0;
		int* numrows_inserted=NULL;
		int numrows=0;

		static int NbElement;
		IPAlias** ipaliasList;
		int max_iplen=0;
		int max_dipnamelen=0;
		int max_dipaliaslen=0;

		if (first_time==1)
		{
			FIRST_TIME=1;
			NbElement=1;
			ipaliasList=(IPAlias**)malloc( NbElement*sizeof(IPAlias));			
		}
		else 
		{
			if (FIRST_TIME==0&&force_insert==0)
			{
				GetErrorMess(appliName, "CACHE HAS NOT BEEN INITIALIZED",ErrMess,1);
				return -1;
			}
			NbElement++;
			ipaliasList=(IPAlias**)realloc(ipaliasList, NbElement*sizeof(IPAlias));
		}

		ipaliasList[NbElement-1]=new IPAlias(ipaddress,ipname,ipalias);
		
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

				if(ip!=NULL)
					free(ip);
				if(dipname!=NULL)
					free(dipname);
				if(dipalias!=NULL)
					free(dipalias);
				if(numrows_inserted!=NULL)
					free(numrows_inserted);
				if(ip_nullList!=NULL)
					free(ip_nullList);
				if(dipname_nullList!=NULL)
					free(dipname_nullList);
				if(dipalias_nullList!=NULL)
					free(dipalias_nullList);
				
				for(i=0;i<NbElement;i++)
					delete ipaliasList[i];
				if(ipaliasList!=NULL)
					free(ipaliasList);
				
				free(errmessg1);
				free(errmessg2);
				return -1;				
			}

			for(i=0;i<NbElement;i++){
				if (ipaliasList[i]->iplen > max_iplen)
					max_iplen=ipaliasList[i]->iplen;
				if (ipaliasList[i]->dipnamelen > max_dipnamelen)
					max_dipnamelen=ipaliasList[i]->dipnamelen;
				if (ipaliasList[i]->dipaliaslen > max_dipaliaslen)
					max_dipaliaslen=ipaliasList[i]->dipaliaslen;
			}
			
			ip=(char*)malloc(NbElement*max_iplen*sizeof(char));
			dipname=(char*)malloc(NbElement*max_dipnamelen*sizeof(char));
			dipalias=(char*)malloc(NbElement*max_dipaliaslen*sizeof(char));
			
			ip_nullList=(int*)malloc(NbElement*sizeof(int));
			dipname_nullList=(int*)malloc(NbElement*sizeof(int));
			dipalias_nullList=(int*)malloc(NbElement*sizeof(int));
			
			if(ip!=NULL && dipname!=NULL && dipalias!=NULL && ip_nullList!=NULL && dipname_nullList!=NULL && dipalias_nullList!=NULL)

			for(i=0;i<NbElement;i++)
			{
				memcpy(ip+i*max_iplen,ipaliasList[i]->ip,ipaliasList[i]->iplen);
				ip_nullList[i]=ipaliasList[i]->ip_null;
				memcpy(dipname+i*max_dipnamelen,ipaliasList[i]->dipname,ipaliasList[i]->dipnamelen);
				dipname_nullList[i]=ipaliasList[i]->dipname_null;
				memcpy(dipalias+i*max_dipaliaslen,ipaliasList[i]->dipalias,ipaliasList[i]->dipaliaslen);
				dipalias_nullList[i]=ipaliasList[i]->dipalias_null;
			}

			numrows_inserted=(int*)malloc(sizeof(int)*NbElement);

			if(ip==NULL || dipname==NULL || dipalias==NULL	|| ip_nullList==NULL || dipname_nullList==NULL || dipalias_nullList==NULL || numrows_inserted==NULL)

			{
				rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");
				GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
				
				FIRST_TIME=0;
				if(ip!=NULL)
					free(ip);
				if(dipname!=NULL)
					free(dipname);
				if(dipalias!=NULL)
					free(dipalias);
				if(numrows_inserted!=NULL)
					free(numrows_inserted);
				if(ip_nullList!=NULL)
					free(ip_nullList);
				if(dipname_nullList!=NULL)
					free(dipname_nullList);
				if(dipalias_nullList!=NULL)
					free(dipalias_nullList);
				
				for(i=0;i<NbElement;i++)
					delete ipaliasList[i];
				if(ipaliasList!=NULL)
                    free(ipaliasList);
				
				NbElement=0;				
				return -1;
			}
			else			
				for(i=0;i<NbElement;i++)
					numrows_inserted[i]=0;

			try{
                HandleAlloc(ociEnv,(dvoid**)&hstmt,OCI_HTYPE_STMT,&status);
				sprintf(sqlstmt,"BEGIN if :ipname='none' then insert into %s(ipaddress,ipalias,created,author,terminal_name) values (:ipadd,:ipalias,sysdate,'%s','%s'); else if :ipadd='none' then insert all into %s (ipaddress,ipalias,created,author,terminal_name) values (ipadd,:ipalias,sysdate,'%s','%s') select ipaddress as ipadd from %s where ipname=:ipname; end if; end if; :numrows:=%s; end; ",IPALIAS_TABLE,login,host,IPALIAS_TABLE,login,host,IPINFO_TABLE,SQLROWCOUNT);	
				StmtPrepare(hstmt,ociError, sqlstmt, &status);

				BindByName(hstmt,&bndp[0],ociError,":ipname",dipname,max_dipnamelen,SQLT_STR,&dipname_nullList[0],&status);
				BindByName(hstmt,&bndp[1],ociError,":ipalias",dipalias,max_dipaliaslen,SQLT_STR,&dipalias_nullList[0],&status);
				BindByName(hstmt,&bndp[2],ociError,":ipadd",ip,max_iplen,SQLT_STR,&ip_nullList[0],&status);
				BindByName(hstmt,&bndp[3],ociError,":numrows",&numrows_inserted[0],sizeof(int),SQLT_INT,0,&status);

				BindArrayOfStruct(bndp[0],ociError,max_dipnamelen,sizeof(int),&status);
				BindArrayOfStruct(bndp[1],ociError,max_dipaliaslen,sizeof(int),&status);
				BindArrayOfStruct(bndp[2],ociError,max_iplen,sizeof(int),&status);
				BindArrayOfStruct(bndp[3],ociError,sizeof(int),0,&status);

				StmtExecute(ociHdbc, hstmt, ociError,NbElement, &status);
				
			}catch(Error err){
				sprintf(appliName,"InsertMultipleIPAliases");	///////
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
				
				if(ip!=NULL)
					free(ip);
				if(dipname!=NULL)
					free(dipname);
				if(dipalias!=NULL)
					free(dipalias);
				if(numrows_inserted!=NULL)
					free(numrows_inserted);
				if(ip_nullList!=NULL)
					free(ip_nullList);
				if(dipname_nullList!=NULL)
					free(dipname_nullList);
				if(dipalias_nullList!=NULL)
					free(dipalias_nullList);
				
				for(i=0;i<NbElement;i++)
					delete ipaliasList[i];
				if(force_insert==1)
				{
                    FIRST_TIME=1;
					force_insert=0;
					ipaliasList=(IPAlias**)realloc(ipaliasList, 0*sizeof(IPAlias));
				}
				else if(ipaliasList!=NULL)
                    free(ipaliasList);

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
				delete ipaliasList[i];

			if(force_insert==1)
			{
				FIRST_TIME=1;
				force_insert=0;
				ipaliasList=(IPAlias**)realloc(ipaliasList, 0*sizeof(IPAlias));
			}
			else if(ipaliasList!=NULL)
                free(ipaliasList);
			NbElement=0;
		}
		else
		{
			if(res_query!=0)
			{
				FIRST_TIME=0;
				GetErrorMess(appliName, "Cache Problem",ErrMess,1);
				for(i=0;i<NbElement;i++)
					delete ipaliasList[i];
				if(ipaliasList!=NULL)
                    free(ipaliasList);
			}
			if(ociEnv==0)
			{
				FIRST_TIME=0;
				res_query=-1;
				GetErrorMess(appliName, "NOT CONNECTED TO ANY DB",ErrMess,1);
				for(i=0;i<NbElement;i++)
					delete ipaliasList[i];
				if(ipaliasList!=NULL)
                    free(ipaliasList);
			}
			status+=res_query;
			if(ociEnv!=0 && FIRST_TIME==1 && res_query==0)
			{
				status=0;
				GetErrorMess(appliName, "NO_ERROR",ErrMess,0);
			}
		}
		if(ip!=NULL)
			free(ip);
		if(dipname!=NULL)
			free(dipname);
		if(dipalias!=NULL)
			free(dipalias);
		if(numrows_inserted!=NULL)
			free(numrows_inserted);
		if(ip_nullList!=NULL)
			free(ip_nullList);
		if(dipname_nullList!=NULL)
			free(dipname_nullList);
		if(dipalias_nullList!=NULL)
			free(dipalias_nullList);
		
		return (rescode+status);
	}
#if defined(__cplusplus) || defined(c_plusplus)
}
#endif