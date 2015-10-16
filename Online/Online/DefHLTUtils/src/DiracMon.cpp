//============================================================================
// Name        : DiracMon.cpp
// Author      :
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <sys/socket.h>
#include <linux/netlink.h>
#include <linux/connector.h>
#include <linux/cn_proc.h>
#include <signal.h>
#include <errno.h>
#include <stdbool.h>
#include <unistd.h>
#include <cstring>
#include <cstdlib>
#include <stdio.h>
#include <iostream>
#include <string>
//#include <iostream>
#include <set>
#include <vector>
#include <string>
#include "string.h"
#include <list>
#include <map>
#include "time.h"
//#include <multimap>

//#include "stdio.h"
#include "dis.hxx"
#include <fstream>
#include <iostream>
//#include <string>

#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "RTL/rtl.h"

using namespace std;
class dyn_string : public std::vector<std::string >
{
public:
  dyn_string() {};
  virtual ~dyn_string()
  {
  }
};
DimService *StatusService;
DimServer *StatusServer;
time_t BootTime;
dyn_string *Strsplit(char *s, char *del)
{
  char *pch;
  pch = strtok(s,del);
  dyn_string *v=new dyn_string();
  int nel=0;
  while (pch != 0)
  {
    v->push_back(std::string(pch));
    if (pch != s) *(pch-1) = *del;
    nel++;
    pch = strtok(0,del);
  }
  return v;
}

dyn_string *Strsplit(const char *s, const char *del)
{
  return Strsplit((char*)s, (char*)del);
}

dyn_string *Strsplit(const char *s, char *del)
{
  return Strsplit((char*)s,del);
}

dyn_string *Strsplit(char *s, const char *del)
{
  return Strsplit(s,(char*)del);
}

