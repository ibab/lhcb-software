#!/usr/bin/env python
#############################################################################
# makeRootFile.py                                              #
#############################################################################
# This PyRoot script parses a flat ASCII particle source file into the root #
# file format which is used in the TimeCorrSource LbMIB generator. The flat #
# file should have one particle per line, where each variable is space      #
# separated. All values are in LHCb coordinates. The particle variables are #
# as follows:                                                               #
#   LossID:     Id number of primary proton loss                            #
#   LossT0:     Time of proton loss w.r.t. LHC clock (ns)                   #
#   LossZ:      Distance from IP8 of proton loss (mm)                       #
#   LossA:      Atom involved in proton loss                                #
#   LossW:      Weight of proton loss (likelyhood)                          #
#   PartPdgId:  PDG ID of arriving particle                                 #
#   PartX:      x position of arriving particle at the interface plane (mm) #
#   PartY:      y position of arriving particle at the interface plane (mm) #
#   PartZ:      Distance from IP8 of particle origin (mm)                   #
#   PartDx:     Cosine of particle angle in the x plane at the interface    #
#   PartDy:     Cosine of particle angle in the y plane at the interface    #
#   PartEk:     Kinetic energy of particle at the interface plane (GeV)     #
#   PartDt:     Particle arrival time at the interface plane w.r.t. LHC     #
#               clock (ns)                                                  #
#   PartW:      Proabaility of particle arriving at interface plane if the  #
#               relevant primary proton loss is occurs                      #
#############################################################################
import os, sys, re
from ROOT import TTree, TFile, AddressOf, gROOT, RooStringVar, TDirectory
from optparse import OptionParser

