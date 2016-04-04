#include <string>
#include <thread>
#include <mutex>
#include <vector>
#include "string.h"
#include "stdio.h"
#include "unistd.h"
#include "stdlib.h"
#include "ctype.h"
#include "dim/dis.hxx"
#include "src/NewLib/NodeThread.h"
using namespace std;

vector<string> StrSplit(string &s, char* tok)
{
  vector<string> ret;
  string ss = s;
  size_t tk = ss.find(tok);
  while(tk!=string::npos)
  {
    ret.push_back(ss.substr(0,tk));
    ss=ss.substr(tk+1,string::npos);
    tk = ss.find(tok);
  }
  ret.push_back(ss);
  return ret;
}
void ToLower(string &s)
{
  for (size_t i=0;i<s.size();i++)
  {
    s[i]=tolower(s[i]);
  }
}
void ToUpper(string &s)
{
  for (size_t i=0;i<s.size();i++)
  {
    s[i]=toupper(s[i]);
  }
}
class NodeListElement : public NodeConfiguration
{
  public:
    NodeListElement(string &N, string &U, string &P, string *sp,
        int statepoll=1, int setuppoll=10):NodeConfiguration(N,U,P,sp,statepoll,setuppoll)
    {
      m_thread = 0;
    };
    NodeListElement():NodeConfiguration()
    {
      m_thread = 0;
    };
    thread *m_thread;
};
void FarmSetup(char * farms, vector<NodeListElement*> &nodelist, NodeListElement &templ)
{
  size_t k;
  char r;
  char nodn[256];
  for (k = 0; k < strlen(farms); k++)
  {
    r = farms[k];
    switch (r)
    {
      case 'a':
      case 'b':
      case 'c':
      case 'd':
      case 'e':
      {
        for (int j = 1; j <= 10; j++)
        {
          for (int i = 1; i <= 28; i++)
          {
            sprintf(nodn, "hlt%c%02d%02d-IPMI", r, j, i);
//            nodes.push_back(string(nodn));
            string nn = nodn;
            ToUpper(nn);
            nodelist.push_back(new NodeListElement(nn,templ.UserName,templ.Password,templ.ServicePrefix));
//            threads.push_back(new thread(NodeThread, new NodeConfiguration(nn,uname,pword,ServicePrefix)
//                ,setup_fail,setup_fail_mtx));
          }
        }
        break;
      }
      case 'f':
      {
        for (int j = 1; j <= 12; j++)
        {
          int nods = 32;
          if (j == 7)
            nods = 28;
          for (int i = 1; i <= nods; i++)
          {
            sprintf(nodn, "hltf%02d%02d-IPMI", j, i);
            string nn = nodn;
            ToUpper(nn);
//            nodes.push_back(string(nodn));
            nodelist.push_back(new NodeListElement(nn,templ.UserName,templ.Password,templ.ServicePrefix));
//            threads.push_back(new thread(NodeThread, new NodeConfiguration(nn,uname,pword,ServicePrefix)
//                ,setup_fail,setup_fail_mtx));
          }
        }
      }
    }
  }

}
size_t fnd;
void ParseConfigFile(char *supFile,vector<NodeListElement*> &nodelist, NodeListElement &templ)
{
  FILE * f;
  char lin[2048];
  f=fopen(supFile,"r");
  string line;
  line.reserve(2048);
//  char *stat;
  if (f == 0) return;
  fgets(lin,2048,f);
  while (!feof(f))
  {
    line = lin;
    line = line.substr(0,line.find("\n"));
    fnd = line.find("#");

    line = line.substr(0,fnd);
    if (line.size() == 0)
    {
      line.reserve(2048);
      fgets(lin,2048,f);
      continue;
    }
    vector<string> splt = StrSplit(line,(char*)"=");
    if (splt.size()>1)
    {
      if (splt[0].find("IPMI_USER")!=string::npos)
      {
        string opt = splt[1];
        while (size_t blk=opt.find(" ") != string::npos)
        {
          opt.replace(blk,1,"");
          blk=opt.find(" ");
        }
        templ.UserName = opt;
      }
      else if (splt[0].find("IPMI_PASSWD")!=string::npos)
      {
        string opt = splt[1];
        while (size_t blk=opt.find(" ") != string::npos)
        {
          opt.replace(blk,1,"");
          blk=opt.find(" ");
        }
        templ.Password = opt;
      }
      else if (splt[0].find("IPMI_CMD")!=string::npos)
      {
        printf("Option IPMI_CMD ignored");
      }
      else
      {
        printf("Illegal Option %s",line.c_str());
      }
      line.reserve(2048);
      fgets(lin,2048,f);
      continue;
    }
    else
    {
//hostName,userName,passWord,port,authType,privLvl,oemType
      vector<string> splt = StrSplit(line,(char*)",");
      string nn="";
      NodeListElement *el;
      if (splt.size()>0)
      {
        el = new NodeListElement(nn,templ.UserName,templ.Password,templ.ServicePrefix);
      }
      else
      {
        line.reserve(2048);
        fgets(lin,2048,f);
        continue;
      }
      for (size_t i=0;i<splt.size();i++)
      {
        switch(i)
        {
          case 0:
          {
            string nname = splt[i];
            ToUpper(nname);
            el->NodeName = nname;
            break;
          }
          case 1:
          {
            if (splt[i].find("NONE") != string::npos)
            {
              el->UserName = splt[i];
            }
            break;
          }
          case 2:
          {
            if (splt[i].find("NONE") != string::npos)
            {
              el->Password = splt[i];
            }
            break;
          }
          case 3:
          {
            if (splt[i].find("NONE") != string::npos)
            {
              printf("Port Option in Node definition ignored\n");
            }
            break;
          }
          case 4:
          {
            if (splt[i].find("NONE") != string::npos)
            {
              printf("authType Option in Node definition ignored\n");
            }
            break;
          }
          case 5:
          {
            if (splt[i].find("NONE") != string::npos)
            {
              printf("privLvl Option in Node definition ignored\n");
            }
            break;
          }
          case 6:
          {
            if (splt[i].find("NONE") != string::npos)
            {
              printf("oemType Option in Node definition ignored\n");
            }
            break;
          }
          default:
          {
            printf("Too many items in node definition. Subsequent items ignored\n");
            break;
          }
        }
      }
      nodelist.push_back(el);
    }
    line.reserve(2048);
    fgets(lin,2048,f);
  }
  fclose(f);
}

