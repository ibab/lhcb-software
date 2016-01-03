#!/usr/bin/env python
# =============================================================================
# @file KaliCalo/MassDistribution/RunMassDistributionCalibration.py
# A way to start the calibration with the Mass Distribution Fit method
# @author Daria Savrina (dsavrina@mail.cern.ch)
# @date 2011-04-15
# =============================================================================
"""
A file which starts the full calibration process with the Mass Distribution Fit
Method.

Usage:
> python RunMassDistributionCalibration.py
"""
## General things
import os, re, random, socket

## Printouts
from AnalysisPython.Logger import getLogger

loggerIt = getLogger('Iterator:')
loggerAn = getLogger('Analyser:')

## Kali modules responsible for filling and fitting the histograms
from KaliCalo.MassDistribution.FillTask import fillDatabase
from KaliCalo.MassDistribution.Analyse import analyse

## Kali. Just Kali
import KaliCalo.Kali as Kali

## Modules to treat the output
## (histograms and calibration constants)
import KaliCalo.Kali.LambdaMap as LambdaMap
import KaliCalo.ZipShelve as zs
from KaliCalo.MassDistribution import HistoMap

## Can be used to join the cells in groups
## in case of low statistics
import KaliCalo.FakeCells as FakeCells
#from RealPrsCoefficients import *
import KaliCalo.Cells     as Cells
Zones = Cells.Zones
AZ = Cells.EcalZone
IZ = Cells.InnerZone
MZ = Cells.MiddleZone
OZ = Cells.OuterZone

## All the paths used during the calibration
## are stored here
#from PyKaliOnline.Paths import Paths, PassIt, MaxIt, loggerAn, loggerIt
from Paths import Paths, PassIt, MaxIt

import sys
sys.path += ['.']

import argparse
parser = argparse.ArgumentParser(description = "Input file delivery")
parser.add_argument("--input-file", type = str, dest = "input_file", nargs = '*', default = None) ## list of input files
parser.add_argument("--index"     , type = int, dest = "index"     , default = None)
parser.add_argument("--process"   , type = str, dest = "process"   , default = 'Fill')
parser.add_argument("--lambdas"   , type = str, dest = "lambdas"   , default = None)
args = parser.parse_args()

input_file = args.input_file
index      = args.index
hn         = socket.gethostname()
pt         = Paths(index, hn)
process    = args.process

print "NUMBER OF INPUT FILES", len(input_file)

## Read the coefficients database
## if there's any
lmap = LambdaMap.LambdaMap()
if args.lambdas: lmap.read(args.lambdas)

StoredHistoPath  = pt.histos_location_it()   ## path to save the filled histogram map
StoredLambdaPath = pt.lambdas_location_it()  ## path to save the filled histogram map
#FittedHistoPath = 'FittedHistograms.gz'     ## path to save the fitted histogram map
LambdasLocation  = pt.lambdas_location()     ## path to the coefficients to be used at the next step
LambdasDBPath    = pt.lambdas_db_location()  ## DBase for re-reconstruction

def FillTheHistograms():
	## ==============================
        ## fill the histograms
	## ==============================
    histos, badfiles = fillDatabase (  ## recieve a map of filled histograms and list of incorrect files
        lmap                           ## map of the calibration coefficients
        , input_file                   ## input nTuples
        , manager = None               ## parallel processing manager
        , cellFunc = FakeCells.SameCell()
        , dbase_name = None
        )

    #hn = socket.gethostname()
    import ROOT
    from KaliCalo.Cells import CellID

    FilledHistoPath = pt.histos_location_an()   ## path to save the filled histogram map
    loggerAn.info('Saving histograms to '+FilledHistoPath)
    histos.save(FilledHistoPath)

    #rootfile = ROOT.TFile(FilledHistoPath,'recreate')
    #
    #for hk in histos.keys():
    #    dname = 'Ecal_%s_%i_%i'%(hk.areaName(),hk.row(),hk.col())
    #    thedir = rootfile.mkdir(dname)
    #    thedir.cd()
    #    for j in range(len(histos[hk])):
    #        histos[hk][j].Write()
    #    thedir.cd()
    #
    #rootfile.Close()

