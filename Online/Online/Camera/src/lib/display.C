
#include "display.h"
#include "container_ROOT.h"

#include "TArrow.h"
#include "TBox.h"
#include "TEllipse.h"
#include "TF1.h"
#include "TF2.h"
#include "TH2F.h"
#include "TLine.h"
#include "TROOT.h"

#include <iostream>

InfoWindow::InfoWindow(int * inalive):TGMainFrame()
{
  fHasImage = false;
  //std::cout <<"Made me"<<std::endl;
  alive = inalive;
  lines=0;
  *alive =1;
}

InfoWindow::~InfoWindow()
{
  *alive = 0;
  cont.reset();
  //  std::cerr << "I am no more" << std::endl;
}

void InfoWindow::ShowCont(const std::string& ins)
{
  //std::cout << "in ShowCont " << ins << std::endl;

  SetBit(kDontCallClose);
  const std::string& cfile = ins;

  this->textclear();
  this->canvas()->Clear();
  this->canvas()->Update();

  //  container_ROOT cont;

  {

    cont.reset();
    cont.fromfile(cfile.c_str());
    cont.convert();
    int nrHistos=0;

    for ( int i=0; i < cont.entries(); ++i )
    {

      const std::size_t pos = cont.name(i).find('.');

      std::string cn = cont.name(i);
      std::string cna = "";

      if (pos!=std::string::npos) cn = cont.name(i).substr(0,pos);
      if (pos!=std::string::npos) cna = cont.name(i).substr(pos+1);

      if (cn=="DIM1D" && (cna.find("SAME") ==std::string::npos)&& (cna.find("same") ==std::string::npos) ) nrHistos++;
      if (cn=="TF1" && (cna.find("SAME") ==std::string::npos)&& (cna.find("same") ==std::string::npos) ) nrHistos++;
      if (cn=="TF2" && (cna.find("SAME") ==std::string::npos)&& (cna.find("same") ==std::string::npos) ) nrHistos++;
      if (cn=="DIM2D" && (cna.find("SAME") ==std::string::npos)&& (cna.find("same") ==std::string::npos) ) nrHistos++;
    }
    //    cout << "We need space for "<< nrHistos<<" Histos"<<endl;
    //if (iwAlive < 1)
    //int dummy;
    //  InfoWindow * iw = new InfoWindow(&dummy);
    if ((*alive) < 2)
      this->display();

    fHasImage = nrHistos > 0;

    int HistoCnt = 1;

    if (nrHistos>1)
    {
    
      int divx=0;
      int divy=0;

      while (divx*divy<nrHistos)
      {
        if ( nrHistos > divx*divy ) divy+=1;
        if ( nrHistos > divx*divy ) divx+=1;
      }

      //cout << "dividing in "<<divx <<", "<<divy<<" Pads" <<endl;
      //cout << nrHistos<<" "<<endl;
      this ->canvas()->Divide(divx,divy);
    }

    numGraphs = 0;
    numTexts = 0;

    for ( int i = 0; i <cont.entries(); ++i ) 
    {
    
      if (cont.object(i) == NULL) continue;

      const std::size_t pos = cont.name(i).find('.');

      std::string cn = cont.name(i);
      std::string cna = "";

      if (pos!=std::string::npos) cn = cont.name(i).substr(0,pos);
      if (pos!=std::string::npos) cna = cont.name(i).substr(pos+1);

      if (cn=="EVAL"){

        numGraphs++;

        char * v = (char *)cont.object(i);

        gROOT->ProcessLine(v);
        this->canvas()->Update();
      }

      if (cn=="DIM2D"){
        numGraphs++;
        // this->appendline("2D Histo ");

        if ((cna.find("SAME") ==std::string::npos)&& (cna.find("same") ==std::string::npos) ) this->canvas()->cd(HistoCnt++);
        TH2F * v = (TH2F *)cont.object(i);
        if (cna == "")
          v->Draw("colz");
        else
          v->Draw(cna.c_str());
        this->canvas()->Update();
      }
      if (cn=="DIM1D"){
        numGraphs++;
        // this->appendline("1D Histo ");

        if ((cna.find("SAME") ==std::string::npos)&& (cna.find("same") ==std::string::npos) ) this->canvas()->cd(HistoCnt++);
        TH1F * v = (TH1F *)cont.object(i);
        if (cna == "")
          v->Draw("hist");
        else
          v->Draw(cna.c_str());
        this->canvas()->Update();
      }
      if (cn=="TF1"){
        numGraphs++;
        // this->appendline("1D Histo ");

        if ( ( cna.find("SAME") == std::string::npos ) && 
             ( cna.find("same") == std::string::npos ) ) { this->canvas()->cd(HistoCnt++); }
        TF1 * v = (TF1 *)cont.object(i);
        //cout << cna<<endl;
        if (cna == "")
          v->Draw();
        else
          v->Draw(cna.c_str());
        this->canvas()->Update();
      }

      if (cn=="TF2"){
        numGraphs++;
        // this->appendline("1D Histo ");

        if ( ( cna.find("SAME") == std::string::npos ) && 
             ( cna.find("same") == std::string::npos ) ) { this->canvas()->cd(HistoCnt++); }
        TF2 * v = (TF2 *)cont.object(i);
        if (cna == "")
          v->Draw();
        else
          v->Draw(cna.c_str());
        this->canvas()->Update();
      }


      if (cont.name(i)=="TEXT"){
        // this->appendline("Text: ");
        this->appendline((char*)cont.object(i));
        ++numTexts;
      }
      if (cont.name(i)=="CTIME"){
        // this->appendline("Date/Time: ");
        ++numTexts;

        this->appendline((char *)cont.object(i));
      }


      if ((cont.name(i)=="TELLIPSE")||(cont.name(i)=="TTELLIPSE" )){
        numGraphs++;
        // this->appendline("Ellipse");

        // this->canvas()->cd(HistoCnt++);
        TEllipse * v = (TEllipse *)cont.object(i);
        v->Draw();
        this->canvas()->Update();
      }


      if ((cont.name(i)=="TBOX")||(cont.name(i)=="TTBOX" )){
        numGraphs++;
        // this->appendline("Box");

        // this->canvas()->cd(HistoCnt++);
        TBox * v = (TBox *)cont.object(i);
        v->Draw();
        this->canvas()->Update();
      }


      if ((cont.name(i)=="TLINE")||(cont.name(i)=="TTLINE" )){
        numGraphs++;
        // this->appendline("Line");

        // this->canvas()->cd(HistoCnt++);
        TLine * v = (TLine *)cont.object(i);
        v->Draw();
        this->canvas()->Update();
      }

      if ((cont.name(i)=="TARROW")||(cont.name(i)=="TTARROW" )){
        numGraphs++;
        // this->appendline("Arrow");

        // this->canvas()->cd(HistoCnt++);
        TArrow * v = (TArrow *)cont.object(i);
        v->Draw();
        this->canvas()->Update();
      }



      if (cont.name(i)=="DIMA"){
        // this->appendline("DIM Adress: ");
        this->appendline((char *)cont.object(i));
        char buf[1024];
        sprintf(buf,"GaudiMonitor.exe histo %s &",(char *)cont.object(i));
        system(buf);
      }

    } // for entries

    this->Layout();




  }// empty block


  //  new TCanvas;
}


