// test.cpp : Defines the entry point for the console application.
//

//#include "stdafx.h"
#include "oci.h"

#include "confdb.h"

#define MAXROW 30000
int ShowErrors (sword status, OCIError *errhp, const char* log);
class ColDesc_int
{
public:
  int *bufferp;
  int colwid;
  OCIDefine* def;

  ColDesc_int()
  {
    bufferp = 0;
    colwid  = 0;
    def = 0;
  };
  void print(int indx)
  {
    printf("%i ",bufferp[indx]);
  }
  ~ColDesc_int()
  {
    if (bufferp != 0)
    {
      free (bufferp);

    }
    colwid  = 0;
  };
int alloc_buff(OCIStmt *statement, int pos, int numrow)
{
  OCIError* ociError ;
  int rescode;
  int status = 0;
  ociError = (OCIError*)get_ErrorHandle();
  bufferp  = (int*)malloc(numrow*sizeof(int));
  if (bufferp == 0)
  {
    return -1;
  }
  colwid = 4;
  status=OCIDefineByPos (statement, &def, ociError,pos, bufferp, colwid, SQLT_INT, 0, 0, 0, OCI_DEFAULT);
  if (status != 0)
  {
    free (bufferp);
    bufferp  = 0;
    colwid = 0;
    rescode=ShowErrors (status, ociError, "OCIParamGet1 unsuccessful");
  }
return 0;
}


};

class ColDesc_char
{
public:
  char *bufferp;
  int colwid;
  OCIDefine* def;

  ColDesc_char()
  {
    bufferp = 0;
    colwid  = 0;
    def = 0;
  };
  void print(int indx)
  {
    printf("%s ",&bufferp[indx*colwid]);
  }
  int alloc_buff(OCIStmt *statement, int pos, int numrow)
{
  OCIParam *parmdp;
  OCIError* ociError ;
  int status;
  char *r =0;
  int rescode;
  int itml=0;
  ociError = (OCIError*)get_ErrorHandle();
  status=OCIParamGet(statement, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) pos);

  if(status==OCI_SUCCESS)
  {
    status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &colwid, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);
    if (status == OCI_SUCCESS)
    {
      colwid++;
      bufferp = (char*)malloc(numrow*(colwid));
    }
    status=OCIDefineByPos (statement, &def, ociError,pos, bufferp, colwid, SQLT_STR, 0, 0, 0, OCI_DEFAULT);
    if (status != 0)
    {
      free (bufferp);
      bufferp  = 0;
      colwid = 0;
      rescode=ShowErrors (status, ociError, "OCIParamGet1 unsuccessful");
    }
  }
  else
  {
    rescode=ShowErrors (status, ociError, "OCIParamGet1 unsuccessful");
  }
  return status;
}

};

  class HopTyp
  {
  public:
    int typ;
    int h;
  };

class ConnectivityCache
{
public:
  OCIStmt *stmthp;
  OCIBind  *bndp;
  CONFDB *db;
  OCIEnv* ociEnv;
  OCIError* ociError;
  OCISvcCtx* ociHdbc;
  int fetched_row;
  int maxrow;
  char *selectstring;

