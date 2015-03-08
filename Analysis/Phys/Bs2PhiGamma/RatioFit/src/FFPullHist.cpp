#include "FFPullHist.h"

/**
    @class FFPullHist
    @brief This class allows to compare histograms along with a pull/residuals plot

    Input histograms are set by the method AddHistogram. At least two input histograms are needed to be able to call Draw,
    which are the ones that will be used to perform the pulls. The rest will be drawn, but the pulls won't be affected.

    Example of usage:
    @code
        FFPullHist pullHist;
        pullHist->addHistogram(h1, kBlack, "ep", "Data sample");
        pullHist->addHistogram(h2, kRed, "same", "Monte Carlo sample");
        pullHist->draw("fig/file.pdf");
    @endcode
*/

FFPullHist::FFPullHist()
    : drawLegend(false), title("")
{
}

FFPullHist::~FFPullHist()
{
    for(auto histogram : histograms)
        delete histogram;
}

void FFPullHist::addHistogram(const TH1F* hist, Color_t color, std::string drawOptions, std::string legendText, bool validErrors)
{
    histograms.push_back( (TH1F*)hist->Clone() );
    colors.push_back( color );
    this->drawOptions.push_back( drawOptions );
    legendTexts.push_back( legendText );
    this->validErrors.push_back( validErrors );

    if(histograms.size() == 1)
    {
        for(int i = 1; i < histograms.back()->GetEntries(); ++i)
            binlist.push_back( histograms.back()->GetBinLowEdge(i) );
    }
    else
    {
        for(int i = 1; i < histograms.back()->GetEntries(); ++i)
        {
            if(histograms.back()->GetBinLowEdge(i) != binlist[i-1])
                std::cout << "Error: Adding histograms with different binning" << std::endl;
        }
    }
}

void FFPullHist::addText(std::string text) {
    paveTexts.push_back(text);
}

