
import os
import sys
from ROOT import* 
from array import array
import theoretical


class graph_init:

	#creates canvas with desired parameters in options string e.g -log, -grid 
	def __init__(self,optStr):
		
		c1 = TCanvas()
		c1.Divide(1,2)
		p1= c1.cd(1)
		p2= c1.cd(2)

		if("-grid" in optStr):
			p1.SetGrid()
			p2.SetGrid()
		
		if("-log" in optStr):
			p1.SetLogx()
			p2.SetLogx()
		
		l1 = TLegend(0.6,0.7,0.9,0.9)
		l1.SetFillColor(0)
		
		self.base = False
		self.ngraphs = 0
		self.topPad = p1
		self.botPad = p2
		self.canvas = c1
		self.legend = l1
		self.canvas = c1

	#Creates base graph with desired axis limits to overlay subsequent graphs
	def addBase(self,name,legname,pos,graph,xmin,xmax,ymin,ymax):

		self.base = True
		self.canvas.cd(pos)
		graph.SetTitle(name)
		graph.GetXaxis().SetLimits(xmin,xmax)
		graph.GetYaxis().SetRangeUser(ymin,ymax)
		graph.Draw("AC")
		if(pos == 1):
			self.legend.AddEntry(graph,legname)
		self.ngraphs +=1

	#overlays on  base
	def overlay(self,name,graph,opt,pos):
		if not (self.base):
			print "Warning: No base graph. Axis limits may not be properly set!"
		
		self.canvas.cd(pos)
		graph.Draw(opt)
		if (pos == 1):
			self.legend.AddEntry(graph,name)
		self.ngraphs +=1

	#Draws legend and saves the canvas
	def save(self,name):
		self.canvas.cd(0)
		self.legend.Draw()
		print 'Saving canvas with '+str(self.ngraphs)+' plots.'
		self.canvas.SaveAs(name)

	def make(self):
		self.canvas.cd(0)
		self.legend.Draw()
	

#calculate betagamma 
def betagam(momentum, particle):
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

	return momentum/mass

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
	
def refchisq(ref,dat):
	chisq = []
	for iter in range (len(ref)):
		chisq.append(((dat[iter]-ref[iter])*(dat[iter]-ref[iter]))/dat[iter])
		
	return float(sum(chisq)/len(chisq))


