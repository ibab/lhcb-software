#ifndef RICHMARKOV_RICH_CONFIGURATION_H
#define RICHMARKOV_RICH_CONFIGURATION_H

#include "Configuration.h"
#include "Constants.h"

#include "probabilityUtilities/FiniteRelativeProbabilityChooser.h"
#include "probabilityUtilities/RegularProbabilityDistribution1D.h"
#include "probabilityUtilities/ProbabilityAbove.h"

namespace RichMarkov {
  
  typedef FiniteRelativeProbabilityChooser<double> RichThetaSampler;
  typedef RegularProbabilityDistribution1D<double> RichThetaDistribution;
  typedef ProbabilityAbove<double>                 RichProbabilityThetaAbove;

  const std::string thetaDirDeJour = getenv("PARAMFILESROOT");
  const std::string thetaFileR2DeJour = "/data/Rich2_MarkovRingFinder_thetaDist.txt";
  const std::string thetaFileR1DeJour = "/data/Rich1_MarkovRingFinder_thetaDist.txt";

  
  class RichConfiguration : public Configuration { 

  public:

    virtual double geometricXYAcceptanceLeftBound() const = 0;
    virtual double geometricXYAcceptanceRightBound() const = 0;
    virtual double geometricXYAcceptanceBottomBound() const = 0;
    virtual double geometricXYAcceptanceTopBound() const = 0;

    virtual double realXYDataInputScaleFactor() const = 0;

    virtual ~RichConfiguration() = 0;
    
    double geometricThetaAcceptanceLeftBound() const {
      return realXYDataInputScaleFactor() * geometricXYAcceptanceLeftBound();
    };
    double geometricThetaAcceptanceRightBound() const {
      return realXYDataInputScaleFactor() * geometricXYAcceptanceRightBound();
    };
    double geometricThetaAcceptanceBottomBound() const {
      return realXYDataInputScaleFactor() * geometricXYAcceptanceBottomBound();
    };
    double geometricThetaAcceptanceTopBound() const {
      return realXYDataInputScaleFactor() * geometricXYAcceptanceTopBound();
    };

    inline const RichThetaSampler & thetaSampler() const { return richThetaSampler; }
    inline double thetaDistribution(const double r) const { return richThetaDistribution(r); }
    inline double probabilityThetaAbove(const double r) const { return richThetaDistribution(r); }

    virtual const std::string approxCoPointSepCacheFileName() const = 0;

  protected:

    const RichThetaSampler          richThetaSampler;
    const RichThetaDistribution     richThetaDistribution;
    const RichProbabilityThetaAbove richProbabilityThetaAbove;

  protected:

    RichConfiguration() {};

    RichConfiguration( const std::string & dir, 
                       const std::string & name ) 
      : richThetaSampler(name,dir),
        richThetaDistribution(name,dir),
        richProbabilityThetaAbove(name,dir)
    { }

  };

}

#endif
