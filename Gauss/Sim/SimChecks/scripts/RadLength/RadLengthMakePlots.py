from ROOT import *

def makePlots(fileName = "Rad_merged.root", path = "plots/", type = "rad") :

	gROOT.Reset()
	f = TFile(fileName)
	tree = f.Get("RadLengthColl/tree")
	if( f!=0 and tree !=0 ) :
		print "Reading: ", fileName
	else :
		print "File or tree not found"

	nplanes = 11

	c = TCanvas()
		
	cumul = TGraphErrors()
	cumulZ = TGraphErrors()
	p2p = TGraphErrors()
	p = 0

	for i in range(2,12) :

		c.SetLogy()
		select = "ID == "+str(i)
		namehisto = "cum_" + type +"lgh_ID"+str(i)
		var = "cum"+type+"lgh>>"+namehisto
		tree.Draw(var,select)
		h1 = gPad.GetPrimitive(namehisto)
		h1.GetXaxis().SetTitle("n_{X0}^{tot}")
		h1.GetYaxis().SetTitle("N_{evt}")
		namefile = path + namehisto + ".pdf"
		c.Print(namefile)
		cumul.SetPoint(p,i,h1.GetMean())
		cumul.SetPointError(p,0,h1.GetRMS()/TMath.Sqrt(h1.GetEntries()))
		
		namehisto = "Z_" + type +"_ID"+str(i)
		var = "Zpos>>" + namehisto
		tree.Draw(var,select)
		hZ = gPad.GetPrimitive(namehisto)
		cumulZ.SetPoint(p,hZ.GetMean(),h1.GetMean())
		cumulZ.SetPointError(p,hZ.GetRMS()/TMath.Sqrt(hZ.GetEntries()),h1.GetRMS()/TMath.Sqrt(h1.GetEntries()))
			
		namehisto = "p2p_" + type + " lgh_ID"+str(i)
		var = "p2p"+type+"lgh>>" + namehisto
		tree.Draw(var,select)
		h2 = gPad.GetPrimitive(namehisto)
		h2.GetXaxis().SetTitle("n_{X0}^{p2p}")
		h2.GetYaxis().SetTitle("N_{evt}")
		p2p.SetPoint(p,i,h2.GetMean())
		p2p.SetPointError(p,0,h2.GetRMS()/TMath.Sqrt(h2.GetEntries()))
		namefile = path + namehisto + ".pdf"
		c.Print(namefile)
	
		c.SetLogy(0)
		gStyle.SetOptStat(0)
		namehisto = type + "lgh_prof_ID"+str(i)
		var = "cum"+type+"lgh:eta:phi>>" + namehisto + "(100,-3.3,3.3,100,2.,5.)"
		tree.Draw(var,select,"profs")
		hh = gPad.GetPrimitive(namehisto)
		hh.GetXaxis().SetTitle("#phi")
		hh.GetYaxis().SetTitle("#eta")
		if(type=="inter") :
			c.SetLogz()
			hh.SetMinimum(20)
		hh.Draw("colz")
		namefile = path + namehisto + ".pdf"
		c.Print(namefile)
	
		gStyle.SetOptStat(0)
		namehisto = "etaphi_ID"+str(i)
		var = "eta:phi>>" + namehisto# + "(80,-3.3,3.3,80,2.,5.)"
		tree.Draw(var,select,"colz")
		hhetaphi = gPad.GetPrimitive(namehisto)
		hhetaphi.GetXaxis().SetTitle("#phi")
		hhetaphi.GetYaxis().SetTitle("#eta")
		namefile = path + namehisto + ".pdf"
		c.Print(namefile)
	
		p+=1


	
	gStyle.SetOptStat(0)
	c.SetLogy()
				
	tree.Draw("cum"+type+"lgh:eta>>hh1(100,2,5)","ID == 11","prof")
	radlgh_eta = gPad.GetPrimitive("hh1")
		
	cumul.GetXaxis().SetTitle("ID plane")
	cumul.GetYaxis().SetTitle("<n_{X0}^{tot}>")
	cumulZ.GetXaxis().SetTitle("Z (mm)")
	cumulZ.GetYaxis().SetTitle("<n_{X0}^{tot}>")
	p2p.GetXaxis().SetTitle("ID plane")
	p2p.GetYaxis().SetTitle("<n_{X0}^{p2p}>")
	radlgh_eta.GetXaxis().SetTitle("#eta")
	radlgh_eta.GetYaxis().SetTitle("<n_{X0}^{tot}>")
	
	cumul.SetMarkerStyle(22)
	cumul.SetMarkerSize(0.8)
	cumul.SetMarkerColor(1)
	cumulZ.SetMarkerStyle(22)
	cumulZ.SetMarkerSize(0.8)
	cumulZ.SetMarkerColor(1)
	p2p.SetMarkerStyle(22)
	p2p.SetMarkerSize(0.8)
	p2p.SetMarkerColor(1)
	radlgh_eta.SetMarkerStyle(22)
	radlgh_eta.SetMarkerSize(0.8)
	radlgh_eta.SetMarkerColor(1)
	
	p2p.Draw("AP")
	c.Print(path+"p2p_" + type + "Length.pdf")
	cumul.Draw("AP")
	c.Print(path+"cum" + type + "Length.pdf")
	cumulZ.Draw("AP")
	c.Print(path+"cum" + type + "Length_vs_Z.pdf")
	radlgh_eta.Draw()
	c.Print(path+"cum" + type + "Length_vs_eta.pdf")





if __name__ == "__main__":
	
	import sys

	fileName = "Rad_merged.root"
	outpath = "plots/"
	type = "rad"

	if(len(sys.argv) > 1) :
		fileName = sys.argv[1]
	if(len(sys.argv) > 2) :
		type = sys.argv[2]
	if(len(sys.argv) > 3) :
		outpath = sys.argv[3]

	makePlots(fileName,outpath,type)
