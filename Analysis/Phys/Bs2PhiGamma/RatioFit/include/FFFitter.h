#ifndef FFFITTER_H
#define FFFITTER_H

#include <cmath>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include "TCanvas.h"
#include "TF1.h"
#include "TGraph.h"
#include "TH1F.h"
#include "TMinuit.h"
#include "FFAux.h"
#include "FFBlind.h"
#include "FFConfig.h"
#include "FFPullHist.h"

class FFFitter {
public:
    static void initialize(TH1F* hist1, TF1* func1, TH1F* hist2, TF1* func2);

    static void direct(TH1F* hist1, TF1* func1);
	static void ratio(TH1F* hist1, TF1* func1, TH1F* hist2, TF1* func2);
	static void simultaneous(TH1F* hist1, TF1* func1, TH1F* hist2, TF1* func2);

	static double* param;
	static double* parerr;
private:
    static double blindingFactor(int i, double ADelta);
    static void draw(std::string title, std::string output);
    static void drawSimultaneous(std::string title, std::string output);

    static void directChi2(int &par, double *gin, double &f, double *u, int flag);
    static void ratioChi2(int &npar, double *gin, double &f, double *u, int flag);
    static void ratioLikelihood(int &npar, double *gin, double &f, double *u, int flag);
    static void simultaneousChi2(int &npar, double *gin, double &f, double *u, int flag);

    static TH1F* h1;
    static TH1F* h2;
    static TH1F* hobserved;
    static TH1F* hexpected;
    static TF1* f1;
    static TF1* f2;
    static int npar1;
    static int npar2;
    static int Nparam;
    static double xmin_norm;
    static double xmax_norm;
    static double norm1;
    static double norm2;
    static int nbins;
    static std::vector<double> binwalls;
    static std::vector<double> integrals1;
    static std::vector<double> integrals2;
    static std::vector<double> vobserved;
    static std::vector<double> vobserved2;
    static std::vector<double> verror;
    static std::vector<double> verror2;
    static std::vector<double> vexpected;
    static std::vector<double> vexpected2;
    static double chi2;
    static double loglike;
    static FFBlind blindEngine;
};

#endif //FFFITTER_H
