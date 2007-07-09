// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/OnlineHistDB/OnlineHistDB/OnlineHistPage.h,v 1.5 2007-07-09 10:17:41 ggiacomo Exp $
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
		 OnlineHistogramStorage* Storage,
		 std::string Name, 
		 std::string Folder);
  virtual ~OnlineHistPage();
  bool setDoc(std::string Doc) {m_doc=Doc;return save();}
  bool setFolder(std::string Folder) {m_folder=Folder;return save();}
  bool declareHistogram(OnlineHistogram* h,
		    float Cx,
		    float Cy,
		    float Sx,
		    float Sy,
		    unsigned int instance=1);
  bool removeHistogram(OnlineHistogram* h,
		       unsigned int instance=1);
  const std::vector<OnlineHistogram*>& hlist() {return m_h;}
  bool getHistLayout(OnlineHistogram* h,
		     float &Cx,
		     float &Cy,
		     float &Sx,
		     float &Sy,
		     unsigned int instance=1) const;
  int nh() const {return m_h.size();}
  const std::string& name() const { return m_name;}
  const std::string& folder() const { return m_folder;}
  const std::string& doc() const { return m_doc;}

 private:
  // private dummy copy constructor and assignment operator 
  OnlineHistPage(const OnlineHistPage&) : OnlineHistDBEnv("dummy") {}
  OnlineHistPage & operator= (const OnlineHistPage&) {return *this;}
  //
  OnlineHistogramStorage* m_Hstorage;
  std::string m_name;
  std::string m_folder;
  std::string m_doc;
  std::vector<OnlineHistogram*> m_h;
  std::vector<float> m_cx;
  std::vector<float> m_cy;
  std::vector<float> m_sx;
  std::vector<float> m_sy;
  int findHistogram(OnlineHistogram* h,
		    unsigned int instance=1) const;
  bool save();
};

class OnlinePageStorage
{
 public:
  OnlinePageStorage(OnlineHistDBEnv* Env, OnlineHistogramStorage* Hstorage);
  virtual ~OnlinePageStorage();
  /// get an OnlineHistPage object, to create a new page or view/edit an existing one
  OnlineHistPage* getPage(std::string Name, std::string Folder="");
 private:
  OnlineHistDBEnv* m_Pagenv;
  OnlineHistogramStorage* m_Hstorage;
  std::vector<OnlineHistPage*> m_myPage;
};

#endif // ONLINEHISTPAGE_H
