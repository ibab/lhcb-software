
#ifndef LESTER_RICH2INFERRER_H
#define LESTER_RICH2INFERRER_H

// fwd dec
#include "Rich2Inferrer.fwd"

// includes
#include "GenericRingFinder/GenericInferrer.h"
#include "GenericRingFinder/GenericInput.fwd"
#include "GenericRingFinder/GenericResults.fwd"
#include "boost/shared_ptr.hpp"
#include "NimTypeRichModel.fwd"

namespace Lester
{

  // declaration
  class Rich2Inferrer : public GenRingF::GenericInferrer
  {

  public: // ----------- Things needed by GenRingF::GenericInferrer ---------
    /// Return probablility hit was made by the given circle
    virtual double probabilityHitWasMadeByGivenCircle(const GenRingF::GenericInput::GenericHits::const_iterator & hIt,
                                                      const GenRingF::GenericResults::GenericRings::const_iterator & cIt) const;
    /// Probability hit was made by no known circles
    virtual double probabilityHitWasMadeBySomethingOtherThanACircle(const GenRingF::GenericInput::GenericHits::const_iterator & hIt) const;
    virtual ~Rich2Inferrer();

  public: // ----------- Things we need ourselves to function ---------
    Rich2Inferrer(const GenRingF::GenericInput & genInput,
                  const boost::shared_ptr<const Lester::NimTypeRichModel> ntrm,
                  const GenRingF::GenericResults::GenericRings & genRings);
  private:
    const GenRingF::GenericInput & m_genInput;
    const boost::shared_ptr<const Lester::NimTypeRichModel> m_ntrm;
    const GenRingF::GenericResults::GenericRings & m_circs;
    const double m_meanBackground;

    typedef GenRingF::GenericInput::GenericHits::const_iterator HitIterator;
    typedef GenRingF::GenericResults::GenericRings::const_iterator CircleIterator;

    //typedef std::set<HitIterator> QueriedHits;
    typedef std::pair<HitIterator, CircleIterator> QueryPair;
    typedef std::map<QueryPair, double> QueryMap;

    //mutable QueriedHits queriedHits;
    mutable QueryMap queryMap;

    void cacheAnswersForAllCirclesWRTHit(const HitIterator & hIt) const;

  };

}

#endif

