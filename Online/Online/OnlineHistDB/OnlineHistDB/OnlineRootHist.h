//$Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/OnlineHistDB/OnlineHistDB/OnlineRootHist.h,v 1.9 2007-10-16 12:16:10 ggiacomo Exp $
#ifndef ONLINEROOTHIST_H
#define ONLINEROOTHIST_H 1
#include "OnlineHistDB/OnlineHistDB.h"
#include "OnlineHistDB/OnlineHistogram.h"
#include "TH1.h"
#include "TStyle.h"

class OnlineRootHist 
{
 public:
  /// constructor using identifier and possibly DB session
  OnlineRootHist(std::string Identifier,
		 OnlineHistDB *Session = NULL,
		 std::string Page="_NONE_",
		 int Instance=1);
  /// constructor from existing OnlineHistogram object
  OnlineRootHist(OnlineHistogram*  oh);
  virtual ~OnlineRootHist() {};
  /// histogram identifier
  std::string identifier()  { return m_identifier;}
  /// corresponding OnlineHistogram object
  OnlineHistogram* dbHist() {return m_dbHist;}
  /// corresponding ROOT TH1 object
  TH1* rootHist() {return m_rootHist;}
  /// link to an existing OnlineHistogram object, returns true on success
  virtual bool setdbHist(OnlineHistogram*  oh);
  /// link to an existing ROOT TH1 object, returns true on success
  bool setrootHist(TH1*  rh);
  /// true if object knows an existing DB session
  bool connected() {return (m_session != NULL);}
  /// returns link to DB session
  OnlineHistDB* dbSession() {return m_session;}
  /// connect to DB session, returns true if the corresponding DB histogram entry is found
  bool connectToDB(OnlineHistDB* Session,
		   std::string Page="_NONE_",
		   int Instance=1);
  /// updates ROOT TH1 display properties from Histogram DB (via OnlineHistogram object) 
  /// (normally called when connecting)
  void setTH1FromDB();
  /// updates current drawing options from Histogram DB (via OnlineHistogram object)
  void setDrawOptionsFromDB();
  /// saves current ROOT display options to OnlineHistogram object and to Histogram DB
  bool saveTH1ToDB();
  // OnlineHistogram methods for setting display options  
  virtual bool setHistoSetDisplayOption(std::string ParameterName, 
					void* value);
  virtual bool setHistDisplayOption(std::string ParameterName, 
				    void* value);
  virtual bool setHistoPageDisplayOption(std::string ParameterName, 
					 void* value,
					 std::string PageName = "_DEFAULT_",
					 int Instance=-1);
  virtual bool setDisplayOption(std::string ParameterName, 
				void* value);

  // TH1 drawing methods
  /// calls TH1 Draw method, calls setDrawOptions()
  virtual void Draw();

 private:
  std::string m_identifier;
  OnlineHistDB *m_session;
  OnlineHistogram* m_dbHist;
  TH1* m_rootHist;
};


#endif // ONLINEROOTHIST_H