int main(int argc, char **argv)
{
#define OPTIONS "r:c:d:s:n:p:"
//  int status;
  string *ServerName = new string("MyIPMIServer");
  string *ServicePrefix = new string("MyIPMIServer");
  string *setup_fail=new string;
  string stat_fail;
  mutex m;
  mutex *setup_fail_mtx = new mutex;;
  mutex stat_fail_mtx;
  vector<string> nodes;
  vector<thread*> threads;
  string uname("root");
  string pword("lhcbipmi");
//  string nnod1("hlta0101");
//  string nnod2("hlta0906");
  vector<NodeListElement*> nodelist;
  if (argc == 1)
  {
    printf("Usage: ipmiSrv <options>\n");
    printf("Options:\n");
    printf("\t-r <Farm Rows> e.g. abc\n");
    printf("\t-c <configuration file>\n");
    printf("\t-d <Polling delay in seconds>\n");
    printf("\t-s <Setup retry delay> \n");
    printf("\t-n <Dim Server Name>\n");
    printf("\t-p <Dim Node Service Prefix>\n");
    printf("\nNote: options -r and -c are mutually exclusive\n");
    return 0;
  }
  int c;
  char *farms=0;
  char *supFile=0;
  int pollDel=1;
  int supDel=10;
  char *sname=0;
  char *servpf=0;
  while ((c = getopt(argc,argv,OPTIONS)) != -1)
  {
    switch(c)
    {
      case 'r':
      {
        farms = optarg;
        break;
      }
      case 'c':
      {
        supFile = optarg;
        break;
      }
      case 'd':
      {
        sscanf("%d",optarg,&pollDel);
        break;
      }
      case 's':
      {
        sscanf("%d",optarg,&supDel);
        break;
      }
      case 'n':
      {
        sname = optarg;
        ServerName = new string(sname);
        break;
      }
      case 'p':
      {
        servpf = optarg;
        ServicePrefix = new string(servpf);
        break;
      }
    };
  };
  if ((farms!=0) && (supFile != 0))
  {
    printf ("the Farms and configuration file options are mutually exclusive\n");
    return 0;
  }
  DimServer::autoStartOn();
  DimServer::start((char*)ServerName->c_str());
//#if 0
//#endif
//  thread t1 = thread(NodeThread,nnod1);
//  thread t2 = thread(NodeThread,nnod2);
//  new thread(NodeThread,"hltd0112");
//  new thread(NodeThread,"hlte0602");
//  new thread(NodeThread,"hlte0603");
//  new thread(NodeThread,"hltd0922");
  string oldstr;
  string failsvcname = *ServerName;
  failsvcname +="/SetupFaileds";
  string nn="";
  NodeListElement NodeTemplate(nn,uname,pword,ServicePrefix);
  if (farms)
  {
    FarmSetup(farms,nodelist,NodeTemplate);
  }
  else if (supFile)
  {
    ParseConfigFile(supFile,nodelist,NodeTemplate);
  }
//  for (size_t ii=0;ii<nodelist.size();ii++)
//  {
//    printf ("%s\n",nodelist[ii]->NodeName.c_str());
//  }
  for (size_t i=0;i<nodelist.size();i++)
  {
    nodelist[i]->m_thread = new thread(NodeThread,nodelist[i],setup_fail,setup_fail_mtx);
  }
  DimService servc(failsvcname.c_str(),"C",(char*)setup_fail->c_str(),1);
  while (1)
  {
//    sleep(0xffffffff);
    sleep(2);
    setup_fail_mtx->lock();
    if (oldstr != *setup_fail)
    {
      servc.updateService((char*)setup_fail->c_str(),setup_fail->size());
//      printf("===> Setup Failing Nodes: %s\n",setup_fail->c_str());
      oldstr=*setup_fail;
    }
    setup_fail_mtx->unlock();
  }
    return 0;
}
