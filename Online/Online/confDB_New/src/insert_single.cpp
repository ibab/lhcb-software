/********************************************************************************/
//	Author: L.Abadie
// version:v3.5
// changes: handling error such that can be extended to C++: (create a class SQLException) (strlen ck + update stmt)
// file which performs single insert: one insert (so one fct) per table
// N.B : Memory management (allocation+release) should be handled by the user application thus, for all the fct provided by this lib
/********************************************************************************/


#include "list_structures.h"
#include "system_info.h"
#include "cicDB.h"
#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif


#if defined( _WIN32) || defined(WIN32)
#define EXTERN_CONFDB _declspec(dllexport)
#else
#define EXTERN_CONFDB extern
#endif

	//extern char PRIME_NUMBER[20];

	extern   OCIError* ociError ;
	extern   OCIEnv* ociEnv ;
	extern   OCISvcCtx* ociHdbc ;
	extern   OCIServer *mysrvhp;
	extern   OCISession *myusrhp; 

/**
* Insert a device type , returning an integer value.
* The user should manage the memory : there is no memory allocation.
* @param systemnameList : list of the subsystems (UPPER CASE) by which this device type is used: a VELO_TELL1 board type is used by the VELO, TFC and DAQ. so you type VELO,TFC,DAQ. you separate the different subsystems with a coma.
* @param devicetype : name of the devicetype
* @param nbrofinput: nb of input ports
* @param nbrofoutput: nb of output ports
* @param description :  some description about the component max size 500 characters.
* @param rgbcolor :  color in rgb system of the devicetype for display.
* @param last_rows : 1 if it's your last insert (save it into the database), 0 otherwise
* @param ErrMess : error message in case of failure
* @return 0 if it is successful
*/

EXTERN_CONFDB
	int InsertDeviceType(char* systemnameList,char* devicetype,int nbrofinput,int nbrofoutput, char* description, char* rgbcolor,int last_rows,char* ErrMess)
	{
		char appliName[100]="InsertDeviceType";
		int numrows=0;
		char sqlstmt[1000];
		OCIStmt *hstmt;
		OCIBind  *bndp[7]; 
		int rescode=0;
		sword status;
		int devicetype_null=null_charptr(devicetype);
		int description_null=null_charptr(description);
		int rgbcolor_null=null_charptr(rgbcolor);
		int len_host=LOGIN_LEN;
		char login[LOGIN_LEN];
		char host[LOGIN_LEN];
		char seq_name[100]="lhcb_devtype_seq.nextval";

		char* errmessg1=(char*)malloc(ERROR_BUFFER*sizeof(char));

		if(errmessg1==NULL)
		{
			GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
			return -1;
		}

		char* errmessg2=(char*)malloc(ERROR_BUFFER*sizeof(char));
		if(errmessg2==NULL)
		{
			GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
			free(errmessg1);
			return -1;
		}
		int descrip_len=0;
		int devtype_len=0;
		int rgbcolor_len=0;
		if(devicetype!=NULL)
			devtype_len=strlen(devicetype)+1;
		if(description!=NULL)
			descrip_len=strlen(description)+1;
		if(rgbcolor!=NULL)
			rgbcolor_len=strlen(rgbcolor)+1;

		int sysID=GetSubsystemID(systemnameList);
		
		if(sysID==-1)
		{
			GetErrorMess(appliName, "systemnameList is null",ErrMess,1);
			free(errmessg1);
			free(errmessg2);
			return -1;
		}
		int res_query=getTerminalName(host,len_host,errmessg1);
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
			free(errmessg1);
			free(errmessg2);
			return -1;
		}
		//need to proceed with messages

		try{
			HandleAlloc(ociEnv,(dvoid**)&hstmt,OCI_HTYPE_STMT,&status);
			sprintf(sqlstmt,"BEGIN insert into %s(devicetypeID,devicetype,nbrofinput,nbrofoutput,description,rgbcolor,author,user_update,terminal_name,system_name) values(%s,:dtype,:din,:dout,:ddescr,:rgbcol,'%s','%s','%s',:sysID); :numrows:=%s; END;",DEVICETYPE_TABLE,seq_name,login,login,host,SQLROWCOUNT);
			
			StmtPrepare(hstmt,ociError, sqlstmt, &status);
			BindByName(hstmt,&bndp[0],ociError,":dtype",devicetype,devtype_len,SQLT_STR,&devicetype_null,&status);
			BindByName(hstmt,&bndp[1],ociError,":din",&nbrofinput,sizeof(nbrofinput),SQLT_INT,0,&status);
			BindByName(hstmt,&bndp[2],ociError,":dout",&nbrofoutput,sizeof(nbrofoutput),SQLT_INT,0,&status);
			BindByName(hstmt,&bndp[3],ociError,":ddescr",description,descrip_len,SQLT_STR,&description_null,&status);
			BindByName(hstmt,&bndp[4],ociError,":rgbcol",rgbcolor,rgbcolor_len,SQLT_STR,&rgbcolor_null,&status);
			BindByName(hstmt,&bndp[5],ociError,":sysID",&sysID,sizeof(sysID),SQLT_INT,0,&status);
			BindByName(hstmt,&bndp[6],ociError,":numrows",&numrows,sizeof(numrows),SQLT_INT,0,&status);
			StmtExecute(ociHdbc, hstmt, ociError,1, &status);			
			
		}catch(Error err){
			sprintf(appliName,"InsertDeviceType");	///////
			rescode=ShowErrors (status, err.ociError, err.log);
			
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrMess,1); 
			else
				GetErrorMess(appliName,err.msg,ErrMess,1);
			
			if(strstr(err.msg,"NOT CONNECTED TO ANY DB")!=NULL)
			return -1;
		}

		if(status==OCI_SUCCESS)
		{
			//even if it's not good for performance, we commit the transaction one by one... for consistency reasons.
			if(numrows==0)
			{
				status = OCITransCommit(ociHdbc, ociError, 0);
				status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
				GetErrorMess(appliName,  "COULDNOT_INSERT_THIS_ROW",ErrMess,1);
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

		if(rescode!=0)
			OCIReportError(ociError,appliName, ErrMess,1); 
		else
			OCIReportError(ociError,appliName, ErrMess,0); 

		return (status+rescode);
	}


/**
* Insert a device into the functional_device table and the hw table :it means that the hw device is currently IN_USE and it's having the given functional fct returning an integer value.
* The user should manage the memory : there is no memory allocation.
* @param systemnameList : list of the subsystems (UPPER CASE) by which this device type is used: a VELO_TELL1 board  is used by the VELO, TFC and DAQ. so you type VELO,TFC,DAQ. you separate the different subsystems with a coma.
* @param devicename : functional name of the device
* @param devicetype : name of the devicetype
* @param node : 1 if it's a host node (usually starts or ends a path such as L1FEs) , 0 otherwise.
* @param promiscuous_mode : 1 if put in this mode (default mode), 0 otherwise
* @param serial_nb :  serial_nb of the device
* @param hwtype :hardware type (market specif)
* @param responsible :guy responsible for this hw
* @param comments : some comments about the hw (max.1000 characters)
* @param location :  location of the device : crate\\rack\\slot (as defined in the equipment DB) or on the detector
* @param function_list : list of the functions of your devices (essentially for the DAQ, for instance DHCP_SERVER, DNS_SERVER...). Functions must be declared using insertSimpleDeviceFunction...If several, separate them with a coma. For ex. DHCP,DNS, NFS.	  
* @param last_rows : 1 if it's your MultipleDevice last insert (save it into the database), 0 otherwise  Maximum in one go: 10000 rows.( we force the insert)
* @param ErrMess : error message in case of failure
* @return 0 if it is successful
*/

//extern "C" __declspec(dllexport)
EXTERN_CONFDB
	int InsertFunctionalDevice(char* systemnameList,char* devicename,char* devicetype,int node,int promiscuous_mode,char* serial_nb,char* hwtype,char* responsible,char* comments,char* location,char* function_list,int last_rows,char* ErrMess)
	{
		char appliName[100]="InsertFunctionalDevice";
		char sqlstmt[2000];
		OCIStmt *hstmt;
		OCIBind  *bndp[12]; 
		int rescode=0;
		sword status;
		int numrows=0;
		int dname1=null_charptr(devicename);
		int dtype1=null_charptr(devicetype);
		int dserialnb=null_charptr(serial_nb);
		int dhwtype=null_charptr(hwtype);
		int dresponsible=null_charptr(responsible);
		int dfunctiondev=null_charptr(function_list);
		int dlocation=null_charptr(location);
		int dcomments=null_charptr(comments);
		
		char seqname[50]="lhcb_history_deviceseq.nextval";
		int len_host=LOGIN_LEN;
		char login[LOGIN_LEN];
		char host[LOGIN_LEN];
		char* errmessg1=(char*)malloc(ERROR_BUFFER*sizeof(char));
		char seq_name[100]="lhcb_lg_deviceseq.nextval";

		int dnamelen=0;
		int dtypelen=0;
		int dcommentslen=0;
		int dresponsiblelen=0;
		int dserialnblen=0;
		int dlocationlen=0;
		int dhwtypelen=0;
		int dfunctiondevlen=0;
		if(devicename!=NULL)
			dnamelen=strlen(devicename)+1;
		if(devicetype!=NULL)
			dtypelen=strlen(devicetype)+1;
		if(serial_nb!=NULL)
			dserialnblen=strlen(serial_nb)+1;
		if(location!=NULL)
			dlocationlen=strlen(location)+1;
		if(hwtype!=NULL)
			dhwtypelen=strlen(hwtype)+1;
		if(responsible!=NULL)
			dresponsiblelen=strlen(responsible)+1;
		if(comments!=NULL)
			dcommentslen=strlen(comments)+1;
		if(function_list!=NULL)
			dfunctiondevlen=strlen(function_list)+1;

		
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
		int res_query=getTerminalName(host,len_host,errmessg1);
		res_query=getLoginUser(login,len_host,errmessg2);
		int sysID=GetSubsystemID(systemnameList);
		if(sysID==-1)
		{
			GetErrorMess(appliName, "systemnameList is null",ErrMess,1);
			free(errmessg1);
			free(errmessg2);
			return -1;
		}	
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
			free(errmessg1);
			free(errmessg2);
			return -1;
		}

		try{
			HandleAlloc(ociEnv,(dvoid**)&hstmt,OCI_HTYPE_STMT,&status);
			sprintf(sqlstmt,
				"BEGIN insert all into %s(serialnb,hwtype,responsible,user_comments,device_status) values(:serialnb,:hwtype,:responsible,:comments,%d) select 1 from %s where devicetype=:dtype; insert all into %s(devicename,devicetypeID,node,deviceid,promiscuous_mode,system_name,serialnb,location,author,user_update,terminal_name,nodeused,functionid) values (:dname,devicetypeID,:dnode,%s,:dpromiscuous_mode,:sysID,:serialnb,:location,'%s','%s','%s',1,fctID) select devicetypeID ,decode(:fctlist,'none',0,%s(:fctlist)) as fctID from %s  where devicetype=:dtype; insert all into %s(historydeviceID,serialnb,deviceid,device_status,user_comment,location,status_change,system_name,created,author,terminal_name) values(%s,:serialnb,deviceid,'IN_USE',:comments,:location,statuschange,:sysID,sysdate,'%s','%s') select deviceid,created as statuschange from %s where devicename=:dname; :numrows:=%s; END;",HW_DEVICE_TABLE,IN_USE,DEVICETYPE_TABLE,LOGICAL_DEVICE_TABLE,seq_name,login,login,host,_ComposeFctID,DEVICETYPE_TABLE,HISTORY_DEVICE_TABLE,seqname,login,host,LOGICAL_DEVICE_TABLE,SQLROWCOUNT);
			
			StmtPrepare(hstmt,ociError, sqlstmt, &status);
			BindByName(hstmt,&bndp[0],ociError,":dname",devicename,dnamelen,SQLT_STR,&dname1,&status);
			BindByName(hstmt,&bndp[1],ociError,":dtype",devicetype,dtypelen,SQLT_STR,&dtype1,&status);
			BindByName(hstmt,&bndp[2],ociError,":dnode",&node,sizeof(node),SQLT_INT,0,&status);
			BindByName(hstmt,&bndp[3],ociError,":dpromiscuous_mode",&promiscuous_mode,sizeof(promiscuous_mode),SQLT_INT,0,&status);
			BindByName(hstmt,&bndp[4],ociError,":sysID",&sysID,sizeof(sysID),SQLT_INT,0,&status);
			BindByName(hstmt,&bndp[5],ociError,":serialnb",serial_nb,dserialnblen,SQLT_STR,&dserialnb,&status);
			BindByName(hstmt,&bndp[6],ociError,":location",location,dlocationlen,SQLT_STR,&dlocation,&status);
			BindByName(hstmt,&bndp[7],ociError,":hwtype",hwtype,dhwtypelen,SQLT_STR,&dhwtype,&status);
			BindByName(hstmt,&bndp[8],ociError,":responsible",responsible,dresponsiblelen,SQLT_STR,&dresponsible,&status);
			BindByName(hstmt,&bndp[9],ociError,":comments",comments,dcommentslen,SQLT_STR,&dcomments,&status);
			BindByName(hstmt,&bndp[10],ociError,":fctlist",function_list,dfunctiondevlen,SQLT_STR,&dfunctiondev,&status);
			BindByName(hstmt,&bndp[11],ociError,":numrows",&numrows,sizeof(numrows),SQLT_INT,0,&status);
			StmtExecute(ociHdbc, hstmt, ociError,1, &status);
			
		}catch(Error err){
			sprintf(appliName,"InsertFunctionalDevice");	///////
			rescode=ShowErrors (status, err.ociError, err.log);
			
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrMess,1); 
			else
				GetErrorMess(appliName,err.msg,ErrMess,1);
			
			if(strstr(err.msg,"NOT CONNECTED TO ANY DB")!=NULL)
			return -1;
		}
		
		if(status==OCI_SUCCESS)
		{
			if(numrows==0)
			{
				status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
				GetErrorMess(appliName,  "COULDNOT_INSERT_THIS_ROW",ErrMess,1);
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
		if(rescode!=0)
			OCIReportError(ociError,appliName, ErrMess,1); 
		else
			OCIReportError(ociError,appliName, ErrMess,0); 

		return (status+rescode);
	}


/************************************************************************************************************/
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
* @param link_information : some info about the links (nb of patch panels, some coefficients for algo. calcualtions, buffer of 1000). Put "" if none.
* @param bidirectional_link_used :  1 if it's a bidirectional link (it means that if it is consider as unidirectional some paths won't be found), 0 otherwise. 
* @param last_rows : 1 if it's your MultipleLink last insert (save it into the database), 0 otherwise. Maximum in one go: 10000 rows. (we force the insert)
* @param ErrMess : error message in case of failure
* @return 0 if it is successful
*/ 

