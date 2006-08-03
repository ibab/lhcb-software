
#ifndef LESTER_CIRCLE_PRIORS_H
#define LESTER_CIRCLE_PRIORS_H

#include "Constants.h"
#include "CircleTheorems.h"
#include "CLHEP/Vector/TwoVector.h"
#include "CLHEP/Random/RandPoisson.h"
#include "CLHEP/Random/RandGauss.h"
#include "CLHEP/Random/RandExponential.h"
#include "ProbabilityUtils.h"
#include "RichModels.h"

namespace Lester {

  class CirclePriors {
  public:
    static const char * getCacheLocation()
    {
      static const char * env = getenv("RICHMFINDERDATALOCATION");
      static const std::string loc = ( env ? std::string(env)+"/approxCoPointSep.cache" : "approxCoPointSep.cache" );
      return loc.c_str();
    }
  public:
    class SampleIsImpossible {
    };
    static inline double sampleFromCircleRadiusDistribution() {
      if (Constants::scenario == Constants::simpleModel) {
        return RandExponential::shoot(Constants::circleMeanRadiusParameter);
      } else if (Constants::scenario == Constants::rich2A) {
        //std::cerr<<"about to ... " << std::flush;
        const double ans = rich2AThetaSampler.sampleAnIndex();
        //std::cerr<<"... done" << std::endl;
        return ans;
      } else {
        throw Constants::scenario;
      };
    };
    static inline double priorProbabilityOfRadius(const double r) {
      if (Constants::scenario == Constants::simpleModel) {
        return exponentialProb(r,Constants::circleMeanRadiusParameter);
      } else if (Constants::scenario == Constants::rich2A) {
        return rich2AThetaDistribution(r);
      } else {
        throw Constants::scenario;
      };
    };
    static inline double sampleFromCircleRadiusDistributionAbove(const double r) {
      if (Constants::scenario == Constants::simpleModel) {
        return r+RandExponential::shoot(Constants::circleMeanRadiusParameter);
      } else if (Constants::scenario == Constants::rich2A) {
        try {
          const double ans = rich2AThetaSampler.sampleAnIndexAbove(r);
          return ans;
        } catch (Rich2AThetaSampler::NoSamplesPossible nsp) {
          throw SampleIsImpossible();
        };
      } else {
        throw Constants::scenario;
      };
    };
    static inline double priorProbabilityOfRadiusAbove(const double r) {
      if (Constants::scenario == Constants::simpleModel) {
        return exponentialProbAbove(r,Constants::circleMeanRadiusParameter);
      } else if (Constants::scenario == Constants::rich2A) {
        // was return 1 in "not-so-bad" version of program!
        return rich2AProbabilityThetaAbove(r);
      } else {
        throw Constants::scenario;
      };
    };

    /*
    // I don't trust this function any more
    static inline double priorProbabilityOfChordLengthGivenRadius(const double rho, const double r) {
    // Given that two points are on a circle of radius r, this function returns the prior probability(-desnity) that their chord separation is rho.
    const double one = 1;
    const double two = 2;
    const double rhoOverTwo = rho/two;
    const double rs4 = rhoOverTwo*rhoOverTwo;
    const double under = r*r-rs4;
    if (under<=0) {
    return 0;
    };
    const double contrib = one/(MathsConstants::pi*sqrt(under));
    return contrib;
    };
    */

    /*
    // I don't trust this function any more
    static inline double PROPTO_priorProbabilityOfRadiusGivenChordLength(const double r, const double rho) {
    // Given that two points separated by rho are considered to be on
    // a circle, this function returns a quantity PRORORTIONAL TO the prior probability(-density) that the circle that they are on has radius r.
    const double ans
    = priorProbabilityOfChordLengthGivenRadius(rho, r)
    * priorProbabilityOfRadius(r); // By Bayes' thm
    return ans;
    };
    */