def main():
  gROOT.SetBatch(True)

  # Parse the options
  parser = OptionParser()
  parser.add_option('-i', '--infile', action='store', type='string' ,dest='infilename', help='Get particle source input from FILE [Required]', metavar='FILE')
  parser.add_option('-o', '--outfile', action='store', type='string', dest='outfilename', help='Write particle source output to FILE.root', metavar='FILE.root')
  parser.add_option('-x', '--xmlheaderfile', action='store', type='string', dest='headerfilename', help='Get particle source header information from FILE.xml', metavar='FILE.xml')
  parser.add_option('-p', '--plot', action='store_true', dest='plot', default=False, help='Add histograms to the output root file')
  parser.add_option('-q', '--quiet', action='store_true', dest='quiet', default=False, help='Only error output')
  parser.add_option('-v', '--verbose', action='store_true', dest='verbose', default=False, help='Verbose output')
  parser.add_option('-d', '--oldformat', action='store_true', dest='oldformat', default=False, help='Input file uses old (non-correlation) file format')
  (options, args) = parser.parse_args()
  
  # Is the input file defined?
  if options.infilename == None:
    print 'ERROR: Input file not defined\n'
    parser.print_help()
    sys.exit()
  
  # Does the input file exist?
  infilename = ''
  if os.path.exists(os.path.join(os.path.curdir,options.infilename)):
    infilename = os.path.join(os.path.curdir,options.infilename)
  elif os.path.exists(options.infilename):
    infilename = options.infilename
  else:
    print 'ERROR: Input file "'+options.infilename+'" not found\n'
    parser.print_help()
    sys.exit()
  if not options.quiet: print 'Reading from file '+infilename

  # Is the header file defined?
  headerfilename = ''
  if options.headerfilename == None:
    if not options.quiet: print 'No header file defined. Header omitted'
  elif os.path.exists(os.path.join(os.path.curdir,options.headerfilename)):
    headerfilename = os.path.join(os.path.curdir,options.headerfilename)
  elif os.path.exists(options.headerfilename):
    headerfilename = options.headerfilename
  else:
    if not options.quiet: print 'Header file '+options.headerfilename+' not found. Header omitted'

  # Is the output file name defined?
  outfilename = ''
  if options.outfilename == None:
    outfilename = os.path.basename(options.infilename)+'.root'
  else:
    outfilename = options.outfilename
    
  # Verify the output ROOT file name
  namecheck = re.compile('^\S*.root$')
  if namecheck.match(outfilename) == None:
    print 'ERROR: Please provide a valid ROOT file name for the output file (FILE.root)\n'
    parser.print_help()
    sys.exit()
  if not options.quiet: print 'Writing to file '+outfilename

  # Make the ROOT file and trees
  RootF = TFile(outfilename,'RECREATE')
  EvtTree = TTree('Events','Event Tree')
  PartTree = TTree('Particles','Particle Tree')

  # Check if info xml file is present
  headers = None
  headerfolder = None
  if headerfilename != '':
    headerfolder = RootF.mkdir('Header')
    headers = parseInfoFile(headerfilename,headerfolder,options.quiet)
  
  # Create event struct
  gROOT.ProcessLine(\
    "struct EvtStruct{\
      Int_t LossId;\
      Double_t LossT0;\
      Double_t LossZ;\
      Double_t LossW;\
      Double_t sumOfWeights;\
      Int_t LossA;\
      Int_t firstPart;\
      Int_t nParts;\
    };")
  from ROOT import EvtStruct

  # Create particle struct
  gROOT.ProcessLine(\
    "struct PartStruct{\
      Int_t PartPdgId;\
      Double_t PartX;\
      Double_t PartY;\
      Double_t PartZ;\
      Double_t PartDx;\
      Double_t PartDy;\
      Double_t PartEk;\
      Double_t PartDt;\
      Double_t PartW;\
      Double_t CombW;\
      Int_t Evt;\
    };")
  from ROOT import PartStruct
  
  # Create branches
  evt = EvtStruct()
  EvtTree.Branch('LossId',AddressOf(evt,'LossId'),'LossId/I. Id number of primary proton loss')
  EvtTree.Branch('LossT0',AddressOf(evt,'LossT0'),'LossT0/D. Time of proton loss w.r.t. LHC clock (ns)') 
  EvtTree.Branch('LossZ',AddressOf(evt,'LossZ'),'LossZ/D. Distance from IP8 of proton loss (mm)')
  EvtTree.Branch('LossA',AddressOf(evt,'LossA'),'LossA/I. Atom involved in proton loss')
  EvtTree.Branch('LossW',AddressOf(evt,'LossW'),'LossW/D. Weight of proton loss (likelyhood)')
  EvtTree.Branch('sumOfWeights',AddressOf(evt,'sumOfWeights'),'sumOfWeights/D. Running sum of LossW')
  EvtTree.Branch('firstPart',AddressOf(evt,'firstPart'),'firstPart/I. Index of first daughter particle')
  EvtTree.Branch('nParts',AddressOf(evt,'nParts'),'nParts/I. Number of daughter particles')
  
  part = PartStruct()
  PartTree.Branch('PartPdgId',AddressOf(part,'PartPdgId'),'PartPdgId/I. PDG ID of arriving particle')
  PartTree.Branch('PartX',AddressOf(part,'PartX'),'PartX/D. x position of arriving particle at the interface plane (mm)')
  PartTree.Branch('PartY',AddressOf(part,'PartY'),'PartY/D. y position of arriving particle at the interface plane (mm)')
  PartTree.Branch('PartZ',AddressOf(part,'PartZ'),'PartZ/D. Distance from IP8 of particle origin (mm)')
  PartTree.Branch('PartDx',AddressOf(part,'PartDx'),'PartDx/D. Cosine of particle angle in the x plane at the interface')
  PartTree.Branch('PartDy',AddressOf(part,'PartDy'),'PartDy/D. Cosine of particle angle in the y plane at the interface')
  PartTree.Branch('PartEk',AddressOf(part,'PartEk'),'PartEk/D. Kinetic energy of particle at the interface plane (GeV)')
  PartTree.Branch('PartDt',AddressOf(part,'PartDt'),'PartDt/D. Particle arrival time at the interface plane w.r.t. LHC clock')
  PartTree.Branch('PartW',AddressOf(part,'PartW'),'PartW/D.  Proabaility of particle arriving at interface plane if the relevant primary proton loss is occurs')
  PartTree.Branch('CombW',AddressOf(part,'CombW'),'CombW/D.  Probability of particle arriving at interface plane. Equal to LossW * PartW')
  PartTree.Branch('Evt',AddressOf(part,'Evt'),'Evt/I. Index of related event')

  # Text file dictionary
  keys = []
  if options.oldformat:
    keys = ['PartPdgId',\
            'LossW',\
            'PartEk',\
            'PartX',\
            'PartY',\
            'PartDx',\
            'PartDy']
  else:
    keys = ['LossId',\
            'LossT0',\
            'LossZ',\
            'LossA',\
            'LossW',\
            'PartPdgId',\
            'PartX',\
            'PartY',\
            'PartZ',\
            'PartDx',\
            'PartDy',\
            'PartEk',\
            'PartDt',\
            'PartW']
  
  # Parse the text file and fill trees
  f = file(infilename)
  partNumber = 0
  inEvtPartNumber = 1
  eventNumber = 0
  lastlist = None
  partlist = {}
  evt.firstPart = partNumber
  evt.sumOfWeights = 0

  # Go through the particle entries
  for i, line in enumerate(f):
    # Positional output
    if i%100000 == 0 and not options.quiet:
      print "Reading line "+str(i)
    
    # Get the data
    rawdata = map(float, line.split())

    # Make into dictionary
    for key, data in zip(keys, rawdata):
      partlist[key] = data

    # Old format needs additional info
    if options.oldformat:
      partlist['LossId'] = i
      partlist['LossT0'] = 0
      partlist['LossZ'] = 0
      partlist['LossA'] = 0
      partlist['PartW'] = 1
      partlist['PartZ'] = 0
      partlist['PartDt'] = 0


    # First line copy for comparison
    if lastlist == None:
      lastlist = partlist.copy()
    
    # Check if event should be stored
    if lastlist['LossId'] != partlist['LossId']:
      EvtTree.Fill()
      lastlist = partlist.copy()
      evt.firstPart = partNumber
      inEvtPartNumber = 1
      evt.sumOfWeights = evt.sumOfWeights + partlist['LossW']
      eventNumber = eventNumber + 1
    
    # Fill info in event
    evt.LossId = partlist['LossId']
    evt.LossZ = partlist['LossZ']
    evt.LossT0 = partlist['LossT0']
    evt.LossW = partlist['LossW']
    evt.LossA = partlist['LossA']
    evt.nParts = inEvtPartNumber

    # Fill info in particle
    part.PartPdgId = partlist['PartPdgId']
    part.PartX = partlist['PartX']
    part.PartY = partlist['PartY']
    part.PartZ = partlist['PartZ']
    part.PartDx = partlist['PartDx']
    part.PartDy = partlist['PartDy']
    part.PartEk = partlist['PartEk']
    part.PartDt = partlist['PartDt']
    part.PartW = partlist['PartW']
    part.CombW = partlist['LossW'] * partlist['PartW']
    part.Evt = eventNumber
    partNumber = partNumber + 1
    inEvtPartNumber = inEvtPartNumber + 1
    PartTree.Fill()

  # Force last event to be written
  EvtTree.Fill()

  # Make histograms
  if options.plot == True:
    histogramfolder = RootF.mkdir('Histograms')
    histogramfolder.cd()
    plots = makePlots(PartTree, EvtTree)

  # Write and close files
  RootF.Write()
  RootF.Close()
  f.close()

