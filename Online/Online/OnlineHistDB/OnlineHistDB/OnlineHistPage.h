// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/OnlineHistDB/OnlineHistDB/OnlineHistPage.h,v 1.12 2007-11-08 16:18:51 ggiacomo Exp $
#ifndef ONLINEHISTPAGE_H
#define ONLINEHISTPAGE_H 1
/** @class  OnlineHistPage OnlineHistPage.h OnlineHistDB/OnlineHistPage.h
 * 
 *  Page Class header for the C++ interface to the Online Monitoring Histogram DB
 *  @author Giacomo Graziani (INFN Firenze)
 *  @date 1/12/2006
 */

#include "OnlineHistDB/OnlineHistDBEnv.h"
#include "OnlineHistDB/OnlineHistogram.h"


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
  /// check if the page object is in sync with the DB
  const bool syncWithDB() const { return  m_syncWithDB;}


  /// set short page description
  bool setDoc(std::string Doc) {m_doc=Doc;return save();}
  /// adds or updates an histogram on the page. Use instance > 1 to use the
  /// same histogram more than once. Returns the object attached to page (the input
  /// one, or a new copy if a new instance needs to be created), or NULL in case of failure. 
  OnlineHistogram* declareHistogram(OnlineHistogram* h,
				    double Cx,
				    double Cy,
				    double Sx,
				    double Sy,
				    unsigned int instance=1);
  /// like declareHistogram, but a new instance is always created
  OnlineHistogram* addHistogram(OnlineHistogram* h,
				double Cx,
				double Cy,
				double Sx,
				double Sy);
  /// removes histogram from page, or one of its instances, returning true on success
  bool removeHistogram(OnlineHistogram* h,
		       unsigned int instance=1);
  /// clean up the page removing all histograms
  bool removeAllHistograms();
  bool remove();
  /// fills the hlist vector with pointers to the histograms attached to this page
  void getHistogramList(std::vector<OnlineHistogram*> *hlist) {
    std::vector<OnlineHistogram*>::iterator ix;
    hlist->reserve(hlist->size()+m_h.size());
    for (ix = m_h.begin();ix != m_h.end(); ++ix)
      hlist->push_back(*ix);
  }
  /// get the layout of given histogram. returns false if histogram is not found
  bool getHistLayout(OnlineHistogram* h,
		     double &Cx,
		     double &Cy,
		     double &Sx,
		     double &Sy,
		     unsigned int instance=1) const;
  /// saves the current page layout to the DB
  bool save();
  /// dumps the current page layout 
  void dump();

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
  bool m_syncWithDB;
  std::vector<OnlineHistogram*> m_h;
  std::vector<float> m_cx;
  std::vector<float> m_cy;
  std::vector<float> m_sx;
  std::vector<float> m_sy;
  int findHistogram(OnlineHistogram* h,
		    unsigned int instance,
		    bool &knownHisto) const;
  unsigned int newHistogramInstance(OnlineHistogram* h);
  unsigned int HistogramInstance(unsigned int xh) const;
};

class OnlinePageStorage
{
 public:
  OnlinePageStorage(OnlineHistDBEnv* Env);
  virtual ~OnlinePageStorage();
  /// get an OnlineHistPage object, to create a new page or view/edit an existing one
  OnlineHistPage* getPage(std::string Name);
  /// removes completely the page, and all associated options (HANDLE WITH CARE!)
  bool removePage(OnlineHistPage* Page);
 private:
  OnlineHistDBEnv* m_Pagenv;
  std::vector<OnlineHistPage*> m_myPage;
};

#endif // ONLINEHISTPAGE_H
