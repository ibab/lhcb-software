#include "FFFitter.h"

/**
    @class FFFitter
    @author Carlos Sánchez Mayordomo (casanche@cern.ch) (IFIC, Valencia)
    @author Pablo Ruiz Valls (pruizval@cern.ch) (IFIC, Valencia)
*/

///
/// @todo Number of parameters from input
/// @todo initialize: do it separately for each single function
///

/**
 * It takes: the binwalls (from h1), the normalizations (if Nparam is zero for f1/f2)
 * When to call it: only once, after defining histograms and functions
 */
void FFFitter::initialize(TH1F* hist1, TF1* func1, TH1F* hist2, TF1* func2) {
    h1 = hist1;
    h2 = hist2;
    f1 = func1;
    f2 = func2;

    npar1 = f1->GetNpar();
    npar2 = f2->GetNpar();
    xmin_norm = FFConfig::get<double>("xmin_norm");
    xmax_norm = FFConfig::get<double>("xmax_norm");
    nbins = h1->GetSize() - 2;

    integrals1.resize(nbins);
    integrals2.resize(nbins);
    binwalls.resize(nbins+1);

    //TODO: check that h2 has the same binning as h1
    for(int i = 0; i < nbins + 1; ++i) {
        binwalls[i] = h1->GetBinLowEdge(i+1);
    }

    if(npar1 == 0) {
        norm1 = f1->Integral(xmin_norm, xmax_norm);
        for(int k=0; k < nbins; ++k) {
            double a = h1->GetBinLowEdge(k+1);
            double b = h1->GetBinLowEdge(k+2);
            integrals1[k] = f1->Integral(a, b) / norm1;
        }
    }
    if(npar2 == 0) {
        norm2 = f2->Integral(xmin_norm, xmax_norm);
        for(int k=0; k < nbins; ++k) {
            double a = h2->GetBinLowEdge(k+1);
            double b = h2->GetBinLowEdge(k+2);
            integrals2[k] = f2->Integral(a, b) / norm2;
        }
    }
}