from xml.sax import saxutils
class GetHeader(saxutils.handler.ContentHandler):
  def __init__(self):
    self.name = ''
    self.desc = ''
    self.beam = ''
    self.energy = ''
    self.plane = ''
    self.scale = ''
    self.misc = ''
    self.tmp = ''

  def endElement(self, name):
    #Get elements
    if name == 'name': self.name = self.tmp
    if name == 'desc': self.desc = self.tmp
    if name == 'beam': self.beam = self.tmp
    if name == 'energy': self.energy = self.tmp
    if name == 'plane': self.plane = self.tmp
    if name == 'scale': self.scale = self.tmp
    if name == 'misc': self.misc = self.tmp
    self.tmp = ''

  def characters(self, value):
    if self.tmp != '' and value.strip() != '':
      self.tmp = self.tmp+' '
    self.tmp = self.tmp + value.strip()

# Parse the xml file
from xml.sax import make_parser
from xml.sax.handler import feature_namespaces
def parseInfoFile(infofile,headfolder,quiet):
  parser = make_parser()
  parser.setFeature(feature_namespaces, 0)
  dh = GetHeader()
  parser.setContentHandler(dh)
  parser.parse(infofile)

  # Register the values
  headers = []
  name = RooStringVar('Name','Name',dh.name)
  headers.append(name)
  if not quiet: print 'Name:\t\t'+name.getVal()
  desc = RooStringVar('Description','Description',dh.desc)
  headers.append(desc)
  if not quiet: print 'Description:\t'+desc.getVal()
  beam = RooStringVar('Beam','Beam',dh.beam)
  headers.append(beam)
  if not quiet: print 'Beam:\t\t'+beam.getVal()
  energy = RooStringVar('Energy','Energy',dh.energy)
  headers.append(energy)
  if not quiet: print 'Energy:\t\t'+energy.getVal()
  plane = RooStringVar('Plane','Plane',dh.plane)
  headers.append(plane)
  if not quiet: print 'Plane:\t\t'+plane.getVal()
  scale = RooStringVar('Scale','Scale',dh.scale)
  headers.append(scale)
  if not quiet: print 'Scale:\t\t'+scale.getVal()
  misc = RooStringVar('Misc','Misc',dh.misc)
  headers.append(misc)
  if not quiet: print 'Misc:\t\t'+misc.getVal()
  
  for header in headers:
    headfolder.Add(header)
  return headers

