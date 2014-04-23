#ifndef WEIGHTED_COUNT_HH
#define WEIGHTED_COUNT_HH

#include "Mint/DiskResidentEventList.h"
#include "EventCounter.h"
#include "Mint/DalitzHistoSet.h"
#include "Mint/IGetDalitzEvent.h"

#include <string>

class IDalitzPdf;
class readPhilipsNtuple;

class WeightedCount{

  IGetDalitzEvent* _LHCbPdf;
  std::vector<IGetDalitzEvent*> _MarkIIIPdf; // list of models;
  readPhilipsNtuple* _ntpReader;

  std::string _name;
  EventCounter _counter;


  DiskResidentEventList _unweightedList, _reweightedList;

 public:
  WeightedCount(const std::string& name
		, IGetDalitzEvent* LHCb
		, const std::vector<IGetDalitzEvent*>& MarkIII
		, readPhilipsNtuple* ntp
		);

  bool OK() const;
  const std::string& name() const{return _name;}
  bool doCount();
  const EventCounter& getCounter()const{return _counter;}
  DalitzHistoSet getUnweightedHistos() const;
  DalitzHistoSet getReweightedHistos() const;

  bool drawAndSave();
};

#endif
//
