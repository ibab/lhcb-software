
#include <cmath>

#ifdef _WIN32
#include "TClient.h"
#endif

#ifndef _WIN32
#include "client.h"
#endif


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
#include "TGaxis.h"

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

#include "Camera/MessagePresenter.h"

#include "Camera/ElogDialog.h"
#include "Elog.h"

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

void MessagePresenter::UpdateRight()
{

  if (!dorun) return;

  //  if (time (NULL) > lastright)
  //  lastright = time(NULL);
  // else
  //  return;

  Int_t i =  fListBox816->GetSelected();

  if (i<0) return;
  fListBox863->RemoveAll();
  extradata.clear();

  std::map<std::string,std::vector<std::string>*>::iterator it;
  std::map<std::string,std::vector<std::string>*>::iterator itbegin;
  std::map<std::string,std::vector<std::string>*>::iterator itend;


  if (i==0)
  {
    int k=0;
    for (int j=allpairs.size()-1;j>=0 && k<fNumberEntry670->GetIntNumber();--j)
    {

      int l=3;

      string xinfostring;

      string::size_type position1 = allpairs[j].find("/");
      string::size_type position2 = allpairs[j].find("\\");
      int isextra=0;
      if ( position2 != string::npos )
      {
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
        entrystring=sss+":  " + ss.substr(0,position1)  + " :  "+ss.substr(position1+3,position2 - (position1+3))+ " -> ";
      //+ ss.substr(position2+1);
      else
        entrystring = sss+ ":  " + ss.substr(0,position1)  + " :  "+ss.substr(position1+3);


      fListBox863->AddEntry(entrystring.c_str(),k);
      extradata.push_back(xinfostring);

      //fListBox863->GetEntry(k)->SetBackgroundColor(TColor::GetColor());
      if (l==0)
        fListBox863->GetEntry(k)->SetBackgroundColor(TColor::RGB2Pixel(256,256,256));
      if (l==1)
        fListBox863->GetEntry(k)->SetBackgroundColor(Pgreen);
      if (l==2)
        fListBox863->GetEntry(k)->SetBackgroundColor(Pyellow);
      if (l==3)
        fListBox863->GetEntry(k)->SetBackgroundColor(Pred);
      ++k;

    }
  }
  else
  {

    itbegin = values.find(names[i]);
    itend = values.find(names[i]);

    if (itend==values.end()) itend--;
    int offset=0;

    for ( it=itend; ; --it )
    {

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

  //  fListBox863->Resize(576,594);
  fMainFrame1933->AddFrame(fListBox863, new TGLayoutHints(kLHintsNormal));
  //  fListBox863->MoveResize(230,24,665,650);
  fListBox863->Layout();

}

void MessagePresenter::UpdateView()
{
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
  // NM: changed.
  fListBox816->Resize(320,650);
  fMainFrame1933->AddFrame(fListBox816, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  // NM: changed.
  fListBox816->MoveResize(5,24,320,650);

}

void MessagePresenter::getwarnings(const char * fname)
{
  const std::string sfname = ( fname ? fname : "" );
  if ( !sfname.empty() )
  {

    FILE * F = fopen(sfname.c_str(),"r");
    if (!F)
    {
      std::cout << "Failed to open " << sfname << std::endl;
    }
    else
    {
      char cstr[1512];
      while ( fgets ( cstr, 1511, F ) )
      {
        const int s = strlen(cstr);
        cstr[s-1] = '\0';
        const std::string sstr = cstr;
        addwarning(sstr,0);
      }
      fclose(F);
    }
    lastleft = -1;
    lastright = -1;
    UpdateView();
    UpdateRight();
  }
}

void MessagePresenter::addwarning(const std::string & msg,const int ref)
{
  //  ifstream F("warnings");
  TDatime t;
  //cout<<cstr<<endl;
  std::string sstr = msg;
  string::size_type position1 = sstr.find("/");
  //cout <<sstr.substr(0,position1).c_str()<<" "<< sstr.substr(position1+1).c_str()<<" "<<i<<endl;
  if (position1 == std::string::npos){ // cpb sanity check!
    std::cerr << "Malformed message: "<<msg<<"="<<sstr<<std::endl;
    return;
  }
  const std::string key   = sstr.substr(0,position1);
  const std::string val   = sstr.substr(position1+1);
  const std::string level = sstr.substr(position1+1,1);
  
  //cout << key << " | " << val << " | " << level << endl;

  allpairs.push_back(msg);
  //val  val;

  int ilevel =0;
  if (level=="1") ilevel=1;
  if (level=="2") ilevel=2;
  if (level=="3") ilevel=3;

  if (ilevel==3 && FLITEOUT && ref==1)
  {
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
  else
  {
    ++(*it).second;
  }

  it = levels.find(key);

  if (it == levels.end())
  {
    levels.insert(std::pair<std::string,int>(key,ilevel));
  }
  else
  {
    if (ilevel > (*it).second) (*it).second = ilevel;
  }

  std::map<std::string,std::vector<std::string>*>::iterator its;
  its = values.find(key);
  if ( its == values.end() )
  {
    std::vector<std::string> * s = new std::vector<std::string>;
    s->push_back(val);
    values.insert(std::pair<std::string,std::vector<std::string>*>(key,s));
  }
  else
  {
    (*its).second->push_back(val);
  }

  //    sources.insert(pair<std::string,std::string>(key,val));

  //map<char,int>::iterator it;

  //  fListBox816->AddEntry(sstr.substr(0,position1).c_str(),i);

  //i++;
  //if (i%5==1)sleep(3);
  if (ref ==1)
  {
    UpdateView();
    UpdateRight();
  }

  //    if (i%5==1)sleep(3);
}

int MessagePresenter::GetXtra(const std::string & str, 
                              std::string & cachedfile)
{
  //cout << "In MessagePresenter::GetXtra " << str << " " 
  //     << cachedfile << endl;

  std::string::size_type position1 = str.find("/");

  if ( position1 == std::string::npos )
  { // cpb format check
    std::cerr << "No / in message"<<std::endl;
    return 0;
  }

  std::string add = str.substr(0,position1);
  const std::string file = str.substr(position1+1);

  const std::string& to = xcachefileName;

  const char * camproxy = getenv("CAMPROXY");
  if (camproxy)
  {
    add = (std::string)camproxy;
    // cerr << "Using proxy "<< getenv("CAMPROXY") <<endl;
  }

  const std::string::size_type position2 = add.find(":");
  std::string host(add), port("8888");

  if ( position2 != std::string::npos )
  {
    host = add.substr(0,position2);
    port = add.substr(position2+1);
  }

  //   char * cpy = strdup(to.c_str());
  //   for (int i=0;i<strlen(cpy);++i){
  //     char slash;
  //     if (cpy[i] == '/' || cpy[i] == '\\'){
  //       slash =;
  //     }
  //   }

  FILE * F = fopen(to.c_str(),"wb");
  if ( F == NULL )
  {
    std::cerr << "Could not open cache file: " << to << endl;
    perror("fopen in GetXtra:");
    return 0;
  }

  client c( host.c_str(), atoi(port.c_str()) );

  if ( c.Connect() > 0 ) 
  {
    //cout <<"GET " <<file.c_str()<<endl;
    c.wr("GET ",4);
    c.wr(file.c_str(),strlen(file.c_str()));
    c.wr("\n",1);
    //c.shutwr();
    char buf[512]; 
    int r;
    while ((r = c.rd(buf,511))!=-1){
      if (r!=-2){
        buf[r] = '\0';
        //      cout << buf<<endl;
        // write(0,buf,r );
        fwrite(buf,1,r,F);
      }
    }
    cachedfile = to;
  }
  else
  {
    std::cerr << "Error Connecting to client" << std::endl;
    fclose(F);
    return -1;
  }

  fclose(F);
  return 1;
}

void MessagePresenter::rootStyle()
{
  // line thickness
  const int lhcbWidth = 1;
  // Marker type
  const int lhcbMarkerType = 8;
  // marker size
  const float lhcbMarkerSize = 0.7;

  gStyle->SetCanvasBorderMode(0);
  gStyle->SetCanvasColor(10);
  gStyle->SetPadBorderMode(0);
  gStyle->SetPadColor(10);
  gStyle->SetPadTopMargin(0.1f);
  gStyle->SetPadBottomMargin(0.1f);
  gStyle->SetPadLeftMargin(0.1f);
  gStyle->SetPadRightMargin(0.125f);
  gStyle->SetPaperSize(18,24);
  gStyle->SetLabelSize(0.04f,"XY");
  gStyle->SetLabelOffset(0.01f,"Y");

  gStyle->SetTitleOffset(0.5,"x");
  gStyle->SetTitleOffset(0.5,"y");
  gStyle->SetTitleSize(0.04f,"XY");
  gStyle->SetTitleX(0.16);
  gStyle->SetTitleY(0.995);
  gStyle->SetTitleBorderSize(0);
  gStyle->SetTitleTextColor(kBlack);
  gStyle->SetTitleColor(kBlack);
  gStyle->SetTitleBorderSize(0);
  gStyle->SetTitleFont(62);
  gStyle->SetTitleFontSize(0.04f);
  gStyle->SetTitleColor(10);

  gStyle->SetStatFont(42);
  gStyle->SetStatBorderSize(1);
  gStyle->SetStatColor(10);
  gStyle->SetStatFontSize(0.04f);
  gStyle->SetOptStat(10);
  gStyle->SetStatX(0.97);
  gStyle->SetStatY(1.0);

  // use bold lines and markers
  gStyle->SetLineWidth(lhcbWidth);
  gStyle->SetFrameLineWidth(lhcbWidth);
  gStyle->SetHistLineWidth(lhcbWidth);
  gStyle->SetFuncWidth(lhcbWidth);
  gStyle->SetGridWidth(lhcbWidth);
  gStyle->SetLineStyleString(2,"[12 12]"); // postscript dashes
  gStyle->SetMarkerStyle(lhcbMarkerType);
  gStyle->SetMarkerSize(lhcbMarkerSize);

  // histogram divisions: only 5 in x to avoid label overlaps
  gStyle->SetNdivisions(505,"x");
  gStyle->SetNdivisions(510,"y");
  
  // Style for 2D zcol plots
  const Int_t NRGBs = 5;
  const Int_t NCont = 255;
  Double_t stops[NRGBs] = { 0.00, 0.34, 0.61, 0.84, 1.00 };
  Double_t red[NRGBs]   = { 0.00, 0.00, 0.87, 1.00, 0.51 };
  Double_t green[NRGBs] = { 0.00, 0.81, 1.00, 0.20, 0.00 };
  Double_t blue[NRGBs]  = { 0.51, 1.00, 0.12, 0.00, 0.00 };
  TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
  gStyle->SetNumberContours(NCont); 

  // put tick marks on top and RHS of plots
  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);

  // Set number of digits on axes
  TGaxis::SetMaxDigits(3);

}

void MessagePresenter::setup()
{
  // define and set root style
  rootStyle();

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

  fTextButtonDump->Connect("Clicked()","MessagePresenter",this,"dumpmsg()");

  fTextButtonElog->Connect("Clicked()","MessagePresenter",this,"dumpelog()");

  fListBox816->Connect("Selected(Int_t)","MessagePresenter",this,"selectleft()");;
  fListBox863->Connect("Selected(Int_t)","MessagePresenter",this,"selectright()");;
  fMainFrame1933->Connect("CloseWindow()","MessagePresenter",this,"DoClose()");
}

void MessagePresenter::DoClose()
{
  writeCacheFile(true);

  //  close all active sockets
  for ( unsigned int i = 0; i<socklist.size(); ++i )
  {
    if (socklist[i])
    {
      ((client *)socklist[i])->shut_close();

    }
  }

  exit(0);
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

static const char *gFiletypes[] = { "All files",     "*",
                                    "Portable Network Graphics",    "*.png",
                                    "Portable Document Format",    "*.pdf",
                                    "Encapsulated Postscript",   "*.eps",
                                    0,               0 };

void MessagePresenter::dumpmsg()
{
  Int_t i =  fListBox863->GetSelected();
  if (i<0) return;

  //std::cerr << iwAlive <<std::endl;

  //static TString dir(".");
  //std:;string
  static Bool_t overwr = kFALSE;
  TGFileInfo fi;
  fi.fFilename  = StrDup(savname);
  fi.fFileTypes = gFiletypes;
  fi.fIniDir    = StrDup(savdir);
  fi.fOverwrite = overwr;
  new TGFileDialog(fClient->GetDefaultRoot(), this, kFDSave, &fi);
  overwr = fi.fOverwrite;
  std::string namestub;
  if (fi.fFilename && strlen(fi.fFilename))
  {
    std::string s = fi.fFilename;

    savdir = fi.fIniDir;
    size_t pos2 = s.find_last_of("/\\");
    if (pos2!= string::npos) savname = s.substr(pos2+1);
    std::string stext,smsg;

    size_t pos = s.find_last_of(".");

    if ((pos2!=string::npos) && (pos2>pos)){ // no "." in name
      stext = s+".txt";
      smsg = s+".msg.txt";
    }
    else{
      stext = s;
      smsg = s;
      if (pos!=string::npos){
        stext.replace(pos,s.length()-pos,".txt");
        smsg.replace(pos,s.length()-pos,"_msg.txt");
      }
      else {
        stext = s+".txt";
        smsg = s+".msg.txt";
      }
    }
    if (iwAlive>1){// also print png

      if (iw->canvas() !=NULL)
        iw->canvas()->Print(s.c_str());

      if (iw->textedit() !=NULL){
        iw->textedit()->SaveFile(stext.c_str());
      }
    }
    FILE * F = fopen(smsg.c_str(),"w");
    if (F!=NULL){
      fprintf(F,"%s\n", fListBox863->GetEntry(i)->GetTitle());
      fclose(F);
    }

  }
}

void MessagePresenter::dumpelog()
{
  Int_t i =  fListBox863->GetSelected();
  if (i<0) return;

  // Construction the subject
  std::string subject = fListBox863->GetEntry(i)->GetTitle();
  boost::replace_all(subject," ->","");
  boost::replace_all(subject,"1: ","INFO: ");
  boost::replace_all(subject,"2: ","WARNING: ");
  boost::replace_all(subject,"3: ","ERROR: ");

  // get the current user
  const char* eloguser = getenv("CAMERA_ELOGAUTHOR");
  const char* user     = getenv("USER");
  static std::string username = ( eloguser ? std::string(eloguser) :
                                  user     ? std::string(user)     : "?" );

  static std::string logbook   = "RICH" ;
  static std::string system    = "RICH" ;
  std::string message   = "";
  int         isOK      = 0;
  std::string runNumber = ""; // not needed for RICH elog

  ElogDialog* elogDialog = new ElogDialog( this, 646, 435 );
  elogDialog->setParameters( logbook, username, system, subject, 
                             message, runNumber, isOK );

  // Make a random filename...
  TString pageName;
  gSystem->TempFileName(pageName);

  bool hasImage = false;
  TString ifile = pageName+".pdf";
  if ( iw->canvas() != NULL && iw->hasImage() )
  {
    hasImage = true;
    iw->canvas()->Print( ifile.Data() );
  }
  bool hasText = false;
  TString tfile = pageName+".txt";
  if ( iw->textedit() != NULL ) 
  {
    hasText = true;
    iw->textedit()->SaveFile( tfile.Data() );
  }

  fClient->WaitFor(dynamic_cast<TGWindow*>( elogDialog ));

  std::string answer("");
  if ( 0 == isOK ) {
    answer = "Request canceled.";
  } else if ( "" == subject ) {
    answer = "Mandatory subject is absent. Ignored.";
  } else {

    // for testing
    if ( m_logBookConfig.empty() ) m_logBookConfig = "lblogbook.cern.ch";

    Elog myElog( m_logBookConfig, 8080 );
    myElog.setCredential( "common", "Common!" );

    myElog.addAttribute( "Author", username );

    myElog.setLogbook( logbook );

    if ( hasImage ) myElog.addAttachment(ifile.Data());
    if ( hasText  ) myElog.addAttachment(tfile.Data());

    myElog.addAttribute( "System",  system );
    myElog.addAttribute( "Subject", subject );
    if ( !runNumber.empty() ) myElog.addAttribute( "Run", runNumber );

    const int number = myElog.submit( message );
    if ( 0 != number ) 
    {
      answer = "Created ELOG entry : " + logbook + " " + std::to_string(number);
    }
    else
    {
      answer = "ERROR : Failed to create elog entry";
    }

  }

  if ( boost::filesystem::exists(ifile.Data()) )
    boost::filesystem::remove(ifile.Data());
  if ( boost::filesystem::exists(tfile.Data()) )
    boost::filesystem::remove(tfile.Data());
  if ( boost::filesystem::exists(pageName.Data()) )
    boost::filesystem::remove(pageName.Data());

  static int msgBoxReturnCode(0);
  new TGMsgBox( fClient->GetRoot(), this, "Elog result",
                answer.c_str(), kMBIconExclamation, kMBOk,
                &msgBoxReturnCode );
 
}

void MessagePresenter::selectright()
{

  Int_t i = fListBox863->GetSelected();
  
  //cout << "In MessagePresenter::selectright " << i << endl;

  if ( extradata[i] == "" ) { 
    cerr << " -> extradata is empty" << endl;
    return ;
  }

  std::string cfile;
  if ( GetXtra(extradata[i],cfile) > 0)
  {
    if (iwAlive < 1) iw = new InfoWindow(&iwAlive);
    if (iwAlive < 2) iw->display();
    if (iwAlive>1) iw ->ShowCont(cfile);
  }
  else
  {
    fStatusBar528->SetText("Error retrieving extra data.");
  }

  //  new TCanvas;
}

void MessagePresenter::Layout(){
  int fWidth =  fMainFrame1933->GetWidth();
  int fHeight =  fMainFrame1933->GetHeight();
  // std::cout<< "Size changed to "<<fWidth<<" "<<fHeight<<endl;
  int leftwidth = 320 ;
  if (fWidth<440)
    leftwidth = fWidth/2;
  fTextButton514->MoveResize(5,2,65,17);
  fTextButton659->MoveResize(75,2,95,17);
  fTextButton699->MoveResize(170,2,55,17);

  fstopped->MoveResize(540,2,90,17);

  fTextButton515->MoveResize(445,2,90,20);
  fTextButtonDump->MoveResize(645,2,90,20);
  fTextButtonElog->MoveResize(745,2,90,20);
  fLabel746->MoveResize(235,2,70,20);
  fNumberEntry670->MoveResize(315,2,120,20);

  // NM: These are the key lines to enlarge the windows:
  fListBox816->MoveResize(5,24,leftwidth,fHeight-24-24);
  fListBox863->MoveResize(leftwidth+10,24,fWidth-leftwidth-15,fHeight-24-24);
  fStatusBar528->MoveResize(0,fHeight-24,fWidth,24);

  // UpdateView();
  // UpdateRight();

}
MessagePresenter::MessagePresenter(): TGMainFrame()
{

  Pred=TColor::RGB2Pixel(240,50,50);
  Pgreen=TColor::RGB2Pixel(80,255,80);
  Pyellow=TColor::RGB2Pixel(242,242,60);

  savdir = ".";
  savname = "msg.pdf";

  //int   dummy_argc   = 1;
  // char *dummy_argv[] =  { "MP", NULL  };
  //m_TApplication = new TApplication("MessagePresenter",&dummy_argc,dummy_argv);

  lastleft=time(NULL);
  lastright=time(NULL);
  savepos = -2;
}

void MessagePresenter::display()
{

  fMainFrame1933 = this;
  fMainFrame1933->SetLayoutBroken(kTRUE);
  fMainFrame1933->SetWindowName("Camera");

  //=====================================

  TGFont *ufont = gClient->GetFont("-adobe-helvetica-bold-r-*-*-14-*-*-*-*-*-iso8859-1");
  //TGFont *ufont = gClient->GetFont("-adobe-helvetica-bold-r-*-*-10-*-*-*-*-*-iso8859-1");
  //TGFont *ufont = gClient->GetFont("-adobe-helvetica-medium-r-*-*-10-*-*-*-*-*-iso8859-1");
  // graphics context changes

  GCValues_t valStopped;
  valStopped.fMask = kGCForeground | kGCBackground | kGCFillStyle | kGCFont | kGCGraphicsExposures;
  gClient->GetColorByName("#000000",valStopped.fForeground);
  gClient->GetColorByName("#c0c0c0",valStopped.fBackground);
  valStopped.fFillStyle = kFillSolid;
  valStopped.fFont = ufont->GetFontHandle();
  valStopped.fGraphicsExposures = kFALSE;
  TGGC   *uGC = gClient->GetGC(&valStopped, kTRUE);
  //

  // NM: don't bother to set the layout here since it's set again lather in the function Layout.
  fTextButton515 = new TGTextButton(fMainFrame1933,"Clear all",-1,uGC->GetGC(),ufont->GetFontStruct());
  fTextButton515->SetTextJustify(36);
  fTextButton515->Resize(90,24);
  fTextButton515->SetToolTipText("Remove all entries from the list");
  fMainFrame1933->AddFrame(fTextButton515, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  fTextButton515->MoveResize(400,2,90,20);

  fTextButtonDump = new TGTextButton(fMainFrame1933,"Print",-1,uGC->GetGC(),ufont->GetFontStruct());
  fTextButtonDump->SetTextJustify(36);
  fTextButtonDump->Resize(90,24);
  fTextButtonDump->SetToolTipText("Save the selected message as text and image files");
  fMainFrame1933->AddFrame(fTextButtonDump, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  fTextButtonDump->MoveResize(600,2,90,20);

  fTextButtonElog = new TGTextButton(fMainFrame1933,"Elog",-1,uGC->GetGC(),ufont->GetFontStruct());
  fTextButtonElog->SetTextJustify(36);
  fTextButtonElog->Resize(90,24);
  fTextButtonElog->SetToolTipText("Send the selected message to the RICH elog");
  fMainFrame1933->AddFrame(fTextButtonElog, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  fTextButtonElog->MoveResize(700,2,90,20);

  fNumberEntry670=new TGNumberEntry(fMainFrame1933, (Double_t) 0,14,-1,(TGNumberFormat::EStyle) 0,(TGNumberFormat::EAttribute) 1);
  fMainFrame1933->AddFrame(fNumberEntry670, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  fNumberEntry670->MoveResize(260,2,120,20);
  fNumberEntry670->SetNumber(5000);
  fLabel746 = new TGLabel(fMainFrame1933,"Max lines",uGC->GetGC(),ufont->GetFontStruct());
  fLabel746->SetTextJustify(36);
  fMainFrame1933->AddFrame(fLabel746, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  fLabel746->MoveResize(200,2,60,20);

  //=====================================
  fTextButton514 = new TGCheckButton(fMainFrame1933,"Errors",-1,uGC->GetGC(),ufont->GetFontStruct());
  fMainFrame1933->AddFrame(fTextButton514, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  fTextButton514->MoveResize(5,0,55,17);

  fstopped = new TGCheckButton(fMainFrame1933,"Update",-1,uGC->GetGC(),ufont->GetFontStruct());
  fMainFrame1933->AddFrame(fstopped, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  fstopped->MoveResize(400,0,55,17);

  fTextButton659 = new TGCheckButton(fMainFrame1933,"Warnings",-1,uGC->GetGC(),ufont->GetFontStruct());
  fMainFrame1933->AddFrame(fTextButton659, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  fTextButton659->MoveResize(75,0,72,17);

  fTextButton699 = new TGCheckButton(fMainFrame1933,"Info",-1,uGC->GetGC(),ufont->GetFontStruct());
  fMainFrame1933->AddFrame(fTextButton699, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  fTextButton699->MoveResize(145,0,42,17);

  //========================================================================================================
  // list box

  // NM: Left list box (algorithms names).
  fListBox816 = new TGListBox(fMainFrame1933,-1,kSunkenFrame);
  fListBox816->AddEntry("Hello",0);
  fMainFrame1933->AddFrame(fListBox816, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));

  //NM: void MoveResize(Int_t x, Int_t y, UInt_t w, UInt_t h)
  fListBox816->MoveResize(5,24,320,650);



  // list box
  // NM: Right list box (massages bodies).
  fListBox863 = new TGListBox(fMainFrame1933,-1,kSunkenFrame);
  fMainFrame1933->AddFrame(fListBox863, new TGLayoutHints(kLHintsNormal));
  fListBox863->MoveResize(330,24,565,650);

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

void MessagePresenter::messageloop( const char * host, const char * file )
{

  //  MessagePresenter mp(NULL,100,100);

  getwarnings(file);

  writeCacheON = ( file == NULL );

  gSystem->ProcessEvents();

  std::vector<std::string> serverlist;
  std::vector<client *> clientlist;

  std::vector<proto *> protolist;
  std::vector<int> connlist;

  if ( strcmp(host,"NULL") != 0 )
  {
    const string s = host;
    string str = s;

    std::string::size_type position1;
    //std::cout << "Trying Servers ' ";
    while ( (position1 = str.find(",")) != string::npos ) 
    {
      serverlist.push_back(str.substr(0,position1));
      //std::cout << str.substr(0,position1) << " ";
      str = str.substr(position1+1);
    }
    serverlist.push_back(str);
    //std::cout << str << " '" << std::endl;

    hostS = "";
    for ( unsigned int i = 0; i<serverlist.size(); ++i )
    {
      std::string hostpart,portpart;
      portpart="12346";
      hostpart=serverlist[i];
      std::string::size_type pos;
      pos = serverlist[i].find(":");

      if (pos!=string::npos)
      {
        hostpart = serverlist[i].substr(0,pos);
        portpart = serverlist[i].substr(pos+1);
      }

      client * c = new client(hostpart.c_str(),atoi(portpart.c_str()));

      clientlist.push_back(c);
      socklist.push_back((void *)c);

      proto * p = new proto(c);
      protolist.push_back(p);

      hostS.append(hostpart);

      connlist.push_back(0);
    }
    TGString savestat = (TGString)"";

    cachefileName  = getCacheFilename();
    //cout << "cache file " << cachefileName << endl;
    xcachefileName = getXCacheFilename();
    //cout << "xcache file " << xcachefileName << endl;
    checkCacheFileLength();
    readCacheFile();

    while ( true )
    {

      //fStatusBar528->SetText(TGString("Connecting to ")+TGString(host));

      for (unsigned int i = 0;i<serverlist.size();++i)
      {
        if (connlist[i] ==0){
          if (clientlist[i]->Connect()>0)
          {
            connlist[i] =1;
            // cerr << "Connected to "<< serverlist[i]<<endl;
          }
          // else
          //  cerr << "Failed to connect to "<< serverlist[i]<<endl;
        }
      }
      TGString stat = "Connected to ";

      for (unsigned int i = 0;i<serverlist.size();++i)
      {
        if ( connlist[i] == 1 )
        {
          stat = (TGString)(stat + (TGString)serverlist[i].c_str() + (TGString)", ");
        }
      }
      stat = (TGString)(stat + " / NOT Connected to: ");
      for (unsigned int i = 0;i<serverlist.size();++i)
      {
        if ( connlist[i] == 0 )
        {
          stat = (TGString)( stat + (TGString)serverlist[i].c_str() + (TGString)", " );
        }
      }

      if ((TGString)stat!=(TGString)savestat)
        fStatusBar528->SetText(stat);
      
      savestat = (TGString)stat;

      gSystem->ProcessEvents();
#ifndef _WIN32
      usleep(100000);
#endif

      for (unsigned int i = 0;i<serverlist.size();++i)
      {

        if ( connlist[i] == 1 ) 
        {

          char buf[512];
          int r;
          while ( (r = protolist[i]->getline(buf,511)) > 0 )
          {
            if (r > 0)
            {
              buf[r] = '\0';

              const std::string s = buf;

              addwarning(s,1);
              cachedWarnings.push_back(s);

              gSystem->ProcessEvents();
            }
            else
            {
              gSystem->ProcessEvents();
            }

            // every now and then, check the cache file length
            // otherwise just write
            const time_t timeNow = time(NULL);
            static time_t lastLengthCheck = timeNow;
            if ( (timeNow-lastLengthCheck) >= 15*60 )
            {
              checkCacheFileLength();
              lastLengthCheck = timeNow;
            }
            else
            {
              writeCacheFile();
            }

          }
          if ( r == -1 ) 
          {
            clientlist[i]->new_sock();
            connlist[i] = 0;
            //string str = "Connection to "+serverlist[i]+" terminated!";
          }

        }// connlist ==1
      } //for serverlist
    }
  }
  else
  {
    while(1)
    {
      gSystem->ProcessEvents();
    }
  }

}

void MessagePresenter::clearlist()
{
  allpairs.clear();
  keys.clear();
  levels.clear();
  cachedWarnings.clear();
  UpdateView();
  UpdateRight();
  clearCacheFile();
}

std::string MessagePresenter::_getCacheFilename(const std::string & _cache_name_)
{
  std::string to;
  const char * camcache = getenv("CAMCACHE");
  if ( camcache )
  {
    cout << "Using CAMCACHE environment variable " << camcache << endl;
    to = (std::string)camcache + "/" + _cache_name_;
  }
  else
  {
    // default to user home area
    const char * uhome = getenv("HOME");
    if ( uhome )
    {
      to = (std::string)uhome + "/" + _cache_name_;
    }
    else
    {
      // just use current dir
      to = "./" + _cache_name_;
    }
  }
  if ( !hostS.empty() ) to = to + "." + hostS;
  const char * user = getenv("USER"); 
  if ( user ) to = to + "." + (std::string)user;
  return to;
}

void MessagePresenter::clearCacheFile()
{
  std::ofstream file(cachefileName.c_str());
  file.close();
}

void MessagePresenter::checkCacheFileLength()
{
  writeCacheFile(true);
  cachedWarnings.clear();

  FILE *F = fopen(cachefileName.c_str(),"r");
  if (!F)
  {
    cerr << "Cannot open " << cachefileName << endl;
  }
  else
  {
    const unsigned int maxSize = fNumberEntry670->GetIntNumber();
    const unsigned int maxStringSize = 1512;
    char cstr[maxStringSize];
    std::vector<std::string> lines;
    unsigned int line = 0;
    while ( fgets ( cstr, maxStringSize-1, F ) && line++ < maxSize )
    {
      const int s = strlen(cstr);
      cstr[s-1] = '\0';
      const std::string sstr = cstr;
      //std::cout << "line " << line << " " << sstr << std::endl;
      lines.push_back(sstr);
      //cout << "Read " << sstr << " from cache file" << endl;
    }
    if ( lines.size() > maxSize )
    {
      clearCacheFile();
      for ( unsigned int i = lines.size()-1-maxSize; i < lines.size(); ++i )
      {
        cachedWarnings.push_back(lines[i]);
      }
      writeCacheFile(true);
    }
    cachedWarnings.clear();
    fclose(F);
  }

}

void MessagePresenter::writeCacheFile(const bool force)
{
  if ( writeCacheON && !cachedWarnings.empty() )
  {
    static time_t lastWrite       = time(NULL);

    // get the time now
    const time_t timeNow = time(NULL);

    // time to write out messages ?
    if ( force || cachedWarnings.size() >= 250 || (timeNow-lastWrite) >= 60 )
    {
      lastWrite = timeNow;

      std::ofstream file(cachefileName.c_str(),std::ios::app);
      //cout << "Writing to cache file " << cachefileName << endl;

      unsigned int count(0);
      for ( std::vector<std::string>::const_iterator i = cachedWarnings.begin();
            i != cachedWarnings.end(); ++i, ++count )
      {
        file << *i << std::endl;
        //cout << "   " << *i << endl;
      }

      // close the file
      file.close();

      // clear the list of messages to write out
      cachedWarnings.clear();      
    }
  }
}

void MessagePresenter::readCacheFile()
{
  if ( writeCacheON )
  {
    //cout << "Reading messages from " << cachefileName << endl;
    std::ofstream file(cachefileName.c_str(),std::ios::app);
    file.close(); // just to 'touch' the file incase it is not there
    getwarnings(cachefileName.c_str());
  }
}

int main(int /* argc */, char ** argv)
{
#ifdef _WIN32
  printf("I will not work properly under windows!\n");
#endif
#ifndef _WIN32
  dlerror();
#endif
  //  std::cout <<"Hello"<<std::endl;

  int   dummy_argc   = 1;
  // NM: modified the following line to couple with warning message:
  // "deprecated conversion from string constant to char*"
  char *dummy_argv[] = { (char*)"MP", NULL  };

  TApplication * TApp =
    new TApplication("MessagePresenter",&dummy_argc,dummy_argv);
  MessagePresenter mp;//(NULL,100,100);
  mp.display();
  //std::cout << "Display done" << std::endl;
  //mp.messageloop(argv[1],argv[2]);

  // NM: modified the following line to couple with warning message:
  // "deprecated conversion from string constant to char*"
  if (argv[1]==NULL)
  {
    mp.messageloop();
  }
  else 
  {
    mp.messageloop(argv[1],argv[2]);
  }
 
  delete TApp;

  return 0;
}
