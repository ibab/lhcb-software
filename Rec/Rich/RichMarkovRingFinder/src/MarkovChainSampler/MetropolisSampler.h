#ifndef RICHMARKOV_METROPOLIS_SAMPLER_H
#define RICHMARKOV_METROPOLIS_SAMPLER_H


#include "CLHEP/Random/RandFlat.h"
#include "MarkovChainSampler/MarkovChainSampler.h"

namespace RichMarkov {

  template <class Space>
  class MetropolisSampler :
    public MarkovChainSampler<Space> {

  private:
    MetropolisSampler(); // Deliberately not implemented!

  public:

    typedef Space SpaceType;
    typedef typename Space::PointType PointType;

    class FirstPointBad : public FinderExternalException {};

    MetropolisSampler(const PointType & initialPoint) :
      initialised(false),
      currentPoint(initialPoint),
      m_pointsReturned(0),
      m_freshPointsReturned(0)
    {
    };

    virtual const PointType & operator* () const {
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

      return top-bot;
    };

  private:

    virtual const PointType suggestNewPoint() const
#ifdef WIN32
    { return PointType(); };
#else
    =0;
#endif
    virtual double logProbOfSuggestingFirstGivenLast(const PointType &,
                                                     const PointType &) const
#ifdef WIN32
    { return 0; };
#else
    =0;
#endif

    void moveToNextState() {
      if (!initialised) initialise(); // Can't initialise in constructor as derived classes are not yet ready ...

      ++m_pointsReturned; // For statistics ...

      const PointType & lastPoint = currentPoint;

      //while (true) {
      const PointType & suggestedPoint = suggestNewPoint();
      const double qQuotientLog =  QQuotientLog(lastPoint, suggestedPoint);
      try {
        const double logPSuggestedPoint = logTargetDistribution(suggestedPoint);
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
          //m_rejectedSuggestion=false;
          notifyOfAcceptanceOfSuggestion();
          ++m_freshPointsReturned; // For statistics ...
          return;
        } else {
          // Stick with current Point!
          //m_rejectedSuggestion=true;
          notifyOfRejectionOfSuggestion();
          return;
        };
      } catch (typename MarkovChainSampler<Space>::LogOfZero &) {
        // a very unlikely point, so stick with current point!
        //m_rejectedSuggestion=true;
        notifyOfRejectionOfSuggestion();
        return;
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
      } catch (typename MarkovChainSampler<Space>::LogOfZero&) {
        notifyOfRejectionOfSuggestion();
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
    inline double logProbabilityOfLastAcceptedPoint() const {
      return logPLastPoint;
    };
    /*
      public:
      inline bool rejectedSuggestedNewPointAndRemainedAtPreviousPoint() const {
      return m_rejectedSuggestion;
      };
      private:
      bool m_rejectedSuggestion;
    */
  private:
    // When a new suggested point is accepted or rejected, the appropriate one of the two virtual methods below will be called to inform derived classes of the outcome of the acceptance/rejection.  Default (null) implementations are provided because the derived classes may somply not care!
    inline virtual void notifyOfRejectionOfSuggestion() const {};
    inline virtual void notifyOfAcceptanceOfSuggestion() const {};
  };

}

#endif
