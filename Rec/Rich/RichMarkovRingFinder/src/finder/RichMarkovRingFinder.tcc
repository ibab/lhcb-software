#ifndef LESTER_RICH_MARKOV_RING_FINDER_TCC
#define LESTER_RICH_MARKOV_RING_FINDER_TCC

#include "RichMarkovRingFinder.h"
#include "Data.h"
#include "CircleParams.h"
#include "RichParams.h"

namespace Lester {

  template<class Mode, const Mode & mode>
  RichMarkovRingFinder<Mode, mode>::~RichMarkovRingFinder() { };

  template<class Mode, const Mode & mode>
  RichMarkovRingFinder<Mode, mode>::RichMarkovRingFinder(const RunInitialisationObject & rio)
    : m_haveFitted(false),
      m_internalRunInitObject(rio),
      //m_numberOfSuggestedPoints(0),
      m_bestRPSoFar() {
  };

  template<class Mode, const Mode & mode>
  void RichMarkovRingFinder<Mode, mode>::doFit() {
    if (!m_haveFitted) {
      internalDoFit();
      m_haveFitted=true;
    };
  };

  template<class Mode, const Mode & mode>
  void RichMarkovRingFinder<Mode, mode>::continueInternalDoFitPostSuccessfulTestInitialisation(MySampler & m) {

    // If we got this far, then the point is OK, so we had better set up the cache:
    m.data.initialiseCache();

    //std::cout << *m << std::endl;

    //typename MySampler::PointType last = *m;
    //bool first=true;
    double bestPSoFar=m.logProbabilityOfLastAcceptedPoint();

    m_bestRPSoFar = *m; //last;

    //unsigned int itsPerFrame = 0;

    const StoppingCondition * const stopCon = m_internalRunInitObject.stoppingCondition();
    const bool runForever =
      ( 0 != dynamic_cast<const RunForever * const>(stopCon) );
    unsigned int numberOfItsIfNotInfinite(0);
    if (!runForever) {
      // we must be a run for finite number of steps, so
      const RichMarkovRingFinder<Mode,mode>::RunForFiniteNumberOfSteps * const runForFiniteNumberOfStepsStoppingCondition =
        dynamic_cast<const RichMarkovRingFinder<Mode,mode>::RunForFiniteNumberOfSteps * const>(stopCon);
      assert(runForFiniteNumberOfStepsStoppingCondition); // If this fails, then we forgot to update this routine when we invented more types of stopping condition!!
      numberOfItsIfNotInfinite = runForFiniteNumberOfStepsStoppingCondition->iterations();
    };
    

    //unsigned int maxFrames = 0;

    //if (runForever) {
    //  itsPerFrame = Constants::numberOfIterationsPerFrame;
    //};

    //if (!runForever) {

      //const RichMarkovRingFinder<Mode,mode>::RunForFiniteNumberOfSteps * const runForFiniteNumberOfSteps =
      //dynamic_cast<const RichMarkovRingFinder<Mode,mode>::RunForFiniteNumberOfSteps * const>(m_internalRunInitObject.stoppingCondition());
      //if (runForFiniteNumberOfSteps) {
        //const unsigned int its = runForFiniteNumberOfSteps->iterations();
        //itsPerFrame = runForFiniteNumberOfSteps->iterationsPerFrame();
        //const double approxFrames = static_cast<double>(its)/static_cast<double>(itsPerFrame);
        //const unsigned int minFrames = 1;
        //if (finite(approxFrames)) {
        //  maxFrames = static_cast<unsigned int>(approxFrames);
        //  if (maxFrames<minFrames) {
        //    maxFrames = minFrames;
        //  };
        ///} else {
        //  maxFrames = minFrames;
        //};
    //};
  //}//;

    // Now do some frames!
    for (unsigned int its=0; runForever || its<numberOfItsIfNotInfinite; ++its ) {

      //if ((!(*m == last)) || first) {
      //  first=false;
      //};

      //last=*m;

      //for (unsigned int i=0; i<itsPerFrame; ++i) {
      {
        const double p = m.logProbabilityOfLastAcceptedPoint();
        if ( p > bestPSoFar ) {
          bestPSoFar = p;
          m_bestRPSoFar = (*m);
        };
      };
      
      /// Increment the sampler. This is the most important 3 characters in the whole thing.
      ++m;        
        
    };

  };