TCanvas * InfoWindow::canvas(){
  return c123;
}

void InfoWindow::textclear(){

  if (fTextEdit532!=NULL){

    fTextEdit532->GetText()->Clear();

    fTextEdit532->Clear();

    //fTextEdit532->GetText()->Clear();

    //fTextEdit532->Update();

    //fTextEdit532->Layout();
    //    this->Layout();

  }


  lines=0;
}
void InfoWindow::appendline(const char * fn){
  if (fTextEdit532!=NULL){
    fTextEdit532->GetText()->InsLine(lines,fn);
    lines++;
  }
  //  only once, please.
  // fTextEdit532->Update();
  //fTextEdit532->Layout();
}




void InfoWindow::DoClose(){
  // cout << "I am being closed()"<<endl;
  fMainFrame892->DontCallClose();
}

void InfoWindow::DoResize(){
  // cout << "I am being resized()"<<endl;
}

void InfoWindow::raise(){
  fMainFrame892->RaiseWindow();

}
void InfoWindow::Layout(){


  unsigned int textsize=100;
  int fWidth,fHeight;

  if (fMainFrame892){
    fWidth  = fMainFrame892->GetWidth();
    fHeight = fMainFrame892->GetHeight();

    if (3*textsize>=fMainFrame892->GetHeight())
      textsize=fMainFrame892->GetHeight()/3;

    if (textsize>=fMainFrame892->GetHeight())
      textsize=0;

    if (numTexts > 0){
      if (numGraphs <1){
        textsize = fHeight-24;
      }
    }
    else{
      textsize = 10;
    }

    //std::cout <<" Layout " << textsize<< " "<<numGraphs <<std::endl;



    if ( *alive > 1 ) 
    {
      fRootEmbeddedCanvas514->MoveResize(0,0,fWidth,fHeight-textsize-24);
      fTextEdit532->MoveResize(0,fHeight-textsize-24,fWidth,textsize);
      fStatusBar528->MoveResize(0,fHeight-24,fWidth,24);
    }
    fTextEdit532->Update();
    fTextEdit532->Layout();

  }
  //std::cout << this->kDontCallClose<<std::endl;
  SetBit(kDontCallClose,false);

  //this->kDontCallClose = 0;


  //  fMainFrame892->CallClose();
}

