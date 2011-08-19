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
class MessagePresenter: public  TGMainFrame
{

private:

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
  int GetXtra(const std::string & str, std::string &);


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

  TGTextButton *fTextButton515,*fTextButton516;
  TGTextButton *fTextButtonDump;

  TGNumberEntry *fNumberEntry670;
  TGLabel *fLabel746;

  bool dowarn,doerr,doinfo,dorun;
  int savepos;

  std::string cachefileName,xcachefileName,hostS;
  std::vector<std::string> cachedWarnings;
  bool writeCacheON;

public:
  void DoClose();
  void messageloop(const char * host = "127.0.0.1", const char * file = NULL);
  MessagePresenter();
  ~MessagePresenter(){ }
  void selectWarn();
  void selectErr();
  void selectRun();
  void clearlist();
  void dumpmsg();

  void selectInfo();
  void selectleft();
  void selectright();
  void setup();
  void addwarning(const std::string &,const int ref = 1);

  void getwarnings(const char *);

  void writeCacheFile(const bool force = false);
  void readCacheFile();
  std::string _getCacheFilename(const std::string & _cache_name_);
  std::string getCacheFilename() { return _getCacheFilename("camera_messages.cache"); }
  std::string getXCacheFilename() { return _getCacheFilename("camera_extra_info.cache"); }
  void clearCacheFile();

  void UpdateView();
  void UpdateRight();

  ClassDef(MessagePresenter,1);

};

#endif
