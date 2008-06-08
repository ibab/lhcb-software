
#include "NimTypeRichModel.h"

#include <string>
#include "EventDescription.h"
#include "CircleParams.h"
#include "Hit.h"
#include "Data.h"
#include "Utils/LogarithmicTools.h"
#include "Utils/MathsConstants.h"
#include "Utils/CheckForNan.h"
#include "Utils/MessageHandler.h"
#include "Constants.h"
#include "CircleTheorems.h"
#include "Small2Vector.h"
#include "ProbabilityUtils.h"
#include "RichModels.h"
#include "CLHEP/Random/RandGauss.h"
#include "CLHEP/Random/RandExponential.h"
#include "CLHEP/Random/RandPoisson.h"
#include "CLHEP/Random/RandFlat.h"

namespace Lester {

#define USE_2006_VALS 1
  /// #define USE_NIM_VALS 1

  // START OF CONSTANTS ----------------------------------------------------------------
#ifdef USE_2006_VALS
  const double NimTypeRichModel::backgroundRadius=0.030*3.;
  const double NimTypeRichModel::backgroundMeanParameter=75;// was 10;
  const int    NimTypeRichModel::meanNumberOfRings=5; // was 10;
  const double NimTypeRichModel::circleHitsPerUnitLengthParameter=30./(MathsConstants::twoPi*0.030); //was 30./(MathsConstants::twoPi*0.030)
  const double NimTypeRichModel::circleMeanRadiusParameter=0.030; // was 0.030;
  const double NimTypeRichModel::circleRadiusSigmaAboutMean = circleMeanRadiusParameter*0.1; //approx guess!
  const double NimTypeRichModel::circleCenXMean=0.;
  const double NimTypeRichModel::circleCenYMean=0.;
  const double NimTypeRichModel::circleCenXSig=0.030*1.5; // was *3
  const double NimTypeRichModel::circleCenYSig=0.030*1.5; // was *3
  // f(a,e) = page 81 Eq 3.6 in Andrew Buckley thesis http://doc.cern.ch/archive/electronic/cern/preprints/thesis/thesis-2005-058.pdf
  const double NimTypeRichModel::circleProbabilityDistributionAlphaParameter=2; // or try something else!
  const double NimTypeRichModel::circleProbabilityDistributionEpsilonParameter=0.05; // was 0.05; // or try something else!
  const double NimTypeRichModel::areaScaleForSignal = MathsConstants::pi*circleCenXSig*circleCenYSig;
  const double NimTypeRichModel::areaScaleForEverything = areaScaleForSignal; // since the signal and background distributions more-or-less over the same area
#endif

#ifdef USE_NIM_VALS
  const double NimTypeRichModel::backgroundRadius=0.030*3.;
  const double NimTypeRichModel::backgroundMeanParameter=10;// was 10;
  const int    NimTypeRichModel::meanNumberOfRings=10; // was 10;
  const double NimTypeRichModel::circleHitsPerUnitLengthParameter=30./(MathsConstants::twoPi*0.030); //was 30./(MathsConstants::twoPi*0.030)
  const double NimTypeRichModel::circleMeanRadiusParameter=0.030; // was 0.030;
  const double NimTypeRichModel::circleRadiusSigmaAboutMean = circleMeanRadiusParameter*0.1; //approx guess!
  const double NimTypeRichModel::circleCenXMean=0.;
  const double NimTypeRichModel::circleCenYMean=0.;
  const double NimTypeRichModel::circleCenXSig=0.030*3; // was *3
  const double NimTypeRichModel::circleCenYSig=0.030*3; // was *3
  const double NimTypeRichModel::circleProbabilityDistributionAlphaParameter=2; // or try something else!
  const double NimTypeRichModel::circleProbabilityDistributionEpsilonParameter=0.05; // was 0.05; // or try something else!
  const double NimTypeRichModel::areaScaleForSignal = MathsConstants::pi*circleCenXSig*circleCenYSig;
  const double NimTypeRichModel::areaScaleForEverything = areaScaleForSignal; // since the signal and background distributions more-or-less over the same area
#endif
  // END OF CONSTANTS -------------------------------------------------------------------



