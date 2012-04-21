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

#include "EvtGenModels/EvtDalitzTable.hh"
#include "EvtGenBase/EvtReport.hh"
#include "EvtGenBase/EvtParserXml.hh"
#include "EvtGenBase/EvtPDL.hh"
#include "EvtGenBase/EvtSpinType.hh"

#include <stdlib.h>
#include <sstream>

using std::endl;
using std::fstream;
using std::ifstream;

EvtDalitzTable::EvtDalitzTable() {
  _dalitztable.clear();
  _readFiles.clear();
}

EvtDalitzTable::~EvtDalitzTable() {
  _dalitztable.clear();
  _readFiles.clear();
}

EvtDalitzTable* EvtDalitzTable::getInstance(const std::string dec_name, bool verbose) { 

  static EvtDalitzTable* theDalitzTable = 0;

  if(theDalitzTable == 0) {
    theDalitzTable = new EvtDalitzTable();
  }

  if(!theDalitzTable->fileHasBeenRead(dec_name)) {
    theDalitzTable->readXMLDecayFile(dec_name,verbose);
  }

  return theDalitzTable;

}

bool EvtDalitzTable::fileHasBeenRead(const std::string dec_name) {
  std::vector<std::string>::iterator i = _readFiles.begin();
  for( ; i!=_readFiles.end(); i++) {
    if((*i).compare(dec_name) == 0) {
      return true;
    }
  }
  return false;
}