void FFFitter::direct(TH1F* hist1, TF1* func1)
{
    //const int Nparam = func1->GetNumberFreeParameters() + 1;
    const int Nparam = 2;
    double vstart[Nparam] = {4000, 0.0};
	double vstep[Nparam] = {100, 0.1};

    /*const int Nparam = 5;
	double vstart[Nparam] = {24000, 1.9, 1.8, 0.170, 0.08};
	double vstep[Nparam] = {100, 0.1, 0.1, 0.01, 0.005};*/

    h1 = hist1;
    f1 = func1;

    vobserved.clear();
    verror.clear();
    vobserved.reserve(nbins);
    verror.reserve(nbins);
    for(int i=0; i < nbins; ++i) {
        double c1 = h1->GetBinContent(i+1);
        double e1 = h1->GetBinError(i+1);

        vobserved[i] = c1;
        verror[i] = e1;
    }

    TMinuit *minuit = new TMinuit(Nparam);
    minuit->SetPrintLevel(-1);
    minuit->fLwarn = true;

    double arglist[10];
	Int_t ierflag = 0;

	arglist[0] = 1; //1=chi2, 0.5=likelihood
	minuit->mnexcm("SET ERR", arglist, 1, ierflag);

    arglist[0] = 1; //1=standard, 2=improve minimum (slower)
	minuit->mnexcm("SET STR", arglist, 1, ierflag);

	for(int i=0; i < Nparam; ++i) {
        minuit->mnparm(i, ("par" + FFAux::toString(i)).c_str(), vstart[i], vstep[i], 0, 0, ierflag);
	}

    minuit->SetFCN(directChi2);

    arglist[0] = 500;
    arglist[1] = 1.;

    minuit->mnexcm("MIGRAD", arglist, 2, ierflag);
    //minuit->mnexcm("MINOS", arglist, 1, ierflag);
    param = new double[Nparam];
    parerr = new double[Nparam];
    for(int i=0; i < Nparam; ++i) {
        minuit->GetParameter(i, param[i], parerr[i]);
    }

    //Store in FFWrapper
    //...

    if(FFConfig::get<bool>("Verbose")) {
        arglist[0] = 1; //parno
        minuit->mnexcm("SCAN", arglist, 1, ierflag);

        TCanvas* canvas = new TCanvas();
        TGraph *gr = (TGraph*)minuit->GetPlot();
        gr->Draw();
        canvas->SaveAs("fig/minuit.pdf");
        delete canvas;

        std::string prefix = "MINUIT ## ";
        int fStatus = minuit->GetStatus();
        std::cout << prefix << std::endl;
        std::cout << prefix << "FIT: direct" << std::endl;
        if(fStatus == 0) {
            std::cout << prefix << "FSTATUS: converged" << std::endl;
        } else {
            std::cout << prefix << "FSTATUS: error" << std::endl;
        }
        std::cout << prefix << "CALLS: " << minuit->fNfcn << std::endl;
        std::cout << prefix << "FCN MINIMUM: " << minuit->fAmin << std::endl;

        if(FFConfig::get<bool>("Blind")) {
            std::cout << prefix << "NORM: " << param[0] << " +- " << parerr[0] << std::endl;
            std::cout << prefix << "ADELTA (BLINDED): " << blindEngine.hide(param[1]) << " +- " << parerr[1] << std::endl;
        } else {
            std::cout << prefix << "NORM: " << param[0] << " +- " << parerr[0] << std::endl;
            std::cout << prefix << "ADELTA: " << param[1] << " +- " << parerr[1] << std::endl;

            std::cout << prefix << "NORM: " << param[0] << " +- " << parerr[0] << std::endl;
            std::cout << prefix << "PAR1: " << param[1] << " +- " << parerr[1] << std::endl;
            std::cout << prefix << "PAR2: " << param[2] << " +- " << parerr[2] << std::endl;
            std::cout << prefix << "PAR3: " << param[3] << " +- " << parerr[3] << std::endl;
            std::cout << prefix << "PAR4: " << param[4] << " +- " << parerr[4] << std::endl;
        }

        std::cout << prefix << std::endl;
    }

    //TEMP
    /*std::ofstream fout("toys.dat", std::ios::app);
    fout << "\t" << param[1] << "\t" << parerr[1] << std::endl;
    fout.close();*/

    /*std::ofstream fout("toys.dat", std::ios::app);
    for(int i=1; i < Nparam; ++i) {
        fout << "\t" << param[i];
    }
    fout << std::endl;*/

    delete minuit;
}

