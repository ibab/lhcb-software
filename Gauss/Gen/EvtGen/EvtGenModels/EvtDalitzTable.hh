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
// Module: EvtGen/EvtGenericDalitz.hh
//
// Description: Model to describe a generic dalitz decay
//
// Modification history:
//
//    DCC     16 December, 2011         Module created
//
//------------------------------------------------------------------------

#ifndef EVTDALITZTABLE_HPP
#define EVTDALITZTABLE_HPP

#include "EvtGenModels/EvtDalitzDecayInfo.hh"
#include "EvtGenBase/EvtId.hh"

#include <map>
#include <string>
#include <vector>

class EvtDalitzTable {
public:

  static EvtDalitzTable* getInstance(const std::string dec_name="", bool verbose=true);

  bool fileHasBeenRead(const std::string dec_name);
  void readXMLDecayFile(const std::string dec_name, bool verbose=true);
  void checkParticle(std::string particle);

  void addDecay(EvtId parent, const EvtDalitzDecayInfo& dec);
  void copyDecay(EvtId parent, EvtId* daughters, EvtId copy, EvtId* copyd);

  std::vector<EvtDalitzDecayInfo> getDalitzTable(const EvtId& parent);

protected:

  EvtDalitzTable();
  ~EvtDalitzTable();

private:

  std::map<EvtId, std::vector<EvtDalitzDecayInfo> > _dalitztable;
  std::vector<std::string> _readFiles;

  EvtDalitzTable(const EvtDalitzTable&);
  EvtDalitzTable& operator=(const EvtDalitzTable&);

};

#endif
