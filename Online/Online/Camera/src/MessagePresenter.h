#ifndef MESSAGEPRESENTER_H
#define MESSAGEPRESENTER_H


#include <map>
#include <vector>

#include "container_ROOT.h"


#include "TApplication.h" 



#include <TGMdiMainFrame.h> 
#include <TGButton.h> 
#include <TGListBox.h>


#include <TGStatusBar.h>


// Must be here such that we can compile the dictionary. Forward declaration is not enough.
//#ifdef _WIN32
//#include "TClient.h"
//#endif


//#ifndef _WIN32
//#include "client.h"
//#endif


#include "display.h"
#include <TString.h>
class MessagePresenter: public  TGMainFrame{
 
  InfoWindow *iw;
  int iwAlive ;
  container_ROOT cont;
  int lastleft;
  
  int lastright;

  Pixel_t Pred,Pgreen,Pyellow;

  static const int MAXLINES=50;
  static const bool FLITEOUT=false;

  std::vector<void* > socklist;

 public:
  void display();
private:  
  TString savdir;//(".");
  TString savname;//(".");
  void Layout();
  int GetXtra(std::string str, std::string &);
  
  
  std::vector<std::string> allpairs;
  
  std::vector<std::string> names;
  
  std::vector<std::string> extradata;
  
  std::map<std::string,int> keys;
  std::map<std::string,int> levels;

  std::map<std::string,std::vector<std::string>* > values;
  std::map<std::string,std::vector<int> > vallevels;
  
  

  TGStatusBar *fStatusBar528;

  TGMainFrame *fMainFrame1933; 
  TGCheckButton *fTextButton514; 
  TGCheckButton *fTextButton659; 

  TGCheckButton *fTextButton699; 

  TGCheckButton *fstopped; 

  TGListBox *fListBox863 ; 
  TGListBox *fListBox816;

  TGTextButton *fTextButton515;
  TGTextButton *fTextButtonDump;

  TGNumberEntry *fNumberEntry670;
  TGLabel *fLabel746;
 
  bool dowarn,doerr,doinfo,dorun;
  int savepos;

 public:  
  void DoClose(); 
  void messageloop(char * host = "127.0.0.1",char * file =NULL);
  MessagePresenter();
  ~MessagePresenter(){}
  void selectWarn();
  void selectErr();
  void selectRun();
  void clearlist(){allpairs.clear();UpdateRight();}
  void dumpmsg();
  
  void selectInfo();
  void selectleft();
  void selectright();
  void setup();
  void addwarning(std::string,int ref = 1);

  void getwarnings(char *);

  void UpdateView();
  void UpdateRight();
  ClassDef(MessagePresenter,1)
};
#endif
