#!/usr/bin/env python
#############################################################################
# makeRootFileFromNewFormat.py                                              #
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
# Usage:                                                                    #
# python makeRootFileFromNewFormat.py <inputTextFile> <nameOfRootFile>      #
#############################################################################
import os, sys, re
from ROOT import TTree, TFile, AddressOf, gROOT

def usage():
  print 'python makeRootFileFromNewFormat.py <inputTextFile> <nameOfRootFile>'

def main():
  # Start by checking the arguments
  if len(sys.argv) != 3:
    print 'ERROR: Exactly two arguments required'
    usage()
    sys.exit()
  
  # Does the input file exist?
  inputpath = ''
  if os.path.exists(os.path.join(os.path.curdir,sys.argv[1])):
    inputpath = os.path.join(os.path.curdir,sys.argv[1])
  elif os.path.exists(sys.argv[1]):
    inputpath = sys.argv[1]
  else:
    print 'ERROR: Input file "'+sys.argv[1]+'" not found'
    usage()
    sys.exit()

  # Verify the output ROOT file name
  namecheck = re.compile('^\S*.root$')
  if namecheck.match(sys.argv[2]) == None:
    print 'ERROR: Please provide a valid ROOT file name (ending with ".root")'
    usage()
    sys.exit()

  # Make the ROOT file and trees
  RootF = TFile(sys.argv[2],'RECREATE')
  EvtTree = TTree('Events','Event Tree')
  PartTree = TTree('Particles','Particle Tree')
  
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

  # Text file dictionary
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
  f = file(inputpath)
  partNumber = 0
  inEvtPartNumber = 1
  lastlist = None
  partlist = {}
  evt.firstPart = partNumber
  evt.sumOfWeights = 0

  # Go through the particle entries
  for i, line in enumerate(f):
    # Positional output
    if i%100000 == 0:
      print "Reading line "+str(i)
    
    # Get the data
    rawdata = map(float, line.split())

    # Make into dictionary
    for key, data in zip(keys, rawdata):
      partlist[key] = data

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
    partNumber = partNumber + 1
    inEvtPartNumber = inEvtPartNumber + 1
    PartTree.Fill()

  # Force last event to be written
  EvtTree.Fill()

  # Write and close files
  RootF.Write()
  RootF.Close()
  f.close()

if __name__=='__main__':main()


