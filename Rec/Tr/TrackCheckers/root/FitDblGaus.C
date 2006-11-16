// FitDblGaus -	fit a double gaussian, taking starting values from
// 		histogram
//
// author: Manuel Schiller
// date: 2006-11-16
//
// given a 1D histogram, the following macro attempts to fit a double
// gaussian to it, taking initial parameters from the histograms itself
// 
// it prints out the mean, width and tail fraction (see below) for the
// narrower of the two gaussians
// (the tail fraction is the fraction of histogram entries more than
// +/- 3 sigma away from the center of the narrower gaussian)
//
// don't forget to check if the result of the automatic fit is good
// enough for you - there is no guarantee that the starting parameters
// picked by this routine make the fit converge to the correct solution
// it just happens to work reasonably well for most of the cases I tried
//
// the macro is inteded to get a rough estimate for e.g. resolution in
// resolution plots while also giving some indication of the importance
// of the tails relative to the narrower peak
//
// the parameters are as follows:
// parameter 0		integral of the sum of the two gaussians
// 			(usually not shown because we fix it, see below)
// parameter 1		fraction the narrower gaussian contributes to this
// 			integral
// parameter 2		mean of narrower gaussian
// parameter 3		sigma of narrower gaussian
// parameter 4		mean of wider gaussian
// parameter 5		sigma of wider gaussian
//
// I imagine that it's useful for other purposes as well, that's why it's
// put here
//
// a small technical note: the number of entries in the histogram is known,
// therefore, if a double gaussian describes the data, the overall integral
// of the two gaussian is known. that's why the routine fixes this parameter
// and tries to parametrize the relative contributions of the two gaussians
// with the fraction each gaussian contributes to the total number of entries
// if that is not what you want, feel free to remove the line fixing
// parameter 0 below

TF1* FitDblGaus(TH1& h)
{
	TF1 *fun = new TF1("dblgaus", "[0]/sqrt(2.*pi)*([1]/[3]*exp(-.5*((x-[2])/[3])**2)+(1.-[1])/[5]*exp(-.5*((x-[4])/[5])**2))");
	fun->SetNpx(400);
	const double m_pi = 4. * atan(1.);
	int hbins = h.GetNbinsX();
	double hmax = h.GetMaximum(),
	       hint = h.Integral("width"),
	       hmean = h.GetMean(),
	       hrms = h.GetRMS();
	double xmin = h.GetBinLowEdge(1);
	double xmax = h.GetBinLowEdge(hbins + 1);
	fun->SetParameters(hint, 0.5, hmean, hrms / 10., hmean, hrms);
	fun->FixParameter(0, hint);
	fun->SetParLimits(1, 0., 1.);
	fun->SetParLimits(2, xmin, xmax);
	fun->SetParLimits(3, 5e-4 * hrms, .5 * hrms);
	fun->SetParLimits(4, xmin, xmax);
	fun->SetParLimits(5, 5e-2 * hrms, 5. * hrms);
	fun->SetLineColor(kRed);
	h.Fit(fun, "MB");
	
	TF1 *funbg = new TF1("bggaus", "gaus(0)", xmin, xmax);
	funbg->SetNpx(400);
	
	double mean = fun->GetParameter(2);
	double width = fabs(fun->GetParameter(3));
	funbg->SetParameters((1. - fun->GetParameter(1)) *
			fun->GetParameter(0) / sqrt(2. * m_pi) /
			fun->GetParameter(5),
			fun->GetParameter(4),
			fun->GetParameter(5));
	if (width > fabs(fun->GetParameter(5))) {
		mean = fun->GetParameter(4);
		width = fabs(fun->GetParameter(5));
		funbg->SetParameters((0. + fun->GetParameter(1)) *
			fun->GetParameter(0) / sqrt(2. * m_pi) /
			fun->GetParameter(3),
			fun->GetParameter(2),
			fun->GetParameter(3));
	}
	funbg->SetLineColor(kBlue);
	funbg->Draw("Same");

	double total = h.Integral();
	int lobin = h.FindBin(mean - 3.*width),
	    hibin = h.FindBin(mean + 3.*width);
	double outside = total - h.Integral(lobin, hibin);

	printf("mean %12.6g width %12.6g fraction %12.6g out of 3 sigma\n",
		mean, width, outside/total);

	//delete fun;
	//delete funbg;
	return 0;
}
