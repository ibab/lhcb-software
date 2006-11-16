// ExFitDblGaus.C
//
// example for the double gaussian fitter in FitDblGaus.C
//
// author: Manuel Schiller
// date: 2006-11-16
//
// to see the example, use the following at the ROOT prompt:
//	.L FitDblGaus.C
//	.X ExFitDblGaus.C

void ExFitDblGaus()
{
	// ok, overlay two normalized gaussians:
	// - one centered at 0, sigma = 0.5
	// - one centered at 0.25, sigma = 2.0
	// scale the second one by a factor of 3.0 before adding up
	TF1 *f1 = new TF1("f1", "exp(-x**2/0.5)/sqrt(2*pi*0.25) + "
			  "3*exp(-(x-0.25)**2/8.)/sqrt(2*pi*4.)", -5., 5.);
	// make a histogram
	TH1D *h1 = new TH1D("h1", "h1", 100, -5., 5.);
	// fill it according to our probability distribution
	h1->FillRandom("f1");
	// fit the double gaussian...
	FitDblGaus(*h1);
}
