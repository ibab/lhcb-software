from ROOT import TFile,kBlue,kGreen,kYellow,TCanvas,gDirectory,TH1F
import os

def getfile(filename):
	file = TFile(filename,"READ")
	if not file:
		print "Didn't find the file : ", filename
	return file

def gethist(file, h, factor, color):
	hist = file.Get(h)
	if not hist:
		print "Didn't find the histogram : ", h, " in file ",filename
	hist.SetLineColor(color)
	hist.SetFillColor(color)
	if hist.GetEntries() != 0 and hist.Integral() != 0:
		hist.Scale((1.*factor))#/hist.Integral())
	return hist

def drawstacked(histlist):

	for i in histlist:
		if type(i) != type(TH1F()):
			print "Warning - Drawstack cant be used to items != TH1F."
			return

	for i in range(len(histlist)):
		print "Integral ", histlist[i].Integral()
		if i+1 < len(histlist):
			histlist[i+1].Add(histlist[i],1.)

	if(len(histlist) > 0):
		histlist[len(histlist)-1].Draw("BARsame")

	for i in range(len(histlist)):
		reverse_index = len(histlist)-i-2
		if len(histlist)-i-2 >= 0:
			histlist[reverse_index].DrawCopy("BARsame")
		
def drawstacked_list(file_list,scalelist,colorlist,varpath):
	histlist=[]
	for i in range(len(file_list)):
		histlist.append(gethist(file_list[i],varpath,scalelist[i],colorlist[i]))
	
	drawstacked(histlist)
		
		
		
def GetKeyNames( self, dir = "" ):
        self.cd(dir)
        return [key.GetName() for key in gDirectory.GetListOfKeys()]
TFile.GetKeyNames = GetKeyNames


c1 = TCanvas()
c1.cd()
c1.SetLogy(1)

#Lumi = 27200/(total) * Lumi2011
EW_Data_WMu_2011_totalevents = 114675123. + 83457830.
print "Total number of events on data: ", EW_Data_WMu_2011_totalevents
ratio_Wmu_ew = 0.2569 / 2.0746
print "Ratio between Wmu Line and EW Stream ", ratio_Wmu_ew
#Lumi2011 = 1.1067
Lumi2011 = 1.02
print "Lumi of 2011 ", Lumi2011

Lumi = 1000. #1 fb

#treefilelist=[getfile(os.path.abspath("../Resources/MC11/ttbar/56/ZJESAllMC.root")),getfile(os.path.abspath("../Resources/MC11/Zmumujet/55/ZJESAllMC.root")),getfile(os.path.abspath("../Resources/MC11/Wmunujet/53/ZJESAllMC.root"))]
treefilelist=[getfile(os.path.abspath("../Resources/MC11/ttbar/74/ZJESAllMC.root")),getfile(os.path.abspath("../Resources/MC11/Zmumu_PHOTOS/78/ZJESAllMC.root")),getfile(os.path.abspath("../Resources/MC11/Wmunujet/75/ZJESAllMC.root"))]

#filelist=[getfile(os.path.abspath("output/ttbar.root")),getfile(os.path.abspath("output/zmumu.root")),getfile(os.path.abspath("output/wmunujet.root"))]
filelist=[getfile(os.path.abspath("output/ttbar.root")),getfile(os.path.abspath("output/zmumu_PHOTOS.root")),getfile(os.path.abspath("output/wmunujet.root"))]
#filelist=[getfile(os.path.abspath("output/ttbar.root"))]
colorlist=[kBlue,kYellow+1,kGreen]

#colorlist=[kBlue]
#scalelist=[(160000.*0.018702), (3000000. * 0.37722), (1.1*(10**7) * 0.25562)] 
#scalelist=[(160000.*0.0086044), (3000000. * 0.37722), (1.1*(10**7) * 0.25562)] 
scalelist=[(160000.*0.0086044), (76.7 * 1000 / .256948667348957882), (1.1*(10**7) * 0.25562)] 


totalentrieslist = [(treefilelist[0].Get("PFJetsMC/StdPFJets")).GetEntries(),(treefilelist[1].Get("PFJetsMC/StdPFJets")).GetEntries(),(treefilelist[2].Get("PFJetsMC/StdPFJets")).GetEntries() ]

data_file = getfile(os.path.abspath("output/EW_Data_WMu.root"))
data_treefile = getfile(os.path.abspath("../Resources/EW_WMu_DataStrip17/79/ZJESAllMC.root"))
data_tree = data_treefile.Get("PFJets/StdPFJets")

print "Total of entries on data_tree: ", data_tree.GetEntries()
lumieff = ((data_tree.GetEntries() / ratio_Wmu_ew ) / EW_Data_WMu_2011_totalevents  ) * Lumi2011
print "Effective Lumi for 2011: ", lumieff
#lumieff = 0.11


print "Scale weights:"
for i in range(len(scalelist)):
	scalelist[i] = scalelist[i] * lumieff / totalentrieslist[i] 
	print scalelist[i]


#scalelist=[1.,1.,1.]

#varpath = "IsoMuon15/ljet_pt"

#directory_path = "IsoMuon15"
#directory_path = "noselection"

directory_paths = filelist[0].GetKeyNames()

print "List of directory paths"
print directory_paths

for directory_path in directory_paths:
	varpaths = filelist[0].GetKeyNames(directory_path)

	if not os.path.exists(directory_path):
		os.system("mkdir -p "+ os.path.abspath("results/"+directory_path))

	for varpath in varpaths:

		varpath = directory_path+"/"+varpath
		data_hist = gethist(data_file,varpath,1.,1)


		data_hist.Draw("")
		data_hist.SetMinimum(0.1)
		drawstacked_list(filelist, scalelist, colorlist,varpath)

		data_hist.SetMarkerStyle(20)
		data_hist.Draw("esame")
		c1.RedrawAxis()

		c1.Print("results/"+ varpath +".png")
