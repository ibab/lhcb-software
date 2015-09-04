##############################################################################
# Option file to run hadronic cross section checks with multiple targets.
# For moreusage informations: https://twiki.cern.ch/twiki/bin/view/LHCb/TargetStudy
# 
# Last modified: Luca Pescatore, 24/11/2014
##############################################################################




from string import find
from ROOT import *
from Target.TargetSummary import Plotter, doMultiHistos
from array import array
from copy import *
import os

vardef = { "TOTAL": "xsec", "INEL" : "inel_xsec", "EL" : "el_xsec",
		"MULTI_NCH" : "multiNCh", "MULTI_NCH_NOGAMMA" : "multiNCh_nogamma",
		"PERC_NCH" : "percNCh", "PERC_MINUS" : "percMinus", "PERC_PLUS" : "percPlus",
		"MULTI_GAMMA" : "multi_gamma", "MULTI" : "multi" }

colors = [1,2,4,6,8,9,38,12,18,41,5,3,20,21,22,23,24,25,27,28,29,30,31,32,33,34,35]

def Plot( dataTree, xvar, finalPlot, outputPath, models = [], pguns = [], materials = [], E0 = -1 , Dx = -1, plotData = False) :
	
	dict = Plotter()

	leg = TLegend(0.10,0.1,0.9,0.9)
	leg.SetTextSize(0.05);

	fPlot = finalPlot.replace("RATIO_","").replace("ASYM_","")
	var = vardef[fPlot]

	if(find(finalPlot,"RATIO") > -1 or find(finalPlot,"ASYM") > -1) :

		mystr = " in " +str(Dx) + " mm"
		if(xvar=="thickness") :
			mystr = " for " + str(E0) + " GeV"

		titleMultigr = finalPlot
		if(finalPlot == "RATIO_TOTAL") :
			titleMultigr = "Ratio of total (inel + el) cross sections" + mystr
		if(finalPlot == "ASYM_TOTAL") :
			titleMultigr = "Asymmetry of total (inel + el) cross sections" + mystr
		if(finalPlot == "RATIO_INEL") :
			titleMultigr = "Ratio of inelastic cross sections" + mystr
		if(finalPlot == "ASYM_INEL") :
			titleMultigr = "Asymmetry of inelastic  cross sections" + mystr

		pdgenergies  = [ 1, 5, 10, 50, 100 ]
		pdgRatios_p  = [ 1.67, 1.42, 1.31, 1.14, 1.10 ]
		pdgRatios_pi = [ 1.22, 1.13, 1.10, 1.05, 1.03 ]
		pdgRatios_K  = [ 1.61, 1.32, 1.23, 1.10, 1.07 ]

		if(xvar=="energy") : 
			ratiotxt = open("%s_in%imm.txt" % (finalPlot,Dx),"w")
		else :
			ratiotxt = open("%s_for%iGeV.txt" % (fialPlot,E0),"w")
		ratiotxt.write("\\begin{tabular}\n")
		
		
		grs = []
		
		for pg in range(0, len(pguns)-1, 2) :
		
			ratiotxt.write( "\\multicolumn{2}{c}{ratio " + str(dict._all_pguns[pguns[pg+1]].GetLatex("$")) + "/" + str(dict._all_pguns[pguns[pg]].GetLatex("$")) + "} \\\\ \\hline \n" )
			nm = 0
			for m in models :
			
				ratiotxt.write( "\\multicolumn{2}{c}{ " + m + "} \\\\ \\hline \n" )

				varexp = "h_"+str(pg)+m
				select_template = "model == {mod} && material == {mat} && pGun == {part}"
				select = select_template.format(mod = ord(m[0]), mat = ord(materials[0][0]), part = dict._all_pguns[pguns[pg]]._pdgID )
				
				if(xvar=="energy") : 
						select += " && thickness == " + str(Dx)
				elif(xvar=="thickness") :
						select += " && energy == " + str(E0)

				dataTree.SetEntryList(0)
				dataTree.Draw(">>"+varexp,select, "entrylist")
				list = gDirectory.Get(varexp)
				entries = list.GetN()
				dataTree.SetEntryList(list)
				errx = array( 'd' , [0.] * entries )
				
				has_error = (find(finalPlot,"MULTI") > -1 or find(finalPlot,"TOTAL")>-1 or find(finalPlot,"INEL")>-1 or find(finalPlot,"EL")>-1)
			
				if entries == 0 :
					print "No entries selected!"
					print "Selection used: " + select
					continue
				
				if(has_error) :
					dataTree.Draw(xvar+":"+var+":"+var+"_err","","colz")
					gr = TGraphErrors(entries,dataTree.GetV1(),dataTree.GetV2(),errx,dataTree.GetV3())
				else :
					dataTree.Draw(xvar+":"+var)
					gr = TGraphErrors(entries,dataTree.GetV1(),dataTree.GetV2())

				ty1 = dataTree.GetV2()
				terry1 = dataTree.GetV3()
				y1=[]
				erry1=[]
				for i in range(0,entries) :
					y1.append(ty1[i])
					if has_error :
						erry1.append(terry1[i])
									
				varexp = "h_"+str(pg+1)+m
				select = select_template.format(mod = ord(m[0]), mat = ord(materials[0][0]), part = dict._all_pguns[pguns[pg+1]]._pdgID )
				
				if(xvar=="energy") : 
						select += " && thickness == " + str(Dx)
				elif(xvar=="thickness") :
						select += " && energy == " + str(E0)

				dataTree.SetEntryList(0)
				dataTree.Draw(">>"+varexp,select, "entrylist")
				list2 = gDirectory.Get(varexp)
				dataTree.SetEntryList(list2)

				if(has_error) :
					dataTree.Draw(xvar+":"+var+":"+var+"_err")
					gr = TGraphErrors(entries,dataTree.GetV1(),dataTree.GetV2(),errx,dataTree.GetV3())
				else :
					dataTree.Draw(xvar+":"+var)
					gr = TGraphErrors(entries,dataTree.GetV1(),dataTree.GetV2())

				tx = dataTree.GetV1() 
				ty2 = dataTree.GetV2() 
				terry2 = dataTree.GetV3()
				x = []
				y2=[]
				erry2=[]
				for i in range(0,entries) :
					y2.append(ty2[i])
					if has_error :
						erry2.append(terry2[i])
					x.append(tx[i])

				y = []
				erry = []
				for ee in range(0,entries) : 

					ratiotxt.write( str(x[ee]) )
					if(find(finalPlot,"RATIO")>-1) :
						y.append(y2[ee] / y1[ee])
					else :
						#y.append(100*TMath.Abs(y1[ee] - y2[ee])/(2. - y1[ee] - y2[ee]))
						y.append(100*TMath.Abs(y1[ee] - y2[ee])/2.)

					if(len(erry1) > 0) :
						totErr2 = TMath.Power(erry1[ee]/y1[ee],2) + TMath.Power(erry2[ee]/y2[ee],2)
						erry.append(y[ee] * TMath.Sqrt(totErr2))
						ratiotxt.write(' & $ {0:4.2} \\pm {1:4.2} $ \\\\ \n'.format(y[ee],erry[ee]) )
					else :
						ratiotxt.write(' & $ {0:4.2} $ \\\\ \n'.format(y[ee]) )
						

				if(len(erry) > 1) :
					gr = TGraphErrors(entries,array('d',x),array('d',y),array('d',errx),array('d',erry))
				else :
					gr = TGraphErrors(entries,array('d',x),array('d',y))

				gr.SetMarkerColor(colors[pg/2])
				gr.SetMarkerStyle(20+nm)

				label = dict._all_pguns[pguns[pg+1]].GetLatex("LEG") + " / " + dict._all_pguns[pguns[pg]].GetLatex("LEG")
				if(len(models) > 1) :
					label += " (" + m + ")"
				leg.AddEntry(gr,label,"P")

				if(plotData and (find(finalPlot,"TOTAL")>-1 or find(finalPlot,"INEL")>-1) and find(finalPlot,"RATIO")>-1 ) :

					grPDG = 0
					if(dict._all_pguns[pguns[pg]].GetName()=="p" and dict._all_pguns[pguns[pg+1]].GetName()=="pbar") :
						grPDG = TGraphErrors(5,array('d',pdgenergies),array('d',pdgRatios_p))
					elif(dict._all_pguns[pguns[pg]].GetName()=="Piplus" and dict._all_pguns[pguns[pg+1]].GetName()=="Piminus") :
						grPDG = TGraphErrors(5,array('d',pdgenergies),array('d',pdgRatios_pi))
					elif(dict._all_pguns[pguns[pg]].GetName()=="Kplus" and dict._all_pguns[pguns[pg+1]].GetName()=="Kminus") :
						grPDG = TGraphErrors(5,array('d',pdgenergies),array('d',pdgRatios_K))

					if grPDG :
						if(len(pguns)>2) :
							grPDG.SetMarkerColor(colors[pg/2])
						else :
							grPDG.SetMarkerColor(4)
						grPDG.SetMarkerStyle(28)
						grPDG.SetMarkerSize(1.2)
						if nm == len(models)-1 :
							grs.append(grPDG)
							leg.AddEntry(grPDG,dict._all_pguns[pguns[pg+1]].GetLatex("LEG") + " / " + dict._all_pguns[pguns[pg]].GetLatex("LEG") +" PDG","P")
			
				nm+=1
				grs.append(gr)	
		
		ratiotxt.write("\\hline\n\\end{tabular}")

		c = TCanvas()
		leg_pad = TPad("leg_pad","",0.73,0,1.,1.)
		gr_pad = TPad("gr_pad","",0.03,0,0.8,1.)
		gr_pad.cd()


		gr_pad.SetLogx()
		gr_pad.SetGrid()

		for gg in grs :
			
			gg.GetYaxis().SetTitleOffset(1.5)
			gg.SetTitle(titleMultigr)
			if(finalPlot.find("RATIO") > -1) :
				gg.GetYaxis().SetTitle("Ratio")
				gg.GetYaxis().SetRangeUser(0,3.)
			else :
				gg.GetYaxis().SetTitle("Asym (%)")
	
			if(xvar=="energy") :
				gg.GetXaxis().SetTitle("|p| (GeV)")
				gg.GetXaxis().SetRangeUser(0.5,300)
			elif(xvar=="thickness") :
				gg.GetXaxis().SetTitle("#Delta x (mm)")
	
			if(gg==grs[0]) :
				gg.Draw("APL")
			else :
				gg.Draw("PL SAME")

		leg_pad.cd()
		leg.Draw()

		c.cd()
		gr_pad.Draw()
		leg_pad.Draw()
		c.Print(os.path.join(outputPath,finalPlot + mystr.replace(" ","_") + ".pdf"))	
		c.Clear()
	
	else :
	
		c = TCanvas()
		leg_pad = TPad("leg_pad","",0.73,0,1.,1.)
		gr_pad = TPad("gr_pad","",0.03,0,0.8,1.)
		gr_pad.cd()

		mystr = " in " +str(Dx) + " mm"
		if(xvar=="thickness") :
			mystr = " for " + str(E0) + " GeV"

		nameMultigr = "mgr" + mystr
		titleMultigr = finalPlot
		if(finalPlot == "TOTAL") :
			titleMultigr = "Total (inel + el) probability of interaction" + mystr
		elif(finalPlot == "INEL") :
			titleMultigr = "Inelastic probability of interaction" + mystr
		elif(finalPlot == "EL") :
			titleMultigr = "Elastic probability of interaction" + mystr 
		elif(finalPlot == "PERC_PLUS") :
			titleMultigr = "Percentage of positive particles"  + mystr
		elif(finalPlot == "PERC_MINUS") :
			titleMultigr = "Percentage of negative particles"  + mystr
		elif(finalPlot == "MULTI") :
			titleMultigr = "Multiplicity of secondaries"  + mystr
		elif(finalPlot == "MULTI_NCH") :
			titleMultigr = "Multiplicity (neutral)"  + mystr
		elif(finalPlot == "MULTI_NCH_NOGAMMA") :
			titleMultigr = "Multiplicity (neutral minus gamma)"  + mystr
		elif(finalPlot == "PERC_NCH") :
			titleMultigr = "Percentage of neutral particles"  + mystr
		elif(finalPlot == "MULTI_NOGAMMA") :
			titleMultigr = "Multiplicity excluding gammas" + mystr

		grs = []

		PintOverSigmaFactor = Dx /(1000. * dict._all_materials[materials[0]].GetSigmaDxOverPintFactor() * 1000.)	

			
		#COMPAS Inelastic Xsec data in Al
		COMPAS_p_x           = [ 1.52, 5., 9., 20., 30., 60. ]
		COMPAS_p_sigmaErr    = [ 10., 4., 4., 5., 5., 7. ]
		COMPAS_p_sigma       = [ 445., 445., 465., 446., 445., 455. ]
		COMPAS_p_y           = [ x*PintOverSigmaFactor for x in COMPAS_p_sigma ]
		COMPAS_p_yErr        = [ x*PintOverSigmaFactor for x in COMPAS_p_sigmaErr ]
		COMPAS_pbar_x        = [ 1.45,6.65,13.3,25.,30.,60. ]
		COMPAS_pbar_sigma    = [ 617., 558., 536., 480., 457., 439. ]
		COMPAS_pbar_sigmaErr = [ 17., 10., 10., 9., 11., 13. ]
		COMPAS_pbar_y        = [ x*PintOverSigmaFactor for x in COMPAS_pbar_sigma ]
		COMPAS_pbar_yErr     = [ x*PintOverSigmaFactor for x in COMPAS_pbar_sigmaErr ]

		#COMPAS Inelastic Xsec data in Be

		COMPAS_inBe_p_x           = [ 3., 5., 9., 30.,50.,60. ]
		COMPAS_inBe_p_sigmaErr    = [ 4., 3., 3., 3., 3., 2. ]
		COMPAS_inBe_p_sigma       = [ 236., 207., 210., 210., 210., 216. ]
		COMPAS_inBe_p_y           = [ x*PintOverSigmaFactor for x in COMPAS_inBe_p_sigma ]
		COMPAS_inBe_p_yErr        = [ x*PintOverSigmaFactor for x in COMPAS_inBe_p_sigmaErr ]
		COMPAS_inBe_pbar_x        = [ 6.65,13.3,20.,30.,40. ]
		COMPAS_inBe_pbar_sigma    = [ 296., 275., 240., 235., 226., 190. ]
		COMPAS_inBe_pbar_sigmaErr = [ 6., 4., 10., 6., 7. ]
		COMPAS_inBe_pbar_y        = [ x*PintOverSigmaFactor for x in COMPAS_inBe_pbar_sigma ]
		COMPAS_inBe_pbar_yErr     = [ x*PintOverSigmaFactor for x in COMPAS_inBe_pbar_sigmaErr ]

		#COMPAS Total Xsec data in Al

		COMPASTot_p_x               = [ 1.52, 1.8, 19.3, 20. ]  
		COMPASTot_p_sigmaErr        = [ 22., 27., 10., 10. ] 
		COMPASTot_p_sigma           = [ 687., 694., 687., 687. ] 
		COMPASTot_p_y               = [ x*PintOverSigmaFactor for x in COMPASTot_p_sigma ] 
		COMPASTot_p_yErr            = [ x*PintOverSigmaFactor for x in COMPASTot_p_sigmaErr ] 
		COMPASTot_pbar_x            = [ 1.45,1.8 ] 
		COMPASTot_pbar_sigma        = [ 1034., 1066. ] 
		COMPASTot_pbar_sigmaErr     = [ 40., 40. ] 
		COMPASTot_pbar_y            = [ x*PintOverSigmaFactor for x in COMPASTot_pbar_sigma ] 
		COMPASTot_pbar_yErr         = [ x*PintOverSigmaFactor for x in COMPASTot_pbar_sigmaErr ]

		COMPAS_p_gr = TGraphErrors(6,array('d', COMPAS_p_x ), array('d', COMPAS_p_y ), array('d', [0.]*6 ), array('d', COMPAS_p_yErr))
		COMPAS_pbar_gr = TGraphErrors(6, array('d', COMPAS_pbar_x), array('d', COMPAS_pbar_y), array ('d', [0.]*6 ), array('d', COMPAS_pbar_yErr) )
		COMPAS_p_gr.SetMarkerStyle(29)
		COMPAS_p_gr.SetMarkerSize(1.1)
		COMPAS_pbar_gr.SetMarkerStyle(30)
		COMPAS_pbar_gr.SetMarkerSize(1.2)

		COMPASTot_p_gr = TGraphErrors(4, array('d', COMPASTot_p_x), array('d', COMPASTot_p_y), array('d', [0.]*4 ), array('d', COMPASTot_p_yErr))
		COMPASTot_pbar_gr = TGraphErrors(2, array('d', COMPASTot_pbar_x), array('d', COMPASTot_pbar_y), array('d', [0.]*2 ), array('d', COMPASTot_pbar_yErr))
		COMPASTot_p_gr.SetMarkerStyle(29)
		COMPASTot_p_gr.SetMarkerSize(1.1)
		COMPASTot_pbar_gr.SetMarkerStyle(30)
		COMPASTot_pbar_gr.SetMarkerSize(1.2)

		COMPAS_inBe_p_gr = TGraphErrors(6, array('d', COMPAS_inBe_p_x), array('d', COMPAS_inBe_p_y), array('d', [0.]*6 ), array('d', COMPAS_inBe_p_yErr))
		COMPAS_inBe_pbar_gr = TGraphErrors(5, array('d', COMPAS_inBe_pbar_x), array('d', COMPAS_inBe_pbar_y), array('d', [0.]*5 ), array('d', COMPAS_inBe_pbar_yErr))
		COMPAS_inBe_p_gr.SetMarkerStyle(29)
		COMPAS_inBe_p_gr.SetMarkerSize(1.1)
		COMPAS_inBe_pbar_gr.SetMarkerStyle(30)
		COMPAS_inBe_pbar_gr.SetMarkerSize(1.2)
		

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
					errx = array( 'd' , [0.] * entries )
					gr = 0
				
					if(finalPlot == "MULTI" or finalPlot == "TOTAL" or finalPlot == "INEL" or finalPlot == "EL") :
						dataTree.Draw(xvar+":"+var+":"+var+"_err","","colz")	
						gr = TGraphErrors(entries,dataTree.GetV1(),dataTree.GetV2(),errx,dataTree.GetV3())
					else :
						dataTree.Draw(xvar+":"+var)
						gr = TGraphErrors(entries,dataTree.GetV1(),dataTree.GetV2())
					
					if(nh%2==0) :
						gr.SetMarkerColor(colors[int(nh/2.-1)])
						gr.SetMarkerStyle(int(24+nh/2.-1))
					else :
						gr.SetMarkerColor(colors[int(nh/2.)])
						gr.SetMarkerStyle(int(20+nh/2.))

					gr.SetMarkerSize(1.1)
					
					n2+=1

					label = dict._all_pguns[pg].GetLatex("LEG") + " in " + material
					if(len(models) > 1) :
						label += " (" + model + ")"
					leg.AddEntry(gr,label,"P")
					
					grs.append(gr)

					if plotData and n0 == len(models)-1 and materials[0] == "Al" :
						if find(finalPlot,"TOTAL")>-1 :
							if dict._all_pguns[pg].GetName()=="p":
								COMPASTot_p_gr.SetMarkerColor(4)#colors[int(nh/2.-1)])
								grs.append(COMPASTot_p_gr)
								leg.AddEntry(COMPASTot_p_gr,"COMPAS p total in Al","P")
							elif dict._all_pguns[pg].GetName()=="pbar":
								COMPASTot_pbar_gr.SetMarkerColor(4)#colors[int(nh/2.-1)])
								grs.append(COMPASTot_pbar_gr)
								leg.AddEntry(COMPASTot_pbar_gr,"COMPAS #bar{p} total in Al","P")
						elif find(finalPlot,"INEL")>-1 :
							if dict._all_pguns[pg].GetName()=="p":
								COMPAS_p_gr.SetMarkerColor(4)#colors[int(nh/2.-1)])
								grs.append(COMPAS_p_gr)
								leg.AddEntry(COMPAS_p_gr,"COMPAS p inel in Al","P")
							elif dict._all_pguns[pg].GetName()=="pbar":
								COMPAS_pbar_gr.SetMarkerColor(4)#colors[int(nh/2.-1)])
								grs.append(COMPAS_pbar_gr)
								leg.AddEntry(COMPAS_pbar_gr,"COMPAS #bar{p} inel in Al","P")
					elif plotData and n0 == len(models)-1 and materials[0] == "Be" and find(finalPlot,"INEL")>-1 :
						if dict._all_pguns[pg].GetName()=="p":
							COMPAS_inBe_p_gr.SetMarkerColor(4)#colors[int(nh/2.-1)])
							grs.append(COMPAS_inBe_p_gr)
							leg.AddEntry(COMPAS_inBe_p_gr,"COMPAS p inel in Be","P")
						elif dict._all_pguns[pg].GetName()=="pbar":
							COMPAS_inBe_pbar_gr.SetMarkerColor(4)#colors[int(nh/2.-1)])
							grs.append(COMPAS_inBe_pbar_gr)
							leg.AddEntry(COMPAS_inBe_pbar_gr,"COMPAS #bar{p} inel in Be","P")

			
							
			n0+=1


		gStyle.SetOptStat(0)
		gr_pad.SetGrid()
		if(xvar=="energy") :
			gr_pad.SetLogx()
		gr_pad.cd()

		ngg = 0
		for gg in grs :

			if(xvar=="energy") :
				gg.GetXaxis().SetTitle("|p| (GeV)")
			elif (xvar=="thickness") :
				gg.GetXaxis().SetTitle("#Delta x (mm)")
				gg.GetYaxis().SetTitleOffset(1.2)

			if(ngg==0) :
				gg.Draw("APL")	
				ngg+=1
			else :
				gg.Draw("PL SAME")	

			gr_pad.Update()

			if(finalPlot == "TOTAL") :
				gg.GetYaxis().SetTitle("P^{tot}_{int} = N^{inel+el}/N^{gen}")
				if(Dx == 1) :
					gg.GetYaxis().SetRangeUser(0.001,0.01)
				else :
					gg.GetYaxis().SetRangeUser(0.001,0.08)

			elif(finalPlot == "INEL") :
				if(Dx == 1) :
					gg.GetYaxis().SetRangeUser(0.001,0.01)
				else :
					gg.GetYaxis().SetRangeUser(0.001,0.08)
				gg.GetYaxis().SetTitle("P^{inel}_{int} = N^{inel}/N^{gen}")
			elif(finalPlot == "EL") :
				gg.GetYaxis().SetTitle("P^{el}_{int} = N^{el}/N^{gen}")
				if(Dx == 1) :
					gg.GetYaxis().SetRangeUser(0.00001,0.0025)
				else :
					gg.GetYaxis().SetRangeUser(0.00001,0.025)
			elif(find(finalPlot,"MULTI") > -1) :
				gg.GetYaxis().SetTitle("< Multi >")
				gg.GetYaxis().SetRangeUser(0.,40.)
			elif(find(finalPlot,"PERC") > -1) :
				gg.GetYaxis().SetTitle("%")
				gg.GetYaxis().SetRangeUser(0.,100.)
	
			gg.SetTitle(titleMultigr)
			gg.GetYaxis().SetTitleOffset(1.5)
		
			gr_pad.Update()
		leg_pad.cd()
		leg.Draw()

		printname = outputPath+"/" + finalPlot + mystr.replace(" ","_") + ".pdf"
		c.cd()
		gr_pad.Draw()
		leg_pad.Draw()
		c.Print(printname)
		c.Clear()




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

	plots = [ "RATIO_TOTAL", "INEL", "TOTAL", "EL", "MULTI", "MULTI_NCH", "MULTI_NCH_NOGAMMA", "PERC_PLUS", "PERC_MINUS" ]

	models = ["FTFP_BERT"]  # any you generated, by default "QGSP_BERT","FTFP_BERT"
	thicks = [1] #1,5,10
	materials = ["Al","Be","Si"] ### Al,Si,Be
	energies = [1,5,10,100] # any you generated, by default 1,5,10,100
	pguns = ["p","pbar","Kplus","Kminus","Piplus","Piminus"] # "p","pbar","Kplus","Kminus","Piplus","Piminus"
	#pguns = ["pbar","p"]
	path = "/afs/cern.ch/work/p/pluca/TargetOutput"

	file = TFile(path+"/TargetsPlots.root")
	dataTree = file.Get("summaryTree")
	
	os.system("mkdir -p "+path+"/Kaons")
	os.system("mkdir -p "+path+"/Protons")
	os.system("mkdir -p "+path+"/Pions")

	for p in plots :
		Plot( dataTree, "energy", p, path, models , pguns , materials , 2 , 1, True )
		Plot( dataTree, "energy", p, path+"/Protons", models , ["p","pbar"] , materials , 2 , 1, True )
		Plot( dataTree, "energy", p, path+"/Kaons", models , ["Kplus","Kminus"] , materials , 2 , 1, True )
		Plot( dataTree, "energy", p, path+"/Pions", models , ["Piplus","Piminus"] , materials , 2 , 1, True )



