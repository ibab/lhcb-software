#include "cicDB.h"
#include "CDB.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
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
int DBContainer::followPath(eConnectivityCache *ecache, iConnectivityCache *icache, int elnkidx, HopTyp* /* hop */, int *i, int dep)
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
  int rescode;
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
		sprintf(numselect,"select count(*) from (%s)", cntselect);
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
    ShowErrors (status, ociError, "OCIStmtFetch2 unsuccessful");
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
      {
        res[j].h  = i;
        res[j].typ  = typ;
        j++;
      }
    }
    return j;
  }



	eConnectivityCache::eConnectivityCache (DBContainer *dbase) : ConnectivityCache (dbase)
  {
		selectstring = "select\
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
											nvl(lnk.link_info,'none')||'?'\
										 from\
											lhcb_macroscopic_connectivity lnk,\
											lhcb_port_properties frport,\
											lhcb_port_properties toport,\
											lhcb_lg_devices frdev,\
											lhcb_lg_devices todev\
										 where\
											mod(lnk.system_name, :system)=0 and\
											(frport.portid = lnk.portidfrom) and\
											(toport.portid=lnk.portidto) and\
											(frdev.deviceid=frport.deviceid) and\
											(todev.deviceid=toport.deviceid)";


  cntselect  = "select lnk.linkid, lnk.system_name \
from lhcb_macroscopic_connectivity lnk \
where lnk.lkused=1 and \
mod(lnk.system_name,:system)=0 ";
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


iConnectivityCache::iConnectivityCache (DBContainer *dbase) : ConnectivityCache (dbase)
  {

selectstring  = "select distinct lnk.linkid,\
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

  cntselect  = "select lnk.linkid\
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