def SplitHistos():
    ## first join the filled histograms from all the workers
    import MergeHistos
    histos = pt.gethistomaps()
    histos.sort()
    print '# of histograms = ',len(histos)

    loggerIt.warning('TRY TO MERGE %d histograms ' % len( histos ))
    sys.stdout.flush()

    hmap = MergeHistos.mergeDBs ( histos  , 60 )

    loggerIt.warning('RESULT Entries:%d Histos:%s' % ( hmap.entries() , len(hmap) ))

    ## find the last one saved and
    ## save current as the next one
    ih   = 1
    Pass = 1
    while True:
        if ih > PassIt/2:
            Pass += 1
            ih    = 1
        if os.path.exists(StoredHistoPath%(Pass,ih)): ## look through all the existing files
            print StoredHistoPath%(Pass,ih), 'exists'
            ih += 1
            continue
        else:                                  ## write to the file next to the latest existing
            print 'Saving histograms to ', StoredHistoPath%(Pass,ih)
            hmap.save(StoredHistoPath%(Pass,ih))
            os.system('chmod a+rwx '+StoredHistoPath%(Pass,ih))
            break

    ## calculate a number of histograms per worker
    nworkers = len(histos)
    nhistos  = len(hmap.keys())
    Nmin = nhistos/nworkers
    Nmax = Nmin+1
    rst = (float(nhistos)/float(nworkers)-Nmin)*nworkers
    rst = int(rst)
    gh = 0
    group_max = HistoMap()
    group_min = HistoMap()

    loggerIt.warning("SPLITTING %s HISTOGRAMS BETWEEN %s WORKERS" %(nhistos,nworkers))
    loggerIt.warning("%s WORKERS WILL HAVE %s HISTOGRAMS PER EACH"%(rst, Nmax))
    loggerIt.warning("%s WORKERS WILL HAVE %s HISTOGRAMS PER EACH"%(nworkers-rst,Nmin))

    for hk in hmap.keys():
        if gh < (rst*Nmax):
            group_max.insert(hmap[hk])
        else:
            group_min.insert(hmap[hk])
        gh += 1

    print len(group_max),"+", len(group_min),"=", len(group_max)+len(group_min), "=", nhistos

    d1 = group_max.split(Nmax)
    d2 = group_min.split(Nmin)
    data = []
    data += d1
    data += d2

    print len(d1),"+",len(d2),"=",len(data),"=",nworkers,"?"

    ## get 'All-Ecal' histograms 
    hA = hmap [ AZ ]
    ## inner area 
    hI = hmap [ IZ ]
    ## middle area 
    hM = hmap [ MZ ]
    ## outer area 
    hO = hmap [ OZ ]

    ## To have the global histograms in each histomap
    #print len(data)
    for dt in data:
        if not hA in dt[0]: dt[0].append(hA)
        if not hI in dt[0]: dt[0].append(hI)
        if not hM in dt[0]: dt[0].append(hM)
        if not hO in dt[0]: dt[0].append(hO)
        #print len(dt[0]),"HISTOGRAMS"

    ## rewrite existing databases by the
    ## parts of the split histo map
    for ih in range(nworkers):
        splitmap = HistoMap()
        hm       = histos[ih]
        print ih, data[ih][0]
        for jh in data[ih][0]:
            splitmap.insert(jh)
        #print "Clearing previous location", hm
        os.remove(hm)
        splitmap.save(hm)

def FitTheHistograms():
    ## ==============================
    ## fit the histograms
    ## ==============================

    hmap = HistoMap()
    hmap.read(input_file[0])

    #print "FITTING THE HISTOGRAMS"
    bad,low,nfit = analyse (         ## recieve a dictionary of the bad, not fit histograms and the ones with low statistics
        hmap              ,          ## map of the filled histograms
        lmap              ,          ## map of the calibration coefficients
        fitted  = False   ,          ## not fitted yet
        manager = None    ,          ## parallel processing manager
        #nHistos = 60                ## number of histograms per core/machine
        )

    LambdasPath     = pt.lambdas_location_an()  ## path to save the calibration coefficients
    print "Saving lambdas to ", LambdasPath
    lmap.save (LambdasPath)

    os.remove(input_file[0])

