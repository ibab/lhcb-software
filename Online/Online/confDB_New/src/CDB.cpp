#include "cicDB.h"
#include "CDB.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <xstring>

#include "list_structures.h"	//for Fill_list(char* system_list);

DBContainer::DBContainer(void)
{
}
	void DBContainer::setup(char* database,char* usr,char* pwd)
	{
		int ln;
		ln	= strlen(database);
		serv	= (char*)malloc(ln+1);
		strcpy(serv,database);
		ln	= strlen(usr);
		user	=	(char*)malloc(ln+1);
		strcpy(user,usr);
		ln	= strlen(pwd);
		passw	= (char*)malloc(ln+1);
		strcpy(passw,pwd);

	}
	DBContainer::DBContainer(char* database,char* usr,char* pwd)
	{
		int status;
		char ErrorMessage[1024];
		setup(database,usr,pwd);
		status = DBConnexion(database,usr,pwd,ErrorMessage);

		ociError	= (OCIError*)get_ErrorHandle();
		ociEnv		= (OCIEnv*)get_OCIEnv(); /* the environment handle */ 
		ociHdbc		= (OCISvcCtx*)get_ContxtHdl(); /* the context handle */
		mysrvhp		= (OCIServer*)get_ServerHandle(); /* the server handle */
		myusrhp	= (OCISession*)get_SessionHandle; /* user session handle */
	}
	DBContainer::DBContainer(char* database,char* usr,char* pwd,OCIEnv*& myenvhp,OCIServer*& mysrvhp1,OCISession*& myusrhp1, OCISvcCtx*& mysvchp, OCIError*& myerrhp, char* ErrorMessage)
	{
		int status;
		setup(database,usr,pwd);
		status = DBConnexion(database,usr,pwd,ErrorMessage);

		ociError	= (OCIError*)get_ErrorHandle();
		ociEnv		= (OCIEnv*)get_OCIEnv(); /* the environment handle */ 
		ociHdbc		= (OCISvcCtx*)get_ContxtHdl(); /* the context handle */
		mysrvhp		= (OCIServer*)get_ServerHandle(); /* the server handle */
		myusrhp	= (OCISession*)get_SessionHandle; /* user session handle */
		myenvhp	= ociEnv;
		mysrvhp1	= mysrvhp;
		myusrhp1	= myusrhp;
		mysvchp	= ociHdbc;
		myerrhp	= ociError;
	}
	DBContainer::~DBContainer()
	{
	}

	int DBContainer::Connect(void)  
{
int c=0;
int len_buffer=1000;
char* ErrorMess=new char[len_buffer];
std::string ErrorMess_copy;

 c=DBConnexion(serv,user,passw,ErrorMess);
 if(c!=0)
 {
	 ErrorMess_copy=ErrorMess;
	 delete [] ErrorMess;
//	 throw CONFDBEXCEPTION(ErrorMess_copy);
	 return c;
 }
  ociEnv  = (OCIEnv*)get_OCIEnv();
  ociError = (OCIError*)get_ErrorHandle();
  ociHdbc = (OCISvcCtx*)get_ContxtHdl();

 delete [] ErrorMess;
return c;
}

int DBContainer::Disconnect(void)  
{
int c=0;
int len_buffer=1000;
char* ErrorMess=new char[len_buffer];
std::string ErrorMess_copy;

c=DBDeconnexion(ErrorMess);
if(c!=0)
{
	ErrorMess_copy=ErrorMess;
	delete [] ErrorMess;
	//throw CONFDBEXCEPTION(ErrorMess_copy);
	return c;
}
delete [] ErrorMess;
return c;
}

int DBContainer::followPath(eConnectivityCache *ecache, iConnectivityCache *icache, int elnkidx, HopTyp *hop, int *i, int dep)
{
		HopTyp ehops[100];
		HopTyp ihops[100];
		HopTyp hopar[100];
		char prefix[1000];
    int j,k, inres, enres;
		for (j=0;j<dep;j++)
		{
			prefix[j]	= 32;	
			prefix[j+1]	= 0;
		}
		printf("%s ", prefix); ecache->print_row(elnkidx);
		
		inres  = icache->FindinCol(&icache->portidfrom,ecache->portidto.bufferp[elnkidx], 1, ihops);
	if (inres <= 0) return 0;
		
		for (k=0;k<inres;k++)
		{
			if (inres > 1) printf("\n");
			enres  = ecache->FindinCol(&ecache->portidfrom,icache->portidto.bufferp[ihops[k].h], 2,ehops);
			if (enres <= 0)
			{
				return 0;
			}
			if (enres > 1)
			{
				printf("multiple external connections from one source (FANOUT) not allowed\n");
				return 0;
			}
			for (j  = 0;j<enres;j++)
			{
//				ecache->print_row(ehops[j].h);
				printf("%s ", prefix); icache->print_row(ihops[k].h);
				dep++;
				followPath(ecache,icache,ehops[j].h,hopar,i, dep);
			}
		}
    return 0;
}

	ColDesc::ColDesc()
{
	colwid	= 0;
	def	= 0;
	stmt	= 0;
	Position = 0;
	nrow	= 0;
	dtyp	= 0;
	format[0]	= 0;
}
 ColDesc::~ColDesc()
{
	free(*bptr);
	colwid	= 0;
	def	= 0;
	stmt	= 0;
	Position = 0;
	nrow	= 0;
	dtyp	= 0;
	format[0]	= 0;
}

void ColDesc::set_colwid(int cw)
{
	colwid	= cw;
}
	ColDesc_int::ColDesc_int()
  {
		bptr	= (void**)&bufferp;
    bufferp = 0;
		dtyp	= SQLT_INT;
		colwid	= 4;
  };
	void ColDesc_int::print(int indx)
  {
    printf("%i ",bufferp[indx]);
  }
	int ColDesc_int::allocate(OCIStmt *statement, int pos, int numrow)
{
  OCIError* ociError ;
  int rescode;
  int status = 0;
	stmt	= statement;
	Position	= pos;
	colwid	= 4;
	incrow	= numrow;
	nrow	+= numrow;

  ociError = (OCIError*)get_ErrorHandle();
	bsize	= nrow*colwid;
  bufferp  = (int*)malloc(bsize);
  if (bufferp == 0)
  {
    return -1;
  }
  status=OCIDefineByPos (stmt, &def, ociError, Position, *bptr, 
		colwid, dtyp, 0, 0, 0, OCI_DEFAULT/*OCI_DYNAMIC_FETCH*/ );
	//status = OCIDefineDynamic(def, ociError, this,  ColMore);
  if (status != 0)
  {
    free (bufferp);
    bufferp  = 0;
    colwid = 0;
		nrow	= 0;
    rescode=ShowErrors (status, ociError, "OCIParamGet1 unsuccessful");
  }
return 0;
}


ColDesc_char::ColDesc_char()
  {
		bptr	= (void**)&bufferp;
    bufferp = 0;
		dtyp	= SQLT_STR;
		colwid	= 0;
  };
  ColDesc_char::ColDesc_char(int cwid)
  {
		bptr	= (void**)&bufferp;
    bufferp = 0;
		dtyp	= SQLT_STR;
		colwid	= cwid;
  };
  void ColDesc_char::print(int indx)
  {
    printf("%s ",&bufferp[indx*colwid]);
  }
  int ColDesc_char::allocate(OCIStmt *statement, int pos, int numrow)
{
  OCIParam *parmdp;
  OCIError* ociError ;
  int status;
  char *r =0;
  int rescode;
  int itml=0;
	nrow	+= numrow;
  ociError = (OCIError*)get_ErrorHandle();
	stmt	= statement;
	if (colwid == 0)
	{
		status=OCIParamGet(statement, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) pos);

		if(status==OCI_SUCCESS)
		{
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &colwid, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);
			if (status == OCI_SUCCESS)
			{
				colwid++;
				bsize	= numrow*colwid;
				bufferp = (char*)malloc(bsize);
			}
		}
    status=OCIDefineByPos (statement, &def, ociError,pos, *bptr, colwid, dtyp, 0, 0, 0, OCI_DEFAULT);
    if (status != 0)
    {
      free (bufferp);
      bufferp  = 0;
      colwid = 0;
			bsize	= 0;
      rescode=ShowErrors (status, ociError, "OCIParamGet1 unsuccessful");
    }
  }
	else
	{
		colwid++;
		bsize	= numrow*colwid;
		bufferp = (char*)malloc(bsize);
    status=OCIDefineByPos (statement, &def, ociError,pos, *bptr, colwid, dtyp, 0, 0, 0, OCI_DEFAULT);
    if (status != 0)
    {
      free (bufferp);
      bufferp  = 0;
      colwid = 0;
			bsize	= 0;
      rescode=ShowErrors (status, ociError, "OCIParamGet1 unsuccessful");
    }
	}

  return status;
}



