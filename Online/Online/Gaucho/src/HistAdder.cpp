#include "Gaucho/HistAdder.h"
#include "Gaucho/ObjService.h"
#include "Gaucho/MonServer.h"
#include "Gaucho/ObjRPC.h"
#include "Gaucho/MonHist.h"
#include "Gaucho/RootHists.h"
#include "Gaucho/RateService.h"
#include "dis.hxx"
#include "TDirectory.h"
#include <vector>
#include "string.h"
#include "Gaucho/Utilities.h"
#include "Gaucho/AdderSys.h"
typedef std::pair<std::string, void*> HistPair;

HistAdder::HistAdder(char *myName, char *serviceName)
{
  m_serviceName = std::string("/Histos/") + serviceName;
  m_MyName      = myName;
  m_type        = ADD_HISTO;
  m_oldProf     = 0;
  m_updated = false;
  AdderSys::Instance().gg_AdderList.push_back(this);
}

HistAdder::~HistAdder()
{
  Adderlist_t::iterator g=std::find( AdderSys::Instance().gg_AdderList.begin(),AdderSys::Instance().gg_AdderList.end(),this);
  if (g != AdderSys::Instance().gg_AdderList.end())
  {
    AdderSys::Instance().gg_AdderList.erase(g);
  }
}

void HistAdder::add(void *buff, int siz, MonInfo *h)
{
    addBuffer(buff,siz,h);
}
void HistAdder::addBuffer(void *buff, int siz,MonInfo *)
{
//    //printf("Adding %s\n",h->m_TargetService.c_str());
  MonMap hmap;
  hmap.clear();
  void *bend = AddPtr(buff,siz);
  void *hstart = AddPtr(buff,sizeof(SerialHeader));
  DimHistbuff1 *pp = (DimHistbuff1*)hstart;
  while (pp<bend)
  {
    char *nam = (char*)AddPtr(pp,pp->nameoff);
    hmap.insert(HistPair(std::string(nam),pp));
    if (pp->reclen == 0) break;
    pp=(DimHistbuff1*)AddPtr(pp,pp->reclen);
  }
  for (MonIter i=hmap.begin();i!=hmap.end();i++)
  {
    MonIter j = m_hmap.find(i->first);
    if (j!=m_hmap.end())
    {
      //printf("HistAdder Locking MAP\n");
      LockMap();
      double *ps, *ph;
      DimHistbuff1 *sumh = (DimHistbuff1*)(j->second);
      DimHistbuff1 *srch = (DimHistbuff1*)(i->second);
      ps = (double*)AddPtr(sumh,sumh->dataoff);
      ph = (double*)AddPtr(srch,srch->dataoff);
      if (srch->type == (int)C_STATENT)
      {
#define MIN(X,Y) ((X) < (Y) ?  (X) : (Y))
#define MAX(X,Y) ((X) > (Y) ?  (X) : (Y))
        DimStatBuff *s=(DimStatBuff*)srch;
        DimStatBuff *d=(DimStatBuff*)sumh;
        d->nentries += s->nentries;
        d->m_sumw += s->m_sumw;
        d->m_sumw2 += s->m_sumw2;
        d->m_min = MIN(d->m_min,s->m_min);
        d->m_min = MAX(d->m_max,s->m_max);
        //printf("HistAdder UNLocking MAP\n");
        UnLockMap();
        continue;
      }
      sumh->nentries += srch->nentries;
      sumh->m_sumw  += srch->m_sumw;
      sumh->m_sumw2 += srch->m_sumw2;
      sumh->m_sumwx += srch->m_sumwx;
      sumh->m_sumwx2  += srch->m_sumwx2;
      sumh->m_sumwx3  += srch->m_sumwx3;
      sumh->m_sumwx4  += srch->m_sumwx4;
      sumh->m_sumwy += srch->m_sumwy;
      sumh->m_sumwy2  += srch->m_sumwy2;
      sumh->m_sumwy3  += srch->m_sumwy3;
      sumh->m_sumwy4  += srch->m_sumwy4;
      ////printf("Histogram %s Number of Entries %f\n",(char*)AddPtr(sumh,sumh->nameoff),sumh->nentries);
      int ndble;
      switch(srch->type)
      {
      case H_1DIM:
        {
          ndble = 2*(srch->nxbin+2);
          break;
        }
      case H_2DIM:
        {
          //2*(m_nx+2)*(m_ny+2)
          DimHistbuff2 *s = (DimHistbuff2*)srch;
          ndble = 2*(s->nxbin+2)*(s->nybin+2);
          break;
        }
      case H_PROFILE:
      case H_RATE:
        {
          ndble = 4*(srch->nxbin+2);
          break;
        }
      default:
        {
          ndble = 0;
          break;
        }
      }
      int icpy;
      switch(srch->type)
      {
      case H_RATE:
        {
          m_RateBuff = sumh;
          for (icpy=0;icpy<5;icpy++)
          {
            ps[icpy]+=ph[icpy];
          }
          for (icpy=8;icpy<ndble;icpy++)
          {
            ps[icpy]+=ph[icpy];
          }
          break;
        }
      default:
        {
          for (icpy=0;icpy<ndble;icpy++)
          {
            ps[icpy]+=ph[icpy];
          }
          break;
        }
      }
      //printf("HistAdder UNLocking MAP\n");
      UnLockMap();
    }
    else
    {
//        //printf("New Histogram... re-allocating...\n");
      int csiz = m_usedSize;
      int hsiz;
      DimHistbuff1 *srch = (DimHistbuff1*)(i->second);
      void *p;
      hsiz = srch->reclen;
      //printf("HistAdder Locking MAP\n");
      LockMap();
      p = ReAllocate(hsiz);
      if (p!=0)
      {
        p = AddPtr(p,csiz);
        memcpy(p,srch,hsiz);
        m_usedSize += hsiz;
        char *nam = (char*)AddPtr(srch,srch->nameoff);
        m_hmap.insert(HistPair(std::string(nam),p));
      }
      //printf("HistAdder UNLocking MAP\n");
      UnLockMap();
    }
  }
  m_received++;
  m_added++;
  ////printf ("New Source %s... %d\n",h->m_TargetService.c_str(),m_received);
}

