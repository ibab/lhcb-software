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
#include "EvtGenBase/EvtDalitzPlot.hh"
#include "EvtGenBase/EvtCyclic3.hh"

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
  EvtId ipar;
  std::string decayParent = "";
  EvtId daughter[3];

  EvtDalitzPlot dp;
  EvtComplex cAmp;
  std::vector< std::pair<EvtCyclic3::Pair,EvtCyclic3::Pair> > angAndResPairs;
  std::string shape("");
  EvtSpinType::spintype spinType(EvtSpinType::SCALAR);
  double mass(0.), width(0.), FFp(0.), FFr(0.);
  std::vector<EvtFlatteParam> flatteParams;

  EvtParserXml parser;
  parser.open(dec_name);

  bool endReached = false;

  while(parser.readNextTag()) {
    //TAGS FOUND UNDER DATA
    if(parser.getParentTagTitle() == "data") {
      if(parser.getTagTitle() == "dalitzDecay") {
        int nDaughters = 0;

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

        double m_d1 = EvtPDL::getMass(daughter[0]), m_d2 = EvtPDL::getMass(daughter[1]), m_d3 = EvtPDL::getMass(daughter[2]), M = EvtPDL::getMass(ipar);

        dp = EvtDalitzPlot( m_d1, m_d2, m_d3, M );

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

        flatteParams.clear();

        //Amplitude
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

        cAmp = ampFactor*EvtComplex(cos(phase*1.0/EvtConst::radToDegrees),sin(phase*1.0/EvtConst::radToDegrees))*mag;

        //Resonance particle properties
        mass = 0.;
        width = 0.;
        spinType = EvtSpinType::SCALAR;

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
            spinType = EvtPDL::getSpinType(resId);
          }
        }

        width = parser.readAttributeDouble("width",width);
        mass = parser.readAttributeDouble("mass",mass);
        switch(parser.readAttributeInt("spin",-1)) {
        case -1://not set here
          break;
        case 0:
          spinType = EvtSpinType::SCALAR;
          break;
        case 1:
          spinType = EvtSpinType::VECTOR;
          break;
        case 2:
          spinType = EvtSpinType::TENSOR;
          break;
        default:
          report(ERROR,"EvtGen") << "Unsupported spin near line "<<parser.getLineNumber()<<" of XML file."<<endl;
          ::abort();
        }

        //Shape and form factors
        shape = parser.readAttribute("shape");
        FFp = parser.readAttributeDouble("BlattWeisskopfFactorParent",0.0);
        FFr = parser.readAttributeDouble("BlattWeisskopfFactorResonance",1.5);

        //Daughter pairs for resonance
        angAndResPairs.clear();

        std::string resDaugStr = parser.readAttribute("resDaughters");
        if(resDaugStr != "") {
          std::istringstream resDaugStream(resDaugStr);
          std::string resDaug;
          int nResDaug(0);
          EvtId resDaughter[2];
          while(std::getline(resDaugStream, resDaug, ' ')) {
            checkParticle(resDaug);
            resDaughter[nResDaug++] = EvtPDL::getId(resDaug);
          }
          if(nResDaug != 2) {
            report(ERROR,"EvtGen") << "Resonance must have exactly 2 daughters near line "<<parser.getLineNumber()<<" of XML file."<<endl;
            ::abort();
          }
          int nRes = getDaughterPairs(resDaughter, daughter, angAndResPairs);
          if(nRes==0) {
            report(ERROR,"EvtGen") << "Resonance daughters must match decay daughters near line "<<parser.getLineNumber()<<" of XML file."<<endl;
            ::abort();
          }
          if(parser.readAttributeBool("normalise",true)) cAmp /= sqrt(nRes);
        }
        if(angAndResPairs.empty()) {
          switch(parser.readAttributeInt("daughterPair")) {
          case 1:
            angAndResPairs.push_back(std::make_pair(EvtCyclic3::BC,EvtCyclic3::AB));
            break;
          case 2:
            angAndResPairs.push_back(std::make_pair(EvtCyclic3::CA,EvtCyclic3::BC));
            break;
          case 3:
            angAndResPairs.push_back(std::make_pair(EvtCyclic3::AB,EvtCyclic3::CA));
            break;
          default:
            if(shape == "NonRes") { //We don't expect a pair for non-resonant terms but add dummy values for convenience
              angAndResPairs.push_back(std::make_pair(EvtCyclic3::AB,EvtCyclic3::AB));
              break;
            }
            report(ERROR,"EvtGen") << "Daughter pair must be 1, 2 or 3 near line "<<parser.getLineNumber()<<" of XML file."<<endl;
            ::abort();
          }
        }

        if(parser.isTagInline()) {
          std::vector< std::pair<EvtCyclic3::Pair,EvtCyclic3::Pair> >::iterator it = angAndResPairs.begin();
          for( ; it != angAndResPairs.end(); it++) {
            std::pair<EvtCyclic3::Pair,EvtCyclic3::Pair> pairs = *it;
            EvtDalitzReso resonance = getResonance(shape, dp, pairs.first, pairs.second, spinType, mass, width, FFp, FFr);
            dalitzDecay->addResonance(cAmp,resonance);
          }
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
        flatteParams.push_back(param);
      } else if(parser.getTagTitle() == "/resonance") {
        std::vector< std::pair<EvtCyclic3::Pair,EvtCyclic3::Pair> >::iterator it = angAndResPairs.begin();
        for( ; it != angAndResPairs.end(); it++) {
          std::pair<EvtCyclic3::Pair,EvtCyclic3::Pair> pairs = *it;
          EvtDalitzReso resonance = getResonance(shape, dp, pairs.first, pairs.second, spinType, mass, width, FFp, FFr);

          std::vector<EvtFlatteParam>::iterator flatteIt = flatteParams.begin();
          for( ; flatteIt != flatteParams.end(); flatteIt++) {
            resonance.addFlatteParam((*flatteIt));
          }

          dalitzDecay->addResonance(cAmp,resonance);
        }
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
      std::vector<std::pair<EvtComplex, EvtDalitzReso> >::const_iterator j = (*i).getResonances().begin();
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


EvtDalitzReso EvtDalitzTable::getResonance(std::string shape, EvtDalitzPlot dp, EvtCyclic3::Pair angPair, EvtCyclic3::Pair resPair,
                                           EvtSpinType::spintype spinType, double mass, double width, double FFp, double FFr) {
  if( shape=="RBW" || shape=="RBW_CLEO") {
    return EvtDalitzReso( dp, angPair, resPair, spinType, mass, width, EvtDalitzReso::RBW_CLEO, FFp, FFr );
  } else if( shape=="RBW_CLEO_ZEMACH" ) {
    return EvtDalitzReso( dp, angPair, resPair, spinType, mass, width, EvtDalitzReso::RBW_CLEO_ZEMACH, FFp, FFr );
  }else if( shape=="Flatte" ) {
    return EvtDalitzReso( dp, resPair, mass );
  } else if( shape=="NonRes" ) {
    return EvtDalitzReso( );
  } else { //NBW
    return EvtDalitzReso( dp, angPair, resPair, spinType, mass, width, EvtDalitzReso::NBW, FFp, FFr );
  }
}

int EvtDalitzTable::getDaughterPairs(EvtId* resDaughter, EvtId* daughter, std::vector< std::pair<EvtCyclic3::Pair,EvtCyclic3::Pair> >& angAndResPairs) {
  int n(0);
  if(resDaughter[0]==daughter[0] && resDaughter[1]==daughter[1]) {
    angAndResPairs.push_back(std::make_pair(EvtCyclic3::BC,EvtCyclic3::AB)); n++;
  } else if(resDaughter[0]==daughter[1] && resDaughter[1]==daughter[0]) {
    angAndResPairs.push_back(std::make_pair(EvtCyclic3::CA,EvtCyclic3::AB)); n++;
  }
  
  if(resDaughter[0]==daughter[1] && resDaughter[1]==daughter[2]) {
    angAndResPairs.push_back(std::make_pair(EvtCyclic3::CA,EvtCyclic3::BC)); n++;
  } else if(resDaughter[0]==daughter[2] && resDaughter[1]==daughter[1]) {
    angAndResPairs.push_back(std::make_pair(EvtCyclic3::AB,EvtCyclic3::BC)); n++;
  }

  if(resDaughter[0]==daughter[2] && resDaughter[1]==daughter[0]) {
    angAndResPairs.push_back(std::make_pair(EvtCyclic3::AB,EvtCyclic3::CA)); n++;
  } else if(resDaughter[0]==daughter[0] && resDaughter[1]==daughter[2]) {
    angAndResPairs.push_back(std::make_pair(EvtCyclic3::BC,EvtCyclic3::CA)); n++;
  }

  return n;
}