class Task
{
  public:
    Task():pid(0),ppid(0),child(0){};
    int pid;
    int ppid;
    string cmdline;
    Task *child;
};
class TNode;
string leads;
typedef map<int,TNode*> NodeMap;
NodeMap Taskmap;
int getPPID(int,time_t*);
void get_cmd_line(int ipid,string &cmdline);
void ReadDir(string dir,vector<string> &procs)
{
//  ifstream fin;
  string filepath;
  DIR *dp;
  struct dirent *dirp;
  struct stat filestat;

  dp = opendir( dir.c_str() );
  if (dp == NULL)
  {
    return;
  }
  while ((dirp = readdir( dp )))
    {
    filepath = dir + "/" + dirp->d_name;
    string dirn(dirp->d_name);
    // If the file is a directory (or is in some way invalid) we'll skip it
    if (stat( filepath.c_str(), &filestat )) continue;
    if (dirn.find_first_not_of("0123456789")!=string::npos) continue;
    if (S_ISDIR( filestat.st_mode ))
    {
      procs.push_back(dirn);
    }
    // Endeavor to read a single number from the file and display it
    }
  closedir( dp );
}
enum
{
  TNODE_PROCESS=1,
  TNODE_THREAD
};
class TNode
{
  public:
  TNode *Parent;
  int Pid;
  int PPid;
  int type;
  time_t starttime;
  list<int> siblings;
  map<int,TNode*> SibMap;
  string cmdline;
  int NumChilds()
  {
    return siblings.size();
  }
  void AddChild(TNode &n)
  {
//    printf("Adding PID %d to Parent %d\n", n.Pid,this->Pid);
    siblings.push_back(n.Pid);
    SibMap[n.Pid]=&n;
  }
  void removeChild(TNode &n, bool del=true)
  {
//    printf("Removing PID %d from Parent %d\n", n.Pid,Pid);
    Taskmap.erase(n.Pid);
    siblings.remove(n.Pid);
    SibMap.erase(n.Pid);

    if (del) delete &n;
  }
  TNode():Parent(0),Pid(0),PPid(0)
  {
    type =TNODE_PROCESS;
  }
  TNode(int pid):Parent(0),Pid(pid),PPid(0)
  {
    type =TNODE_PROCESS;
  }
  ~TNode ()
  {
    if (NumChilds() != 0)
    {
      printf("FUNNY>>>> deleting TNode (pid %d, ppid %d) while number of siblings is no Zero. Removing siblings...\n",Pid, PPid);
      for (NodeMap::iterator it=SibMap.begin();it!=SibMap.end();it=SibMap.begin())
      {
        removeChild(*(*it).second);
      }
    }
    Parent = 0;
  }
  TNode(int pid, int Ppid)
  {
    Pid = pid;
    Parent =0;
    type =TNODE_PROCESS;
//    printf("New Tree Node with PID %d, Parent PID %d\n",Pid,Ppid);
    Taskmap[Pid] = this;
    PPid = Ppid;
    if (PPid == -1) return;
    NodeMap::iterator it=Taskmap.find(PPid);
    if (it != Taskmap.end())
    {
      time_t stt;
      getPPID(pid,&stt);
      starttime = BootTime+stt;
      Parent = (*it).second;
      Parent->AddChild(*this);
    }
    else
    {
//      printf("Parent Node (%d) of PID %d not yet in Map. Adding it...\n",PPid, Pid);
      time_t stt;
      int pPPid = getPPID(PPid,&stt);
      Parent = new TNode(PPid,pPPid);
      Parent->starttime = BootTime+stt;
      getPPID(pid,&stt);
      starttime = BootTime+stt;
      Parent->AddChild(*this);
    }
//    AddThreads()
  }
  void AddThreads()
  {
    char cpid[128];
//    sprintf(cpid,"%d",Pid);
    string dir = "/proc/";
    dir += cpid;
    dir += "/task";
    vector<string> threads;
    ReadDir(dir,threads);
//    printf ("PID %d Tasks ",Pid);
//    for (size_t i=0;i<threads.size();i++) printf("%s ",threads[i].c_str());
//    printf("\n");
    for (size_t i=0;i<threads.size();i++)
    {
      int tid;
      sscanf(threads[i].c_str(),"%d",&tid);
      if ((tid == Pid)||(tid == PPid)) continue;
      NodeMap::iterator it = Taskmap.find(tid);
      if (it == Taskmap.end())
      {
        TNode *nod = new TNode(tid,Pid);
        nod->type = TNODE_THREAD;
      }
      else
      {
        AddChild(*(*it).second);
        (*it).second->type = TNODE_THREAD;
      }
    }
  }
  void setCmdline()
  {
    get_cmd_line(Pid,cmdline);
  }
  void setCmdline(string c)
  {
    cmdline = c;
  }
  void Print(string &blanks,bool cmd=true)
  {
    string l;
    if (cmd)
    {
      printf("%s[%d] %s\n",blanks.c_str(),Pid,cmdline.c_str());
    }
    else
    {
      printf("%s[%d]\n",blanks.c_str(),Pid);
    }
    l = blanks;
    l.append("   ");
    for (list<int>::iterator i = siblings.begin();i!= siblings.end();i++)
    {
      SibMap[*i]->Print(l,cmd);
    }
  }
  int Dump(char* buff,bool nothread = true,bool top=false, int level=-1,char del=char(1),int max_level =25)
  {
    int ii;
    int s=0;
    if (level > max_level) return s;
    if (nothread && (type == TNODE_THREAD)) return s;
    if (top)
    {
      s+=sprintf(buff+s,"[%d,%ld,%s]",Pid,starttime,cmdline.c_str());
      s+=sprintf(buff+s,"%c",del);
    }
    ii=0;
    for (list<int>::iterator i = siblings.begin();i!= siblings.end();i++)
    {
      if (nothread && (SibMap[*i]->type == TNODE_THREAD)) continue;
      if (ii >0)
      {
//        s+=sprintf(buff+s,"%c",del);
      }
      s+=SibMap[*i]->Dump(buff+s,nothread,true,level+1,del+1,max_level);
      ii++;
//      list<int>::iterator j =i;
//      j++;
//      if (j != siblings.end()) s+=sprintf(buff+s,"%c",del);
    }
//    if (!top) s += sprintf(buff+s,"%c",del);
//    else      s += sprintf(buff+s,"%c",del);
    return s;
  }
  TNode *find(int pid)
  {
    TNode *ret = 0;
    if (pid == Pid)
    {
      ret = this;
    }
    else
    {

      map<int,TNode*>::iterator i = SibMap.find(pid);
      if (i != SibMap.end())
      {
        ret = (*i).second;
      }
      else
      {
        for (list<int>::iterator j=siblings.begin();j!= siblings.end();j++)
        {
          ret = SibMap[*j]->find(pid);
          if (ret!=0) break;
        }
      }
    }
    return ret;
  }
  void fillMap(NodeMap &m)
  {
    m[Pid] = this;
    for (list<int>::iterator i=siblings.begin();i!= siblings.end();i++)
    {
      SibMap[*i]->fillMap(m);
    }
  }
};


