//--------------------------------------------------------------------------
//
// Environment:
//      This software is part of the EvtGen package developed jointly
//      for the BaBar and CLEO collaborations.  If you use all or part
//      of it, please give an appropriate acknowledgement.
//
// Copyright Information: See EvtGen/COPYRIGHT
//      Copyright (C) 1998      Caltech, UCSB
//
// Module: EvtGen/EvtId.hh
//
// Description:Class for particle Id used in EvtGen.
//
// Modification history:
//
//    DJL/RYD     Jan 5,2000         Module created
//
//------------------------------------------------------------------------

#ifndef EVTIDSET_HH
#define EVTIDSET_HH

#include "EvtGen/EvtId.hh"
#include <iostream>

class EvtString;
class EvtId;

class EvtIdSet {

public:

  //need a default constructor

  EvtIdSet(const EvtId name1);
  EvtIdSet(const EvtString name1);

  EvtIdSet(const EvtId name1,
	   const EvtId name2);

  EvtIdSet(const EvtString name1,
	   const EvtString name2);

  EvtIdSet(const EvtId name1,
	   const EvtId name2,
	   const EvtId name3);

  EvtIdSet(const EvtString name1,
	   const EvtString name2,
	   const EvtString name3);

  EvtIdSet(const EvtId name1,
	   const EvtId name2,
	   const EvtId name3,
	   const EvtId name4);

  EvtIdSet(const EvtString name1,
	   const EvtString name2,
	   const EvtString name3,
	   const EvtString name4);

  EvtIdSet(const EvtId name1,
	   const EvtId name2,
	   const EvtId name3,
	   const EvtId name4,
	   const EvtId name5);

  EvtIdSet(const EvtString name1,
	   const EvtString name2,
	   const EvtString name3,
	   const EvtString name4,
	   const EvtString name5);

  EvtIdSet(const EvtId name1,
	   const EvtId name2,
	   const EvtId name3,
	   const EvtId name4,
	   const EvtId name5,
	   const EvtId name6);

  EvtIdSet(const EvtString name1,
	   const EvtString name2,
	   const EvtString name3,
	   const EvtString name4,
	   const EvtString name5,
	   const EvtString name6);

  EvtIdSet(const EvtId name1,
	   const EvtId name2,
	   const EvtId name3,
	   const EvtId name4,
	   const EvtId name5,
	   const EvtId name6,
	   const EvtId name7);

  EvtIdSet(const EvtString name1,
	   const EvtString name2,
	   const EvtString name3,
	   const EvtString name4,
	   const EvtString name5,
	   const EvtString name6,
	   const EvtString name7);

  EvtIdSet(const EvtId name1,
	   const EvtId name2,
	   const EvtId name3,
	   const EvtId name4,
	   const EvtId name5,
	   const EvtId name6,
	   const EvtId name7,
	   const EvtId name8);

  EvtIdSet(const EvtString name1,
	   const EvtString name2,
	   const EvtString name3,
	   const EvtString name4,
	   const EvtString name5,
	   const EvtString name6,
	   const EvtString name7,
	   const EvtString name8);

  EvtIdSet(const EvtId name1,
	   const EvtId name2,
	   const EvtId name3,
	   const EvtId name4,
	   const EvtId name5,
	   const EvtId name6,
	   const EvtId name7,
	   const EvtId name8,
	   const EvtId name9);

  EvtIdSet(const EvtString name1,
	   const EvtString name2,
	   const EvtString name3,
	   const EvtString name4,
	   const EvtString name5,
	   const EvtString name6,
	   const EvtString name7,
	   const EvtString name8,
	   const EvtString name9);

  EvtIdSet(const EvtId name1,
	   const EvtId name2,
	   const EvtId name3,
	   const EvtId name4,
	   const EvtId name5,
	   const EvtId name6,
	   const EvtId name7,
	   const EvtId name8,
	   const EvtId name9,
	   const EvtId name10);

  EvtIdSet(const EvtString name1,
	   const EvtString name2,
	   const EvtString name3,
	   const EvtString name4,
	   const EvtString name5,
	   const EvtString name6,
	   const EvtString name7,
	   const EvtString name8,
	   const EvtString name9,
	   const EvtString name10);

  EvtIdSet(const EvtId name1,
	   const EvtId name2,
	   const EvtId name3,
	   const EvtId name4,
	   const EvtId name5,
	   const EvtId name6,
	   const EvtId name7,
	   const EvtId name8,
	   const EvtId name9,
	   const EvtId name10,
	   const EvtId name11);

  EvtIdSet(const EvtString name1,
	   const EvtString name2,
	   const EvtString name3,
	   const EvtString name4,
	   const EvtString name5,
	   const EvtString name6,
	   const EvtString name7,
	   const EvtString name8,
	   const EvtString name9,
	   const EvtString name10,
	   const EvtString name11);

  EvtIdSet(const EvtId name1,
	   const EvtId name2,
	   const EvtId name3,
	   const EvtId name4,
	   const EvtId name5,
	   const EvtId name6,
	   const EvtId name7,
	   const EvtId name8,
	   const EvtId name9,
	   const EvtId name10,
	   const EvtId name11,
	   const EvtId name12);

  EvtIdSet(const EvtString name1,
	   const EvtString name2,
	   const EvtString name3,
	   const EvtString name4,
	   const EvtString name5,
	   const EvtString name6,
	   const EvtString name7,
	   const EvtString name8,
	   const EvtString name9,
	   const EvtString name10,
	   const EvtString name11,
	   const EvtString name12);

  ~EvtIdSet() { delete [] _list;  }

  EvtIdSet(const EvtIdSet& set1);
  EvtIdSet(const EvtIdSet& set1, const EvtIdSet& set2);

  int contains(const EvtId id);
  int contains(const EvtString id);

  void append(const EvtIdSet set1);
  int sizeOfSet() const;
  EvtId getElem(const int i) const;

private:

  int _numInList;
  EvtId *_list;
  
}; 

#endif