void FFFitter::ratio(TH1F* hist1, TF1* func1, TH1F* hist2, TF1* func2)
{
    //Nparam = func1->GetNumberFreeParameters() + func2->GetNumberFreeParameters() + 1;
    const int Nparam = 2;
    double vstart[Nparam] = {0.15, 0.0};
	double vstep[Nparam] = {0.1, 0.05};

    h1 = hist1;
    h2 = hist2;
    f1 = func1;
    f2 = func2;

    vobserved.clear();
    verror.clear();
    vobserved.reserve(nbins);
    verror.reserve(nbins);
    for(int i=0; i < nbins; ++i) {
        double c1 = h1->GetBinContent(i+1);
        double e1 = h1->GetBinError(i+1);
        double c2 = h2->GetBinContent(i+1);
        double e2 = h2->GetBinError(i+1);

        vobserved[i] = c1 / c2;
        verror[i] = sqrt( pow(e1/c2, 2) + pow(c1*e2/(c2*c2), 2) );
    }

    TMinuit *minuit = new TMinuit(Nparam);
    minuit->SetPrintLevel(-1);
    minuit->fLwarn = true;

    double arglist[10];
	Int_t ierflag = 0;

	arglist[0] = 0.5; //1=chi2, 0.5=likelihood
	minuit->mnexcm("SET ERR", arglist, 1, ierflag);

    arglist[0] = 2; //1=standard, 2=improve minimum (slower)
	minuit->mnexcm("SET STR", arglist, 1, ierflag);

    minuit->mnparm(0, "Norm",   vstart[0], vstep[0], 0, 0, ierflag);
	// minuit->mnparm(0, "Norm",   vstart[0], 0, 0, 0, ierflag);
	minuit->mnparm(1, "ADelta", vstart[1], vstep[1], 0, 0, ierflag);

    // minuit->SetFCN(ratioChi2);
    minuit->SetFCN(ratioLikelihood);

    arglist[0] = 500;
    arglist[1] = 0.0001;

    minuit->mnexcm("MIGRAD", arglist, 2, ierflag);
    //minuit->mnexcm("MINOS", arglist, 1, ierflag);
    param = new double[Nparam];
    parerr = new double[Nparam];
    minuit->GetParameter(0, param[0], parerr[0]);
    minuit->GetParameter(1, param[1], parerr[1]);

    if(FFConfig::get<bool>("Verbose")) {
        /* SCAN
        arglist[0] = 0; //parno
        arglist[1] = 50; //numpts
        arglist[2] = param[1] - 3*parerr[1]; //min
        arglist[3] = param[1] + 3*parerr[1]; //max
        minuit->mnexcm("SCAN", arglist, 4, ierflag);

        TCanvas* canvas = new TCanvas();
        TGraph *gr = (TGraph*)minuit->GetPlot();
        gr->Draw();
        gr->GetXaxis()->SetLabelSize(0);
        gr->SetTitle("Ratio Scan;Blinded XAxis (#pm 3 #sigma);");

        canvas->SaveAs("fig/minuit.pdf");
        delete canvas;*/

        std::string prefix = "MINUIT ## ";
        std::cout << prefix << std::endl;
        std::cout << prefix << "FIT: ratio" << std::endl;
        int fStatus = minuit->GetStatus();
        if(fStatus == 0) {
            std::cout << prefix << "FSTATUS: converged" << std::endl;
        } else {
            std::cout << prefix << "FSTATUS: error" << std::endl;
        }
        std::cout << prefix << "CALLS: " << minuit->fNfcn << std::endl;
        std::cout << prefix << "FCN MINIMUM: " << minuit->fAmin << std::endl;

        std::cout << prefix << "NORM: " << param[0] << " +- " << parerr[0] << std::endl;
        if(FFConfig::get<bool>("Blind")) {
            std::cout << prefix << "ADELTA (BLINDED): " << blindEngine.hide(param[1]) << " +- " << parerr[1] << std::endl;
        } else {
            std::cout << prefix << "ADELTA: " << param[1] << " +- " << parerr[1] << std::endl;
        }
        std::cout << prefix << std::endl;

        draw("Direct Loose - PDG 2014", "fig/ratio");
    }

    //TEMP
    /*std::ofstream fout("toys.dat", std::ios::app);
    fout << "\t" << param[1] << "\t" << parerr[1] << std::endl;
    fout.close();*/
    //FFFitter::draw("Trigger: Std", "/home/carlos/Escritorio/draw");

    delete minuit;
}

