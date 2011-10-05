#include "Gaucho/HistAdder.h"
#include "Gaucho/MonHist.h"
#include "TH1D.h"
#include "TProfile.h"
#include "TH2D.h"
#include "TH3D.h"
#include "TPad.h"
#include "TCanvas.h"
TCanvas *canv;
static int nCB=0;

void CycleFn(void * ,void *,int , MonMap *hm, MonAdder *)
{
  printf("=====================================Cycle CallBack Function\n");
  int j=0;
  if ((nCB % 10) != 0)
  {
    nCB++;
    return;
  }
  nCB++;
  for (MonIter i=hm->begin();i!=hm->end();i++)
   {
      j++;
//      printf("%s ",i->first.c_str());
      DimBuffBase *p=(DimBuffBase*)i->second;
      char *nam =(char*)AddPtr(p,p->nameoff);
      char *stype=0;
      switch (p->type)
      {
        case H_1DIM:
        {
          stype = (char*)"H_1DIM";
          break;
        }
        case H_PROFILE:
        {
          stype = (char*)"H_PROFILE";
          break;
        }
        case H_RATE:
        {
          stype = (char*)"H_RATE";
          break;
        }
        case H_2DIM:
        {
          stype = (char*)"H_2DIM";
          break;
        }
        case H_3DIM:
        {
          stype = (char*)"H_3DIM";
          break;
        }
        case C_STATENT:
        {
          stype = (char*)"C_STATENT";
          break;
        }
      }
      MonHist h;
      TObject *ro;
      ro = (TObject*)MonHist::de_serialize(i->second);
      bool draw=((j==5) || (j==7) || (j==13));
      switch (p->type)
      {
        case H_1DIM:
        case H_PROFILE:
        case H_RATE:
        {
          DimHistbuff1 *p=(DimHistbuff1*)i->second;
          printf("\t\t%s %s. Dimension %d #bins %d Nentries %lli\n",stype,nam, p->dim,p->nxbin,(long long)p->nentries);
          if ((MONTYPE)p->type == H_1DIM)
          {
            TH1D *r=(TH1D *)ro;
            if (draw) r->Draw();
          }
          else
          {
            TProfile *r=(TProfile *)ro;
            if (draw) r->Draw();

          }
          break;
        }
        case H_2DIM:
        {
          DimHistbuff2 *p=(DimHistbuff2*)i->second;
          printf("\t\t%s %s. Dimension %d #x-bins %d #y-bins %d Nentries %lli\n",stype,nam,p->dim,p->nxbin,p->nybin,(long long)p->nentries);
          TH2D *r=(TH2D *)ro;
          if (draw) r->Draw();
          break;
        }
        case H_3DIM:
        {
          DimHistbuff3 *p=(DimHistbuff3*)i->second;
          printf("\t\t%s %s. Dimension %d #x-bins %d #y-bins %d Nentries %lli\n",stype,nam,p->dim,p->nxbin,p->nybin,(long long)p->nentries);
          TH3D *r=(TH3D *)ro;
          if (draw) r->Draw();
          break;
        }
        case C_STATENT:
        {
          DimStatBuff *p=(DimStatBuff*)i->second;
          printf("\t\t%s %s. Nentries %lli\n",stype,nam, (long long)p->nentries);
          break;
        }
      }
   }
  canv->Update();
}
#include "TROOT.h"
#include "TApplication.h"
int main (int argc, char *argv[])
{
  TH1D::SetDefaultSumw2();
  TH2D::SetDefaultSumw2();
  TApplication *app=new TApplication("Shit",&argc,argv);
  canv =  gROOT->MakeDefCanvas();
  gROOT->Draw();
  canv->Show();
  canv->Draw();
  char *nam = (char*)"AAABBB";
  if (argc > 1)
  {
    nam = argv[1];
  }
  HistAdder *a=new HistAdder((char*)"Adder",(char*)"Histos/Data");
  a->SetCycleFn(&CycleFn,0);
  app->Run();
  sleep(1000000000);
  return 0;
}
