#include "Gaucho/RPCRec.h"
#include "stdio.h"
#include "TCanvas.h"
#include "TROOT.h"
#include "TApplication.h"
#include "Gaucho/MonHist.h"
#include "TH1D.h"
#include "TProfile.h"
#include "TH2D.h"
#include "TTimer.h"
class MyTTimer : public TTimer
{

public:
  RPCCommRead *rrd;
  int rdlen;
  RPCRec *rec;
  MyTTimer(RPCRec* r, RPCCommRead *rdc, int rlen) :  TTimer(5000,true)
  {
    rec = r;
    rrd = rdc;
    rdlen = rlen;
  }

  Bool_t Notify()
  {
    printf("Timer Fired\n");
    rec->setData(rrd,rdlen);
    Start(5000,true);
    return true;
  };
};

TCanvas *canv;
std::vector <std::string*> hnames;
static TObject *o=0;
extern "C"
{
  void DirCB(NAMEVEC *nam)
    {
      NAMEVEC::iterator i;
      hnames.clear();
      for (i=nam->begin();i!=nam->end();i++)
      {
        hnames.push_back(new std::string(*i));
      }
//      printf("Total Number of Histograms: %d\n",n_hist);
    }
  void DatCB(PTRMAP *hists)
    {
//      MonHist h;
      PTRMAP::iterator i;
      for (i=hists->begin();i!=hists->end();i++)
      {
//        printf("Looping over Histos...\n");
        DimBuffBase *hist = (DimBuffBase *)i->second;
        if (o) delete o;
        o = (TObject*)MonHist::de_serialize(hist);
        o->Draw();
      }
      canv->Update();
    }
}

int main(int argc, char *argv[])
{
//  int rpcCBValue = 0;
  char *nam;
  if (argc<=1)
  {
    nam = (char*)"plus16_MON_Adder_MON_Moore_0/Histos/Data/Command";
  }
  else
  {
    nam = argv[1];
  }
  RPCRec rpcCB(nam, 1000);
  rpcCB.declareDirCB(&DirCB);
  rpcCB.declareDatCB(&DatCB);
  //RPCRec rpcCB("ALPC15_HIST_shit_0/HistCommand", 1000);

//  void *req;
  hnames.clear();
  RPCComm *rda;
  RPCCommRead *rrd;
  rda = new RPCComm;
  rda->c = RPCCReadAll;
  TH1D::SetDefaultSumw2();
  TH2D::SetDefaultSumw2();
  TProfile::SetDefaultSumw2();
  int dat;
  dat = RPCCDirectory;
  rpcCB.setData(dat);
  while (hnames.size() == 0)
  {
    sleep(1);
  }
  for (int i=0;i<(int)hnames.size();i++)
  {
    printf("%d %s\n",i,hnames[i]->c_str());
  }
  TApplication *app=new TApplication("Shit",&argc,argv);
  canv =  gROOT->MakeDefCanvas();
  gROOT->Draw();
  canv->Show();
  canv->Draw();
  int rdlen = sizeof(RPCCommRead)+strlen(hnames[1023]->c_str());
  rrd = (RPCCommRead*)malloc(rdlen);
  memset(rrd,0,rdlen);
  strcpy(&rrd->which,hnames[1023]->c_str());
  printf("asking for histogram %s\n",hnames[1023]->c_str());
  rrd->c = RPCCRead;
  MyTTimer *timr = new MyTTimer(&rpcCB, rrd,rdlen);
  timr->TurnOn();
  timr->Start(5000,true);
  while (1)
  {
    rpcCB.setData(rrd,rdlen);
    app->Run(true);
  }
  return 0;
}

