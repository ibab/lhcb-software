
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
  alive = inalive;
  lines=0;
  *alive =1;
}

InfoWindow::~InfoWindow(){
  *alive =0;
  cont.reset();
  //  std::cerr << "I am no more" << std::endl;

}
void InfoWindow::ShowCont(std::string ins){

  std::string cfile =ins;


  //  container_ROOT cont;

  {

    cont.reset();
    cont.fromfile(cfile.c_str());
    cont.convert();
    int nrHistos=0;


    for (int i=0;i<cont.entries();++i){

      std::size_t pos = cont.name(i).find('.');
      std::string cn, cna;
      cn = cont.name(i);
      cna = "";

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
    //if (iwAlive < 2)
    // this->display();
    this->textclear();

    int HistoCnt = 1;


    this ->canvas()->Clear();
    this ->canvas()->Update();

    if (nrHistos>1){
      int divx=0;
      int divy=0;

      while (divx*divy<nrHistos){

        if (nrHistos > divx*divy) divy+=1;
        if (nrHistos > divx*divy) divx+=1;

      }

      //      cout << "dividing in "<<divx <<", "<<divy<<" Pads" <<endl;
      //cout << nrHistos<<" "<<endl;
      this ->canvas()->Divide(divx,divy);
    }



    for (int i=0;i<cont.entries();++i){
      if (cont.object(i) == NULL) continue;


      std::size_t pos = cont.name(i).find('.');
      std::string cn, cna;
      cn = cont.name(i);
      cna = "";

      if (pos!=std::string::npos) cn = cont.name(i).substr(0,pos);
      if (pos!=std::string::npos) cna = cont.name(i).substr(pos+1);

      if (cn=="EVAL"){
        char * v = (char *)cont.object(i);

        gROOT->ProcessLine(v);
        this->canvas()->Update();
      }

      if (cn=="DIM2D"){

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

        // this->appendline("1D Histo ");

        if ((cna.find("SAME") ==std::string::npos)&& (cna.find("same") ==std::string::npos) ) this->canvas()->cd(HistoCnt++);
        TF1 * v = (TF1 *)cont.object(i);
        //cout << cna<<endl;
        if (cna == "")
          v->Draw();
        else
          v->Draw(cna.c_str());
        this->canvas()->Update();
      }

      if (cn=="TF2"){

        // this->appendline("1D Histo ");

        if ((cna.find("SAME") ==std::string::npos)&& (cna.find("same") ==std::string::npos) ) this->canvas()->cd(HistoCnt++);
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

      }
      if (cont.name(i)=="CTIME"){
        // this->appendline("Date/Time: ");
        this->appendline((char *)cont.object(i));
      }


      if ((cont.name(i)=="TELLIPSE")||(cont.name(i)=="TTELLIPSE" )){

        // this->appendline("Ellipse");

        // this->canvas()->cd(HistoCnt++);
        TEllipse * v = (TEllipse *)cont.object(i);
        v->Draw();
        this->canvas()->Update();
      }


      if ((cont.name(i)=="TBOX")||(cont.name(i)=="TTBOX" )){

        // this->appendline("Box");

        // this->canvas()->cd(HistoCnt++);
        TBox * v = (TBox *)cont.object(i);
        v->Draw();
        this->canvas()->Update();
      }


      if ((cont.name(i)=="TLINE")||(cont.name(i)=="TTLINE" )){

        // this->appendline("Line");

        // this->canvas()->cd(HistoCnt++);
        TLine * v = (TLine *)cont.object(i);
        v->Draw();
        this->canvas()->Update();
      }

      if ((cont.name(i)=="TARROW")||(cont.name(i)=="TTARROW" )){

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

    }

  }

  //  new TCanvas;
}


TCanvas * InfoWindow::canvas(){return c123;}

void InfoWindow::textclear(){   fTextEdit532->GetText()->Clear(); fTextEdit532->Layout(); lines=0;}
void InfoWindow::appendline(const char * fn){fTextEdit532->GetText()->InsLine(lines,fn);lines++; fTextEdit532->Update(); fTextEdit532->Layout(); }




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

  if (3*textsize>=fMainFrame892->GetHeight())
    textsize=fMainFrame892->GetHeight()/3;

  if (textsize>=fMainFrame892->GetHeight())
    textsize=0;

  int fWidth =  fMainFrame892->GetWidth();
  int fHeight =  fMainFrame892->GetHeight();
  fRootEmbeddedCanvas514->MoveResize(0,0,fWidth,fHeight-textsize-24);
  fTextEdit532->MoveResize(0,fHeight-textsize-24,fWidth,textsize);
  fStatusBar528->MoveResize(0,fHeight-24,fWidth,24);
}

void InfoWindow::display(int x,int y)
{

  int textsize = 100;
  // main frame


  // fMainFrame892 = new MyFrame(gClient->GetRoot(),10,10,kMainFrame | kVerticalFrame);

  fMainFrame892 = this;//new MyFrame;

  fMainFrame892->SetLayoutBroken(kTRUE);

  fMainFrame892->Connect("CloseWindow()","InfoWindow",this,"DoClose()");
  // fMainFrame892->Connect("Resize()","InfoWindow",this,"DoResize()");

  fRootEmbeddedCanvas514 = new TRootEmbeddedCanvas(0,fMainFrame892,x,y-textsize-24);
  Int_t wfRootEmbeddedCanvas514 = fRootEmbeddedCanvas514->GetCanvasWindowId();

  c123 = new TCanvas("C0", 10, 10, wfRootEmbeddedCanvas514);
  fRootEmbeddedCanvas514->AdoptCanvas(c123);

  fMainFrame892->AddFrame(fRootEmbeddedCanvas514, new TGLayoutHints(kLHintsLeft | kLHintsTop| kLHintsExpandX,2,2,2,2));
  fRootEmbeddedCanvas514->MoveResize(0,0,x,y-textsize-24);

  // status bar
  fStatusBar528 = new TGStatusBar(fMainFrame892,655,24);

  fMainFrame892->AddFrame(fStatusBar528, new TGLayoutHints(kLHintsBottom | kLHintsExpandX));
  fStatusBar528->MoveResize(0,y-24,x,24);

  fTextEdit532 = new TGTextEdit(fMainFrame892,653,96);
  //fTextEdit532->LoadFile("TxtEdit532");
  //fTextEdit532->GetText()->Clear();
  //    fTextEdit532->GetText()->InsLine(0,"Hello World 1");
  //fTextEdit532->GetText()->InsLine(0,"Hello World 2");

  fMainFrame892->AddFrame(fTextEdit532, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  fTextEdit532->MoveResize(0,y-textsize-24,x,textsize);

  fMainFrame892->MapSubwindows();

  //  fMainFrame892->Resize(fMainFrame892->GetDefaultSize());
  fMainFrame892->MapWindow();
  fMainFrame892->Resize(x,y);
  fStatusBar528->SetText("Ready");
  *alive = 2;
}

