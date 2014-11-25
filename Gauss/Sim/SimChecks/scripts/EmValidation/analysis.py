
import os
import sys
from ROOT import *
from array import array
import theoretical


#calculate gamma (beta assumed to be 1)
def betagam(energy, particle):
	mass = 0
	print("betagam")
	if (float(particle) == 11):
		print("electron")
		mass = 0.000511
	elif(float(particle) == 13):
		print("muon")
		mass = 0.1057
	elif(float(particle) == 211):
		print("pion")
		mass = 0.1396
	else:
		print("No mass known for chosen particle. If particle choice correct, please add new particle in betagam() function in analysis.py")

	return energy/mass

def plotSet(particle,plist):
	if (particle == 11):
		pname = "electrons "+plist
		colour = kGreen
	
	elif (particle == 13):
		pname = "muons "+plist
		colour = kRed
	
	elif (particle == 211):
		pname = "pions "+plist
		colour = kBlue
	else:
		print("Unknown particle. If ID is correct please add new particle case to plotSet() in analysis.py")
	
	return	{
			'pname':	pname,
			'colour':	colour
		 	}





def bremAnalysis(particles, plists, energies,opts):
	for particle in particles:
		for plist in plists:
			for energy in energies:
				
				histospath = 'emstudy_'+str(plist)+'_'+str(particle)+'_'+str(energy)+'_'+str(opts['veloType'])+'_'+str(opts['testType'])+'-histos.root'
				fBrem = TFile(histospath)
				fBrem.cd("photon energy")
				plot1 = gROOT.FindObject("1")
				photonEplot = plot1.Clone("photonEplot")
				photonEplot.SetDirectory(0)
				fBrem.cd()
				fBrem.cd("number of photons per track")
				plot2 = gROOT.FindObject("1")
				nphotonplot = plot2.Clone("nphotonplot")
				nphotonplot.SetDirectory(0)
				fBrem.Close()

				photonEplot.Draw()
				nphotonplot.Draw()
				
			
				c1 = TCanvas()
				c2 = TCanvas()

				c1.cd()
				photonEplot.GetXaxis().SetRange(0,200)
				photonEplot.GetXaxis().SetName("GeV")
				photonEplot.SetFillColor(kRed)
				photonEplot.SetFillStyle(3001)
				photonEplot.Draw("E")
				ROOT.gPad.Update()	
				ptnE = photonEplot.GetMean()
				
				c2.cd()
				nphotonplot.GetXaxis().SetRange(0,20)
				nphotonplot.GetXaxis().SetName("\# per track")
				nphotonplot.Draw("E")
				ROOT.gPad.Update()
				nptn = nphotonplot.GetMean()


				c1.SaveAs("plots/photonE_"+str(energy)+"_"+str(particle)+"_"+str(plist)+".pdf")
				c2.SaveAs("plots/nphotons_"+str(energy)+"_"+str(particle)+"_"+str(plist)+".pdf")
				f=open("plots/photonEn_"+str(energy)+"_"+str(particle)+"_"+str(plist)+".txt",'w')
				f.write("mean energy\t\t\tmean number\n")
				f.write(str(ptnE)+"\t\t"+str(nptn))
				f.close()



