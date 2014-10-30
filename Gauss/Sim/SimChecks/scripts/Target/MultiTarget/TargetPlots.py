from string import find
from ROOT import *
from TargetSummary import *
from array import array
from copy import *

def ScalarProd(ncomp, v, c) :

	res = []
	for i in v :
		res.append(i*c)

	return res


def Plot( dataTree, xvar, finalPlot, outputPath, models = [], pguns = [], materials = [], E0 = -1 , Dx = -1, plotData = False) :
	
	dict = Plotter()
	
	leg = TLegend(0.7,0.15,0.99,0.85)

	var = ""
	if(find(finalPlot,"MULTI") > -1) :
		var = "multi"
	elif(find(finalPlot,"TOTAL") > -1) :
		var = "xsec"
	elif(find(finalPlot,"INEL")> -1) :
		var = "inel_xsec"
	elif(find(finalPlot,"EL")> -1) :
		var = "el_xsec"
	elif(find(finalPlot,"MULTI_NCH")> -1) :
		var = "multiNCh"
	elif(find(finalPlot,"MULTI_NCH_NOGAMMA")> -1) :
		var = "multiNCh_nogamma"
	elif(find(finalPlot,"PERC_NCH")> -1) :
		var = "percNCh"
	elif(find(finalPlot,"PERC_MINUS")> -1) :
		var = "percMinus"
	elif(find(finalPlot,"PERC_PLUS")> -1) :
		var = "percPlus"
	elif(find(finalPlot,"MULTI_GAMMA")> -1) :
		var = "multi_gamma"

	if(find(finalPlot,"RATIO") > -1 or find(finalPlot,"ASYM") > -1) :

		mystr = " in " +str(Dx) + " mm"
		if(xvar=="thickness") :
			mystr = " for " + str(E0) + " GeV"

		titleMultigr = finalPlot
		if(finalPlot == "RATIO_TOTAL") :
			titleMultigr = "Ratio of total (inel + el) cross sections" + mystr
		if(finalPlot == "ASYM_TOTAL") :
			titleMultigr = "Asymmetry of total (inel + el) cross sections" + mystr
		if(finalPlot == "RATIO_INELL") :
			titleMultigr = "Ratio of inelastic cross sections" + mystr
		if(finalPlot == "ASYM_INEL") :
			titleMultigr = "Asymmetry of inelastic  cross sections" + mystr



		pdgenergies  = [ 1, 5, 10, 50, 100 ]
		pdgRatios_p  = [ 1.67, 1.42, 1.31, 1.14, 1.10 ]
		pdgRatios_pi = [ 1.22, 1.13, 1.10, 1.05, 1.03 ]
		pdgRatios_K  = [ 1.61, 1.32, 1.23, 1.10, 1.07 ]

		if(find(finalPlot,"RATIO") > -1) :
			ratiotxt = open(outputPath+"/ratio_inAl.txt","w")
		else :
			ratiotxt = open(outputPath+"/asym_inAl.txt","w")

		#ratiotxt.write(  "\\begin{tabular}{| c | c | c |} \n" )
		#ratiotxt.write( "$p$ (GeV) & LHEP & BERT \\\\ \\hline \n" )

		PDGRatio = []
		mgRatio = TMultiGraph("mgrRatio",titleMultigr)

		leg2 = TLegend(0.60,0.79,0.99,0.99)

		for pg in range(0, len(pguns)-1, 2) :
		
			ratiotxt.write( "\\multicolumn{2}{c}{ratio " + str(dict._all_pguns[pguns[pg+1]].GetLatex("$")) + "/" + str(dict._all_pguns[pguns[pg]].GetLatex("$")) + "} \\\\ \\hline \n" )
			nm = 0
			for m in models :
			
				ratiotxt.write( "\\multicolumn{2}{c}{ratio " + m + "} \\\\ \\hline" )

				varexp = "h_"+str(pg)+m
				select = "model == " + str(ord(m[0])) + " && material == " + str(ord(materials[0][0])) + " && pGun == " + str(dict._all_pguns[pguns[pg]].GetPDG())
				if(xvar=="energy") : 
						select += " && thickness == " + str(Dx)
				elif(xvar=="thickness") :
						select += " && energy == " + str(E0)

				dataTree.SetEntryList(0)
				dataTree.Draw(">>"+varexp,select, "entrylist")
				list = gDirectory.Get(varexp)
				entries = list.GetN()
				dataTree.SetEntryList(list)
				dataTree.Draw(xvar+":"+var+":"+var+"_err")

				errx = array( 'd' , [0.] * entries )
				ty1 = dataTree.GetV2()
				terry1 = dataTree.GetV3()
				y1=[]
				erry1=[]
				for i in range(0,entries) :
					y1.append(ty1[i])
					erry1.append(terry1[i])
									
				varexp = "h_"+str(pg+1)+m
				select = "model == " + str(ord(m[0])) + " && material == " + str(ord(materials[0][0])) + " && pGun == " + str(dict._all_pguns[pguns[pg+1]].GetPDG())
				if(xvar=="energy") : 
						select += " && thickness == " + str(Dx)
				elif(xvar=="thickness") :
						select += " && energy == " + str(E0)

				dataTree.SetEntryList(0)
				dataTree.Draw(">>"+varexp,select, "entrylist")
				list2 = gDirectory.Get(varexp)
				dataTree.SetEntryList(list2)
				dataTree.Draw(xvar+":"+var+":"+var+"_err")

				tx = dataTree.GetV1() 
				ty2 = dataTree.GetV2() 
				terry2 = dataTree.GetV3() 
				x = []
				y2=[]
				erry2=[]
				for i in range(0,entries) :
					y2.append(ty2[i])
					erry2.append(terry2[i])
					x.append(tx[i])

				y = []
				erry = []
				for ee in range(0,entries) : 

					ratiotxt.write( str(x[ee]) )
					if(find(finalPlot,"RATIO")>-1) :
						y.append(y2[ee] / y1[ee])
					else :
						y.append(100*TMath.Abs(y1[ee] - y2[ee])/(2. - y1[ee] - y2[ee]))

					totErr2 = TMath.Power(erry1[ee]/y1[ee],2) + TMath.Power(erry2[ee]/y2[ee],2)
					erry.append(y[ee] * TMath.Sqrt(totErr2))

					ratiotxt.write(' & $ {:4.2} \\pm {:4.2} $ \\\\ \n'.format(y[ee],erry[ee]) )

				gr = TGraphErrors(entries,array('d',x),array('d',y),array('d',errx),array('d',erry))
				gr.SetMarkerColor(1+pg/2)
				gr.SetMarkerStyle(20+nm)

				label = dict._all_pguns[pguns[pg+1]].GetLatex("LEG") + " / " + dict._all_pguns[pguns[pg]].GetLatex("LEG")
				if(len(models) > 1) :
					label += " (" + m + ")"
				leg2.AddEntry(gr,label,"P")

				if(plotData and (find(finalPlot,"TOTAL")>-1 or find(finalPlot,"INEL")>-1) and find(finalPlot,"RATIO")>-1 ) :
				
					if(dict._all_pguns[pguns[pg]].GetName()=="p") :
						grPDG = TGraphErrors(5,array('d',pdgenergies),array('d',pdgRatios_p))
					elif(dict._all_pguns[pguns[pg]].GetName()=="Piplus") :
						grPDG = TGraphErrors(5,array('d',pdgenergies),array('d',pdgRatios_pi))
					elif(dict._all_pguns[pguns[pg]].GetName()=="Kplus") :
						grPDG = TGraphErrors(5,array('d',pdgenergies),array('d',pdgRatios_K))

					grPDG.SetMarkerColor(4)
					grPDG.SetMarkerStyle(34)
					grPDG.SetMarkerSize(1.2)
					if(nm==(len(models)-1)) :
						PDGRatio.append(grPDG)
						leg2.AddEntry(grPDG,"PDG ratio","P")
				
				mgRatio.Add(gr)
			
			#ratiotxt << "\\hline" << endl
		
		#ratiotxt << "\\end{tabular}" << endl


		if(plotData and len(PDGRatio)>0) :
			mgRatio.Add(PDGRatio[0])

		cratio = TCanvas()
		cratio.SetLogx()
		cratio.SetGrid()

		mgRatio.Draw("APL")
		if(finalPlot.find("RATIO") > 0) :
			mgRatio.GetYaxis().SetTitle("Ratio")
		else :
			mgRatio.GetYaxis().SetTitle("Asym (%)")
		
		if(xvar=="energy") :
			mgRatio.GetXaxis().SetTitle("|p| (GeV)")
			mgRatio.GetXaxis().SetLimits(0.5,300)
		elif(xvar=="thickness") :
			mgRatio.GetXaxis().SetTitle("#Delta x (mm)")
	
		mgRatio.Draw("APL")

		leg2.Draw("same")

		cratio.Print(outputPath + "/" + finalPlot + mystr.replace(" ","_") +".pdf")	
	
	else :
	
		c1 = TCanvas()

		mystr = " in " +str(Dx) + " mm"
		if(xvar=="thickness") :
			mystr = " for " + str(E0) + " GeV"

		nameMultigr = "mgr" + mystr
		titleMultigr = finalPlot
		if(finalPlot == "TOTAL") :
			titleMultigr = "Total (inel + el) cross sections" + mystr
		elif(finalPlot == "INEL") :
			titleMultigr = "Inelastic cross sections" + mystr
		elif(finalPlot == "EL") :
			titleMultigr = "Elastic cross sections" + mystr 
		elif(finalPlot == "PERC_PLUS") :
			titleMultigr = "Percentage of positive particles"  + mystr
		elif(finalPlot == "PERC_MINUS") :
			titleMultigr = "Percentage of negative particles"  + mystr
		elif(finalPlot == "MULTI_NCH") :
			titleMultigr = "Multiplicity (neutral)"  + mystr
		elif(finalPlot == "MULTI_NCH_NOGAMMA") :
			titleMultigr = "Multiplicity (neutral minus gamma)"  + mystr
		elif(finalPlot == "PERC_NCH") :
			titleMultigr = "Percentage of neutral particles"  + mystr
		elif(finalPlot == "MULTI_NOGAMMA") :
			titleMultigr = "Gamma multiplicity" + mystr

		mgr = TMultiGraph(nameMultigr,titleMultigr)

		PintOverSigmaFactor = Dx /(1000. * dict._all_materials[materials[0]].GetSigmaDxOverPintFactor() * 1000.)	

		if(plotData and (materials[0] == "Al" or materials[0] == "Be") and (find(finalPlot,"TOTAL")>-1 or find(finalPlot,"INEL")>-1) ) :
	
			#COMPAS Inelastic Xsec data in Al
			COMPAS_p_x           = [ 1.52, 5., 9., 20., 30., 60. ]
			COMPAS_p_sigmaErr    = [ 10., 4., 4., 5., 5., 7. ]
			COMPAS_p_sigma       = [ 445., 445., 465., 446., 445., 455. ]
			COMPAS_p_y           = ScalarProd(6,COMPAS_p_sigma,PintOverSigmaFactor)
			COMPAS_p_yErr        = ScalarProd(6,COMPAS_p_sigmaErr,PintOverSigmaFactor)
			COMPAS_pbar_x        = [ 1.45,6.65,13.3,25.,30.,60. ]
			COMPAS_pbar_sigma    = [ 617., 558., 536., 480., 457., 439. ]
			COMPAS_pbar_sigmaErr = [ 17., 10., 10., 9., 11., 13. ]
			COMPAS_pbar_y        = ScalarProd(6,COMPAS_pbar_sigma,PintOverSigmaFactor)
			COMPAS_pbar_yErr     = ScalarProd(6,COMPAS_pbar_sigmaErr,PintOverSigmaFactor)

			#COMPAS Inelastic Xsec data in Be

			COMPAS_inBe_p_x           = [ 3., 5., 9., 30.,50.,60. ]
			COMPAS_inBe_p_sigmaErr    = [ 4., 3., 3., 3., 3., 2. ]
			COMPAS_inBe_p_sigma       = [ 236., 207., 210., 210., 210., 216. ]
			COMPAS_inBe_p_y           = ScalarProd(6,COMPAS_inBe_p_sigma,PintOverSigmaFactor)
			COMPAS_inBe_p_yErr        = ScalarProd(6,COMPAS_inBe_p_sigmaErr,PintOverSigmaFactor)
			COMPAS_inBe_pbar_x        = [ 6.65,13.3,20.,30.,40. ]
			COMPAS_inBe_pbar_sigma    = [ 296., 275., 240., 235., 226., 190. ]
			COMPAS_inBe_pbar_sigmaErr = [ 6., 4., 10., 6., 7. ]
			COMPAS_inBe_pbar_y        = ScalarProd(5,COMPAS_inBe_pbar_sigma,PintOverSigmaFactor)
			COMPAS_inBe_pbar_yErr     = ScalarProd(5,COMPAS_inBe_pbar_sigmaErr,PintOverSigmaFactor)

			#COMPAS Total Xsec data in Al

			COMPASTot_p_x               = [ 1.52, 1.8, 19.3, 20. ]  
			COMPASTot_p_sigmaErr        = [ 22., 27., 10., 10. ] 
			COMPASTot_p_sigma           = [ 687., 694., 687., 687. ] 
			COMPASTot_p_y               = ScalarProd(4,COMPASTot_p_sigma,PintOverSigmaFactor) 
			COMPASTot_p_yErr            = ScalarProd(4,COMPASTot_p_sigmaErr,PintOverSigmaFactor) 
			COMPASTot_pbar_x            = [ 1.45,1.8 ] 
			COMPASTot_pbar_sigma        = [ 1034., 1066. ] 
			COMPASTot_pbar_sigmaErr     = [ 40., 40. ] 
			COMPASTot_pbar_y            = ScalarProd(2,COMPASTot_pbar_sigma,PintOverSigmaFactor) 
			COMPASTot_pbar_yErr         = ScalarProd(2,COMPASTot_pbar_sigmaErr,PintOverSigmaFactor) 

			COMPAS_p_gr = TGraphErrors(6,array('d', COMPAS_p_x ), array('d', COMPAS_p_y ), array('d', [0.]*6 ), array('d', COMPAS_p_yErr))
			COMPAS_pbar_gr = TGraphErrors(6, array('d', COMPAS_pbar_x), array('d', COMPAS_pbar_y), array ('d', [0.]*6 ), array('d', COMPAS_pbar_yErr) )
			COMPAS_p_gr.SetMarkerColor(4)
			COMPAS_p_gr.SetMarkerStyle(29)
			COMPAS_p_gr.SetMarkerSize(1.1)
			COMPAS_pbar_gr.SetMarkerColor(4)
			COMPAS_pbar_gr.SetMarkerStyle(30)
			COMPAS_pbar_gr.SetMarkerSize(1.2)

			COMPASTot_p_gr = TGraphErrors(4, array('d', COMPASTot_p_x), array('d', COMPASTot_p_y), array('d', [0.]*4 ), array('d', COMPASTot_p_yErr))
			COMPASTot_pbar_gr = TGraphErrors(2, array('d', COMPASTot_pbar_x), array('d', COMPASTot_pbar_y), array('d', [0.]*2 ), array('d', COMPASTot_pbar_yErr))
			COMPASTot_p_gr.SetMarkerColor(4)
			COMPASTot_p_gr.SetMarkerStyle(29)
			COMPASTot_p_gr.SetMarkerSize(1.1)
			COMPASTot_pbar_gr.SetMarkerColor(4)
			COMPASTot_pbar_gr.SetMarkerStyle(30)
			COMPASTot_pbar_gr.SetMarkerSize(1.2)

			COMPAS_inBe_p_gr = TGraphErrors(6, array('d', COMPAS_inBe_p_x), array('d', COMPAS_inBe_p_y), array('d', [0.]*6 ), array('d', COMPAS_inBe_p_yErr))
			COMPAS_inBe_pbar_gr = TGraphErrors(5, array('d', COMPAS_inBe_pbar_x), array('d', COMPAS_inBe_pbar_y), array('d', [0.]*5 ), array('d', COMPAS_inBe_pbar_yErr))
			COMPAS_inBe_p_gr.SetMarkerColor(4)
			COMPAS_inBe_p_gr.SetMarkerStyle(29)
			COMPAS_inBe_p_gr.SetMarkerSize(1.1)
			COMPAS_inBe_pbar_gr.SetMarkerColor(4)
			COMPAS_inBe_pbar_gr.SetMarkerStyle(30)
			COMPAS_inBe_pbar_gr.SetMarkerSize(1.2)

			if(finalPlot == "TOTAL") :
				mgr.Add(COMPASTot_p_gr)
				mgr.Add(COMPASTot_pbar_gr)
				leg.AddEntry(COMPASTot_p_gr,"COMPASTot p","P")
				leg.AddEntry(COMPASTot_pbar_gr,"COMPASTot #bar{p}","P")
			
			elif(finalPlot == "INEL") :
				mgr.Add(COMPAS_p_gr)
				mgr.Add(COMPAS_pbar_gr)
				leg.AddEntry(COMPAS_p_gr,"COMPAS p","P")
				leg.AddEntry(COMPAS_pbar_gr,"COMPAS #bar{p}","P")
			

		#Plotting Gauss values
		n0 = 0
		nh = 0
		for model in models : 
			for material in materials :

				n2 = 0

				for pg in pguns : 
				
					varexp = "h_"+str(nh)
					nh+=1
				
					select = "model == " + str(ord(model[0])) + " && material == " + str(ord(material[0])) + " && pGun == " + str(dict._all_pguns[pg].GetPDG())
					if(xvar=="energy") : 
						select += " && thickness == " + str(Dx)
					elif(xvar=="thickness") :
						select += " && energy == " + str(E0)

					dataTree.SetEntryList(0)
					dataTree.Draw(">>"+varexp,select, "entrylist")
					list = gDirectory.Get(varexp)
					entries = list.GetN()
					dataTree.SetEntryList(list)

					dataTree.SetEstimate(entries)
					if(finalPlot == "MULTI" or finalPlot == "TOTAL" or finalPlot == "INEL" or finalPlot == "EL") :
						dataTree.Draw(xvar+":"+var+":"+var+"_err","","colz")	
					else :
						dataTree.Draw("energy:"+var)

					errx = array( 'd' , [0.] * entries )	
					gr = TGraphErrors(entries,dataTree.GetV1(),dataTree.GetV2(),errx,dataTree.GetV3())

					gr.SetMarkerColor(1+n2)
					if(n0==0) :
						gr.SetMarkerStyle(20+n0)
					else :
						gr.SetMarkerStyle(24+n0)

					if(n0==0) :
						gr.SetMarkerSize(1.1)
					n2+=1

					label = dict._all_pguns[pg].GetLatex("LEG") + " in " + material
					if(len(models) > 1) :
						label += " (" + model + ")"
					leg.AddEntry(gr,label,"P")

					mgr.Add(gr)

			n0+=1

		c1.Clear()
		mgr.Draw("APL")

		if(finalPlot == "TOTAL") :
			mgr.GetYaxis().SetTitle("P^{tot}_{int} = N^{inel+el}/N^{gen}")
		elif(finalPlot == "INEL") :
			mgr.GetYaxis().SetTitle("P^{inel}_{int} = N^{inel}/N^{gen}")
		elif(finalPlot == "EL") :
			mgr.GetYaxis().SetTitle("P^{el}_{int} = N^{el}/N^{gen}")
		elif(find(finalPlot,"MULTI") > 0) :
			mgr.GetYaxis().SetTitle("< Multi >")
		elif(find(finalPlot,"PERC") > 0) :
			mgr.GetYaxis().SetTitle("%")

		if(xvar=="evergy") :
			mgr.GetXaxis().SetTitle("|p| (GeV)")
			mgr.GetXaxis().SetLimits(0.5,900)
		elif (xvar=="thickness") :
			mgr.GetXaxis().SetTitle("#Delta x (mm)")
			mgr.GetXaxis().SetLimits(0.,20)

		c1.SetGrid()
		if(xvar=="evergy") :
			c1.SetLogx()
		leg.Draw("same")

		printname = outputPath+"/" + finalPlot + mystr.replace(" ","_") + ".pdf"
		c1.Print(printname)




