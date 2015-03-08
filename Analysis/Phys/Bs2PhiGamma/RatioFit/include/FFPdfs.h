#ifndef FFPDFS_H
#define FFPDFS_H

#include <cmath>
#include <functional>
#include <vector>

#include "FFAux.h"
#include "FFConfig.h"

class FFPdfs
{
public:
    static void initialize();

    static double Bs2PhiGamma(double* x, double* par);
    static double Bs2PhiGamma_5par(double* x, double* par);
    static double Bd2KstGamma(double* x, double* par);
    static double Bd2KstGamma_custom(double*x, double* par);
    static double Bd2KstGamma_1par(double* x, double* par);
    static double Bd2KstGamma_4par(double* x, double* par);
    static double Bd2KstGamma_mc2012(double* x, double* par);
    static double Bd2JPsiKst(double* x, double* par);
    static double CombBkg(double* x, double* par);

    static bool FitResolutionBias;
    static double MuPhi;
    static double MuKst;
    static double accBs_mean_beta;
    static double accBs_mean_a;
    static double accBs_mean_n;
    static double accBs_mean_t0;
    static double accBd_mean_beta;
    static double accBd_mean_a;
    static double accBd_mean_n;
    static double accBd_mean_t0;
    static double Gs;
    static double DGs;
    static double Gd;
};

#endif // FFPDFS_H
