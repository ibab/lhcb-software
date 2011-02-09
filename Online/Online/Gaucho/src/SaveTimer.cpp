#include "Gaucho/SaveTimer.h"
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

SaveTimer::SaveTimer(HistAdder *add, int period) : GenTimer((void*)add,period*1000,TIMER_TYPE_PERIODIC)
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

}

SaveTimer::~SaveTimer( )
{
}
void SaveTimer::timerHandler ( void )
{
  if (m_Adder->m_inputServicemap.empty()) return;
  m_Adder->Lock();
  if (m_Adder->m_usedSize == 0)
  {
    m_Adder->UnLock();
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
  SavetoFile(m_buffadd);
}
void SaveTimer::Stop()
{
  GenTimer::Stop();
}
void SaveTimer::SavetoFile(void *buff)
{
  SerialHeader *hd = (SerialHeader *)buff;
  char fn[1024];
  char fdir[1024];
  void *bend = AddPtr(buff,hd->buffersize);
  int runo = (int)hd->run_number;
  buff = AddPtr(buff,sizeof(SerialHeader));
  fdir[0]=0;
  fn[0]=0;
  if (!m_EOR)
  {
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
    sprintf(fn,"%s/%s-%d-%4d%02d%02dT%02d%02d%02d.root",fdir,m_taskname.c_str(),runo,
        tstruct->tm_year+1900,tstruct->tm_mon+1,tstruct->tm_mday,
        tstruct->tm_hour,tstruct->tm_min,tstruct->tm_sec);
//    printf("File Saver: Filename %s\n",fn);
  }
  else
  {
    sprintf(fdir,"%s",m_rootdir.c_str());
    mkdir(fdir,01777);
    sprintf(fdir,"%s/%s",fdir,m_taskname.c_str());
    mkdir(fdir,01777);
    sprintf(fdir,"%s/%d",fdir,(runo/10000)*10000);
    mkdir(fdir,01777);
    sprintf(fdir,"%s/%d",fdir,(runo/1000)*1000);
    mkdir(fdir,01777);
    sprintf(fn,"%s/%s-run%d.root",fdir,m_taskname.c_str(),runo);
//    printf("File Saver: Filename %s\n",fn);
  }
  m_Adder->Lock();
  TFile *f = new TFile(fn,"RECREATE");
  m_Adder->UnLock();
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
        m_Adder->Lock();
        r = (TH1*)(h.de_serialize(buff));
        m_Adder->UnLock();
//        TProfile *tp = (TProfile*)r;
//        TH1D *th1 = (TH1D*)r;
//        TH2D *th2 = (TH2D*)r;
        TProfile *tp;
        TH1D *th1;
        TH2D *th2;
        char hnam[1024];
        char *tok;
        char *ptok;
        int ntok;
        ntok = 0;
        char *ctxt;
        strcpy(hnam,r->GetName());
        tok = strtok_r(hnam,"/",&ctxt);
        while (tok != 0)
        {
          ptok = tok;
          ntok++;
          tok = strtok_r(0,"/",&ctxt);
        }
        strcpy(hnam,r->GetName());
        tok = strtok_r(hnam,"/",&ctxt);
        gDirectory->Cd("/");
        for (int i=0;i<ntok-1;i++)
        {
          TKey *k;
          k = gDirectory->GetKey(tok);
          if (k == 0)
          {
            gDirectory->mkdir(tok);
          }
          gDirectory->Cd(tok);
          tok = strtok_r(0,"/",&ctxt);
        }
//        r->SetName(ptok);
        switch(b->type)
        {
          case H_1DIM:
          {
            m_Adder->Lock();
            th1 = (TH1D*)r->Clone(ptok);
            th1->Write(ptok);
            m_Adder->UnLock();
            break;
          }
          case H_2DIM:
          {
            m_Adder->Lock();
            th2=(TH2D*)r->Clone(ptok);
            th2->Write(ptok);
            m_Adder->UnLock();
            break;
          }
          case H_PROFILE:
          case H_RATE:
          {
            m_Adder->Lock();
            tp = (TProfile*)r->Clone(ptok);
            tp->Write(ptok);
            m_Adder->UnLock();
            break;
          }
        }
        m_Adder->Lock();
        h.FreeDeser();
        m_Adder->UnLock();
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
  m_Adder->Lock();
  f->Close();
  m_Adder->UnLock();
}