from ROOT import TH1D, TH2D
def makePlots(PartTree, EvtTree):
  plots = []

  # Particle plots
  # For all particles
  plots = plots + genPartPlots(PartTree, 'All', 'CombW')
  # For electrons
  plots = plots + genPartPlots(PartTree, 'Elec', 'CombW*(abs(PartPdgId)==11)')
  # For muons
  plots = plots + genPartPlots(PartTree, 'Muon', 'CombW*(abs(PartPdgId)==13)')
  # For gammas
  plots = plots + genPartPlots(PartTree, 'Gamma', 'CombW*(abs(PartPdgId)==22)')
  # For neutrons
  plots = plots + genPartPlots(PartTree, 'Neutr', 'CombW*(abs(PartPdgId)==2112)')
  # For ch. hadrons
  plots = plots + genPartPlots(PartTree, 'ChHad', 'CombW*(abs(PartPdgId)==211 || abs(PartPdgId)==321 || abs(PartPdgId)==2212)')

  # Event plots
  plots = plots + genEvtPlots(EvtTree)

  return plots

def genPartPlots(tree, name, cut):
  plots = []

  # Kinetic energy
  plots.append(TH1D(name+'Ekin','Kinetic Energy of '+name+' (MeV)', 200, 0,50))
  tree.Draw('PartEk >> '+name+'Ekin', cut)

  # Time offset
  plots.append(TH1D(name+'Time','Time Offset of '+name+' (ns)', 200, 0,50))
  tree.Draw('PartDt >> '+name+'Time', cut)
  
  # Origin
  plots.append(TH1D(name+'Orig','Particle Origin of '+name+' (mm)', 200, -200000,0))
  tree.Draw('PartZ >> '+name+'Orig', cut)

  # Radial distribution
  plots.append(TH1D(name+'Rad','Radial Distribution of '+name+' (mm)', 200, 0 ,4000))
  tree.Draw('sqrt(PartX*PartX+PartY*PartY) >> '+name+'Rad', cut)

  # xy distribution
  plots.append(TH2D(name+'XY','XY Distribution of '+name+' (mm)', 200, -4000 ,4000, 200, -4000, 4000))
  tree.Draw('PartX:PartY >> '+name+'XY', cut)

  return plots

def genEvtPlots(tree):
  plots = []

  # Proton loss location
  plots.append(TH1D('ProtonLossOrig','Proton loss location (mm)', 200, -200000,0))
  tree.Draw('LossZ >> ProtonLossOrig', 'LossW')

  # Particle multiplicity per proton loss
  plots.append(TH1D('Multiplicity','Particle Multiplicity per Proton Loss', 200, 0, 200))
  tree.Draw('nParts >> Multiplicity', 'LossW')

  return plots

if __name__=='__main__':main()