if __name__ == "__main__" :

	###### Possible types of plots are :
	##          INEL:                                   inelastic cross section
	##          EL:                                     elastic cross section
	##          TOTAL:                                  total cross section
	##          MULTI:                                  total multiplicity 
	##          MULTI_PLUS(MINUS / NCH / NCH_NOGAMMA):  multiplicity of positive (negative, neutral, neutral but no gammas) secondary particles produced
	##          PERC_PLUS(MINUS / NCH / NCH_NOGAMMA):   percentage of positive (negative, neutral, neutral but no gammas) secondary particles produced
	##
	##		For each of the plots above you can have them is form of a ratio of particles (the consecutive ones in the "pguns" array, see below)
	##      or as asymmetries adding RATIO or ASYM to the plot type. e.g. RATIO_TOTAL or RATIO_MULTI_NCH or ASYM_INEL, etc

	plots = [ "RATIO_TOTAL" ]
	
	### N.B.: Options need to have been generated with Targets_RunAll.py!
	### In your output directory a file options.txt has been created where the options you generated are listed.

	models = ["QGSP_BERT","FTFP_BERT"]  # any you generated, by default "QGSP_BERT","FTFP_BERT"
	thicks = [1] #1,5,10
	materials = ["Al"] ### Al,Si,Be
	energies = [1,5,10,100] # any you generated, by default 1,5,10,100
	pguns = ["p","pbar","Kplus","Kminus","Piplus","Piminus"] # "p","pbar","Kplus","Kminus","Piplus","Piminus"
	path = "TargetOutput"

	file = TFile(path+"/TargetsPlots.root")
	dataTree = file.Get("summaryTree")
	
	for p in plots :
		tp.Plot( dataTree, "energy", p, path, models , pguns , materials , 2 , 1, True )




