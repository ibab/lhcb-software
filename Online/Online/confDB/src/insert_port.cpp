/********************************************************************************/
//	Author: L.Abadie
// version:v3.5
// changes: handling error such that can be extended to C++: (create a class SQLException) (strlen ck + update stmt)
// file which performs bulk collect insert for the port table
// N.B : Memory management (allocation+release) should be handled by the user application thus, for all the fct provided by this lib
/********************************************************************************/

#include "list_structures.h"
#include "system_info.h"
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

	/***********************************GV for port***************************************************/
	int FIRST_TIME_PORT=0;
	char* _devicenameList1;
	int _devicenameNbElement1;
	int _max_devicename_len1;
	int _devicenameList_len1;

	int* _devicename2_nullvalue;
	int _devicename2NbEl;

	char* _port_nbrList;
	int _port_nbrNbElement;
	int _max_port_nbr_len;
	int _port_nbrList_len;
	int* _port_nbr_nullvalue;
	int _port_nbrNbEl;

	int* _adminstatus;
	int _adminstatusNbElement;

	int* _portwayList;
	int _portwayNbElement;

	int* _speedList;
	int _speedNbElement;

	int* _pxibootingList;
	int _pxibootingNbElement;


	char* _portypeList;
	int _portypeNbElement;
	int _portypeList_len;
	int _max_portype_len;

	int* _portype_nullvalue;
	int _portypeNbEl;

	char* _biaList;
	int _biaNbElement;
	int _biaList_len;
	int _max_bia_len;

	int* _bia_nullvalue;
	int _biaNbEl;

	char* _macaddressList;
	int _macaddressNbElement;
	int _macaddressList_len;
	int _max_macaddress_len;

	int* _macaddress_nullvalue;
	int _macaddressNbEl;

	char* _ipaddressList;
	int _ipaddressNbElement;
	int _ipaddressList_len;
	int _max_ipaddress_len;

	int* _ipaddress_nullvalue;
	int _ipaddressNbEl;

	char* _ipnameList;
	int _ipnameNbElement;
	int _ipnameList_len;
	int _max_ipname_len;

	int* _ipname_nullvalue;
	int _ipnameNbEl;

	char* _subnetList;
	int _subnetNbElement;
	int _subnetList_len;
	int _max_subnet_len;

	int* _subnet_nullvalue;
	int _subnetNbEl;

	char* _phyList;
	int _phyNbElement;
	int _max_phy_len;
	int _phyList_len;

	int* _phy_nullvalue;
	int _phyNbEl;



	int FIRST_TIME_PORT_SPARE=0;
	char* _sparesnbList1;
	int _sparesnbNbElement1;
	int _max_sparesnb_len1;
	int _sparesnbList_len1;

	int* _sparesnb2_nullvalue;
	int _sparesnb2NbEl;

	char* _spareport_nbrList;
	int _spareport_nbrNbElement;
	int _max_spareport_nbr_len;
	int _spareport_nbrList_len;
	int* _spareport_nbr_nullvalue;
	int _spareport_nbrNbEl;


	int* _spareportwayList;
	int _spareportwayNbElement;



	char* _spareportypeList;
	int _spareportypeNbElement;
	int _spareportypeList_len;
	int _max_spareportype_len;

	int* _spareportype_nullvalue;
	int _spareportypeNbEl;

	char* _sparebiaList;
	int _sparebiaNbElement;
	int _sparebiaList_len;
	int _max_sparebia_len;

	int* _sparebia_nullvalue;
	int _sparebiaNbEl;

	char* _sparemacaddressList;
	int _sparemacaddressNbElement;
	int _sparemacaddressList_len;
	int _max_sparemacaddress_len;

	int* _sparemacaddress_nullvalue;
	int _sparemacaddressNbEl;

	int FIRST_TIME_IP_ALIAS=0;

	char* _ipAliasnbList;
	int _ipAliasNbElement;
	int _ipAliasList_len;
	int _max_ipAlias_len;

	int* _ipAlias_nullvalue;
	int _ipAliasNbEl;

	char* _ipNamenbList;
	int _ipNameNbElement;
	int _ipNameList_len;
	int _max_ipName_len;

	int* _ipName_nullvalue;
	int _ipNameNbEl;


	char* _ipAddnbList;
	int _ipAddNbElement;
	int _ipAddList_len;
	int _max_ipAdd_len;

	int* _ipAdd_nullvalue;
	int _ipAddNbEl;

	int freeIPAliases()
	{
		int status=0;
		_ipAliasnbList=(char*)realloc(_ipAliasnbList,0*sizeof(char));
		_ipAliasnbList=NULL;

		_ipNamenbList=(char*)realloc( _ipNamenbList,0*sizeof(char));
		_ipNamenbList=NULL;

		_ipAddnbList=(char*)realloc(_ipAddnbList,0*sizeof(char));
		_ipAddnbList=NULL;

		_ipAlias_nullvalue=(int*)realloc(_ipAlias_nullvalue,0*sizeof(int));
		_ipAlias_nullvalue=NULL;

		_ipName_nullvalue=(int*)realloc(_ipName_nullvalue,0*sizeof(int));
		_ipName_nullvalue=NULL;

		_ipAdd_nullvalue=(int*)realloc(_ipAdd_nullvalue,0*sizeof(int));
		_ipAdd_nullvalue=NULL;

		FIRST_TIME_IP_ALIAS=0;

		return status;
	}

	int freePort()
	{
		int status=0;
		_devicenameList1=(char*)realloc(_devicenameList1,0*sizeof(char));
		_devicenameList1=NULL;

		_port_nbrList=(char*)realloc( _port_nbrList,0*sizeof(char));
		_port_nbrList=NULL;

		_adminstatus=(int*)realloc(_adminstatus,0*sizeof(int));
		_adminstatus=NULL;


		_portwayList=(int*)realloc(_portwayList,0*sizeof(int));
		_portwayList=NULL;

		_speedList=(int*)realloc(_speedList,0*sizeof(int));
		_speedList=NULL;

		_pxibootingList=(int*)realloc(_pxibootingList,0*sizeof(int));
		_pxibootingList=NULL;


		_portypeList=(char*)realloc(_portypeList,0*sizeof(char));
		_portypeList=NULL;

		_biaList=(char*)realloc(_biaList,0*sizeof(char));
		_biaList=NULL;

		_macaddressList=(char*)realloc(_macaddressList,0*sizeof(char));
		_macaddressList=NULL;

		_ipaddressList=(char*)realloc(_ipaddressList,0*sizeof(char));
		_ipaddressList=NULL;


		_ipnameList=(char*)realloc(_ipnameList,0*sizeof(char));
		_ipnameList=NULL;


		_ipaddressList=(char*)realloc(_ipaddressList,0*sizeof(char));
		_ipaddressList=NULL;


		_subnetList=(char*)realloc(_subnetList,0*sizeof(char));
		_subnetList=NULL;

		_phyList=(char*)realloc(_phyList,0*sizeof(char));
		_phyList=NULL;


		_phy_nullvalue=(int*)realloc(_phy_nullvalue,0*sizeof(int));
		_phy_nullvalue=NULL;

		_ipaddress_nullvalue=(int*)realloc(_ipaddress_nullvalue,0*sizeof(int));
		_ipaddress_nullvalue=NULL;

		_ipname_nullvalue=(int*)realloc(_ipname_nullvalue,0*sizeof(int));
		_ipname_nullvalue=NULL;

		_subnet_nullvalue=(int*)realloc(_subnet_nullvalue,0*sizeof(int));
		_subnet_nullvalue=NULL;

		_macaddress_nullvalue=(int*)realloc(_macaddress_nullvalue,0*sizeof(int));
		_macaddress_nullvalue=NULL;

		_bia_nullvalue=(int*)realloc(_bia_nullvalue,0*sizeof(int));
		_bia_nullvalue=NULL;

		_portype_nullvalue=(int*)realloc(_portype_nullvalue,0*sizeof(int));
		_portype_nullvalue=NULL;

		_devicename2_nullvalue=(int*)realloc(_devicename2_nullvalue,0*sizeof(int));
		_devicename2_nullvalue=NULL;
		FIRST_TIME_PORT=0;

		return status;
	}


	int freeSparePort()
	{
		int status=0;
		_sparesnbList1=(char*)realloc(_sparesnbList1,0*sizeof(char));
		_sparesnbList1=NULL;

		_spareport_nbrList=(char*)realloc( _spareport_nbrList,0*sizeof(char));
		_spareport_nbrList=NULL;

		_spareportwayList=(int*)realloc(_spareportwayList,0*sizeof(int));
		_spareportwayList=NULL;



		_spareportypeList=(char*)realloc(_spareportypeList,0*sizeof(char));
		_spareportypeList=NULL;

		_sparebiaList=(char*)realloc(_sparebiaList,0*sizeof(char));
		_sparebiaList=NULL;

		_sparemacaddressList=(char*)realloc(_sparemacaddressList,0*sizeof(char));
		_sparemacaddressList=NULL;


		_sparemacaddress_nullvalue=(int*)realloc(_sparemacaddress_nullvalue,0*sizeof(int));
		_sparemacaddress_nullvalue=NULL;

		_sparebia_nullvalue=(int*)realloc(_sparebia_nullvalue,0*sizeof(int));
		_sparebia_nullvalue=NULL;

		_spareportype_nullvalue=(int*)realloc(_spareportype_nullvalue,0*sizeof(int));
		_spareportype_nullvalue=NULL;

		_sparesnb2_nullvalue=(int*)realloc(_sparesnb2_nullvalue,0*sizeof(int));
		_sparesnb2_nullvalue=NULL;
		FIRST_TIME_PORT_SPARE=0;

		return status;
	}



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
		int InsertMultiplePorts(char* devicename,char* port_nb,int admin_status,int port_way,int speed,int pxi_booting,char* port_type,char* bia,char* ipadd1,char* ipname,char* subnet,char* macadd,char* phy,int first_time1,int last_rows1,char* ErrMess)
	{
		char appliName[100]="InsertMultiplePorts";
		char sqlstmt[2000];
		OCIStmt *hstmt;
		OCIBind  *bndp[13]; 

		int res_query=0;
		int free_mem=1;
		sword status=0;
		char* devname=NULL;
		char* dphy=NULL;
		char* dportype=NULL;
		char* dbia=NULL;
		char* dmac=NULL;
		char* dip=NULL;
		char* dipname=NULL;
		char* dsubnet=NULL;
		char* dportnb=NULL;
		int rescode=0;
		int i=0;
		int force_insert=0;
		int first_time=first_time1;
		int last_rows=last_rows1;
		int* numrows_inserted=NULL;
		int* rescode_inserted=NULL;
		int numrows=0;
		OCIBind  *bndp3 = (OCIBind *) 0;
		char seq_name[100]="lhcb_portseq.nextval";
		char ipadd[30];
		OCIBind  *bndp2 = (OCIBind *) 0;
		int devname1=null_charptr(devicename);
		int dphy1=null_charptr(phy);
		int dbia1=null_charptr(bia);
		int dportype1=null_charptr(port_type);
		int dportnb1=null_charptr(port_nb);
		int dipadd1=null_charptr(ipadd);
		int dipname1=null_charptr(ipname);
		int dmacadd1=null_charptr(macadd);
		int dsubnet1=null_charptr(subnet);

		if(FIRST_TIME_PORT==1 && _devicenameList1==NULL)
			first_time=1;
		if(first_time==1)
		{

			status+=freePort();
			FIRST_TIME_PORT=1;
		}
		res_query=AppendString(devicename,_devicenameList1,_devicenameList_len1,_devicenameNbElement1, _max_devicename_len1,first_time);
		res_query+=AppendString(phy,_phyList,_phyList_len,_phyNbElement,_max_phy_len,first_time);

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
		//std:://cout<<"value of ipadd "<<ipadd<<std::endl;
		res_query+=AppendString(ipadd,_ipaddressList,_ipaddressList_len,_ipaddressNbElement,_max_ipaddress_len,first_time);
		res_query+=AppendString(macadd,_macaddressList,_macaddressList_len,_macaddressNbElement,_max_macaddress_len,first_time);
		res_query+=AppendString(ipname,_ipnameList,_ipnameList_len,_ipnameNbElement,_max_ipname_len,first_time);
		res_query+=AppendString(bia,_biaList,_biaList_len,_biaNbElement,_max_bia_len,first_time);
		res_query+=AppendString(port_type,_portypeList,_portypeList_len,_portypeNbElement,_max_portype_len,first_time);
		res_query+=AppendString(port_nb,_port_nbrList,_port_nbrList_len,_port_nbrNbElement,_max_port_nbr_len,first_time);
		res_query+=AppendString(subnet,_subnetList,_subnetList_len,_subnetNbElement,_max_subnet_len,first_time);

		res_query+=AppendInt(admin_status,_adminstatus,_adminstatusNbElement,first_time);
		res_query+=AppendInt(port_way,_portwayList,_portwayNbElement,first_time);
		res_query+=AppendInt(pxi_booting,_pxibootingList,_pxibootingNbElement,first_time);
		res_query+=AppendInt(speed,_speedList,_speedNbElement,first_time);

		res_query+=AppendInt(devname1,_devicename2_nullvalue,_devicename2NbEl,first_time);
		res_query+=AppendInt(dipname1,_ipname_nullvalue,_ipnameNbEl,first_time);
		res_query+=AppendInt(dphy1,_phy_nullvalue,_phyNbEl,first_time);
		res_query+=AppendInt(dmacadd1,_macaddress_nullvalue,_macaddressNbEl,first_time);
		res_query+=AppendInt(dipadd1,_ipaddress_nullvalue,_ipaddressNbEl,first_time);
		res_query+=AppendInt(dbia1,_bia_nullvalue,_biaNbEl,first_time);
		res_query+=AppendInt(dportype1,_portype_nullvalue,_portypeNbEl,first_time);
		res_query+=AppendInt(dportnb1,_port_nbr_nullvalue,_port_nbrNbEl,first_time);
		res_query+=AppendInt(dsubnet1,_subnet_nullvalue,_subnetNbEl,first_time);



		if(last_rows!=1 && _devicenameNbElement1==MAXROWS)
		{
			last_rows=1;
			force_insert=1;
		}


		status=res_query;
		if(last_rows==1 && res_query==0 && ociEnv!=0 && FIRST_TIME_PORT==1)
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
				free(errmessg1);
				free(errmessg2);
				status=freePort();
				if(devname!=NULL)
					free(devname);
				if(dphy!=NULL)
					free(dphy);
				if(dsubnet!=NULL)
					free(dsubnet);
				if(dip!=NULL)
					free(dip);
				if(dipname!=NULL)
					free(dipname);
				if(dbia!=NULL)
					free(dbia);
				if(dmac!=NULL)
					free(dmac);
				if(dportype!=NULL)
					free(dportype);
				if(dportnb!=NULL)
					free(dportnb);

				return -1;
			}
			//need to proceed with messages

			status =OCIHandleAlloc (ociEnv, (void**)&hstmt, OCI_HTYPE_STMT , 0, 0);
			//std:://cout<<"value of _macaddressNbEl "<<_macaddressNbEl<<"value of max "<<_max_macaddress_len<<std::endl;
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIHandleAlloc unsuccessful");
			}
			else
				sprintf(sqlstmt,"BEGIN if instr(:ipadd,'none')=0 then :rescode:=%s(:ipadd,:ipname,:subnet,'%s','%s'); end if; insert ALL WHEN (instr(:ipadd,'none')>0) then into %s (portid,deviceid,port_nbr,port_way,administrative_status,speed,port_type,phy,pxi_booting,ipaddress,author,user_update,terminal_name) values (%s,devid,:port_nbr,:portway,:adminstatus,:speed,:portype,:phy,:pxi,NULL,'%s','%s','%s') else into %s (serialnb,port_nbr,port_type,port_way,bia,macaddress,author,created,terminal_name) values (snb,:port_nbr,:portype,:portway,:bia,:mac,'%s',sysdate,'%s') into %s(portid,deviceid,port_nbr,port_way,administrative_status,speed,port_type,phy,pxi_booting,ipaddress,author,user_update,terminal_name) values (%s,devid,:port_nbr,:portway,:adminstatus,:speed,:portype,:phy,:pxi,:ipadd,'%s','%s','%s') select deviceid as devid,serialnb as snb from  %s  where devicename=:dname; :numrows:=%s; end; ",_insertIpaddress,login,host,PORT_TABLE,seq_name,login,login,host,HWPORT_TABLE,login,host,PORT_TABLE,seq_name,login,login,host,LOGICAL_DEVICE_TABLE,SQLROWCOUNT);	

			//std:://cout<<"sql "<<sqlstmt<<"and its length "<<strlen(sqlstmt)<<std::endl;
			if (OCIStmtPrepare(hstmt, ociError, (text*)sqlstmt, (ub4)strlen((char *)sqlstmt), (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT))
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");

			}
			else
			{
				devname=(char*)malloc(_devicenameNbElement1*_max_devicename_len1*sizeof(char));
				if(devname!=NULL)
					status+=NormalizeVector(_devicenameList1,_devicenameNbElement1, _max_devicename_len1,devname);
				dphy=(char*)malloc(_phyNbElement*_max_phy_len*sizeof(char));
				if(dphy!=NULL)
					status+=NormalizeVector(_phyList, _phyNbElement,_max_phy_len,dphy);

				dip=(char*)malloc(_ipaddressNbElement*_max_ipaddress_len*sizeof(char));
				if(dip!=NULL)
					status+=NormalizeVector(_ipaddressList,_ipaddressNbElement,_max_ipaddress_len,dip);
				dmac=(char*)malloc(_macaddressNbElement*_max_macaddress_len*sizeof(char));
				if(dmac!=NULL)
					status+=NormalizeVector(_macaddressList,_macaddressNbElement,_max_macaddress_len,dmac);
				dipname=(char*)malloc(_ipnameNbElement*_max_ipname_len*sizeof(char));
				if(dipname!=NULL)
					status+=NormalizeVector(_ipnameList,_ipnameNbElement,_max_ipname_len,dipname);

				dsubnet=(char*)malloc(_subnetNbElement*_max_subnet_len*sizeof(char));
				if(dsubnet!=NULL)
					status+=NormalizeVector(_subnetList,_subnetNbElement,_max_subnet_len,dsubnet);

				dbia=(char*)malloc(_biaNbElement*_max_bia_len*sizeof(char));
				if(dbia!=NULL)
					status+=NormalizeVector(_biaList,_biaNbElement,_max_bia_len,dbia);		

				dportype=(char*)malloc(_portypeNbElement*_max_portype_len*sizeof(char));
				if(dportype!=NULL)
					status+=NormalizeVector(_portypeList,_portypeNbElement,_max_portype_len,dportype);	

				dportnb=(char*)malloc(_port_nbrNbElement*_max_port_nbr_len*sizeof(char));
				if(dportnb!=NULL)
					status+=NormalizeVector(_port_nbrList,_port_nbrNbElement,_max_port_nbr_len,dportnb);

				numrows_inserted=(int*)malloc(sizeof(int)*_devicenameNbElement1);
				rescode_inserted=(int*)malloc(sizeof(int)*_devicenameNbElement1);
				//std:://cout<<"value of devicename "<<devname<<"port_nb "<<dportnb<<"and"<<std::endl;
				//std:://cout<<"value of portype "<<dportype<<"bia "<<bia<<"and"<<std::endl;


				if(devname==NULL||dphy==NULL||dportnb==NULL||dbia==NULL||dmac==NULL||dip==NULL||dipname==NULL||dsubnet==NULL||dportype==NULL||numrows_inserted==NULL||rescode_inserted==NULL)
				{
					status=-10;
					rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");
					status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
					GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
					status+=freePort();
					if(devname!=NULL)
						free(devname);
					if(dphy!=NULL)
						free(dphy);
					if(dsubnet!=NULL)
						free(dsubnet);
					if(dip!=NULL)
						free(dip);
					if(dipname!=NULL)
						free(dipname);
					if(dbia!=NULL)
						free(dbia);
					if(dmac!=NULL)
						free(dmac);
					if(dportype!=NULL)
						free(dportype);
					if(dportnb!=NULL)
						free(dportnb);
					if(numrows_inserted!=NULL)
						free(numrows_inserted);
					if(rescode_inserted!=NULL)
						free(rescode_inserted);
					return -1;
				}
				else
				{
					for(i=0;i<_devicenameNbElement1;i++)
					{
						numrows_inserted[i]=0;
						rescode_inserted[i]=0;
					}
					status=OCIBindByName(hstmt, &bndp[0], ociError,(text*) ":port_nbr", -1,(dvoid*)dportnb, _max_port_nbr_len,  SQLT_STR, (dvoid *) &_port_nbr_nullvalue[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
				}

			}

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp[1], ociError,(text*) ":ipadd", -1,(dvoid*)dip, _max_ipaddress_len,  SQLT_STR, (dvoid *) &_ipaddress_nullvalue[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp[2], ociError,(text*) ":ipname", -1,(dvoid*)dipname, _max_ipname_len,  SQLT_STR, (dvoid *) &_ipname_nullvalue[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp[3], ociError,(text*) ":subnet", -1,(dvoid*)dsubnet, _max_subnet_len,  SQLT_STR, (dvoid *) &_subnet_nullvalue[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");

			}
			else
				status =OCIBindByName(hstmt, &bndp[4], ociError,(text*) ":portway", -1,(dvoid*)&_portwayList[0], sizeof(port_way),  SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");

			}
			else
				status =OCIBindByName(hstmt, &bndp[5], ociError,(text*) ":speed", -1,(dvoid*)&_speedList[0], sizeof(speed),  SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");

			}
			else
				status =OCIBindByName(hstmt, &bndp[6], ociError,(text*) ":adminstatus", -1,(dvoid*)&_adminstatus[0], sizeof(admin_status),  SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");

			}
			else
				status =OCIBindByName(hstmt, &bndp[7], ociError,(text*) ":portype", -1,(dvoid*)dportype, _max_portype_len,  SQLT_STR, (dvoid *)&_portype_nullvalue[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");

			}
			else
				status =OCIBindByName(hstmt, &bndp[8], ociError,(text*) ":phy", -1,(dvoid*)dphy, _max_phy_len,  SQLT_STR, (dvoid *) &_phy_nullvalue[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");

			}
			else
				status =OCIBindByName(hstmt, &bndp[9], ociError,(text*) ":pxi", -1,(dvoid*)&_pxibootingList[0], sizeof(pxi_booting),  SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);


			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
				status=freePort();


			}
			else
				status =OCIBindByName(hstmt, &bndp[10], ociError,(text*) ":bia", -1,(dvoid*)dbia, _max_bia_len,  SQLT_STR, (dvoid *)& _bia_nullvalue[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");


			}
			else
				status =OCIBindByName(hstmt, &bndp[11], ociError,(text*) ":mac", -1,(dvoid*)dmac, _max_macaddress_len,  SQLT_STR, (dvoid *) &_macaddress_nullvalue[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");

			}
			else
				status =OCIBindByName(hstmt, &bndp[12], ociError,(text*) ":dname", -1,(dvoid*)devname,  _max_devicename_len1,  SQLT_STR, (dvoid *) &_devicename2_nullvalue[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);

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
				status =OCIBindByName(hstmt, &bndp2, ociError,(text*) ":rescode", -1,(dvoid*)&rescode_inserted[0], sizeof(numrows),SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");

			}
			else
				status=OCIBindArrayOfStruct(bndp[0], ociError,_max_port_nbr_len,sizeof(int), 0, 0);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");

			}
			else
				status=OCIBindArrayOfStruct(bndp[1], ociError,_max_ipaddress_len,  sizeof(int),  0, 0);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");

			}
			else
				status=OCIBindArrayOfStruct(bndp[2], ociError,_max_ipname_len, sizeof(int),  0, 0);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");	
			}
			else
				status=OCIBindArrayOfStruct(bndp[3], ociError, _max_subnet_len, sizeof(int),  0, 0);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp[4], ociError, sizeof(int), 0, 0, 0);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp[5], ociError,  sizeof(int), 0, 0, 0);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp[6], ociError,  sizeof(int), 0, 0, 0);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp[7], ociError, _max_portype_len, sizeof(int), 0, 0);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp[8], ociError,  _max_phy_len, sizeof(int), 0, 0);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp[9], ociError, sizeof(int), 0, 0, 0);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp[10], ociError,_max_bia_len, sizeof(int),  0, 0);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp[11], ociError,_max_macaddress_len, sizeof(int),  0, 0);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp[12], ociError, _max_devicename_len1, sizeof(int), 0, 0);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp3, ociError,  sizeof(int), 0, 0, 0);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp2, ociError,  sizeof(int), 0, 0, 0);


			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status= OCIStmtExecute(ociHdbc, hstmt, ociError, _devicenameNbElement1, 0, 0, 0, OCI_DEFAULT );


			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
			}
			else
			{
				numrows=-1;
				for(i=0;i< _devicenameNbElement1;i++)
				{
					if(numrows_inserted[i]==0)
					{
						numrows=0;
						i=  _devicenameNbElement1+5;
					}
					else
						numrows=numrows_inserted[i];
				}
				if(numrows==0)
				{
					status = OCITransCommit(ociHdbc, ociError, 0);
					status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
					status=freePort();
					if(devname!=NULL)
						free(devname);
					if(dphy!=NULL)
						free(dphy);
					if(dsubnet!=NULL)
						free(dsubnet);
					if(dip!=NULL)
						free(dip);
					if(dipname!=NULL)
						free(dipname);
					if(dbia!=NULL)
						free(dbia);
					if(dmac!=NULL)
						free(dmac);
					if(dportype!=NULL)
						free(dportype);
					if(dportnb!=NULL)
						free(dportnb);

					free(numrows_inserted);
					free(rescode_inserted);
					GetErrorMess(appliName,  "COULDNOT_INSERT_ALL_ROWS",ErrMess,1);
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

			if(devname!=NULL)
				free(devname);
			if(dphy!=NULL)
				free(dphy);
			if(dsubnet!=NULL)
				free(dsubnet);
			if(dip!=NULL)
				free(dip);
			if(dipname!=NULL)
				free(dipname);
			if(dbia!=NULL)
				free(dbia);
			if(dmac!=NULL)
				free(dmac);
			if(dportype!=NULL)
				free(dportype);
			if(dportnb!=NULL)
				free(dportnb);
			if(numrows_inserted!=NULL)
				free(numrows_inserted);
			if(rescode_inserted!=NULL)
				free(rescode_inserted);
			status=freePort();
			status+=OCIHandleFree (hstmt,OCI_HTYPE_STMT);

			if(rescode!=0)
				if(ociError!=0)
					OCIReportError(ociError,appliName, ErrMess,1); 
				else
					GetErrorMess(appliName,  "NOT CONNECTED TO ANY DB",ErrMess,1); 
			else
				OCIReportError(ociError,appliName, ErrMess,0);
			if(force_insert==1 && rescode==0)
				FIRST_TIME_PORT=1;
		}
		else
		{
			if (res_query!=0)
			{
				status=freePort();
				rescode=res_query;
				GetErrorMess(appliName, "Cache Problem",ErrMess,1);
			}
			if (FIRST_TIME_PORT!=1)
			{
				status=freePort();
				res_query=-1;
				GetErrorMess(appliName, "CACHE HAS NOT BEEN INITIALIZED",ErrMess,1);
			}
			if(ociEnv==0)
			{
				status=freePort();
				res_query=-1;
				GetErrorMess(appliName, "NOT CONNECTED TO ANY DB",ErrMess,1);
			}	
			if(ociEnv!=0 && FIRST_TIME_PORT==1 && res_query==0)
			{
				status=0;
				GetErrorMess(appliName, "NO_ERROR",ErrMess,0);
			}


		}
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
		int InsertMultipleSparePorts(char* serialnb,char* port_nb,int port_way,char* port_type,char* bia,char* macadd,int first_time1,int last_rows1,char* ErrMess)
	{
		char appliName[100]="InsertMultipleSparePorts";
		char sqlstmt[1000];
		OCIStmt *hstmt;
		OCIBind  *bndp[6]; 

		int res_query=0;
		int free_mem=1;
		sword status=0;
		char* devname=NULL;
		char* dportype=NULL;
		char* dbia=NULL;
		char* dmac=NULL;
		char* dportnb=NULL;
		int rescode=0;
		int i=0;
		int force_insert=0;
		int first_time=first_time1;
		int last_rows=last_rows1;
		int* numrows_inserted=NULL;
		int numrows=0;
		OCIBind  *bndp3 = (OCIBind *) 0;


		int devname1=null_charptr(serialnb);
		int dbia1=null_charptr(bia);
		int dportype1=null_charptr(port_type);
		int dportnb1=null_charptr(port_nb);
		int dmacadd1=null_charptr(macadd);

		if(FIRST_TIME_PORT_SPARE==1 && _sparesnbList1==NULL)
			first_time=1;
		if(first_time==1)
		{

			status+=freeSparePort();
			FIRST_TIME_PORT_SPARE=1;
		}
		res_query=AppendString(serialnb,_sparesnbList1,_sparesnbList_len1,_sparesnbNbElement1, _max_sparesnb_len1,first_time);


		res_query+=AppendString(macadd,_sparemacaddressList,_sparemacaddressList_len,_sparemacaddressNbElement,_max_sparemacaddress_len,first_time);
		res_query+=AppendString(bia,_sparebiaList,_sparebiaList_len,_sparebiaNbElement,_max_sparebia_len,first_time);
		res_query+=AppendString(port_type,_spareportypeList,_spareportypeList_len,_spareportypeNbElement,_max_spareportype_len,first_time);
		res_query+=AppendString(port_nb,_spareport_nbrList,_spareport_nbrList_len,_spareport_nbrNbElement,_max_spareport_nbr_len,first_time);
		res_query+=AppendInt(port_way,_spareportwayList,_spareportwayNbElement,first_time);

		res_query+=AppendInt(devname1,_sparesnb2_nullvalue,_sparesnb2NbEl,first_time);
		res_query+=AppendInt(dmacadd1,_sparemacaddress_nullvalue,_sparemacaddressNbEl,first_time);
		res_query+=AppendInt(dbia1,_sparebia_nullvalue,_sparebiaNbEl,first_time);
		res_query+=AppendInt(dportype1,_spareportype_nullvalue,_spareportypeNbEl,first_time);
		res_query+=AppendInt(dportnb1,_spareport_nbr_nullvalue,_spareport_nbrNbEl,first_time);



		if(last_rows!=1 && _sparesnbNbElement1==MAXROWS)
		{
			last_rows=1;
			force_insert=1;
		}


		status=res_query;
		if(last_rows==1 && res_query==0 && ociEnv!=0 && FIRST_TIME_PORT_SPARE==1)
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
				free(errmessg1);
				free(errmessg2);
				status=freeSparePort();
				if(devname!=NULL)
					free(devname);

				if(dbia!=NULL)
					free(dbia);
				if(dmac!=NULL)
					free(dmac);
				if(dportype!=NULL)
					free(dportype);
				if(dportnb!=NULL)
					free(dportnb);

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
				sprintf(sqlstmt,"BEGIN insert into %s (serialnb,port_nbr,port_way,port_type,bia,macaddress,created,author,terminal_name) values (:snb,:port_nbr,:portway,:portype,:bia,:mac,sysdate,'%s','%s'); :numrows:=%s; end; ",HWPORT_TABLE,login,host,SQLROWCOUNT);
			if (OCIStmtPrepare(hstmt, ociError, (text*)sqlstmt, (ub4)strlen((char *)sqlstmt), (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT))
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");

			}
			else
			{

				devname=(char*)malloc(_sparesnbNbElement1*_max_sparesnb_len1*sizeof(char));
				if(devname!=NULL)
					status+=NormalizeVector(_sparesnbList1,_sparesnbNbElement1, _max_sparesnb_len1,devname);

				dmac=(char*)malloc(_sparemacaddressNbElement*_max_sparemacaddress_len*sizeof(char));
				if(dmac!=NULL)
					status+=NormalizeVector(_sparemacaddressList,_sparemacaddressNbElement,_max_sparemacaddress_len,dmac);

				dbia=(char*)malloc(_sparebiaNbElement*_max_sparebia_len*sizeof(char));
				if(dbia!=NULL)
					status+=NormalizeVector(_sparebiaList,_sparebiaNbElement,_max_sparebia_len,dbia);		
				dportype=(char*)malloc(_spareportypeNbElement*_max_spareportype_len*sizeof(char));
				if(dportype!=NULL)
					status+=NormalizeVector(_spareportypeList,_spareportypeNbElement,_max_spareportype_len,dportype);	
				dportnb=(char*)malloc(_spareport_nbrNbElement*_max_spareport_nbr_len*sizeof(char));
				if(dportnb!=NULL)
					status+=NormalizeVector(_spareport_nbrList,_spareport_nbrNbElement,_max_spareport_nbr_len,dportnb);

				numrows_inserted=(int*)malloc(sizeof(int)*_sparesnbNbElement1);


				if(devname==NULL||dportnb==NULL||dbia==NULL||dmac==NULL||dportype==NULL||numrows_inserted==NULL)
				{
					status=-10;
					rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");
					status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
					GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
					status+=freeSparePort();
					if(devname!=NULL)
						free(devname);

					if(dbia!=NULL)
						free(dbia);
					if(dmac!=NULL)
						free(dmac);
					if(dportype!=NULL)
						free(dportype);
					if(dportnb!=NULL)
						free(dportnb);
					if(numrows_inserted!=NULL)
						free(numrows_inserted);
					return -1;
				}
				else
				{
					for(i=0;i<_sparesnbNbElement1;i++)
						numrows_inserted[i]=0;
					status=OCIBindByName(hstmt, &bndp[0], ociError,(text*) ":port_nbr", -1,(dvoid*)dportnb, _max_spareport_nbr_len,  SQLT_STR, (dvoid *) &_spareport_nbr_nullvalue[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
				}

			}
			//std:://cout<<"before binding 1 "<<std::endl;
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");

			}
			else
				status =OCIBindByName(hstmt, &bndp[1], ociError,(text*) ":portway", -1,(dvoid*)&_spareportwayList[0], sizeof(port_way),  SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");

			}
			else
				status =OCIBindByName(hstmt, &bndp[2], ociError,(text*) ":portype", -1,(dvoid*)dportype, _max_spareportype_len,  SQLT_STR, (dvoid *)&_spareportype_nullvalue[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);


			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
				status=freeSparePort();


			}
			else
				status =OCIBindByName(hstmt, &bndp[3], ociError,(text*) ":bia", -1,(dvoid*)dbia, _max_sparebia_len,  SQLT_STR, (dvoid *)& _sparebia_nullvalue[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");


			}
			else
				status =OCIBindByName(hstmt, &bndp[4], ociError,(text*) ":mac", -1,(dvoid*)dmac, _max_sparemacaddress_len,  SQLT_STR, (dvoid *) &_sparemacaddress_nullvalue[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");

			}
			else
				status =OCIBindByName(hstmt, &bndp[5], ociError,(text*) ":snb", -1,(dvoid*)devname,  _max_sparesnb_len1,  SQLT_STR, (dvoid *) &_sparesnb2_nullvalue[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);

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
				status=OCIBindArrayOfStruct(bndp[0], ociError,_max_spareport_nbr_len,sizeof(int), 0, 0);
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
				status=OCIBindArrayOfStruct(bndp[2], ociError, _max_spareportype_len, sizeof(int), 0, 0);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp[3], ociError,_max_sparebia_len, sizeof(int),  0, 0);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp[4], ociError,_max_sparemacaddress_len, sizeof(int),  0, 0);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp[5], ociError, _max_sparesnb_len1, sizeof(int), 0, 0);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp3, ociError,  sizeof(int), 0, 0, 0);

			//std:://cout<<"before executing 1 "<<std::endl;

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status= OCIStmtExecute(ociHdbc, hstmt, ociError, _sparesnbNbElement1, 0, 0, 0, OCI_DEFAULT );


			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
			}
			else
			{
				numrows=-1;
				for(i=0;i< _sparesnbNbElement1;i++)
				{
					if(numrows_inserted[i]==0)
					{
						numrows=0;
						i=  _sparesnbNbElement1+5;
					}
					else
						numrows=numrows_inserted[i];
				}
				//std:://cout<<"numrows= "<<numrows<<std::endl;
				if(numrows==0)
				{
					status = OCITransCommit(ociHdbc, ociError, 0);
					status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
					status=freeSparePort();
					if(devname!=NULL)
						free(devname);

					if(dbia!=NULL)
						free(dbia);
					if(dmac!=NULL)
						free(dmac);
					if(dportype!=NULL)
						free(dportype);
					if(dportnb!=NULL)
						free(dportnb);

					free(numrows_inserted);
					GetErrorMess(appliName,  "COULDNOT_INSERT_ALL_ROWS",ErrMess,1);
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

			if(devname!=NULL)
				free(devname);
			if(dbia!=NULL)
				free(dbia);
			if(dmac!=NULL)
				free(dmac);
			if(dportype!=NULL)
				free(dportype);
			if(dportnb!=NULL)
				free(dportnb);
			if(numrows_inserted!=NULL)
				free(numrows_inserted);

			status=freeSparePort();
			status+=OCIHandleFree (hstmt,OCI_HTYPE_STMT);

			if(rescode!=0)
				if(ociError!=0)
					OCIReportError(ociError,appliName, ErrMess,1); 
				else
					GetErrorMess(appliName,  "NOT CONNECTED TO ANY DB",ErrMess,1); 
			else
				OCIReportError(ociError,appliName, ErrMess,0);
			if(force_insert==1 && rescode==0)
				FIRST_TIME_PORT_SPARE=1;
		}
		else
		{
			if (res_query!=0)
			{
				status=freeSparePort();
				rescode=res_query;
				GetErrorMess(appliName, "Cache Problem",ErrMess,1);
			}
			if (FIRST_TIME_PORT_SPARE!=1)
			{
				status=freeSparePort();
				res_query=-1;
				GetErrorMess(appliName, "CACHE HAS NOT BEEN INITIALIZED",ErrMess,1);
			}
			if(ociEnv==0)
			{
				status=freeSparePort();
				res_query=-1;
				GetErrorMess(appliName, "NOT CONNECTED TO ANY DB",ErrMess,1);
			}	
			if(ociEnv!=0 && FIRST_TIME_PORT_SPARE==1 && res_query==0)
			{
				status=0;
				GetErrorMess(appliName, "NO_ERROR",ErrMess,0);
			}

		}
		return (rescode+status);
	}
	//insert IP aliases
	/**
	* Insert Multiple IP aliases (only for the DAQ ), returning an integer value.
	* The user should manage the memory : there is no memory allocation. IP address or IP alias should already be inserted using InsertPorts
	* @param IP address : IP address for which you want to give an IP alias. Put "none" if you provide an IP name
	* @param IP name   : IP name for which you want to give an IP alias. Put "none" if you provide an IP address
	* @param IP alias   : 1 if it's an input, 2 if it's an output : the reference is the data coming from the detector.
	* @param port_type   : type of the port interface (control, data,... used for the DAQ). Put "" if none.
	* @param bia : burned mac address (used for the DAQ, for addresses). Put "" if none.
	* @param macadd : mac address (used for the DAQ, for addresses). Put "" if none.
	* @param first_time1 : 1 if it's your first Port insert (save it into the database), 0 otherwise   
	* @param last_rows1 : 1 if it's your last Port insert (save it into the database), 0 otherwise
	* @param ErrMess : error message in case of failure
	* @return 0 if it is successful
	*/ 
	EXTERN_CONFDB
		int InsertMultipleIPAliases(char* ipaddress,char* ipname,char* ipalias,int first_time1,int last_rows1,char* ErrMess)
	{
		char appliName[100]="InsertMultipleIPAlias";
		char sqlstmt[1000];
		OCIStmt *hstmt;
		OCIBind  *bndp[3]; 

		int res_query=0;
		int free_mem=1;
		sword status=0;
		char* dipname=NULL;
		char* dipalias=NULL;
		char* dipadd=NULL;
		int rescode=0;
		int i=0;
		int force_insert=0;
		int first_time=first_time1;
		int last_rows=last_rows1;
		int* numrows_inserted=NULL;
		int numrows=0;
		OCIBind  *bndp3 = (OCIBind *) 0;


		int dipname1=null_charptr(ipname);
		int dipadd1=null_charptr(ipaddress);
		int dipalias1=null_charptr(ipalias);

		if(FIRST_TIME_IP_ALIAS==1 && _ipAliasnbList==NULL)
			first_time=1;
		if(first_time==1)
		{

			status+=freeIPAliases();
			FIRST_TIME_IP_ALIAS=1;
		}


		res_query=AppendString(ipaddress,_ipAddnbList,_ipAddList_len,_ipAddNbElement, _max_ipAdd_len,first_time);
		res_query+=AppendString(ipname,_ipNamenbList,_ipNameList_len,_ipNameNbElement,_max_ipName_len,first_time);
		res_query+=AppendString(ipalias,_ipAliasnbList, _ipAliasList_len,_ipAliasNbElement,_max_ipAlias_len,first_time);

		res_query+=AppendInt(dipname1, _ipName_nullvalue,_ipNameNbEl,first_time);
		res_query+=AppendInt(dipadd1,_ipAdd_nullvalue,_ipAddNbEl,first_time);
		res_query+=AppendInt(dipalias1,_ipAlias_nullvalue,_ipAliasNbEl,first_time);



		if(last_rows!=1 && _ipAliasNbElement==MAXROWS)
		{
			last_rows=1;
			force_insert=1;
		}


		status=res_query;
		if(last_rows==1 && res_query==0 && ociEnv!=0 && FIRST_TIME_IP_ALIAS==1)
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
				free(errmessg1);
				free(errmessg2);
				status=freeIPAliases();
				if(dipname!=NULL)
					free(dipname);
				if(dipalias!=NULL)
					free(dipalias);
				if(dipadd!=NULL)
					free(dipadd);

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

				sprintf(sqlstmt,"BEGIN if :ipname='none' then insert into %s(ipaddress,ipalias,created,author,terminal_name) values (:ipadd,:ipalias,sysdate,'%s','%s'); else if :ipadd='none' then insert all into %s (ipaddress,ipalias,created,author,terminal_name) values (ipadd,:ipalias,sysdate,'%s','%s') select ipaddress as ipadd from %s where ipname=:ipname; end if; end if; :numrows:=%s; end; ",IPALIAS_TABLE,login,host,IPALIAS_TABLE,login,host,IPINFO_TABLE,SQLROWCOUNT);	

			}
			if (OCIStmtPrepare(hstmt, ociError, (text*)sqlstmt, (ub4)strlen((char *)sqlstmt), (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT))
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");

			}
			else
			{


				dipalias=(char*)malloc(_ipAliasNbElement*_max_ipAlias_len*sizeof(char));
				if(dipalias!=NULL)
					status+=NormalizeVector(_ipAliasnbList,_ipAliasNbElement, _max_ipAlias_len,dipalias);

				dipname=(char*)malloc(_ipNameNbElement*_max_ipName_len*sizeof(char));
				if(dipname!=NULL)
					status+=NormalizeVector(_ipNamenbList,_ipNameNbElement,_max_ipName_len,dipname);

				dipadd=(char*)malloc(_ipAddNbElement*_max_ipAdd_len*sizeof(char));
				if(dipadd!=NULL)
					status+=NormalizeVector(_ipAddnbList,_ipAddNbElement,_max_ipAdd_len,dipadd);		

				numrows_inserted=(int*)malloc(sizeof(int)*_ipAliasNbElement);


				if(dipname==NULL||dipadd==NULL||dipalias==NULL||numrows_inserted==NULL)
				{
					status=-10;
					rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");
					status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
					GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
					status=freeIPAliases();
					if(dipname!=NULL)
						free(dipname);

					if(dipadd!=NULL)
						free(dipadd);

					if(dipalias!=NULL)
						free(dipalias);

					if(numrows_inserted!=NULL)
						free(numrows_inserted);
					return -1;
				}
				else
				{
					for(i=0;i<_sparesnbNbElement1;i++)
						numrows_inserted[i]=0;
					status=OCIBindByName(hstmt, &bndp[0], ociError,(text*) ":ipname", -1,(dvoid*)dipname, _max_ipName_len,  SQLT_STR, (dvoid *) &_ipName_nullvalue[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
				}

			}
			//std:://cout<<"before binding 1 "<<std::endl;
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");

			}
			else
				status =OCIBindByName(hstmt, &bndp[1], ociError,(text*) ":ipalias", -1,(dvoid*)dipalias, _max_ipAlias_len,  SQLT_STR, (dvoid *) &_ipAlias_nullvalue[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");

			}
			else
				status =OCIBindByName(hstmt, &bndp[2], ociError,(text*) ":ipadd", -1,(dvoid*)dipadd, _max_ipAdd_len,  SQLT_STR, (dvoid *) &_ipAdd_nullvalue[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);


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
				status=OCIBindArrayOfStruct(bndp[0], ociError,_max_ipName_len,sizeof(int), 0, 0);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp[1], ociError,_max_ipAlias_len, 0, 0, 0);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp[2], ociError, _max_ipAdd_len, sizeof(int), 0, 0);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp3, ociError,  sizeof(int), 0, 0, 0);

			//std:://cout<<"before executing 1 "<<std::endl;

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status= OCIStmtExecute(ociHdbc, hstmt, ociError, _ipAliasNbElement, 0, 0, 0, OCI_DEFAULT );


			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
			}
			else
			{
				numrows=-1;
				for(i=0;i< _ipAliasNbElement;i++)
				{
					if(numrows_inserted[i]==0)
					{
						numrows=0;
						i=  _ipAliasNbElement+5;
					}
					else
						numrows=numrows_inserted[i];
				}
				//std:://cout<<"numrows= "<<numrows<<std::endl;
				if(numrows==0)
				{
					status = OCITransCommit(ociHdbc, ociError, 0);
					status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
					status=freeIPAliases();
					if(dipname!=NULL)
						free(dipname);

					if(dipadd!=NULL)
						free(dipadd);

					if(dipalias!=NULL)
						free(dipalias);

					if(numrows_inserted!=NULL)
						free(numrows_inserted);
					GetErrorMess(appliName,  "COULDNOT_INSERT_ALL_ROWS",ErrMess,1);
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

			if(dipname!=NULL)
				free(dipname);

			if(dipadd!=NULL)
				free(dipadd);

			if(dipalias!=NULL)
				free(dipalias);

			if(numrows_inserted!=NULL)
				free(numrows_inserted);

			status=freeIPAliases();

			status+=OCIHandleFree (hstmt,OCI_HTYPE_STMT);

			if(rescode!=0)
				if(ociError!=0)
					OCIReportError(ociError,appliName, ErrMess,1); 
				else
					GetErrorMess(appliName,  "NOT CONNECTED TO ANY DB",ErrMess,1); 
			else
				OCIReportError(ociError,appliName, ErrMess,0);
			if(force_insert==1 && rescode==0)
				FIRST_TIME_IP_ALIAS=1;
		}
		else
		{
			if (res_query!=0)
			{
				status=freeIPAliases();
				rescode=res_query;
				GetErrorMess(appliName, "Cache Problem",ErrMess,1);
			}
			if (FIRST_TIME_IP_ALIAS!=1)
			{
				status=freeIPAliases();
				res_query=-1;
				GetErrorMess(appliName, "CACHE HAS NOT BEEN INITIALIZED",ErrMess,1);
			}
			if(ociEnv==0)
			{
				status=freeIPAliases();
				res_query=-1;
				GetErrorMess(appliName, "NOT CONNECTED TO ANY DB",ErrMess,1);
			}	
			if(ociEnv!=0 && FIRST_TIME_IP_ALIAS==1 && res_query==0)
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