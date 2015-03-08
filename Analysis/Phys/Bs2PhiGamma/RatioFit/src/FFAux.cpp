#include "FFAux.h"

/**
    @file Implementation file of the class FFAux
    @author Carlos Sánchez Mayordomo (casanche@cern.ch) (IFIC, Valencia)
*/

/**
    @brief Cholesky decomposition \f$ A = L L^* \f$

    @param A Input matrix as a pointer array
    @param n Dimension of the A matrix
    @return Lower triangular matrix L
*/
double* FFAux::Cholesky(double *A, int n) {
	double *L = (double*)calloc(n * n, sizeof(double));
	if (L == NULL)
		exit(EXIT_FAILURE);

    for (int i = 0; i < n; i++)
		for (int j = 0; j < (i+1); j++) {
			double s = 0;
			for (int k = 0; k < j; k++)
				s += L[i * n + k] * L[j * n + k];
			L[i * n + j] = (i == j) ?
						   sqrt(A[i * n + i] - s) :
						   (1.0 / L[j * n + j] * (A[i * n + j] - s));
		}

	return L;
}

/**
 *  @brief Checks if the file exists or not
 *
 *  #include <sys/stat.h>
 */
bool FFAux::ExistFile(const std::string& filename)
{
    struct stat buffer;
    return ( stat(filename.c_str(), &buffer) == 0 );
}

/**
    @brief Faster alternative (sometimes) to the standard pow. \f$ a^{b} \f$

    Since log(a) is calculated, the parameter "a" can't be negative.

    @param a base
    @param b exponent
*/
double FFAux::FastPow(double a, double b) {
	return exp( b*log(a) );
}

/**
    @brief Get an histogram stored in a file
*/
TH1F FFAux::FromHist(const std::string& filepath, const std::string& histname_in, const std::string& histname_out) {
    TFile file(filepath.c_str());
    TH1F* hpointer = (TH1F*)file.Get(histname_in.c_str());
    TH1F hist = TH1F(*hpointer);
    hist.SetName(histname_out.c_str());
    hist.SetTitle(histname_out.c_str());
    return hist;
}
/**
    @brief Get an histogram from a tree
*/
TH1F FFAux::FromTree(const std::string& filepath, const std::string& treename, const std::string& leafname, const std::string& weightsname, const std::vector<double>& binwalls) {
    TFile file(filepath.c_str());
    TTree *tree;
    file.GetObject(treename.c_str(), tree);

    if(weightsname == "") {
        TH1F hist("hist", "hist", 100, 0, 10);
        hist.Sumw2();
        tree->Draw((leafname + ">> hist").c_str());
        return hist;
    } else {
        double tau, sWeight;
        tree->SetBranchAddress(leafname.c_str(), &tau);
        tree->SetBranchAddress(weightsname.c_str(), &sWeight);
        tree->SetBranchStatus("*",0);
        tree->SetBranchStatus(leafname.c_str(),1);
        tree->SetBranchStatus(weightsname.c_str(),1);

        TH1F hist("hist", "hist", binwalls.size() - 1, &binwalls[0]);
        hist.Sumw2();
        for(int i = 0; i < tree->GetEntries(); ++i) {
            tree->GetEntry(i);
            hist.Fill(tau, sWeight);
        }
        return hist;
    }
}

/**
    @brief Fast drawing an histogram

    Encapsulates the creation of a canvas
*/
void FFAux::HistDraw(TH1F* hist, std::string options, std::string output) {
    TCanvas *canvas = new TCanvas();
    hist->Draw(options.c_str());
    if(output != "") {
        canvas->SaveAs(output.c_str());
    }
    delete canvas;
}

/**
    @brief Returns the input histogram normalized to "area"
*/
TH1F* FFAux::HistNorm(TH1F *hist, float area) {
    double scale;
    scale = area / ( hist->Integral() * hist->GetBinWidth(1) );
    hist->Scale(scale);
    return hist;
}

 /**
    @brief A progress bar showing the current toy number and percentage.

    Includes:
    @code
        #include <iostream>
        #include <iomanip>
        #include "TStopwatch.h"
    @endcode

    @param x Iteration index
    @param n Total number of iterations
    @param w Width to display in screen
 */
void FFAux::ProgressBarTPS(int x, int n, int w, TStopwatch sw) {
	double tps = x / sw.CpuTime();
	if( (x != n) && ( x % (n / n) != 0 ) ) return;
	float ratio = x/(float)n;
	int c = ratio * w;
	std::cout << std::setw(3) << (int)(ratio*100) << "% [";
	for(int x=0; x<c; ++x) std::cout << "=";
	for(int x=c; x<w; ++x) std::cout << " ";
	std::cout << "] " << x << ", " << std::setprecision(4) << tps << " toys/s   " << "\r" << std::flush;
	if(ratio >= 1) std::cout << std::endl;
}

/**
    @brief Converts a string to a bool.

    Includes:
    @code
        #include <sstream>
        #include <string>
        #include <iomanip>
        #include <algorithm>
        #include <cctype>
    @endcode

    @param str The string to be converted. It should be "true" or "false",
    and is case insensitive.
    Accepted values could be: TRUE, true, tRuE, FALSE...
*/
bool FFAux::toBool(std::string str) {
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    std::istringstream is(str);
    bool b;
    is >> std::boolalpha >> b;
    return b;
}