//map<int,Task*> Tasks;
//map<int,Task*> All_Tasks;
//set<int> Agent_Tree;
//set<int> AllChilds;

int tmSrv_PID;
static volatile bool need_exit = false;
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

time_t get_Boot_Time()
{
  char line[4*4096];
  string sline;
  sline.erase();
  string filen;
  filen.erase();
  filen.append( "/proc/stat");
  FILE *f=fopen(filen.c_str(),"r");
  if (f == 0) return 0;
//      char *stat = fgets(line,sizeof(line),f);
  size_t cnt = fread(line,1,sizeof(line),f);
  fclose(f);
  line[cnt] = 0;
  sline.assign(line);
  size_t start;
  size_t end;
  start = sline.find("btime");
  end = sline.find('\n',start);
  char dummy[128];
  time_t btime;
  sscanf(sline.substr(start,end-start+1).c_str(),"%s %ld",dummy,&btime);
  return btime;
}

void get_cmd_line(int ipid,string &cmdline)
{
  char line[4096];
  char statl[512];
  char pid[128];
  cmdline.erase();
  sprintf(pid,"%d",ipid);
  string filen;
  filen.append( "/proc/");
  filen.append(pid);
  filen.append("/cmdline");
  FILE *f=fopen(filen.c_str(),"r");
  if (f == 0) return;
//      char *stat = fgets(line,sizeof(line),f);
  size_t cnt = fread(line,1,sizeof(line),f);
  fclose(f);
  if (cnt == 0)
  {
    string filen;
    filen.append( "/proc/");
    filen.append(pid);
    filen.append("/stat");
    FILE *f=fopen(filen.c_str(),"r");
    if (f == 0) return;
  //      char *stat = fgets(line,sizeof(line),f);
    cnt = fread(statl,1,sizeof(line),f);
    char *blk = strchr(statl,' ');
    blk++;
    char *blk1 = strchr(blk,' ');
    cnt = blk1-blk;
    strncpy(line,blk,cnt);
  }
 cmdline.assign(line,cnt);
  for (unsigned int j=0;j<cmdline.size();j++)
  {
    if (cmdline[j]==0) cmdline[j] = ' ';
  }
}

int find_tmsrv(char *nam)
{
  vector<string> procs;
  procs.clear();
  ReadDir("/proc",procs);
  printf("Process ID present\n");
  char line[4096];
  string cmdline;
  for (unsigned int i=0;i<procs.size();i++)
  {
    string filen = "/proc/"+procs[i]+"/cmdline";
    FILE *f=fopen(filen.c_str(),"r");
    if (f == 0) continue;
    size_t cnt = fread(line,1,sizeof(line),f);
    fclose(f);
    if (cnt == 0) continue;
    cmdline.assign(line,cnt);
    for (unsigned int j=0;j<cmdline.size();j++)
    {
      if (cmdline[j]==0) cmdline[j] = ' ';
    }
    if (cmdline.find("tmSrv") == string::npos) continue;
    if (cmdline.find(nam) == string::npos) continue;
    printf ("tmSrv for %s found at PID %s\n",nam,procs[i].c_str());
    int tmSrvPid;
    sscanf(procs[i].c_str(),"%d",&tmSrvPid);
    return tmSrvPid;
  }
  printf("tmSrv for %s not found\n",nam);
  return -1;
}

/*
 * connect to netlink
 * returns netlink socket, or -1 on error
 */
