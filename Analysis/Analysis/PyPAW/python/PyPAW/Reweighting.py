#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# $Id$
# =============================================================================
## @file
#  Module with utilities for reweigting 
#
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2014-05-10
#  
#                    $Revision$
#  Last modification $Date$
#  by                $Author$
# =============================================================================
"""
Module with utilities for reweighting 
"""
# =============================================================================
__version__ = "$Revision$"
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__    = "2011-06-07"
__all__     = (
    'Weight'      ,
    'makeWeights' ,
    ) 
# =============================================================================
import ROOT
from   PyPAW.PyRoUts   import cpp, VE,SE,iszero,hID   
import PyPAW.ZipShelve as     ZipShelve  ## needed to store the weights&histos 
# =============================================================================
# logging 
# =============================================================================
from AnalysisPython.Logger import getLogger 
logger = getLogger( __name__ )
# =============================================================================
logger.info ( 'Set of utitilities for re-weigthing')
# =============================================================================
## @class Weight
#  helper class for semiautomatic reweighting of data 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2014-05-10
class Weight(object) :
    """
    Helper class for semi-automatic reweighting of data 
    """
    def __init__ ( self                   ,
                   dbase   = "weights.db" , ## the name of data base with the weights 
                   factors = []           ) :
        
        #
        ## make some statistic
        #
        self._counter = SE ()
        self._nzeroes = 0 

        self.vars = [] 
        if not factors : return

        ## open database 
        with ZipShelve.open ( dbase , 'r' ) as db : ## READONLY
            
            ## db.ls ()
            for k in db :
                logger.info( 'DBASE "%.15s" key "%.15s" #%d' % ( dbase ,  k, len( db[k] ) ) ) 
                
            ## loop over the weighting factors and build the function
            for f in factors :

                funval  = f[0]  ## accessor to the variable 
                funname = f[1]  ## address  in database 

                ## 
                functions  = db.get ( funname , [] ) ## db[ funname ]
                if not functions :
                    logger.warning('No reweighting is availabe for %s, skip it' % funname )

                merge = True
                if 2 < len ( f ) : merge = f[2] 

                ## merge list of functions into single function 
                if merge and functions : 
                
                    single_func = functions[0]
                
                    for fun in functions [ 1: ] : single_func *= fun
                
                    functions  = [ single_func ]
                    
                self.vars += [ ( funname , funval , functions ) ]  


    ## get the statistic of weights 
    def stat    ( self ) :
        "Get the statistic of used weights"
        return self._counter
    ## number of zero weights 
    def nZeroes ( self ) :
        "Number of null weights"
        return self._nzeroes
    
    ## calculate the weight for the given "event"
    def __call__ ( self , s ) :
        """
        Calculate the weigth for the given ``event''
        (record in TTree/TChain or RooDataSet)
        
        """

        ## initialize the weight 
        weight  = VE(1,0) 

        ## lop over functions 
        for i in self.vars :
            
            funval    = i[1] ## accessor 
            functions = i[2] ## the functions 

            ##  get the valuet for given event 
            v       = funval ( s )
            
            for f in functions :

                if isinstance ( v , tuple ) : w = f ( *v )
                else                        : w = f (  v )

                weight *= w # update the weigth factor 

                
        vw = weight.value()
        
        self._counter += vw 
        if iszero ( vw ) : self._nzeroes += 1
            
        return vw 

# =============================================================================
## make one re-weighting iteration 
#  and reweight "MC"-data set to looks as "data"(reference) dataset
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2014-05-10
def makeWeights  ( dataset                 ,
                   plots    = []           ,
                   database = "weights.db" ,
                   compare  = None         ,
                   delta    = 0.001        ) : ## delta for weigth variance 

    more = False 
    ## loop over plots 
    for r in plots  :

        what    = r [0]         ## variable/function to plot/compare 
        how     = r [1]         ## weight or additional cuts 
        address = r [2]         ## address in database 
        hdata0  = r [3]                          .Clone ( hID () ) ## original "DATA" histogram
        hmc0    = r [4] if 4 < len(r) else hdata0.Clone ( hID () ) ## original "MC"   histogram 

        #
        ## black magic to take into account the difference in bins and normalizations
        # 
        hdata = hdata0.rescale_bins ( 1.0   )
        ## hdata . scale               ( 1.0   )
        
        hmean  = hdata.mean()             ## normalization point
        #
        if isinstance ( hdata , ROOT.TH2 ) : hdata /= hdata ( *hmean )
        else                               : hdata /= hdata (  hmean )

        #
        ## make a plot on (MC) data with the weight
        # 
        dataset.project ( hmc0 , what , how )
        
        #
        ## black magic to take into account the difference in bins and normalizations
        # 
        hmc = hmc0.rescale_bins ( 1.0 )
        ## hmc.scale               ( 1.0 )
        
        #
        if isinstance ( hmc , ROOT.TH2 ) : hmc /= hmc ( *hmean )
        else                             : hmc /= hmc (  hmean )

        #
        ## calculate  the reweigting factor : a bit conservative
        #  this is the only important line 
        w = ( ( 1.0 / hmc ) * hdata ) ** ( 1.0 / ( len ( plots ) ) )  

        #
        ## 
        # 
        save = True
        # rcnt = w.is_constant ( 0.001 , 1000 , 0.95 , delta = 0.001  )
        # if rcnt : save = False
        
        #
        ## get the statistics of weights 
        #
        cnt  = w.stat()
        mnmx = cnt.minmax()
        if not mnmx [0] <= 1 <= mnmx[1] : w /= cnt.mean().value()
        cnt  = w.stat()
        #
        wvar = cnt.rms()/cnt.mean()
        logger.info ( 'Reweighting "%-.15s: Mean/rms/minmax:%s/%.4f/(%.4f,%.4f) Vars:%s[%%]' %
                      ( address    ,
                        cnt.mean() , cnt.rms(),
                        cnt.minmax()[0] ,
                        cnt.minmax()[1] , wvar * 100 ) ) 
        #
        ## make decision based on variance of weights 
        # 
        if wvar.value() <= delta :
            save = False
            logger.info("No more reweighting for %s [%.3f%%]" %  ( address , wvar * 100 ) ) 
        else            :
            save = True 

        #
        ## make a comparison (if needed)
        # 
        if compare :
            compare ( hdata0 , hmc0 , address )
        
        
        ## update dat abase 
        if save and database and address :
            with ZipShelve.open ( database ) as db :
                db[address] = db.get( address , [] ) + [ w ]

        ## 
        more = more or save

        del hdata0, hmc0, hdata, hmc, w  
        
    return more

## return (hdata0,hmc0),(hdata,hmc) 

## some simple comparsion 
def hCompare ( data , mc , title = '' , spline = True ) :

    
    if not isinstance ( data , ( ROOT.TH1D , ROOT.TH1F ) ) : return
    if not isinstance ( mc   , ( ROOT.TH1D , ROOT.TH1F ) ) : return

    data.cmp_prnt( mc )

    hd  = data.rescale_bins ( 1 ) 
    hm  = mc  .rescale_bins ( 1 ) 

        
# =============================================================================
if '__main__' == __name__ :
    
    print 80*'*'
    print __doc__
    print ' Author  : ' , __author__
    print ' Version : ' , __version__
    print ' Date    : ' , __date__    
    print ' Symbols : ' , __all__    
    print 80*'*'
    
# =============================================================================
# The END 
# =============================================================================
