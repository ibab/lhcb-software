//$Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/OnlineHistDB/OnlineHistDB/OnlineRootHist.h,v 1.12 2007-11-22 18:23:40 ggiacomo Exp $
#ifndef ONLINEROOTHIST_H
#define ONLINEROOTHIST_H 1
#include "OnlineHistDB/OnlineHistDB.h"
#include "OnlineHistDB/OnlineHistogram.h"
#include <TH1.h>
#include <TPad.h>

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
  virtual ~OnlineRootHist();
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
  void setDrawOptionsFromDB(TPad* &Pad);
  /// saves current ROOT display options to OnlineHistogram object and to Histogram DB
  bool saveTH1ToDB(TPad* Pad = NULL);
  // OnlineHistogram methods for setting display options  
  /// sets display option (see OnlineHistogram method). Change is sent to the DB only 
  /// after a call to saveTH1ToDB()
  virtual bool setDisplayOption(std::string ParameterName, 
				void* value);
  /// save provided histogram as a reference in the standard file, with optional run period and data type
  bool setReference(TH1 *ref,
		    int startrun = 1,
		    std::string DataType = "default");
  /// get reference histogram if available
  TH1* getReference(int startrun = 1,
		    std::string DataType = "default");
  // TH1 drawing methods
  /// calls TH1 Draw method, calls setDrawOptions()
  virtual void Draw(TPad* &Pad);
  /// normalize reference (if existing and requested) to current plot
  void normalizeReference();

 private:
  bool updateDBOption(std::string opt, void *value, bool isdefault);
  void drawReference();
  std::string m_identifier;
  OnlineHistDB *m_session;
  OnlineHistogram* m_dbHist;
  TH1* m_rootHist;

  std::string m_refOption;
  TH1* m_reference;
  int m_startrun;
  std::string m_DataType;
};


#endif // ONLINEROOTHIST_H