  // was in Rich.h -------------------------------------------------------------------

  /// Guarantees to either return a "reasonable" logProb or else throw Lester::LogarithmicTools::LogOfZero()
  double NimTypeRichModel::totalLogProbOfEventDescriptionGivenData(const EventDescription & rp, /*Given*/ const Data & d) const {

    /* The general idea is to calculate the result by Bayes' Theorem:
     *
     *   totalLogProbOfEventDescriptionGivenData \propto
     *             totalLogProbOfDataGivenEventDescription + totalLogProbOfEventDescription
     *
     */

    // FIRST PART IS likelihood:
    // ---------- totalLogProbOfDataGivenEventDescription ----------------------------------------
    double logAns = totalLogProbOfDataGivenEventDescription(d,rp);

    // SECOND PART IS prior:
    // ---------- totalLogProbOfEventDescription ----------------------------------------
    {
      const double pep=priorProbabilityOf(rp);
      const double lpep = std::log(pep);
      if (pep==0 || !Lester::lfin(lpep)) {
        throw Lester::LogarithmicTools::LogOfZero();
      };
      logAns+=lpep;
      assert(Lester::lfin(logAns) && "step3");

    };

    return logAns;
  }


  double NimTypeRichModel::totalLogProbOfDataGivenEventDescription(const Data & d,
                                                                   /*Given*/ const EventDescription & rp) const {
    double logAns=0;

    // expected number of total hits:
    const double mu = meanNumberOfHitsMuExpectedGiven(rp);

    // actual number of hits:
    const int n = d.hits.size();

    // should actually have
    //      ans *= Lester::poissonProb(n,mu);
    // followed by the
    //      ans *= factorial(n)
    // which occurs in the next block to account for a combinatorial factor for the hit probabilities that are about to arrive, so to save time I combine the two factors together into
    const bool muIsTooSmall=!(Lester::lfin(log(mu)));
    if (mu==0 && n>0) {
      throw Lester::LogarithmicTools::LogOfZero();// infintely unlikely!
    } else if (mu==0 && n==0) {
      logAns+=0; //ans *= 1;
    } else if (muIsTooSmall) {
      // what about numerical issues related to taking log of mu when mu is small.
      if (n==0) {
        logAns -= mu; //ans *= exp(-mu);
      } else {
        throw Lester::LogarithmicTools::LogOfZero();// infintely unlikely!
      };
    } else {
      logAns+= (n*log(mu)-mu); //ans *= exp(n*log(mu)-mu);
    }

    // check how we are doing!
    assert(Lester::lfin(logAns) && "step1" );

    //      ans *= factorial(n) // not needed as incorporated above
    for (Data::Hits::const_iterator hit = d.hits.begin();
         hit!=d.hits.end();
         ++hit) {
      const double p = singleHitProbabilityDistributionGivenEventDescription(*hit, rp);
      const double lp = log(p);
      if (p==0 || !Lester::lfin(lp)) {
        throw Lester::LogarithmicTools::LogOfZero();
      };
      logAns+=lp;
    };

    // check how we are doing!
    assert(Lester::lfin(logAns) && "step2");
    return logAns;
  }


  // were in RichPriors.h: ------------------------------------------------------------

  int NimTypeRichModel::sampleFromNumberOfCirclesDistribution() const {
    return RandPoisson::shoot(meanNumberOfRings);
  };
  double NimTypeRichModel::priorProbabilityOfNumberOfCircles(const int n) const {
    return poissonProb(n, meanNumberOfRings);
  };


  int NimTypeRichModel::sampleFromNumberOfHitsDueToCircle(const CircleParams & cp) const {
    return RandPoisson::shoot(meanNumberOfHitsMuExpectedOn(cp));
  };
  // probability counterpart not yet implemented

  int NimTypeRichModel::sampleFromNumberOfHitsDueToBackground() const {
    return RandPoisson::shoot(meanNumberOfHitsMuExpectedFromBg());
  };
  // probability counterpart not yet implemented

