#include <iostream>
#include <string>
#include "Utils/DiscreteProbabilityTree.h"
//#include "Utils/OrderedProbabilitySet.h"
#include "stringToNumber/stringToNumber.h"

#include "Utils/ExponentialDistribution.h"
#include "Utils/NormalDistribution.h"
//#include "Utils/ProbabilityModuleSum.h"
#include "Utils/SampleAndProbabilityModuleSum.h"
#include "Utils/ConditionsStream.h"



int main() {
  ConditionsStream::instance() << "I thought I should note that there is a very ver y very long   very ver y very long  very ver y very long very ver y very long  very ver y very long  very ver y very long  very ver y very long  very ver y very long  very ver y very long  very ver y very long  very ver y very long  very ver y very long ry long very ver y very long  very ver y very long  very ver y very long  very ver y very long  very ver y very long  very ver y very long  very ver y very long  very ver y very long  very ver y very long  very ver y very long  very ver y very long sdhf slkdfh lkhf gldgh ldkh lfg hldry long very ver y very long  very ver y very long  very ver y very long  very ver y very long  very ver y very long  very ver y very long  very ver y very long  very ver y very long  very ver y very long  very ver y very long  very ver y very long sdhf slkdfh lkhf gldgh ldkh lfg hldry long very ver y very long  very ver y very long  very ver y very long  very ver y very long  very ver y very long  very ver y very long  very ver y very long  very ver y very long  very ver y very long  very ver y very long  very ver y very long sdhf slkdfh lkhf gldgh ldkh lfg hldry long very ver y very long  very ver y very long  very ver y very long  very ver y very long  very ver y very long  very ver y very long  very ver y very long  very ver y very long  very ver y very long  very ver y very long  very ver y very long sdhf slkdfh lkhf gldgh ldkh lfg hldry long very ver y very long  very ver y very long  very ver y very long  very ver y very long  very ver y very long  very ver y very long  very ver y very long  very ver y very long  very ver y very long  very ver y very long  very ver y very long sdhf slkdfh lkhf gldgh ldkh lfg hld very ver y very long  very ver y very long sdhf slkdfh lkhf gldgh ldkh lfg hldkgh lk list of conditions\n";
  ConditionsStream::instance() << "I thought I should note that\n";
  boost::shared_ptr<ExponentialDistribution> expDist(new ExponentialDistribution());
  boost::shared_ptr<NormalDistribution> normDist(new NormalDistribution(-1,0.3));
  ConditionsStream::instance() << "there are two lines of extra conditions\n";
  SampleAndProbabilityModuleSum<double> sum(expDist,normDist,1,1);
  for (double i=-2; i<4; i+=0.001) {
    ConditionsStream::instance().flushTo(std::cout);
    std::cout << i << "\t" << sum.probabilityOf(i) << std::endl;
    std::cout << i << "\t" << -exp(sum.logOfProbabilityOf(i)) << std::endl;
  };
  unsigned int i=0;
  while(true) {
    std::cout << i++ << "\t" << sum.sample() << std::endl;
  };
};