    static inline double sampleFromApproximateCoPointSeparationDistribution() {
      // CoPointSeparationDistribution is the distrib of
      // distances between two points on a random circle
      // Here we approximate it in the limit that the circles are whole and sharp
      const double r = sampleFromCircleRadiusDistribution();
      const double theta = RandFlat::shoot(MathsConstants::pi);
      const double two = 2;
      const double rho = r*sqrt(two-two*cos(theta));
      return rho;
    };
  private:
    static inline double approxCoPointSepFunctionPart2(const double deltaOnTwo, const double rSq) {
      const double deltaOnTwoSq = deltaOnTwo*deltaOnTwo;
      const double inner = rSq - deltaOnTwoSq;
      if (inner<=0) {
        return 0;
      };
      const double ans = rSq/(deltaOnTwo*sqrt(inner));
      return ans;
    };
  private:
    static inline double approxCoPointSepFunctionPart1(const double deltaOnTwo) {
      // montecarlo answer and cache ...
      typedef std::map<double,double> Map;
      static Map cache;

      static bool first=true;

      if (first) {
        first = false;

        std::ifstream f(getCacheLocation());
        if (true/*FIX*/) {
          double key,ans;
          while (f>>key) {
            f>>ans;
            cache[key]=ans;
            //std::cout << "Read approxCoPointSep [ " << key << " ] = " << ans << " from file" << std::endl;
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
            //std::cout<<"Cache-found-exact-match cop[ "<<key<<" ] = " << ans << " for cop["<<deltaOnTwo<<"]"<<std::endl;
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
            } catch (CirclePriors::SampleIsImpossible sii) {
              //std::cout << " SampleIsImpossibleInCop at " <<deltaOnTwo<< std::endl;
              avg=0;
            };

            cache[deltaOnTwo]=avg;
            //std::cout<<"Calculated cop[ "<<deltaOnTwo<<" ] = " << avg<<std::endl;
            {
              try
              {
                // need to add read/write idea here
                std::ofstream cf(getCacheLocation(),std::ios::app);
                if (finite(deltaOnTwo)&&finite(avg)) {
                  cf << std::setprecision(25) << deltaOnTwo << " " << avg << std::endl;
                };
              }
              catch ( const std::exception & expt )
              {
                std::cout << "Exception caught writing to cache -> results not saved" << std::endl;
              }
            };
            return avg;
          };
        } else {
          // serve from cache
          assert(index>=0 && index<static_cast<int>(possibilities.size()));
          const double ans=possibilities[index]->second;
          //const double key=possibilities[index]->first; std::cout<<"Cache-found cop[ "<<key<<" ] = " << ans << " for cop["<<deltaOnTwo<<"]"<<std::endl;
          return ans;
        };
      };
    };