  Hit NimTypeRichModel::sampleHitDueToBackground() const {
    const double x = RandGauss::shoot(0,backgroundRadius);
    const double y = RandGauss::shoot(0,backgroundRadius);
    return Hit(x,y);
  };
  double NimTypeRichModel::priorProbabilityOfHitDueToBackground(const Small2Vector & p) const {
    const double sigSq = backgroundRadius*backgroundRadius;
    const double rSqOnSigSq = p.mag2()/sigSq;
    const double half = 0.5;
    const double one = 1;
    const double ans = (one/(MathsConstants::twoPi*sigSq))*exp(-half*rSqOnSigSq);
    return ans;
  };



  Hit NimTypeRichModel::sampleHitDueToCircle(const CircleParams & c) const {
    const double theta=RandFlat::shoot(0.,MathsConstants::twoPi);
    const double r0=c.radius();
    const double epsilon=circleProbabilityDistributionEpsilonParameter;
    // the maths of the next bit depends on alpha being 2 (seelab book 5) so:
    assert (circleProbabilityDistributionAlphaParameter==2);
    const double t = RandGauss::shoot(log(r0),epsilon);
    const double rWobbled = exp(t);
    const double x=rWobbled*cos(theta)+c.centre().x();
    const double y=rWobbled*sin(theta)+c.centre().y();
    const Hit ans(x,y);
    return ans;
  };
  double NimTypeRichModel::priorProbabilityOfHitDueToCircle(const Small2Vector & p, const CircleParams & cp) const {
    const double alpha = circleProbabilityDistributionAlphaParameter;
    const double alphaSq = alpha*alpha;
    const double epsilon = circleProbabilityDistributionEpsilonParameter;
    const double epsSq = epsilon*epsilon;
    const double r     = (p-cp.centre()).mag();
    const double r0    = cp.radius();
    const double logg  = std::log(r/r0);
    const double logsq = logg*logg;
    const double two = 2;
    const double substituteForPow = ( alpha==2 ? 1 : std::pow(r/r0,alpha-two) );
    const double ans = 1./(MathsConstants::twoPi)/sqrt(MathsConstants::twoPi*epsSq)*substituteForPow*exp(-0.5*(logsq/epsSq + alphaSq*epsSq  )) / (r*r);
    return ans;
  };

  double NimTypeRichModel::singleHitProbabilityDistributionGivenEventDescription(const Hit & hit,
                                                                                 // given
                                                                                 const EventDescription & ed) const {
    double ans=0;
    double sigmaMu=0;
    for ( EventDescription::Circs::const_iterator it=ed.circs.begin();
          it!=ed.circs.end();
          ++it )
    {
      const double mu = meanNumberOfHitsMuExpectedOn(*it);
      sigmaMu+= mu;
      ans += mu*priorProbabilityOfHitDueToCircle(hit,*it);
    }
    sigmaMu += meanNumberOfHitsMuExpectedFromBgGiven(ed);
    ans += meanNumberOfHitsMuExpectedFromBgGiven(ed)*priorProbabilityOfHitDueToBackground(hit);
    return ( ans/sigmaMu );
  }

  // were in CircleParams.h --------------------------------------------------------------

  /// assume number of hits appearing on ring is poisson.  Determine the mean of that poisson dist, given a CircleParams.
  double NimTypeRichModel::meanNumberOfHitsMuExpectedOn(const CircleParams & cp) const {
    return MathsConstants::pi * circleHitsPerUnitLengthParameter * cp.radius();
  }
  /// don't confuse "meanNumberOfHitsMuExpectedFromBg()" with "meanNumberOfHitsMuExpectedFromBgGiven(const EventDescription & ed)" !
  double NimTypeRichModel::meanNumberOfHitsMuExpectedFromBg() const {
    return backgroundMeanParameter;
  }
  /// don't confuse "meanNumberOfHitsMuExpectedFromBg()" with "meanNumberOfHitsMuExpectedFromBgGiven(const EventDescription & ed)" !
  double NimTypeRichModel::meanNumberOfHitsMuExpectedFromBgGiven(const EventDescription &) const {
    //#warning "Should perhaps the bg mean be expected to be higher for events with lots of rings?  Or are these independent?  We assume independence here!"
    return meanNumberOfHitsMuExpectedFromBg();
  }
  double NimTypeRichModel::meanNumberOfHitsMuExpectedGiven(const EventDescription & ed) const {
    double ans=0;
    for (EventDescription::Circs::const_iterator it=ed.circs.begin();
         it!=ed.circs.end();
         ++it) {
      ans += meanNumberOfHitsMuExpectedOn(*it);
    };
    ans += meanNumberOfHitsMuExpectedFromBgGiven(ed);
    return ans;
  }

