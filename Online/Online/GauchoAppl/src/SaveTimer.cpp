#include "GauchoAppl/SaveTimer.h"
//#include "Gaucho/MonSubSys.h"
#include "Gaucho/ObjService.h"
#include "stdio.h"
#include "TFile.h"
#include "TROOT.h"
#include "TThread.h"
#include "Gaucho/SerialHeader.h"
#include "Gaucho/MonHist.h"
#include "sys/time.h"
#include "sys/stat.h"
#include "sys/types.h"
#include "Gaucho/RootHists.h"
#include "Gaucho/Utilities.h"
#include "RTL/Lock.h"

SaveTimer::SaveTimer(MonAdder *add, int period) : GenTimer((void*)add,period*1000,TIMER_TYPE_PERIODIC)
{
  m_Adder = add;
  m_dueTime = 0;
  m_buffadd = 0;
  m_bsiz = 0;
  m_EOR=false;
//  printf("==================SaveTimer Constructor.................%llu\n",m_period);
  TH1D::SetDefaultSumw2();
  TH2D::SetDefaultSumw2();
  TProfile::SetDefaultSumw2();
  m_filenamesvc =0;
  m_dontdimlock = true;
  ROOT::GetROOT();
  if (!TThread::IsInitialized())
  {
    TThread::Initialize();
  }
}

