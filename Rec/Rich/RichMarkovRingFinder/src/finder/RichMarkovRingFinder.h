
#ifndef RICHMARKOV_RICH_MARKOV_RING_FINDER_H
#define RICHMARKOV_RICH_MARKOV_RING_FINDER_H

#include <iostream>
#include <cassert>
#include <map>
#include "MyRichMetropolisSampler.h"

#include "Data.fwd"

namespace RichMarkov {
  class Hit;
};
#include "CircleParams.fwd"
#include "RichParams.fwd"
#include "Inferrer.fwd"
#include "FinderExternalException.h"

namespace RichMarkov {

  template <class Mode, const Mode & mode>
  class RichMarkovRingFinder {

  public:
    typedef RichSpace<Mode, mode> RichSpaceT;
    typedef MetropolisSampler<RichSpaceT> MetropolisSamplerT;
    typedef RichMarkovRingFinder<Mode, mode> RichMarkovRingFinderT;
    typedef Data<Mode, mode> DataT;
    typedef RichStuff<Mode, mode> RichStuffT;
    typedef CircleParams<Mode, mode> CircleParamsT;
    typedef RichParams<Mode, mode> RichParamsT;
    typedef Inferrer<Mode, mode> InferrerT;

    const Mode & getmode() const { return mode; }

  public:

    class FitFailed : public FinderExternalException {};

    class CouldNotFindGoodInitialPoint : public FitFailed {};

    class StoppingCondition {
    public:
      virtual ~StoppingCondition() {};
    };

    class RunForever : public StoppingCondition {};

    typedef RunForever DefaultStoppingCondition;
    class RunForFiniteNumberOfSteps : public StoppingCondition {

    private:

      const unsigned int m_iterations;
      const unsigned int m_iterationsPerFrame;

    public:

      RunForFiniteNumberOfSteps(const unsigned int iterations = Constants::DefaultNumberOfIterations)
        : m_iterations(iterations) {
      };

      inline const unsigned int iterations() const { return m_iterations; };

    };

    class RunInitialisationObject {
    private:

      Canvas * m_globalCanvas1;
      Canvas * m_globalCanvas2;
      Canvas * m_globalCanvas3;
      StoppingCondition * m_stoppingCondition;

    public:

      RunInitialisationObject();

      ~RunInitialisationObject();

      void setCanvasses(Canvas & c1, Canvas & c2, Canvas & c3);

      template <class StopCondition>
      void setStoppingCondition(const StopCondition stoppingCondition) {
        delete m_stoppingCondition;
        // we take a copy, so that we may own it ourselves
        // and not depend on the memory management of the user!
        m_stoppingCondition = new StopCondition(stoppingCondition);
      };

      //inline Canvas * globalCanvas1() const { return m_globalCanvas1; };

      //inline Canvas * globalCanvas2() const { return m_globalCanvas2; };

      //inline Canvas * globalCanvas3() const { return m_globalCanvas3; };

      inline const StoppingCondition * const stoppingCondition() const { return m_stoppingCondition; };

    };

    class EventInitialisationObject {
    private:

      DataT * const m_dataPtr;

    public:

      EventInitialisationObject();

      ~EventInitialisationObject();

      inline double realXYScaleFactor() const;

      void addHit(const Hit & h);

      void addHitsFrom(std::istream & f);

      void addCircleSuggestion(const CircleParamsT & circle);

      inline const DataT & data() const { return *m_dataPtr; };

    };

  public:

    typedef MyRichMetropolisSampler<Mode, mode> MySampler;

    typedef typename MySampler::PointType PointType;

    template <class UniqueIdentifier>
    static RunInitialisationObject & theRunInitialisationObjectFor(const UniqueIdentifier & uniqueIdentifier) {
      typedef std::map<UniqueIdentifier, RunInitialisationObject *> Map;
      static Map mapOfIdentifiers;
      typename Map::const_iterator it = mapOfIdentifiers.find(uniqueIdentifier);
      if (it!=mapOfIdentifiers.end()) {
        // We have made this RIO before!
        return *(it->second);
      } else {
        // We must make a new RIO!
        RunInitialisationObject * const rioPtr = new RunInitialisationObject();
        const typename Map::value_type newItem = typename Map::value_type(uniqueIdentifier, rioPtr);
        mapOfIdentifiers.insert(newItem);
        return *rioPtr;
      };
    };

  private:

    bool m_haveFitted;
    const RunInitialisationObject   & m_internalRunInitObject;
    const EventInitialisationObject * m_internalEventInitObjectPtr;
    //unsigned int m_numberOfSuggestedPoints;
    typename MySampler::PointType m_bestRPSoFar;
    Canvas * m_globalCanvas1;
    Canvas * m_globalCanvas2;
    Canvas * m_globalCanvas3;
    const DataT * m_dataPtr;

    void internalDoFit(); // PRIVATE: for internal use only!
    RichMarkovRingFinder(); // deliberately not implemented
    PointType suggestInitialPoint(unsigned int & numberOfSuggestedPoints) ;

    /// continue Internal Do Fit Post Successful Test Initialisation
    void continueInternalDoFitPostSuccessfulTestInitialisation(MySampler & m);

  public:

    explicit RichMarkovRingFinder(const RunInitialisationObject & rio);
    ~RichMarkovRingFinder();
    void initialiseEvent(const EventInitialisationObject & eio) {
      m_haveFitted = false;
      m_internalEventInitObjectPtr = &eio;
      //m_numberOfSuggestedPoints = 0;
      m_bestRPSoFar=typename MySampler::PointType(); // probably not necessary as nothing should pay any attention to m_bestRPSoFar unless m-HaveFitted is true!
      m_dataPtr = &(eio.data());
    };

    void doFit(); // only the first call to doFit will do anything!
    const RichParamsT bestFit() const;

  };

}

#include "RichMarkovRingFinder.tcc"

#endif
