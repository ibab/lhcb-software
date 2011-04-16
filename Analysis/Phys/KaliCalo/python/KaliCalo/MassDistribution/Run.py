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
from KaliCalo.MassDistribution.FillTask import fillDatabase
from KaliCalo.MassDistribution.Analyse import analyse
    
import KaliCalo.Kali as Kali
import KaliCalo.Kali.LambdaMap as LambdaMap
import KaliCalo.ZipShelve as ZipShelve 
import KaliCalo.FakeCells as FakeCells
import os

import sys
sys.path += ['.']

## input Kali nTuples
castor = 'castor:/castor/cern.ch/user/d/dsavrina/1759/'
pattern  =  '%d/outputdata/KaliPi0_Tuples_2010test.root'
files = [ castor+pattern%i for i in range(0,6) ] ## there are eleven files

FilledHistoPath = 'FilledHistograms%i.gz'        ## path to save the filled histogram map
FittedHistoPath = 'FittedHistograms%i.gz'        ## path to save the fitted histogram map
LambdasPath = 'Lambdas%i.gz'                     ## path to save the calibration coefficients
ProblemsPath = 'ProblematicCells%i.gz'           ## path to save the problematic cells info

if '__main__' == __name__ :
    
    ## setup parallel processing
    from GaudiMP.Parallel import WorkManager
    manager = WorkManager( ppservers = () , ncpus = 7 )
    
    lambdas  = LambdaMap.LambdaMap()
    fakeCell = FakeCells.SameCell ()  ## use 
    
    deltas  = [] 

    import sys
    
    output = sys.stdout
    
    ## make 10 "primary" iterations
    for i in range ( 0 , 10 ) :
        
        output.flush()
        
	## ==============================
        ## fill the histograms
	## ==============================
        histos, badfiles = fillDatabase (  ## recieve a map of filled histograms and list of incorrect files
            lambdas                        ## map of the calibration coefficients
            , files                        ## input nTuples
            , manager = manager            ## parallel processing manager
            )

	## save the filled histograms
        print 'SAVE local_zdb.gz'
        if os.path.exists ( FilledHistoPath%i ) :
            os.remove ( FilledHistoPath%i )
        histos.save (FilledHistoPath%i)
        
        print 'HISTOS', len(histos), histos.entries()
        output.flush()
        
	## ==============================
        ## fit the histograms
	## ==============================
        bad,low,nfit = analyse (         ## recieve a dictionary of the bad, not fit histograms and the ones with low statistics
            histos            ,          ## map of the filled histograms
            lambdas           ,          ## map of the calibration coefficients
            fitted  = False   ,          ## not fitted yet
            manager = manager ,          ## parallel processing manager
            nHistos = 60                 ## number of histograms per core/machine
            ) 
        
	## save the fitted histograms
        print 'SAVE fit_zdb.gz'
        if os.path.exists ( FittedHistoPath%i ) :
            os.remove ( FittedHistoPath%i )
        histos.save (FittedHistoPath%i)
            
	## save the obtained coefficients
        print 'SAVE lambdas_zdb.gz'
        lambdas.save ( LambdasPath%i)
        lambdas.save ( LambdasPath%i )

        ## Report problematic cells:
        dbase = ZipShelve.open ( ProblemsPath%i )

        ## Print out and save the problematic cells
        print 'BAD-CELLS : #', len ( bad  ) 
        dbase['BadCells' ] = bad
        for b in bad  :            
            print 'BadCell : ', b , histos[b].entries()
            key = 'BadCell ' + str(b)
            dbase [key] = histos[b]            
        
        print 'LOW-CELLS : #', len ( low  ) 
        dbase['LowCells' ] = low
        for b in low  :
            print 'LowCell : ', b , histos[b].entries()
            key = 'LowCell ' + str(b)
            dbase [key] = histos[b]
                
        print 'NFIT-CELLS: #', len ( nfit ) 
        dbase['NFitCells'] = nfit
        for b in nfit :
            print 'nFitCell: ', b , histos[b].entries()
            key = 'nFitCell ' + str(b)
            dbase [key] = histos[b]


        ## print out the statistics of the coefficients variation
        large = lambdas.large ( 0.15 )
        print '# of >15% ' , len(large)
        dbase['Large_15%'] = large

        large = lambdas.large ( 0.10 )
        print '# of >10% ' , len(large)
        dbase['Large_10%'] = large
        
        large = lambdas.large ( 0.05 )
        print '# of > 5% ' , len(large)
        dbase['Large_5%'] = large
        
        large = lambdas.large ( 0.02 )
        print '# of > 2% ' , len(large)
        dbase['Large_2%'] = large

        large = lambdas.large ( 0.01 )
        print '# of > 1% ' , len(large)

        print 'DB keys: '
        for key in dbase : print key
        dbase.close()
        
        output.flush()
