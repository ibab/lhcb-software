// By ROOT version 5.12/00 on 2007-05-17 12:02:29
// Mainframe macro generated from application: /usr/local/root/bin/root.exe

#include <cmath>
#include "client.h"

#include <string>
#include <stdio.h>

#include <stdlib.h>
 #ifndef _WIN32
#include <dlfcn.h>
#endif

#if !defined( __CINT__) || defined (__MAKECINT__)

#include <TColor.h>

#include <TStyle.h>
#include "TGDockableFrame.h"
#include "TGMenu.h"
#include "TGMdiDecorFrame.h"
#include "TG3DLine.h"
#include "TGMdiFrame.h"
#include "TGMdiMainFrame.h"
#include "TGuiBldHintsButton.h"
#include "TGMdiMenu.h"

//#include "TGColorDialog.h"

#include "TGListBox.h"
#include "TGNumberEntry.h"
#include "TGScrollBar.h"
#include "TGuiBldHintsEditor.h"
#include "TGFileDialog.h"
#include "TGShutter.h"
#include "TGButtonGroup.h"
#include "TGCanvas.h"
#include "TGFSContainer.h"
#include "TGFontDialog.h"
#include "TGuiBldEditor.h"
#include "TGColorSelect.h"
#include "TGButton.h"
#include "TGSplitter.h"
#include "TGFSComboBox.h"

#ifndef ROOT_TGLabel
#include "TGLabel.h"
#endif
#ifndef ROOT_TGMsgBox
#include "TGMsgBox.h"
#endif
#ifndef ROOT_TRootGuiBuilder
#include "TRootGuiBuilder.h"
#endif
#ifndef ROOT_TGTab
#include "TGTab.h"
#endif
#ifndef ROOT_TGListView
#include "TGListView.h"
#endif
#ifndef ROOT_TGStatusBar
#include "TGStatusBar.h"
#endif
#ifndef ROOT_TGToolTip
#include "TGToolTip.h"
#endif
#ifndef ROOT_TGToolBar
#include "TGToolBar.h"
#endif
#ifndef ROOT_TGuiBldDragManager
#include "TGuiBldDragManager.h"
#endif

#include "Riostream.h"
#include "RQ_OBJECT.h"
#endif

#include <TApplication.h>
#include "TGWindow.h"
#include "TCanvas.h"

#include <map>
#include <TROOT.h>
#include <TSystem.h>



//TROOT mesageapp("messageapp","messageapp",NULL);

using namespace std;

#include "MessagePresenter.h"