def CollectLambdas():
    ## all the coefficients databases
    ToCollect = pt.getlambdamaps()
    PrevLams = []
    for i in range(1+MaxIt/PassIt):
        if LambdasLocation%i in ToCollect:
            ToCollect.remove(LambdasLocation%i)
            PrevLams.append(LambdasLocation%i)
    ToCollect.sort()
    PrevLams.sort()

    ## total database for all of them
    lmap_tot  = LambdaMap.LambdaMap()
    if PrevLams: lmap_tot.read(PrevLams[-1]); print PrevLams[-1]
    print PrevLams, len(lmap_tot)

    for ldb in ToCollect:
        print "Adding the coefficients from", ldb
        lmap      = LambdaMap.LambdaMap()
        lmap.read(ldb)
        for k in lmap:
            if lmap_tot.has_key(k):
                lmap_tot[k].append(lmap[k][-1]*lmap_tot[k][-1])
            else:
                lmap_tot[k].append(lmap[k][-1])
        print "Removing", ldb
        os.remove(ldb)

    ## find the last one saved and
    ## save current as the next one
    ih   = 1
    Pass = 1
    while True:
        print ih, PassIt
        if ih > PassIt/2:
            Pass += 1
            ih    = 1
        if os.path.exists(StoredLambdaPath%(Pass,ih)): ## look through all the existing files
            print StoredLambdaPath%(Pass,ih), 'exists'
            ih += 1
            continue
        else:                                  ## write to the file next to the latest existing
            print 'Saving coefficients to ', StoredLambdaPath%(Pass,ih)
            lmap_tot.save(StoredLambdaPath%(Pass,ih))
            os.system('chmod a+rwx '+StoredLambdaPath%(Pass,ih))
            break

    lmap_tot.save(LambdasLocation%Pass)
    print "Saving coefficients to ", LambdasLocation%Pass

    #lmap_tot.save(pt.lambdas_location_it())

    ## Print statistics
    #print 'BAD-CELLS : #', len ( bad  ) 
    #print 'LOW-CELLS : #', len ( low  ) 
    #print 'NFIT-CELLS: #', len ( nfit ) 
    ## print out the statistics of the coefficients variation
    large = lmap_tot.large ( 0.15 )
    print '# of >15% ' , len(large)

    large = lmap_tot.large ( 0.10 )
    print '# of >10% ' , len(large)
    
    large = lmap_tot.large ( 0.05 )
    print '# of > 5% ' , len(large)
    
    large = lmap_tot.large ( 0.02 )
    print '# of > 2% ' , len(large)
    
    large = lmap_tot.large ( 0.01 )
    print '# of > 1% ' , len(large)

    ## create the database for the re-reconstruction
    dbase = zs.open(LambdasDBPath%Pass)
    dbase['ecal'] = lmap_tot.lambdas()
    dbase.close()