  CircleParams NimTypeRichModel::sampleCircle() const {
    return CircleParams(sampleFromCircleCentreDistribution(),
                        sampleFromCircleRadiusDistribution());
  }
  double NimTypeRichModel::priorProbabilityOf(const CircleParams & cp) const {
    const Small2Vector c=cp.centre();
    const double r=cp.radius();
    const double part1 =
      priorProbabilityOfCentre(c);
    const double part2 =
      priorProbabilityOfRadius(r);
    const double ans = part1*part2;
    return ans;
  }



  // Were in CirclePriors ----------------------------

  const char * NimTypeRichModel::getCacheLocation() const {
    static const char * env = getenv("RICHMFINDERDATALOCATION");
    static const std::string loc = ( env ? std::string(env)+"/approxCoPointSep.cache" : "approxCoPointSep.cache" );
    return loc.c_str();
  }

  double NimTypeRichModel::sampleFromCircleRadiusDistribution() const {
    if (Constants::scenario == Constants::simpleModel) {
      return RandExponential::shoot(circleMeanRadiusParameter);
    } else if (Constants::scenario == Constants::rich2A) {
      const double ans = rich2AThetaSampler.sampleAnIndex();
      return ans;
    } else {
      throw Constants::scenario;
    };
  }

  double NimTypeRichModel::priorProbabilityOfRadius(const double r) const {
    if (Constants::scenario == Constants::simpleModel) {
      return exponentialProb(r,circleMeanRadiusParameter);
    } else if (Constants::scenario == Constants::rich2A) {
      return rich2AThetaDistribution(r);
    } else {
      throw Constants::scenario;
    };
  }

  double NimTypeRichModel::sampleFromCircleRadiusDistributionAbove(const double r) const {
    if (Constants::scenario == Constants::simpleModel) {
      return r+RandExponential::shoot(circleMeanRadiusParameter);
    } else if (Constants::scenario == Constants::rich2A) {
      try {
        const double ans = rich2AThetaSampler.sampleAnIndexAbove(r);
        return ans;
      } catch ( Rich2AThetaSampler::NoSamplesPossible & ) {
        throw SampleIsImpossible();
      };
    } else {
      throw Constants::scenario;
    };
  }

  double NimTypeRichModel::priorProbabilityOfRadiusAbove(const double r) const {
    if (Constants::scenario == Constants::simpleModel) {
      return exponentialProbAbove(r,circleMeanRadiusParameter);
    } else if (Constants::scenario == Constants::rich2A) {
      // was return 1 in "not-so-bad" version of program!
      return rich2AProbabilityThetaAbove(r);
    } else {
      throw Constants::scenario;
    };
  }

  double NimTypeRichModel::sampleFromApproximateCoPointSeparationDistribution() const 
  {
    // CoPointSeparationDistribution is the distrib of
    // distances between two points on a random circle
    // Here we approximate it in the limit that the circles are whole and sharp
    const double r = sampleFromCircleRadiusDistribution();
    const double theta = RandFlat::shoot(MathsConstants::pi);
    const double two = 2;
    const double rho = r*sqrt(two-two*cos(theta));
    return rho;
  }

  double NimTypeRichModel::approxCoPointSepFunctionPart2(const double deltaOnTwo, 
                                                         const double rSq) const 
  {
    const double deltaOnTwoSq = deltaOnTwo*deltaOnTwo;
    const double inner = rSq - deltaOnTwoSq;
    if (inner<=0) {
      return 0;
    };
    const double ans = rSq/(deltaOnTwo*sqrt(inner));
    return ans;
  }

