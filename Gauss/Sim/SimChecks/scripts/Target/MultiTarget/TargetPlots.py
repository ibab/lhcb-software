from string import find
from ROOT import *
from TargetSummary import *
from array import array

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
	
		pdgenergies  = [ 1, 5, 10, 50, 100 ]
		pdgRatios_p  = [ 1.67, 1.42, 1.31, 1.14, 1.10 ]
		pdgRatios_pi = [ 1.22, 1.13, 1.10, 1.05, 1.03 ]
		pdgRatios_K  = [ 1.61, 1.32, 1.23, 1.10, 1.07 ]

		#ratiotxt = open(outputPath+"/ratio_inAl.txt")
		#ratiotxt.write(  "\\begin{tabular}{| c | c | c |} \n" )
		#ratiotxt.write( "$p$ (GeV) & LHEP & BERT \\\\ \\hline \n" )

		PDGRatio = []
		mgRatio = TMultiGraph("mgrRatio","")

		leg2 = TLegend(0.60,0.79,0.99,0.99)

		for pg in range(0, len(pguns)-1, 2) :
		
			#ratiotxt.write( "\\multicolumn{2}{c}{ratio " + pguns[pg+1].GetLatex("$") + "/"  << pguns[pg].GetLatex("$") + "} \\\\ \\hline \n" )

			for m in models :
			
				#ratiotxt.write( "\\multicolumn{2}{c}{ratio " + models[m] + "} \\\\ \\hline" )

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
				gr1 = TGraphErrors(entries,dataTree.GetV1(),dataTree.GetV2(),errx,dataTree.GetV3())

				varexp = "h_"+str(pg)+m
				select = "model == " + str(ord(m[0])) + " && material == " + str(ord(materials[0][0])) + " && pGun == " + str(dict._all_pguns[pguns[pg]].GetPDG())
				if(xvar=="energy") : 
						select += " && thickness == " + str(Dx)
				elif(xvar=="thickness") :
						select += " && energy == " + str(E0)

				dataTree.SetEntryList(0)
				dataTree.Draw(">>"+varexp,select, "entrylist")
				list2 = gDirectory.Get(varexp)
				dataTree.SetEntryList(list2)
				dataTree.Draw(xvar+":"+var+":"+var+"_err")

				gr2 = TGraphErrors(entries,dataTree.GetV1(),dataTree.GetV2(),errx,dataTree.GetV3())
				x = dataTree.GetV1()

				for ee in range(0,gr1.GetN()) : 
				
					#ratiotxt << energies[ee]

					x1 = 0
					x2 = 0
					y1 = 0
					y2 = 0
					e1 = 0
					e2 = 0

					gr1.GetPoint(ee, x1,y1)
					gr2.GetPoint(ee, x2,y2)
					e1 = gr1.GetErrorY(ee)
					e2 = gr2.GetErrorY(ee)

					if(find(finalPlot,"RATIO")>0) :
						y[ee] = y2 / y1
					else :
						y[ee] = 100*TMath.Abs(y1 - y2)/(2. - y1 - y2)

					totErr2 = TMath.Power(e1/y1,2) + TMath.Power(e2/y2,2)
					erry[ee] = y[ee] * TMath.Sqrt(totErr2)

					#ratiotxt << fixed << setprecision(2) << " & $" << y[ee]  << " \\pm " << erry[ee] << "\\\\" << endl


				gr = TGraphErrors(entries,x,y,errx,erry)

				gr.SetMarkerColor(1+pg/2)
				gr.SetMarkerStyle(20+m)

				label = dict._all_pguns[pg+1].GetLatex("LEG") + " / " + dict._all_pguns[pg].GetLatex("LEG")
				if(len(models) > 1) :
					label += " (" + m + ")"
				leg2.AddEntry(gr,label,"P")

				if(plotData) :
				
					if(pguns[pg].GetName()=="p") :
						grPDG = TGraphErrors(5,pdgenergies,pdgRatios_p)
					elif(pguns[pg].GetName()=="Piplus") :
						grPDG = TGraphErrors(5,pdgenergies,pdgRatios_pi)
					elif(pguns[pg].GetName()=="Kplus") :
						grPDG = TGraphErrors(5,pdgenergies,pdgRatios_K)

					grPDG.SetMarkerColor(4)
					grPDG.SetMarkerStyle(34)
					grPDG.SetMarkerSize(1.2)
					if(m==0) :
						PDGRatio.push_back(grPDG)
						leg2.AddEntry(grPDG,"PDG ratio","P")
				
				mgrRatio.Add(gr)
			
			#ratiotxt << "\\hline" << endl
		
		#ratiotxt << "\\end{tabular}" << endl


		if(plotData and finalPlot == "RATIO_TOTAL") :
			mgrRatio.Add(PDGRatio[0])

		cratio = TCanvas()
		cratio.SetLogx()
		cratio.SetGrid()

		mgrRatio.SetTitle(finalPlot)
		mgrRatio.Draw("APL")
		if(finalPlot.find("RATIO") > 0) :
			mgrRatio.GetYaxis().SetTitle("Ratio")
		else :
			mgrRatio.GetYaxis().SetTitle("Asym (%)")
		
		if(xvar=="energy") :
			mgrRatio.GetXaxis().SetTitle("|p| (GeV)")
			mgrRatio.GetXaxis().SetLimits(0.5,300)
		elif(xvar=="thickness") :
			mgrRatio.GetXaxis().SetTitle("#Delta x (mm)")

		leg2.Draw("same")

		cratio.Print(outputPath + "/" + finalPlot + ".pdf")
		
	
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

		PintOverSigmaFactor = Dx /(1000. * dict._all_materials["Al"].GetSigmaDxOverPintFactor() * 1000.)	

		if(plotData) :
	
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

			COMPASTot_p_x               = array ( 'd' , [ 1.52, 1.8, 19.3, 20. ] ) 
			COMPASTot_p_sigmaErr        = array ( 'd' , [ 22., 27., 10., 10. ] )
			COMPASTot_p_sigma           = array ( 'd' , [ 687., 694., 687., 687. ] )
			COMPASTot_p_y               = array ( 'd' , ScalarProd(4,COMPASTot_p_sigma,PintOverSigmaFactor) )
			COMPASTot_p_yErr            = array ( 'd' , ScalarProd(4,COMPASTot_p_sigmaErr,PintOverSigmaFactor) )
			COMPASTot_pbar_x            = array ( 'd' , [ 1.45,1.8 ] )
			COMPASTot_pbar_sigma        = array ( 'd' , [ 1034., 1066. ] )
			COMPASTot_pbar_sigmaErr     = array ( 'd' , [ 40., 40. ] )
			COMPASTot_pbar_y            = array ( 'd' , ScalarProd(2,COMPASTot_pbar_sigma,PintOverSigmaFactor) )
			COMPASTot_pbar_yErr         = array ( 'd' , ScalarProd(2,COMPASTot_pbar_sigmaErr,PintOverSigmaFactor) )

			COMPAS_p_gr = TGraphErrors(6,COMPAS_p_x,COMPAS_p_x)#,array ( 'd' , [0.]*6 ),COMPAS_p_yErr)
			COMPAS_pbar_gr = TGraphErrors(6,COMPAS_pbar_x,COMPAS_pbar_y)#,array ( 'd' , [0.]*6 ),COMPAS_pbar_yErr)
			COMPAS_p_gr.SetMarkerColor(4)
			COMPAS_p_gr.SetMarkerStyle(29)
			COMPAS_p_gr.SetMarkerSize(1.1)
			COMPAS_pbar_gr.SetMarkerColor(4)
			COMPAS_pbar_gr.SetMarkerStyle(30)
			COMPAS_pbar_gr.SetMarkerSize(1.2)

			COMPASTot_p_gr = TGraphErrors(4,COMPASTot_p_x,COMPASTot_p_y,array ( 'd' , [0.]*4 ),COMPASTot_p_yErr)
			COMPASTot_pbar_gr = TGraphErrors(2,COMPASTot_pbar_x,COMPASTot_pbar_y,array ( 'd' , [0.]*2 ),COMPASTot_pbar_yErr)
			COMPASTot_p_gr.SetMarkerColor(4)
			COMPASTot_p_gr.SetMarkerStyle(29)
			COMPASTot_p_gr.SetMarkerSize(1.1)
			COMPASTot_pbar_gr.SetMarkerColor(4)
			COMPASTot_pbar_gr.SetMarkerStyle(30)
			COMPASTot_pbar_gr.SetMarkerSize(1.2)

			COMPAS_inBe_p_gr = TGraphErrors(6,COMPAS_inBe_p_x,COMPAS_inBe_p_y,0,COMPAS_inBe_p_yErr)
			COMPAS_inBe_pbar_gr = TGraphErrors(5,COMPAS_inBe_pbar_x,COMPAS_inBe_pbar_y,0,COMPAS_inBe_pbar_yErr)
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

	models = ["QGSP_BERT","FTFP_BERT"]
	thicks = [1]
	materials = ["Al"]
	energies = [1,5,10,100]
	pguns = ["p","pbar"]
	path = "TargetOutput"

	file = TFile(path+"/TargetsPlots.root")
	dataTree = file.Get("summaryTree")
	
	Plot( dataTree, "thickness", "RATIO_TOTAL", path, models , pguns , materials , 2 , 1 )
