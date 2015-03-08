#include "FFBinning.h"

/**
    @file Implementation file of the class FFBinning
    @author Carlos Sánchez Mayordomo (casanche@cern.ch) (IFIC, Valencia)
    @author Pablo Ruiz Valls (pruizval@cern.ch) (IFIC, Valencia)
*/

std::vector<double> FFBinning::adaptive(TF1* f1, double param) {
    //Backup previous param
    double param_before = f1->GetParameter(0);
    f1->SetParameter(0, param);

	//Binning: generation
	//TRandom3 *random = new TRandom3(0);

    const int Nbins = 1E4;
    const int Nevts = 1E5;
    const double BinOcc = 1.0f/30.0f;

    const double xmin = FFConfig::get<double>("xmin");
    const double xmax = FFConfig::get<double>("xmax");
    TH1F* hbinning = new TH1F("hbinning", "hbinning", Nbins, xmin, xmax);

    for(int i = 0; i < Nevts; ++i) {
        hbinning->Fill( f1->GetRandom() );
    }

    std::vector<double> binwalls;
    binwalls.push_back( xmax );
    for(int k = Nbins; k > 0; --k) {
        int count = 0;
        while( count < (Nevts * BinOcc) && k > 0 ) {
            count += hbinning->GetBinContent(k);
            --k;
        }
        binwalls.insert( binwalls.begin(), hbinning->GetBinCenter(k) );
    }
    binwalls.erase( binwalls.begin()+1 );
    binwalls[0] = 0.0f;

    //Restore previous param
    f1->SetParameter(0, param_before);
    hbinning->Delete();

    //Print
	/*for(unsigned int k = 0; k < binwalls.size(); ++k) {
        std::cout << binwalls[k] << ", " << std::flush;
    }
    std::cout << std::endl;*/

	return binwalls;
}

std::vector<double> FFBinning::manual() {
    std::vector<double> binwalls;
    double manual[] = {0, 0.6478, 0.7422, 0.8324, 0.9153, 0.9991, 1.076, 1.153, 1.235, 1.312,
		1.397, 1.483, 1.578, 1.674, 1.774, 1.876, 1.987, 2.103, 2.233, 2.368, 2.521, 2.695, 2.892, 3.127, 3.404,
		3.713, 4.11, 4.701, 5.68, 10};
	for(unsigned int i=0; i < 30; ++i) {
		binwalls.push_back(manual[i]);
	}
    return binwalls;
}