void FFFitter::simultaneous(TH1F* hist1, TF1* func1, TH1F* hist2, TF1* func2) {
    const int Nparam = 7;
    double vstart[Nparam] = {24603, 3781, 0, 1.8, 2.0, 0.2, 0.095};
	double vstep[Nparam] = {500, 100, 0.1, 0.1, 0.1, 0.01, 0.005};

    h1 = hist1;
    h2 = hist2;
    f1 = func1;
    f2 = func2;

    vobserved.clear();
    vobserved2.clear();
    verror.clear();
    verror2.clear();
    vobserved.reserve(nbins);
    vobserved2.reserve(nbins);
    verror.reserve(nbins);
    verror2.reserve(nbins);
    for(int i=0; i < nbins; ++i) {
        double c1 = h1->GetBinContent(i+1);
        double e1 = h1->GetBinError(i+1);
        double c2 = h2->GetBinContent(i+1);
        double e2 = h2->GetBinError(i+1);

        vobserved[i] = c1;
        verror[i] = e1;
        vobserved2[i] = c2;
        verror2[i] = e2;
    }

    TMinuit *minuit = new TMinuit(Nparam);
    minuit->SetPrintLevel(-1);
    minuit->fLwarn = true;

    double arglist[10];
	Int_t ierflag = 0;

	arglist[0] = 1; //1=chi2, 0.5=likelihood
	minuit->mnexcm("SET ERR", arglist, 1, ierflag);

    arglist[0] = 2; //1=standard, 2=improve minimum (slower)
	minuit->mnexcm("SET STR", arglist, 1, ierflag);

    for(int i=0; i < Nparam; ++i) {
        minuit->mnparm(i, ("par" + FFAux::toString(i)).c_str(), vstart[i], vstep[i], 0, 0, ierflag);
	}

	//minuit->FixParameter(1);
	//minuit->FixParameter(3);
	//minuit->FixParameter(4);
	//minuit->FixParameter(5);
	//minuit->FixParameter(6);

    minuit->SetFCN(simultaneousChi2);

    arglist[0] = 500;
    arglist[1] = 1.;

    minuit->mnexcm("MIGRAD", arglist, 2, ierflag);
    //minuit->mnexcm("MINOS", arglist, 1, ierflag);
    param = new double[Nparam];
    parerr = new double[Nparam];
    for(int i=0; i < Nparam; ++i) {
        minuit->GetParameter(i, param[i], parerr[i]);
    }

    if(FFConfig::get<bool>("Verbose")) {
        arglist[0] = 1; //parno
        minuit->mnexcm("SCAN", arglist, 1, ierflag);

        TCanvas* canvas = new TCanvas();
        TGraph *gr = (TGraph*)minuit->GetPlot();
        gr->Draw();
        canvas->SaveAs("fig/minuit.pdf");
        delete canvas;

        std::string prefix = "MINUIT ## ";
        int fStatus = minuit->GetStatus();
        std::cout << prefix << std::endl;
        std::cout << prefix << "FIT: simultaneous" << std::endl;
        if(fStatus == 0) {
            std::cout << prefix << "FSTATUS: converged" << std::endl;
        } else {
            std::cout << prefix << "FSTATUS: error" << std::endl;
        }
        std::cout << prefix << "CALLS: " << minuit->fNfcn << std::endl;
        std::cout << prefix << "FCN MINIMUM: " << minuit->fAmin << std::endl;

        std::cout << prefix << "PAR0: " << param[0] << " +- " << parerr[0] << std::endl;
        std::cout << prefix << "PAR1: " << param[1] << " +- " << parerr[1] << std::endl;
        if(FFConfig::get<bool>("Blind")) {
            std::cout << prefix << "PAR2: " << blindEngine.hide(param[2]) << " +- " << parerr[2] << std::endl;
        } else {
            std::cout << prefix << "PAR2: " << param[2] << " +- " << parerr[2] << std::endl;
        }
        std::cout << prefix << "PAR3: " << param[3] << " +- " << parerr[3] << std::endl;
        std::cout << prefix << "PAR4: " << param[4] << " +- " << parerr[4] << std::endl;
        std::cout << prefix << "PAR5: " << param[5] << " +- " << parerr[5] << std::endl;
        std::cout << prefix << "PAR6: " << param[6] << " +- " << parerr[6] << std::endl;
        std::cout << prefix << "PAR7: " << param[7] << " +- " << parerr[7] << std::endl;

        std::cout << prefix << std::endl;

        drawSimultaneous("Simultaneous, K*#gamma - PDG 2014", "fig/simultaneous");
    }

    delete minuit;
}

