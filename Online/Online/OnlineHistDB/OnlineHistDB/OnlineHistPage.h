// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/OnlineHistDB/OnlineHistDB/OnlineHistPage.h,v 1.18 2009-02-16 10:37:42 ggiacomo Exp $
#ifndef ONLINEHISTPAGE_H
#define ONLINEHISTPAGE_H 1
/** @class  OnlineHistPage OnlineHistPage.h OnlineHistDB/OnlineHistPage.h
 * 
 *  Page Class header for the C++ interface to the Online Monitoring Histogram DB
 *  @author Giacomo Graziani (INFN Firenze)
 *  @date 1/12/2006
 */

#include "OnlineHistDB/OnlineHistDBEnv.h"
#include "OnlineHistDB/OnlineHistoOnPage.h"


class OnlineHistPage : public OnlineHistDBEnv
{
 public:
  OnlineHistPage(OnlineHistDBEnv& Env,
                 std::string Name);
  virtual ~OnlineHistPage();
  /// number of histograms on page
  int nh() const {return m_h.size();}
  /// page name (with full path)
  const std::string& name() const { return m_name;}
  /// page folder name
  const std::string& folder() const { return m_folder;}
  /// short page description
  const std::string& doc() const { return m_doc;}
  /// file name containing optional ROOT background pattern 
  const std::string& patternFile() const { return m_pattern;}
  /// check if the page object is in sync with the DB
  const bool syncWithDB() const { return  m_syncWithDB;}
  
  
  /// set short page description
  bool setDoc(std::string &Doc) {m_doc=Doc;return save();}
  /// set file name containing optional ROOT background pattern 
  void setPatternFile(std::string &Pattern) {m_pattern = Pattern;}
  /// adds or updates an histogram on the page. Use instance > 1 to use the
  /// same histogram more than once. Returns the object attached to page (the input
  /// one, or a new copy if a new instance needs to be created), or NULL in case of failure.
  /// Positions are relative to the Pad size (i.e. should be numbers from 0 to 1)
  OnlineHistogram* declareHistogram(OnlineHistogram* h,
                                    double Xmin,
                                    double Ymin,
                                    double Xmax,
                                    double Ymax,
                                    unsigned int instance=1);
  /// adds or updates an histogram that has to be overdrawn on an existing histogram
  /// in case of overlap of several histograms, use OverlapIndex to specify the overlap order
  OnlineHistogram* declareOverlapHistogram(OnlineHistogram* h,
                                           OnlineHistogram* onTopOf,
                                           unsigned int instance=1,
                                           int onTopOfInst=1,
                                           int OverlapIndex = 1);
  /// like declareHistogram, but a new instance is always created
  OnlineHistogram* addHistogram(OnlineHistogram* h,
                                double Xmin,
                                double Ymin,
                                double Xmax,
                                double Ymax);
  /// like declareOvlapHistogram, but a new instance is always created
  OnlineHistogram* addOverlapHistogram(OnlineHistogram* h,
                                       OnlineHistogram* onTopOf=NULL,
                                       int onTopOfInst=1,
                                       int OverlapIndex = 1);
  /// removes histogram from page, or one of its instances, returning true on success
  bool removeHistogram(OnlineHistogram* h,
                       unsigned int instance=1);
  /// clean up the page removing all histograms
  bool removeAllHistograms();
  /// remove the page
  bool remove();
  /// changes Page name 
  void rename(std::string NewName);
  /// fills the hlist vector with pointers to the histograms attached to this page
  void getHistogramList(std::vector<OnlineHistoOnPage*> *hlist) {
    std::vector<OnlineHistoOnPage*>::iterator ix;
    hlist->reserve(hlist->size()+m_h.size());
    for (ix = m_h.begin();ix != m_h.end(); ++ix)
      hlist->push_back(*ix);
  }
  /// saves the current page layout to the DB
  bool save();
  /// dumps the current page layout 
  void dump();
  // load the page from DB
  void load();
private:
  // private dummy copy constructor and assignment operator 
  OnlineHistPage(const OnlineHistPage&) : OnlineHistDBEnv("dummy") {}
  OnlineHistPage & operator= (const OnlineHistPage&) {return *this;}
  //
  std::string m_name;
  std::string m_shortname;
  std::string m_folder;
  std::string m_doc;
  sb2 m_doc_null;
  std::string m_pattern;
  sb2 m_pattern_null;
  bool m_syncWithDB;
  
  std::vector<OnlineHistoOnPage*>::iterator m_ih;
  std::vector<OnlineHistoOnPage*> m_h;
  
  OnlineHistogram* mydeclareHistogram(OnlineHistogram* h,
                                      double Xmin,
                                      double Ymin,
                                      double Xmax,
                                      double Ymax,
                                      unsigned int instance=1,
                                      OnlineHistogram* onTopOf=NULL,
                                      int onTopOfInst=1,
                                      int OverlapIndex = 1);
  OnlineHistoOnPage* findHistogram(OnlineHistogram* h,
                                   unsigned int instance,
                                   bool &knownHisto);
  unsigned int newHistogramInstance(OnlineHistogram* h);
};

class OnlinePageStorage
{
 public:
  OnlinePageStorage() {}
  virtual ~OnlinePageStorage();
  /// get an OnlineHistPage object, to create a new page or view/edit an existing one
  OnlineHistPage* getPage(std::string Name);
  /// alias for getPage
  OnlineHistPage* declarePage(std::string Name) { return getPage(Name); }
  /// removes completely the page, and all associated options (HANDLE WITH CARE!)
  bool removePage(OnlineHistPage* Page);
 protected:
  void setPageEnv(OnlineHistDBEnv* Env) {m_Pagenv = Env;}
  void reloadPages();
 private:
  OnlineHistDBEnv* m_Pagenv;
  std::vector<OnlineHistPage*> m_myPage;
};

#endif // ONLINEHISTPAGE_H
