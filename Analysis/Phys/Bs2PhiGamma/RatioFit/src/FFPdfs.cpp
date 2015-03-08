#include "FFPdfs.h"

/**
    @class FFPdfs
    @author Carlos Sánchez Mayordomo (casanche@cern.ch) (IFIC, Valencia)
*/

void FFPdfs::initialize() {
    //TODO: check first that FFConfig has been read
    FitResolutionBias = FFConfig::get<bool>("FitResolutionBias");
    MuPhi = FFConfig::get<double>("MuPhi");
    MuKst = FFConfig::get<double>("MuKst");
    accBs_mean_beta = FFConfig::get<double>("accBs_mean_beta");
    accBs_mean_a = FFConfig::get<double>("accBs_mean_a");
    accBs_mean_n = FFConfig::get<double>("accBs_mean_n");
    accBs_mean_t0 = FFConfig::get<double>("accBs_mean_t0");
    accBd_mean_beta = FFConfig::get<double>("accBd_mean_beta");
    accBd_mean_a = FFConfig::get<double>("accBd_mean_a");
    accBd_mean_n = FFConfig::get<double>("accBd_mean_n");
    accBd_mean_t0 = FFConfig::get<double>("accBd_mean_t0");
    Gs = FFConfig::get<double>("Gs");
    DGs = FFConfig::get<double>("DGs");
    Gd = FFConfig::get<double>("Gd");
}

double FFPdfs::Bs2PhiGamma(double* x, double* par)
{
    if(FitResolutionBias) {
        x[0] -= MuPhi;
    }

    double acc_beta = accBs_mean_beta;
    double acc_a = accBs_mean_a;
    double acc_n = accBs_mean_n;
    double acc_t0 = accBs_mean_t0;

    double accterm = pow( acc_a * (x[0] - acc_t0), acc_n);
    double acc = ( 1 - 1/( accterm + 1 ) ) * exp(- acc_beta * x[0]);

    double ideal = exp( - Gs*x[0] ) * ( cosh( DGs*x[0]/2 ) - par[0]*sinh( DGs*x[0]/2 ) );

    if(x[0] > acc_t0) {
        double result = acc * ideal;
        if(result > 0) {
            return result;
        } else {
            return 0.0f;
        }
    } else {
        return 0.0f;
    }
}

double FFPdfs::Bs2PhiGamma_5par(double* x, double* par)
{
    if(FitResolutionBias) {
        x[0] -= MuPhi;
    }

    double acc_beta = par[4];
    double acc_a = par[1];
    double acc_n = par[2];
    double acc_t0 = par[3];

    double accterm = pow( acc_a * (x[0] - acc_t0), acc_n);
    double acc = ( 1 - 1/( accterm + 1 ) ) * exp(- acc_beta * x[0]);

    double ideal = exp( - Gs*x[0] ) * ( cosh( DGs*x[0]/2 ) - par[0]*sinh( DGs*x[0]/2 ) );

    if(x[0] > acc_t0) {
        double result = acc * ideal;
        if(result > 0) {
            return result;
        } else {
            return 0.0f;
        }
    } else {
        return 0.0f;
    }
}

double FFPdfs::Bd2KstGamma(double* x, double* par)
{
    if(FitResolutionBias) {
        x[0] -= MuKst;
    }

    double acc_beta = accBd_mean_beta;
    double acc_a = accBd_mean_a;
    double acc_n = accBd_mean_n;
    double acc_t0 = accBd_mean_t0;

    double accterm = pow( acc_a * (x[0] - acc_t0), acc_n);
    double acc = ( 1 - 1/( accterm + 1 ) ) * exp(- acc_beta * x[0]);

    double ideal = exp( -Gd*x[0] );

    if(x[0] > acc_t0) {
        double result = acc * ideal;
        if(result > 0) {
            return result;
        } else {
            return 0.0f;
        }
    } else {
        return 0.0f;
    }
}

double FFPdfs::Bd2KstGamma_custom(double*x, double* par) {
    if(FitResolutionBias) {
        x[0] -= MuKst;
    }

    double acc_beta = par[3];
    double acc_a = par[0];
    double acc_n = par[1];
    double acc_t0 = par[2];

    double accterm = pow( acc_a * (x[0] - acc_t0), acc_n);
    double acc = ( 1 - 1/( accterm + 1 ) ) * exp(- acc_beta * x[0]);

    double ideal = exp( -Gd*x[0] );

    if(x[0] > acc_t0) {
        double result = acc * ideal;
        if(result > 0) {
            return result;
        } else {
            return 0.0f;
        }
    } else {
        return 0.0f;
    }
}

