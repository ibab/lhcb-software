#!/usr/bin/env python
# =============================================================================
# @file FillSelector.py
# @author Albert Puig Navarro (albert.puig@cern.ch)
# @based on Vanya Belyaev's Pi0FillHistos.py
# @date 2010-11-03
# =============================================================================
"""Implementation of FillPi0 for Candidate Minimization method for Pi0 calibration.
"""

__author__ = "Albert Puig (albert.puig@cern.ch)"
__date__ = "$Date$"
__version__ = "$Revision$"

from math import sqrt

import KaliCalo.Kali.FillSelectorBase     as FillBase
from KaliCalo.Cells                       import CellID
from KaliCalo.CandidateMinimization       import HistoMap

class FillPi0( FillBase.FillPi0Base ):
    ## the major method: processing of the tree
    def Process( self, entry ):
        """
        Fills the histograms from a tree
        """
        # == getting the next entry from the tree
        if self.GetEntry ( entry ) <= 0 :
            return 0

        # == for more convenience
        bamboo=self.fChain

        if not self._frequency :
            entries  = bamboo.GetEntries()
            entries  = int ( entries /  50.0 / 10000.0 ) * 10000
            entries  = max ( entries , 100000 ) 
            self._frequency = entries

        # == printout
        if 0 == entry % self._frequency :
            self._print ( 'Process %d' % entry )

        ## 3x3 spd ?? 
        if bamboo.spd1 or bamboo.spd2 :
            return 1

        # == cell (category) number

        ecalCell1 = CellID ( bamboo.ind1 ) 
        ecalCell2 = CellID ( bamboo.ind2 )
        ecalCell1.setCalo ( 2 )
        ecalCell2.setCalo ( 2 )
        
        prsCell1 = CellID ( bamboo.ind1 )
        prsCell2 = CellID ( bamboo.ind2 )
        prsCell1.setCalo ( 1 )
        prsCell2.setCalo ( 2 )
        
        area1 = ecalCell1.area()
        area2 = ecalCell2.area()

        background = bool(bamboo.bkg)

        # "massage" cell-ID, e.g. group them
        ecalCell1 = CellID ( self._cellFunc ( ecalCell1 ) )
        ecalCell2 = CellID ( self._cellFunc ( ecalCell2 ) )
        prsCell1  = CellID ( self._cellFunc ( prsCell1 ) )
        prsCell2  = CellID ( self._cellFunc ( prsCell2 ) )

        ## get the correction coefficients:
        coefEcal1 = self._lambdas [ ecalCell1 ] [ -1 ] ## the last one is relevant here
        coefEcal2 = self._lambdas [ ecalCell2 ] [ -1 ] ## the last one is relevant here 
        coefPrs1  = self._lambdas [ prsCell1 ] [ -1 ] ## the last one is relevant here
        coefPrs2  = self._lambdas [ prsCell2 ] [ -1 ] ## the last one is relevant here 

        if not 0 < bamboo.m12 * self._Unit < 350 :
            return 1                ## RETURN
        if not bamboo.dist > 3                   :
            return 1

        ## get the histograms
        histo1 = self._histos  [ ecalCell1 ].histos()        
        histo2 = self._histos  [ ecalCell2 ].histos()  
        ## get the counters
        counter1 = self._histos  [ ecalCell1 ].counters() 
        counter2 = self._histos  [ ecalCell2 ].counters() 
        ## get 'per-area' histos
        zone1 = CellID ( ecalCell1.calo() , area1 , self._gRow , self._gCol )
        zone2 = CellID ( ecalCell2.calo() , area2 , self._gRow , self._gCol )
        histoZone1    = self._histos  [ zone1 ].histos   () ## global 'per-zone' histos
        histoZone2    = self._histos  [ zone2 ].histos   () ## global 'per-zone' histos
        ## get per-area counters
        counterZone1  = self._histos  [ zone1 ].counters () ## global 'per-zone' counters
        counterZone2  = self._histos  [ zone2 ].counters () ## global 'per-zone' counter
        ## globals
        histoGlobal   = self._global.histos   ()          ## global histos 
        counterGlobal = self._global.counters ()          ## global counters

        ## Determine if we need to feel the prs<10 histogram
        prsLT10 = ( max ( bamboo.prs1 , bamboo.prs2 ) * self._Unit < 10 )

        # Calculate corrected mass
        mass      = bamboo.m12 * self._Unit
        corrMass  = mass
        factor1   = bamboo.prs1 * self._betas[area1] * ( coefPrs1 - coefEcal1) + coefEcal1 * bamboo.g1E
        factor2   = bamboo.prs2 * self._betas[area2] * ( coefPrs2 - coefEcal2) + coefEcal2 * bamboo.g2E
        corrMass *= sqrt( ( factor1 * factor2 ) / ( bamboo.g1E * bamboo.g2E ) )        
        
        # Fill counters
        pionNumber = (1-2*background)
        counter1[0] += pionNumber
        counter2[0] += pionNumber
        counterZone1[0] += pionNumber
        counterZone2[0] += pionNumber
        counterGlobal[0] += pionNumber
        if prsLT10:
          counter1[1] += pionNumber
          counter2[1] += pionNumber
          counterZone1[1] += pionNumber
          counterZone2[1] += pionNumber
          counterGlobal[1] += pionNumber
          
        if corrMass > 250 :
            return 1 
        # Fill signal histo
        hID = int( background ) * 2
        histo1[ hID ].Fill ( corrMass )
        histo2[ hID ].Fill ( corrMass )
        histoZone1[ hID ].Fill ( corrMass )
        histoZone2[ hID ].Fill ( corrMass )
        histoGlobal[ hID ].Fill ( corrMass )
        # Fill subtracted
        histo1[ 4 ].Fill ( corrMass , pionNumber )
        histo2[ 4 ].Fill ( corrMass , pionNumber )
        histoZone1[ 4 ].Fill ( corrMass , pionNumber )
        histoZone2[ 4 ].Fill ( corrMass , pionNumber )
        histoGlobal[ 4 ].Fill ( corrMass , pionNumber )
        # Fill prs<10 if necessary
        if prsLT10:
            histo1[ hID+1 ].Fill ( corrMass )
            histo2[ hID+1 ].Fill ( corrMass )
            histoZone1[ hID+1 ].Fill ( corrMass )
            histoZone2[ hID+1 ].Fill ( corrMass )
            histoGlobal[ hID+1 ].Fill ( corrMass )
            histo1[ 5 ].Fill ( corrMass , pionNumber )
            histo2[ 5 ].Fill ( corrMass , pionNumber )
            histoZone1[ 5 ].Fill ( corrMass , pionNumber )
            histoZone2[ 5 ].Fill ( corrMass , pionNumber )
            histoGlobal[ 5 ].Fill ( corrMass , pionNumber )
            
        return 1
    
fillDataBase = FillBase.FillDatabase( FillPi0 , HistoMap )

# EOF