void HistAdder::Update()
{
  if (m_received >= m_expected)
  {
    this->m_timer->Stop();
//    printf("%s Finished one cycle. Updating our service... \n", this->m_MyName.c_str());
//    fflush(stdout);
    if (m_isSaver)
    {
      if (m_locked)
      {
        //printf("HistAdder UNLocking\n");
        UnLock();
      }
      m_locked = false;
    }
    m_added = 0;
    m_received = 0;
    ////printf(" %d %d\n", m_received,m_m_expected);
    if (m_outservice != 0)
    {
      m_outservice->Serialize();
      m_outservice->Update();
      m_updated = true;
    }
    if (m_expandRate)
    {
#define MAX_SERVICE_SIZE 128
      std::string nnam;
      if (m_RateBuff != 0)
      {
        DimHistbuff1 *pdiff;
        DimHistbuff1 *RateBuff=(DimHistbuff1*)m_RateBuff;
        DimHistbuff1 *oldProf = (DimHistbuff1*)m_oldProf;
        if (m_oldProf !=0)
        {
          int ndble = (RateBuff->nxbin+2);
          pdiff = (DimHistbuff1*)malloc(RateBuff->reclen);
          double *obincont  = (double*)AddPtr(AddPtr(oldProf,oldProf->dataoff),0*(oldProf->nxbin+2)*sizeof(double));
          double *nbincont  = (double*)AddPtr(AddPtr(RateBuff,RateBuff->dataoff),0*(RateBuff->nxbin+2)*sizeof(double));
          double *dbincont  = (double*)AddPtr(AddPtr(pdiff,RateBuff->dataoff),0*(RateBuff->nxbin+2)*sizeof(double));
          double *ocont  = (double*)AddPtr(AddPtr(oldProf,oldProf->dataoff),1*(oldProf->nxbin+2)*sizeof(double));
          double *ncont  = (double*)AddPtr(AddPtr(RateBuff,RateBuff->dataoff),1*(RateBuff->nxbin+2)*sizeof(double));
          double *dcont  = (double*)AddPtr(AddPtr(pdiff,RateBuff->dataoff),1*(RateBuff->nxbin+2)*sizeof(double));
          std::vector<char*> labvec;
          labvec.clear();
          labvec.push_back(0);
          {
            int i;
            int nbin=oldProf->nxbin;
            char *labs = (char*)AddPtr(oldProf,oldProf->xlaboff);
            char *lab = labs;
            for (i = 1; i < (nbin+1) ; ++i)
            {
              labvec.push_back(lab);
              lab = (char*)AddPtr(lab,strlen(lab)+1);
            }
          }
          for (int i = 9;i<ndble;i++)
          {
            dbincont[i] = nbincont[i]-obincont[i];
            dcont[i] = ncont[i]-ocont[i];
//            //printf("%d\t%f = %f - %f\n\t%f = %f - %f\n",i,dbincont[i], nbincont[i], obincont[i],dcont[i], ncont[i], ocont[i]);
          }
          OUTServiceDescr *outs;
          std::string nams;
          int i;
          int dim;
          double rate;
          double denom;
//          //printf("Time Bin %f\n",p->GetBinContent(8));
//          denom  = p->GetBinContent(8)/1.0e09;
//          denom = dbincont[8] == 0 ? 0 : dcont[8]/dbincont[8]/1.0e09;
          denom = ncont[8]/m_added/1.0e09;
          dim = RateBuff->nxbin;
          std::string sname;
          for (i=9;i<dim;i++)
          {
            nams  = labvec[i];
            outs = findOUTService(nams);
            RateService *s;
            if (outs != 0)
            {
              s = (RateService*)outs->m_svc;
            }
            else
            {
              DimServer::autoStartOn();
              sname = m_NamePrefix+nams;
              if (sname.size() < MAX_SERVICE_SIZE)
              {
                s= new RateService(sname.c_str(),(char*)"D:2;C");
                outs = new OUTServiceDescr((char*)nams.c_str(),(void*)s);
                this->m_outputServicemap.insert(OUTServicePair(nams,outs));
              }
              else
              {
                continue;
              }
            }
//            rate = dbincont[i] == 0 ? 0 : dcont[i]/dbincont[i]/denom;
            double c = dcont[i];
            rate = c/denom;
//            //printf("%d %d %s %f %f\n",dim,i,nams.c_str(), pdiff->GetBinContent(i),rate);
            s->m_data.d1 = rate;
            s->m_data.d2 = c;
            int upsiz = 2*sizeof(s->m_data.d1)+strlen(s->m_data.c);
            s->Updater(upsiz);
          }
          delete pdiff;
        }
        else
        {
          m_oldProf = (DimHistbuff1*)malloc(m_RateBuff->reclen);
        }
      }
      m_added = 0;
      memcpy(m_oldProf,m_RateBuff,m_RateBuff->reclen);
    }
    if (CycleFn!= 0)
    {
      (*CycleFn)(CycleCBarg, m_buffer, m_buffersize, &m_hmap, this);
    }
  }
}
