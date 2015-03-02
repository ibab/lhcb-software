#ifndef DISK_RESIDENT_EVENT_LIST_HH
#define DISK_RESIDENT_EVENT_LIST_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:00 GMT

#include "Mint/IMinimalEventList.h"
#include "Mint/IReturnRealForEvent.h"
#include "Mint/IReturnComplexForEvent.h"

#include "Mint/IDalitzEvent.h"
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
class DiskResidentEventList : virtual public MINT::IMinimalEventList<DalitzEvent>{
 protected:
  std::string _fname;
  std::string _opt;

  TFile* _f; // object ownership with gDirectory
  TNtupleD* _ntp; // object ownership with gDirectory
  double _scaleData;


  bool _initialised;

  static int __maxBytes;
  std::string cName();
  std::string ntpName();

  bool openFile();
  bool makeNtp(const DalitzEvent& evt);
  bool makeNewFile();
  bool fromFile();

  bool init();
 public:

  DiskResidentEventList();

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

  DiskResidentEventList(const MINT::IMinimalEventList<DalitzEvent>& otherList
			);
  //DiskResidentEventList(const MINT::IMinimalEventList<IDalitzEvent>& otherList
  //			);
  DiskResidentEventList(const MINT::IMinimalEventList<DalitzEvent>& otherList
			, const std::string& newFname
			, const std::string& opt="RECREATE"
			);
  //DiskResidentEventList(const MINT::IMinimalEventList<IDalitzEvent>& otherList
  //			, const std::string& newFname
  //			, const std::string& opt="RECREATE"
  //			);

  virtual ~DiskResidentEventList();

  static std::string generateFname();
 
  virtual unsigned int size() const;
  virtual bool empty() const;

  // DalitzEvent operator[](unsigned int i) const;
  DalitzEvent getEvent(unsigned int i) const;

  virtual bool Add(const DalitzEvent& evt);
  bool Add(const IDalitzEvent& evt);
  bool Add(const IDalitzEvent* evt); // for backward compatibility
  bool Add(const MINT::counted_ptr<IDalitzEvent>& evt);
  bool Add(const MINT::IMinimalEventList<DalitzEvent>& otherList);
  //bool Add(const MINT::IMinimalEventList<IDalitzEvent>& otherList);

  bool save();
  bool Close();
    

  DalitzHistoSet histoSet() const;
  DalitzHistoSet weightedHistoSet()const;
  DalitzHistoSet reWeightedHistoSet(MINT::IReturnRealForEvent<IDalitzEvent>* w) const;
  DalitzHistoSet weighedReWeightedHistoSet(MINT::IReturnRealForEvent<IDalitzEvent>* w) const;

  bool makePlots(const std::string& filename);


  DalitzEvent operator()(unsigned int i);
 private:
  std::string _cName;
  std::string _ntpName;

};

#endif
//
