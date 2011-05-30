/*
 * HistTask.cpp
 *
 *  Created on: Jan 31, 2011
 *      Author: beat
 */

#include "Gaucho/HistTask.h"
#include "Gaucho/Utilities.h"
#include "Gaucho/MonHist.h"
//HistTask::HistTask(const std::string &task,const std::string &dns)
//: m_DNS(dns),m_task(task)
//{
//  char *srvs=0;
//  if (m_DNS == "")
//  {
//    m_DNS = std::string(getenv("DIM_DNS_NODE"));
//  }
//  DimClient::setDnsNode(m_DNS.c_str());
//  m_taskexp = boost::regex(("(.*)"+m_task+"(.*)").c_str(),boost::regex_constants::icase);
//  DimCurrentInfo *dns_info=new DimCurrentInfo((char*)"DIS_DNS/SERVER_LIST", (char*)"DEAD");
//  srvs = dns_info->getString();
////  std::string dimServers(srvs);
//  dyn_string *svcs = Strsplit(srvs,(char*)"|");
//  unsigned int i;
//  m_service = "";
//  m_RPC = 0;
//  for (i=0;i<svcs->size();i++)
//  {
//    int status = boost::regex_search(svcs->at(i),m_taskexp);
//    if (status)
//    {
//      m_service = std::string("MON_")+svcs->at(i).substr(0,svcs->at(i).find("@"))+std::string("/Histos/HistCommand");
//      m_RPC = new RPCRec((char*)m_service.c_str(),3,true);
//    }
//  }
//}
HistTask::~HistTask()
{
  deletePtr(m_RPC);
}
HistTask::HistTask(const std::string &task,const std::string &dns, int tmo)
: m_DNS(dns),m_task(task),m_RPC(0)
{
  if (m_DNS == "")
  {
    m_DNS = std::string(getenv("DIM_DNS_NODE"));
  }
  DimClient::setDnsNode(m_DNS.c_str());
  DimBrowser b;
  m_service = "MON_"+m_task+"/Histos/HistCommand";
  int nservcs = b.getServices((m_service+"*").c_str());
  if (nservcs != 0)
  {
    m_RPC = new RPCRec((char*)m_service.c_str(),tmo,true);
  }
}
int HistTask::Directory(std::vector<std::string> &hists)
{
  int status;
  if (m_RPC == 0) return 1;
  int dat = RPCCDirectory;
  m_RPC->setData(dat);
  status = m_RPC->analyseReply();
  unsigned int i;
  for (i=0;i<m_RPC->names->size();i++)
  {
    hists.push_back(std::string(m_RPC->names->at(i)));
  }
  return status;
}
int HistTask::Histos(std::vector<std::string> &hists,std::vector<TObject*> &histos)
{
  int status;
  if (m_RPC == 0) return 1;
  RPCCommRead *rrd;
  int rdlen = sizeof(RPCCommRead);
  unsigned int i;
  for (i=0;i<hists.size();i++)
  {
    rdlen += strlen(hists[i].c_str())+1;
  }
  rrd = (RPCCommRead*)malloc(rdlen);
  memset(rrd,0,rdlen);
  rrd->c = RPCCRead;
  char *dst = &rrd->which;
  for (i=0;i<hists.size();i++)
  {
    int clen;
    clen = strlen(hists[i].c_str());
    memcpy(dst,hists[i].c_str(),clen);
    dst += clen;
    *dst = '\n';
    dst++;
  }
  m_RPC->setData((void*)rrd,rdlen);
  status  = m_RPC->analyseReply();
  if (status == 0)
  {
    PTRMAP::iterator k;
    for (k=m_RPC->hists->begin() ;k!=m_RPC->hists->end();k++)
    {
      TObject *o = (TObject*)MonHist::de_serialize(k->second);
      histos.push_back(o);
    }
  }
  return status;
}
int HistTask::Histos(std::vector<std::string> &hists)
{
  int status;
  RPCCommRead *rrd;
  int rdlen = sizeof(RPCCommRead);
  unsigned int i;
  for (i=0;i<hists.size();i++)
  {
    rdlen += strlen(hists[i].c_str())+1;
  }
  rrd = (RPCCommRead*)malloc(rdlen);
  memset(rrd,0,rdlen);
  rrd->c = RPCCRead;
  char *dst = &rrd->which;
  for (i=0;i<hists.size();i++)
  {
    int clen;
    clen = strlen(hists[i].c_str());
    memcpy(dst,hists[i].c_str(),clen);
    dst += clen;
    *dst = '\n';
    dst++;
  }
  m_RPC->setData((void*)rrd,rdlen);
  status  = m_RPC->analyseReply();
  return status;
}

int HistTask::Histos(std::vector<std::string> &hists,std::map<std::string,TObject*> &histos)
{
  int status;
  status = Histos(hists);
  if (status == 0)
  {
    PTRMAP::iterator k;
    for (k=m_RPC->hists->begin() ;k!=m_RPC->hists->end();k++)
    {
      TObject *o = (TObject*)MonHist::de_serialize(k->second);
      histos.insert(std::pair<std::string,TObject*>(k->first,o));
    }
  }
  return status;
}

int HistTask::TaskList(std::string dns, std::vector<std::string> &tasklist)
{
  if (dns == "")
  {
    dns = std::string(getenv("DIM_DNS_NODE"));
  }
  DimClient::setDnsNode(dns.c_str());
  DimBrowser b;
  int nservcs = b.getServices("MON_*/Histos/Data",5);
  int i;
  for (i=0;i<nservcs;i++)
  {
    char *svcn, *fmt;
    int stat = b.getNextService(svcn,fmt);
    if (stat != 0)
    {
      std::string svc = std::string(svcn);
      tasklist.push_back(svc.substr(strlen("MON_"),svc.find("/Histos")-strlen("MON_")));
    }
  }
  return 0;
}