static int nl_connect()
{
    int rc;
    int nl_sock;
    struct sockaddr_nl sa_nl;

    nl_sock = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_CONNECTOR);
    if (nl_sock == -1) {
        perror("socket");
        return -1;
    }

    sa_nl.nl_family = AF_NETLINK;
    sa_nl.nl_groups = CN_IDX_PROC;
    sa_nl.nl_pid = getpid();

    rc = bind(nl_sock, (struct sockaddr *)&sa_nl, sizeof(sa_nl));
    if (rc == -1) {
        perror("bind");
        close(nl_sock);
        return -1;
    }
    return nl_sock;
}


namespace   {
  struct __attribute__ ((aligned(NLMSG_ALIGNTO))) NLCN_MSG_MCAST {
    struct nlmsghdr nl_hdr;
    struct __attribute__ ((__packed__)) payload {
      struct cn_msg cn_msg;
      enum proc_cn_mcast_op cn_mcast;
    } data;
  };
  struct __attribute__ ((aligned(NLMSG_ALIGNTO))) NLCN_MSG_PROCEV {
    struct nlmsghdr nl_hdr;
    struct __attribute__ ((__packed__)) payload {
      struct cn_msg cn_msg;
      struct proc_event proc_ev;
    } data;
  };
}

/*
 * subscribe on proc events (process notifications)
 */
static int set_proc_ev_listen(int nl_sock, bool enable)
{
    int rc;
    NLCN_MSG_MCAST nlcn_msg;
    memset(&nlcn_msg, 0, sizeof(nlcn_msg));
    nlcn_msg.nl_hdr.nlmsg_len = sizeof(nlcn_msg);
    nlcn_msg.nl_hdr.nlmsg_pid = getpid();
    nlcn_msg.nl_hdr.nlmsg_type = NLMSG_DONE;

    nlcn_msg.data.cn_msg.id.idx = CN_IDX_PROC;
    nlcn_msg.data.cn_msg.id.val = CN_VAL_PROC;
    nlcn_msg.data.cn_msg.len = sizeof(enum proc_cn_mcast_op);

    nlcn_msg.data.cn_mcast = enable ? PROC_CN_MCAST_LISTEN : PROC_CN_MCAST_IGNORE;

    rc = send(nl_sock, &nlcn_msg, sizeof(nlcn_msg), 0);
    if (rc == -1) {
        perror("netlink send");
        return -1;
    }

    return 0;
}
static void on_sigint(int )
{
    need_exit = true;
}

/*
 * handle a single process event
 */
