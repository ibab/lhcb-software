#ifndef DISK_RESIDENT_EVENT_LIST_HH
#define DISK_RESIDENT_EVENT_LIST_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:00 GMT

#include "Mint/IDalitzEvent.h"
#include "Mint/IDalitzEventList.h"
#include "Mint/IGetDalitzEvent.h"
#include "Mint/DalitzEvent.h"
#include "Mint/DalitzEventPattern.h"
#include "Mint/DalitzHistoSet.h"
#include "Mint/counted_ptr.h"

#include <string>

class TNtupleD;
class TFile;

/* 
   WARNING: Event pointer returned by this list are only
   valid until the next operation on the list is carried out,
   so if you want to use the event longer, use getEventCopy.
 */
class DiskResidentEventList : virtual public IDalitzEventList{
 protected:
  MINT::counted_ptr<DalitzEvent> _currentEvent;
  mutable MINT::counted_ptr<DalitzEvent> _rEvent;
  std::string _fname;
  std::string _opt;
  double _scaleData;



  TFile* _f; // object ownership with gDirectory
  TNtupleD* _ntp; // object ownership with gDirectory
  bool _initialised;
  unsigned int currentPosition, nextPosition;

  static int __maxBytes;
  std::string cName();
  std::string ntpName();

  bool openFile();
  bool makeNtp(const DalitzEvent& evt);
  bool makeNewFile();
  bool fromFile();

  bool init();
  bool Start_noInit();
 public:

  DiskResidentEventList();
//  DiskResidentEventList(const std::string& fname
//		  	, const std::string& treeName = "DalitzEventList"
//			, const std::string& opt="UPDATE");

  DiskResidentEventList(const std::string& fname
			, const std::string& opt="UPDATE"
);
  DiskResidentEventList(const std::string& fname
		    , int scales
			, const std::string& treeName
			, const std::string& opt="UPDATE"
);
  //  DiskResidentEventList(TNtupleD* ntp);
  DiskResidentEventList(const DalitzEventPattern& pat
			);
  DiskResidentEventList(const DalitzEventPattern& pat
			, const std::string& fname
			, const std::string& opt="RECREATE");

  DiskResidentEventList(const IDalitzEventList& otherList
			);
  DiskResidentEventList(const IDalitzEventList& otherList
			, const std::string& newFname
			, const std::string& opt="RECREATE"
			);

  virtual ~DiskResidentEventList();

  static std::string generateFname();
 
  virtual unsigned int size() const;
  virtual bool empty() const;
  virtual bool Start();
  virtual bool Next();
  bool curryCheck();
  virtual const IDalitzEvent* getEvent() const;
  virtual IDalitzEvent* getEvent();

  const IDalitzEvent* getREvent(unsigned int i) const;
  MINT::counted_ptr<IDalitzEvent> getEventCopy(unsigned int i) const;

  virtual bool Add(const DalitzEvent& evt);
  bool Add(const IDalitzEvent* evt);
  bool Add(const MINT::counted_ptr<IDalitzEvent>& evt);
  bool Add(const IDalitzEventList& otherList);

  bool save();
  bool Close();
    

  DalitzHistoSet histoSet() const;
  DalitzHistoSet weightedHistoSet()const;
  DalitzHistoSet reWeightedHistoSet(IGetDalitzEvent* w) const;
  DalitzHistoSet weighedReWeightedHistoSet(IGetDalitzEvent* w) const;

  bool makePlots(const std::string& filename);


  DalitzEvent operator()(unsigned int i);
 private:
  std::string _cName;
  std::string _ntpName;

};

#endif
//
