#!/usr/bin/env python
# =============================================================================
# @file FillSelector.py
# @author Albert Puig Navarro (albert.puig@cern.ch)
# @based on Vanya Belyaev's Pi0FillHistos.py
# @date 2010-11-03
# =============================================================================
"""Implementation of FillSelector for Mass Distribution method for Pi0 calibration.
"""

__author__ = "Albert Puig (albert.puig@cern.ch)"
__date__ = "$Date$"
__version__ = "$Revision$"

__all__ = [ 'FillPi0' , 'fillDataBase' ]

from math import sqrt

import KaliCalo.Kali.FillSelectorBase as FillBase

from KaliCalo.MassDistribution import HistoMap
from KaliCalo.Cells            import CellID

class FillPi0( FillBase.FillPi0Base ):
    def Process( self, entry ):
        """
        Fills the histograms from a tree
        """
        # == getting the next entry from the tree
        if self.GetEntry ( entry ) <= 0 : return 0
      
        # == for more convenience
        bamboo=self.fChain
      
        if not self._frequency :
            entries  = bamboo.GetEntries()
            entries  = int ( entries /  50.0 / 10000.0 ) * 10000
            entries  = max ( entries , 100000 ) 
            self._frequency = entries
          
        # == printout
        if 0 == entry % self._frequency:
            self._print ( 'Process %d' % entry )
          
        ## 3x3 spd ?? 
        ## if bamboo.spd1 and bamboo.spd2 : return 1
        if bamboo.spd1 or bamboo.spd2 : return 1
      
        # == cell (category) number
      
        ic1 = CellID ( bamboo.ind1 ) 
        ic2 = CellID ( bamboo.ind2 )
      
        ic1.setCalo ( 2 )
        ic2.setCalo ( 2 )

        area1 = ic1.area()
        area2 = ic2.area()

        background = 1 == bamboo.bkg

        if background and not self._background : return 1 

        #
        # "massage" cell-ID, e.g. group them
        # 
        ic1 = CellID ( self._cellFunc ( ic1 ) ) 
        ic2 = CellID ( self._cellFunc ( ic2 ) ) 

        ## get the correction coefficients:
        lam1 = self._lambdas [ ic1 ] [ -1 ] ## the last one is relevant here
        lam2 = self._lambdas [ ic2 ] [ -1 ] ## the last one is relevant here 

        # == caculating the corrected mass
        corrMass = bamboo.m12 * sqrt( lam1 * lam2 ) * self._Unit
        if not 0 < corrMass < 350 : return 1                ## RETURN

        ## get the histograms for the first cell 
        hs1  = self._histos  [ ic1 ].histos()        
        ## get the histograms and for the first cell 
        hs2  = self._histos  [ ic2 ].histos()  
      
        hc1  = self._histos  [ ic1 ].counters()        
        hc2  = self._histos  [ ic2 ].counters()        


        ## get 'per-area' histos
        iz1 = CellID ( ic1.calo() , area1 , self._gRow , self._gCol )
        iz2 = CellID ( ic2.calo() , area2 , self._gRow , self._gCol )
        hz1  = self._histos  [ iz1 ].histos   () ## global 'per-zone' histos
        hz2  = self._histos  [ iz2 ].histos   () ## global 'per-zone' histos
        cz1  = self._histos  [ iz1 ].counters () ## global 'per-zone' histos
        cz2  = self._histos  [ iz2 ].counters () ## global 'per-zone' histos
        ## everything 
        hgl  = self._global.histos   ()          ## global histos 
        cgl  = self._global.counters ()          ## global counters
      
        ## 

        ok1 = False 
        ok2 = False 
        ok3 = False
      
        if   max ( bamboo.prs1 , bamboo.prs2 ) * self._Unit < 10 : ok1 = True 
        elif min ( bamboo.prs1 , bamboo.prs2 ) * self._Unit > 10 : ok3 = True
        else:
            ok2 = True 

      
        if ok2 :
            corrMass  = bamboo.m12 * self._Unit
            factor2   = self._betas[area2] * bamboo.prs2/bamboo.g2E
            corrMass *= sqrt ( lam1 )
            corrMass *= sqrt ( lam2 + ( 1 - lam2 ) * factor2 )
            if abs ( corrMass - 135. ) < 20 :
                hc2[0]   += factor2
                cz2[0]   += factor2
                cgl[0]   += factor2
              
        elif ok3 :
            corrMass  = bamboo.m12 * self._Unit
            factor1   = self._betas[area1] * bamboo.prs1/bamboo.g1E
            factor2   = self._betas[area2] * bamboo.prs2/bamboo.g2E
            corrMass *= sqrt ( lam1 + ( 1 - lam1 ) * factor1 )
            corrMass *= sqrt ( lam2 + ( 1 - lam2 ) * factor2 ) 
            if abs ( corrMass - 135. ) < 20 :
                hc1[1]   += factor1 
                cz1[1]   += factor1
                cgl[1]   += factor1                
                hc2[1]   += factor2
                cz2[1]   += factor2
                cgl[1]   += factor2                

        if corrMass > 250  : return 1 

        if not background :

            if   ok1 : 
                hs1 [ 0 ].Fill ( corrMass )
                hs2 [ 0 ].Fill ( corrMass )
                hz1 [ 0 ].Fill ( corrMass )
                hz2 [ 0 ].Fill ( corrMass )
                hgl [ 0 ].Fill ( corrMass )
            elif ok2 :
                hs1 [ 1 ].Fill ( corrMass )
                hz1 [ 1 ].Fill ( corrMass )
                hgl [ 1 ].Fill ( corrMass )
            elif ok3 :
                hs1 [ 2 ].Fill ( corrMass )
                hs2 [ 2 ].Fill ( corrMass )
                hz1 [ 2 ].Fill ( corrMass )
                hz2 [ 2 ].Fill ( corrMass )
                hgl [ 2 ].Fill ( corrMass )
              
        else :

            if   ok1 : 
                hs1 [ 3 ].Fill ( corrMass )
                hs2 [ 3 ].Fill ( corrMass )
                hz1 [ 3 ].Fill ( corrMass )
                hz2 [ 3 ].Fill ( corrMass )
                hgl [ 3 ].Fill ( corrMass )
            elif ok2 : 
                hs1 [ 4 ].Fill ( corrMass )
                hz1 [ 4 ].Fill ( corrMass )
                hgl [ 4 ].Fill ( corrMass )
            elif ok3 : 
                hs1 [ 5 ].Fill ( corrMass )
                hs2 [ 5 ].Fill ( corrMass )
                hz1 [ 5 ].Fill ( corrMass )
                hz2 [ 5 ].Fill ( corrMass )
                hgl [ 5 ].Fill ( corrMass )
      
        return 1

fillDataBase = FillBase.FillDatabase( FillPi0 , HistoMap )

# EOF