void *handle_proc_ev(void *arg)
{
  int nl_sock = *(int*)arg;
    int rc;
    NLCN_MSG_PROCEV nlcn_msg;
    signal(SIGINT, &on_sigint);
    siginterrupt(SIGINT, true);
    pthread_mutex_lock(&mtx);
    pthread_mutex_unlock(&mtx);
    while (!need_exit)
    {
      rc = recv(nl_sock, &nlcn_msg, sizeof(nlcn_msg), 0);
      pthread_mutex_lock(&mtx);
      if (rc == 0)
      {
          /* shutdown? */
          pthread_mutex_unlock(&mtx);
          return 0;
      }
      else if (rc == -1)
      {
          if (errno == EINTR) continue;
          perror("netlink recv");
          pthread_mutex_unlock(&mtx);
          return (void*)-1;
      }
      switch (nlcn_msg.data.proc_ev.what)
      {
        case proc_event::PROC_EVENT_NONE:
        {
            printf("set mcast listen ok\n");
            break;
        }
        case proc_event::PROC_EVENT_FORK:
        {
          int ppid = nlcn_msg.data.proc_ev.event_data.fork.parent_tgid;
          int pid = nlcn_msg.data.proc_ev.event_data.fork.child_tgid;
          int tid = nlcn_msg.data.proc_ev.event_data.fork.child_pid;
          int typ = TNODE_PROCESS;
          if (tid != pid)
          {
            ppid = pid;
            pid = tid;
            typ = TNODE_THREAD;
          }
//              printf("fork: parent tid=%d pid=%d (%d) -> child tid=%d pid=%d\n",
//                      nlcn_msg.data.proc_ev.event_data.fork.parent_pid,
//                      nlcn_msg.data.proc_ev.event_data.fork.parent_tgid,tmSrv_PID,
//                      nlcn_msg.data.proc_ev.event_data.fork.child_pid,
//                      nlcn_msg.data.proc_ev.event_data.fork.child_tgid);
          if (Taskmap.find(ppid) == Taskmap.end()) break;
//          printf("Fork++++++++: TID %d PID %d (PPID %d)\n",tid,pid,ppid);
          TNode *n=new TNode(pid,ppid);
          n->type = typ;
//          printf("Fork-------: TID %d PID %d (PPID %d)\n",tid, pid,ppid);
//          TNode *tms = Taskmap[tmSrv_PID];
//          char b[4096];
//          tms->Dump(b);
//          printf("%s\n",b);
          break;
        }
        case proc_event::PROC_EVENT_EXEC:
        {
          int pid = nlcn_msg.data.proc_ev.event_data.exec.process_pid;
          NodeMap::iterator it=Taskmap.find(pid);
          if ( it == Taskmap.end())break;
//          printf("Exec+++++++++: PID %d \n",pid);
          TNode *t = (*it).second;
          t->setCmdline();
//          printf("Exec--------: PID %d (cmdline %s)\n",t->Pid,t->cmdline.c_str());
//          TNode *tms = Taskmap[tmSrv_PID];
//          char b[8*8192];
//          int siz=tms->Dump(b,true,false,0,char(2));
//          b[siz]=0;siz++;
//          StatusService->updateService(b,siz);
//          printf("%s\n",b);
          break;
        }
        case proc_event::PROC_EVENT_UID:
        {
//                printf("uid change: tid=%d pid=%d from %d to %d\n",
//                        nlcn_msg.data.proc_ev.event_data.id.process_pid,
//                        nlcn_msg.data.proc_ev.event_data.id.process_tgid,
//                        nlcn_msg.data.proc_ev.event_data.id.r.ruid,
//                        nlcn_msg.data.proc_ev.event_data.id.e.euid);
            break;
        }
        case proc_event::PROC_EVENT_GID:
        {
//                printf("gid change: tid=%d pid=%d from %d to %d\n",
//                        nlcn_msg.data.proc_ev.event_data.id.process_pid,
//                        nlcn_msg.data.proc_ev.event_data.id.process_tgid,
//                        nlcn_msg.data.proc_ev.event_data.id.r.rgid,
//                        nlcn_msg.data.proc_ev.event_data.id.e.egid);
            break;
        }
        case proc_event::PROC_EVENT_EXIT:
        {
          int pid = nlcn_msg.data.proc_ev.event_data.exit.process_tgid;
          int tid = nlcn_msg.data.proc_ev.event_data.exit.process_pid;
          NodeMap::iterator it=Taskmap.find(tid);
          if (pid == tmSrv_PID)
          {
            need_exit=true;
            break;
          }
          if ( it == Taskmap.end())break;
          TNode *t = (*it).second;
//          printf("---------------EXIT: TID %d, pid=%d exit code: %d cmdline: %s\n",tid,pid,nlcn_msg.data.proc_ev.event_data.exit.exit_code,t->cmdline.c_str());
          TNode *p = t->Parent;
          if (p == 0)
          {
            printf("LOGIC ERROR***** Parent pointer 0\n");
            it = Taskmap.find(t->PPid);
            if ( it == Taskmap.end())
            {
              printf("Parent TNode for Pid %d does not exist...\n", t->PPid);
              break;
            }
            else
            {
              p = (*it).second;
            }
          }
          p->removeChild(*t);
//          TNode *tms = Taskmap[tmSrv_PID];
//          char b[8*8192];
//          *b=0;
//          int siz=tms->Dump(b,true,false,0,char(2));
//          b[siz]=0;siz++;
//          StatusService->updateService(b,siz);
//          printf("%s\n",b);
          break;
        }
        default:
        {
	  //printf("unhandled proc event\n");
            break;
        }
      }
      pthread_mutex_unlock(&mtx);
    }
    pthread_mutex_unlock(&mtx);
    return 0;
}

int getPPID(int pid, time_t *st)
{
  char fnam[128];
  char stat[4096];
  sprintf(fnam,"/proc/%d/stat",pid);
  FILE *f = fopen(fnam,"r");
  if (f !=0)
  {
    fgets(stat,sizeof(stat),f);
    fclose(f);
    int ppid;
    dyn_string &dstatl = *Strsplit(stat," ");
    sscanf(dstatl[3].c_str(),"%d",&ppid);
    sscanf(dstatl[21].c_str(),"%ld",st);
    delete &dstatl;
    return ppid;
  }
  else
  {
    return -1;
  }
}