  public:
    static inline double priorProbabilityOfThreePointsBeingOnACircle
    (const Hep2Vector & a,
     const Hep2Vector & b,
     const Hep2Vector & c) {
      try {
        const double r = CircleTheorems::radiusOfCircleThrough(a,b,c);
        const double ans = priorProbabilityOfThreePointsBeingOnACircleWithKnownCircumradius(a,b,c,r);
        return ans;
      } catch (CircleTheorems::RadiusIsInfinite pai) {
        return 0;
      };
    };
    static inline double priorProbabilityOfThreePointsBeingOnACircleWithKnownCircumradius
    (const Hep2Vector & a,
     const Hep2Vector & b,
     const Hep2Vector & c,
     const double r) {
      const double one = 1;
      const double averageHitsFromACircle = MathsConstants::pi * Constants::circleMeanRadiusParameter * Constants::circleHitsPerUnitLengthParameter;
      const double averageHitsFromCircles = averageHitsFromACircle * Constants::meanNumberOfRings;
      const double averageHitsFromBg = Constants::backgroundMeanParameter;
      const double averageHitsTotal = averageHitsFromCircles+averageHitsFromBg;
      const double signalPurity = averageHitsFromCircles/(averageHitsTotal);
      const double signalPurityCubed = signalPurity*signalPurity*signalPurity;
      const double nC = Constants::meanNumberOfRings;
      const double nCSq = nC*nC;
      const double PS = signalPurityCubed/nCSq;
      const double mu // mean number of hits per unit length on circles
        = Constants::circleHitsPerUnitLengthParameter;
      const double muCubed = mu*mu*mu;

      const double aAll = Constants::areaScaleForEverything;
      const double aSig = Constants::areaScaleForSignal;
      const double aAllCubed = aAll*aAll*aAll;
      const double two=2;
      //const double three=3;
      const double rSq=r*r;

      const double rProb = priorProbabilityOfRadius(r);

      //const Hep3Vector a3(a);
      //const Hep3Vector b3(b);
      //const Hep3Vector c3(c);
      const double modCross
        = fabs(CircleTheorems::twoVectorCross(a,b)
               + CircleTheorems::twoVectorCross(b,c)
               + CircleTheorems::twoVectorCross(c,a));

      const double extraTerm = averageHitsTotal*(averageHitsTotal-one)*(averageHitsTotal-two);

      const double ans = one/(one+((extraTerm*aSig*modCross*(one-PS))/(two*aAllCubed*muCubed*rProb*rSq*PS)));
      //std::cerr << (one-PS)/PS << " " << modCross/rSq << " " << extraTerm*aSig/(two*aAllCubed*muCubed*rProb) << " " << rProb << " ans " <<ans<<" "<<a<<" " <<b<<" " <<c<<std::endl;
      return ans;
    };


  public:
    static inline double PROPTO_priorProbabilityOfTwoPointsBeingOnACircle
    (const Hep2Vector & a,
     const Hep2Vector & b) {
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
        = Constants::circleHitsPerUnitLengthParameter;
      const double muSq = mu*mu;

      const double averageAreaScale = Constants::areaScaleForEverything*Constants::areaScaleForEverything / Constants::areaScaleForSignal;

      static bool first = true;
      if (first) {
        first = false;
        std::cerr << "Estimation of PS could be improved by looking at the present number of hits rather than using the average number, and by removing crude dependence on average radius etc.  This would be very important if the circle radius distribution became bimodal or hits per unit arc length came to depend upon r." << std::endl;
        // for example, could base estimate on evidence of current fit!
      };
      const double one = 1;
      //const double two = 2;
      const double averageHitsFromACircle = MathsConstants::pi * Constants::circleMeanRadiusParameter * Constants::circleHitsPerUnitLengthParameter;
      const double averageHitsFromCircles = averageHitsFromACircle * Constants::meanNumberOfRings;
      const double averageHitsFromBg = Constants::backgroundMeanParameter;
      const double averageHitsTotal=averageHitsFromCircles+averageHitsFromBg;
      const double signalPurity = averageHitsFromCircles/(averageHitsTotal);
      const double signalPuritySq = signalPurity*signalPurity;
      const double PS = signalPuritySq/Constants::meanNumberOfRings;

      const double extraTerm = averageHitsTotal*(averageHitsTotal-one);

      const double sepFun=approxCoPointSepFunctionPart1(deltaOnTwo);
      const double ans
        = one/(one+(one-PS)*extraTerm/(PS * averageAreaScale * muSq * sepFun));
      //std::cerr << "jjj " << ans << " "<<(one-PS)/PS << " " << averageAreaScale * muSq << " " <<extraTerm<<" " <<sepFun << std::endl;

      return ans;
    };


    static inline Hep2Vector sampleFromCircleCentreDistribution() {
      return Hep2Vector(RandGauss::shoot(Constants::circleCenXMean,Constants::circleCenXSig),
                        RandGauss::shoot(Constants::circleCenYMean,Constants::circleCenYSig));
    };
    static inline double priorProbabilityOfCentre(const Hep2Vector & cent) {
      return
        gaussianProb(cent.x(), Constants::circleCenXMean, Constants::circleCenXSig)*
        gaussianProb(cent.y(), Constants::circleCenYMean, Constants::circleCenYSig);
    };



  };

};

#endif