void MessagePresenter::UpdateRight(){

  if (!dorun) return;

  //  if (time (NULL) > lastright)
  //  lastright = time(NULL);
  // else
  //  return;

  Int_t i =  fListBox816->GetSelected();

  if (i<0) return;
  fListBox863->RemoveAll();
  extradata.clear();

  //  cout<<  fListBox816->GetSelected()<<endl;
  //cout << names[i]<<endl;

  std::map<std::string,std::vector<std::string>*>::iterator it;
  std::map<std::string,std::vector<std::string>*>::iterator itbegin;
  std::map<std::string,std::vector<std::string>*>::iterator itend;


  if (i==0){
    int k=0;
    for (int j=allpairs.size()-1;j>=0 && k<fNumberEntry670->GetIntNumber();--j){

      int l=3;

      string xinfostring;

      string::size_type position1 = allpairs[j].find("/");
      string::size_type position2 = allpairs[j].find("\\");
      int isextra=0;
      if (position2 != string::npos ){
        isextra=1;
        xinfostring = allpairs[j].substr(position2+1);
      }


      //      cout << position2<<endl;
      //cout<<" allpairs "<<allpairs[j].substr(position1+1,1)<<endl;

      // cout <<(*s)[j]<<endl;
      std::string ss = allpairs[j];
      std::string sss = allpairs[j].substr(position1+1,1);
      l=0;
      if (sss=="1"){
        l=1;
      }
      if (sss=="2"){
        l=2;
      }
      if (sss=="3"){
        l=3;
      }

      if (l==1 && !doinfo) continue;
      if (l==2 && !dowarn) continue;
      if (l==3 && !doerr) continue;



      string entrystring;

      if (isextra ==1)
        entrystring = sss+ ":  " + ss.substr(0,position1)  + " :  "+ss.substr(position1+3,position2 - (position1+3))+ " -> ";//+ ss.substr(position2+1);
      else
        entrystring = sss+ ":  " + ss.substr(0,position1)  + " :  "+ss.substr(position1+3);


      fListBox863->AddEntry(entrystring.c_str(),k);
      extradata.push_back(xinfostring);

      //fListBox863->GetEntry(k)->SetBackgroundColor(TColor::GetColor());
      if (l==0)
        fListBox863->GetEntry(k)->SetBackgroundColor(TColor::RGB2Pixel(256,256,256));
      if (l==1)
        fListBox863->GetEntry(k)->SetBackgroundColor(TColor::RGB2Pixel(32,220,32));
      if (l==2)
        fListBox863->GetEntry(k)->SetBackgroundColor(TColor::RGB2Pixel(242,242,16));
      if (l==3)
        fListBox863->GetEntry(k)->SetBackgroundColor(TColor::RGB2Pixel(230,20,20));
      k++;

    }
  }
  else{

    itbegin = values.find(names[i]);
    itend = values.find(names[i]);

    if (itend==values.end()) itend--;
    int offset=0;

    for (it=itend; ;it--){

      int k=0;
      //string::size_type position1 = allpairs[j].find("/");
      //std::string ss = allpairs[j];
      //std::string sss = allpairs[j].substr(position1-1,1);

      std::vector<std::string>* s = (*it).second;


      for (int j=s->size()-1;j>=0;--j){
        int l=3;

        //   cout <<(*s)[j]<<endl;

        std::string ss = (*s)[j];

        string xinfostring="";

        string::size_type position1 = ss.find("/");
        string::size_type position2 = ss.find("\\");

        int isextra=0;

        if (position2 != string::npos ){
          isextra=1;
          xinfostring = ss.substr(position2+1);
        }


        //  string::size_type position1 = ss.find("/");

        std::string sss = ss.substr(position1-1,1);
        l=0;
        if (sss=="1"){
          l=1;
        }
        if (sss=="2"){
          l=2;
        }
        if (sss=="3"){
          l=3;
        }

        if (l==1 && !doinfo) continue;
        if (l==2 && !dowarn) continue;
        if (l==3 && !doerr) continue;


        string entrystring;

        if (isextra ==1)
          entrystring = sss+ ":  " + ss.substr(position1+1 , position2-position1-1 )  + " -> ";//+ ss.substr(position2+1);
        else
          entrystring = sss+ ":  " + ss.substr(position1+1)  ;

        fListBox863->AddEntry(entrystring.c_str(),k+offset);
        extradata.push_back(xinfostring);

        if (l==0)
          fListBox863->GetEntry(k+offset)->SetBackgroundColor(TColor::RGB2Pixel(256,256,256));
        if (l==1)
          fListBox863->GetEntry(k+offset)->SetBackgroundColor(Pgreen);
        if (l==2)
          fListBox863->GetEntry(k+offset)->SetBackgroundColor(Pyellow);
        if (l==3)
          fListBox863->GetEntry(k+offset)->SetBackgroundColor(Pred);

        k++;
        if (offset+k>fNumberEntry670->GetIntNumber()) break;
      }
      offset += k;

      if (it==itbegin) break;
    }

  }

  //    fListBox863->Resize(576,594);
  fMainFrame1933->AddFrame(fListBox863, new TGLayoutHints(kLHintsNormal));
  //  fListBox863->MoveResize(230,24,665,650);
  fListBox863->Layout();

}