#include "pthread.h"
//#include "tree.hh"
//typedef tree<int>::iterator triterator;
//multimap<int,tree<int>*> PPidmap;
//typedef multimap<int,tree<int>*>::iterator mmapit;
set<int> AllProcs;
set<int> AllParents;
vector<int> iprocs;
vector<int> ppids;


int main(int argc, char**argv)
{
  char Empty[1];
  *Empty = 0;
  int nl_sock;
  pthread_t thID;
  vector<string> procs;
  BootTime = get_Boot_Time();
  string ServiceName="DiracStatus";
//  mmapit mit;
  if (argc >1)
  {
    if (strcmp(argv[1],"-p") == 0)
    {
      sscanf(argv[2],"%d",&tmSrv_PID);
    }
    else if (strcmp(argv[1],"-n")==0)
    {
      tmSrv_PID = find_tmsrv(argv[2]);
    }
    ServiceName = argv[3];
  }
  else
  {
    tmSrv_PID = find_tmsrv((char*)"lhcbprod");
  }
//  curr_agents_pids.insert(tmSrv_PID);
  int rc = EXIT_SUCCESS;


  nl_sock = nl_connect();
  if (nl_sock == -1)
      exit(EXIT_FAILURE);

  rc = set_proc_ev_listen(nl_sock, true);
  if (rc == -1) {
      rc = EXIT_FAILURE;
      close(nl_sock);
      exit(rc);
//      goto out;
  }
//  int status;
  pthread_mutex_lock(&mtx);
  pthread_create(&thID,0,&handle_proc_ev,(void*)&nl_sock);
  procs.clear();
  ReadDir("/proc",procs);
//  Agent_Tree.insert(tmSrv_PID);
  for (size_t i=0;i<procs.size();i++)
  {
    int ip;
    sscanf(procs[i].c_str(),"%d",&ip);
    iprocs.push_back(ip);
    AllProcs.insert(ip);
    time_t starttime;
    int Ppid = getPPID(ip,&starttime);
    TNode *nod;
    if (Taskmap.find(ip) == Taskmap.end())
    {
      nod = new TNode(ip,Ppid);
      nod->starttime= BootTime+starttime;
    }
    else
    {
      nod = Taskmap[ip];
      nod->starttime= BootTime+starttime;
    }
    nod->AddThreads();
    nod->setCmdline();
    if (Ppid == -1) continue;
    AllParents.insert(Ppid);
    ppids.push_back(Ppid);
  }
  char b[8*8192];
  char b1[8*8192];
  TNode *tm=Taskmap[tmSrv_PID];
//  TNode *root=Taskmap[0];
  printf("%d\n",(int)Taskmap.size());
  Taskmap.clear();
  tm->fillMap(Taskmap);
  tm->Print(leads,true);
  *b=0;
  int s = tm->Dump(b,true,false,0,char(1),1);
  b[s] = 0;
  s++;
  string serverName = RTL::nodeNameShort()+"_"+ServiceName;
  StatusServer = new DimServer();
  StatusServer->autoStartOn();
  StatusServer->start(serverName.c_str());
  StatusService = new DimService(serverName.c_str(),"C",Empty,sizeof(Empty));
  StatusService->updateService(b,s);
//  printf("%s\n",b);
  pthread_mutex_unlock(&mtx);
  while (!need_exit)
  {
    usleep(100000);
    TNode *tms = Taskmap[tmSrv_PID];
    pthread_mutex_lock(&mtx);
    int siz=tms->Dump(b1,true,false,0,char(1),1);
    pthread_mutex_unlock(&mtx);
    b1[siz]=0;siz++;
    if (strcmp(b,b1) != 0)
    {
      StatusService->updateService(b1,siz);
      memcpy(b, b1, siz);
    }
    int tstat = pthread_tryjoin_np(thID,0);
    if (tstat != EBUSY)
    {
      exit(0);
    }
  }
  pthread_join(thID,0);
  set_proc_ev_listen(nl_sock, false);
  return 0;
}
