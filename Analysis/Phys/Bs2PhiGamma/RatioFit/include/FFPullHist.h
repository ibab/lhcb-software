#ifndef FFPULLHIST_H
#define FFPULLHIST_H

#include <iostream>
#include <string>
#include <vector>

#include "TCanvas.h"
#include "TColor.h"
#include "TH1F.h"
#include "TLegend.h"
#include "TLine.h"
#include "TPaveText.h"
#include "TMath.h"
#include "TPad.h"
#include "TStyle.h"

class FFPullHist
{
public:
    FFPullHist();
    ~FFPullHist();
    void addHistogram(const TH1F* hist, Color_t color, std::string drawOptions, std::string legendText, bool validErrors = true);
    void addText(std::string text);
    void draw(const std::string& filename);
    void setMaximum(double max);
    void setMinimum(double min);
    void setTitle(std::string title);
private:
    std::vector<double> binlist;
    bool drawLegend;
    std::string filename;
    std::vector<TH1F*> histograms;
    std::vector<Color_t> colors;
    std::vector<std::string> drawOptions;
    std::vector<std::string> legendTexts;
    std::vector<std::string> paveTexts;
    std::string title;
    std::vector<bool> validErrors;
    double min;
    double max;
};

#endif // FFPULLHIST_H
