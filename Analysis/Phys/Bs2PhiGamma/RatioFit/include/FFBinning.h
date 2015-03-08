#ifndef FFBINNING_H
#define FFBINNING_H

#include <vector>
#include "TF1.h"
#include "TH1.h"
#include "TRandom3.h"
#include "FFConfig.h"

/**
    @brief Different types of binning
*/
class FFBinning
{
    public:
        static std::vector<double> adaptive(TF1* f1, double param);
        static std::vector<double> manual();
};

#endif // FFBINNING_H