void EvtDalitzTable::readXMLDecayFile(const std::string dec_name, bool verbose){

  _readFiles.push_back(dec_name);

  EvtDalitzDecayInfo* dalitzDecay = 0;
  EvtResonancePrototype* resonance = 0;
  EvtId ipar;
  std::string decayParent = "";

  EvtParserXml parser;
  parser.open(dec_name);

  bool endReached = false;

  while(parser.readNextTag()) {
    //TAGS FOUND UNDER DATA
    if(parser.getParentTagTitle() == "data") {
      if(parser.getTagTitle() == "dalitzDecay") {
        int nDaughters = 0;
        EvtId daughter[3];

        decayParent = parser.readAttribute("particle");
        std::string daugStr = parser.readAttribute("daughters");
        double probMax = parser.readAttributeDouble("probMax");

        checkParticle(decayParent);
        ipar=EvtPDL::getId(decayParent);

        std::istringstream daugStream(daugStr);

        std::string daugh;
        while(std::getline(daugStream, daugh, ' ')) {
          checkParticle(daugh);
          daughter[nDaughters++] = EvtPDL::getId(daugh);
        }

        if(nDaughters!=3) {
          report(ERROR,"EvtGen") <<
                "Expected to find three daughters for dalitzDecay of "<<decayParent<<" near line "
                <<parser.getLineNumber()<<", "<<"found "<<nDaughters<<endl;
              report(ERROR,"EvtGen") << "Will terminate execution!"<<endl;
              ::abort();
        }

        dalitzDecay = new EvtDalitzDecayInfo(daughter[0],daughter[1],daughter[2]);
        dalitzDecay->setProbMax(probMax);

      } else if(parser.getTagTitle() == "copyDalitz") {
        int nDaughters = 0;
        EvtId daughter[3];
        int nCopyDaughters = 0;
        EvtId copyDaughter[3];

        decayParent = parser.readAttribute("particle");
        std::string daugStr = parser.readAttribute("daughters");

        std::string copyParent = parser.readAttribute("copy");
        std::string copyDaugStr = parser.readAttribute("copyDaughters");

        checkParticle(decayParent);
        ipar=EvtPDL::getId(decayParent);

        checkParticle(copyParent);
        EvtId copypar=EvtPDL::getId(copyParent);

        std::istringstream daugStream(daugStr);
        std::istringstream copyDaugStream(copyDaugStr);

        std::string daugh;
        while(std::getline(daugStream, daugh, ' ')) {
          checkParticle(daugh);
          daughter[nDaughters++] = EvtPDL::getId(daugh);
        }
        while(std::getline(copyDaugStream, daugh, ' ')) {
          checkParticle(daugh);
          copyDaughter[nCopyDaughters++] = EvtPDL::getId(daugh);
        }

        if(nDaughters!=3 || nCopyDaughters!=3) {
          report(ERROR,"EvtGen") <<
                "Expected to find three daughters for copyDecay of "<<decayParent<<
                " from "<<copyParent<<" near line "<<parser.getLineNumber()<<
                ", "<<"found "<<nDaughters<<" and "<<nCopyDaughters<<endl;
          report(ERROR,"EvtGen") << "Will terminate execution!"<<endl;
              ::abort();
        }

        copyDecay(ipar, daughter, copypar, copyDaughter);

      } else if(parser.getTagTitle() == "/data") { //end of data
        endReached = true;
        parser.close();
        break;
      }
    //TAGS FOUND UNDER DALITZDECAY
    } else if(parser.getParentTagTitle() == "dalitzDecay") {
      if(parser.getTagTitle() == "resonance") {

        EvtComplex ampFactor(parser.readAttributeDouble("ampFactorReal",1.),
                             parser.readAttributeDouble("ampFactorImag",0.));
        double mag = parser.readAttributeDouble("mag",-999.);
        double phase = parser.readAttributeDouble("phase",-999.);
        double real = parser.readAttributeDouble("real",-999.);
        double imag = parser.readAttributeDouble("imag",-999.);

        if((real!=-999. || imag!=-999.) && mag==-999. && phase==-999.) {
          if(real==-999.) { real = 0; }
          if(imag==-999.) { imag = 0; }
          mag = sqrt(real*real + imag*imag);
          phase = atan2(imag,real) * EvtConst::radToDegrees;
        }
        if( mag==-999. ) {
          mag = 1.;
        }
        if( phase==-999. ) {
          phase = 0.;
        }

        double mass(0.), width(0.);
        int spin(0);

        std::string particle = parser.readAttribute("particle");
        if(particle != "") {
          EvtId resId = EvtPDL::getId(particle);
          if(resId == EvtId(-1,-1)) {
            report(ERROR,"EvtGen") <<"Unknown particle name:"<<particle.c_str()<<endl;
            report(ERROR,"EvtGen") <<"Will terminate execution!"<<endl;
            ::abort();
          } else {
            mass = EvtPDL::getMeanMass(resId);
            width = EvtPDL::getWidth(resId);
            spin = EvtSpinType::getSpin2(EvtPDL::getSpinType(resId))/2;
          }
        }

        width = parser.readAttributeDouble("width",width);
        mass = parser.readAttributeDouble("mass",mass);
        spin = parser.readAttributeInt("spin",spin);
        int daughterPair = parser.readAttributeInt("daughterPair");
        int bwFactor = parser.readAttributeInt("bwFactor");
        bool invMassAngDenom = parser.readAttributeBool("invMassAngDenom");
        std::string shape = parser.readAttribute("shape");

        if(shape=="NonRes") {
          resonance = new EvtResonancePrototype(ampFactor, mag, phase);
        } else if(shape=="RelBW") {
          resonance = new EvtResonancePrototype(ampFactor, mag, phase, width, mass, spin, daughterPair, bwFactor);
        } else if(shape=="Flatte") {
          resonance = new EvtResonancePrototype(ampFactor, mag, phase, mass, daughterPair);
        } else { //BW
          resonance = new EvtResonancePrototype(ampFactor, mag, phase, width, mass, spin, daughterPair, invMassAngDenom);
        }

        if(parser.isTagInline()) {
          dalitzDecay->addResonance(*resonance);
          delete resonance;
          resonance = 0;
        }
      } else if(parser.getTagTitle() == "/dalitzDecay") {
        addDecay(ipar, *dalitzDecay);
        delete dalitzDecay;
        dalitzDecay = 0;
      } else if(verbose) {
        report(INFO,"EvtGen") << "Unexpected tag "<<parser.getTagTitle()
                              <<" found in XML decay file near line "
                              <<parser.getLineNumber()<<". Tag will be ignored."<<endl;
      }
    //TAGS FOUND UNDER RESONANCE
    } else if(parser.getParentTagTitle() == "resonance"){
      if(parser.getTagTitle() == "flatteParam") {
        EvtFlatteParam param(parser.readAttributeDouble("mass1"),
                             parser.readAttributeDouble("mass2"),
                             parser.readAttributeDouble("g"));
        resonance->addFlatteParam(param);
      } else if(parser.getTagTitle() == "/resonance") {
        dalitzDecay->addResonance(*resonance);
        delete resonance;
        resonance = 0;
      }
    }
  }

  if(!endReached) {
    report(ERROR,"EvtGen") << "Either the decay file ended prematurely or the file is badly formed.\n"
                          <<"Error occured near line"<<parser.getLineNumber()<<endl;
    ::abort();
  }
}