double FFFitter::blindingFactor(int i, double ADelta) {
    double initValue = f1->GetParameter(0);
    f1->SetParameter(0, ADelta);
    double norm = f1->Integral(xmin_norm, xmax_norm);
    double denominator = f1->Integral(binwalls[i], binwalls[i+1]) / norm;
    f1->SetParameter(0, initValue);

    TF1 *fBlind = new TF1(*f2);
    double normBlind = fBlind->Integral(xmin_norm, xmax_norm);
    double numerator = fBlind->Integral(binwalls[i], binwalls[i+1]) / normBlind;

    return numerator / denominator;
}

void FFFitter::draw(std::string title, std::string output) {
    TCanvas* canvas = new TCanvas();
    if(hobserved == nullptr) {
        hobserved = new TH1F(*h1);
    }
    if(hexpected == nullptr) {
        hexpected = new TH1F(*h1);
    }
    for(int k=0; k < nbins; ++k) {
        hobserved->SetBinContent(k+1, vobserved[k]);
        hobserved->SetBinError(k+1, verror[k]);
        hexpected->SetBinContent(k+1, vexpected[k]);
        hexpected->SetBinError(k+1, 0);
    }
    if(FFConfig::get<bool>("Blind")) {
        for(int k=0; k < nbins; ++k) {
            hobserved->SetBinContent(k+1, hobserved->GetBinContent(k+1) * blindingFactor(k, param[1]));
            hobserved->SetBinError(k+1, hobserved->GetBinError(k+1) * blindingFactor(k, param[1]));
            hexpected->SetBinContent(k+1, hexpected->GetBinContent(k+1) * blindingFactor(k, param[1]));
            hexpected->SetBinError(k+1, 0);
        }
    }
    FFPullHist pullHist;
    pullHist.addHistogram(hobserved, kBlack, "ep", "Data", true);
    pullHist.addHistogram(hexpected, kRed, "same", "Fit", true);
    pullHist.setMinimum(0.0);
    pullHist.setMaximum(0.4);

    std::ostringstream stream1;
    if(FFConfig::get<bool>("Blind")) {
        stream1 << "A^{#Delta}_{blind} = " << std::setprecision(3) << blindEngine.hide(param[1]) << " #pm ";
    } else {
        stream1 << "A^{#Delta} = " << std::setprecision(3) << param[1] << " #pm ";
        //stream1 << "#Gamma_{d} = " << std::setprecision(3) << param[1] << " #pm ";
    }
    stream1 << std::setprecision(2) << parerr[1];
    pullHist.addText(stream1.str());

    std::ostringstream stream2;
    stream2 << "#chi^{2} / ndof = " << std::setprecision(2) << chi2 / 26.0f;
    pullHist.addText(stream2.str());

    pullHist.setTitle(title + ";#tau (ps);Propertime ratio");
    //pullHist.setTitle(title + ";#tau (ps);t(data)/t(mc) ratio");
    pullHist.draw((output + ".pdf").c_str());
    pullHist.draw((output + ".png").c_str());
    delete canvas;
}