void FFPullHist::draw(const std::string& filename)
{
    //Error checking
    if(histograms.size() < 2) {
        std::cout << "Error: At least two input histograms are necessary to draw the pull histogram" << std::endl;
        return;
    }

    TCanvas *canvas = new TCanvas("canvas","",800,800);
    canvas->SetBottomMargin(0.2);
    canvas->SetFillColor(10);
    double r = 0.2;
    TPad *pad1 = new TPad("pad1", "The pad 80% of the height", 0.0, r  , 1.0, 1.0);
    TPad *pad2 = new TPad("pad2", "The pad 20% of the height", 0.0, 0.0, 1.0, r  );
    pad2->SetBottomMargin(0.25);
    pad1->SetLeftMargin(0.13);
    pad2->SetLeftMargin(0.13);
    pad2->SetBottomMargin(0.2);
    pad1->Draw();
    pad2->Draw();

    //Distributions
    pad1->cd();
    histograms[0]->SetStats(0);
    histograms[0]->SetLabelSize(0.045,"XY"); //mod
    histograms[0]->SetTitleSize(0.045,"XY"); //mod
    histograms[0]->SetLineColor(colors[0]);
    histograms[0]->SetMarkerColor(colors[0]);
    histograms[0]->SetMarkerStyle(kFullCircle);
    histograms[0]->SetMarkerSize(1.0);
    histograms[0]->SetLineWidth(1);
    histograms[0]->SetTitleOffset(1.4,"y"); //mod
    histograms[0]->SetTitleOffset(1.1,"x"); //mod
    histograms[0]->SetMinimum(min);
    histograms[0]->SetMaximum(max);
    histograms[0]->Draw("ep");

    histograms[1]->SetLineColor(colors[1]);
    histograms[1]->SetMarkerColor(colors[1]);
    histograms[1]->SetLineWidth(1.8);
    histograms[1]->Draw("same");

    //Legend
    if(drawLegend) {
        TLegend *legend = new TLegend(0.55,0.64,0.82,0.82);
        legend->SetFillColor(0);
        legend->SetTextSize(0.035);
        legend->AddEntry(histograms[0], legendTexts[0].c_str(),"P");
        legend->AddEntry(histograms[1], legendTexts[1].c_str(),"L");
        legend->SetBorderSize(0);
        legend->Draw("same");
    }

    /**
        @todo Change borders depending on the number of paveTexts (after AddText). Probably
        define some borders as private members
    */
    //PaveTexts
    TPaveText *text = new TPaveText(gStyle->GetPadLeftMargin() + 0.4,
                               0.82 - gStyle->GetPadTopMargin(),
                               gStyle->GetPadLeftMargin() + 0.75,
                               0.95 - gStyle->GetPadTopMargin(),
                               "BRNDC");
    for(size_t i=0; i < paveTexts.size(); ++i) {
        text->AddText(paveTexts[i].c_str());
    }
    text->SetFillColor(0);
    text->SetTextAlign(12);
    text->SetBorderSize(0);
    text->Draw("same");

    //Residuals
    pad2->cd();
    int N = histograms[1]->GetXaxis()->GetNbins();
    double xmin = histograms[1]->GetXaxis()->GetXmin();
    double xmax = histograms[1]->GetXaxis()->GetXmax();
    //TH1F* pullHist = new TH1F("pullHist","", N, xmin, xmax);
    TH1F* pullHist = new TH1F(*histograms[0]);
    pullHist->SetTitle("pullHist");
    for (int i = 0; i <= N; ++i) {
        double y1 = histograms[1]->GetBinContent(i);
        double y2 = histograms[0]->GetBinContent(i);
        double res = y1 - y2;
        double e1 = histograms[1]->GetBinError(i);
        double e2 = histograms[0]->GetBinError(i);
        if( !validErrors[1] ) e1 = 0;
        if( !validErrors[0] ) e2 = 0;
        double sigma = sqrt(e1*e1 + e2*e2);
        double pull;
        if(e1 != 0 || e2 != 0)
            pull = res/sigma;
        else
            pull = 0.;
            pullHist->SetBinContent(i, pull);
    }
    double sr = 1/(r+0.1);
    TAxis* xr = pullHist->GetXaxis();
    xr->SetTickLength(xr->GetTickLength()*sr);
    xr->SetLabelSize(xr->GetLabelSize()*sr);
    xr->SetTitleSize(xr->GetTitleSize()*sr);
    xr->SetLabelOffset(xr->GetLabelOffset()*sr);
    TAxis* yr = pullHist->GetYaxis();
    yr->SetNdivisions(504);
    yr->SetLabelSize(yr->GetLabelSize()*sr);
    pullHist->SetMaximum(5);
    pullHist->SetMinimum(-5);
    pullHist->SetMarkerStyle(20);
    pullHist->SetLineWidth(1.4);
    pullHist->SetLineColor(kBlack);
    pullHist->SetStats(0);
    pullHist->SetTitle("");
    pullHist->SetFillColor(kGray);
    pullHist->Draw("hist");
    TLine* centerLine = new TLine(xmin, 0, xmax, 0);
    centerLine->Draw("SAME");
    TLine* m2SigLine = new TLine(xmin, -2, xmax, -2);
    m2SigLine->SetLineColor(kBlack);
    m2SigLine->SetLineStyle(5);
    m2SigLine->Draw("SAME");
    TLine* p2SigLine = new TLine(xmin, +2, xmax, +2);
    p2SigLine->SetLineColor(kBlack);
    p2SigLine->SetLineStyle(5);
    p2SigLine->Draw("SAME");

    //Save
    histograms[0]->SetTitle( title.c_str() );
    canvas->SaveAs( filename.c_str() );

    pullHist->Delete();
    delete canvas;
}

void FFPullHist::setMaximum(double max) { this->max = max; }
void FFPullHist::setMinimum(double min) { this->min = min; }
void FFPullHist::setTitle(std::string title) { this->title = title; }
