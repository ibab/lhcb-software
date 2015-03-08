#ifndef FFAUX_H
#define FFAUX_H

#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>
#include <algorithm>
#include <cctype>
#include <sys/stat.h>

#include "FFConfig.h"

#include "TCanvas.h"
#include "TCut.h"
#include "TFile.h"
#include "TH1F.h"
#include "TMath.h"
#include "TTree.h"
#include "TStopwatch.h"

/**
    @brief A set of general purpose auxiliary functions
*/
class FFAux {
public:
    static double* Cholesky(double *A, int n);
    static bool ExistFile(const std::string& filename);
	static double FastPow(double a, double b);
	static TH1F FromHist(const std::string& filepath, const std::string& histname_in, const std::string& histname_out);
	static TH1F FromTree(const std::string& filepath, const std::string& treename, const std::string& leafname, const std::string& weightsname, const std::vector<double>& binwalls);
	static void HistDraw(TH1F* hist, std::string options, std::string output);
    static TH1F* HistNorm(TH1F *hist, float area);
	static void ProgressBarTPS(int x, int n, int w, TStopwatch sw);
	static bool toBool(std::string str);
    template<typename T> static std::string toString(T number);
};

/**
    @brief Converts a number into a string.
*/
template<typename T> std::string FFAux::toString(T number) {
	std::ostringstream ss;
	ss << number;
	return ss.str();
}

#endif //FFAUX_H