void MessagePresenter::UpdateView(){
  // if (time (NULL) > lastleft)
  //   lastleft = time(NULL);
  // else
  //   return;

  if (savepos>-1) savepos = fListBox816->GetSelected();
  else savepos =0;

  fListBox816->RemoveAll();
  names.clear();
  std::map<std::string,int>::iterator it;
  std::map<std::string,int>::iterator it2;
  int i=1;
  fListBox816->AddEntry("All",0);
  names.push_back("All");

  for (it = keys.begin();it!=keys.end();it++){
    //cout << (*it).first<< " "<<(*it).second<<endl;
    names.push_back((*it).first);
    char buf[512];
    sprintf(buf,"%s - %d", (*it).first.c_str(),(*it).second);
    fListBox816->AddEntry(buf,i);
    it2 = levels.find((*it).first);
    if (it2!=levels.end()){
      //cout << (*it2).second<<endl;
      if  ((*it2).second ==1)
        fListBox816->GetEntry(i)->SetBackgroundColor(Pgreen);
      if  ((*it2).second ==2)
        fListBox816->GetEntry(i)->SetBackgroundColor(Pyellow);
      if  ((*it2).second >2)
        fListBox816->GetEntry(i)->SetBackgroundColor(Pred);
    }
    ++i;
  }
  fListBox816->Select(savepos);
  fListBox816->Resize(220,650);
  fMainFrame1933->AddFrame(fListBox816, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  fListBox816->MoveResize(5,24,220,650);

}


void MessagePresenter::getwarnings(char * fname){
  char cstr[1512];
  std::string sstr;



  //  ifstream F("warnings");
  if (fname!=NULL){
    FILE *F;
    F = fopen(fname,"r");
    if (F==NULL){
      printf("Cant open %s\n",fname);
    }
    else{

      while (fgets(cstr,1511,F)!=NULL){
        int s = strlen(cstr);
        cstr[s-1] = '\0';
        sstr = cstr;
        //usleep(1000);
        //cout << "Adding from file"<< sstr<<endl;
        addwarning(sstr,0);

      }
    }
    //    cout << "S " <<allpairs.size()<<endl;
    lastleft = -1;
    lastright = -1;
    UpdateView();
    UpdateRight();

    //    if (i%5==1)sleep(3);

  }

}

void MessagePresenter::addwarning(std::string msg,int ref){
  // cout << "Adding "<< msg<<endl;

  std::string sstr;



  //  ifstream F("warnings");
  TDatime t;
  //cout<<cstr<<endl;
  sstr = msg;
  string::size_type position1 = sstr.find("/");
  //cout <<sstr.substr(0,position1).c_str()<<" "<< sstr.substr(position1+1).c_str()<<" "<<i<<endl;

  std::string key = sstr.substr(0,position1);
  std::string val = sstr.substr(position1+1);
  std::string level =  sstr.substr(position1+1,1);

  allpairs.push_back(msg);
  //val  val;

  int ilevel =0;
  if (level=="1") ilevel=1;
  if (level=="2") ilevel=2;
  if (level=="3") ilevel=3;

  if (ilevel==3 && FLITEOUT && ref==1){
    std::string cmd = "flite/bin/flite -t \""+ sstr.substr(position1+3) + "\" -o msg.wav && play msg.wav &";
    printf(cmd.c_str());
    system(cmd.c_str());
    //system("play msg.wav");

  }

  std::map<std::string,int>::iterator it;

  it = keys.find(key);
  if (it == keys.end())
  {
    keys.insert(std::pair<std::string,int>(key,1));
  }
  else{
    (*it).second++;
  }

  it = levels.find(key);

  if (it == levels.end())
  {
    levels.insert(std::pair<std::string,int>(key,ilevel));
  }
  else{
    //int i = (*it).second;
    if (ilevel > (*it).second) (*it).second = ilevel;
  }

  std::map<std::string,std::vector<std::string>*>::iterator its;
  its = values.find(key);
  if (its == values.end())
  {
    std::vector<std::string> * s = new  std::vector<std::string>;
    s->push_back(val);
    values.insert(std::pair<std::string,std::vector<std::string>*>(key,s));
  }
  else{
    (*its).second->push_back(val);
  }

  //    sources.insert(pair<std::string,std::string>(key,val));

  //map<char,int>::iterator it;

  //  fListBox816->AddEntry(sstr.substr(0,position1).c_str(),i);

  //i++;
  //if (i%5==1)sleep(3);
  if (ref ==1){
    UpdateView();
    UpdateRight();
  }

  //    if (i%5==1)sleep(3);



}

int MessagePresenter::GetXtra(std::string str , std::string & cachedfile){

  std::string::size_type position1 = str.find("/");

  std::string add = str.substr(0,position1);
  std::string file = str.substr(position1+1);

  std::string to;
  if (getenv("CAMCACHE")!=NULL){
    to = (std::string)getenv("CAMCACHE");
    to = to + "/" + file;
  }
  else{
    to = "cache/"+file;
  }

  if (getenv("CAMPROXY")!=NULL){
    add = (std::string)getenv("CAMPROXY");
    // cerr << "Using proxy "<< getenv("CAMPROXY") <<endl;
  }

  std::string::size_type position2 = add.find(":");
  std::string host,port;

  host = add;
  port = "8888";


  if (position2 != std::string::npos){
    host = add.substr(0,position2);
    port = add.substr(position2+1);
  }

  FILE *F = fopen(to.c_str(),"wb");
  if (F==NULL){
    //std::cerr<< "Could not open cache file: "<<to.c_str()<<endl;
    perror("");
    return 0;
  }

  client c(host.c_str(),atoi(port.c_str()));

  if (c.Connect()>0){
    //cout <<"GET " <<file.c_str()<<endl;
    c.wr("GET ",4);
    c.wr(file.c_str(),strlen(file.c_str()));
    c.wr("\n",1);
    c.shutwr();
    char buf[512];int r;
    while ((r = c.rd(buf,511))!=-1){
      if (r!=-2){
        buf[r] = '\0';
        //      cout << buf<<endl;
        // write(0,buf,r );
        fwrite(buf,1,r,F);
      }
    }
    fclose(F);
    cachedfile = to;

  }
  else{

    // std::cerr << "Error Connecting"<<std::endl;
    return -1;
  }

  return 1;
}

void MessagePresenter::setup(){
  const int colNum     = 1024;   // Number of colors in the palette
  int       startIndex =  1000;    // starting index of allocated colors
  int       palette[colNum];
  float     val;
  int       iCount     =    0;
  const int iStep      =    1;
  std::vector<TColor*>          m_RootColourVector;
  m_RootColourVector.reserve(colNum);

  // blue(0,0,255) -> cyan(0,255,255)
  for (int i=0; i < 256; i += iStep) {
    val = i/(float)256;
    TColor *color = new TColor(startIndex+iCount, 0, val, 1);
    m_RootColourVector.push_back(color);
    palette[iCount ] = startIndex + iCount;
    iCount++;
  }

  // cyan (0,255,255) -> green (0,255,0)
  for (int i=0; i < 256; i += iStep){
    val = i/(float)256;
    TColor *color = new TColor(startIndex+iCount, 0, 1, 1-val);
    m_RootColourVector.push_back(color);
    palette[iCount] = startIndex + iCount;
    iCount ++;
  }

  //green (0,255,0) -> yellow (255,255,0)
  for (int i=0; i < 256; i += iStep){
    val = i/(float)256;
    TColor *color = new TColor(startIndex+iCount, val, 1, 0);
    m_RootColourVector.push_back(color);
    palette[iCount] = startIndex + iCount;
    iCount ++;
  }

  // yellow (255,255,0) -> red (255,0,0)
  for (int i=0; i < 256; i += iStep){
    val = i/(float)256;
    TColor *color = new TColor(startIndex+iCount, 1, 1-val, 0);
    m_RootColourVector.push_back(color);
    palette[iCount] = startIndex + iCount;
    iCount ++;
  }
  // define and set style

  gStyle->SetCanvasBorderMode(0);
  gStyle->SetCanvasColor(10);
  gStyle->SetPadBorderMode(1);
  gStyle->SetPadColor(10);
  gStyle->SetPadTopMargin(0.1);
  gStyle->SetPadBottomMargin(0.1);
  gStyle->SetPadLeftMargin(0.1);
  gStyle->SetPaperSize(18,24);
  gStyle->SetLabelSize(0.04,"XY");
  gStyle->SetLabelOffset(0.01,"Y");
  gStyle->SetTitleOffset(1.1,"XY");
  gStyle->SetTitleSize(0.06,"XY");
  gStyle->SetStatFont(42);
  gStyle->SetStatBorderSize(1);
  gStyle->SetStatColor(10);
  gStyle->SetStatFontSize(0.08);
  gStyle->SetTitleBorderSize(1);
  gStyle->SetTitleFont(62);
  gStyle->SetTitleFontSize(0.06);

  gStyle->SetTitleColor(10);
  gStyle->SetOptStat(10);

  gStyle->SetPalette(colNum, palette);

  Pred=TColor::RGB2Pixel(230,20,20);
  Pgreen=TColor::RGB2Pixel(32,220,32);
  Pyellow=TColor::RGB2Pixel(242,242,16);


  fTextButton659->SetState(kButtonDown);
  fTextButton699->SetState(kButtonDown);
  fTextButton514->SetState(kButtonDown);

  fstopped->SetState(kButtonDown);
  dorun = fstopped->IsOn();

  doerr=dowarn=doinfo=true;

  fTextButton659->Connect("Clicked()","MessagePresenter",this,"selectWarn()");
  fTextButton699->Connect("Clicked()","MessagePresenter",this,"selectInfo()");

  fstopped->Connect("Clicked()","MessagePresenter",this,"selectRun()");

  fTextButton514->Connect("Clicked()","MessagePresenter",this,"selectErr()");

  fTextButton515->Connect("Clicked()","MessagePresenter",this,"clearlist()");


  fListBox816->Connect("Selected(Int_t)","MessagePresenter",this,"selectleft()");;
  fListBox863->Connect("Selected(Int_t)","MessagePresenter",this,"selectright()");;
  //  fMainFrame1933->Connect("CloseWindow()","MessagePresenter",this,"DoClose()");
}

void MessagePresenter::DoClose(){
  // cout << "I do not like to be closed."<<endl;
  //  fMainFrame1933->DontCallClose();
}

void  MessagePresenter::selectWarn(){

  //  cout << "selected warn "<<fTextButton659->IsOn()<<endl;
  dowarn = fTextButton659->IsOn();
  UpdateRight();
}

void  MessagePresenter::selectErr(){
  //cout << "selected Err "<< fTextButton514->IsOn()<<endl;
  doerr = fTextButton514->IsOn();
  UpdateRight();
}

void  MessagePresenter::selectRun(){
  //cout << "selected Err "<< fTextButton514->IsOn()<<endl;
  dorun = fstopped->IsOn();
  UpdateRight();
}

void  MessagePresenter::selectInfo(){
  //cout << "selected Info "<<fTextButton699->IsOn()<<endl;
  doinfo = fTextButton699->IsOn();
  UpdateRight();
}

void MessagePresenter::selectleft(){
  //cout << "selectleft"<<endl;
  UpdateRight();

}


void MessagePresenter::selectright(){

  Int_t i =  fListBox863->GetSelected();

  if (
      extradata[i]==""){

    return ;

  }
  std::string cfile;
  if ( GetXtra(extradata[i],cfile)> 0){

    if (iwAlive < 1) iw = new InfoWindow(&iwAlive);
    if (iwAlive < 2) iw->display();

    iw ->ShowCont(cfile);

  }

  else{
    fStatusBar528->SetText("Error retrieving extra data.");
  }
  //  new TCanvas;
}
void MessagePresenter::Layout(){
  int fWidth =  fMainFrame1933->GetWidth();
  int fHeight =  fMainFrame1933->GetHeight();
  // std::cout<< "Size changed to "<<fWidth<<" "<<fHeight<<endl;
  int leftwidth = 220 ;
  if (fWidth<440)
    leftwidth = fWidth/2;
  fTextButton514->MoveResize(5,2,55,17);
  fTextButton659->MoveResize(75,2,72,17);
  fTextButton699->MoveResize(145,2,42,17);

  fstopped->MoveResize(500,2,90,17);

  fTextButton515->MoveResize(400,2,90,20);
  fLabel746->MoveResize(200,2,60,20);
  fNumberEntry670->MoveResize(260,2,120,20);


  fListBox816->MoveResize(5,24,leftwidth,fHeight-24-24);
  fListBox863->MoveResize(leftwidth+10,24,fWidth-leftwidth-15,fHeight-24-24);
  fStatusBar528->MoveResize(0,fHeight-24,fWidth,24);
  // UpdateView();
  // UpdateRight();

}
MessagePresenter::MessagePresenter():TGMainFrame()
{



  //int   dummy_argc   = 1;
  // char *dummy_argv[] =  { "MP", NULL  };
  //m_TApplication = new TApplication("MessagePresenter",&dummy_argc,dummy_argv);


  lastleft=time(NULL);
  lastright=time(NULL);
  savepos =-2;
}
void MessagePresenter::display(){
  // main frame
  //fMainFrame1933 = new TGMainFrame(gClient->GetRoot(),10,10,kMainFrame | kVerticalFrame);

  fMainFrame1933 = this;
  fMainFrame1933->SetLayoutBroken(kTRUE);

  fMainFrame1933->SetWindowName("CAMERA");

  //=====================================

  //TGTextButton *
  fTextButton515 = new TGTextButton(fMainFrame1933,"Clear all");
  fTextButton515->SetTextJustify(36);
  fTextButton515->Resize(90,24);
  fTextButton515->SetToolTipText("Remove all entries from the list");
  fMainFrame1933->AddFrame(fTextButton515, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  fTextButton515->MoveResize(400,2,90,20);
  //   TGNumberEntry *
  fNumberEntry670 = new TGNumberEntry(fMainFrame1933, (Double_t) 0,14,-1,(TGNumberFormat::EStyle) 0,(TGNumberFormat::EAttribute) 1);
  fMainFrame1933->AddFrame(fNumberEntry670, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  fNumberEntry670->MoveResize(260,2,120,20);
  fNumberEntry670->SetNumber(50);
  fLabel746 = new TGLabel(fMainFrame1933,"Max lines");
  fLabel746->SetTextJustify(36);
  fMainFrame1933->AddFrame(fLabel746, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  fLabel746->MoveResize(200,2,60,20);

  //=====================================
  GCValues_t valButton514;
  GCValues_t valButton659;
  GCValues_t valButton699;
  GCValues_t valStopped;

  fTextButton514 = new TGCheckButton(fMainFrame1933,"Errors");
  TGFont *ufont;         // will reflect user font changes
  ufont = gClient->GetFont("-adobe-helvetica-medium-r-*-*-12-*-*-*-*-*-iso8859-1");
  TGGC   *uGC;           // will reflect user GC changes
  // graphics context changes

  // GCValues_t valButton514;
  valButton514.fMask = kGCForeground | kGCBackground | kGCFillStyle | kGCFont | kGCGraphicsExposures;
  gClient->GetColorByName("#ff0000",valButton514.fForeground);
  gClient->GetColorByName("#c0c0c0",valButton514.fBackground);
  valButton514.fFillStyle = kFillSolid;
  valButton514.fFont = ufont->GetFontHandle();
  valButton514.fGraphicsExposures = kFALSE;
  uGC = gClient->GetGC(&valButton514, kTRUE);
  //,-1,uGC->GetGC());
  fMainFrame1933->AddFrame(fTextButton514, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  fTextButton514->MoveResize(5,0,55,17);



  fstopped = new TGCheckButton(fMainFrame1933,"Update");
  //  TGFont *ufont;         // will reflect user font changes
  ufont = gClient->GetFont("-adobe-helvetica-medium-r-*-*-12-*-*-*-*-*-iso8859-1");
  //TGGC   *uGC;           // will reflect user GC changes
  // graphics context changes

  // GCValues_t valButton514;
  valStopped.fMask = kGCForeground | kGCBackground | kGCFillStyle | kGCFont | kGCGraphicsExposures;
  gClient->GetColorByName("#ff0000",valStopped.fForeground);
  gClient->GetColorByName("#c0c0c0",valStopped.fBackground);
  valStopped.fFillStyle = kFillSolid;
  valStopped.fFont = ufont->GetFontHandle();
  valStopped.fGraphicsExposures = kFALSE;
  uGC = gClient->GetGC(&valButton514, kTRUE);
  //,-1,uGC->GetGC());
  fMainFrame1933->AddFrame(fstopped, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  fstopped->MoveResize(400,0,55,17);




  fTextButton659 = new TGCheckButton(fMainFrame1933,"warnings");
  ufont = gClient->GetFont("-adobe-helvetica-medium-r-*-*-12-*-*-*-*-*-iso8859-1");


  // graphics context changes
  //=============================
  valButton659.fMask = kGCForeground | kGCBackground | kGCFillStyle | kGCFont | kGCGraphicsExposures;
  gClient->GetColorByName("#ffff00",valButton659.fForeground);
  gClient->GetColorByName("#c0c0c0",valButton659.fBackground);
  valButton659.fFillStyle = kFillSolid;
  valButton659.fFont = ufont->GetFontHandle();
  valButton659.fGraphicsExposures = kFALSE;
  uGC = gClient->GetGC(&valButton659, kTRUE);
  //,-1,uGC->GetGC());
  fMainFrame1933->AddFrame(fTextButton659, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  fTextButton659->MoveResize(75,0,72,17);
  fTextButton699 = new TGCheckButton(fMainFrame1933,"info");
  ufont = gClient->GetFont("-adobe-helvetica-medium-r-*-*-12-*-*-*-*-*-iso8859-1");
  //====================================================================================================
  // graphics context changes
  valButton699.fMask = kGCForeground | kGCBackground | kGCFillStyle | kGCFont | kGCGraphicsExposures;
  gClient->GetColorByName("#00ff00",valButton699.fForeground);
  gClient->GetColorByName("#c0c0c0",valButton699.fBackground);
  valButton699.fFillStyle = kFillSolid;
  valButton699.fFont = ufont->GetFontHandle();
  valButton699.fGraphicsExposures = kFALSE;
  uGC = gClient->GetGC(&valButton699, kTRUE);
  //,-1,uGC->GetGC());
  fMainFrame1933->AddFrame(fTextButton699, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  fTextButton699->MoveResize(145,0,42,17);

  //========================================================================================================
  // list box
  fListBox816 = new TGListBox(fMainFrame1933,-1,kSunkenFrame);
  fListBox816->AddEntry("Hello",0);
  //   fListBox816->Resize(216,650);
  fMainFrame1933->AddFrame(fListBox816, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));

  fListBox816->MoveResize(5,24,220,650);



  // list box
  fListBox863 = new TGListBox(fMainFrame1933,-1,kSunkenFrame);
  //fListBox863->AddEntry("Message Presenter",0);
  // fListBox863->Resize(665,650);
  fMainFrame1933->AddFrame(fListBox863, new TGLayoutHints(kLHintsNormal));
  fListBox863->MoveResize(230,24,665,650);



  fStatusBar528 = new TGStatusBar(fMainFrame1933,700-24,24);

  fMainFrame1933->AddFrame(fStatusBar528, new TGLayoutHints(kLHintsBottom | kLHintsExpandX));
  fStatusBar528->MoveResize(0,700-24,900,24);

  fMainFrame1933->MapSubwindows();
  fMainFrame1933->Resize(fMainFrame1933->GetDefaultSize());
  fMainFrame1933->MapWindow();
  fMainFrame1933->Resize(900,700);


  fStatusBar528->SetText("Setup...");
  setup();
  fStatusBar528->SetText("Ready...");
  // fStatusBar528->Update();
  // getwarnings();
  iw = new InfoWindow(&iwAlive);
  Layout();
}

void MessagePresenter::messageloop(char * host,char * file){



  //  MessagePresenter mp(NULL,100,100);

  getwarnings(file);

  gSystem->ProcessEvents();

  std::vector<std::string> serverlist;
  std::vector<client *> clientlist;
  std::vector<proto *> protolist;
  std::vector<int> connlist;


  if (strcmp(host,"NULL")!=0){

    string s = host;
    string str = s;

    std::string::size_type position1;
    // cout << "Trying Servers:"<<endl;
    while ((position1 = str.find(","))!=string::npos){
      // cout << str.substr(0,position1)<<endl;;
      serverlist.push_back(str.substr(0,position1));
      str = str.substr(position1+1);

    }
    // cout << str<<endl;;
    serverlist.push_back(str);

    for (unsigned int i = 0;i<serverlist.size();++i){
      client *  c = new client(serverlist[i].c_str(),12346);
      // cout <<serverlist[i]<<endl;
      clientlist.push_back(c);

      proto *  p = new proto(c);
      protolist.push_back(p);

      connlist.push_back(0);
    }
    TGString savestat=(TGString)"";
    while (1)
    {


      //fStatusBar528->SetText(TGString("Connecting to ")+TGString(host));


      for (unsigned int i = 0;i<serverlist.size();++i){
        if (connlist[i] ==0){
          if (clientlist[i]->Connect()>0){
            connlist[i] =1;
            // cerr << "Connected to "<< serverlist[i]<<endl;
          }
          // else
          //  cerr << "Failed to connect to "<< serverlist[i]<<endl;
        }
      }
      TGString stat = "Connected to ";


      for (unsigned int i = 0;i<serverlist.size();++i){
        if (connlist[i] ==1){
          stat = (TGString)(stat + (TGString)serverlist[i].c_str() + (TGString)", ");
        }
      }
      stat = (TGString)(stat + " / NOT Connected to: ");
      for (unsigned int i = 0;i<serverlist.size();++i){
        if (connlist[i] ==0){
          stat = (TGString)(stat + (TGString)serverlist[i].c_str() + (TGString)", ");
        }
      }

      if ((TGString)stat!=(TGString)savestat)
        fStatusBar528->SetText(stat);
      savestat = (TGString)stat;

      gSystem->ProcessEvents();
#ifndef _WIN32
      usleep(100000);
#endif
      for (unsigned int i = 0;i<serverlist.size();++i){

        if (connlist[i] ==1){

          char buf[512];
          int r;
          while ((r = protolist[i]->getline(buf,511)) >0 ){
            if (r >0){
              buf[r] = '\0';
              // printf("%s\n",buf);
              std::string s = buf;
              addwarning(s,1);
              gSystem->ProcessEvents();
            }
            else{
              gSystem->ProcessEvents();
            }

          }
          if (r==-1){
            clientlist[i]->new_sock();
            connlist[i] =0;
            string str = "Connection to "+serverlist[i]+" terminated!";
            //   fStatusBar528->SetText(TGString(str.c_str() ));
            //gSystem->ProcessEvents();
            //sleep(2);
          }

          //     gSystem->ProcessEvents();
          //fStatusBar528->SetText(TGString("Connected to ")+TGString(host));

          //      proto p(clientlist[0]);
          //      char buf[512];
          //      int r;
          //      while ((r = p.getline(buf,511)) != -1 ){
          //        if (r >0){
          //   buf[r] = '\0';
          //   // printf("%s\n",buf);
          //   std::string s = buf;
          //   addwarning(s,1);
          //   gSystem->ProcessEvents();
          //        }
          //        else{
          //   gSystem->ProcessEvents();
          //        }

          //      }

          //      clientlist[0]->new_sock();
          //      fStatusBar528->SetText(TGString("Connection terminated! ")) ;

          //      else{
          //        fStatusBar528->SetText(TGString("Connection failed to ")+TGString(host)); ;
          //        for (int i=0;i<100;++i){

          //   gSystem->ProcessEvents();
          //   usleep(20000);
          //        }
          //      }


        }// connlist ==1
      } //for serverlist
    }
  }

  else{
    while(1){
      gSystem->ProcessEvents();
    }
  }

}

int main(int /* argc */, char ** argv){
#ifdef _WIN32
  printf("I will not work properly under windows!\n");
 #endif
 #ifndef _WIN32
  dlerror();
 #endif
  int   dummy_argc   = 1;
  char *dummy_argv[] =  { "MP", NULL  };
  TApplication * TApp = 
    new TApplication("MessagePresenter",&dummy_argc,dummy_argv);

  MessagePresenter mp;//(NULL,100,100);
  mp.display();
  //mp.messageloop(argv[1],argv[2]);

  if (argv[1]==NULL) mp.messageloop();
  else mp.messageloop(argv[1],argv[2]);

  delete TApp;

  return 0;
}
