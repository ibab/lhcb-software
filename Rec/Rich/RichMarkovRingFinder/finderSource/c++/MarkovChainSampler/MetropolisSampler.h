#ifndef LESTER_METROPOLIS_SAMPLER_H
#define LESTER_METROPOLIS_SAMPLER_H


#include "CLHEP/Random/RandFlat.h"
#include "MarkovChainSampler/MarkovChainSampler.h"
#include "Utils/LogarithmicTools.h"

namespace Lester {

  template <class PointType>
  class MetropolisSampler :
    public MarkovChainSampler<PointType> {
  private:
    MetropolisSampler(); // Deliberately not implemented!
  public:
    //typedef Space SpaceType;
    //typedef typename Space::PointType PointType;
    class FirstPointBad {};
    MetropolisSampler(const PointType & initialPoint) :
      initialised(false),
      currentPoint(initialPoint),
       m_pointsReturned(0),
       m_freshPointsReturned(0) {
    };
    virtual const PointType operator* () const {
      return currentPoint;
    };
    virtual ~MetropolisSampler() {
    };
    // Here is the all important target distribution the user hopes to sample from.
    // It is not necessarily normalised to *unit* area, but it is normalised to 
    // *const* area!
  public:
    virtual double QQuotientLog(const PointType & lastPoint,
			     const PointType & suggestedPoint) const {
      // Default implementation:

      const double top = 
	logProbOfSuggestingFirstGivenLast(lastPoint, suggestedPoint);
      const double bot =
	logProbOfSuggestingFirstGivenLast(suggestedPoint, lastPoint);
      
      //if (bot==0) {
      //std::cout << "Oh .. bot=0, top=" << top << " last=" << lastPoint<<" sugg="<<suggestedPoint<<std::endl;
      //	//return -1;
      //};

      return top-bot;

    };
  private:
    virtual PointType suggestNewPoint() const = 0;
    virtual double logProbOfSuggestingFirstGivenLast(const PointType &,
						  const PointType &) const = 0;
    void moveToNextState() {
      if (!initialised) initialise(); // Can't initialise in constructor as derived classes are not yet ready ...

      ++m_pointsReturned; // For statistics ...
      
      const PointType & lastPoint = currentPoint;

      //while (true) {
      const PointType & suggestedPoint = suggestNewPoint();
      const double qQuotientLog =  QQuotientLog(lastPoint, suggestedPoint);
      try {
	const double logPSuggestedPoint = logTargetDistribution(suggestedPoint);
	//std::cout << "suggested point has " << logPSuggestedPoint << std::endl;
	// Old ??crazy?? line: const double totalQuotient = (qQuotient>=0)?pSuggestedPoint/pLastPoint*qQuotient:1;
	const double totalQuotientLog = logPSuggestedPoint-logPLastPoint+qQuotientLog;
	const double totalQuotient = (totalQuotientLog<=0 ? exp(totalQuotientLog) : -1);
	if (!finite(totalQuotientLog)) {
	  std::cerr << "Please investigate MetropolisSampler as a matter of urgency because the following totalQuotientLog is not finite: " << totalQuotientLog  << std::endl;
	};
	if (totalQuotientLog>=0 || RandFlat::shoot()< totalQuotient /* totalQuotientLog is guaranteed negative and finite at this part, provided the above warning is not produced! */ ) {
	  // Accept!
	  logPLastPoint = logPSuggestedPoint;
	  currentPoint = suggestedPoint;
	  ++m_freshPointsReturned; // For statistics ...
	  return;
	} else {
	  // Stick with current Point!
	  return;
	};
      } catch (LogarithmicTools::LogOfZero &) {
	// a very unlikely point, so stick with current point!
	return;
      } catch(...) {
	std::cerr << "WARNING !!!! " << __FILE__ << " " << __LINE__ << " has detected a throw during moveToNextState() which was NOT of type LogarithmicTools::LogOfZero, which is the type you should be throwing if the point is infinitely unlikely.  I am going to veto this point, but FIX THE BUG!!!" << std::endl;
	
      };
      //};
      // Should never get here!
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
      try {
	logPLastPoint=logTargetDistribution(currentPoint);
      } catch (LogarithmicTools::LogOfZero loz) {
	throw FirstPointBad();	
      };
      initialised=true;
    };
    mutable bool initialised;
    mutable double logPLastPoint;
    PointType currentPoint; 
    int m_pointsReturned;
    int m_freshPointsReturned;
  public:
    double logProbabilityOfLastAcceptedPoint() const {
      return logPLastPoint;
    };
  };
  
};

#endif