  double NimTypeRichModel::approxCoPointSepFunctionPart1(const double deltaOnTwo) const 
  {
    // montecarlo answer and cache ...
    typedef std::map<double,double> Map;
    static Map cache;
    
    static bool first = true;
    if (first) 
    {
      first = false;
      const std::string filename ( getCacheLocation() );
      Lester::messHandle().info() << "Opening cache file '" << filename << "'" << Lester::endmsg;
      std::ifstream f(filename.c_str());
      if (true/*FIX*/) 
      {
        double key,ans;
        while (f>>key) 
        {
          f >> ans;
          cache[key]=ans;
          Lester::messHandle().debug() << " -> Read approxCoPointSep[ " << key << " ] = "
                                       << ans << " from file" << Lester::endmsg;
        };
      };
    };

    const double one = 1;
    const double tol = 0.02; // interpolate at 2% intervals
    const double convergenceTol=0.0000001; // evaluate to appro 1% acc
    const double deltaOnTwoMax = deltaOnTwo*(one+tol);
    const double deltaOnTwoMin = deltaOnTwo/(one+tol);

    typedef std::vector<Map::const_iterator> Possibilities;
    Possibilities possibilities;
    {
      Map::const_iterator geRhoIt = cache.lower_bound(deltaOnTwo);
      if (geRhoIt!=cache.end()) {
        const double key=geRhoIt->first;
        if (key == deltaOnTwo) {
          // we found an exact match
          const double ans = geRhoIt->second;
          return ans;
        };
        if (geRhoIt->first <= deltaOnTwoMax ) {
          possibilities.push_back(geRhoIt);
        };
      };
      if (geRhoIt!=cache.begin()) {
        --geRhoIt;
        if (geRhoIt->first>=deltaOnTwoMin) {
          possibilities.push_back(geRhoIt);
        };
      };
    };

    {
      int tindex=-1;
      if (possibilities.size()==1) {
        tindex=0;
      } else if (possibilities.size()==2) {
        if (fabs(possibilities[0]->first-deltaOnTwo)<fabs(possibilities[1]->first-deltaOnTwo)) {
          tindex=0;
        } else {
          tindex=1;
        };
      };

      const int index=tindex;
      if (index==-1) {
        //calculate and cache!
        int n=0;
        double sum=0;
        double avg=0;
        double lastAvg=0;
        const double rmin=deltaOnTwo;
        {
          try {
            const int warmUpSteps=10; // must be at least 2
            assert (warmUpSteps>=2);
            for (; n<warmUpSteps || (n>=warmUpSteps && fabs((avg-lastAvg)/(avg+lastAvg))>convergenceTol  ); ++n) {
              const double r = sampleFromCircleRadiusDistributionAbove(rmin);
              const double rSq = r*r;
              const double contrib = approxCoPointSepFunctionPart2(deltaOnTwo, rSq);
              lastAvg=avg;
              sum+= contrib;
              avg = sum/static_cast<double>(n);
            };
            // the answer should now be in avg, all bar the accounting for the rmin parameter used above ...
            avg*=priorProbabilityOfRadiusAbove(rmin);
          } catch (NimTypeRichModel::SampleIsImpossible&) {
            Lester::messHandle().verbose() << " SampleIsImpossibleInCop at " <<deltaOnTwo<< Lester::endmsg;
            avg=0;
          };

          cache[deltaOnTwo]=avg;
          {
            try
            {
              // need to add read/write idea here
              std::ofstream cf(getCacheLocation(),std::ios::app);
              if ( Lester::lfin(deltaOnTwo) && Lester::lfin(avg)) {
                cf << std::setprecision(25) << deltaOnTwo << " " << avg << std::endl;
              };
            }
            catch ( const std::exception & expt )
            {
              Lester::messHandle().warning() << "Exception '" << expt.what()
                                             << "' caught writing to cache -> results not saved" << Lester::endmsg;
            }
            Lester::messHandle().debug() << "Wrote approxCoPointSep[ "<<deltaOnTwo<<" ] = " << avg << " to cache file" << Lester::endmsg;
          };
          return avg;
        };
      } else {
        // serve from cache
        assert(index>=0 && index<static_cast<int>(possibilities.size()));
        const double ans = possibilities[index]->second;
        return ans;
      };
    };
  }

