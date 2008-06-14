
#ifndef LESTER_RICH2INFERRER_H
#define LESTER_RICH2INFERRER_H

// fwd dec
#include "RichInferrer.fwd"

// includes
#include "GenericRingFinder/GenericInferrer.h"
#include "GenericRingFinder/GenericInput.fwd"
#include "GenericRingFinder/GenericResults.fwd"

#include "boost/shared_ptr.hpp"

namespace Lester
{

  template < class DATAMODEL >
  class RichInferrer : public GenRingF::GenericInferrer
  {

  public: // ----------- Things needed by GenRingF::GenericInferrer ---------

    /// Return probablility hit was made by the given circle
    virtual double
    probabilityHitWasMadeByGivenCircle(const GenRingF::GenericInput::GenericHits::const_iterator & hIt,
                                       const GenRingF::GenericResults::GenericRings::const_iterator & cIt) const;

    /// Probability hit was made by no known circles
    virtual double
    probabilityHitWasMadeBySomethingOtherThanACircle(const GenRingF::GenericInput::GenericHits::const_iterator & hIt) const;

    /// Destructor
    virtual ~RichInferrer();

  public: // ----------- Things we need ourselves to function ---------

    // The inferrer will be wrong in what it says about the probabilities of hits coming from BG versus non-BG."
    RichInferrer(const GenRingF::GenericInput & genInput,
                 const boost::shared_ptr<const DATAMODEL> ntrm,
                 const GenRingF::GenericResults::GenericRings & genRings)
      : m_genInput(genInput),
        m_ntrm(ntrm),
        m_circs(genRings),
        m_meanBackground(0) { }

  private:

    const GenRingF::GenericInput & m_genInput;
    const boost::shared_ptr<const DATAMODEL> m_ntrm;
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

