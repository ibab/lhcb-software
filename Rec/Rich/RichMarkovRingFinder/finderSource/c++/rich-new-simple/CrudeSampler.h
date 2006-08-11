
#ifndef LESTER_CRUDESAMPLER_H
#define LESTER_CRUDESAMPLER_H

// fwd dec
#include "CrudeSampler.fwd"

// includes
#include <iostream>
#include "GenericRingFinder/GenericRingFinder.h"
#include "GenericRingFinder/GenericResults.fwd"
#include "EventDescription.fwd"
#include "ThreePointCircleProposerB.fwd"
#include "NimTypeRichModel.fwd"
#include "boost/shared_ptr.hpp"
#include "Data.fwd"

// declaration
class CrudeSampler : public GenRingF::GenericRingFinder {
private:
  boost::shared_ptr<Lester::NimTypeRichModel> m_ntrm;
public:
  CrudeSampler(boost::shared_ptr<Lester::NimTypeRichModel> ntrm);
  virtual boost::shared_ptr<GenRingF::GenericResults> fit(const GenRingF::GenericInput & input) throw (CouldNotFit);
  std::ostream & printMeTo(std::ostream & os) const;
private:
  void doTheWork( Lester::EventDescription & currentPoint,
                  double & currentLogProb,
                  Lester::ThreePointCircleProposerB & p,
                  const Lester::NimTypeRichModel & ntrm,
                  const Lester::Data & data);
};

std::ostream & operator<<(std::ostream & os, const CrudeSampler & obj);

#endif