  double NimTypeRichModel::priorProbabilityOfThreePointsBeingOnACircle
  (const Small2Vector & a,
   const Small2Vector & b,
   const Small2Vector & c) const {
    try {
      const double r = CircleTheorems::radiusOfCircleThrough(a,b,c);
      const double ans = priorProbabilityOfThreePointsBeingOnACircleWithKnownCircumradius(a,b,c,r);
      return ans;
    } catch (CircleTheorems::RadiusIsInfinite&) {
      return 0;
    };
  }

  double NimTypeRichModel::priorProbabilityOfThreePointsBeingOnACircleWithKnownCircumradius
  (const Small2Vector & a,
   const Small2Vector & b,
   const Small2Vector & c,
   const double r) const {
    const double one = 1;
    const double averageHitsFromACircle = MathsConstants::pi * circleMeanRadiusParameter * circleHitsPerUnitLengthParameter;
    const double averageHitsFromCircles = averageHitsFromACircle * meanNumberOfRings;
    const double averageHitsFromBg = backgroundMeanParameter;
    const double averageHitsTotal = averageHitsFromCircles+averageHitsFromBg;
    const double signalPurity = averageHitsFromCircles/(averageHitsTotal);
    const double signalPurityCubed = signalPurity*signalPurity*signalPurity;
    const double nC = meanNumberOfRings;
    const double nCSq = nC*nC;
    const double PS = signalPurityCubed/nCSq;
    const double mu // mean number of hits per unit length on circles
      = circleHitsPerUnitLengthParameter;
    const double muCubed = mu*mu*mu;

    const double aAll = areaScaleForEverything;
    const double aSig = areaScaleForSignal;
    const double aAllCubed = aAll*aAll*aAll;
    const double two=2;
    //const double three=3;
    const double rSq=r*r;

    const double rProb = priorProbabilityOfRadius(r);

    //const Small3Vector a3(a);
    //const Small3Vector b3(b);
    //const Small3Vector c3(c);
    const double modCross
      = fabs(CircleTheorems::twoVectorCross(a,b)
             + CircleTheorems::twoVectorCross(b,c)
             + CircleTheorems::twoVectorCross(c,a));

    const double extraTerm = averageHitsTotal*(averageHitsTotal-one)*(averageHitsTotal-two);

    const double ans = one/(one+((extraTerm*aSig*modCross*(one-PS))/(two*aAllCubed*muCubed*rProb*rSq*PS)));
    return ans;
  }

  double NimTypeRichModel::PROPTO_priorProbabilityOfTwoPointsBeingOnACircle
  (const Small2Vector & a,
   const Small2Vector & b) const {
    // See sheets one two and three stapled into lab book 5 at 19/11/2003
    // Assumptions:
    // *   Assumes circles are reasonably narrow (well focused).
    // Good points:
    // *   Takes into account that the number of hits on circle is
    //     proportional to circumference
    // *   Takes prior distribution of circle radii into account.
    // Bad points:
    // *   assumes circles are distributed evenly across space, so makes no use
    //     of any knowledge that could be gleaned from the distribution of centres.
    const double deltaSq = (a-b).mag2();
    const double delta = sqrt(deltaSq);
    const double deltaOnTwo = delta*0.5;

    const double mu // mean number of hits per unit length on circles
      = circleHitsPerUnitLengthParameter;
    const double muSq = mu*mu;

    const double averageAreaScale = areaScaleForEverything*areaScaleForEverything / areaScaleForSignal;

    static bool first = true;
    if (first) {
      first = false;
      Lester::messHandle().debug() << "Estimation of PS could be improved by looking at the present number of hits rather than using the average number, and by removing crude dependence on average radius etc.  This would be very important if the circle radius distribution became bimodal or hits per unit arc length came to depend upon r." << Lester::endmsg;
      // for example, could base estimate on evidence of current fit!
    };
    const double one = 1;
    //const double two = 2;
    const double averageHitsFromACircle = MathsConstants::pi * circleMeanRadiusParameter * circleHitsPerUnitLengthParameter;
    const double averageHitsFromCircles = averageHitsFromACircle * meanNumberOfRings;
    const double averageHitsFromBg = backgroundMeanParameter;
    const double averageHitsTotal=averageHitsFromCircles+averageHitsFromBg;
    const double signalPurity = averageHitsFromCircles/(averageHitsTotal);
    const double signalPuritySq = signalPurity*signalPurity;
    const double PS = signalPuritySq/meanNumberOfRings;

    const double extraTerm = averageHitsTotal*(averageHitsTotal-one);

    const double sepFun=approxCoPointSepFunctionPart1(deltaOnTwo);
    const double ans
      = one/(one+(one-PS)*extraTerm/(PS * averageAreaScale * muSq * sepFun));
    
    return ans;
  }