void FFFitter::drawSimultaneous(std::string title, std::string output) {
    TCanvas* canvas = new TCanvas();
    if(hobserved == nullptr) {
        hobserved = new TH1F(*h1);
    }
    if(hexpected == nullptr) {
        hexpected = new TH1F(*h1);
    }
    for(int k=0; k < nbins; ++k) {
        vobserved[k] = vobserved2[k];
        verror[k] = verror2[k];
        vexpected[k] = vexpected2[k];

        hobserved->SetBinContent(k+1, vobserved[k] / hobserved->GetBinWidth(k+1) );
        hobserved->SetBinError(k+1, verror[k] / hobserved->GetBinWidth(k+1) );
        hexpected->SetBinContent(k+1, vexpected[k] / hobserved->GetBinWidth(k+1) );
        hexpected->SetBinError(k+1, 0);
    }
    hobserved->Print();
    hexpected->Print();
    FFPullHist pullHist;
    pullHist.addHistogram(hobserved, kBlack, "ep", "Data", true);
    pullHist.addHistogram(hexpected, kRed, "same", "Fit", true);
    pullHist.setMinimum(0.0);
    pullHist.setMaximum(2200 * 6);

    std::ostringstream stream1;
    if(FFConfig::get<bool>("Blind")) {
        stream1 << "A^{#Delta}_{blind} = " << std::setprecision(2) << blindEngine.hide(param[2]) << " #pm ";
    } else {
        stream1 << "A^{#Delta} = " << std::setprecision(2) << param[2] << " #pm ";
    }
    stream1 << std::setprecision(2) << parerr[2];
    pullHist.addText(stream1.str());

    std::ostringstream stream2;
    stream2 << "#chi^{2} / ndof = " << std::setprecision(2) << chi2 / 50.0f;
    pullHist.addText(stream2.str());

    pullHist.setTitle(title + ";#tau (ps);");
    //pullHist.setTitle(title + ";#tau (ps);t(data)/t(mc) ratio");
    pullHist.draw((output + ".pdf").c_str());
    pullHist.draw((output + ".png").c_str());
    delete canvas;
}

void FFFitter::directChi2(int& par, double* gin, double& f, double* u, int flag)
{
    Nparam = 2;
    //Nparam = 5;

    double nr = u[0];

    if(npar1 != 0) {
        for(int i=0; i < Nparam; ++i) {
            f1->SetParameter(i, u[i+1]);
        }
        norm1 = f1->Integral(xmin_norm, xmax_norm);
    }

    vexpected.clear();
    vexpected.reserve(nbins);
    for(int k=0; k < nbins; ++k) {
        double a = binwalls[k];
        double b = binwalls[k+1];
        if(npar1 != 0) {
            integrals1[k] = f1->Integral(a, b) / norm1;
        }
        double intr = integrals1[k];
        vexpected[k] = nr * intr;
    }

    chi2 = 0;
    for(int i=0; i < nbins; ++i) {
        double delta = (vobserved[i] - vexpected[i]) / verror[i];
        chi2 += delta*delta;
    }
    f = chi2;

    return;
}

void FFFitter::ratioChi2(int &npar, double *gin, double &f, double *u, int flag) {
    double nr = u[0];

    if(npar1 != 0) {
        f1->SetParameter(0, u[1]);
        norm1 = f1->Integral(xmin_norm, xmax_norm);
    }
    if(npar2 != 0) {
        norm2 = f2->Integral(xmin_norm, xmax_norm);
    }

    vexpected.clear();
    vexpected.reserve(nbins);
    for(int k=0; k < nbins; ++k) {
        double a = binwalls[k];
        double b = binwalls[k+1];
        if(npar1 != 0) {
            integrals1[k] = f1->Integral(a, b) / norm1;
        }
        if(npar2 != 0) {
            integrals2[k] = f2->Integral(a, b) / norm2;
        }
        double intr = integrals1[k] / integrals2[k];
        vexpected[k] = nr * intr;
    }

    chi2 = 0;
    for(int i=0; i < nbins; ++i) {
        double delta = (vobserved[i] - vexpected[i]) / verror[i];
        chi2 += delta*delta;
    }
    f = chi2;

    return;
}