def dedxAnalysis(particles, plists, energies,opts):
	gROOT.ProcessLine('.L landauGenn.C')
	gROOT.ProcessLine('.L landau4new.C')

	mpv_theo_array = array('d')
	fwhm_theo_array = array('d')
	bg_theo_array = array('d')

	mpv_theo_array.fromlist(theoretical.theo_mpv())
	fwhm_theo_array.fromlist(theoretical.theo_fwhm())
	bg_theo_array.fromlist(theoretical.theo_bg())

	MPVtheograph = TGraph(len(theoretical.theo_bg()),bg_theo_array,mpv_theo_array )	
	FWHMtheograph = TGraph(len(theoretical.theo_bg()),bg_theo_array,fwhm_theo_array )

	MPVtheograph.SetMarkerColor(kMagenta )
	MPVtheograph.SetLineColor(kMagenta )
	MPVtheograph.SetTitle("theoretical")
	
	FWHMtheograph.SetMarkerColor(kMagenta )
	FWHMtheograph.SetLineColor(kMagenta )
	FWHMtheograph.SetTitle("theoretical")


	for plist in plists:
		MPV_list = []
		FWHM_list = []
		for particle in particles:

			histosFiles =[]
			hfiledir = ""

			for pgunE in energies:
				histosPath = 'emstudy_'+str(plist)+'_'+str(particle)+'_'+str(pgunE)+'_'+str(opts['veloType'])+'_'+str(opts['testType'])+'-histos.root'
				histosFiles += [histosPath]

			textfile_new="output/landau_"+str(particle)+"_"+str(plist)+"_output.txt"
			textfile="output/landau_output.txt"
			if os.path.exists(textfile):
				print "Removing old version  of ", textfile
				os.remove(textfile)

			for histosFile in histosFiles:
				hfile=hfiledir+histosFile
				print "opening file", hfile
				landau4new(hfile)
				# Rename outputfile to avoid overwriting
				print "Moving output/landau_output.root to output/landau_"+histosFile
				os.rename("output/landau_output.root","output/landau_"+histosFile)

			print "Moving output/landau_output.txt to output/landau_"+str(particle)+"_"+str(plist)+".txt"
			os.rename(textfile,textfile_new)
	
			betagam_values = []	
			# Theor. predictions    
			for energy in energies:
				landauGenn(energy)
				betagam_values += [betagam(energy,particle)]

			listMPV = []
			listFWHM = []

			infile = open(textfile_new)
			
			for line in infile:
				line = line.strip('\n').split()
				listMPV.append(float(line[0]))
				listFWHM.append(float(line[1]))

			mpv_array = array('d')
			fwhm_array = array('d')
			bg_array = array('d')

			bg_array.fromlist(betagam_values)
			mpv_array.fromlist(listMPV)
			fwhm_array.fromlist(listFWHM)
			
			print bg_array
			print mpv_array
			print fwhm_array
			
			MPVgraph = TGraph(len(energies),bg_array,mpv_array )	
			FWHMgraph = TGraph(len(energies),bg_array,fwhm_array )	
			

			colour = plotSet(particle,plist)['colour']
			name = plotSet(particle,plist)['pname']

			MPVgraph.SetMarkerColor(colour)
			MPVgraph.SetLineColor(colour)
			MPVgraph.SetTitle(name)
			MPVgraph.SetMarkerStyle( 29 )
			MPVgraph.SetFillStyle(0)

			FWHMgraph.SetMarkerColor(colour )
			FWHMgraph.SetLineColor(colour )
			FWHMgraph.SetTitle(name)
			FWHMgraph.SetMarkerColor( 4 )
			FWHMgraph.SetMarkerStyle( 29 )
			FWHMgraph.SetFillStyle(0)

#			mgMPV = TMultiGraph()
#			mgMPV.Add(MPVgraph,'cp')
#			mgMPV.Add(MPVtheograph,'cp')
#			mgFWHM = TMultiGraph()
#			mgFWHM.Add(FWHMgraph,'cp')
#			mgFWHM.Add(FWHMtheograph,'cp')

#			c1 = TCanvas()
#			c1.SetLogx()
#			c2 = TCanvas()
#			c2.SetLogx()

#			c1.cd()
#			c1.SetGrid()
#			mgMPV.Draw("ALCP")
#			c2.cd()
#			c2.SetGrid()
#			mgFWHM.Draw("ALCP");

#			c1.SaveAs("plots/MPV_"+str(particle)+"_"+str(plist)+".pdf")
#			c2.SaveAs("plots/FWHM_"+str(particle)+"_"+str(plist)+".pdf")

			MPV_list+=[MPVgraph]
			FWHM_list+=[FWHMgraph]
		
		mgMPV_full = TMultiGraph()
		mgFWHM_full = TMultiGraph()
		
		for MPV in MPV_list:			
			mgMPV_full.Add(MPV,"cp")

		for FWHM in FWHM_list:	
			mgFWHM_full.Add(FWHM,"cp")
	
		mgMPV_full.Add(MPVtheograph,'cp')
		mgFWHM_full.Add(FWHMtheograph,'cp')
	
		c3 = TCanvas()
		c3.SetLogx()
		c3.SetGrid()
		c3.cd()
		mgMPV_full.Draw("ALCP")
		c3.BuildLegend()
		
		c4 = TCanvas()
		c4.SetLogx()
		c4.SetGrid()
		c4.cd()
		mgFWHM_full.Draw("ALCP");
		c4.BuildLegend()



		
		partStrings = map(str,particles)
		particleRange = '-'.join(partStrings)
		
		c3.SaveAs("plots/MPV_"+particleRange+"_"+str(plist)+".pdf")
		c4.SaveAs("plots/FWHM_"+particleRange+"_"+str(plist)+".pdf")