  Small2Vector NimTypeRichModel::sampleFromCircleCentreDistribution() const {
    return Small2Vector(RandGauss::shoot(circleCenXMean,circleCenXSig),
                        RandGauss::shoot(circleCenYMean,circleCenYSig));
  }
  double NimTypeRichModel::priorProbabilityOfCentre(const Small2Vector & cent) const {
    return
      gaussianProb(cent.x(), circleCenXMean, circleCenXSig)*
      gaussianProb(cent.y(), circleCenYMean, circleCenYSig);
  }

  // were in EventDescription.h --------------------------------------------------------

  EventDescription NimTypeRichModel::sampleAnEventDescriptionFromPrior() const {
    EventDescription ans;
    assert(ans.circs.empty());
    const int n=sampleFromNumberOfCirclesDistribution();
    for (int i=0; i<n; i++) {
      ans.circs.push_back(sampleCircle());
    };
    return ans;
  }
  double NimTypeRichModel::priorProbabilityOf(const EventDescription & ed) const {
    //#warning "Need to treat meanBackground correctly!"
    double ans = 1;

    // in principle I should have
    //        ans *= Lester::poissonProb(ed.circs.size(),meanNumberOfRings);
    // followed by
    //        ans *= factorial(ed.circs.size())
    // to accound for a combinatorial factor in what is about to follow.
    // however this involves a needless double (cancelling) evaluation of n!,
    // where n=ed.circs.size(), so instead I write the following faster combination
    // of the two:

    // actually I have just realised that contrary to the comments above, since in my case the order DOES count, as I don't always sort my circles into some order, I should NOT multiply by the factorial factor mentioned above, so I should just stick  to using the poisson factor.  As a result, I comment out the three lines below and replace them by the one four below!
    //const double n = static_cast<double>(ed.circs.size());
    //const double mu = meanNumberOfRings;
    //ans *= exp(n*log(mu)-mu);  /* times n! divided by n! */
    // SUPERSEDED BY NEXT LINE      ans *= poissonProb(ed.circs.size(),meanNumberOfRings);
    ans *= priorProbabilityOfNumberOfCircles(ed.circs.size());
    for (EventDescription::Circs::const_iterator it = ed.circs.begin();
         it!=ed.circs.end();
         it++)
    {
      const CircleParams & c = *it;
      const double contribution = priorProbabilityOf(c);
      ans *= contribution;
    }

    // and now the part I left out for ages (by accident!) which ensures that we compare hypotheses with different numbers of circles on an equivalent footing.  This insertion is somewhat guessed, so would be a good idea to get a statistician to confirm what I am doing.

    // Basically, for every circle I multiply the prior by one over the expectation (under the circles prior) of the circles prior itself!

    const double two = 2;
    const double strangeFactor = (two*MathsConstants::sqrtPi);

    const double oneOnCharacteristicAreaForCentresWandering = strangeFactor*strangeFactor*(circleCenXSig*circleCenYSig);
    const double oneOnCharacteristicLengthForRadiiWandering = strangeFactor*(circleRadiusSigmaAboutMean);
    const double characteristicThingForOneCircle
      = oneOnCharacteristicAreaForCentresWandering
      * oneOnCharacteristicLengthForRadiiWandering;

    const double characteristicThingForAllCircles
      = std::pow( characteristicThingForOneCircle, (int)ed.circs.size() );

    ans *= characteristicThingForAllCircles;

    return ans;
  }

}
