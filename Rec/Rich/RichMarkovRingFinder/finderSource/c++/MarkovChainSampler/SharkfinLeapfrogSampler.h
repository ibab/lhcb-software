#ifndef LESTER_SHARKFINLEAPFROG_SAMPLER_H
#define LESTER_SHARKFINLEAPFROG_SAMPLER_H


#include "CLHEP/Random/RandFlat.h"
#include "MarkovChainSampler/MarkovChainSampler.h"
#include "Utils/LogarithmicTools.h"
#include "Utils/MessageHandler.h"
#include "boost/shared_ptr.hpp"
#include "Utils/SampleAndProbabilityModule.h"
#include <list>
#include <vector>

namespace Lester {

  template <class PointType>
  class SharkfinLeapfrogSampler :
    public MarkovChainSampler<PointType, std::list<boost::shared_ptr<const PointType> > > {
  private:
    struct PointPlus {
      PointPlus(boost::shared_ptr<const PointType> pt, const double logPrb) :
        point(pt),
        logProb(logPrb) {
      };
      PointPlus(boost::shared_ptr<const PointType> pt) :
        point(pt),
        logProb(nan("")) {
        assert(!(finite(logProb)));
      };
      boost::shared_ptr<const PointType> point;
      double logProb;  // use NaN to indicate impossible!
    };
    typedef std::vector<PointPlus> InternalPointsType;
  private:
    SharkfinLeapfrogSampler(); // Deliberately not implemented!
  public:
    //typedef Space SpaceType;
    //typedef typename Space::PointType PointType;
    typedef std::list<boost::shared_ptr<const PointType> > ExternalPointsType;
    typedef boost::shared_ptr<const SampleAndProbabilityModule<double> > Sharkfin;
    class FirstPointBad {};
    SharkfinLeapfrogSampler(const ExternalPointsType & initialPoints,
                            const Sharkfin & sharkfin /*To make this skilling, supply a delta function at +1.  For canonical sharkfin, supply a dist with half its probability below +1, and half above +1.  Possibly restrict to only positive x*/
                            ) :
      initialised(false),
      currentPoints(),
      m_numberOfPoints(initialPoints.size()),
      m_numberOfPointsMinusOne(initialPoints.size()-1), // see check for whether this is >=0 or not below!
      m_pointsReturned(0),
      m_freshPointsReturned(0),
      m_sharkfin(sharkfin) {
      if (initialPoints.size()<=1) {
        throw FirstPointBad();
      };
      for (typename ExternalPointsType::const_iterator it = initialPoints.begin();
           it != initialPoints.end();
           ++it) {
        currentPoints.push_back(PointPlus(*it)); // could speed this up as this may messily reallocate the vector a few too many times for optimum speed
      };
    };

    boost::shared_ptr<const PointType> last() const { // Note!  Returns 0 if there was no last point .. eg just after construction!
      return m_last;
    };
    virtual const ExternalPointsType operator* () const {
      ExternalPointsType ans;
      for (typename InternalPointsType::iterator it = currentPoints.begin();
           it != currentPoints.end();
           ++it) {
        ans.push_back(it->point);
      };
      return ans;
    };
    virtual ~SharkfinLeapfrogSampler() {
    };
    // Here is the all important target distribution the user hopes to sample from.
    // It is not necessarily normalised to *unit* area, but it is normalised to
    // *const* area!
  private:
    void moveToNextState() {
      //usleep(100);
      if (!initialised) initialise(); // Can't initialise in constructor as derived classes are not yet ready ...

      ++m_pointsReturned; // For statistics ...

      // frogger = "point doing the jumping"
      const unsigned int froggerIndex = RandFlat::shootInt(m_numberOfPoints);
      // froggee = "point being jumped over"
      const unsigned int froggeeIndex = (froggerIndex + 1 + RandFlat::shootInt(m_numberOfPointsMinusOne)) % m_numberOfPoints;

      assert(froggerIndex!=froggeeIndex);
      assert(froggerIndex>=0);
      assert(froggerIndex<m_numberOfPoints);
      assert(froggeeIndex>=0);
      assert(froggeeIndex<m_numberOfPoints);

      PointPlus & froggerPlus = currentPoints[froggerIndex];
      const PointPlus & froggeePlus = currentPoints[froggeeIndex];

      const PointType & frogger = *(froggerPlus.point);
      const double froggerLP = froggerPlus.logProb;
      assert(finite(froggerLP));

      const PointType & froggee = *(froggeePlus.point);

      // Where are we going?
      const double sharkfinForwardFactor = m_sharkfin->sample();
      const double sharkfinReverseFactor = 1./sharkfinForwardFactor;
      double logProbOfForwardProposal;
      double logProbOfReverseProposal;
      try {
        if (!finite(sharkfinReverseFactor)) {
          throw Lester::LogarithmicTools::LogOfZero();
        };
        logProbOfForwardProposal = m_sharkfin->logOfProbabilityOf(sharkfinForwardFactor);
        logProbOfReverseProposal = m_sharkfin->logOfProbabilityOf(sharkfinReverseFactor);
        assert(finite(logProbOfForwardProposal));
        assert(finite(logProbOfReverseProposal));
      } catch (Lester::LogarithmicTools::LogOfZero) {
        Lester::messHandle().error() << "Hey! Idiot!  Yes! You there!  Use a SENSIBLE Sharkfin proposal distribution!   Jumps of "
                                     << sharkfinForwardFactor << " cannot be reversed!" << Lester::endmsg;
        m_last = froggerPlus.point;
        return;
      };

      boost::shared_ptr<const PointType> proposedPoint
        =  boost::shared_ptr<const PointType>(new PointType(froggee +
                                                            (froggee-frogger)*sharkfinForwardFactor));

      try {
        const double logProposedProb = logTargetDistribution(*proposedPoint);
        assert(finite(logProposedProb));

        // The first two terms in the following are the likelihood ratio, the second two terms are the Q factors.
        const double diff = logProposedProb-froggerLP+(logProbOfReverseProposal-log(fabs(sharkfinForwardFactor)))-logProbOfForwardProposal;
        assert(finite(diff));

        const bool accept = (diff>=0 || RandFlat::shoot()<exp(diff));
        if (accept) {
          froggerPlus = PointPlus(proposedPoint, logProposedProb);
          ++m_freshPointsReturned;
          m_last = proposedPoint;
          return;
        }

      } catch (LogarithmicTools::LogOfZero &) {
        // a very unlikely point, so stick with current point!
      } catch(...) {
        Lester::messHandle().warning() << __FILE__ << " " << __LINE__ 
                                       << " has detected a throw during moveToNextState() which was NOT of type LogarithmicTools::LogOfZero, which is the type you should be throwing if the point is infinitely unlikely.  I am going to veto this point, but FIX THE BUG!!!" << Lester::endmsg;

      };
      m_last = froggerPlus.point;
      return;
    };
  public:
    inline double efficiency() const {
      return (m_pointsReturned>0)?static_cast<double>(m_freshPointsReturned)/static_cast<double>(m_pointsReturned):1;
    };
  public:
    void test() const {
      // the user has no *need* to call this, but can do so if he desires!
      if (!initialised) {
        initialise();
      };
    };
  private:
    // Would like to call next function in constructor, only we have to wait for derived classes to be ready first ...
    void initialise() const {
      for(typename InternalPointsType::iterator it = currentPoints.begin();
          it != currentPoints.end();
          ++it) {
        try {
          it->logProb = logTargetDistribution(*(it->point));
        } catch (LogarithmicTools::LogOfZero loz) {
          throw FirstPointBad();
        };
      };
      initialised=true;
    };
    mutable bool initialised;
    mutable InternalPointsType currentPoints;
    const unsigned int m_numberOfPoints;
    const unsigned int m_numberOfPointsMinusOne;
    int m_pointsReturned;
    int m_freshPointsReturned;
    boost::shared_ptr<const PointType> m_last;
    const Sharkfin m_sharkfin;
  public:
    double logProbabilityOfLastAcceptedPoint() const {
      double ans=0;
      for (typename InternalPointsType::iterator it = currentPoints.begin();
           it != currentPoints.end();
           ++it) {
        ans += it->logProb;
      };
      return ans;
    };
  };

};

#endif