  template<class Mode, const Mode & mode>
  void RichMarkovRingFinder<Mode, mode>::internalDoFit() {
    typename MySampler::PointType initialPoint;
    unsigned int numberOfSuggestedPoints = 0;
    while(numberOfSuggestedPoints<1000) {
      initialPoint = suggestInitialPoint(numberOfSuggestedPoints);
      MySampler m(initialPoint, m_internalEventInitObjectPtr->data());
      try {
        m.test();
//        std::cout << "Initial point " << numberOfSuggestedPoints << " was fine ... no need to try any more!" << std::endl;
        continueInternalDoFitPostSuccessfulTestInitialisation(m);
        return;
      } catch (typename MetropolisSamplerT::FirstPointBad&) {
//        std::cout << "Initial point " << numberOfSuggestedPoints << " failed ... will try another" << std::endl;
      };
    };
    std::cout << "Exceeded max number of iterations for good initial point" << std::endl;
    throw CouldNotFindGoodInitialPoint();
  };

  template<class Mode, const Mode & mode>
  typename RichMarkovRingFinder<Mode, mode>::PointType
  RichMarkovRingFinder<Mode, mode>::suggestInitialPoint(unsigned int &
                                                        numberOfSuggestedPoints) {
    // Tries to set initialPoint to a good "initial point" (one with finite probability) using info in the initialisation object, and returns true if it succeeded, false otherwise.
    PointType p;
    if (numberOfSuggestedPoints==0) {
      // first time!
      if (m_dataPtr->secretCircles().empty()) {
        p.jokeSetRandom();
      } else {
        // use jonesc suggestions
        const typename DataT::Circs suggs = m_dataPtr->secretCircles();
        for (typename DataT::Circs::const_iterator it = suggs.begin();
             it != suggs.end();
             ++it) {
          const CircleParamsT & circle = *it;
          p.addCircle(circle);
        };
      };
    } else {
      // not first time!
      p.jokeSetRandom();
    };
    ++numberOfSuggestedPoints;
    return p;
  };

  template<class Mode, const Mode & mode>
  const typename RichMarkovRingFinder<Mode, mode>::RichParamsT RichMarkovRingFinder<Mode, mode>::bestFit() const {
    return m_bestRPSoFar;
  };

  template<class Mode, const Mode & mode>
  RichMarkovRingFinder<Mode, mode>::RunInitialisationObject::RunInitialisationObject() :
    m_stoppingCondition(new DefaultStoppingCondition()) {
  };

  template<class Mode, const Mode & mode>
  RichMarkovRingFinder<Mode, mode>::EventInitialisationObject::EventInitialisationObject() :
    m_dataPtr(new DataT()) {
  };

  template<class Mode, const Mode & mode>
  RichMarkovRingFinder<Mode, mode>::RunInitialisationObject::~RunInitialisationObject() {
    delete m_stoppingCondition;
  };

  template<class Mode, const Mode & mode>
  RichMarkovRingFinder<Mode, mode>::EventInitialisationObject::~EventInitialisationObject() {
    delete m_dataPtr;
  };

  template<class Mode, const Mode & mode>
  inline void RichMarkovRingFinder<Mode, mode>::EventInitialisationObject::addHit(const Hit & h) {
    m_dataPtr->addHit(h);
  };

  template<class Mode, const Mode & mode>
  inline void RichMarkovRingFinder<Mode, mode>::EventInitialisationObject::addHitsFrom(std::istream & f) {
    double x,y;
    while (f >> x) {
      f >> y;

      x *= Constants::realXYDataInputScaleFactor;
      y *= Constants::realXYDataInputScaleFactor;
      Hit h(x,y);
      addHit(h);
    }
  }

  template<class Mode, const Mode & mode>
  inline void RichMarkovRingFinder<Mode, mode>::EventInitialisationObject::addCircleSuggestion(const CircleParamsT & c) {
    m_dataPtr->secretCircles().push_back(c);
  }

  template<class Mode, const Mode & mode>
  inline double RichMarkovRingFinder<Mode, mode>::EventInitialisationObject::realXYScaleFactor() const {
    return mode.realXYScaleFactor();
  }

};

#endif