  ConnectivityCache (CONFDB *dbase)
  {
    db  = dbase;
    ociEnv  = db->ociEnv;
    ociError  = db->ociError;
    ociHdbc = db->ociHdbc;
  }
  virtual void alloc_buffs(void)=0;
  int Fill(char *dev, int maxr)
  {
    int status;
    maxrow  = maxr;
    status =OCIHandleAlloc (ociEnv, (void**)&stmthp, OCI_HTYPE_STMT , 0, 0);
    if (status != 0) goto end;
    status=OCIStmtPrepare(stmthp, ociError, (text*) selectstring,(ub4) strlen(selectstring),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
    if (status != 0) goto end;
    status =OCIBindByName(stmthp, &bndp, ociError,(text*) ":devname", -1,(dvoid*)dev, strlen(dev)+1,  SQLT_STR, (dvoid *)0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
    if (status != 0) goto end;

    status=OCIAttrSet (stmthp,OCI_HTYPE_STMT,&maxrow,0,OCI_ATTR_PREFETCH_ROWS,ociError);
    if (status != 0) goto end;
    status=OCIStmtExecute(ociHdbc, stmthp, ociError, 0, 0,(OCISnapshot *) 0, (OCISnapshot *) 0, OCI_DEFAULT);
    if (status != 0) goto end;

    alloc_buffs();

    fetched_row=0;
    status =OCIStmtFetch2(stmthp, ociError, maxrow, OCI_FETCH_NEXT,1, OCI_DEFAULT);
    if (status == 100)
    {
      int stat1;
      stat1=OCIAttrGet((dvoid*) stmthp, (ub4) OCI_HTYPE_STMT,(dvoid*) &fetched_row, (ub4 *) 0, (ub4) OCI_ATTR_ROWS_FETCHED ,(OCIError *) ociError);
    }
end:;
    return status;
  }
  int FindinCol(ColDesc_int *col, int val, int typ, HopTyp *res)
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
  int FindinCol(ColDesc_char *col, char *val, int typ, HopTyp *res)
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
};


class eConnectivityCache : public ConnectivityCache
{
public:
  ColDesc_int linkid;
  ColDesc_int devfrom;
  ColDesc_int devto;
  ColDesc_char pnumfrom;
  ColDesc_char pnumto;
  ColDesc_int linktyp;
  ColDesc_int bidir;
  ColDesc_int linkused;
  ColDesc_int linkweight;
  ColDesc_char devnamfrom;
  ColDesc_char devnamto;
  ColDesc_int sysnam;
  ColDesc_int portidfrom;
  ColDesc_int portidto;
  ColDesc_char linkinfo;
  ColDesc_int odevid;

  eConnectivityCache (CONFDB *dbase) : ConnectivityCache (dbase)
  {
    selectstring  = &"select t.linkid,\
  g.deviceid,\
  f.deviceid,\
  g.port_nbr||'|'||nvl(g.port_type,'none')||'?',\
  f.port_nbr||'|'||nvl(f.port_type,'none')||'?',\
  t.link_type,\
  t.bidirectional_link_used,\
  t.lkused,\
  t.link_weight,\
  m.devicename||'?',\
  n.devicename||'?',\
  nvl(t.link_info,'none')||'?',\
  t.system_name,\
  t.portidfrom,\
  t.portidto, \
  l.deviceid \
  from lhcb_macroscopic_connectivity t,\
  lhcb_port_properties g,\
  lhcb_port_properties f,\
  lhcb_lg_devices l,\
  lhcb_lg_devices m,\
  lhcb_lg_devices n \
  where t.lkused=1 and \
  g.deviceid=m.deviceid and \
  f.deviceid=n.deviceid and \
  t.portidfrom=g.portid and \
  t.portidto=f.portid and \
  mod(l.system_name,t.system_name)*mod(t.system_name,l.system_name)=0 and \
  (l.devicename = :devname)";
  }
  virtual void alloc_buffs(void)
  {
    int status;
    status = linkid.alloc_buff(stmthp, 1, maxrow);
    status = devfrom.alloc_buff(stmthp, 2, maxrow);
    status = devto.alloc_buff(stmthp, 3, maxrow);
    status = pnumfrom.alloc_buff(stmthp, 4, maxrow);
    status = pnumto.alloc_buff(stmthp, 5, maxrow);
    status = linktyp.alloc_buff(stmthp, 6, maxrow);
    status = bidir.alloc_buff(stmthp, 7, maxrow);
    status = linkused.alloc_buff(stmthp, 8, maxrow);
    status = linkweight.alloc_buff(stmthp, 9, maxrow);
    status = devnamfrom.alloc_buff(stmthp, 10, maxrow);
    status = devnamto.alloc_buff(stmthp, 11, maxrow);
    status = linkinfo.alloc_buff(stmthp, 12, maxrow);
    status = sysnam.alloc_buff(stmthp, 13, maxrow);
    status = portidfrom.alloc_buff(stmthp, 14, maxrow);
    status = portidto.alloc_buff(stmthp, 15, maxrow);
    status = odevid.alloc_buff(stmthp, 16, maxrow);
  }
  void print_row(int r)
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
};

class iConnectivityCache : public ConnectivityCache
{
public:
  ColDesc_int linkid;
  ColDesc_int devfrom;
  ColDesc_char devnamfrom;
  ColDesc_int portidfrom;
  ColDesc_char pnumfrom;

  ColDesc_int devto;
  ColDesc_char devnamto;
  ColDesc_int portidto;
  ColDesc_char pnumto;

  iConnectivityCache (CONFDB *dbase) : ConnectivityCache (dbase)
  {
    selectstring  = &"select distinct lnk.linkid, \
  fromdev.deviceid, \
  fromdev.devicename||'?', \
  fromport.portid, \
  fromport.port_nbr||'?', \
  todev.deviceid, \
  todev.devicename||'?', \
  toport.portid, \
  toport.port_nbr||'?' \
from LHCB_MICROSCOPIC_CONNECTIVITY lnk, \
  lhcb_port_properties fromport, \
  lhcb_lg_devices fromdev, \
  lhcb_port_properties toport, \
  lhcb_lg_devices todev, \
  lhcb_lg_devices n, \
  lhcb_subsystemList l \
where fromport.portid=lnk.portidfrom  and \
  fromdev.deviceid=fromport.deviceid and \
  toport.portid=lnk.portidto and \
  todev.deviceid=toport.deviceid and \
  mod(n.system_name,l.systemid)=0 and \
  mod(fromdev.system_name,l.systemid)=0 and \
  mod(todev.system_name,l.systemid)=0 and \
  n.devicename=:devname"; 
  }


  virtual void alloc_buffs(void)
  {
    int status;
    status = linkid.alloc_buff(stmthp, 1, maxrow);
    status = devfrom.alloc_buff(stmthp, 2, maxrow);
    status = devnamfrom.alloc_buff(stmthp, 3, maxrow);
    status = portidfrom.alloc_buff(stmthp, 4, maxrow);
    status = pnumfrom.alloc_buff(stmthp, 5, maxrow);
    status = devto.alloc_buff(stmthp, 6, maxrow);
    status = devnamto.alloc_buff(stmthp, 7, maxrow);
    status = portidto.alloc_buff(stmthp, 8, maxrow);
    status = pnumto.alloc_buff(stmthp, 9, maxrow);
  }
  void print_row(int r)
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

};
int followPath(eConnectivityCache *ecache, iConnectivityCache *icache, int elnkidx, HopTyp *hop, int *i)
{
    int j,k, nres;
    nres  = icache->FindinCol(&icache->portidfrom,ecache->portidto.bufferp[elnkidx], 1, &hop[*i]);
    *i  += nres;
    if (nres <= 0) return 0;
    nres  = ecache->FindinCol(&ecache->portidfrom,icache->portidto.bufferp[hop[*i-1].h], 2,&hop[*i]);
    j = *i;
    HopTyp *h = &hop[j];
    *i  += nres;
    for (k  = 0;k<nres;k++)
    {
      followPath(ecache,icache,h[k].h,hop,i);
    }
    return 0;
}


int main(int argc, char* argv[])
{
  CONFDB *cdb;
  int status;
  string db("devdb");
  string user("lhcb_configuration_dev");
  string passw("T_75_op_LbDB9");
  cdb = new CONFDB(db,user,passw);
  cdb->PyDBConnexion();
  eConnectivityCache *ecache;
  iConnectivityCache *icache;
  ecache  = new eConnectivityCache(cdb);
  char devfro[]="MUONC_M4C20A1_PAD1_0_0";

  status  = ecache->Fill(devfro,MAXROW);


  icache  = new iConnectivityCache(cdb);
    
  status  = icache->Fill(devfro,MAXROW);
  HopTyp hop[100];

  HopTyp indx [10];
  int nres;
  int j;
  int dev;
  nres  = ecache->FindinCol(&ecache->devfrom,ecache->odevid.bufferp[0], 2, hop);
  int i = 1;
  followPath(ecache,icache,hop[0].h,hop,&i);
  for (j=0;j<i;j++)
  {
    if (hop[j].typ == 1)
    {
      icache->print_row(hop[j].h);
    }
    else
    {
      ecache->print_row(hop[j].h);
    }
  }
  //hop[0]  = indx;
  //for (j=0;j<nres;j++)
  //{
  //  ecache->print_row(indx[j]);
  //}
  //  int i=0;
  //  while (hop[i] >= 0)
  //  {
  //    i++;
  //    hop[i]  = icache->FindinCol(&icache->portidfrom,ecache->portidto.bufferp[hop[i-1]], indx);
  //    if (hop[i] >= 0)icache->print_row(hop[i]);
  //    if (hop[i] < 0) break;
  //    i++;
  //      hop[2]  = ecache->FindinCol(&ecache->portidfrom,icache->portidto.bufferp[hop[i-1]],indx);
  //    if (hop[2] >= 0 )ecache->print_row(hop[2]);
  //  }
  cdb->PyDBDeconnexion();

  
  return 0;
}