EXTERN_CONFDB
	int InsertMacroLink(char* node_from,char* node_to,char* port_nbfrom,char* port_nbto,char* port_typefrom,char* port_typeto,char* link_type,char* link_information,int bidirectional_link_used, int last_rows,char* ErrMess)
	{
		char appliName[100]="InsertMacroLink";
		char sqlstmt[2500];
		OCIStmt *hstmt;
		OCIBind  *bndp[10];
		int rescode=0; 
		sword status;	
		char seq_name[100]="lhcb_macroconnseq.nextval";

		char* errmessg1=(char*)malloc(ERROR_BUFFER*sizeof(char));

		if(errmessg1==NULL)
		{
			GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
			return -1;
		}
		char* errmessg2=(char*)malloc(ERROR_BUFFER*sizeof(char));
		if(errmessg2==NULL)
		{
			GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
			free(errmessg1);
			return -1;
		}
		char port_typefrom1[100];
		if(port_typefrom!=NULL)
		{
			if(strlen(port_typefrom)<1)
				strcpy(port_typefrom1,"none");
			else
				strcpy(port_typefrom1,port_typefrom);
		}
		else
		{
			strcpy(port_typefrom1,"none");
		}

		char port_typeto1[100];
		if(port_typeto!=NULL)
		{
			if(strlen(port_typeto)<1)
				strcpy(port_typeto1,"none");
			else
				strcpy(port_typeto1,port_typeto);
		}
		else
		{
			strcpy(port_typeto1,"none");
		}

		int pfromlen=0;
		int ptolen=0;
		int nfromlen=0;
		int ntolen=0;
		int lktypelen=0;
		int ptypefromlen=0;
		int ptypetolen=0;
		int lkinfolen=0;
		int numrows=0;

		int pfrom=null_charptr(port_nbfrom, &pfromlen);
		int pto=null_charptr(port_nbto, &ptolen);
		int nodefrom=null_charptr(node_from, &nfromlen);
		int nodeto=null_charptr(node_to, &ntolen);
		int lktype=null_charptr(link_type, &lktypelen);
		int ptypefrom=null_charptr(port_typefrom, &ptypefromlen);
		int ptypeto=null_charptr(port_typeto, &ptypetolen);
		int lkinfo_null=null_charptr(link_information, &lkinfolen);

		int len_host=LOGIN_LEN;
		char login[LOGIN_LEN];
		char host[LOGIN_LEN];
		int res_query=getTerminalName(host,len_host,errmessg1);
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
			free(errmessg1);
			free(errmessg2);
			return -1;
		}

		try{
			HandleAlloc(ociEnv,(dvoid**)&hstmt,OCI_HTYPE_STMT,&status);
			sprintf(sqlstmt,"BEGIN insert ALL into %s (linkid,portidfrom,portidto,link_type,bidirectional_link_used,link_weight,lkused,link_info,author,user_update,terminal_name) values (%s,portidfrom,portidto,lknb,:biused, lkweight,1,:lkinfo,'%s','%s','%s') select f.portid as portidfrom,g.portid as portidto,s.linktypeID as lknb,decode(m.node,1,l.node+m.node+1,l.node+m.node) as lkweight from %s f,%s g,%s l,%s m,%s s ,%s k where f.deviceid=l.deviceid and l.devicename=:nfrom and g.deviceid=m.deviceid and m.devicename=:nto and s.link_name=:lktype and f.port_nbr=:portnbfrom and f.port_way=2 and nvl(f.port_type,'none')=:ptypefrom and g.port_nbr=:portnbto and g.port_way=1 and nvl(g.port_type,'none')=:ptypeto and mod(l.system_name,k.systemID)=0 and mod(m.system_name,k.systemID)=0 and rownum=1; if %s=1 then update %s r set r.system_name=(select case when ceil(power(10,sum(distinct log(10,t.systemID))))-power(10,sum(distinct log(10,t.systemID)))>0.5  then floor(power(10,sum(distinct log(10,t.systemid)))) else ceil(power(10,sum(distinct log(10,t.systemid)))) end from %s t,%s o,%s d,%s p,%s h where t.systemid not in (select q.systemid from %s v, %s q where mod(v.systemid,q.systemid)=0 and v.systemid!=q.systemid) and mod(o.system_name,t.systemid)=0 and mod(d.system_name,t.systemid)=0 and o.deviceid=p.deviceid and p.portid=r.portidfrom and r.portidto=h.portid and h.deviceid=d.deviceid) where r.system_name=-1 and r.portidfrom in (select v.portid from %s v,%s b where v.deviceid=b.deviceid and b.devicename=:nfrom) and r.portidto in (select v.portid from %s v,%s b where v.deviceid=b.deviceid and b.devicename=:nto);  end if;  :numrows:=%s; END;",MACRO_CONNECTIVITY_TABLE,seq_name,login,login,host,PORT_TABLE,PORT_TABLE,LOGICAL_DEVICE_TABLE,LOGICAL_DEVICE_TABLE,LINKTYPE_TABLE,SUBSYSTEM_TABLE,SQLROWCOUNT,MACRO_CONNECTIVITY_TABLE,SUBSYSTEM_TABLE,LOGICAL_DEVICE_TABLE,LOGICAL_DEVICE_TABLE,PORT_TABLE,PORT_TABLE,SUBSYSTEM_TABLE,SUBSYSTEM_TABLE,PORT_TABLE,LOGICAL_DEVICE_TABLE,PORT_TABLE,LOGICAL_DEVICE_TABLE,SQLROWCOUNT);	
			StmtPrepare(hstmt,ociError, sqlstmt, &status);
			BindByName(hstmt,&bndp[0],ociError,":portnbfrom",port_nbfrom,pfromlen,SQLT_STR,&pfrom,&status);
			BindByName(hstmt,&bndp[1],ociError,":portnbto",port_nbto,ptolen,SQLT_STR,&pto,&status);
			BindByName(hstmt,&bndp[2],ociError,":biused",&bidirectional_link_used,sizeof(bidirectional_link_used),SQLT_INT,0,&status);
			BindByName(hstmt,&bndp[3],ociError,":nfrom",node_from,nfromlen,SQLT_STR,&nodefrom,&status);
			BindByName(hstmt,&bndp[4],ociError,":nto",node_to,ntolen,SQLT_STR,&nodeto,&status);
			BindByName(hstmt,&bndp[5],ociError,":lktype",link_type,lktypelen,SQLT_STR,&lktype,&status);
			BindByName(hstmt,&bndp[6],ociError,":ptypefrom",port_typefrom1,ptypefromlen,SQLT_STR,&ptypefrom,&status);
			BindByName(hstmt,&bndp[7],ociError,":ptypeto",port_typeto1,ptypetolen,SQLT_STR,&ptypeto,&status);
			BindByName(hstmt,&bndp[8],ociError,":lkinfo",link_information,lkinfolen,SQLT_STR,&lkinfo_null,&status);
			BindByName(hstmt,&bndp[9],ociError,":numrows",&numrows,sizeof(numrows),SQLT_INT,0,&status);
			StmtExecute(ociHdbc, hstmt, ociError,1, &status);
	
		}catch(Error err){
			sprintf(appliName,"InsertMacroLink");	///////
			rescode=ShowErrors (status, err.ociError, err.log);
			
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrMess,1); 
			else
				GetErrorMess(appliName,err.msg,ErrMess,1);
			
			if(strstr(err.msg,"NOT CONNECTED TO ANY DB")!=NULL)
			return -1;
		}

		if(status==OCI_SUCCESS)
		{

			if(numrows==0)
			{
				//status = OCITransRollback(ociHdbc, ociError, 0);
				status = OCITransCommit(ociHdbc, ociError, 0);
				status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
				GetErrorMess(appliName,  "COULDNOT_INSERT_THIS_ROWS: CHECK SYSTEM COMPATIBILITY",ErrMess,1);
				return -1;
			}
			else
			{
				status = OCITransCommit(ociHdbc, ociError, 0);
				if(last_rows==1)
				{
					if(status!=OCI_SUCCESS)
					{
						if(rescode==0)	
							rescode=ShowErrors (status, ociError, "OCITransCommit unsuccessful");
					}
					else
					{
						char res_query1[100]="";
						int sysID=GetSubsystemID("DAQ,TFC");

                        try{
							sprintf(sqlstmt,"BEGIN :rescode:=%s(%d); :numrows:=%s; END; ",_UpdateDynamicTables,sysID,SQLROWCOUNT);
							StmtPrepare(hstmt,ociError, sqlstmt, &status);
							BindByName(hstmt,&bndp[9],ociError,":numrows",&numrows,sizeof(int),SQLT_INT,0,&status);
							BindByName(hstmt,&bndp[0],ociError,":rescode",res_query1,101,SQLT_STR,0,&status);
							StmtExecute(ociHdbc, hstmt, ociError,1, &status);
							TransCommit(ociHdbc, ociError, 0, &status);
			
						}catch(Error err){
							sprintf(appliName,"InsertMacroLink");	///////
							rescode=ShowErrors (status, err.ociError, err.log);
			
							if(ociError!=0)
								OCIReportError(ociError,appliName,ErrMess,1); 
							else
								GetErrorMess(appliName,err.msg,ErrMess,1);
						}
					}
				}
			}
		}
		status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
		if(rescode!=0)
			OCIReportError(ociError,appliName, ErrMess,1); 
		else
			OCIReportError(ociError,appliName, ErrMess,0); 

		return (rescode+status);
	}