double FFPdfs::Bd2KstGamma_1par(double* x, double* par)
{
    if(FitResolutionBias) {
        x[0] -= MuKst;
    }

    double acc_beta = accBd_mean_beta;
    double acc_a = accBd_mean_a;
    double acc_n = accBd_mean_n;
    double acc_t0 = accBd_mean_t0;

    double accterm = pow( acc_a * (x[0] - acc_t0), acc_n);
    double acc = ( 1 - 1/( accterm + 1 ) ) * exp(- acc_beta * x[0]);

    double ideal = exp( -par[0]*x[0] );

    if(x[0] > acc_t0) {
        double result = acc * ideal;
        if(result > 0) {
            return result;
        } else {
            return 0.0f;
        }
    } else {
        return 0.0f;
    }
}

double FFPdfs::Bd2KstGamma_4par(double* x, double* par) {
    if(FitResolutionBias) {
        x[0] -= MuKst;
    }

    double acc_beta = par[3];
    double acc_a = par[0];
    double acc_n = par[1];
    double acc_t0 = par[2];

    double accterm = pow( acc_a * (x[0] - acc_t0), acc_n);
    double acc = ( 1 - 1/( accterm + 1 ) ) * exp(- acc_beta * x[0]);

    double ideal = exp( -Gd*x[0] );

    if(x[0] > acc_t0) {
        double result = acc * ideal;
        if(result > 0) {
            return result;
        } else {
            return 0.0f;
        }
    } else {
        return 0.0f;
    }
}

double FFPdfs::Bd2KstGamma_mc2012(double* x, double* par)
{
    double Gd_mc = 1 / 1.519;

    if(FitResolutionBias) {
        x[0] -= MuKst;
    }

    double acc_beta = accBd_mean_beta;
    double acc_a = accBd_mean_a;
    double acc_n = accBd_mean_n;
    double acc_t0 = accBd_mean_t0;

    double accterm = pow( acc_a * (x[0] - acc_t0), acc_n);
    double acc = ( 1 - 1/( accterm + 1 ) ) * exp(- acc_beta * x[0]);

    double ideal = exp( -Gd_mc * x[0] );

    if(x[0] > acc_t0) {
        double result = acc * ideal;
        if(result > 0) {
            return result;
        } else {
            return 0.0f;
        }
    } else {
        return 0.0f;
    }
}

double FFPdfs::Bd2JPsiKst(double* x, double* par) {
    if(FitResolutionBias) {
        x[0] -= MuKst;
    }

    double acc_beta = accBd_mean_beta - 0.0331;
    double acc_a = accBd_mean_a;
    double acc_n = accBd_mean_n;
    double acc_t0 = accBd_mean_t0;

    double accterm = pow( acc_a * (x[0] - acc_t0), acc_n);
    double acc = ( 1 - 1/( accterm + 1 ) ) * exp(- acc_beta * x[0]);

    double ideal = exp( -Gd*x[0] );

    if(x[0] > acc_t0) {
        double result = acc * ideal;
        if(result > 0) {
            return result;
        } else {
            return 0.0f;
        }
    } else {
        return 0.0f;
    }
}

double FFPdfs::CombBkg(double* x, double* par) {
    double acc_beta = accBd_mean_beta;
    double acc_a = accBd_mean_a;
    double acc_n = accBd_mean_n;
    double acc_t0 = accBd_mean_t0;

    double accterm = pow( acc_a * (x[0] - acc_t0), acc_n);
    double acc = ( 1 - 1/( accterm + 1 ) ) * exp(- acc_beta * x[0]);

    double ideal = exp( - Gd*x[0] ) * cosh( DGs*x[0]/2 );

    if(x[0] > acc_t0) {
        double result = acc * ideal;
        if(result > 0) {
            return result;
        } else {
            return 0.0f;
        }
    } else {
        return 0.0f;
    }
}

bool FFPdfs::FitResolutionBias = 0;
double FFPdfs::MuPhi = 0;
double FFPdfs::MuKst = 0;
double FFPdfs::accBs_mean_beta = 0;
double FFPdfs::accBs_mean_a = 0;
double FFPdfs::accBs_mean_n = 0;
double FFPdfs::accBs_mean_t0 = 0;
double FFPdfs::accBd_mean_beta = 0;
double FFPdfs::accBd_mean_a = 0;
double FFPdfs::accBd_mean_n = 0;
double FFPdfs::accBd_mean_t0 = 0;
double FFPdfs::Gs = 0;
double FFPdfs::DGs = 0;
double FFPdfs::Gd = 0;