#compares reference values to reults for dedx studies, typically the physics list used in official productions on latest G4 release and are in ./reference/
#def emreference(inputdata,part):
#
#	infile = open("reference/"+type+"_"+part+".dat")
#
#	for line in infile:
#		line = line.strip('\n').split()
#		list_mpv.append(float(line[0]))
#		list_fwhm.append(float(line[1]))
#
#	ref_file = file.open("reference/"+type+"_"+part+".dat")
#
#	ref_data = array('d')
#	ref_data.fromfile(ref_file,len(ref_file))
#	
#	new_data = array('d')
#	new_data.fromlist(inputdata)
##
#	if (len(inputdata) == len(ref_file):


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

				c1.SaveAs("results/photonE_"+str(energy)+"_"+str(particle)+"_"+str(plist)+".pdf")
				c2.SaveAs("results/nphotons_"+str(energy)+"_"+str(particle)+"_"+str(plist)+".pdf")
				f=open("results/photonEn_"+str(energy)+"_"+str(particle)+"_"+str(plist)+".txt",'w')
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

	MPVtheograph = TGraphErrors(len(theoretical.theo_bg()),bg_theo_array,mpv_theo_array )	
	FWHMtheograph = TGraphErrors(len(theoretical.theo_bg()),bg_theo_array,fwhm_theo_array )

	MPVtheograph.SetMarkerColor(kMagenta )
	MPVtheograph.SetLineColor(kMagenta )
	MPVtheograph.SetTitle("theoretical")
	
	FWHMtheograph.SetMarkerColor(kMagenta )
	FWHMtheograph.SetLineColor(kMagenta )
	FWHMtheograph.SetTitle("theoretical")

	mpv_plists = []
	fwhm_plists = []

	mpv_comp_plists = []
	fwhm_comp_plists = []
	
	for plist in plists:

		with open("results/chisq_"+plist+".txt", "w") as chiFile:
			chiFile.write("PID\t\tMPV\t\tFWHM\n")
		
		mpv_parts = []
		fwhm_parts = []
		
		mpv_comp_parts = []
		fwhm_comp_parts = []
		allparts_mpv = []
		allparts_ref_mpv = []
		allparts_fwhm = []
		allparts_ref_fwhm = []
		
		#looping over particles and drawing all particles on same graph for each physics list
		for particle in particles:
			graphs_mpv_parts = []
			graphs_fwhm_parts = []

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
			listrefMPV = []
			listrefFWHM = []

			#reading in data fpr MPV and FWHM graphs for each particle
			infile = open(textfile_new)
			for line in infile:

				line = line.strip('\n').split()
				listMPV.append(float(line[0]))
				listFWHM.append(float(line[1]))

			#reading in reference data for each particle
			refFile = open("reference/pdgID_"+str(particle)+".dat")
			for line in refFile:

				line = line.strip('\n').split()
				listrefMPV.append(float(line[0]))
				listrefFWHM.append(float(line[1]))

			#comparing to reference data
			mpv_compare = [a/b for a,b in zip(listrefMPV,listMPV)]
			fwhm_compare = [a/b for a,b in zip(listrefFWHM,listFWHM)]

			#chisq calculation and saving

			chisq_part_mpv = refchisq(listrefMPV,listMPV)	
			chisq_part_fwhm = refchisq(listrefMPV,listMPV)

			print chisq_part_mpv
			print chisq_part_fwhm

			with open("results/chisq_"+plist+".txt", "a") as chiFile:
				chiFile.write(str(particle)+"\t\t"+str(chisq_part_mpv)+"\t\t"+str(chisq_part_fwhm)+"\n")




			#converting to arrays to use with TGraph
			mpv_array = array('d')
			fwhm_array = array('d')
			bg_array = array('d')
			mpv_comp_array = array('d')
			fwhm_comp_array = array('d')
			
			
			bg_array.fromlist(betagam_values)
			mpv_array.fromlist(listMPV)
			fwhm_array.fromlist(listFWHM)
			mpv_comp_array.fromlist(mpv_compare)
			fwhm_comp_array.fromlist(fwhm_compare)
			
			print mpv_comp_array
			print bg_array
			print mpv_array
			print fwhm_array
			
			#Producing graphs
			
			MPVgraph = TGraphErrors(len(energies),bg_array,mpv_array )	
			MPVgraph.Draw("LP")
		 	
			FWHMgraph = TGraphErrors(len(energies),bg_array,fwhm_array )	
			FWHMgraph.Draw("LP")

			MPVcompgraph = TGraphErrors(len(energies),bg_array,mpv_comp_array )	
			MPVcompgraph.Draw("LP")
		 	
			FWHMcompgraph = TGraphErrors(len(energies),bg_array,fwhm_comp_array )	
			FWHMcompgraph.Draw("LP")


			FWHMgraph.GetXaxis().SetRangeUser(0,1E6)
			FWHMgraph.GetYaxis().SetRangeUser(20,90)
			FWHMgraph.GetXaxis().SetTitle("#beta#gamma")
			FWHMgraph.GetYaxis().SetTitle("Landau full width at half maximum")
			
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
			FWHMgraph.SetMarkerStyle( 29 )
			FWHMgraph.SetFillStyle(0)
			
			MPVcompgraph.SetMarkerColor(colour)
			MPVcompgraph.SetLineColor(colour)
			MPVcompgraph.SetTitle(name)
			MPVcompgraph.SetMarkerStyle( 29 )
			MPVcompgraph.SetFillStyle(0)
			FWHMcompgraph.SetMarkerColor(colour )
			FWHMcompgraph.SetLineColor(colour )
			FWHMcompgraph.SetTitle(name)
			FWHMcompgraph.SetMarkerStyle( 29 )
			FWHMcompgraph.SetFillStyle(0)
			
			#appending graphs to lists of  all particle cases, for individual physics lists
			mpv_parts.append(MPVgraph)
			fwhm_parts.append(FWHMgraph)
	
			mpv_comp_parts.append(MPVcompgraph)
			fwhm_comp_parts.append(FWHMcompgraph)
			
			allparts_mpv.append(listMPV)
			allparts_ref_mpv.append(listrefMPV)
			allparts_fwhm.append(listFWHM)
			allparts_ref_fwhm.append(listrefFWHM)
			#end of particle loop
		
		allparts_mpv_app = []
		allparts_fwhm_app = []
		allparts_ref_mpv_app = []
		allparts_ref_fwhm_app = []
		#end of particle loop
		
		#calculating chisq of all particles in the list
		for it1 in range (len(allparts_mpv)):
			for it2 in range (len(allparts_mpv[0])):
				allparts_mpv_app.append(allparts_mpv[it1][it2])
				allparts_fwhm_app.append(allparts_fwhm[it1][it2])
				allparts_ref_mpv_app.append(allparts_ref_mpv[it1][it2])
				allparts_ref_fwhm_app.append(allparts_ref_fwhm[it1][it2])


		chisq_full_mpv = refchisq(allparts_ref_mpv_app,allparts_mpv_app)
		chisq_full_fwhm = refchisq(allparts_ref_fwhm_app,allparts_fwhm_app)

		with open("results/chisq_"+plist+".txt", "a") as chiFile:
			chiFile.write("Total\t"+str(chisq_full_mpv)+"\t\t"+str(chisq_full_fwhm)+"\n")
		
		mpv_plists.append(mpv_parts)
		fwhm_plists.append(fwhm_parts)
		
		mpv_comp_plists.append(mpv_comp_parts)
		fwhm_comp_plists.append(fwhm_comp_parts)

		#end of physics list loop

	#concatenated string of all particles
	partStrings = map(str,particles)
	particleRange = '-'.join(partStrings)
	
	plistindex = 0

	drawopt = "CP SAME"
	

	for plist in plists:

		c1 = graph_init("-grid-log")
		c1.addBase("MPV distributions: "+plist,"theoretical estimation",1,MPVtheograph,0.3,1.1E6,70.,230.)
		c1.addBase("Comparison with reference","",2,mpv_comp_plists[plistindex][0],0.3,1.1E6,0.7,1.3)

		c2 = graph_init("-grid-log")
		c2.addBase("FWHM distributions: "+plist,"theoretical estimation",1,FWHMtheograph,0.3,1.1E6,20.,80.)
		c2.addBase("Comparison with reference","",2,fwhm_comp_plists[plistindex][0],0.3,1.1E6,0.7,1.3)


		partindex = 0
		for part in particles:
			print partindex
			print plistindex
			pname = plotSet(part,"")['pname']
			mpv = mpv_plists[plistindex][partindex]
			fwhm = fwhm_plists[plistindex][partindex]
			mpvcomp = mpv_comp_plists[plistindex][partindex]
			fwhmcomp = fwhm_comp_plists[plistindex][partindex]
			


			c1.overlay(pname,mpv,drawopt,1)
			c1.overlay("",mpvcomp,drawopt,2)
			
			c2.overlay(pname,fwhm,drawopt,1)
			c2.overlay("",fwhmcomp,drawopt,2)
			partindex += 1

		c1.save("results/MPV_"+particleRange+"_"+str(plist)+".pdf")
		c2.save("results/FWHM_"+particleRange+"_"+str(plist)+".pdf")
		
		del c1
		del c2
		plistindex += 1
		


	
		


			