SaveTimer::~SaveTimer( )
{
  deletePtr(m_filenamesvc);
}
void SaveTimer::timerHandler ( void )
{
  if (m_Adder->m_inputServicemap.empty()) return;
  m_Adder->Lock();
//  if (m_Adder->m_lockid != 0)
//  {
//    RTL::Lock alock(m_Adder->m_lockid);
    if (m_Adder->m_usedSize == 0)
    {
      return;
    }
    if (m_bsiz < m_Adder->m_usedSize)
    {
      if (m_buffadd != 0)
      {
        free (m_buffadd);
      }
      m_buffadd = malloc(m_Adder->m_usedSize);
      m_bsiz = m_Adder->m_usedSize;
    }
    memcpy(m_buffadd,m_Adder->m_buffer,m_bsiz);
    m_Adder->UnLock();
//  }
//  else
//  {
//    printf("ERROR !!!!!!!!!!!!!!!!!!!! Bad Logic... running a SaveTimer without an adder lock...\n");
//    return;
//  }
  SavetoFile(m_buffadd);
}
//void SaveTimer::Stop()
//{
//  GenTimer::Stop();
//}
void SaveTimer::setSaveSetService()
{

  m_infoFileStatus = "SAVESETLOCATION/......................................................";
  std::string infoName = m_partname+"/"+m_taskname+"/SAVESETLOCATION";
  if (m_filenamesvc == 0)
  {
    m_filenamesvc = new DimService(infoName.c_str(),(char*)m_infoFileStatus.c_str());
  }
}
void SaveTimer::SavetoFile(void *buff)
{
  SerialHeader *hd = (SerialHeader *)buff;
  char fn[1024];
  char fdir[1024];
  void *bend = AddPtr(buff,hd->buffersize);
  int runo = (int)hd->run_number;
  static BRTLLock *loclock = 0;
  if (loclock == 0)
  {
    loclock = new BRTLLock();
    loclock->m_name = "RootSaverHackLock";
  }
  if (m_EOR && (runo == 0))
  {
//    printf("Run Number == 0. No attempt to save....\n");
    return;
  }

  buff = AddPtr(buff,sizeof(SerialHeader));
  fdir[0]=0;
  fn[0]=0;
    timeval ctim;
    struct tm *tstruct;
    gettimeofday(&ctim,NULL);
    tstruct = localtime(&ctim.tv_sec);
    sprintf(fdir,"%s",m_rootdir.c_str());
    mkdir(fdir,01777);
    sprintf(fdir,"%s/%4d",fdir,tstruct->tm_year+1900);
    mkdir(fdir,01777);
    if (m_partname != "")
    {
      sprintf(fdir,"%s/%s",fdir,m_partname.c_str());
      mkdir(fdir,01777);
    }
    sprintf(fdir,"%s/%s",fdir,m_taskname.c_str());
    mkdir(fdir,01777);
    sprintf(fdir,"%s/%02d",fdir,tstruct->tm_mon+1);
    mkdir(fdir,01777);
    sprintf(fdir,"%s/%02d",fdir,tstruct->tm_mday);
    mkdir(fdir,01777);
    if (!m_EOR)
    {
      sprintf(fn,"%s/%s-%d-%4d%02d%02dT%02d%02d%02d.root",fdir,m_taskname.c_str(),runo,
          tstruct->tm_year+1900,tstruct->tm_mon+1,tstruct->tm_mday,
          tstruct->tm_hour,tstruct->tm_min,tstruct->tm_sec);
    }
    else
    {
      sprintf(fn,"%s/%s-%d-%4d%02d%02dT%02d%02d%02d-EOR.root",fdir,m_taskname.c_str(),runo,
          tstruct->tm_year+1900,tstruct->tm_mon+1,tstruct->tm_mday,
          tstruct->tm_hour,tstruct->tm_min,tstruct->tm_sec);
    }

  printf("File Saver: Filename %s\n",fn);
  loclock->lockMutex();
  m_Adder->Lock();
  TFile *f = TFile::Open(fn,"RECREATE");
  m_Adder->UnLock();
  if (!f || f->IsZombie())
  {
    printf("Root File %s cannot be opened or is Zombie\n",fn);
    fflush (stdout);
    loclock->unlockMutex();
    delete f;
    return;
  }
  TH1 *r;
  MonHist h;
  Bool_t dirstat = TH1::AddDirectoryStatus();
  TH1::AddDirectory(kFALSE);
  DimBuffBase *prevb = 0;
  while (buff <bend)
  {
    DimBuffBase *b = (DimBuffBase*)buff;
    if (b->reclen == 0)
    {
      printf("Bad Record. Record Length 0!!! Previous record %0X! \n",prevb);
    }
    switch (b->type)
    {
      case H_1DIM:
      case H_2DIM:
      case H_3DIM:
      case H_PROFILE:
      case H_RATE:
      case H_2DPROFILE:
      {
        dyn_string *hname;
        m_Adder->Lock();
        r = (TH1*)(h.de_serialize(buff));
        m_Adder->UnLock();
        char hnam[4096];
        hname = Strsplit(r->GetName(),"/");
        strcpy(hnam,r->GetName());
//        fprintf(((HistAdder*)m_Adder)->logFile,"==========>[Error] Saving histogram with name '%s'\n",hnam);
        gDirectory->Cd("/");
        for (unsigned int i=0;i<hname->size()-1;i++)
        {
          TKey *k;
          k = gDirectory->GetKey(hname->at(i).c_str());
          if (k == 0)
          {
            gDirectory->mkdir(hname->at(i).c_str());
          }
          gDirectory->Cd(hname->at(i).c_str());
        }
//        r->SetName(ptok);
        switch(b->type)
        {
          case H_1DIM:
          case H_2DIM:
          case H_3DIM:
          case H_PROFILE:
          case H_2DPROFILE:
          case H_RATE:
          {
            m_Adder->Lock();
            r->Write(hname->at(hname->size()-1).c_str());
            m_Adder->UnLock();
            break;
          }
        }
        m_Adder->Lock();
        deletePtr(r);
        h.FreeDeser();
        m_Adder->UnLock();
//        r->Write(ptok);
        prevb = b;
        buff = AddPtr(buff,b->reclen);
        continue;
      }
      case H_ILLEGAL:
      case C_INT:
      case C_LONGLONG:
      case C_FLOAT:
      case C_DOUBLE:
      case C_STATENT:
      {
        buff = AddPtr(buff,b->reclen);
        continue;
      }
    }
  }
//  fflush(((HistAdder*)m_Adder)->logFile);
  TH1::AddDirectory(dirstat);
  m_Adder->Lock();
  f->Close();
  delete f;
  m_Adder->UnLock();
  loclock->unlockMutex();
  if (m_filenamesvc != 0) m_filenamesvc->updateService(fn);
}