/**************************************************************************************************/
/**
* Insert a Simple Link Type,  and returning an integer value.
* The user should manage the memory : there is no memory allocation.
* @param link_name : name of the link type
* @param last_rows : 1 if it's your last insert (save it into the database), 0 otherwise
* @param ErrMess : error message in case of failure
* @return 0 if it is successful
*/ 

EXTERN_CONFDB
	int InsertSimpleLinkType(char *link_name,int last_rows,char* ErrMess)
	{
		char appliName[100]="InsertSimpleLinkType";
		int numrows=0;
		char sqlstmt[1000];
		OCIStmt *hstmt;
		int rescode=0;
		OCIBind  *bndp[2];
		sword status;
		char seqname[100]="lhcb_lktypeseq.nextval";

		if(link_name==NULL)
		{
			GetErrorMess(appliName, "Link name can't be null",ErrMess,1);
			return -1;
		}
		char* errmessg1=(char*)malloc(ERROR_BUFFER*sizeof(char));
		if(errmessg1==NULL)
		{
			GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
			return -1;
		}
		char* errmessg2=(char*)malloc(ERROR_BUFFER*sizeof(char));
		if(errmessg2==NULL)
		{
			GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
			free(errmessg1);
			return -1;
		}

		int lkname=null_charptr(link_name);
		int len_host=LOGIN_LEN;
		char login[LOGIN_LEN];
		char host[LOGIN_LEN];
		int res_query=getTerminalName(host,len_host,errmessg1);
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
			free(errmessg1);
			free(errmessg2);
			return -1;
		}

		try{
			HandleAlloc(ociEnv,(dvoid**)&hstmt,OCI_HTYPE_STMT,&status);
			//use of insert all in order to put a bind variable and then do a bulk insertion
			sprintf(sqlstmt,"BEGIN insert ALL into %s(linktypeID,link_name,link_nbr,author,user_update,terminal_name) values (%s,:lkname,prime_nb,'%s','%s','%s') select prime_nb from %s  where prime_nb_position=(select nvl(count(link_nbr),0)+1 from %s ); :numrows:=%s; end;",LINKTYPE_TABLE,seqname,login,login,host,PRIME_NUMBER_TABLE,LINKTYPE_TABLE,SQLROWCOUNT);
			StmtPrepare(hstmt,ociError, sqlstmt, &status);
			BindByName(hstmt,&bndp[0],ociError,":lkname",link_name,strlen(link_name)+1,SQLT_STR,&lkname,&status);
			BindByName(hstmt,&bndp[1],ociError,":numrows",&numrows,sizeof(numrows),SQLT_INT,0,&status);
			StmtExecute(ociHdbc, hstmt, ociError,1, &status);
			
		}catch(Error err){
			sprintf(appliName,"InsertSimpleLinkType");	///////
			rescode=ShowErrors (status, err.ociError, err.log);
			
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrMess,1); 
			else
				GetErrorMess(appliName,err.msg,ErrMess,1);
			
			if(strstr(err.msg,"NOT CONNECTED TO ANY DB")!=NULL)
			return -1;
		}

		if(status==OCI_SUCCESS)
		{
			if(numrows==0)
			{
				//status = OCITransRollback(ociHdbc, ociError, 0);
				status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
				GetErrorMess(appliName,  "COULDNOT_INSERT_THIS_ROW",ErrMess,1);
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
		if(rescode!=0)
			OCIReportError(ociError,appliName, ErrMess,1); 
		else
			OCIReportError(ociError,appliName, ErrMess,0); 

		return (status+rescode);
	}
	
/*****************************************************************/
/**
* Insert a Composite Link Type, returning an integer value.
* The user should manage the memory : there is no memory allocation.
* @param link_name : name of the link type
* @param simple_lk_list : list of the simple_lk_list, separated by a coma: "data,control"
* @param last_rows : 1 if it's your last insert (save it into the database), 0 otherwise
* @param ErrMess : error message in case of failure
* @return 0 if it is successful
*/ 

EXTERN_CONFDB
	int InsertCompositeLinkType( char* link_name,char * simple_lk_list,int last_rows,char* ErrMess)
	{
		char appliName[100]="InsertCompositeLinkType";
		char seqname[100]="lhcb_lktypeseq.nextval";
		OCIBind  *bndp[3];
		int numrows=0;
		sword status;
		
		if(link_name==NULL)
		{
			GetErrorMess(appliName, "Link name can't be null",ErrMess,1);
			return -1;
		}
		if(simple_lk_list==NULL)
		{
			GetErrorMess(appliName, "simple_lk_list can't be null",ErrMess,1);
			return -1;
		}
		char* errmessg1=(char*)malloc(ERROR_BUFFER*sizeof(char));
		if(errmessg1==NULL)
		{
			GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
			return -1;
		}
		char* errmessg2=(char*)malloc(ERROR_BUFFER*sizeof(char));
		if(errmessg2==NULL)
		{
			GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
			free(errmessg1);
			return -1;
		}
		char sqlstmt[1000];
		OCIStmt *hstmt;
		int rescode=0;
		
		int len_host=LOGIN_LEN;
		char login[LOGIN_LEN];
		char host[LOGIN_LEN];
		int res_query=getTerminalName(host,len_host,errmessg1);
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
			free(errmessg1);
			free(errmessg2);
			return -1;
		}
		int lkname=null_charptr(link_name);
		int lknamelist=null_charptr(simple_lk_list);

		try{
			HandleAlloc(ociEnv,(dvoid**)&hstmt,OCI_HTYPE_STMT,&status);
			//use of log to do the product 
			sprintf(sqlstmt,"BEGIN insert ALL into %s (linktypeID,link_name,link_nbr,author,user_update,terminal_name) values (%s,:lkname,lknb,'%s','%s','%s') select case when ceil(power(10,sum(log(10,link_nbr))))-power(10,sum(log(10,link_nbr)))>0.5  then floor(power(10,sum(log(10,link_nbr)))) else ceil(power(10,sum(log(10,link_nbr)))) end as lknb from %s where instr(','||:lklist||',',','||link_name||',')>0; :numrows:=%s; END;",LINKTYPE_TABLE,seqname,login,login,host,LINKTYPE_TABLE,SQLROWCOUNT);
			StmtPrepare(hstmt,ociError, sqlstmt, &status);
			BindByName(hstmt,&bndp[0],ociError,":lkname",link_name,strlen(link_name)+1,SQLT_STR,&lkname,&status);
			BindByName(hstmt,&bndp[1],ociError,":lklist",simple_lk_list,strlen(simple_lk_list)+1,SQLT_STR, &lknamelist,&status);
			BindByName(hstmt,&bndp[2],ociError,":numrows",&numrows,sizeof(numrows),SQLT_INT,0,&status);
			StmtExecute(ociHdbc, hstmt, ociError,1, &status);
			
		}catch(Error err){
			sprintf(appliName,"InsertCompositeLinkType");	///////
			rescode=ShowErrors (status, err.ociError, err.log);
			
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrMess,1); 
			else
				GetErrorMess(appliName,err.msg,ErrMess,1);
			
			if(strstr(err.msg,"NOT CONNECTED TO ANY DB")!=NULL)
			return -1;
		}

		if(status==OCI_SUCCESS)
		{
			if(numrows==0)
			{
				//status = OCITransRollback(ociHdbc, ociError, 0);
				status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
				GetErrorMess(appliName,  "COULDNOT_INSERT_THIS_ROW",ErrMess,1);
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
		if(rescode!=0)
			OCIReportError(ociError,appliName, ErrMess,1); 
		else
			OCIReportError(ociError,appliName, ErrMess,0); 

		return (status+rescode);
	}
	

/**
* Insert  Port, returning an integer value.
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
* @param last_rows : 1 if it's your last Port insert (save it into the database), 0 otherwise
* @param ErrMess : error message in case of failure
* @return 0 if it is successful
*/ 

EXTERN_CONFDB
	int InsertPort(char* devicename,char* port_nb,int admin_status,int port_way,int speed,int pxi_booting,char* port_type,char* bia,char* ipadd1,char* ipname,char* subnet,char* macadd,char* phy,int last_rows,char* ErrMess)
	{
		char sqlstmt[2000];
		char appliName[100]="InsertPort";
		OCIStmt *hstmt;
		OCIBind  *bndp[15];
		int rescode=0; 
		sword status;
		char seq_name[100]="lhcb_portseq.nextval";
		int numrows=0;
		char* errmessg1=(char*)malloc(ERROR_BUFFER*sizeof(char));
		if(errmessg1==NULL)
		{
			GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
			return -1;
		}
		char* errmessg2=(char*)malloc(ERROR_BUFFER*sizeof(char));
		if(errmessg2==NULL)
		{
			GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
			free(errmessg1);
			return -1;
		}
		char ipadd[30];
		if(ipadd1!=NULL)
		{
			if(strlen(ipadd1)<5 ||strcspn(ipadd1,".")==strlen(ipadd1))
				strcpy(ipadd,"none");
			else
				strcpy(ipadd,ipadd1);
		}
		else
		{
			strcpy(ipadd,"none");
		}
		int devname1=null_charptr(devicename);
		int dphy1=null_charptr(phy);
		int dbia1=null_charptr(bia);
		int dportype1=null_charptr(port_type);
		int dportnb1=null_charptr(port_nb);
		int dipadd1=null_charptr(ipadd);
		int dipname1=null_charptr(ipname);
		int dmacadd1=null_charptr(macadd);
		int dsubnet1=null_charptr(subnet);

		int dnamelen=0;
		int dphylen=0;
		int dbialen=0;
		int dmaclen=0;
		int diplen=0;
		int dipnamelen=0;
		int dportnblen=0;
		int dsubnetlen=0;
		int dportypelen=0;
		if(port_nb!=NULL)
			dportnblen=strlen(port_nb)+1;
		if(ipadd!=NULL)
			diplen=strlen(ipadd)+1;
		if(ipname!=NULL)
			dipnamelen=strlen(ipname)+1;
		if(subnet!=NULL)
			dsubnetlen=strlen(subnet)+1;
		if(port_type!=NULL)
			dportypelen=strlen(port_type)+1;
		if(phy!=NULL)
			dphylen=strlen(phy)+1;
		if(bia!=NULL)
			dbialen=strlen(bia)+1;
		if(macadd!=NULL)
			dmaclen=strlen(macadd)+1;
		if(devicename!=NULL)
			dnamelen=strlen(devicename)+1;

		int len_host=LOGIN_LEN;
		char login[LOGIN_LEN];
		char host[LOGIN_LEN];
		int rescode_plsql=0;
		int res_query=getTerminalName(host,len_host,errmessg1);
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
			free(errmessg1);
			free(errmessg2);
			return -1;
		}

		try{
			HandleAlloc(ociEnv,(dvoid**)&hstmt,OCI_HTYPE_STMT,&status);
			sprintf(sqlstmt,"BEGIN if instr(:ipadd,'none')=0 then :rescode:=%s(:ipadd,:ipname,:subnet,'%s','%s'); end if; insert ALL WHEN (instr(:ipadd,'none')>0) then into %s (portid,deviceid,port_nbr,port_way,administrative_status,speed,port_type,phy,pxi_booting,ipaddress,author,user_update,terminal_name) values (%s,devid,:port_nbr,:portway,:adminstatus,:speed,:portype,:phy,:pxi,NULL,'%s','%s','%s') else into %s (serialnb,port_nbr,port_type,port_way,bia,macaddress,author,created,terminal_name) values (snb,:port_nbr,:portype,:portway,:bia,:mac,'%s',sysdate,'%s') into %s(portid,deviceid,port_nbr,port_way,administrative_status,speed,port_type,phy,pxi_booting,ipaddress,author,user_update,terminal_name) values (%s,devid,:port_nbr,:portway,:adminstatus,:speed,:portype,:phy,:pxi,:ipadd,'%s','%s','%s') select deviceid as devid,serialnb as snb from  %s  where devicename=:dname; :numrows:=%s; end; ",_insertIpaddress,login,host,PORT_TABLE,seq_name,login,login,host,HWPORT_TABLE,login,host,PORT_TABLE,seq_name,login,login,host,LOGICAL_DEVICE_TABLE,SQLROWCOUNT);	
			StmtPrepare(hstmt,ociError, sqlstmt, &status);
			BindByName(hstmt,&bndp[0],ociError,":port_nbr",port_nb,dportnblen,SQLT_STR,&dportnb1,&status);
			BindByName(hstmt,&bndp[1],ociError,":ipadd",ipadd,diplen,SQLT_STR,&dipadd1,&status);
			BindByName(hstmt,&bndp[2],ociError,":ipname",ipname,dipnamelen,SQLT_STR,&dipname1,&status);
			BindByName(hstmt,&bndp[3],ociError,":subnet",subnet,dsubnetlen,SQLT_STR,&dsubnet1,&status);
			BindByName(hstmt,&bndp[4],ociError,":portway",&port_way,sizeof(port_way),SQLT_INT,0,&status);
			BindByName(hstmt,&bndp[5],ociError,":speed",&speed,sizeof(speed),SQLT_INT,0,&status);
			BindByName(hstmt,&bndp[6],ociError,":adminstatus",&admin_status,sizeof(admin_status),SQLT_INT,0,&status);
			BindByName(hstmt,&bndp[7],ociError,":portype",port_type,dportypelen,SQLT_STR,&dportype1,&status);
			BindByName(hstmt,&bndp[8],ociError,":phy",phy,dphylen,SQLT_STR,&dphy1,&status);
			BindByName(hstmt,&bndp[9],ociError,":pxi",&pxi_booting,sizeof(pxi_booting),SQLT_INT,0,&status);
			BindByName(hstmt,&bndp[10],ociError,":bia",bia,dbialen,SQLT_STR,&dbia1,&status);
			BindByName(hstmt,&bndp[11],ociError,":mac",macadd,dmaclen,SQLT_STR,&dmacadd1,&status);
			BindByName(hstmt,&bndp[12],ociError,":dname",devicename,dnamelen,SQLT_STR,&devname1,&status);
			BindByName(hstmt,&bndp[13],ociError,":numrows",&numrows,sizeof(numrows),SQLT_INT,0,&status);
			BindByName(hstmt,&bndp[14],ociError,":rescode",&rescode_plsql,sizeof(rescode_plsql),SQLT_INT,0,&status);
			StmtExecute(ociHdbc, hstmt, ociError,1, &status);
				
		}catch(Error err){
			sprintf(appliName,"InsertPort");	///////
			rescode=ShowErrors (status, err.ociError, err.log);
			
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrMess,1); 
			else
				GetErrorMess(appliName,err.msg,ErrMess,1);
			
			if(strstr(err.msg,"NOT CONNECTED TO ANY DB")!=NULL)
			return -1;
		}

		if(status==OCI_SUCCESS)
		{
			if(numrows==0)
			{
				//status = OCITransRollback(ociHdbc, ociError, 0);
				status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
				GetErrorMess(appliName,  "COULDNOT_INSERT_THIS_ROW",ErrMess,1);
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
		if(rescode!=0)
			OCIReportError(ociError,appliName, ErrMess,1); 	
		else
			OCIReportError(ociError,appliName, ErrMess,0); 

		return (status+rescode);
	}


/**
* Insert  a spare Port (of a spare device), returning an integer value.
* The user should manage the memory : there is no memory allocation.
* @param serialnb : serialnb of the hw
* @param port_nb   : port nb
* @param port_way   : 1 if it's an input, 2 if it's an output : the reference is the data coming from the detector.
* @param port_type   : type of the port interface (control, data,... used for the DAQ). Put "" if none.
* @param bia : burned mac address (used for the DAQ, for addresses). Put "" if none.
* @param macadd : mac address (used for the DAQ, for addresses). Put "" if none.
* @param last_rows : 1 if it's your last Port insert (save it into the database), 0 otherwise
* @param ErrMess : error message in case of failure
* @return 0 if it is successful
*/ 

EXTERN_CONFDB
	int InsertSparePort(char* serialnb,char* port_nb,int port_way,char* port_type,char* bia,char* macadd,int last_rows,char* ErrMess)
	{
		char sqlstmt[1000];
		char appliName[100]="InsertSparePort";
		OCIStmt *hstmt;
		OCIBind  *bndp[7];
		int rescode=0; 
		sword status;
		char* errmessg1=(char*)malloc(ERROR_BUFFER*sizeof(char));
		if(errmessg1==NULL)
		{
			GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
			return -1;
		}
		char* errmessg2=(char*)malloc(ERROR_BUFFER*sizeof(char));
		if(errmessg2==NULL)
		{
			GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
			free(errmessg1);
			return -1;
		}
		int dsrlen=0;
		int dportnblen=0;
		int dportypelen=0;
		int dbialen=0;
		int dmaclen=0;	
		int numrows=0;

		int dserial1=null_charptr(serialnb, &dsrlen);
		int dportnb1=null_charptr(port_nb, &dportnblen);
		int dportype1=null_charptr(port_type, &dportypelen);
		int dbia1=null_charptr(bia, &dbialen);
		int dmacadd1=null_charptr(macadd, &dmaclen);
	
		int len_host=LOGIN_LEN;
		char login[LOGIN_LEN];
		char host[LOGIN_LEN];
		int res_query=getTerminalName(host,len_host,errmessg1);
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
			free(errmessg1);
			free(errmessg2);
			return -1;
		}

		//checking if there's row with the given serialnb, port_nb,port_way and port_type
		char* ErrorMsg=(char*)malloc(100*sizeof(char));
		char* port_row_result=new char[1000];
		int len_array=100;
		GetSparePortRow_snb(serialnb,port_nb,port_way,port_type,len_array, port_row_result,ErrorMsg);
		if(strncmp("NO_ERROR",ErrorMsg,8)==0){
			free(ErrorMsg);
			GetErrorMess(appliName, "SPARE PORT ALREADY EXISTS",ErrMess,1);
			return -1;
		}
		else

		try{
			HandleAlloc(ociEnv,(dvoid**)&hstmt,OCI_HTYPE_STMT,&status);
			sprintf(sqlstmt,"BEGIN insert into %s (serialnb,port_nbr,port_way,port_type,bia,macaddress,author,created,terminal_name) values (:snb,:port_nbr,:portway,:portype,:bia,:mac,'%s',sysdate,'%s'); :numrows:=%s; end; ",HWPORT_TABLE,login,host,SQLROWCOUNT);
			StmtPrepare(hstmt,ociError, sqlstmt, &status);
			BindByName(hstmt,&bndp[0],ociError,":port_nbr",port_nb,dportnblen,SQLT_STR,&dportnb1,&status);
			BindByName(hstmt,&bndp[1],ociError,":portway",&port_way,sizeof(port_way),SQLT_INT,0,&status);
			BindByName(hstmt,&bndp[2],ociError,":portype",port_type,dportypelen,SQLT_STR,&dportype1,&status);
			BindByName(hstmt,&bndp[3],ociError,":bia",bia,dbialen,SQLT_STR,&dbia1,&status);
			BindByName(hstmt,&bndp[4],ociError,":mac",macadd,dmaclen,SQLT_STR,&dmacadd1,&status);
			BindByName(hstmt,&bndp[5],ociError,":snb",serialnb,dsrlen,SQLT_STR,&dserial1,&status);
			BindByName(hstmt,&bndp[6],ociError,":numrows",&numrows,sizeof(numrows),SQLT_INT,0,&status);
			StmtExecute(ociHdbc, hstmt, ociError,1, &status);
	
		}catch(Error err){
			sprintf(appliName,"InsertSparePort");	///////
			rescode=ShowErrors (status, err.ociError, err.log);
			
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrMess,1); 
			else
				GetErrorMess(appliName,err.msg,ErrMess,1);
			
			if(strstr(err.msg,"NOT CONNECTED TO ANY DB")!=NULL)
			{
				free(ErrorMsg);
				return -1;
			}
		}

		if(status==OCI_SUCCESS)
		{
			if(numrows==0)
			{
				//status = OCITransRollback(ociHdbc, ociError, 0);
				status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
				GetErrorMess(appliName,  "COULDNOT_INSERT_THIS_ROW",ErrMess,1);
				free(ErrorMsg);
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
		if(rescode!=0)
			OCIReportError(ociError,appliName, ErrMess,1); 	
		else
			OCIReportError(ociError,appliName, ErrMess,0); 

		free(ErrorMsg);
		return (status+rescode);
	}


/**
* Insert a spare device into the  the hw table :it means that the hw device has the status SPARE and it's having the given functional fct returning an integer value.
* The user should manage the memory : there is no memory allocation.
* @param serial_nb :  serial_nb of the device
* @param hwname : you can put "spare_nb";
* @param hwtype : hardware type (market specif) prefixed by the subsystem like "VELO_", if the spare is specific to it.hw type is sth which specifies whta type of spare it is so that we can group spare. Ex. Tell1 board is a hw type. 
* @param responsible :guy responsible for this hw
* @param comments : some comments about the hw (max.1000 characters)
* @param location :  location of the spare :which building shelf?
* @param last_rows1 : 1 if it's your MultipleDevice last insert (save it into the database), 0 otherwise  Maximum in one go: 10000 rows.( we force the insert)
* @param ErrMess : error message in case of failure
* @return 0 if it is successful
*/

//extern "C" __declspec(dllexport)

EXTERN_CONFDB
	int InsertSpareDevice(char* hwname,char* hwtype,char* serial_nb,char* responsible,char* comments,char* location,int last_rows1,char* ErrMess)
	{
		char appliName[100]="InsertSpareDevice";
		char sqlstmt[2000];
		OCIStmt *hstmt;
		OCIBind  *bndp[7]; 
		int rescode=0;
		int res_query=0;
		sword status=0;
		char seqname[100]="lhcb_history_deviceseq.nextval";
		int numrows=0;
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
		int dname1=null_charptr(hwname);
		int dserialnb1=null_charptr(serial_nb);
		int dlocation1=null_charptr(location);
		int dresponsible1=null_charptr(responsible);
		int dhwtype1=null_charptr(hwtype);
		int dcomments1=null_charptr(comments);

		int hwnamelen=1;
		int hwtypelen=1;
		int serialnblen=1;
		int locationlen=1;
		int responsiblelen=1;
		int commentslen=1;
		if(serial_nb!=NULL)
			serialnblen=strlen(serial_nb);
		if(hwname!=NULL)
			hwnamelen=strlen(hwname);
		if(hwtype!=NULL)
			hwtypelen=strlen(hwtype);
		if(location!=NULL)
			locationlen=strlen(location);
		if(responsible!=NULL)
			responsiblelen=strlen(responsible);
		if(comments!=NULL)
			commentslen=strlen(comments);

		if(serial_nb==NULL)
		{
			GetErrorMess(appliName, "SERIAL_NB CAN'T BE NULL",ErrMess,1);
			return -1;
		}

		int len_host=LOGIN_LEN;
		char login[LOGIN_LEN];
		char host[LOGIN_LEN];
		res_query=getTerminalName(host,len_host,errmessg1);
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
			free(errmessg1);
			free(errmessg2);
			return -1;
		}
		//need to proceed with messages

		try{
			HandleAlloc(ociEnv,(dvoid**)&hstmt,OCI_HTYPE_STMT,&status);
			sprintf(sqlstmt,"BEGIN insert into %s(serialnb,hwtype,hwname,responsible,user_comments,location,device_status) values(:serialnb,:hwtype,:hwname,:responsible,:comments,:location,%d);insert into %s(historydeviceID,serialnb,device_status,user_comment,location,status_change,created,author,terminal_name) values(%s,:serialnb,'SPARE',:comments,:location,sysdate,sysdate,'%s','%s'); :numrows:=%s; END;",HW_DEVICE_TABLE,SPARE,HISTORY_DEVICE_TABLE,seqname,login,host,SQLROWCOUNT);
			StmtPrepare(hstmt,ociError, sqlstmt, &status);
			BindByName(hstmt,&bndp[0],ociError,":hwname",hwname,hwnamelen,SQLT_STR,&dname1,&status);
			BindByName(hstmt,&bndp[1],ociError,":serialnb",serial_nb,serialnblen,SQLT_STR,&dserialnb1,&status);
			BindByName(hstmt,&bndp[2],ociError,":location",location,locationlen,SQLT_STR,&dlocation1,&status);
			BindByName(hstmt,&bndp[3],ociError,":hwtype",hwtype,hwtypelen,SQLT_STR,&dhwtype1,&status);
			BindByName(hstmt,&bndp[4],ociError,":responsible",responsible,responsiblelen,SQLT_STR,&dresponsible1,&status);
			BindByName(hstmt,&bndp[5],ociError,":comments",comments,commentslen,SQLT_STR,&dcomments1,&status);
			BindByName(hstmt,&bndp[6],ociError,":numrows",&numrows,sizeof(numrows),SQLT_INT,0,&status);
			StmtExecute(ociHdbc, hstmt, ociError,1, &status);
			
		}catch(Error err){
			sprintf(appliName,"InsertSpareDevice");	///////
			rescode=ShowErrors (status, err.ociError, err.log);
			
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrMess,1); 
			else
				GetErrorMess(appliName,err.msg,ErrMess,1);
			
			if(strstr(err.msg,"NOT CONNECTED TO ANY DB")!=NULL)
			return -1;
		}

		if(status==OCI_SUCCESS)
		{
			if(numrows==0)
			{
				status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
				GetErrorMess(appliName,  "COULDNOT_INSERT_THIS_ROW",ErrMess,1);
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
		if(rescode!=0)
			OCIReportError(ociError,appliName, ErrMess,1); 	
		else
			OCIReportError(ociError,appliName, ErrMess,0); 

		return (status+rescode);
	}


	
/**
* Insert a test functional device into the functional_device table (its name is TEST_BOARD_XX; these boards will be used for local tests in the pits returning an integer value.
* The user should manage the memory : there is no memory allocation.
* Put all the value which are not null first..
* @param location : where these tests boards are sit
* @param ipaddressList : list of the ip address : each element is separated with '\\0'(same order to enable the association between mac, port nbr, port_type and port way
* @param ipaddlen : total length of the ipaddress list, (correspond to your memory allocation) ;
* @param subnetList : list of the subnet '\\0'(same order to enable the association between mac, port nbr, port_type and port way)
* @param subnetlen : total length of the subnet list, (correspond to your memory allocation) ;
* @param ipnameList : list of the ipname list, included '\\0' (same order to enable the association between mac, port nbr, port_type and port way);
* @param ipnamelen : total length of the ipname list, (correspond to your memory allocation) ;
* @param portnbList : list of portnb '\\0'(same order to enable the association between mac, port nbr, port_type and port way)
* @param portnblen : total length of the portnb list, (correspond to your memory allocation) ;
* @param portypeList : list of portype '\\0'(same order to enable the association between mac, port nbr, port_type and port way)
* @param ptypelen : total length of list of port type , (correspond to your memory allocation) ;
* @param nb_ipadd :  nb of ip addresses in the list=length of the portwayList
* @param portwayList : list of portway (same order to enable the association between mac, port nbr, port_type and port way)
* @param location :  location of the device : crate\\rack\\slot (as defined in the equipment DB) or on the detector
* @param last_rows : 1 if it's your MultipleDevice last insert (save it into the database), 0 otherwise  Maximum in one go: 10000 rows.( we force the insert)
* @param ErrMess : error message in case of failure
* @return 0 if it is successful
*/
/*****************************************************/

EXTERN_CONFDB
	int InsertTestFunctionalDevice(char* location,char* ipaddressList,int ipaddlen,char* ipnameList,int ipnamelen,char* subnetList,int subnetlen,char* portnbList,int portnblen,char* portypeList,int ptypelen,int* portwayList,int nb_ipadd,int last_rows,char* ErrMess)
	{
		char appliName[100]="InsertTestFunctionalDevice";
		char sqlstmt[1000];
		OCIStmt *hstmt;
		OCIBind  *bndp[7]; 
		int rescode=0;
		sword status;
		int numrows=0;
		int len_host=LOGIN_LEN;
		char login[LOGIN_LEN];
		char host[LOGIN_LEN];
		char* errmessg1=(char*)malloc(ERROR_BUFFER*sizeof(char));
		char seqname[100]="lhcb_lg_deviceseq.nextval";
		int	pos1=0;
		int	pos2=0;
		int	pos3=0;
		int	pos4=0;
		int	pos5=0;
		int	len1=0;
		int	len2=0;
		int	len3=0;
		int	len4=0;
		int	len5=0;
		int	i=0;
		char logMess[100];
		int ipadd_maxlen=0;
		int subnet_maxlen=0;
		int portnb_maxlen=0;
		int portype_maxlen=0;
		int ipname_maxlen=0;
		char* ipnameVector=NULL;
		char* ipaddVector=NULL;
		char* subnetVector=NULL;
		char* ptnbVector=NULL;
		char* ptypeVector=NULL;
		int* ipnamenullVector=NULL;
		int* ptnbnullVector=NULL;
		int* ptypenullVector=NULL;
		int* subnetnullVector=NULL;
		int* ipaddnullVector=NULL;
		int* numrows_inserted=NULL;

		int dnamelen=0;
		int rescode_plsql=-1000;
		int devid=-1000;

		int dlocation=null_charptr(location, &dnamelen);

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
		int res_query=getTerminalName(host,len_host,errmessg1);
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
			free(errmessg1);
			free(errmessg2);
			return -1;
		}

		try{
			HandleAlloc(ociEnv,(dvoid**)&hstmt,OCI_HTYPE_STMT,&status);
			sprintf(sqlstmt,"BEGIN %s(:location,'%s','%s',:rescode,:devid); end;",_InsertTestBoard,login,host);
			StmtPrepare(hstmt,ociError, sqlstmt, &status);
			BindByName(hstmt,&bndp[0],ociError,":location",location,dnamelen,SQLT_STR,&dlocation,&status);
			BindByName(hstmt,&bndp[1],ociError,":rescode",&rescode_plsql,sizeof(rescode_plsql),SQLT_INT,0,&status);
			BindByName(hstmt,&bndp[2],ociError,":devid",&devid,sizeof(devid),SQLT_INT,0,&status);
			StmtExecute(ociHdbc, hstmt, ociError,1, &status);

		}catch(Error err){
			sprintf(appliName,"InsertTestFunctionalDevice");	///////
			rescode=ShowErrors (status, err.ociError, err.log);
			
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrMess,1); 
			else
				GetErrorMess(appliName,err.msg,ErrMess,1);
			
			if(strstr(err.msg,"NOT CONNECTED TO ANY DB")!=NULL)			
				return -1;
		}

		if(rescode_plsql!=0)
		{
			status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
			sprintf(logMess,"ORACLE_ERROR_%d",rescode_plsql);
			GetErrorMess(appliName,logMess,ErrMess,1);
			return -1;
		}
		else

		try{
			sprintf(sqlstmt,"BEGIN insert into %s (ipaddress,subnet_info,ipname,terminal_name,author,created) values(:ipadd,:subnet,:ipname,'%s','%s',sysdate);  insert into %s (portid,deviceid,port_nbr,port_way,port_type,ipaddress,author,terminal_name ) values (%s,%d,:ptnb,:ptway,:ptype,:ipadd,'%s','%s'); :numrows:=%s; end;" ,IPINFO_TABLE,host,login,PORT_TABLE,seqname,devid,login,host, SQLROWCOUNT);
			StmtPrepare(hstmt,ociError, sqlstmt, &status);

			ipaddnullVector=(int*)malloc(nb_ipadd*sizeof(int));
			ipnamenullVector=(int*)malloc(nb_ipadd*sizeof(int));
			ptnbnullVector=(int*)malloc(nb_ipadd*sizeof(int));
			ptypenullVector=(int*)malloc(nb_ipadd*sizeof(int));
			subnetnullVector=(int*)malloc(nb_ipadd*sizeof(int));
			numrows_inserted=(int*)malloc(nb_ipadd*sizeof(int));
			if(numrows_inserted==NULL||ipaddnullVector==NULL||ipnamenullVector==NULL||ptnbnullVector==NULL||ptypenullVector==NULL||subnetnullVector==NULL)
			{
				if(ipaddnullVector!=NULL)
					free(ipaddnullVector);
				if(ipnamenullVector!=NULL)
					free(ipnamenullVector);
				if(ptnbnullVector!=NULL)
					free(ptnbnullVector);
				if(ptypenullVector!=NULL)
					free(ptypenullVector);
				if(subnetnullVector!=NULL)
					free(subnetnullVector);
				if(numrows_inserted!=NULL)
					free(numrows_inserted);

				status = OCITransRollback(ociHdbc, ociError, 0);
				status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
				GetErrorMess(appliName,  "MALLOC_UNSUCCESSFUL ",ErrMess,1);
				return -1;
			}

			while(i<nb_ipadd)
			{
				if((ipaddressList+pos1)!=NULL && pos1<ipaddlen)
				{
					len1=strlen(ipaddressList+pos1);
					ipaddnullVector[i]=0;
					pos1+=len1+1;
					if(len1>ipadd_maxlen)
						ipadd_maxlen=len1+1;
				}
				else
				{
					ipaddnullVector[i]=-1;
					pos1++;
				}
				if((ipnameList+pos5)!=NULL && pos5<ipnamelen)
				{
					len5=strlen(ipnameList+pos5);
					ipnamenullVector[i]=0;
					pos5+=len5+1;
					if(len5>ipname_maxlen)
						ipname_maxlen=len5+1;
				}
				else
				{
					ipnamenullVector[i]=-1;
					pos5++;
				}
				if((subnetList+pos2)!=NULL && pos2<subnetlen)
				{
					len2=strlen(subnetList+pos2);
					subnetnullVector[i]=0;
					pos2+=len2+1;
					if(len2>subnet_maxlen)
						subnet_maxlen=len2+1;
				}
				else
				{
					subnetnullVector[i]=-1;
					pos2++;
				}
				if((portnbList+pos3)!=NULL && pos3<portnblen)
				{
					len3=strlen(portnbList+pos3);
					ptnbnullVector[i]=0;
					pos3+=len3+1;
					if(len3>portnb_maxlen)
						portnb_maxlen=len3+1;
				}
				else
				{
					ptnbnullVector[i]=-1;
					pos3++;
				}

				if((portypeList+pos4)!=NULL && pos4<ptypelen)
				{
					len4=strlen(portypeList+pos4);
					ptypenullVector[i]=0;
					pos4+=len4+1;
					if(len4>portype_maxlen)
						portype_maxlen=len4+1;
				}
				else
				{
					ptypenullVector[i]=-1;
					pos4++;
				}
				numrows_inserted[i]=0;

				i++;
			}
		
			ipaddVector=(char*)malloc(ipadd_maxlen*nb_ipadd*sizeof(char));
			if (ipaddVector!=NULL)
				rescode=NormalizeVector(ipaddressList, nb_ipadd,ipadd_maxlen,ipaddVector);
			else
			{
				status = OCITransRollback(ociHdbc, ociError, 0);
				status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
				if(ipaddnullVector!=NULL)
					free(ipaddnullVector);
				if(ipnamenullVector!=NULL)
					free(ipnamenullVector);
				if(ptnbnullVector!=NULL)
					free(ptnbnullVector);
				if(ptypenullVector!=NULL)
					free(ptypenullVector);
				if(subnetnullVector!=NULL)
					free(subnetnullVector);
				free(numrows_inserted);

				GetErrorMess(appliName,  "MALLOC_UNSUCCESSFUL ",ErrMess,1);
				return -1;
			}

			ipnameVector=(char*)malloc(ipname_maxlen*nb_ipadd*sizeof(char));
			if (ipnameVector!=NULL)
				rescode=NormalizeVector(ipnameList, nb_ipadd,ipname_maxlen,ipnameVector);
			else
			{
				status = OCITransRollback(ociHdbc, ociError, 0);
				status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
				free(ipaddVector);
				if(ipaddnullVector!=NULL)
					free(ipaddnullVector);
				if(ipnamenullVector!=NULL)
					free(ipnamenullVector);
				if(ptnbnullVector!=NULL)
					free(ptnbnullVector);
				if(ptypenullVector!=NULL)
					free(ptypenullVector);
				if(subnetnullVector!=NULL)
					free(subnetnullVector);
				free(numrows_inserted);
	
				GetErrorMess(appliName,  "MALLOC_UNSUCCESSFUL ",ErrMess,1);
				return -1;
			}
			subnetVector=(char*)malloc(subnet_maxlen*nb_ipadd*sizeof(char));
			if (subnetVector!=NULL)
				rescode=NormalizeVector(subnetList, nb_ipadd,subnet_maxlen,subnetVector);
			else
			{
				status = OCITransRollback(ociHdbc, ociError, 0);
				status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
				free(ipaddVector);
				free(ipnameVector);
				if(ipaddnullVector!=NULL)
					free(ipaddnullVector);
				if(ipnamenullVector!=NULL)
					free(ipnamenullVector);
				if(ptnbnullVector!=NULL)
					free(ptnbnullVector);
				if(ptypenullVector!=NULL)
					free(ptypenullVector);
				if(subnetnullVector!=NULL)
					free(subnetnullVector);
				free(numrows_inserted);
	
				GetErrorMess(appliName,  "MALLOC_UNSUCCESSFUL ",ErrMess,1);
				return -1;
			}
			ptypeVector=(char*)malloc(portype_maxlen*nb_ipadd*sizeof(char));
			if(ptypeVector!=NULL)
				rescode=NormalizeVector(portypeList, nb_ipadd,portype_maxlen,ptypeVector);
			else
			{
				status = OCITransRollback(ociHdbc, ociError, 0);
				status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
				free(ipaddVector);
				free(subnetVector);
				free(ipnameVector);
				if(ipaddnullVector!=NULL)
					free(ipaddnullVector);
				if(ipnamenullVector!=NULL)
					free(ipnamenullVector);
				if(ptnbnullVector!=NULL)
					free(ptnbnullVector);
				if(ptypenullVector!=NULL)
					free(ptypenullVector);
				if(subnetnullVector!=NULL)
					free(subnetnullVector);
				free(numrows_inserted);
				GetErrorMess(appliName,  "MALLOC_UNSUCCESSFUL ",ErrMess,1);
				return -1;
			}
			ptnbVector=(char*)malloc(portnb_maxlen*nb_ipadd*sizeof(char));
			if(ptnbVector!=NULL)
				rescode=NormalizeVector(portnbList, nb_ipadd,portnb_maxlen,ptnbVector);
			else
			{
				status = OCITransRollback(ociHdbc, ociError, 0);
				status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
				free(ipaddVector);
				free(subnetVector);
				free(ptypeVector);
				free(ipnameVector);
				if(ipaddnullVector!=NULL)
					free(ipaddnullVector);
				if(ipnamenullVector!=NULL)
					free(ipnamenullVector);
				if(ptnbnullVector!=NULL)
					free(ptnbnullVector);
				if(ptypenullVector!=NULL)
					free(ptypenullVector);
				if(subnetnullVector!=NULL)
					free(subnetnullVector);
				free(numrows_inserted);
				GetErrorMess(appliName,  "MALLOC_UNSUCCESSFUL ",ErrMess,1);
				return -1;
			}

			BindByName(hstmt,&bndp[0],ociError,":subnet",subnetVector,subnet_maxlen,SQLT_STR,&subnetnullVector[0],&status);
			BindByName(hstmt,&bndp[1],ociError,":ptnb",ptnbVector,portnb_maxlen,SQLT_STR,&ptnbnullVector[0],&status);
			BindByName(hstmt,&bndp[2],ociError,":ipname",ipnameVector,ipname_maxlen,SQLT_STR, &ipnamenullVector[0],&status);
			BindByName(hstmt,&bndp[3],ociError,":ptway",&portwayList[0],sizeof(int),SQLT_INT,0,&status);
			BindByName(hstmt,&bndp[4],ociError,":ipadd",ipaddVector,ipadd_maxlen,SQLT_STR, &ipaddnullVector[0],&status);
			BindByName(hstmt,&bndp[5],ociError,":ptype",ptypeVector,portype_maxlen,SQLT_STR,&ptypenullVector[0],&status);
			BindByName(hstmt,&bndp[6],ociError,":numrows",&numrows_inserted[0],sizeof(int),SQLT_INT,0,&status);

			BindArrayOfStruct(bndp[0],ociError,subnet_maxlen,sizeof(int),&status);
			BindArrayOfStruct(bndp[1],ociError,portnb_maxlen,sizeof(int),&status);
			BindArrayOfStruct(bndp[2],ociError,ipname_maxlen,sizeof(int),&status);
			BindArrayOfStruct(bndp[3],ociError,sizeof(int),0,&status);
			BindArrayOfStruct(bndp[4],ociError,ipadd_maxlen,sizeof(int),&status);
			BindArrayOfStruct(bndp[5],ociError,portype_maxlen,sizeof(int),&status);
			BindArrayOfStruct(bndp[6],ociError,sizeof(int),0,&status);
			StmtExecute(ociHdbc, hstmt, ociError,nb_ipadd,&status);
	
		}catch(Error err){
			sprintf(appliName,"InsertTestFunctionalDevice");	///////
			rescode=ShowErrors (status, err.ociError, err.log);
			
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrMess,1); 
			else
				GetErrorMess(appliName,err.msg,ErrMess,1);
		}

		if(status==OCI_SUCCESS)
		{
			numrows=-1;
			for(i=0;i< nb_ipadd;i++)
			{
				if(numrows_inserted[i]==0)
				{
					numrows=0;
					i=  nb_ipadd+5;
				}
				else
					numrows=numrows_inserted[i];
			}
			if(numrows==0)
			{
				status = OCITransRollback(ociHdbc, ociError, 0);
				status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
				if(ipaddVector!=NULL)
					free(ipaddVector);
				if(ptnbVector!=NULL)
					free(ptnbVector);
				if(subnetVector!=NULL)
					free(subnetVector);
				if(ptypeVector!=NULL)
					free(ptypeVector);
				if(ipnameVector!=NULL)
					free(ipnameVector);
				if(ipaddnullVector!=NULL)
					free(ipaddnullVector);
				if(ipnamenullVector!=NULL)
					free(ipnamenullVector);
				if(ptnbnullVector!=NULL)
					free(ptnbnullVector);
				if(ptypenullVector!=NULL)
					free(ptypenullVector);
				if(subnetnullVector!=NULL)
					free(subnetnullVector);
				free(numrows_inserted);
				GetErrorMess(appliName,  "COULDNOT_INSERT_THIS_ROW",ErrMess,1);
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
		if(ipaddVector!=NULL)
			free(ipaddVector);
		if(ptnbVector!=NULL)
			free(ptnbVector);
		if(subnetVector!=NULL)
			free(subnetVector);
		if(ptypeVector!=NULL)
			free(ptypeVector);
		if(ipnameVector!=NULL)
			free(ipnameVector);
		if(ipaddnullVector!=NULL)
			free(ipaddnullVector);
		if(ipnamenullVector!=NULL)
			free(ipnamenullVector);
		if(ptnbnullVector!=NULL)
			free(ptnbnullVector);
		if(ptypenullVector!=NULL)
			free(ptypenullVector);
		if(subnetnullVector!=NULL)
			free(subnetnullVector);
		free(numrows_inserted);

		status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
		if(rescode!=0)
			OCIReportError(ociError,appliName, ErrMess,1); 
		else
			OCIReportError(ociError,appliName, ErrMess,0); 

		return (status+rescode);
	}


/**************************************************************************************************/
/**
* Insert a Simple function type,  and returning an integer value.
* The user should manage the memory : there is no memory allocation.
* @param function_name : name of the link type
* @param last_rows : 1 if it's your last insert (save it into the database), 0 otherwise
* @param ErrMess : error message in case of failure
* @return 0 if it is successful
*/ 

EXTERN_CONFDB
	int InsertSimpleDeviceFunction(char *function_name,int last_rows,char* ErrMess)
	{
		char appliName[100]="InsertSimpleDeviceFunction";
		int numrows=0;
		char sqlstmt[1000];
		OCIStmt *hstmt;
		int rescode=0;
		OCIBind  *bndp[2];
		sword status;

		if(function_name==NULL)
		{
			GetErrorMess(appliName, "function name can't be null",ErrMess,1);
			return -1;
		}
		char* errmessg1=(char*)malloc(ERROR_BUFFER*sizeof(char));
		if(errmessg1==NULL)
		{
			GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
			return -1;
		}
		char* errmessg2=(char*)malloc(ERROR_BUFFER*sizeof(char));
		if(errmessg2==NULL)
		{
			GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
			free(errmessg1);
			return -1;
		}

		int fcname=null_charptr(function_name);
		int len_host=LOGIN_LEN;
		char login[LOGIN_LEN];
		char host[LOGIN_LEN];
		int res_query=getTerminalName(host,len_host,errmessg1);
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
			free(errmessg1);
			free(errmessg2);
			return -1;
		}
		
		try{
			HandleAlloc(ociEnv,(dvoid**)&hstmt,OCI_HTYPE_STMT,&status);
			//use of insert all in order to put a bind variable and then do a bulk insertion
			sprintf(sqlstmt,"BEGIN insert ALL into %s(function_name,functionID,author,user_update,terminal_name) values (:fcname,prime_nb,'%s','%s','%s') select prime_nb from %s  where prime_nb_position=(select nvl(count(functionID),0)+1 from %s ); :numrows:=%s; end;",DEVICE_FUNCTION_TABLE,login,login,host,PRIME_NUMBER_TABLE,DEVICE_FUNCTION_TABLE,SQLROWCOUNT);
			StmtPrepare(hstmt,ociError, sqlstmt, &status);
			BindByName(hstmt,&bndp[0],ociError,":fcname",function_name,strlen(function_name)+1,SQLT_STR,&fcname,&status);
			BindByName(hstmt,&bndp[1],ociError,":numrows",&numrows,sizeof(numrows),SQLT_INT,0,&status);
			StmtExecute(ociHdbc, hstmt, ociError,1, &status);
		
		}catch(Error err){
			sprintf(appliName,"InsertSimpleDeviceFunction");	///////
			rescode=ShowErrors (status, err.ociError, err.log);
			
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrMess,1); 
			else
				GetErrorMess(appliName,err.msg,ErrMess,1);
			
			if(strstr(err.msg,"NOT CONNECTED TO ANY DB")!=NULL)
			return -1;
		}

		if(status==OCI_SUCCESS)
		{
			if(numrows==0)
			{
				//status = OCITransRollback(ociHdbc, ociError, 0);
				status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
				GetErrorMess(appliName,  "COULDNOT_INSERT_THIS_ROW",ErrMess,1);
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
		if(rescode!=0)
			OCIReportError(ociError,appliName, ErrMess,1); 
		else
			OCIReportError(ociError,appliName, ErrMess,0); 

		return (status+rescode);

	}


/**************************************************************************************************/
/**
* Insert a subsystem,  and returning an integer value.
* The user should manage the memory : there is no memory allocation.
* @param system_name : name of the subsystem. For instance VELO, OT_A
* @param parent_sysname : Put "none", if the system has no parent system name in the FSM tree, such as VELO, RICH. Otherwise put the parent system name. For instance for VELO_A->VELO, RICH1->RICH, IT->ST.
* @param ErrMess : error message in case of failure
* @return 0 if it is successful
*/ 

EXTERN_CONFDB
	int InsertSubsystem(char *system_name,char* parent_sysname,char* ErrMess)
	{
		char appliName[100]="InsertSubsystem";
		int rescode_len=1000;
		char rescode_plsql[1000];
		int rescode=0;
		char sqlstmt[1000];
		OCIStmt *hstmt;
		OCIBind  *bndp[3];
		sword status;
		int sysname=null_charptr(system_name);
		int psysname=null_charptr(parent_sysname);
		//int rescode_temp=null_charptr(rescode_plsql);
		std::cout<<"sysname="<<system_name<<" and parent="<<parent_sysname<<std::endl;
		if(system_name==NULL)
		{
			GetErrorMess(appliName, "system name can't be null",ErrMess,1);
			return -1;
		}

		int sysID = GetSubsystemID(system_name);
		if(sysID>0)
		{
			GetErrorMess(appliName, "SYSTEM NAME ALREADY EXISTS",ErrMess,1);
			return -1;
		}
		try{
			HandleAlloc(ociEnv,(dvoid**)&hstmt,OCI_HTYPE_STMT,&status);
			//use of insert all in order to put a bind variable and then do a bulk insertion
			sprintf(sqlstmt,"BEGIN :rescode:=%s(:sysname,:parent_sysname);  end;",_InsertSubsystem);
			StmtPrepare(hstmt,ociError, sqlstmt, &status);
			BindByName(hstmt,&bndp[0],ociError,":sysname",system_name,strlen(system_name)+1,SQLT_STR,&sysname,&status);

			//BindByName(hstmt,&bndp[1],ociError,":rescode",rescode_plsql,rescode_len,&status);
			BindByName(hstmt,&bndp[1],ociError,":rescode",rescode_plsql,rescode_len,SQLT_STR,0,&status);
			

			BindByName(hstmt,&bndp[2],ociError,":parent_sysname",parent_sysname,strlen(parent_sysname)+1,SQLT_STR,&psysname,&status);
			StmtExecute(ociHdbc, hstmt, ociError,1, &status);

		}catch(Error err){
			sprintf(appliName,"InsertSubsystem");	///////
			rescode=ShowErrors (status, err.ociError, err.log);
			
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrMess,1); 
			else
				GetErrorMess(appliName,err.msg,ErrMess,1);
			
			if(strstr(err.msg,"NOT CONNECTED TO ANY DB")!=NULL)
			return -1;
		}

		if(status==OCI_SUCCESS)
		{
			status = OCITransCommit(ociHdbc, ociError, 0);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCITransCommit unsuccessful");
			}	
		}

		status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
		if(strstr(rescode_plsql,"NO_ERROR")==NULL)
		{
			GetErrorMess(appliName,  "COULDNOT_INSERT_THIS_ROW",ErrMess,1);
		}
		else
		{
			if(rescode!=0)
				OCIReportError(ociError,appliName, ErrMess,1); 
			else
				OCIReportError(ociError,appliName, ErrMess,0); 
		}
		return (status+rescode);
	}

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif
