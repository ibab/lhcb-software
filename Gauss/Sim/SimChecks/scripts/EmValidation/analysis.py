
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

#draw settings for different particles and lists
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

#draws and saved multigraph canvas for dedx analysis
def addCanvas(type,mGraph,particles,plist):

	#string of paricles being displayed on multigraph
	partStrings = map(str,particles)
	particleRange = '-'.join(partStrings)
	
	c1 = TCanvas()
	c1.SetLogx()
	c1.SetGrid()
	c1.cd(0)
	
	mGraph.Draw("ALCP")
	
	c1.BuildLegend()
	c1.SaveAs("plots/"+type+"_"+particleRange+"_"+str(plist)+".pdf")
	c1.Close()
		
#BremVeloCheck output analysis
def bremAnalysis(particles, plists, energies,opts):
	
	for particle in particles:
		for plist in plists:
			for energy in energies:

				#extracting histograms from job output
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
				#Drawing/Saving histograms and mean values	
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


#EmGaussMoni output analysis
def dedxAnalysis(particles, plists, energies,opts):
	
	#processing Bethe Bloch calculator and landau analysis scripts
	gROOT.ProcessLine('.L landauGenn.C')
	gROOT.ProcessLine('.L landau4new.C')

	#creating theoretical distributions
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
		
		#looping over particles and drawing all particles on same graph for each physics list
		for particle in particles:

			histosFiles =[]
			hfiledir = ""

			for pgunE in energies:
				histosPath = 'emstudy_'+str(plist)+'_'+str(particle)+'_'+str(pgunE)+'_'+str(opts['veloType'])+'_'+str(opts['testType'])+'-histos.root'
				histosFiles += [histosPath]
			
			#estimating MPV and FWHM data values and printing to file
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

			#reading in data and plotting graphs for each particle
			infile = open(textfile_new)
			for line in infile:

				line = line.strip('\n').split()
				listMPV.append(float(line[0]))
				listFWHM.append(float(line[1]))

			#converting to arrays to use with TGraph
			mpv_array = array('d')
			fwhm_array = array('d')
			bg_array = array('d')
			bg_array.fromlist(betagam_values)
			mpv_array.fromlist(listMPV)
			fwhm_array.fromlist(listFWHM)
			

			print bg_array
			print mpv_array
			print fwhm_array
			
			#Producing graphs
			MPVgraph = TGraph(len(energies),bg_array,mpv_array )	
			FWHMgraph = TGraph(len(energies),bg_array,fwhm_array )	
			
			#Setting visual options
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
			
			#appending graphs to lists of  all particle cases, for individual physics lists
			MPV_list+=[MPVgraph]
			FWHM_list+=[FWHMgraph]
	
			#end of particle loop

		#creating multigraph for superimpose all particles for individual physics lists	
		mgMPV_full = TMultiGraph()
		mgFWHM_full = TMultiGraph()
		
		for MPV in MPV_list:			
			mgMPV_full.Add(MPV,"cp")

		for FWHM in FWHM_list:	
			mgFWHM_full.Add(FWHM,"cp")
	
		mgMPV_full.Add(MPVtheograph,'cp')
		mgFWHM_full.Add(FWHMtheograph,'cp')
		
		#configuring and printing canvases and graphs
		addCanvas("MPV",mgMPV_full,particles,plist)
		addCanvas("FWHM",mgFWHM_full,particles,plist)

