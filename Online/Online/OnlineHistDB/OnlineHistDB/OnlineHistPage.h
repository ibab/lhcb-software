// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/OnlineHistDB/OnlineHistDB/OnlineHistPage.h,v 1.2 2007-01-22 17:07:31 ggiacomo Exp $
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
  OnlineHistPage(std::string Name, 
		 std::string Folder,
		 Connection* Conn);
  virtual ~OnlineHistPage();
  void setDoc(std::string Doc) {m_doc=Doc;}
  void setFolder(std::string Folder) {m_folder=Folder;}
  void declareHistogram(OnlineHistogram* h,
		    float Cx,
		    float Cy,
		    float Sx,
		    float Sy,
		    unsigned int instance=1);
  void removeHistogram(OnlineHistogram* h,
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
  std::string m_name;
  std::string m_folder;
  std::string m_doc;
  std::vector<OnlineHistogram*> m_h;
  std::vector<OnlineHistogram*> m_privh;
  std::vector<float> m_cx;
  std::vector<float> m_cy;
  std::vector<float> m_sx;
  std::vector<float> m_sy;
  int findHistogram(OnlineHistogram* h,
		    unsigned int instance=1) const;
  void save();
};

#endif // ONLINEHISTPAGE_H