def GetOutput():
    ##
    ## Create the resulting histos
    ##
    from KaliCalo.FitUtils import fitPi0, getPi0Params
    from KaliCalo.Cells import CellID
    import ROOT

    nit = PassIt/2

    Conv = {}
    Sigm = {}

    oh = os.path.join(pt.store_location(),"OutputHistograms.root")
    histofile = ROOT.TFile(oh,'recreate')

    for j in range(1,1+MaxIt/PassIt):
        MassInner  = []
        MassMiddle = []
        MassOuter  = []
        Mass       = []
        Histos     = []

        #if j == 2: nit = nit-1

        Conv['Pass%i'%j] = ROOT.TH1F('Conv%i'%j,'Convergency',nit,0,nit)
        Sigm['Pass%i'%j] = ROOT.TH1F('Sigm%i'%j,'Resolution' ,nit,0,nit)
        for i in range(1,nit+1):
        #for i in range(1,nit+1):
            Histos    .append(HistoMap())
            ni = i-1
            Histos[ni].read(StoredHistoPath%(j,i))
            MassInner .append(ROOT.TH1F('MassInnerPass%iIt%i'%(j,i) ,'Inner  mass at Pass%i Iteration N%i'%(j,i), 250, 0, 250))
            MassMiddle.append(ROOT.TH1F('MassMiddlePass%iIt%i'%(j,i),'Middle mass at Pass%i Iteration N%i'%(j,i), 250, 0, 250))
            MassOuter .append(ROOT.TH1F('MassOuterPass%iIt%i'%(j,i) ,'Outer  mass at Pass%i Iteration N%i'%(j,i), 250, 0, 250))
            Mass      .append(ROOT.TH1F('MassPass%iIt%i'%(j,i)      ,'Mass at Pass%i Iteration N%i'%(j,i)       , 250, 0, 250))

            for ck in range(0,3):
                MassInner[ni] .Add(Histos[ni][CellID('Ecal','Inner' ,31,31)][ck])
                MassMiddle[ni].Add(Histos[ni][CellID('Ecal','Middle',31,31)][ck])
                MassOuter[ni] .Add(Histos[ni][CellID('Ecal','Outer' ,31,31)][ck])
            Mass[ni].Add(MassInner[ni])
            Mass[ni].Add(MassMiddle[ni])
            Mass[ni].Add(MassOuter[ni])

            fitPi0(Mass[ni])
            mp = getPi0Params(Mass[ni])
            print "All:",mp
            Conv['Pass%i'%j].SetBinContent(i,mp[1].value())
            Conv['Pass%i'%j].SetBinError  (i,mp[1].error())

            Sigm['Pass%i'%j].SetBinContent(i,mp[2].value())
            Sigm['Pass%i'%j].SetBinError  (i,mp[2].error())

            fitPi0(MassInner[ni])
            mpi = getPi0Params(MassInner[ni])
            print "Inner:", mpi

            fitPi0(MassMiddle[ni])
            print "Middle:",getPi0Params(MassMiddle[ni])

            fitPi0(MassOuter[ni])
            print "Outer:",getPi0Params(MassOuter[ni])

            MassInner[ni].Write()
            MassMiddle[ni].Write()
            MassOuter[ni].Write()
            Mass[ni].Write()

        Conv['Pass%i'%j].Write()
        Sigm['Pass%i'%j].Write()

    histofile.Close()
    os.system('chmod a+rwx '+oh)

    lmap = LambdaMap.LambdaMap()
    lmap.read(pt.lambdas_location_it()%(1,1))

    lmap2 = LambdaMap.LambdaMap()
    lmap2.read(pt.lambdas_location_it()%(MaxIt/PassIt,nit))
    #lmap2.read(pt.lambdas_location_it()%(MaxIt/PassIt,3))

    lams  = lmap.lambdas()
    lams2 = lmap2.lambdas()

    ROOT.gStyle.SetPalette(1)

    It12distr       = ROOT.TH1F('It12distr'      ,'Calibration constants: All   ',100,0.5,1.5)
    It12distrInner  = ROOT.TH1F('It12distrInner' ,'Calibration constants: Inner' ,100,0.5,1.5)
    It12distrMiddle = ROOT.TH1F('It12distrMiddle','Calibration constants: Middle',100,0.5,1.5)
    It12distrOuter  = ROOT.TH1F('It12distrOuter' ,'Calibration constants: Outer' ,100,0.5,1.5)

    It1distr        = ROOT.TH1F('It1distr'       ,'Calibration constants: All'   ,100,0.5,1.5)
    It1distrInner   = ROOT.TH1F('It1distrInner'  ,'Calibration constants: Inner' ,100,0.5,1.5)
    It1distrMiddle  = ROOT.TH1F('It1distrMiddle' ,'Calibration constants: Middle',100,0.5,1.5)
    It1distrOuter   = ROOT.TH1F('It1distrOuter'  ,'Calibration constants: Outer' ,100,0.5,1.5)

    It2distr        = ROOT.TH1F('It2distr'       ,'Calibration constants: All'   ,100,0.5,1.5)
    It2distrInner   = ROOT.TH1F('It2distrInner'  ,'Calibration constants: Inner' ,100,0.5,1.5)
    It2distrMiddle  = ROOT.TH1F('It2distrMiddle' ,'Calibration constants: Middle',100,0.5,1.5)
    It2distrOuter   = ROOT.TH1F('It2distrOuter'  ,'Calibration constants: Outer' ,100,0.5,1.5)

    comp2DFaceInner = ROOT.TH2F('comp2DFaceInner','',48,-970,970,36,-725,725)
    comp2DFaceInner.SetMinimum(0.5)
    comp2DFaceInner.SetMaximum(1.5)
    comp2DFaceMiddle = ROOT.TH2F('comp2DFaceMiddle','',64,-1940,1940,40,-1210,1210)
    comp2DFaceMiddle.SetMinimum(0.5)
    comp2DFaceMiddle.SetMaximum(1.5)
    comp2DFaceOuter = ROOT.TH2F('comp2DFaceOuter','',64,-3880,3880,52,-3150,3150)
    comp2DFaceOuter.SetMinimum(0.5)
    comp2DFaceOuter.SetMaximum(1.5)

    lamsa = {}

    cfs = os.path.join(pt.store_location(),'CalibCoefficients.txt')
    filtxt = open(cfs,'w')
    for ic in lams.keys():
        if not lams2.has_key(ic):
            lams2[ic] = 1.
            print ic
        lamsa[ic] = lams[ic]*lams2[ic]
        print >> filtxt, ic.index(), lamsa[ic]

    for ic2 in lams2.keys():
        if ic2 in lamsa.keys(): continue
        print ic2
        lamsa[ic2] = lams2[ic2]
        print >> filtxt, ic2.index(), lamsa[ic2]
    filtxt.close()

    os.system('chmod a+rwx '+cfs)

    for ic in lamsa.keys():
         if ic.area() == 2:
             #print ic.col(),ic.row()
             comp2DFaceInner.SetBinContent(ic.col()-7,ic.row()-13,lamsa[ic])
             if ic in lams2.keys():
                 It2distr.Fill(lams2[ic])
                 It2distrInner.Fill(lams2[ic])
             if ic in lams.keys():
                 It1distr.Fill(lams[ic])
                 It1distrInner.Fill(lams[ic])
             It12distr.Fill(lamsa[ic])
             It12distrInner.Fill(lamsa[ic])
         if ic.area() == 1:
             #print ic.col(),ic.row()
             comp2DFaceMiddle.SetBinContent(ic.col()+1,ic.row()-11,lamsa[ic])
             if ic in lams2.keys():
                 It2distr.Fill(lams2[ic])
                 It2distrMiddle.Fill(lams2[ic])
             if ic in lams.keys():
                 It1distr.Fill(lams[ic])
                 It1distrMiddle.Fill(lams[ic])
             It12distr.Fill(lamsa[ic])
             It12distrMiddle.Fill(lamsa[ic])
         if ic.area() == 0:
             #print ic.col(),ic.row()
             comp2DFaceOuter.SetBinContent(ic.col()+1,ic.row()-5,lamsa[ic])
             if ic in lams2.keys():
                 It2distr.Fill(lams2[ic])
                 It2distrOuter.Fill(lams2[ic])
             if ic in lams.keys():
                 It1distr.Fill(lams[ic])
                 It1distrOuter.Fill(lams[ic])
             It12distr.Fill(lamsa[ic])
             It12distrOuter.Fill(lamsa[ic])

    It2distr      .Fit("gaus",'','',0.5,1.5)
    It2distrInner .Fit("gaus",'','',0.5,1.5)
    It2distrMiddle.Fit("gaus",'','',0.5,1.5)
    It2distrOuter .Fit("gaus",'','',0.5,1.5)

    It12distr      .Fit("gaus",'','',0.5,1.5)
    It12distrInner .Fit("gaus",'','',0.5,1.5)
    It12distrMiddle.Fit("gaus",'','',0.5,1.5)
    It12distrOuter .Fit("gaus",'','',0.5,1.5)

    It1distr       .Fit("gaus",'','',0.5,1.5)
    It1distrInner  .Fit("gaus",'','',0.5,1.5)
    It1distrMiddle .Fit("gaus",'','',0.5,1.5)
    It1distrOuter  .Fit("gaus",'','',0.5,1.5)


    oc = os.path.join(pt.store_location(),"OutputCoefficients.root")
    fillam = ROOT.TFile(oc,'recreate')
    It2distr        .Write()
    It2distrInner   .Write()
    It2distrMiddle  .Write()
    It2distrOuter   .Write()
    It12distr       .Write()
    It12distrInner  .Write()
    It12distrMiddle .Write()
    It12distrOuter  .Write()
    It1distr        .Write()
    It1distrInner   .Write()
    It1distrMiddle  .Write()
    It1distrOuter   .Write()
    comp2DFaceInner .Write()
    comp2DFaceMiddle.Write()
    comp2DFaceOuter .Write()

    fillam          .Close()

    os.system('chmod a+rwx '+oc)

if '__main__' == __name__ :

    if process == 'Fill':
        print "Filling the histograms"
        FillTheHistograms()
    elif process == 'Fit':
        print "Fitting the histograms"
        FitTheHistograms()
    elif process == 'Split':
        print "Preparing histograms for fitting"
        SplitHistos()
    elif process == 'Collect':
        print "Collecting the calibration constants"
        CollectLambdas()
    elif process == 'Statistics':
        print "Preparing the resulting histograms"
        GetOutput()
    else:
        print 'Please specify the task:'
        print '"Fill" for filling the histograms'
        print '"Fit" for fitting the histograms'