void FFFitter::ratioLikelihood(int &npar, double *gin, double &f, double *u, int flag) {
    double nr = u[0];

    if(npar1 != 0) {
        f1->SetParameter(0, u[1]);
        norm1 = f1->Integral(xmin_norm, xmax_norm);
    }
    if(npar2 != 0) {
        norm2 = f2->Integral(xmin_norm, xmax_norm);
    }

    vexpected.clear();
    vexpected.reserve(nbins);
    for(int k=0; k < nbins; ++k) {
        double a = binwalls[k];
        double b = binwalls[k+1];
        if(npar1 != 0) {
            integrals1[k] = f1->Integral(a, b) / norm1;
        }
        if(npar2 != 0) {
            integrals2[k] = f2->Integral(a, b) / norm2;
        }
        double intr = integrals1[k] / integrals2[k];
        vexpected[k] = nr * intr;
    }

    // safety measure against negative logs
    static const double epsilon = 1e-300;
    loglike = 0;
    for(int i=0; i < nbins; ++i) {
        double logtmp;
        // protections against negative argument to the log
        if (vexpected[i] <= epsilon)
            logtmp = 0.0;
        else
            logtmp = std::log(vobserved[i]/vexpected[i]);
        loglike += (vexpected[i] - vobserved[i] * (1.0 - logtmp));
    }
    f = 2 * loglike;

    return;
}

void FFFitter::simultaneousChi2(int &npar, double *gin, double &f, double *u, int flag) {
    double n1 = u[1];
    double n2 = u[0];

    if(npar1 != 0) {
        f1->SetParameter(0, u[2]);
        f1->SetParameter(1, u[3]);
        f1->SetParameter(2, u[4]);
        f1->SetParameter(3, u[5]);
        f1->SetParameter(4, u[6]);
        norm1 = f1->Integral(xmin_norm, xmax_norm);
    }
    if(npar2 != 0) {
        f2->SetParameter(0, u[3]);
        f2->SetParameter(1, u[4]);
        f2->SetParameter(2, u[5]);
        f2->SetParameter(3, u[6]);
        norm2 = f2->Integral(xmin_norm, xmax_norm);
    }

    vexpected.clear();
    vexpected2.clear();
    vexpected.reserve(nbins);
    vexpected2.reserve(nbins);
    for(int k=0; k < nbins; ++k) {
        double a = binwalls[k];
        double b = binwalls[k+1];
        if(npar1 != 0) {
            integrals1[k] = f1->Integral(a, b) / norm1;
        }
        if(npar2 != 0) {
            integrals2[k] = f2->Integral(a, b) / norm2;
        }
        vexpected[k] = n1 * integrals1[k];
        vexpected2[k] = n2 * integrals2[k];
    }

    chi2 = 0;
    for(int i=0; i < nbins; ++i) {
        double delta = (vobserved[i] - vexpected[i]) / verror[i];
        chi2 += delta*delta;
        delta = (vobserved2[i] - vexpected2[i]) / verror2[i];
        chi2 += delta*delta;
    }
    f = chi2;

    return;
}

TH1F* FFFitter::h1 = nullptr;
TH1F* FFFitter::h2 = nullptr;
TH1F* FFFitter::hobserved = nullptr;
TH1F* FFFitter::hexpected = nullptr;
TF1* FFFitter::f1 = nullptr;
TF1* FFFitter::f2 = nullptr;
int FFFitter::npar1 = 0;
int FFFitter::npar2 = 0;
int FFFitter::Nparam = 0.0f;
double FFFitter::norm1 = 0.0f;
double FFFitter::norm2 = 0.0f;
double FFFitter::xmin_norm = 0.0f;
double FFFitter::xmax_norm = 0.0f;
int FFFitter::nbins = 0;
std::vector<double> FFFitter::binwalls;
std::vector<double> FFFitter::integrals1;
std::vector<double> FFFitter::integrals2;
std::vector<double> FFFitter::vobserved;
std::vector<double> FFFitter::vobserved2;
std::vector<double> FFFitter::verror;
std::vector<double> FFFitter::verror2;
std::vector<double> FFFitter::vexpected;
std::vector<double> FFFitter::vexpected2;
double* FFFitter::param = nullptr;
double* FFFitter::parerr = nullptr;
double FFFitter::chi2 = 0.0f;
double FFFitter::loglike = 0.0f;
FFBlind FFFitter::blindEngine("bs2phigammarocks", 2);