ConnectivityCache::ConnectivityCache (DBContainer *dbase)
  {
    db  = dbase;
    ociEnv  = db->gOCIEnv();
    ociError  = db->gErrorHandle();
    ociHdbc = db->gContxtHdl();
  }

	int ConnectivityCache::get_col_wid(OCISvcCtx* ociHdbc, void * env, char *selectstr)
	{
		int status;
		OCIStmt *stmthp;
		ColDesc_int cwid;
		

		status = OCIHandleAlloc ( env, (void**)&stmthp, OCI_HTYPE_STMT, 0, 0 );
    if (status != 0) goto end;
    status=OCIStmtPrepare(stmthp, ociError, (text*) selectstr,(ub4) strlen(selectstr),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
    if (status != 0) goto end;
    status=OCIStmtExecute(ociHdbc, stmthp, ociError, 0, 0,(OCISnapshot *) 0, (OCISnapshot *) 0, OCI_DEFAULT );
    if (status != 0) goto end;
		cwid.allocate(stmthp,1,1);
		status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT,1, OCI_DEFAULT);
end:
		if (status != 0)
		{
			ShowErrors (status, ociError, "OCI calls unsuccessful");
			return -1;
		}
		int ret	= *cwid.bufferp;
		return ret;
	}
	int ConnectivityCache::getnumrows(int system)
	{
		int status;
		OCIDefine* def;
		int nchar;
		//OCIStmt *stmt;
		nchar	= strlen("select count(*) from (")+strlen(")")+strlen(cntselect);
		char *numselect	= (char*)malloc(nchar);
		sprintf(numselect,"select count(*) from (%s)\0", cntselect);
    status =OCIHandleAlloc (ociEnv, (void**)&stmthp, OCI_HTYPE_STMT , 0, 0);
    if (status != 0) goto end;
    status=OCIStmtPrepare(stmthp, ociError, (text*) numselect,(ub4) strlen(numselect),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
    if (status != 0) goto end;
    status =OCIBindByName(stmthp, &bndp, ociError,(text*) ":system", -1,(dvoid*)&system, 4,  SQLT_INT, (dvoid *)0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
    if (status != 0) goto end;

    //status=OCIAttrSet (stmthp,OCI_HTYPE_STMT,&maxrow,0,OCI_ATTR_PREFETCH_ROWS,ociError);
    //if (status != 0) goto end;
    status=OCIStmtExecute(ociHdbc, stmthp, ociError, 0, 0,(OCISnapshot *) 0, (OCISnapshot *) 0, OCI_DEFAULT );
    if (status != 0) goto end;
	int nrows;
  OCIError* ociError ;
  int rescode;

  ociError = (OCIError*)get_ErrorHandle();
  status=OCIDefineByPos (stmthp, &def, ociError, 1, &nrows, 
		4, SQLT_INT, 0, 0, 0, OCI_DEFAULT/*OCI_DYNAMIC_FETCH*/ );
  if (status != 0)
  {
		nrows	= 0;
    rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
  }

		status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT,1, OCI_DEFAULT);
     rescode=ShowErrors (status, ociError, "OCIStmtFetch2 unsuccessful");

end:;
    return nrows;
  }
  int ConnectivityCache::Fill(int system)
  {
    int status;
		OCISvcCtx *svchp = ociHdbc;
		OCITrans *txnhp;
		//status	= OCIHandleAlloc( ociEnv, (void**)&svchp, OCI_HTYPE_SVCCTX, 0, 0);
		OCIHandleAlloc((dvoid *)ociEnv, (dvoid **)&txnhp,  OCI_HTYPE_TRANS, 0, 0);
		OCIAttrSet((dvoid *)svchp, OCI_HTYPE_SVCCTX, (dvoid *)txnhp, 0,
														OCI_ATTR_TRANS, ociError);


		status	= OCITransStart(svchp, ociError, 100, OCI_TRANS_NEW+OCI_TRANS_READONLY);
		ShowErrors (status, ociError, "Transaction Start Failed");
		printf("getting number of returned rows...");
    maxrow  = getnumrows(system);
		printf("%i...done\n",maxrow);
    status =OCIHandleAlloc (ociEnv, (void**)&stmthp, OCI_HTYPE_STMT , 0, 0);
    if (status != 0) goto end;
    status=OCIStmtPrepare(stmthp, ociError, (text*) selectstring,(ub4) strlen(selectstring),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
    if (status != 0) goto end;
    status =OCIBindByName(stmthp, &bndp, ociError,(text*) ":system", -1,(dvoid*)&system, 4,  SQLT_INT, (dvoid *)0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
    if (status != 0) goto end;

    status=OCIAttrSet (stmthp,OCI_HTYPE_STMT,&maxrow,0,OCI_ATTR_PREFETCH_ROWS,ociError);
    if (status != 0) goto end;
    status=OCIStmtExecute(ociHdbc, stmthp, ociError, 0, 0,(OCISnapshot *) 0, (OCISnapshot *) 0, OCI_DEFAULT );
    if (status != 0) goto end;

    alloc_buffs();

    fetched_row=0;
		status	= 0;
		status =OCIStmtFetch2(stmthp, ociError, maxrow, OCI_FETCH_NEXT,1, OCI_DEFAULT);
    int rescode=ShowErrors (status, ociError, "OCIStmtFetch2 unsuccessful");
		//status =OCIStmtFetch2(stmthp, ociError, maxrow/2, OCI_FETCH_NEXT,1, OCI_DEFAULT);
  //  rescode=ShowErrors (status, ociError, "OCIStmtFetch2 unsuccessful");
		//print_row(0);

		if ((status == 100) || (status == 0))
		{
			int stat1;
			int fetch;
			stat1=OCIAttrGet((dvoid*) stmthp, (ub4) OCI_HTYPE_STMT,(dvoid*) &fetch, (ub4 *) 0, (ub4) OCI_ATTR_ROWS_FETCHED ,(OCIError *) ociError);
			fetched_row	+= fetch;
		}

end:;
		status	= OCITransCommit(svchp, ociError, OCI_DEFAULT );
		status	= OCIHandleFree((dvoid *)txnhp,  OCI_HTYPE_TRANS);
		status	= OCIAttrSet((dvoid *)svchp, OCI_HTYPE_SVCCTX, (dvoid *)0, 0,
														OCI_ATTR_TRANS, ociError);

    return status;
  }
  int ConnectivityCache::FindinCol(ColDesc_int *col, int val, int typ, HopTyp *res)
  {
    int i;
    int j;
    j = 0;
    for (i=0;i<fetched_row;i++)
    {
      if (col->bufferp[i] == val)
      {
        res[j].h  = i;
        res[j].typ  = typ;
        j++;
      }
    }
	if(j==0){
		res[0].h=0;
		res[0].typ=0;
	}
    return j;
  }
  int ConnectivityCache::FindinCol(ColDesc_char *col, char *val, int typ, HopTyp *res)
  {
    int i;
    int lv;
    int j;
    j = 0;
    lv = strlen(val);
    for (i=0;i<fetched_row;i++)
    {
	    if (strncmp(col->bufferp+i*col->colwid, val, lv) == 0)
//		if (strncmp(&col->bufferp[i*col->colwid], val, lv) == 0)	
      {
        res[j].h  = i;
        res[j].typ  = typ;
        j++;
      }
    }
	if(j==0){
		res[0].h=0;
		res[0].typ=0;
	}
    return j;
  }



	eConnectivityCache::eConnectivityCache (DBContainer *dbase) : ConnectivityCache (dbase)
  {
		selectstring  = &"select\
											lnk.linkid,\
											lnk.system_name,\
											frdev.deviceid,\
											frdev.devicename||' ',\
											lnk.portidfrom,\
											frport.port_nbr||'|'||nvl(frport.port_type,'none')||'?',\
											todev.deviceid,\
											todev.devicename||' ',\
											lnk.portidto,\
											toport.port_nbr||'|'||nvl(toport.port_type,'none')||'?',\
											lnk.link_type,\
											lnk.bidirectional_link_used,\
											lnk.lkused,\
											lnk.link_weight,\
											nvl(lnk.link_info,'none')||'?',\
											fr_dtype.devicetypeid,\
											fr_dtype.devicetype||' ',\
											to_dtype.devicetypeid,\
											to_dtype.devicetype||' '\
										 from\
											lhcb_macroscopic_connectivity lnk,\
											lhcb_port_properties frport,\
											lhcb_port_properties toport,\
											lhcb_lg_devices frdev,\
											lhcb_lg_devices todev,\
											lhcb_lg_device_types fr_dtype,\
											lhcb_lg_device_types to_dtype\
										 where\
											mod(lnk.system_name, :system)=0 and\
											(fr_dtype.devicetypeid=frdev.devicetypeid) and\
											(to_dtype.devicetypeid=todev.devicetypeid) and\
											(frport.portid = lnk.portidfrom) and\
											(toport.portid=lnk.portidto) and\
											(frdev.deviceid=frport.deviceid) and\
											(todev.deviceid=toport.deviceid)";


  cntselect  = &"select lnk.linkid, lnk.system_name \
from lhcb_macroscopic_connectivity lnk \
where lnk.lkused=1 and \
mod(lnk.system_name,:system)=0 ";

/////////////////
  cntselect_2  = &"select lnk.linkid, lnk.system_name \
from lhcb_macroscopic_connectivity lnk, lhcb_subsystemlist sys \
where lnk.lkused=1 and mod(lnk.system_name,sys.systemid)=0 \
and sys.systemid in (:system_list) ";

  }
  void eConnectivityCache::alloc_buffs(void)
  {
    int status;
		int cw;
    status = linkid.allocate(stmthp, 1, maxrow);
		
    status = devfrom.allocate(stmthp, 3, maxrow );
    status = devto.allocate(stmthp, 7, maxrow);
		cw	= get_col_wid(ociHdbc, ociEnv, "select max(length(port_nbr||'|'||nvl(port_type,'none')||'?')) from lhcb_port_properties");
		pnumfrom.set_colwid(cw);
		status = pnumfrom.allocate(stmthp, 6, maxrow);
		pnumto.set_colwid(cw);
    status = pnumto.allocate(stmthp, 10, maxrow);
    status = linktyp.allocate(stmthp, 11, maxrow);
    status = bidir.allocate(stmthp, 12, maxrow);
    status = linkused.allocate(stmthp, 13, maxrow);
    status = linkweight.allocate(stmthp, 14, maxrow);
		cw	= get_col_wid(ociHdbc, ociEnv, "select max(length(devicename||'?')) from lhcb_lg_devices");
    devnamfrom.set_colwid(cw);
    status = devnamfrom.allocate(stmthp, 4, maxrow);
		devnamto.set_colwid(cw);
    status = devnamto.allocate(stmthp, 8, maxrow);
		cw	= get_col_wid(ociHdbc, ociEnv, "select max(length(nvl(link_info,'none')||'?')) from lhcb_macroscopic_connectivity");
    linkinfo.set_colwid(cw);
    status = linkinfo.allocate(stmthp, 15, maxrow);
    status = sysnam.allocate(stmthp, 2, maxrow);
    status = portidfrom.allocate(stmthp, 5, maxrow);
    status = portidto.allocate(stmthp, 9, maxrow);
    //status = odevid.allocate(stmthp, 16, maxrow);

	status = devtypefrom_id.allocate(stmthp, 16, maxrow);	//device type id
		cw = get_col_wid(ociHdbc, ociEnv, "select max(length(devicetype||' ')) from lhcb_lg_device_types");
		devtypefrom.set_colwid(cw);
	status = devtypefrom.allocate(stmthp, 17, maxrow);
	status = devtypeto_id.allocate(stmthp, 18, maxrow);
		devtypeto.set_colwid(cw);
	status = devtypeto.allocate(stmthp, 19, maxrow);
  }
  //virtual void realloc_buffs(void)
  //{
  //  int status;
  //  status = linkid.reallocate(maxrow);
  //  status = devfrom.reallocate(maxrow);
  //  status = devto.reallocate(maxrow);
  //  status = pnumfrom.reallocate(maxrow);
  //  status = pnumto.reallocate(maxrow);
  //  status = linktyp.reallocate( maxrow);
  //  status = bidir.reallocate( maxrow);
  //  status = linkused.reallocate( maxrow);
  //  status = linkweight.reallocate( maxrow);
  //  status = devnamfrom.reallocate(  maxrow);
  //  status = devnamto.reallocate(  maxrow);
  //  status = linkinfo.reallocate( maxrow);
  //  status = sysnam.reallocate( maxrow);
  //  status = portidfrom.reallocate( maxrow);
  //  status = portidto.reallocate(maxrow);
  //  status = odevid.reallocate(maxrow);
  //}
  void eConnectivityCache::print_row(int r)
  {
	 if(r==-1){
		  printf("\nno external path to device\n");
	 }
	 else if(r==-2){
		  printf("\nno external path from device\n");
	 }
	 else{
		printf("Ext: ");
		linkid.print(r);
		devfrom.print(r);
		portidfrom.print(r);
		devnamfrom.print(r);
		pnumfrom.print(r);
		printf(" -----> ");
		devto.print(r);
		portidto.print(r);
		devnamto.print(r);
		pnumto.print(r);
		printf("\n");
//    printf("%i\n",odevid.bufferp[r]);
	  }
  }
  /***************************************/
  void eConnectivityCache::print_row_devType(int r)
  {
	  if(r==-1){
		  printf("\nno external path to device\n");
	 }
	 else if(r==-2){
		  printf("\nno external path from device\n");
	 }
	 else{
		printf("Ext: ");
		linkid.print(r);
		devfrom.print(r);
		portidfrom.print(r);
		devnamfrom.print(r);
		printf("/ ");
		devtypefrom.print(r);	//print type of device from
		pnumfrom.print(r);
		printf(" -----> ");
		devto.print(r);
		portidto.print(r);
		devnamto.print(r);
		printf("/ ");
		devtypeto.print(r);		//print type of device to
		pnumto.print(r);
		printf("\n");
	 }
  }
  /***************************************/
  void eConnectivityCache::print_row_linkType(int r)
  {
	  if(r==-1){
		  printf("\nno external link of given type to device\n");
	 }
	 else if(r==-2){
		  printf("\nno external link of given type from device\n");
	 }
	 else{
		printf("Ext: ");
	    linkid.print(r);
		linktyp.print(r);		//print type of link
	    devfrom.print(r);
		portidfrom.print(r);
		devnamfrom.print(r);
		pnumfrom.print(r);
		printf(" -----> ");
		devto.print(r);
		portidto.print(r);
		devnamto.print(r);
		pnumto.print(r);
		printf("\n");
	 }
  }
  /***************************************/

iConnectivityCache::iConnectivityCache (DBContainer *dbase) : ConnectivityCache (dbase)
  {

selectstring  = &"select distinct lnk.linkid,\
  fromdev.deviceid,\
  fromdev.devicename||'?',\
  fromport.portid,\
  fromport.port_nbr||'|'||nvl(fromport.port_type,'none')||'?',\
  todev.deviceid,\
  todev.devicename||'?',\
  toport.portid,\
  toport.port_nbr||'|'||nvl(toport.port_type,'none')||'?'\
from LHCB_MICROSCOPIC_CONNECTIVITY lnk,\
  lhcb_port_properties fromport,\
  lhcb_lg_devices fromdev,\
  lhcb_port_properties toport,\
  lhcb_lg_devices todev\
  where fromport.portid=lnk.portidfrom  and\
  fromdev.deviceid=fromport.deviceid and\
  toport.portid=lnk.portidto and\
  todev.deviceid=toport.deviceid and\
	mod(fromdev.system_name,:system)=0";

  cntselect  = &"select lnk.linkid\
  from LHCB_MICROSCOPIC_CONNECTIVITY lnk,\
  lhcb_port_properties fromport,\
  lhcb_lg_devices fromdev,\
  lhcb_port_properties toport,\
  lhcb_lg_devices todev\
  where fromport.portid=lnk.portidfrom  and\
  fromdev.deviceid=fromport.deviceid and\
  toport.portid=lnk.portidto and\
  todev.deviceid=toport.deviceid  and\
	mod(fromdev.system_name,:system)=0 ";

  }


  //virtual void realloc_buffs(void)
  //{
  //  int status;
  //  status = linkid.reallocate(maxrow);
  //  status = devfrom.reallocate(maxrow);
  //  status = devnamfrom.reallocate(maxrow);
  //  status = portidfrom.reallocate(maxrow);
  //  status = pnumfrom.reallocate(maxrow);
  //  status = devto.reallocate(maxrow);
  //  status = devnamto.reallocate(maxrow);
  //  status = portidto.reallocate(maxrow);
  //  status = pnumto.reallocate(maxrow);
  //}
   void iConnectivityCache::alloc_buffs(void)
  {
    int status;
		int cw;

    status = linkid.allocate(stmthp, 1, maxrow);
    status = devfrom.allocate(stmthp, 2, maxrow);
		cw	= get_col_wid(ociHdbc, ociEnv, "select max(length(devicename||'?')) from lhcb_lg_devices");
    devnamfrom.set_colwid(cw);
		devnamto.set_colwid(cw);
    status = devnamfrom.allocate(stmthp, 3, maxrow);
    status = portidfrom.allocate(stmthp, 4, maxrow);
		cw	= get_col_wid(ociHdbc, ociEnv, "select max(length(port_nbr||'|'||nvl(port_type,'none')||'?')) from lhcb_port_properties");
		pnumfrom.set_colwid(cw);
		pnumto.set_colwid(cw);
    status = pnumfrom.allocate(stmthp, 5, maxrow);
    status = devto.allocate(stmthp, 6, maxrow);
    status = devnamto.allocate(stmthp, 7, maxrow);
    status = portidto.allocate(stmthp, 8, maxrow);
    status = pnumto.allocate(stmthp, 9, maxrow);
  }
  void iConnectivityCache::print_row(int r)
  {
	  if (r==-1){	//case where device doesn't have internal connectivity
		  printf("no internal connectivity\n");
	  }
	  else if (r==-2){//case where there's only one possible internal connectivity
		  printf("no internal connectivity - only one input/output in the device\n");
	  }
	  else {
    printf("Int: ");
    linkid.print(r);
    devfrom.print(r);
    portidfrom.print(r);
    devnamfrom.print(r);
    pnumfrom.print(r);
    printf(" -----> ");
    devto.print(r);
    portidto.print(r);
    devnamto.print(r);
    pnumto.print(r);
    printf("\n");
//    printf("%i\n",odevid.bufferp[r]);
	  }
  }

/*****************************************
*GetAllPathsPerDevice_LinkType - shows all links of the given type going through device
*to find paths going through the device with exact device name the name should be given with space at the and (ex. "MUONC_Q4 ")
*reload_cache - 1 if you want to load or reload the cache (of the 'system_name' subsystem); 1 should be set after a change or if it is the first time or if the device is not in the same subsystem, otherwise 0
*****************************************/
int DBContainer::GetAllPathsPerDevice_LinkType(char* dev_name, int link_type, eConnectivityCache *ecache, iConnectivityCache *icache, int system_name, int reload_cache){
	printf("**********************");
	printf("\nGetAllPathsPerDevice_LinkType \ndevice: %s\nlink type: %i\n",dev_name, link_type);

	HopTyp ehop_to[1000];
	HopTyp ehop_from[1000];
	HopTyp ihop[1000];
	int i,j,k,inres, enres_to, enres_from;
	char prefix[2];
	prefix[0]=32;		
	prefix[1]=0;

	if (reload_cache==1)	{
		ecache->Fill(system_name);
		icache->Fill(system_name);
	}

	enres_to  = ecache->FindinCol_LinkType(&ecache->devnamto,dev_name,&ecache->linktyp, link_type, 2, ehop_to);		//finding links going to 'dev_name' device
	if(enres_to > 1){														//if there's more than one external link to device
		printf("\n%i external connections of given link type to device", enres_to);
	}

	inres  = icache->FindinCol(&icache->devnamto,dev_name, 1, ihop);		//finding internal connections in 'dev_name'	
	if(inres > 1){														//if there's more than one internal path
		printf("\n%i internal connections", inres);
	}

	enres_from  = ecache->FindinCol_LinkType(&ecache->devnamfrom,dev_name, &ecache->linktyp, link_type, 2, ehop_from);	//finding links going from 'dev_name' device to any device 
	if(enres_from > 1){													//if there's more than one external path from device
		printf("\n%i external connections of given link type from device\n", enres_from);}
	
	if(enres_to==0 && inres==0 && enres_from==0){					//if no external and no internal connectivity
		printf("\nNo device like %s having links type %i found\n",dev_name, link_type);
		return -1;
	}
	
	if(inres>0){	//if there is any internal connection
		for(j=0;j<inres;j++){		//for every internal connection
			printf("\n%i.\n",j+1);

			enres_to  = ecache->FindinCol_LinkType(&ecache->portidto,icache->portidfrom.bufferp[ihop[j].h], &ecache->linktyp, link_type, 2, ehop_to);	//for j-th internal connection find such external link where portidto of external link = portidfrom of internal one
			if(enres_to > 1){
				printf("multiple external connections to one source not allowed\n");
				return -1;
			}
			if(enres_to > 0){	
				for(i=0;i<enres_to;i++){	//for every external path going to the port originating internal connectivity
					printf("%s ", prefix);
					ecache->print_row_linkType(ehop_to[i].h);
					printf("%s ", prefix);
					icache->print_row(ihop[j].h);
				}
			}

			enres_from = ecache->FindinCol_LinkType(&ecache->portidfrom,icache->portidto.bufferp[ihop[j].h], &ecache->linktyp, link_type, 2, ehop_from);	//for j-th internal connection find such external link where portidfrom of external link = portidto of internal one
			if (enres_from > 1)		//if more than one external path from the same port found
			{
				printf("multiple external connections from one source (FANOUT) not allowed\n");
				return -1;
			}
			if(enres_from > 0){	
				for(k=0;k<enres_from;k++){	//for every external path going from port terminating internal connectivity
					printf("%s %s ", prefix,prefix);
					ecache->print_row_linkType(ehop_from[k].h);
				}
			}

		}
	}
	if(inres==0){				//if there is no internal connection in device
		if (enres_to==0){			//if no external path going to device
			printf("\nno external links type %i to %s\n", link_type,  dev_name);
		}else {
			printf("\n");	
			for(i=0;i<enres_to;i++){	//for every external path going to device
				printf("%s ", prefix);
				ecache->print_row_linkType(ehop_to[i].h);}
			//if (enres_to==1&&enres_from!=1){	//if there's only one external link to device
			//	printf("no internal connectivity in %s - only one input to device\n",dev_name);
			//}
		}
		printf("\nno internal connectivity in %s\n",dev_name);

		if (enres_from==0){				//if no external path from device
			printf("\nno external links type %i from %s\n", link_type, dev_name);
		}else {
			printf("\n");
		//	if (enres_from==1&&enres_to!=1){		//if only one external path from device
		//		printf("no internal connectivity in %s - only one output from device\n",dev_name);
		//	}else 
		//	if(enres_from==1&&enres_to==1){		//if only one path from and only one path to device
		//		printf("no internal connectivity in %s - only one input and one output in device\n\n",dev_name);
		//	}
			for(k=0;k<enres_from;k++){				//for every external path going from device
				printf("%s %s ", prefix, prefix);
				ecache->print_row_linkType(ehop_from[k].h);}
		}
	}	
	return 0;
}
/*********************************************/
  int ConnectivityCache::FindinCol_LinkType(ColDesc_char *col, char *val, ColDesc_int *lk, int lktype, int typ, HopTyp *res)
  {
    int i;
    int lv;
    int j;
    j = 0;
    lv = strlen(val);
    for (i=0;i<fetched_row;i++)
    {
	    if ((strncmp(col->bufferp+i*col->colwid, val, lv) == 0)&&(lk->bufferp[i] == lktype))
      {
        res[j].h  = i;
        res[j].typ  = typ;
        j++;
      }
    }
	if(j==0){
		res[0].h=0;
		res[0].typ=0;
	}
    return j;
  }
/*******************************************/
  int ConnectivityCache::FindinCol_LinkType(ColDesc_int *col, int val, ColDesc_int *lk, int lktype, int typ, HopTyp *res)
  {
    int i;
    int j;
    j = 0;
    for (i=0;i<fetched_row;i++)
    {
      if ((col->bufferp[i] == val)&&(lk->bufferp[i] == lktype))
      {
        res[j].h  = i;
        res[j].typ  = typ;
        j++;
      }
    }
	if(j==0){
		res[0].h=0;
		res[0].typ=0;
	}
    return j;
  }
/******************************************
*followPath_device_LinkType - follows links type of link_type between devices until dev_name; first time should be invoked with dep=1
*******************************************/
int DBContainer::followPath_device_LinkType(char* dev_name, int link_type, eConnectivityCache *ecache, iConnectivityCache *icache, int elnkidx, HopTyp *hop, int *i, int dep)
{
		HopTyp ehops[1000];	
		HopTyp ihops[1000];
		HopTyp hopar[1000];
		char prefix[100];
		int j,k,m, inres, enres_from, len;//int enres_to;
		char* device;
		static int eidx[100];	
		static int iidx[100];
		len=strlen(dev_name);
				
		eidx[dep-1]=elnkidx;
		device=&ecache->devnamto.bufferp[elnkidx*ecache->devnamto.colwid];	//name of the device to which the external path elnkidx is going to

		if (strncmp(device, dev_name, len) == 0){			//if external path elnkidx is going to dev_name connection
			*i=*i+1;										//i - counter of connections found
			printf("\n%i. connection:\n",*i);				
			prefix[0]	= 32;prefix[1]	= 0;
			for (m=0;m<dep-1;m++){	//printing connection
				prefix[m]=32;	
				prefix[m+1]=0;
				printf("%s", prefix);
				ecache->print_row_linkType(eidx[m]);
				printf("%s", prefix);
				icache->print_row(iidx[m]);
			}
			printf("%s ", prefix);
			ecache->print_row_linkType(eidx[dep-1]);
		}

    inres  = icache->FindinCol(&icache->portidfrom,ecache->portidto.bufferp[elnkidx], 1, ihops);	//searching for internal paths
	if (inres < 0) {
		return -1;}
	dep++;
	if (inres > 0) {		//if any internal path found
		for (k=0;k<inres;k++)
		{
			enres_from = ecache->FindinCol_LinkType(&ecache->portidfrom,icache->portidto.bufferp[ihops[k].h],&ecache->linktyp, link_type, 2,ehops); //searching for external paths going from device
			if (enres_from < 0){
				return -1;
			}
			if (enres_from == 0){
				printf("\nno external paths from device\n");
				return -1;
			}
			if (enres_from > 1)	//if there's more than one external path going from the same port
			{
				printf("\nmultiple external connections from one source (FANOUT) not allowed\n");
				return -1;
			}
			for (j  = 0;j<enres_from;j++)
			{	
				iidx[dep-2]=ihops[k].h;	//(dep-2 because dep was already increased once)
				followPath_device_LinkType(dev_name,link_type, ecache,icache,ehops[j].h,hopar, i, dep);
			}
		}	
	}

	if(inres==0){	//if no internal path found
		//enres_to = ecache->FindinCol_LinkType(&ecache->devnamto,device,&ecache->linktyp, link_type, 2, ehops);	//searching for external paths going to device
		enres_from = ecache->FindinCol_LinkType(&ecache->devnamfrom,device, &ecache->linktyp, link_type,2, ehops);	//searching for external paths going from device
	
		//if(enres_from==1||enres_to==1){
		//	iidx[dep-2]=-2;	
		//}
		//else 
		iidx[dep-2]=-1;
		
		for (j=0;j<enres_from;j++){		//for every external path going from device
			followPath_device_LinkType(dev_name,link_type,ecache,icache,ehops[j].h,hopar,i,dep);
		}
	}
		    return *i;
			}
/******************************************
*GetConnectivityBetweenDevices_LinkType - searching for connectivity type of link_type between devices dev_from and dev_to:
*dev_from - should be with space at the end in other case searching for every device with 'dev_from' as a part of the name
*dev_to - should be with space at the end in other case searching for every device with 'dev_to' as a part of the name
*reload_cache - 1 if you want to load or reload the cache (of the 'system_name' subsystem); 1 should be set after a change or if it is the first time or if the device is not in the same subsystem, otherwise 0
*******************************************/
int DBContainer::GetConnectivityBetweenDevices_LinkType(char* dev_from, char* dev_to, int link_type, eConnectivityCache *ecache, iConnectivityCache *icache, int system_name, int reload_cache){
	printf("*******************************");
	printf("\nGetConnectivityBetweenDevices_LinkType:\ndev_from: %s\ndev_to: %s\nlink type: %i\n", dev_from,dev_to, link_type);

	HopTyp hop[1000];
	int i = 0,k, enres;

	if (reload_cache==1)	{
		ecache->Fill(system_name);
		icache->Fill(system_name);
	}

	enres  = ecache->FindinCol_LinkType(&ecache->devnamfrom,dev_from, &ecache->linktyp, link_type, 2, hop);	//searching for external paths going from 'dev_from'
	//printf("enres: %i\n", enres);
	for(k=0;k<enres;k++){			//for every external path found
	i=followPath_device_LinkType(dev_to,link_type, ecache,icache,hop[k].h,hop,&i,1);
	}
	if(i==1){
		printf("\n%i connection found\n", i);
	}
	if(i>1){
		printf("\n%i connections found\n", i);
	}
	if(i<0){
		return -1;
	}
	if(i==0) {
		printf("\nfollow path reverse...\n");
					//searching for connectivity in reverse direction (from 'dev_to' to 'dev_from')
		enres  = ecache->FindinCol_LinkType(&ecache->devnamfrom,dev_to,&ecache->linktyp, link_type, 2, hop);	//searching for external paths going from 'dev_to'
		//printf("\nenres rev: %i\n", enres);
		for(k=0;k<enres;k++){		//for every external path found
		i=followPath_device_LinkType(dev_from,link_type, ecache,icache,hop[k].h,hop,&i,1);
		}
		if(i==1){
			printf("%i connection found\n", i);
		}
		if(i>1){
			printf("%i connections found\n", i);
		}
		if(i<0){
			return -1;
		}
		else{
		printf("\nLink type %i between devices not found\n", link_type);
		return -1;
		}
	}

return 0;
}
/*****************************************
*followPath_normal_LinkType - follows link type of link_type going from device until there's no internal connectivity in the device; first time should be invoked with dep=1; 
*ecache->FindinCol_LinkType(ColDesc_char *col, char *dev_name, ColDesc_int *lk, int lktype, int typ, HopTyp *hop) should be invoked before (to fill hop[] used in followPath_normal)
******************************************/
int DBContainer::followPath_normal_LinkType(int link_type, eConnectivityCache *ecache, iConnectivityCache *icache, int elnkidx, HopTyp *hop, int *i, int dep)
{
		HopTyp ehops[1000];
		HopTyp ihops[1000];
		HopTyp hopar[1000];
		char prefix[100];
		char* device;
	    int j,k,m, inres, enres_out,enres_in;
		static int eidx[100];	
		static int iidx[100];

		if(hop[0].typ==0&&dep==1){	//when there's no external path from device
			eidx[dep-1]=-2;	//to print comment that there's no external path from device
		}else
		eidx[dep-1]=elnkidx;

		inres  = icache->FindinCol(&icache->portidfrom,ecache->portidto.bufferp[elnkidx], 1, ihops);
		if (inres == 0) {
			//inres = 0 if there is only one output or only one input, in other case there is end of path
			device=&ecache->devnamto.bufferp[elnkidx*ecache->devnamfrom.colwid]; //name of device with no internal connections
			enres_in= ecache->FindinCol(&ecache->devnamto,device, 2,ehops);	//no. of external paths going to device
			enres_out= ecache->FindinCol(&ecache->devnamfrom,device, 2,ehops);	//no. of external paths going from device
									
			if((enres_out==1||enres_in==1)&&enres_out!=0){	//case when there is only one possible internal path 
				iidx[dep-1]=-2;	//to print comment that there's only one internal path
				dep++;
				
				enres_out= ecache->FindinCol_LinkType(&ecache->devnamfrom,device,&ecache->linktyp, link_type, 2,ehops);	//no. of external of given type going from device
				for(m=0; m<enres_out; m++){	//for every external path going from device
					followPath_normal_LinkType(link_type, ecache,icache,ehops[m].h,hopar,i, dep);}
			}
			else{	//when there's end of the path
				printf("\n");
				prefix[0]=32;	
				prefix[1]=0;
				for (j=0;j<dep-1;j++){	//printing connection
				prefix[j]=32;	
				prefix[j+1]=0;
				printf("%s", prefix);
				ecache->print_row_linkType(eidx[j]);
				printf("%s", prefix);
				icache->print_row(iidx[j]);
				}
				printf("%s ", prefix);
				ecache->print_row_linkType(eidx[dep-1]);
			}
			return 0;
		}
	dep++;
		for (k=0;k<inres;k++)	//for every internal path
		{
			if (inres > 1) printf("\n");
			enres_out  = ecache->FindinCol_LinkType(&ecache->portidfrom,icache->portidto.bufferp[ihops[k].h],&ecache->linktyp, link_type, 2,ehops);	//searching external paths
			if (enres_out <= 0)//if error or if no external path going from the port terminating the internal path
			{
				return -1;
			}	
			if (enres_out > 1)	//more than one external path going from the same port
			{
				printf("multiple external connections from one source (FANOUT) not allowed\n");
				return -1;
			}
			for (j  = 0;j<enres_out;j++)	//for every external path found
			{
				iidx[dep-2]=ihops[k].h;	//(dep-2 because dep was already increased once)
				followPath_normal_LinkType(link_type, ecache,icache,ehops[j].h,hopar,i, dep);
			}
		}
    return 0;
}
/*****************************************
*followPath_reverse_LinkType - follows link type of link_type going to device until there's no internal connectivity in the device; first time should be invoked with dep=1
*ecache->FindinCol_LinkType(ColDesc_char *col, char *dev_name,ColDesc_int *lk, int lktype, int typ, HopTyp *hop) should be invoked before (to fill hop[] used in followPath_reverse)
******************************************/
int DBContainer::followPath_reverse_LinkType(int link_type, eConnectivityCache *ecache, iConnectivityCache *icache, int elnkidx, HopTyp *hop, int *i, int dep)
{
		HopTyp ehops[1000];
		HopTyp ihops[1000];
		HopTyp hopar[1000];
		char prefix[100];
		char* device;
	    int j,k,m, inres, enres_out,enres_in;
		static int eidx[100];	
		static int iidx[100];

		if(hop[0].typ==0&&dep==1){	//when there's no external link to device
			eidx[dep-1]=-1;	//to print comment that there's no external path to device
		}else
		eidx[dep-1]=elnkidx;

		inres  = icache->FindinCol(&icache->portidto,ecache->portidfrom.bufferp[elnkidx], 1, ihops);
		if (inres == 0) {
			//inres = 0 if there is only one output or only one input, in other case there is end of path
			device=&ecache->devnamfrom.bufferp[elnkidx*ecache->devnamfrom.colwid]; //name of device with no internal connections
			enres_out= ecache->FindinCol(&ecache->devnamfrom,device, 2,ehops);	//no. of external paths going from device
			enres_in= ecache->FindinCol(&ecache->devnamto,device, 2,ehops);	//no. of external paths going to device
			
			if((enres_out==1||enres_in==1)&&enres_in!=0){	//case when there is only one possible internal path 
				iidx[dep-1]=-2;	//to print comment that there's only one internal path
				dep++;

				enres_in= ecache->FindinCol_LinkType(&ecache->devnamto,device,&ecache->linktyp, link_type, 2,ehops);	//no. of external links of given type going to device
				for(m=0; m<enres_in; m++){	//for every external path going to device
					followPath_reverse_LinkType(link_type, ecache,icache,ehops[m].h,hopar,i, dep);}
			}

			else{	//when there's end of the path
				printf("\n");
				prefix[0]=32;	
				prefix[1]=0;
				printf("%s ", prefix);
				ecache->print_row_linkType(eidx[dep-1]);	

				for (j=1;j<dep;j++){	//printing connection
				prefix[j]=32;	
				prefix[j+1]=0;
				printf("%s", prefix);
				icache->print_row(iidx[dep-1-j]);
				printf("%s ", prefix);
				ecache->print_row_linkType(eidx[dep-1-j]);
				}
			}
			return 0;
		}
	dep++;
		for (k=0;k<inres;k++)	//for every internal path
		{
			if (inres > 1) printf("\n");
			enres_in = ecache->FindinCol_LinkType(&ecache->portidto,icache->portidfrom.bufferp[ihops[k].h],&ecache->linktyp, link_type, 2,ehops);	//searching external paths
			if (enres_in <= 0)//if error or if no external path going to the port originating the internal path
			{
				return -1;
			}	
			if (enres_in > 1)	//more than one external path going to the same port
			{
				printf("multiple external connections to one source\n");
				return -1;
			}
			for (j  = 0;j<enres_in;j++)	//for every external path found
			{
				iidx[dep-2]=ihops[k].h;	//(dep-2 because dep was already increased once)
				followPath_reverse_LinkType(link_type, ecache,icache,ehops[j].h,hopar,i, dep);
			}
		}
    return 0;
}
//////////////
/////////////
//////////////
int DBContainer::GetConnectivityBetweenDevices(char* dev_from, char* dev_to, eConnectivityCache *ecache, iConnectivityCache *icache,int& len_array, int &pfrom1_arraylen,int &nfrom1_arraylen,int &pto1_arraylen,int &nto1_arraylen,int &lkinfo_arraylen,char* nfrom1_list,char* nto1_list,char* pfrom1_list,int* pfrom_list,char* pto1_list,int* pto_list,int* pathid_list,int* link_pos_list,char* lkinfo_list,int reload_connectivity,int delete_connectivity,char* ErrMess){

	HopTyp hop[1000];
	int i = 0,k, enres;
	int system_name=1;	/////////////////

	if (reload_connectivity==1)	{
		ecache->Fill(system_name);
		icache->Fill(system_name);
	}

	if(ecache->fetched_row==0&&icache->fetched_row==0){
		sprintf(ErrMess,"ERROR in GetConnectivityBetweenDevices : CONNECTIVITY CACHE EMPTY");
		printf("\nconnectivity cache empty\n");
		return -1;
	}

	enres  = ecache->FindinCol(&ecache->devnamfrom,dev_from, 2, hop);	//searching for external paths going from 'dev_from'
	for(k=0;k<enres;k++){			//for every external path found
	i=followPath_device(dev_to, ecache,icache,hop[k].h,hop,&i,1,len_array,pfrom1_arraylen,nfrom1_arraylen,pto1_arraylen,nto1_arraylen,lkinfo_arraylen, nfrom1_list,nto1_list, pfrom1_list,pfrom_list,pto1_list,pto_list, pathid_list,link_pos_list,lkinfo_list);
	}
	if(i>0){
		if(delete_connectivity==1){	
			ecache->deleteConnectivity();
			icache->deleteConnectivity();
			printf("\nconnectivity deleted\n");
		}
		sprintf(ErrMess,"No ERROR in GetConnectivityBetweenDevices");
		return 0;
	}
	if(i<0){
		if(delete_connectivity==1){	
			ecache->deleteConnectivity();
			icache->deleteConnectivity();
			printf("\nconnectivity deleted\n");
		}
		sprintf(ErrMess,"ERROR in GetConnectivityBetweenDevices : NO CONNECTIVITY FOUND");
		return -1;
	}
	if(i==0) {
					//searching for connectivity in reverse direction (from 'dev_to' to 'dev_from')
		enres  = ecache->FindinCol(&ecache->devnamfrom,dev_to, 2, hop);	//searching for external paths going from 'dev_to'
		for(k=0;k<enres;k++){		//for every external path found
		i=followPath_device(dev_from, ecache,icache,hop[k].h,hop,&i,1,len_array,pfrom1_arraylen,nfrom1_arraylen,pto1_arraylen,nto1_arraylen,lkinfo_arraylen, nfrom1_list,nto1_list, pfrom1_list,pfrom_list,pto1_list,pto_list, pathid_list,link_pos_list,lkinfo_list);
		}
		if(i>0){
			if(delete_connectivity==1){	
				ecache->deleteConnectivity();
				icache->deleteConnectivity();
				printf("\nconnectivity deleted\n");
			}
			sprintf(ErrMess,"No ERROR in GetConnectivityBetweenDevices");
			return 0;
		}
		if(i<=0){
			if(delete_connectivity==1){	
				ecache->deleteConnectivity();
				icache->deleteConnectivity();
				printf("\nconnectivity deleted\n");
			}
			sprintf(ErrMess,"ERROR in GetConnectivityBetweenDevices : NO CONNECTIVITY FOUND");
			return -1;
		}
	}

	if(delete_connectivity==1){	
		ecache->deleteConnectivity();
		icache->deleteConnectivity();
		printf("\nconnectivity deleted\n");
	}
	sprintf(ErrMess,"No ERROR in GetConnectivityBetweenDevices");
	return 0;
}
/****************************************
*followPath_device is used in GetConnectivityBetweenDevices
*****************************************/
int DBContainer::followPath_device(char* dev_name, eConnectivityCache *ecache, iConnectivityCache *icache, int elnkidx, HopTyp *hop, int *i,int dep, int& len_array, int &pfrom1_arraylen,int &nfrom1_arraylen,int &pto1_arraylen,int &nto1_arraylen,int &lkinfo_arraylen,char* nfrom1_list,char* nto1_list,char* pfrom1_list,int* pfrom_list,char* pto1_list,int* pto_list,int* pathid_list,int* link_pos_list,char* lkinfo_list)
{
		HopTyp ehops[1000];	
		HopTyp ihops[1000];
		HopTyp hopar[1000];
		char prefix[100];
		int j,k, inres, enres_from, enres_to, len;
		static int pos1, pos2, pos3, pos4, pos5, cnt_lk;	//link counter
		int	x;
		char* device;
		static int eidx[100];	
		static int iidx[100];
		
		if(*i==0){
			pos1=0;pos2=0;pos3=0;pos4=0;pos5=0;cnt_lk=0;
		}
		len=strlen(dev_name);
						
		eidx[dep-1]=elnkidx;
		device=&ecache->devnamto.bufferp[elnkidx*ecache->devnamto.colwid];	//name of the device to which the external path elnkidx is going to

		if (strncmp(device, dev_name, len) == 0){			//if external path elnkidx is going to dev_name connection											
			printf("\n%i. connection:\n",*i+1);
			prefix[0]	= 32;prefix[1]	= 0;

			for(x=0;x<dep;x++){
				
				prefix[x]=32;					//printing connection
				prefix[x+1]=0;
				printf("%s", prefix);
				ecache->print_row(eidx[x]);
				if(x<dep-1){					//last internal connection isn't printed
					printf("%s", prefix);
					icache->print_row(iidx[x]);
				}										

				pathid_list[x+cnt_lk]=*i;		//no. of path
				pfrom_list[x+cnt_lk]=ecache->portidfrom.bufferp[eidx[x]];
				pto_list[x+cnt_lk]=ecache->portidto.bufferp[eidx[x]];
				link_pos_list[x+cnt_lk]=x+1;	//position of the link in the path (start with 1)
					
				
				strcpy(nfrom1_list+pos1,&ecache->devnamfrom.bufferp[eidx[x]*ecache->devnamfrom.colwid]);
				pos1+=strlen(&ecache->devnamfrom.bufferp[eidx[x]*ecache->devnamfrom.colwid]);
				nfrom1_list[pos1]='\0';
				pos1+=1;
				
				strcpy(nto1_list+pos2,&ecache->devnamto.bufferp[eidx[x]*ecache->devnamto.colwid]);
		        pos2+=strlen(&ecache->devnamto.bufferp[eidx[x]*ecache->devnamto.colwid]);  		
				nto1_list[pos2]='\0';
				pos2+=1;
				
				strcpy(pfrom1_list+pos3,&ecache->pnumfrom.bufferp[eidx[x]*ecache->pnumfrom.colwid]);
				pos3+=strlen(&ecache->pnumfrom.bufferp[eidx[x]*ecache->pnumfrom.colwid]);
				pfrom1_list[pos3]='\0';
				pos3+=1;

				strcpy(pto1_list+pos4,&ecache->pnumto.bufferp[eidx[x]*ecache->pnumto.colwid]);
				pos4+=strlen(&ecache->pnumto.bufferp[eidx[x]*ecache->pnumto.colwid]);
				pto1_list[pos4]='\0';
				pos4+=1;

				strcpy(lkinfo_list+pos5,&ecache->linkinfo.bufferp[eidx[x]*ecache->linkinfo.colwid]);
				pos5+=strlen(&ecache->linkinfo.bufferp[eidx[x]*ecache->linkinfo.colwid]);
				lkinfo_list[pos5]='\0';
				pos5+=1;
			}
			*i=*i+1;	
			cnt_lk+=dep;

			len_array=cnt_lk;	
			pfrom1_arraylen=pos3;
			nfrom1_arraylen=pos1;
			pto1_arraylen=pos4;
			nto1_arraylen=pos2;
			lkinfo_arraylen=pos5;
		}

    inres  = icache->FindinCol(&icache->portidfrom,ecache->portidto.bufferp[elnkidx], 1, ihops);	//searching for internal paths
	if (inres < 0) {
		return -1;}
	dep++;
	if (inres > 0) {		//if any internal path found
		for (k=0;k<inres;k++)
		{
			enres_from = ecache->FindinCol(&ecache->portidfrom,icache->portidto.bufferp[ihops[k].h], 2,ehops); //searching for external paths going from device
			if (enres_from < 0){
				return -1;
			}
			if (enres_from == 0){
				printf("\nno external paths from device\n");
				return -1;
			}
			if (enres_from > 1)	//if there's more than one external path going from the same port
			{
				printf("\nmultiple external connections from one source (FANOUT) not allowed\n");
				return -1;
			}
			for (j  = 0;j<enres_from;j++)
			{	
				iidx[dep-2]=ihops[k].h;	//(dep-2 because dep was already increased once)
				followPath_device(dev_name, ecache,icache,ehops[j].h,hopar, i, dep, len_array,pfrom1_arraylen,nfrom1_arraylen,pto1_arraylen,nto1_arraylen,lkinfo_arraylen, nfrom1_list,nto1_list, pfrom1_list,pfrom_list,pto1_list,pto_list, pathid_list,link_pos_list,lkinfo_list);
			}
		}	
	}

	if(inres==0){	//if no internal path found
		enres_to = ecache->FindinCol(&ecache->devnamto,device, 2, ehops);	//searching for external paths going to device
		enres_from = ecache->FindinCol(&ecache->devnamfrom,device, 2, ehops);	//searching for external paths going from device
	
		if(enres_from==1||enres_to==1){
			iidx[dep-2]=-2;	
		}
		else iidx[dep-2]=-1;
		
		for (j=0;j<enres_from;j++){		//for every external path going from device
			followPath_device(dev_name,ecache,icache,ehops[j].h,hopar,i,dep, len_array,pfrom1_arraylen,nfrom1_arraylen,pto1_arraylen,nto1_arraylen,lkinfo_arraylen, nfrom1_list,nto1_list, pfrom1_list,pfrom_list,pto1_list,pto_list, pathid_list,link_pos_list,lkinfo_list);
		}
	}
	return *i;
}
/////////////
int DBContainer::GetDetailedConnBetweenDeviceDevType(char* dev_from, char* dev_to,int devto_type_given,eConnectivityCache *ecache, iConnectivityCache *icache, char* nfrom_list,char* pfrom_list,char* pto_list,int& len_array,int & nfrom_arraylen,int & pfrom_arraylen, int & nto_arraylen, int & pto_arraylen,int &lkinfo_arraylen, int* pfrom1_list,int* pto1_list,char* nto_list,int* pathid_list, int* link_pos_list,char* lkinfo_list,int reload_connectivity,int delete_connectivity,char* ErrMess){

	HopTyp hop[1000];
	int i = 0,k, enres;
	char* type;
	int system_name=1;

	if (reload_connectivity==1)	{
		ecache->Fill(system_name);	
		icache->Fill(system_name);	
	}
	if(ecache->fetched_row==0&&icache->fetched_row==0){
		sprintf(ErrMess,"ERROR in GetDetailedConnBetweenDeviceDevType : CONNECTIVITY CACHE EMPTY");
		printf("\nconnectivity cache empty\n");
		return -1;
	}
	
	enres  = ecache->FindinCol(&ecache->devnamfrom,dev_from, 2, hop); //searching for external paths going from 'dev_name'
	
	if(hop[0].typ==0){	//when there's no external link from device
		sprintf(type,"not found");
	}else
	{
	type=&ecache->devtypefrom.bufferp[hop[0].h*ecache->devtypefrom.colwid];	//type of 'dev_name'
	}
	
	printf("\ndev_name: %s (device type : %s) ", dev_from, type);	
	printf("\ndev_type: %s \n", dev_to);
		
	for(k=0;k<enres;k++){		//for every external path going from device
	i=followPath_devType(dev_to, ecache,icache,hop[k].h,hop,&i,1,nfrom_list,pfrom_list,pto_list,len_array,nfrom_arraylen,pfrom_arraylen,nto_arraylen,pto_arraylen,lkinfo_arraylen,pfrom1_list,pto1_list,nto_list,pathid_list,link_pos_list,lkinfo_list);
	}
	if(i>0){
		if(delete_connectivity==1){

			ecache->deleteConnectivity();
			icache->deleteConnectivity();
			printf("\nconnectivity deleted\n");
		}

		sprintf(ErrMess,"No ERROR in GetDetailedConnBetweenDeviceDevType");
		return 0;
	}
	if(i<0){
		if(delete_connectivity==1){
			ecache->deleteConnectivity();
			icache->deleteConnectivity();
			printf("\nconnectivity deleted\n");
		}

		sprintf(ErrMess,"ERROR in GetDetailedConnBetweenDeviceDevType");
		return -1;
	}
	if(i==0) {				//if no connection found searching in reverse direction (from any device type of 'dev_type' to 'dev_name' device)
		//printf("\nsearching in reverse direction ...\n");
		
		enres  = ecache->FindinCol(&ecache->devnamto,dev_from, 2, hop);//searching external connections going to 'dev_from'
		
		for(k=0;k<enres;k++){
		i=followPath_devType_reverse(dev_to, ecache,icache,hop[k].h,hop,&i,1,nfrom_list,pfrom_list,pto_list,len_array,nfrom_arraylen,pfrom_arraylen,nto_arraylen,pto_arraylen,lkinfo_arraylen,pfrom1_list,pto1_list,nto_list,pathid_list,link_pos_list,lkinfo_list);
		}
		if(i>0){
			if(delete_connectivity==1){
				ecache->deleteConnectivity();
				icache->deleteConnectivity();
				printf("\nconnectivity deleted\n");
			}

			sprintf(ErrMess,"No ERROR in GetDetailedConnBetweenDeviceDevType");
			return 0;
		}
		if(i<=0){
			if(delete_connectivity==1){
				ecache->deleteConnectivity();
				icache->deleteConnectivity();
				printf("\nconnectivity deleted\n");
			}

			sprintf(ErrMess,"ERROR in GetDetailedConnBetweenDeviceDevType : NO CONNECTIVITY FOUND");
			return -1;
		}
	}

	if(delete_connectivity==1){	
		ecache->deleteConnectivity();
		icache->deleteConnectivity();
		printf("\nconnectivity deleted\n");
	}

	sprintf(ErrMess,"No ERROR in GetDetailedConnBetweenDeviceDevType");
	return 0;
}
/******************
*followPath_devType used in GetDetailedConnBetweenDeviceDevType
*******************/
int DBContainer::followPath_devType(char* dType_to, eConnectivityCache *ecache, iConnectivityCache *icache, int elnkidx, HopTyp *hop, int *i, int dep,char* nfrom_list,char* pfrom_list,char* pto_list,int& len_array,int & nfrom_arraylen,int & pfrom_arraylen, int & nto_arraylen, int & pto_arraylen,int &lkinfo_arraylen, int* pfrom1_list,int* pto1_list,char* nto_list,int* pathid_list, int* link_pos_list,char* lkinfo_list){

		HopTyp ehops[1000];	
		HopTyp ihops[1000];
		HopTyp hopar[1000];
		char prefix[100];
		int j,k,m, inres, enres_from, enres_to, len;
		static int pos1, pos2, pos3, pos4, pos5, cnt_lk;	//link counter
		char* type;
		char* device;
		static int eidx[100];
		static int iidx[100];
		len=strlen(dType_to);

		if(*i==0){
			pos1=0;pos2=0;pos3=0;pos4=0;pos5=0;cnt_lk=0;
		}

		eidx[dep-1]=elnkidx;
		type=&ecache->devtypeto.bufferp[elnkidx*ecache->devtypeto.colwid];	//type of device to which the external path elnkidx is going to
		device=&ecache->devnamto.bufferp[elnkidx*ecache->devnamto.colwid];	//device to which the external path elnkidx is going to
				
		if (strncmp(type, dType_to, len) == 0){		//if connection found
			
//			*i=*i+1;								
			printf("\n%i. connection:\n",*i+1);
			prefix[0]	= 32;prefix[1]	= 0;

			for (m=0;m<dep;m++){

				prefix[m]=32;	
				prefix[m+1]=0;
				printf("%s", prefix); 
				ecache->print_row_devType(eidx[m]);		//printing row with information about type of device
				if(m<dep-1){
					printf("%s", prefix);
					icache->print_row(iidx[m]);
				}
				
				if(len_array!=0){				//len_array=0 when followPath_devType used in GetConnOutBetweenDeviceDeviceType
				pathid_list[m+cnt_lk]=*i;		//no. of path
				pfrom1_list[m+cnt_lk]=ecache->portidfrom.bufferp[eidx[m]];
				pto1_list[m+cnt_lk]=ecache->portidto.bufferp[eidx[m]];
				link_pos_list[m+cnt_lk]=m+1;	//position of the link in the path (start with 1)
				
				
				strcpy(nfrom_list+pos1,&ecache->devnamfrom.bufferp[eidx[m]*ecache->devnamfrom.colwid]);
				pos1+=strlen(&ecache->devnamfrom.bufferp[eidx[m]*ecache->devnamfrom.colwid]);
				nfrom_list[pos1]='\0';
				pos1+=1;
				
				strcpy(nto_list+pos2,&ecache->devnamto.bufferp[eidx[m]*ecache->devnamto.colwid]);
		        pos2+=strlen(&ecache->devnamto.bufferp[eidx[m]*ecache->devnamto.colwid]);  		
				nto_list[pos2]='\0';
				pos2+=1;
				
				strcpy(pto_list+pos4,&ecache->pnumto.bufferp[eidx[m]*ecache->pnumto.colwid]);
				pos4+=strlen(&ecache->pnumto.bufferp[eidx[m]*ecache->pnumto.colwid]);
				pto_list[pos4]='\0';
				pos4+=1;

				strcpy(lkinfo_list+pos5,&ecache->linkinfo.bufferp[eidx[m]*ecache->linkinfo.colwid]);
				pos5+=strlen(&ecache->linkinfo.bufferp[eidx[m]*ecache->linkinfo.colwid]);
				lkinfo_list[pos5]='\0';
				pos5+=1;
				}

				strcpy(pfrom_list+pos3,&ecache->pnumfrom.bufferp[eidx[m]*ecache->pnumfrom.colwid]);
				pos3+=strlen(&ecache->pnumfrom.bufferp[eidx[m]*ecache->pnumfrom.colwid]);
				pfrom_list[pos3]='\0';
				pos3+=1;

			}
			*i=*i+1;	
			cnt_lk+=dep;

			len_array=cnt_lk;	
			pfrom_arraylen=pos3;
			nfrom_arraylen=pos1;
			pto_arraylen=pos4;
			nto_arraylen=pos2;
			lkinfo_arraylen=pos5;
		}

	inres  = icache->FindinCol(&icache->portidfrom,ecache->portidto.bufferp[elnkidx], 1, ihops);	//searching for internal paths

	if (inres < 0) {
		return -1;}//error
	dep++;
	if (inres >0){					//if any internal path found
		for (k=0;k<inres;k++)		//for every internal path found
		{
			enres_from = ecache->FindinCol(&ecache->portidfrom,icache->portidto.bufferp[ihops[k].h], 2,ehops); //searching for external paths going from the port terminating internal link
			if (enres_from == 0){
				printf("\nno external paths from device\n");
				return -1;
			}
			if (enres_from < 0){
				return -1;
			}
			if (enres_from > 1){			//if more than one external path going from the same port
				printf("\nmultiple external connections from one source (FANOUT) not allowed\n");
				return -1;
			}
			for (j  = 0;j<enres_from;j++)		//for every external path found
			{	
				iidx[dep-2]=ihops[k].h;		//(dep-2 because dep was already increased once)
				followPath_devType(dType_to, ecache,icache,ehops[j].h,hopar, i, dep,nfrom_list,pfrom_list,pto_list,len_array,nfrom_arraylen,pfrom_arraylen,nto_arraylen,pto_arraylen,lkinfo_arraylen,pfrom1_list,pto1_list,nto_list,pathid_list,link_pos_list,lkinfo_list);
			}
		}
	}
	
	if(inres==0){				//if no internal path found
		enres_to = ecache->FindinCol(&ecache->devnamto,device, 2, ehops);	//searching for external paths going to 'device'
		enres_from = ecache->FindinCol(&ecache->devnamfrom,device, 2, ehops);	//searching for external paths going from 'device'

		if(enres_from==1||enres_to==1){
			iidx[dep-2]=-2;	
		}
		else iidx[dep-2]=-1;

		for (j=0;j<enres_from;j++){		//for every external path found
			followPath_devType(dType_to,ecache,icache,ehops[j].h,hopar,i,dep,nfrom_list,pfrom_list,pto_list,len_array,nfrom_arraylen,pfrom_arraylen,nto_arraylen,pto_arraylen,lkinfo_arraylen,pfrom1_list,pto1_list,nto_list,pathid_list,link_pos_list,lkinfo_list);
		}
	}
    return *i;	
}
/***********************
*followPath_devType_reverse used in GetDetailedConnBetweenDeviceDevType
************************/
int DBContainer::followPath_devType_reverse(char* dType_from, eConnectivityCache *ecache, iConnectivityCache *icache, int elnkidx, HopTyp *hop, int *i, int dep,char* nfrom_list,char* pfrom_list,char* pto_list,int& len_array,int & nfrom_arraylen,int & pfrom_arraylen, int & nto_arraylen, int & pto_arraylen,int &lkinfo_arraylen, int* pfrom1_list,int* pto1_list,char* nto_list,int* pathid_list, int* link_pos_list,char* lkinfo_list){

		HopTyp ehops[1000];	
		HopTyp ihops[1000];
		HopTyp hopar[1000];
		char prefix[100];
		int j,k,m, inres, enres_to, enres_from, len;
		static int pos1, pos2, pos3, pos4, pos5, cnt_lk;	//link counter
		char* device;
		char* type;
		static int eidx[100];	
		static int iidx[100];
		static int eidx_rev[100];	
		static int iidx_rev[100];	
		len=strlen(dType_from);

		if(*i==0){
			pos1=0;pos2=0;pos3=0;pos4=0;pos5=0;cnt_lk=0;
		}
				
		eidx[dep-1]=elnkidx;
		device=&ecache->devnamfrom.bufferp[elnkidx*ecache->devnamfrom.colwid];	//name of device originating path elnkidx
		type=&ecache->devtypefrom.bufferp[elnkidx*ecache->devtypefrom.colwid];	//type of device originating path elnkidx

		if (strncmp(type, dType_from, len) == 0){					//if connection found
			printf("\n%i. connection:\n",*i+1);

			for (m=0;m<dep;m++){	//printing connection
				prefix[m]=32;	
				prefix[m+1]=0;
				if(m>0){
					printf("%s", prefix);
					icache->print_row(iidx[dep-1-m]);	//dep=4: 3,2,1,0
				}
				printf("%s ", prefix);
				ecache->print_row_devType(eidx[dep-1-m]);		//printing row with information about type of device
			
				if(len_array!=0){				//len_array=0 when followPath_devType_reverse used in GetConnOutBetweenDeviceDeviceType
				pathid_list[m+cnt_lk]=*i;		//no. of path
				pfrom1_list[m+cnt_lk]=ecache->portidfrom.bufferp[eidx[dep-1-m]];
				pto1_list[m+cnt_lk]=ecache->portidto.bufferp[eidx[dep-1-m]];
				link_pos_list[m+cnt_lk]=m+1;	//position of the link in the path (start with 1)
				
				strcpy(nfrom_list+pos1,&ecache->devnamfrom.bufferp[eidx[dep-1-m]*ecache->devnamfrom.colwid]);
				pos1+=strlen(&ecache->devnamfrom.bufferp[eidx[dep-1-m]*ecache->devnamfrom.colwid]);
				nfrom_list[pos1]='\0';
				pos1+=1;
				
				strcpy(nto_list+pos2,&ecache->devnamto.bufferp[eidx[dep-1-m]*ecache->devnamto.colwid]);
		        pos2+=strlen(&ecache->devnamto.bufferp[eidx[dep-1-m]*ecache->devnamto.colwid]);  		
				nto_list[pos2]='\0';
				pos2+=1;
				
				strcpy(pto_list+pos4,&ecache->pnumto.bufferp[eidx[dep-1-m]*ecache->pnumto.colwid]);
				pos4+=strlen(&ecache->pnumto.bufferp[eidx[dep-1-m]*ecache->pnumto.colwid]);
				pto_list[pos4]='\0';
				pos4+=1;

				strcpy(lkinfo_list+pos5,&ecache->linkinfo.bufferp[eidx[dep-1-m]*ecache->linkinfo.colwid]);
				pos5+=strlen(&ecache->linkinfo.bufferp[eidx[dep-1-m]*ecache->linkinfo.colwid]);
				lkinfo_list[pos5]='\0';
				pos5+=1;
				}

				strcpy(pfrom_list+pos3,&ecache->pnumfrom.bufferp[eidx[dep-1-m]*ecache->pnumfrom.colwid]);
				pos3+=strlen(&ecache->pnumfrom.bufferp[eidx[dep-1-m]*ecache->pnumfrom.colwid]);
				pfrom_list[pos3]='\0';
				pos3+=1;

			}
			*i=*i+1;	
			cnt_lk+=dep;

			len_array=cnt_lk;	
			pfrom_arraylen=pos3;
			nfrom_arraylen=pos1;
			pto_arraylen=pos4;
			nto_arraylen=pos2;
			lkinfo_arraylen=pos5;
		}

    inres  = icache->FindinCol(&icache->portidto,ecache->portidfrom.bufferp[elnkidx], 1, ihops);	//searching for internal paths
	if (inres < 0) {
		return -1;}//error
	dep++;
	if (inres > 0) {				//if any internal path found
		for (k=0;k<inres;k++)		//for every internal link
		{
			enres_to = ecache->FindinCol(&ecache->portidto,icache->portidfrom.bufferp[ihops[k].h], 2,ehops);	//searching for external paths going to the port originating the internal link
			if (enres_to < 0){
				return -1;
			}
			if (enres_to == 0){
				printf("\nno external paths to device\n");
				return -1;
			}
			if (enres_to > 1){		//if more than one external path going to the same port
				printf("multiple external connections to one source\n");
				return -1;
			}	
			for (j  = 0;j<enres_to;j++)
			{	
				iidx[dep-2]=ihops[k].h;		//(dep-2 because dep was already increased once)
				followPath_devType_reverse(dType_from, ecache,icache,ehops[j].h,hopar, i, dep,nfrom_list,pfrom_list,pto_list,len_array,nfrom_arraylen,pfrom_arraylen,nto_arraylen,pto_arraylen,lkinfo_arraylen,pfrom1_list,pto1_list,nto_list,pathid_list,link_pos_list,lkinfo_list);
			}
		}	
	}

	if(inres==0){		//if no internal path
		enres_from = ecache->FindinCol(&ecache->devnamfrom,device, 2, ehops);	//searching for external paths going from 'device'
		enres_to = ecache->FindinCol(&ecache->devnamto,device, 2, ehops);	//searching for external paths going to 'device'
		
		if(enres_from==1||enres_to==1){		//if only one output or only one input in the device
			iidx[dep-2]=-2;	
		}
		else iidx[dep-2]=-1;
		
		for (j=0;j<enres_to;j++){			//for every external path going to device
			followPath_devType_reverse(dType_from,ecache,icache,ehops[j].h,hopar,i,dep,nfrom_list,pfrom_list,pto_list,len_array,nfrom_arraylen,pfrom_arraylen,nto_arraylen,pto_arraylen,lkinfo_arraylen,pfrom1_list,pto1_list,nto_list,pathid_list,link_pos_list,lkinfo_list);
		}
	}	
		    return *i;
}
//////////////////////////////
int DBContainer::GetAllPathsPerDevice(char* systemnameList,char* dev_name, eConnectivityCache *ecache, iConnectivityCache *icache, int& len_array, int* lkid_list5,int* pathid_list,int* link_pos_list,int reload_connectivity,int delete_connectivity,char* ErrMessage){

	HopTyp ehop_to[1000];
	HopTyp ehop_from[1000];
	HopTyp ihop[1000];
	int i,j,k,inres, enres_to, enres_from;
	int path_cnt=0;			//no. of path
	
	if(reload_connectivity==1&&strncmp(systemnameList,"ALL",3)==0){
		ecache->Fill(1);
		icache->Fill(1);
	}
	else if(reload_connectivity==1){
		ecache->Fill_list(systemnameList);
		icache->Fill_list(systemnameList);
	}

	if(ecache->fetched_row==0&&icache->fetched_row==0){
		sprintf(ErrMessage,"ERROR in GetAllPathsPerDevice : CONNECTIVITY CACHE EMPTY");
		printf("\nconnectivity cache empty\n");
		return -1;
	}

	enres_to  = ecache->FindinCol(&ecache->devnamto,dev_name, 2, ehop_to);		//links going to 'dev_name'
	inres  = icache->FindinCol(&icache->devnamto,dev_name, 1, ihop);		//internal connections in 'dev_name'	
	enres_from  = ecache->FindinCol(&ecache->devnamfrom,dev_name, 2, ehop_from);	//links going from 'dev_name' to any device 
	
	if(enres_to==0 && inres==0 && enres_from==0){					//if no external and no internal connectivity
		if(delete_connectivity==1){	
			ecache->deleteConnectivity();
			icache->deleteConnectivity();
			printf("\nconnectivity deleted\n");
		}
		printf("\nNo device like %s found\n",dev_name);
		sprintf(ErrMessage,"ERROR in GetAllPathsPerDevice : DEVICE NOT CONNECTED");
		return -1;
	}

	if(inres>0){				//if there is any internal connection
		for(j=0;j<inres;j++){
			printf("\n******************************\n");

			enres_to  = ecache->FindinCol(&ecache->portidto,icache->portidfrom.bufferp[ihop[j].h], 2, ehop_to);	//finding external links going to the port originating the j-th internal connection
			if(enres_to > 1){				////
				printf("multiple external connections to one source not allowed\n");
				sprintf(ErrMessage,"ERROR in GetAllPathsPerDevice : MULTIPLE EXTERNAL CONNECTIONS TO ONE SOURCE");
				if(delete_connectivity==1){	
					ecache->deleteConnectivity();
					icache->deleteConnectivity();
					printf("\nconnectivity deleted\n");
				}
				return -1;
			}
			if (enres_to == 0){				////
				printf("\nno external paths going to device\n");
				sprintf(ErrMessage,"ERROR in GetAllPathsPerDevice : NO EXTERNAL PATHS GOING TO DEVICE");
				if(delete_connectivity==1){	
					ecache->deleteConnectivity();
					icache->deleteConnectivity();
					printf("\nconnectivity deleted\n");
				}
				return -1;
			}
			if(enres_to > 0){	
				for(i=0;i<enres_to;i++){	//for every external link found
					followPath_reverse(ecache,icache,ehop_to[i].h,ehop_to,&path_cnt,1,len_array,lkid_list5,pathid_list,link_pos_list);
				}
			}
			printf("\n-----------------------------------------\n   ");
			icache->print_row(ihop[j].h);
			printf("-----------------------------------------\n");
	
			enres_from = ecache->FindinCol(&ecache->portidfrom,icache->portidto.bufferp[ihop[j].h], 2, ehop_from);	//finding external links going from the port terminating the j-th internal connection
			if (enres_from > 1)		//if more than one external path from the same port found
			{
				printf("multiple external connections from one source (FANOUT) not allowed\n");
				sprintf(ErrMessage,"ERROR in GetAllPathsPerDevice : MULTIPLE EXTERNAL CONNECTIONS FROM ONE SOURCE");
				if(delete_connectivity==1){	
					ecache->deleteConnectivity();
					icache->deleteConnectivity();
					printf("\nconnectivity deleted\n");
				}
				return -1;
			}
			if (enres_from == 0){			////
				printf("\nno external paths going from device\n");
				sprintf(ErrMessage,"ERROR in GetAllPathsPerDevice : NO EXTERNAL PATHS GOING FROM DEVICE");
				if(delete_connectivity==1){	
					ecache->deleteConnectivity();
					icache->deleteConnectivity();
					printf("\nconnectivity deleted\n");
				}
				return -1;
			}
			if(enres_from > 0){	
				for(k=0;k<enres_from;k++){	//for every external link found
					followPath_normal(ecache,icache,ehop_from[k].h,ehop_from,&path_cnt,1,len_array,lkid_list5,pathid_list,link_pos_list);
				}
			}
		}
	}

	if(inres==0){				//if there is no internal connection in device
		if (enres_to==0){
			printf("\nno external paths to %s\n", dev_name);
		}else {
			printf("\n");	
			for(i=0;i<enres_to;i++){	//for every external path going to device
				followPath_reverse(ecache,icache,ehop_to[i].h,ehop_to,&path_cnt,1,len_array,lkid_list5,pathid_list,link_pos_list);
			}	
		}

		printf("\n-----------------------------------------\n");
		if (enres_to==1&&enres_from!=1){	//if there's only one external link to device
			printf("no internal connectivity in %s - only one input to device",dev_name);
			printf("\n-----------------------------------------\n");
		}
		else if (enres_from==1&&enres_to!=1){		//if only one external link from device
			printf("no internal connectivity in %s, only one output from device",dev_name);
			printf("\n-----------------------------------------\n");
		}
		else if(enres_from==1&&enres_to==1){		//if only one path from and only one path to device
			printf("no internal connectivity in %s - only one input and one output in device",dev_name);
			printf("\n-----------------------------------------\n");
		}
		else if (enres_from > 1 || enres_to >1){
			printf("no internal connectivity in %s", dev_name);
			printf("\n-----------------------------------------\n");
		}

		if (enres_from==0){				//if no external path from device
			printf("\nno external paths from %s\n", dev_name);
		}
		else {
			for(k=0;k<enres_from;k++){				//for every external path going from device				
				followPath_normal(ecache,icache,ehop_from[k].h,ehop_from,&path_cnt,1,len_array,lkid_list5,pathid_list,link_pos_list);
			}
		}
	}
	sprintf(ErrMessage,"NO ERROR in GetAllPathsPerDevice");
	if(delete_connectivity==1){	
		ecache->deleteConnectivity();
		icache->deleteConnectivity();
		printf("\nconnectivity deleted\n");
	}
	return 0;
}
//followPath_reverse is used in GetAllPathsPerDevice
int DBContainer::followPath_reverse(eConnectivityCache *ecache, iConnectivityCache *icache, int elnkidx, HopTyp *hop, int *i, int dep,int& len_array, int* lkid_list5,int* pathid_list,int* link_pos_list)
{
		HopTyp ehops[1000];
		HopTyp ihops[1000];
		HopTyp hopar[1000];
		char prefix[100];
		char* device;
	    int j,k,m, inres, enres_out,enres_in;
		static int eidx[100];	
		static int iidx[100];
						
		if(*i==0){
			len_array=0;
		}

		if(hop[0].typ==0&&dep==1){	//when there's no external link to device
			eidx[dep-1]=-1;			//to print comment
		}else
		eidx[dep-1]=elnkidx;

		inres  = icache->FindinCol(&icache->portidto,ecache->portidfrom.bufferp[elnkidx], 1, ihops);
		if (inres == 0) {
			device=&ecache->devnamfrom.bufferp[elnkidx*ecache->devnamfrom.colwid]; //name of device with no internal connections
			enres_out= ecache->FindinCol(&ecache->devnamfrom,device, 2,ehops);	   //no. of external paths going from device
			enres_in= ecache->FindinCol(&ecache->devnamto,device, 2,ehops);		   //no. of external paths going to device
			
			if((enres_out==1||enres_in==1)&&enres_in!=0){	//case when there is only one possible internal path 
				iidx[dep-1]=-2;					//to print comment
				dep++;
								
				for(m=0; m<enres_in; m++){	//for every external path going to device
					followPath_reverse(ecache,icache,ehops[m].h,hopar,i, dep,len_array,lkid_list5,pathid_list,link_pos_list);
				}
			}
			/*
			else if(enres_in!=0){		//path ends where there's no external links going to device
				iidx[dep-1]=-1;
				dep++;
				for(m=0; m<enres_in; m++){
					followPath_reverse(ecache,icache,ehops[m].h,hopar,i, dep,len_array,lkid_list5,pathid_list,link_pos_list);
				}
			}
			*/
			else{			//when there's end of the path
				printf("\n%i. path:\n",*i);
				for (j=0;j<dep;j++){	//printing connection
					prefix[j]=32;	
					prefix[j+1]=0;
					if(j>0){
						printf("%s", prefix);
						icache->print_row(iidx[dep-1-j]);
					}
					printf("%s ", prefix);
					ecache->print_row(eidx[dep-1-j]);

					lkid_list5[j+len_array]=ecache->linkid.bufferp[eidx[dep-1-j]];
					pathid_list[j+len_array]=*i;
					link_pos_list[j+len_array]=j+1;
				}
				len_array+=dep;
				*i+=1;		//path counter
			}			
			return 0;
		}
		dep++;

		for (k=0;k<inres;k++)	//for every internal path
		{
			if (inres > 1) printf("\n");
			enres_in = ecache->FindinCol(&ecache->portidto,icache->portidfrom.bufferp[ihops[k].h], 2,ehops);	//searching external paths
			if (enres_in <= 0)	//if no external path going to the port originating the internal path
			{
				return -1;
			}	
			if (enres_in > 1)	//more than one external path going to the same port
			{
				printf("multiple external connections to one source\n");
				return -1;
			}
			for (j  = 0;j<enres_in;j++)		//for every external path found
			{
				iidx[dep-2]=ihops[k].h;			//(dep-2 because dep was already increased once)
				followPath_reverse(ecache,icache,ehops[j].h,hopar,i, dep,len_array,lkid_list5,pathid_list,link_pos_list);
			}
		}
	
    return 0;
}
//followPath_normal is used in GetAllPathsPerDevice
int DBContainer::followPath_normal(eConnectivityCache *ecache, iConnectivityCache *icache, int elnkidx, HopTyp *hop, int *i, int dep,int& len_array, int* lkid_list5,int* pathid_list,int* link_pos_list)
{
		HopTyp ehops[1000];
		HopTyp ihops[1000];
		HopTyp hopar[1000];
		char prefix[100];
		char* device;
	    int j,k,m, inres, enres_out,enres_in;
		static int eidx[100];	
		static int iidx[100];
			
		if(hop[0].typ==0&&dep==1){	//when there's no external path from device
			eidx[dep-1]=-2;			//to print comment
		}else
		eidx[dep-1]=elnkidx;

		inres  = icache->FindinCol(&icache->portidfrom,ecache->portidto.bufferp[elnkidx], 1, ihops);
		if (inres == 0) {
			device=&ecache->devnamto.bufferp[elnkidx*ecache->devnamfrom.colwid]; //name of device with no internal connections
			enres_in= ecache->FindinCol(&ecache->devnamto,device, 2,ehops);	     //no. of external paths going to device
			enres_out= ecache->FindinCol(&ecache->devnamfrom,device, 2,ehops);	 //no. of external paths going from device
									
			if((enres_out==1||enres_in==1)&&enres_out!=0){	//case when there is only one possible internal path 
				iidx[dep-1]=-2;					//to print comment
				dep++;
				
				for(m=0; m<enres_out; m++){		//for every external path going from device
					followPath_normal(ecache,icache,ehops[m].h,hopar,i, dep,len_array,lkid_list5,pathid_list,link_pos_list);
				}
			}
			/*
			else if(enres_out!=0){		//path ends where there's no external links going from device
				iidx[dep-1]=-1;
				dep++;
				for(m=0; m<enres_out; m++){
					followPath_normal(ecache,icache,ehops[m].h,hopar,i, dep,len_array,lkid_list5,pathid_list,link_pos_list);
				}
			}
			*/
			else{	//when there's end of the path
				printf("%i. path:\n",*i);
				for (j=0;j<dep;j++){	//printing connection
				prefix[j]=32;
				prefix[j+1]=0;
				printf("%s   ", prefix);
				ecache->print_row(eidx[j]);
				if(j<dep-1){
					printf("%s    ", prefix);
					icache->print_row(iidx[j]);
				}

				lkid_list5[j+len_array]=ecache->linkid.bufferp[eidx[j]];
				pathid_list[j+len_array]=*i;
				link_pos_list[j+len_array]=j+1;
				}

				*i+=1;	//path counter
				len_array+=dep;			
			}
			return 0;
		}
        dep++;

		for (k=0;k<inres;k++)	//for every internal path
		{
			if (inres > 1) printf("\n");
			enres_out  = ecache->FindinCol(&ecache->portidfrom,icache->portidto.bufferp[ihops[k].h], 2,ehops);	//searching external paths
			if (enres_out <= 0)		//if no external path going from the port terminating the internal path
			{
				return -1;
			}	
			if (enres_out > 1)	//more than one external path going from the same port
			{
				printf("multiple external connections from one source (FANOUT) not allowed\n");
				return -1;
			}
			for (j  = 0;j<enres_out;j++)	//for every external path found
			{
				iidx[dep-2]=ihops[k].h;		//(dep-2 because dep was already increased once)
				followPath_normal(ecache,icache,ehops[j].h,hopar,i, dep,len_array,lkid_list5,pathid_list,link_pos_list);
			}
		}
    return 0;
}
//OCIReportError; GetErrorMess
void eConnectivityCache::deleteConnectivity(){

		free(linkid.bufferp);
		linkid.bufferp=0;
		linkid.colwid=0;
		free(devfrom.bufferp);		
		devfrom.bufferp=0;		
		devfrom.colwid=0;
		free(devto.bufferp);
		devto.bufferp=0;
		devto.colwid=0;
		free(pnumfrom.bufferp);
		pnumfrom.bufferp=0;
		pnumfrom.colwid=0;
		free(pnumto.bufferp);
		pnumto.bufferp=0;
		pnumto.colwid=0;
		free(linktyp.bufferp);
		linktyp.bufferp=0;
		linktyp.colwid=0;
		free(bidir.bufferp);
		bidir.bufferp=0;
		bidir.colwid=0;
		free(linkused.bufferp);
		linkused.bufferp=0;
		linkused.colwid=0;
		free(linkweight.bufferp);
		linkweight.bufferp=0;
		linkweight.colwid=0;
		free(devnamfrom.bufferp);
		devnamfrom.bufferp=0;
		devnamfrom.colwid=0;
		free(devnamto.bufferp);
		devnamto.bufferp=0;
		devnamto.colwid=0;
		free(linkinfo.bufferp);
		linkinfo.bufferp=0;
		linkinfo.colwid=0;
		free(sysnam.bufferp);
		sysnam.bufferp=0;
		sysnam.colwid=0;
		free(portidfrom.bufferp);
		portidfrom.bufferp=0;
		portidfrom.colwid=0;
		free(portidto.bufferp);
		portidto.bufferp=0;
		portidto.colwid=0;
		free(devtypefrom_id.bufferp);
		devtypefrom_id.bufferp=0;
		devtypefrom_id.colwid=0;
		free(devtypefrom.bufferp);
		devtypefrom.bufferp=0;
		devtypefrom.colwid=0;
		free(devtypeto_id.bufferp);
		devtypeto_id.bufferp=0;
		devtypeto_id.colwid=0;
		free(devtypeto.bufferp);
		devtypeto.bufferp=0;
		devtypeto.colwid=0;
		free(linkinfo.bufferp);
		linkinfo.bufferp=0;
		linkinfo.colwid=0;
		free(odevid.bufferp);
		odevid.bufferp=0;
		odevid.colwid=0;

		fetched_row=0;
}
void iConnectivityCache::deleteConnectivity(){
		free(linkid.bufferp);
		linkid.bufferp=0;
		linkid.colwid=0;
		free(devfrom.bufferp);
		devfrom.bufferp=0;
		devfrom.colwid=0;
		free(devnamfrom.bufferp);
		devnamfrom.bufferp=0;
		devnamfrom.colwid=0;
		free(portidfrom.bufferp);
		portidfrom.bufferp=0;
		portidfrom.colwid=0;
		free(pnumfrom.bufferp);
		pnumfrom.bufferp=0;
		pnumfrom.colwid=0;
		free(devto.bufferp);
		devto.bufferp=0;
		devto.colwid=0;	
		free(devnamto.bufferp);
		devnamto.bufferp=0;
		devnamto.colwid=0;
		free(portidto.bufferp);
		portidto.bufferp=0;
		portidto.colwid=0;
		free(pnumto.bufferp);
		pnumto.bufferp=0;
		pnumto.colwid=0;

		fetched_row=0;
}
//
/////
///
int eConnectivityCache::Fill_list(char* system_list)
  {
	  int sysID=0;
	  char sysIDlist[100];

	   sysID=RetrieveSysIDList(system_list,sysIDlist);	//list of systemid in numbers; definition in initialize_struct;
	  	    
		int status;
		OCISvcCtx *svchp = ociHdbc;
		OCITrans *txnhp;
		//status	= OCIHandleAlloc( ociEnv, (void**)&svchp, OCI_HTYPE_SVCCTX, 0, 0);
		OCIHandleAlloc((dvoid *)ociEnv, (dvoid **)&txnhp,  OCI_HTYPE_TRANS, 0, 0);
		OCIAttrSet((dvoid *)svchp, OCI_HTYPE_SVCCTX, (dvoid *)txnhp, 0,
														OCI_ATTR_TRANS, ociError);


		status	= OCITransStart(svchp, ociError, 100, OCI_TRANS_NEW+OCI_TRANS_READONLY);
		ShowErrors (status, ociError, "Transaction Start Failed");
		printf("\ngetting number of returned rows...");
    maxrow  = getnumrows_list(sysIDlist);
		printf("%i...done\n",maxrow);

		char *selectstring	= (char*)malloc(1050);	////////////

    status =OCIHandleAlloc (ociEnv, (void**)&stmthp, OCI_HTYPE_STMT , 0, 0);
    if (status != 0) goto end;

	sprintf(selectstring,"select lnk.linkid,lnk.system_name,frdev.deviceid,frdev.devicename||' ',lnk.portidfrom,\
						 frport.port_nbr||'|'||nvl(frport.port_type,'none')||'?',todev.deviceid,todev.devicename||' ',\
						 lnk.portidto,toport.port_nbr||'|'||nvl(toport.port_type,'none')||'?',lnk.link_type,\
						 lnk.bidirectional_link_used,lnk.lkused,lnk.link_weight,nvl(lnk.link_info,'none')||'?',\
						 fr_dtype.devicetypeid,fr_dtype.devicetype||' ',to_dtype.devicetypeid,to_dtype.devicetype||' ' \
						 from	lhcb_macroscopic_connectivity lnk,lhcb_port_properties frport,lhcb_port_properties toport,\
						 lhcb_lg_devices frdev,lhcb_lg_devices todev,lhcb_lg_device_types fr_dtype,lhcb_lg_device_types to_dtype,\
						 lhcb_subsystemlist  sys\
						 where	 mod(lnk.system_name,sys.systemid)=0 and sys.systemid in (%s) and (fr_dtype.devicetypeid=frdev.devicetypeid)\
						 and (to_dtype.devicetypeid=todev.devicetypeid) and (frport.portid = lnk.portidfrom) and\
						 (toport.portid=lnk.portidto) and (frdev.deviceid=frport.deviceid) and (todev.deviceid=toport.deviceid)\0",sysIDlist);


    status=OCIStmtPrepare(stmthp, ociError, (text*) selectstring,(ub4) strlen(selectstring),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
    if (status != 0) goto end;
   // status =OCIBindByName(stmthp, &bndp, ociError,(text*) ":system", -1,(dvoid*)&system, 4,  SQLT_INT, (dvoid *)0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
   // if (status != 0) goto end;

    status=OCIAttrSet (stmthp,OCI_HTYPE_STMT,&maxrow,0,OCI_ATTR_PREFETCH_ROWS,ociError);
    if (status != 0) goto end;
    status=OCIStmtExecute(ociHdbc, stmthp, ociError, 0, 0,(OCISnapshot *) 0, (OCISnapshot *) 0, OCI_DEFAULT );
    if (status != 0) goto end;

    alloc_buffs();

    fetched_row=0;
		status	= 0;
		status =OCIStmtFetch2(stmthp, ociError, maxrow, OCI_FETCH_NEXT,1, OCI_DEFAULT);
    int rescode=ShowErrors (status, ociError, "OCIStmtFetch2 unsuccessful");
		//status =OCIStmtFetch2(stmthp, ociError, maxrow/2, OCI_FETCH_NEXT,1, OCI_DEFAULT);
  //  rescode=ShowErrors (status, ociError, "OCIStmtFetch2 unsuccessful");
		//print_row(0);

		if ((status == 100) || (status == 0))
		{
			int stat1;
			int fetch;
			stat1=OCIAttrGet((dvoid*) stmthp, (ub4) OCI_HTYPE_STMT,(dvoid*) &fetch, (ub4 *) 0, (ub4) OCI_ATTR_ROWS_FETCHED ,(OCIError *) ociError);
			fetched_row	+= fetch;
		}

end:;
		status	= OCITransCommit(svchp, ociError, OCI_DEFAULT );
		status	= OCIHandleFree((dvoid *)txnhp,  OCI_HTYPE_TRANS);
		status	= OCIAttrSet((dvoid *)svchp, OCI_HTYPE_SVCCTX, (dvoid *)0, 0,
														OCI_ATTR_TRANS, ociError);

    return status;
	  return 0;
  }
  	int eConnectivityCache::getnumrows_list(char* system_list)
	{	
		int nrows=0;

		int status;
		OCIDefine* def;
		int nchar;
		//OCIStmt *stmt;
		nchar	= strlen("select count(*) from (")+strlen(")")+strlen(cntselect_2);
		char *numselect	= (char*)malloc(nchar);
		//sprintf(numselect,"select count(*) from (%s)\0", cntselect_2);
		sprintf(numselect,"select count(*) from (select lnk.linkid, lnk.system_name\
						  from lhcb_macroscopic_connectivity lnk, lhcb_subsystemlist sys \
						  where lnk.lkused=1 and mod(lnk.system_name,sys.systemid)=0 \
						  and sys.systemid in (%s))\0",system_list);

    status =OCIHandleAlloc (ociEnv, (void**)&stmthp, OCI_HTYPE_STMT , 0, 0);
    if (status != 0) goto end;
    status=OCIStmtPrepare(stmthp, ociError, (text*) numselect,(ub4) strlen(numselect),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
    if (status != 0) goto end;
		//status =OCIBindByName(stmthp, &bndp, ociError,(text*) ":system_list", -1,(dvoid*)&system_list, 4,  SQLT_INT, (dvoid *)0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
    //**status =OCIBindByName(stmthp, &bndp, ociError,(text*) ":system_list", -1,                               (dvoid*)  system_list, 10*sizeof(int),  SQLT_STR, (dvoid *)0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
	 // status= OCIBindByName(stmthp, &bndp, ociError,(text*)":system_list",(sb4)strlen((char *)":system_list"),(dvoid *) system_list, strlen(system_list)+1,     SQLT_STR, (dvoid *)0,(ub2 *) 0, (ub2 *)0, (ub4) 0,(ub4 *)0, OCI_DEFAULT);
	 // status= OCIBindByName(stmthp, &bndp, ociError,(text*)":system_list", strlen(":system_list"), (ub1 *) system_list, strlen(system_list)+1, SQLT_STR, (dvoid *)0,(ub2 *) 0, (ub2) 0,  (ub4) 0,(ub4 *)0, OCI_DEFAULT);
	//**status= OCIBindByName(stmthp, &bndp, ociError,(text*)":system_list",-1,(dvoid *) system_list, strlen(system_list)+1,SQLT_STR,(dvoid *)0,(ub2 *) 0, (ub2 *)0, (ub4) 0,(ub4 *)0, OCI_DEFAULT);

	int errcode;////////////////////////////////
	char errbuf[100];//////////////////////////


	if (status != 0) goto end;
	
    //status=OCIAttrSet (stmthp,OCI_HTYPE_STMT,&maxrow,0,OCI_ATTR_PREFETCH_ROWS,ociError);
    //if (status != 0) goto end;
    status=OCIStmtExecute(ociHdbc, stmthp, ociError, 0, 0,(OCISnapshot *) 0, (OCISnapshot *) 0, OCI_DEFAULT );	
	if (status != 0) {

		OCIErrorGet((dvoid *) ociError, (ub4) 1, (text *) NULL, &errcode,
                        (OraText *) errbuf, (ub4) sizeof(errbuf),
                        OCI_HTYPE_ERROR);
            printf("Execute Error - %.*s\n", 512, errbuf);

		goto end;}
	
  OCIError* ociError ;
  int rescode;

  ociError = (OCIError*)get_ErrorHandle();
  status=OCIDefineByPos (stmthp, &def, ociError, 1, &nrows, 
		4, SQLT_INT, 0, 0, 0, OCI_DEFAULT/*OCI_DYNAMIC_FETCH*/ );
  if (status != 0)
  {
		nrows	= 0;
    rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
  }

		status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT,1, OCI_DEFAULT);
     rescode=ShowErrors (status, ociError, "OCIStmtFetch2 unsuccessful");

end:;
    return nrows;
  }

int iConnectivityCache::Fill_list(char* system_list)
  {

	  int sysID=0;
	  char sysIDlist[100];

	  sysID=RetrieveSysIDList(system_list,sysIDlist);	//list of systemid in numbers; definition in initialize_struct;
	    
    int status;
		OCISvcCtx *svchp = ociHdbc;
		OCITrans *txnhp;
		//status	= OCIHandleAlloc( ociEnv, (void**)&svchp, OCI_HTYPE_SVCCTX, 0, 0);
		OCIHandleAlloc((dvoid *)ociEnv, (dvoid **)&txnhp,  OCI_HTYPE_TRANS, 0, 0);
		OCIAttrSet((dvoid *)svchp, OCI_HTYPE_SVCCTX, (dvoid *)txnhp, 0,
														OCI_ATTR_TRANS, ociError);


		status	= OCITransStart(svchp, ociError, 100, OCI_TRANS_NEW+OCI_TRANS_READONLY);
		ShowErrors (status, ociError, "Transaction Start Failed");
		printf("\ngetting number of returned rows...");
    maxrow  = getnumrows_list(sysIDlist);
		printf("%i...done\n",maxrow);

		char *selectstring	= (char*)malloc(1050);	////////////

    status =OCIHandleAlloc (ociEnv, (void**)&stmthp, OCI_HTYPE_STMT , 0, 0);
    if (status != 0) goto end;

	sprintf(selectstring,"select distinct lnk.linkid,\
	fromdev.deviceid,\
	fromdev.devicename||'?',\
	fromport.portid,\
	fromport.port_nbr||'|'||nvl(fromport.port_type,'none')||'?',\
	todev.deviceid,\
	todev.devicename||'?',\
	toport.portid,\
	toport.port_nbr||'|'||nvl(toport.port_type,'none')||'?'\
	from LHCB_MICROSCOPIC_CONNECTIVITY lnk,\
	lhcb_port_properties fromport,\
	lhcb_lg_devices fromdev,\
	lhcb_port_properties toport,\
	lhcb_lg_devices todev,\
	lhcb_subsystemlist  sys\
	where fromport.portid=lnk.portidfrom  and\
	fromdev.deviceid=fromport.deviceid and\
	toport.portid=lnk.portidto and\
	todev.deviceid=toport.deviceid and\
	mod(fromdev.system_name,sys.systemid)=0 and sys.systemid in (%s)",sysIDlist);


    status=OCIStmtPrepare(stmthp, ociError, (text*) selectstring,(ub4) strlen(selectstring),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
    if (status != 0) goto end;
   // status =OCIBindByName(stmthp, &bndp, ociError,(text*) ":system", -1,(dvoid*)&system, 4,  SQLT_INT, (dvoid *)0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
   // if (status != 0) goto end;

    status=OCIAttrSet (stmthp,OCI_HTYPE_STMT,&maxrow,0,OCI_ATTR_PREFETCH_ROWS,ociError);
    if (status != 0) goto end;
    status=OCIStmtExecute(ociHdbc, stmthp, ociError, 0, 0,(OCISnapshot *) 0, (OCISnapshot *) 0, OCI_DEFAULT );
    if (status != 0) goto end;

    alloc_buffs();

    fetched_row=0;
		status	= 0;
		status =OCIStmtFetch2(stmthp, ociError, maxrow, OCI_FETCH_NEXT,1, OCI_DEFAULT);
    int rescode=ShowErrors (status, ociError, "OCIStmtFetch2 unsuccessful");
		//status =OCIStmtFetch2(stmthp, ociError, maxrow/2, OCI_FETCH_NEXT,1, OCI_DEFAULT);
  //  rescode=ShowErrors (status, ociError, "OCIStmtFetch2 unsuccessful");
		//print_row(0);

		if ((status == 100) || (status == 0))
		{
			int stat1;
			int fetch;
			stat1=OCIAttrGet((dvoid*) stmthp, (ub4) OCI_HTYPE_STMT,(dvoid*) &fetch, (ub4 *) 0, (ub4) OCI_ATTR_ROWS_FETCHED ,(OCIError *) ociError);
			fetched_row	+= fetch;
		}

end:;
		status	= OCITransCommit(svchp, ociError, OCI_DEFAULT );
		status	= OCIHandleFree((dvoid *)txnhp,  OCI_HTYPE_TRANS);
		status	= OCIAttrSet((dvoid *)svchp, OCI_HTYPE_SVCCTX, (dvoid *)0, 0,
														OCI_ATTR_TRANS, ociError);

    return status;
	  return 0;
  }

    	int iConnectivityCache::getnumrows_list(char* system_list)
	{	
		int nrows=0;

		int status;
		OCIDefine* def;
		int nchar;
		//OCIStmt *stmt;
		nchar	=1000;
		char *numselect	= (char*)malloc(nchar);

		sprintf(numselect,"select count(*) from (select lnk.linkid\
		from LHCB_MICROSCOPIC_CONNECTIVITY lnk,\
		lhcb_port_properties fromport,\
		lhcb_lg_devices fromdev,\
		lhcb_port_properties toport,\
		lhcb_lg_devices todev,\
		lhcb_subsystemlist sys\
		where fromport.portid=lnk.portidfrom  and\
		fromdev.deviceid=fromport.deviceid and\
		toport.portid=lnk.portidto and\
		todev.deviceid=toport.deviceid  and\
		mod(fromdev.system_name,sys.systemid)=0 and sys.systemid in (%s))\0",system_list);


    status =OCIHandleAlloc (ociEnv, (void**)&stmthp, OCI_HTYPE_STMT , 0, 0);
    if (status != 0) goto end;
    status=OCIStmtPrepare(stmthp, ociError, (text*) numselect,(ub4) strlen(numselect),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
    if (status != 0) goto end;
		
	int errcode;////////////////////////////////
	char errbuf[100];//////////////////////////


	if (status != 0) goto end;
	
    //status=OCIAttrSet (stmthp,OCI_HTYPE_STMT,&maxrow,0,OCI_ATTR_PREFETCH_ROWS,ociError);
    //if (status != 0) goto end;
    status=OCIStmtExecute(ociHdbc, stmthp, ociError, 0, 0,(OCISnapshot *) 0, (OCISnapshot *) 0, OCI_DEFAULT );	
	if (status != 0) {

		OCIErrorGet((dvoid *) ociError, (ub4) 1, (text *) NULL, &errcode,
                        (OraText *) errbuf, (ub4) sizeof(errbuf),
                        OCI_HTYPE_ERROR);
            printf("Execute Error - %.*s\n", 512, errbuf);

		goto end;}
	
  OCIError* ociError ;
  int rescode;

  ociError = (OCIError*)get_ErrorHandle();
  status=OCIDefineByPos (stmthp, &def, ociError, 1, &nrows, 
		4, SQLT_INT, 0, 0, 0, OCI_DEFAULT/*OCI_DYNAMIC_FETCH*/ );
  if (status != 0)
  {
		nrows	= 0;
    rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
  }

		status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT,1, OCI_DEFAULT);
     rescode=ShowErrors (status, ociError, "OCIStmtFetch2 unsuccessful");

end:;
    return nrows;
  }
