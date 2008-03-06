#include "cicDB.h"
#include "CDB.h"
#include <stdlib.h>
#include <stdio.h>


static DBContainer *cdb;
static eConnectivityCache *ecache;
static iConnectivityCache *icache;
  
int DBConn(char* server,char* usr,char* pwd,char* ErrorMessage)
//int DBConnexion(char* server,char* usr,char* pwd,char* ErrorMessage)
{
  cdb=new DBContainer(server,usr,pwd);
  ecache  = new eConnectivityCache(cdb);
  icache  = new iConnectivityCache(cdb);

    return 0;
}


int _GetConnectivityBetweenDevices(char* dev_from, char* dev_to, int& len_array, int &pfrom1_arraylen,int &nfrom1_arraylen,int &pto1_arraylen,int &nto1_arraylen,int &lkinfo_arraylen,char* nfrom1_list,char* nto1_list,char* pfrom1_list,int* pfrom_list,char* pto1_list,int* pto_list,int* pathid_list,int* link_pos_list,char* lkinfo_list,int reload_connectivity,int delete_connectivity,char* ErrMess){
	
	return cdb->GetConnectivityBetweenDevices(dev_from,dev_to,ecache,icache,len_array,pfrom1_arraylen,nfrom1_arraylen,pto1_arraylen,nto1_arraylen,lkinfo_arraylen,nfrom1_list,nto1_list,pfrom1_list,pfrom_list,pto1_list,pto_list,pathid_list,link_pos_list,lkinfo_list,reload_connectivity,delete_connectivity,ErrMess);
}

int _GetDetailedConnBetweenDeviceDevType(char* dev_from, char* dev_to,int devto_type_given,  char* nfrom_list,char* pfrom_list,char* pto_list,int& len_array,int & nfrom_arraylen,int & pfrom_arraylen, int & nto_arraylen, int & pto_arraylen,int &lkinfo_arraylen, int* pfrom1_list,int* pto1_list,char* nto_list,int* pathid_list, int* link_pos_list,char* lkinfo_list,int reload_connectivity,int delete_connectivity,char* ErrMess){
	//devto_type_given... 
	return cdb->GetDetailedConnBetweenDeviceDevType(dev_from, dev_to,devto_type_given,ecache,icache,nfrom_list,pfrom_list,pto_list,len_array, nfrom_arraylen, pfrom_arraylen, nto_arraylen, pto_arraylen,lkinfo_arraylen, pfrom1_list, pto1_list, nto_list, pathid_list, link_pos_list, lkinfo_list, reload_connectivity,delete_connectivity, ErrMess);
}

int _GetConnOutBetweenDeviceDeviceType(char* dev_from, char* devtype,int devtype_give, int& len_array, char* pfrom_arraylist, int reload_connectivity,int delete_connectivity,char* ErrMess){
	
	int tmp=0;
	return cdb->GetDetailedConnBetweenDeviceDevType(dev_from, devtype,devtype_give,ecache,icache,0,pfrom_arraylist,0,tmp,tmp, len_array,tmp,tmp,tmp,0,0,0,0,0,0, reload_connectivity,delete_connectivity, ErrMess);
}

int _GetDetailedConnectivityBetweenDevices(char* dev_from, char* dev_to, char* nfrom_list,int& len_array,int & nfrom_arraylen,int & pfrom_arraylen, int & nto_arraylen, int & pto_arraylen,int &lkinfo_arraylen,char* pfrom_list,char* pto_list,int* pfrom1_list,int* pto1_list,char* nto_list,int* pathid_list, int* link_pos_list,char* lkinfo_list,int reload_connectivity,int delete_connectivity,char* ErrMess){
	//difference between GetDetailedConnectivityBetweenDevices and GetConnectivityBetweenDevices ?
	return cdb->GetConnectivityBetweenDevices(dev_from,dev_to,ecache,icache,len_array,pfrom_arraylen,nfrom_arraylen,pto_arraylen,nto_arraylen,lkinfo_arraylen,nfrom_list,nto_list,pfrom_list,pfrom1_list,pto_list,pto1_list,pathid_list,link_pos_list,lkinfo_list,reload_connectivity,delete_connectivity,ErrMess);
}

int _GetAllPathsPerDevice(char* systemnameList,char* devname,int& len_array, int* lkid_list5,int* pathid_list,int* link_pos_list,int reload_connectivity,int delete_connectivity,char* ErrMessage){
	//sytemnameList...
	return cdb->GetAllPathsPerDevice(systemnameList,devname, ecache,icache, len_array,lkid_list5,pathid_list,link_pos_list,reload_connectivity,delete_connectivity,ErrMessage);
}
//reload connectivity
//baza danych - czy wszystko sie dowiazuje?