void EvtDalitzTable::checkParticle(std::string particle) {
  if (EvtPDL::getId(particle)==EvtId(-1,-1)) {
    report(ERROR,"EvtGen") <<"Unknown particle name:"<<particle.c_str()<<endl;
    report(ERROR,"EvtGen") <<"Will terminate execution!"<<endl;
    ::abort();
  }
}

void EvtDalitzTable::addDecay(EvtId parent, const EvtDalitzDecayInfo& dec) {
  if(_dalitztable.find(parent)!=_dalitztable.end()) {
    _dalitztable[parent].push_back(dec);
  } else {
    _dalitztable[parent].push_back(dec);
  }
}

void EvtDalitzTable::copyDecay(EvtId parent, EvtId* daughters,
                               EvtId copy, EvtId* copyd) {
  EvtDalitzDecayInfo decay(daughters[0],daughters[1],daughters[2]);
  std::vector<EvtDalitzDecayInfo> copyTable = getDalitzTable(copy);
  std::vector<EvtDalitzDecayInfo>::iterator i = copyTable.begin();
  for( ; i != copyTable.end(); i++) {
    EvtId daughter1 = (*i).daughter1();
    EvtId daughter2 = (*i).daughter2();
    EvtId daughter3 = (*i).daughter3();

    if((copyd[0] == daughter1 && copyd[1] == daughter2 && copyd[2] == daughter3) ||
       (copyd[0] == daughter1 && copyd[1] == daughter3 && copyd[2] == daughter2) ||
       (copyd[0] == daughter2 && copyd[1] == daughter1 && copyd[2] == daughter3) ||
       (copyd[0] == daughter2 && copyd[1] == daughter3 && copyd[2] == daughter1) ||
       (copyd[0] == daughter3 && copyd[1] == daughter1 && copyd[2] == daughter2) ||
       (copyd[0] == daughter3 && copyd[1] == daughter2 && copyd[2] == daughter1)) {
      decay.setProbMax((*i).getProbMax());
      std::vector<EvtResonancePrototype>::const_iterator j = (*i).getResonances().begin();
      for( ; j != (*i).getResonances().end(); j++) {
        decay.addResonance((*j));
      }
      addDecay(parent,decay);
      return;
    }
  }
  //if we get here then there was no match
  report(ERROR,"EvtGen") << "Did not find dalitz decays for particle:"
         <<copy<<"\n";
}

std::vector<EvtDalitzDecayInfo> EvtDalitzTable::getDalitzTable(const EvtId& parent) {
  std::vector<EvtDalitzDecayInfo> table;
  if ( _dalitztable.find(parent)!=_dalitztable.end() ) {
    table=_dalitztable[parent];
  }

  if (table.empty()){
    report(ERROR,"EvtGen") << "Did not find dalitz decays for particle:"
         <<parent<<"\n";
  }

  return table;
}
