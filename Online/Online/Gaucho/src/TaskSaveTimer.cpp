//#include "Gaucho/MonSubSys.h"
#include "Gaucho/ObjService.h"
#include "stdio.h"
#include "TFile.h"
#include "Gaucho/SerialHeader.h"
#include "Gaucho/MonHist.h"
#include "sys/time.h"
#include "sys/stat.h"
#include "sys/types.h"
#include "Gaucho/RootHists.h"
#include "Gaucho/Utilities.h"
#include "Gaucho/dimhist.h"
#include "Gaucho/TaskSaveTimer.h"

TaskSaveTimer::TaskSaveTimer(MonSubSys *add, int period) : GenTimer((void*)add,period*1000,TIMER_TYPE_PERIODIC)
{
  m_subsys = add;
  m_dueTime = 0;
  m_buffadd = 0;
  m_bsiz = 0;
  m_EOR=false;
  m_prevbuff  = 0;
  m_prevsiz = 0;
  m_prevmap = new HISTMAP();
  m_prevmap->clear();
  TH1D::SetDefaultSumw2();
  TH2D::SetDefaultSumw2();
  TProfile::SetDefaultSumw2();
}

TaskSaveTimer::~TaskSaveTimer( )
{
  if (m_prevmap != 0)
  {
    delete m_prevmap;
    m_prevmap = 0;
  }
}

void TaskSaveTimer::timerHandler ( void )
{
  if ( m_subsys ) {
    // Need protection against m_subsys==0:
    // If the save time runs, but no histograms were
    // ever published, m_subsys (histograms) is NULL!
    m_subsys->Lock();
    if (m_subsys->m_ser->buffersize == 0)
    {
      m_subsys->unLock();
      return;
    }
    if (m_bsiz < m_subsys->m_ser->buffersize)
    {
      if (m_buffadd != 0)
      {
	free (m_buffadd);
      }
      m_buffadd = malloc(m_subsys->m_ser->buffersize);
      m_bsiz = m_subsys->m_ser->buffersize;
    }
    memcpy(m_buffadd,m_subsys->m_ser->buffer,m_bsiz);
    m_subsys->Clear();
    m_subsys->unLock();
    SavetoFile(m_buffadd);
  }
}

void TaskSaveTimer::Stop()
{
  GenTimer::Stop();
}

void TaskSaveTimer::SavetoFile(void *buff)
{
  SerialHeader *hd = (SerialHeader *)buff;
  if (hd->m_magic != SERIAL_MAGIC)
  {
    printf("ERROR: %s Bad Magic Word in buffer while saving to file\n",RTL::processName().c_str());
    return;
  }
  char fn[1024];
  char fdir[1024];
  void *bend = AddPtr(buff,hd->buffersize);
  int runo = (int)hd->run_number;
  if (runo == 0)
  {
    return;
  }

  buff = AddPtr(buff,sizeof(SerialHeader));
  fdir[0]=0;
  fn[0]=0;
//  if (!m_EOR)
//  {
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
  m_subsys->Lock();
  TFile *f = TFile::Open(fn,"RECREATE");
  m_subsys->unLock();
  if (!f || f->IsZombie())
  {
    printf("Root File %s cannot be opened or is Zombie\n",fn);
    fflush (stdout);
    return;
  }
  TH1 *r;
  MonHist h;
  while (buff <bend)
  {
    DimBuffBase *b = (DimBuffBase*)buff;
    switch (b->type)
    {
      case H_1DIM:
      case H_2DIM:
      case H_PROFILE:
      case H_RATE:
      {
        dyn_string *hname;
        m_subsys->Lock();
        r = (TH1*)(h.de_serialize(buff));
        m_subsys->unLock();
//        TProfile *tp = (TProfile*)r;
//        TH1D *th1 = (TH1D*)r;
//        TH2D *th2 = (TH2D*)r;
        TProfile *tp;
        TH1D *th1;
        TH2D *th2;
        char hnam[4096];
        int ntok;
        ntok = 0;
        hname = Strsplit(r->GetName(),"/");
        strcpy(hnam,r->GetName());
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
          {
            m_subsys->Lock();
            th1 = (TH1D*)r->Clone(hname->at(hname->size()-1).c_str());
            th1->Write(hname->at(hname->size()-1).c_str());
            m_subsys->unLock();
            break;
          }
          case H_2DIM:
          {
            m_subsys->Lock();
            th2=(TH2D*)r->Clone(hname->at(hname->size()-1).c_str());
            th2->Write(hname->at(hname->size()-1).c_str());
            m_subsys->unLock();
            break;
          }
          case H_PROFILE:
          case H_RATE:
          {
            m_subsys->Lock();
            tp = (TProfile*)r->Clone(hname->at(hname->size()-1).c_str());
            tp->Write(hname->at(hname->size()-1).c_str());
            m_subsys->unLock();
            break;
          }
        }
        m_subsys->Lock();
        h.FreeDeser();
        m_subsys->unLock();
//        r->Write(ptok);
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
  m_subsys->Lock();
  f->Close();
  m_subsys->unLock();
  if (m_filenamesvc != 0)
  {
    DimLock l;
    m_filenamesvc->updateService(fn);
  }
}