void InfoWindow::display(const int x, const int y)
{

  int textsize = 100;
  // main frame


  // fMainFrame892 = new MyFrame(gClient->GetRoot(),10,10,kMainFrame | kVerticalFrame);

  fMainFrame892 = this;//new MyFrame;
  fMainFrame892->SetWindowName("Camera Message Information");//cpb set the window title
  fMainFrame892->SetLayoutBroken(kTRUE);
  fMainFrame892->DontCallClose();
  //fMainFrame892->Connect("CloseWindow()","InfoWindow",this,"DoClose()");
  // fMainFrame892->Connect("Resize()","InfoWindow",this,"DoResize()");

  fRootEmbeddedCanvas514 = new TRootEmbeddedCanvas(0,fMainFrame892,x,y-textsize-24);
  Int_t wfRootEmbeddedCanvas514 = fRootEmbeddedCanvas514->GetCanvasWindowId();

  c123 = new TCanvas("C0", 10, 10, wfRootEmbeddedCanvas514);
  fRootEmbeddedCanvas514->AdoptCanvas(c123);

  fMainFrame892->AddFrame(fRootEmbeddedCanvas514, 
                          new TGLayoutHints(kLHintsLeft | kLHintsTop| kLHintsExpandX,2,2,2,2));
  //  fRootEmbeddedCanvas514->MoveResize(0,0,x,y-textsize-24);

  // status bar
  fStatusBar528 = new TGStatusBar(fMainFrame892,655,24);

  fMainFrame892->AddFrame(fStatusBar528, new TGLayoutHints(kLHintsBottom | kLHintsExpandX));
  //fStatusBar528->MoveResize(0,y-24,x,24);

  fTextEdit532 = new TGTextEdit(fMainFrame892,653,96);
  //fTextEdit532->LoadFile("TxtEdit532");
  //fTextEdit532->GetText()->Clear();
  //    fTextEdit532->GetText()->InsLine(0,"Hello World 1");
  //fTextEdit532->GetText()->InsLine(0,"Hello World 2");

  fMainFrame892->AddFrame(fTextEdit532, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  //fTextEdit532->MoveResize(0,y-textsize-24,x,textsize);

  fMainFrame892->MapSubwindows();

  //  fMainFrame892->Resize(fMainFrame892->GetDefaultSize());
  fMainFrame892->MapWindow();
  fMainFrame892->Resize(x,y);
  //fStatusBar528->SetText("Ready");
  //std::cout << "Ready"<<std::endl;

  *alive = 2;
}

