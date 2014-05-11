#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
## @file FitBasic.py
#
#  Set of useful basic utilities to build various fit models 
#
#  @author Vanya BELYAEV Ivan.Belyaeve@itep.ru
#  @date 2011-07-25
# 
#                    $Revision: 172520 $
#  Last modification $Date: 2014-05-10 20:27:44 +0200 (Sat, 10 May 2014) $
#                 by $Author: ibelyaev $
# =============================================================================
"""
Set of useful basic utilities to build various fit models 
"""
# =============================================================================
__version__ = "$Revision:"
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__    = "2011-07-25"
__all__     = (
    ##
    'makeVar'   , ## helper function to create the proper RooRealVar
    'Mass_pdf'  , ## useful base class to create "signal" PDFs for mass-fits
    'H1D_dset'  , ## convertor of 1D-histo to RooDataHist 
    'H2D_dset'  , ## convertor of 2D-histo to RooDataHist 
    'H1D_pdf'   , ## convertor of 1D-histo to RooHistPdf 
    'H2D_pdf'   , ## convertor of 1D-histo to RooDataPdf
    ##
    'Fit1DBase' , ## useful base class fro 1D-models
    'Fit1D'     , ## the model for 1D-fit: signal + background + optional components  
    'Fit2D'     , ## the model for 2D-fit: signal + background + optional components
    ##
    'Adjust1D'  , ## addjust PDF to avoid zeroes (sometimes useful)
    )
# =============================================================================
import ROOT, math
from   AnalysisPython.Logger     import getLogger
from   PyPAW.PyRoUts             import VE, hID, allInts, cpp, rootID
from   PyPAW.Utils               import RooSilent, NoContext 
# =============================================================================
logger = getLogger ( __name__ ) 
# =============================================================================

# =============================================================================
_nemax = 80000  ## number of events per CPU-core 
_ncmax =     6  ## maximal number of CPUs: there are some problems with >= 7
                ## @see https://sft.its.cern.ch/jira/browse/ROOT-4897
#
_ncpus = []
# 
def ncpu (  events ) :
    #
    #
    ### return  ROOT.RooFit.Save()
    #
    n  = events // _nemax
    if n       <= 1 : return ROOT.RooFit.Save() ## fake!!! 
    # 
    import multiprocessing
    n_cores = multiprocessing.cpu_count()
    if n_cores <= 1 : return ROOT.RooFit.Save () ## fake!!! 
    #
    num = min ( n , n_cores , _ncmax )
    if not _ncpus :
        _ncpus.append ( num )   
    #
    return ROOT.RooFit.NumCPU ( num )

# =============================================================================
## create/modify  the variable
#  Helper function for creation/modification/adjustment of variable
#  @code
#    v = makeVar ( 10   , 'myvar' , 'mycomment' )
#    v = makeVar ( 10   , 'myvar' , 'mycomment' , '' ,     -1 , 1 )
#    v = makeVar ( 10   , 'myvar' , 'mycomment' , '' , 0 , -1 , 1 )
#    v = makeVar ( None , 'myvar' , 'mycomment' , '' , 0 , -1 , 1 )
#    v = makeVar ( None , 'myvar' , 'mycomment' , 10 , 0 , -1 , 1 )
#    v = makeVar ( v    , 'myvar' , 'mycomment' , 10 )
#  @endcode
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2013-12-01
def makeVar ( var , name , comment , fix  , *args ) :
    """
    Make/modify  the variable:
    
    v = makeVar ( 10   , 'myvar' , 'mycomment' )
    v = makeVar ( 10   , 'myvar' , 'mycomment' , '' ,     -1 , 1 )
    v = makeVar ( 10   , 'myvar' , 'mycomment' , '' , 0 , -1 , 1 )
    v = makeVar ( None , 'myvar' , 'mycomment' , '' , 0 , -1 , 1 )
    v = makeVar ( None , 'myvar' , 'mycomment' , 10 , 0 , -1 , 1 )
    v = makeVar ( v    , 'myvar' , 'mycomment' , 10 )
    
    """
    ## create the fixed variable 
    if isinstance   ( var , ( float , int , long ) ) :
        var = ROOT.RooRealVar ( name , comment , var   )
    ## create the variable from parameters 
    if not var                                       :
        var = ROOT.RooRealVar ( name , comment , *args )
    ## fix it if needed 
    if isinstance ( fix , ( float , int , long ) )  and not var.isConstant () :
        if var.getMin() <= fix <= var.getMax() :  var.fix ( fix )
        else : logger.error ( 'Unable to fix %s at %s ' % ( var.GetName() , fix ) ) 
        #
        
    return var



# =============================================================================
## helper base class for implementaiton of various helper pdfs 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2013-12-01
class Mass_pdf(object) :
    """
    helper base class for implementation of various pdfs 
    """
    def __init__ ( self            ,
                   name            ,
                   mn              ,
                   mx              , 
                   mass     = None ,
                   mean     = None ,
                   sigma    = None ,
                   fixMass  = None ,
                   fixSigma = None ) :
        
        #
        ## adjust the mass and edges, create if needed
        #
        self.mass = makeVar ( mass              ,
                              "m_%s"     % name ,
                              "mass(%s)" % name , None ,  min ( mn , mx ) , max( mn , mx ) )
        #
        self._mn = self.mass.getMin ()
        self._mx = self.mass.getMax ()
        #
        _dm      = self._mx - self._mn 
        
        #
        ## mean-value
        # 
        self.mean = makeVar ( mean              ,
                              "mean_%s"  % name ,
                              "mean(%s)" % name , fixMass ,  self._mn  , self._mx )
        #
        if self.mean.isConstant() :
            if not self._mn <= self.mean.getVal() <= self._mx :
                raise AttributeError ( 'Fixed mass is not in mass-range (%s,%s)' % ( self._mn , self._mx ) )
        elif hasattr ( self.mean , 'setMin' ) and hasattr( self.mean , 'setMax' ) : 
            self.mean.setMin ( max ( self.mean.getMin () , self.mass.getMin() - 0.1 * _dm ) )
            self.mean.setMax ( min ( self.mean.getMax () , self.mass.getMax() + 0.1 * _dm ) )
            logger.debug ( 'Mean range is redefined to be (%s,%s)' % ( self.mean.getMin() , self.mean.getMax() ) ) 
            
        #
        ## check the fixed mass
        # 
        if  isinstance ( fixMass , float ) and not self.mean.isConstant() : 
            
            if not self.mass.getMin() <= fixMass <= self.mass.getMax() :
                raise AttributeError ( 'Fixed mass is not in mass-range (%s,%s)' % ( self.mass.getMin() , self.mass.getMax() ) )
            if not self.mean.getMin() <= fixMass <= self.mean.getMax() :
                raise AttributeError ( 'Fixed mass is not in mean-range (%s,%s)' % ( self.mean.getMin() , self.mean.getMax() ) )
            
            self.mean.fix ( fixMass )
            
        #
        ## sigma
        #
        sigma_max  = 1.2 * _dm / math.sqrt ( 12 )
        self.sigma = makeVar ( sigma               ,
                               "sigma_%s"   % name ,
                               "#sigma(%s)" % name , fixSigma , 0 , sigma_max ) 
        #
        
    ## fit 
    def fitTo ( self             ,
                dataset          ,
                draw   = False   ,
                nbins  = 100     ,
                silent = False   ,
                *args , **kwargs ) :
        """
        Perform the fit
        """
        
        context = NoContext () 
        if silent : context = RooSilent() 
        
        with context :
            
            result = self.pdf.fitTo ( dataset , ROOT.RooFit.Save() , *args, **kwargs )
            
            if not draw :
                return result, None
            
            frame = self.mass.frame ( nbins )
            #
            
            dataset  .plotOn ( frame )
            
            self.pdf .plotOn ( frame  , ROOT.RooFit.LineColor  ( ROOT.kRed ) )

            
            frame.SetXTitle('')
            frame.SetYTitle('')
            frame.SetZTitle('')
            
            frame.Draw()
        
        return result, frame 
    
    
    ## fit the histogram 
    def fitHisto ( self , histo , draw = False , silent = False , *args , **kwargs ) :
        """
        Fit the histogram
        """
        
        context = NoContext () 
        if silent : context = RooSilent() 
        
        ## convert it! 
        hdset     = H1D_dset ( '',  histo , self.mass )
        self.hset = hdset.dset
        
        ## fit it! 
        return  self.fitTo ( self.hset     ,
                             draw          ,
                             len ( histo ) ,
                             silent        ,                             
                             *args         ,
                             **kwargs      )
    


# =============================================================================
## simple convertor of 1D-histo to data set
#  @author Vanya Belyaev Ivan.Belyaev@itep.ru
#  @date 2013-12-01
class H1D_dset(object) :
    """
    Simple convertor of 1D-histogram into data set
    """
    def __init__ ( self         ,
                   name         ,
                   histo        ,
                   mass  = None ) :
        #
        ## use mass-variable
        #
        if mass :
            mmin = mass.getMin ()
            mmax = mass.getMax ()
            
        self.mass = makeVar ( mass , 'm_%s' % name , 'mass(%s)' % name , None , *(histo.xminmax()) )

        if mass :
            mass.setMin ( mmin )
            mass.setMax ( mmax )
            
        self.vlst  = ROOT.RooArgList    ( self.mass )
        self.vimp  = ROOT.RooFit.Import ( histo     )
        self.dset  = ROOT.RooDataHist   (
            rootID ( 'hds_' ) ,
            "Data set for histogram '%s'" % histo.GetTitle() ,
            self.vlst  ,
            self.vimp  )

# =============================================================================
## simple convertor of 1D-histogram into PDF
#  @author Vanya Belyaev Ivan.Belyaev@itep.ru
#  @date 2013-12-01
class H1D_pdf(H1D_dset) :
    """
    Simple convertor of 1D-histogram into PDF 
    """
    def __init__ ( self         ,
                   name         ,
                   histo        ,
                   mass  = None ) :
        
        H1D_dset.__init__ ( self , name , histo , mass )
        
        self.vset  = ROOT.RooArgSet  ( self.mass )
        
        #
        ## finally create PDF :
        #
        self.pdf    = ROOT.RooHistPdf (
            'hpdf_%s'            % name ,
            'HistoPDF(%s/%s/%s)' % ( name , histo.GetName() , histo.GetTitle() ) , 
            self.vset  , 
            self.dset  ) 
        
# =============================================================================
## simple convertor of 2D-histo to data set
#  @author Vanya Belyaev Ivan.Belyaev@itep.ru
#  @date 2013-12-01
class H2D_dset(object) :
    """
    Simple convertor of 2D-histogram into data set
    """
    def __init__ ( self         ,
                   name         ,
                   histo2       ,
                   mass  = None ,
                   mass2 = None ) :
        #
        ## use mass-variable
        #
        self.mass  = makeVar ( mass  , 'm_%s'  % name , 'mass (%s)' % name , None , *(histo2.xminmax()) )
        self.mass1 = self.mass 
        self.mass2 = makeVar ( mass2 , 'm2_%s' % name , 'mass2(%s)' % name , None , *(histo2.yminmax()) )
        
        self.vlst  = ROOT.RooArgList    ( self.mass1 , self.mass2 )
        self.vimp  = ROOT.RooFit.Import ( histo2    )
        self.dset  = ROOT.RooDataHist   (
            rootID ( 'hds_' ) ,
            "Data set for histogram '%s'" % histo2.GetTitle() ,
            self.vlst  ,
            self.vimp  )


# =============================================================================
## simple convertor of 2D-histogram into PDF
#  @author Vanya Belyaev Ivan.Belyaev@itep.ru
#  @date 2013-12-01
class H2D_pdf(H2D_dset) :
    """
    Simple convertor of 2D-histogram into PDF 
    """
    def __init__ ( self         ,
                   name         ,
                   histo2       ,
                   mass  = None , 
                   mass2 = None ) :
        
        H2D_dset.__init__ ( name , histo2 , mass , mass2 )
        
        self.vset  = ROOT.RooArgSet  ( self.mass , self.mass2 )
        
        #
        ## finally create PDF :
        #
        self.pdf    = ROOT.RooHistPdf (
            'hpdf_%s'            % name ,
            'HistoPDF(%s/%s/%s)' % ( name , histo2.GetName() , histo2.GetTitle() ) , 
            self.vset  , 
            self.dset  )


        
# =============================================================================
## @class Fit1DBase
#  The base class for implementation of 1D-PDFs
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2011-08-02
class Fit1DBase (object) :
    """
    Useful base class for implementation of generic PDFs for 1D-fit
    """
    def __init__ ( self              ,
                   signalset         ,
                   backgroundset     ) : 
        
        #
        self._signal_set     = signalset
        self._background_set = backgroundset
        #
        self._splots = []

    def signal_set     ( self ) : return self._signal_set
    def background_set ( self ) : return self._background_set
    
    ## make the actual fit (and optionally draw it!)
    def fitTo ( self , dataset , draw = False , nbins = 100 , silent = False , *args ) :
        """
        Perform the actual fit (and draw it)
        """
        context = NoContext () 
        if silent : context = RooSilent() 

        #
        ## define silent context
        #
        with context :
            
            result =  self.pdf.fitTo ( dataset   ,
                                       ROOT.RooFit.Save (   ) ,
                                       ncpu ( len ( dataset ) ) ,
                                       *args     )

        if 0 != result.status() and silent :
            logger.warning ( 'Fit status is %s. Refit in non-silent regime '
                             % result.status() )
            
            return self.fitTo ( dataset , draw , nbins , False , *args )
        
        if hasattr ( self , 'alist2' ) :
            
            nsum = VE()            
            for i in self.alist2 :
                nsum += i.as_VE() 
                if i.getVal() > 0.9 * i.getMax() :
                    logger.warning ( 'Variable %s == %s [close to maximum %s]'
                                     % ( i.GetName() , i.getVal () , i.getMax () ) )
                    
            if not dataset.isWeighted() : 
                nl = nsum.value() - 0.10 * nsum.error()
                nr = nsum.value() + 0.10 * nsum.error()
                if not nl <= len ( dataset ) <= nr :
                    logger.error ( 'Fit is problematic:  sum %s != %s '
                                   % ( nsum , len( dataset ) ) )  
                    
        if not draw :
            return result, None 
        
        #
        ## again the context
        # 
        context = NoContext () 
        if silent : context = RooSilent() 
        
        with context :

            frame = self.mass.frame( nbins )
            #
            dataset  .plotOn ( frame )
            #
            self.pdf .plotOn (
                frame ,
                ROOT.RooFit.Components ( self.background_set () ) ,
                ROOT.RooFit.LineStyle  ( ROOT.kDashed           ) ,
                ROOT.RooFit.LineColor  ( ROOT.kBlue             ) )
            #
            self.pdf .plotOn (
                frame ,
                ROOT.RooFit.Components ( self.signal_set     () ) , 
                ROOT.RooFit.LineWidth  ( 2                      ) ,
                ROOT.RooFit.LineStyle  ( ROOT.kDotted           ) ,
                ROOT.RooFit.LineColor  ( ROOT.kMagenta          ) )
            
            self.pdf .plotOn (
                frame ,
                ROOT.RooFit.LineColor  ( ROOT.kRed              ) )
            
            frame.SetXTitle('')
            frame.SetYTitle('')
            frame.SetZTitle('')
            
            frame.Draw()
            
            return result, frame 

    ## fit the histogram (and draw it)
    def fitHisto ( self , histo , draw = False , silent = False , *args ) :
        """
        Fit the histogram (and draw it)
        """
        context = NoContext () 
        if silent : context = RooSilent() 

        ## convert it! 
        with context : 
            hdset     = H1D_dset ( '',  histo , self.mass )
            self.hset = hdset.dset
            
        ## fit it!!
        return self.fitTo ( self.hset , draw , len ( histo ) , silent , *args )

    ## perform sPlot-analysis 
    def sPlot ( self , dataset , *args    ) : 
        """
        Make sPlot analysis 
        """
        splot = ROOT.RooStats.SPlot ( rootID( "sPlot_" ) ,
                                      "sPlot"            ,
                                      dataset            ,
                                      self.pdf           ,
                                      self.alist2        )
        
        self._splots += [ splot ]
        
        return splot 


# =============================================================================
## @class Fit1D
#  The actual model for 1D-mass fits 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2011-08-02
class Fit1D (Fit1DBase) :
    """
    The actual model for generic 1D-fits 
    """
    def __init__ ( self              , 
                   signal            ,
                   background = None ,
                   components = []   , ## components
                   suffix     = ''   ,
                   bpower     = 0    ) :  
        
        self.signal     =      signal 
        self.mass       = self.signal.mass
        #
        if background : self.background = background 
        else          : self.background = Bkg_pdf  ( 'Background' + suffix ,
                                                     self.mass , power = bpower )

        #
        self.s = makeVar ( None , "S"+suffix , "Signal"     + suffix , None , 0 , 1.e+6 )
        self.b = makeVar ( None , "B"+suffix , "Background" + suffix , None , 0 , 1.e+6 )
        
        self.S = self.s
        self.B = self.b
        
        #
        self.alist1 = ROOT.RooArgList (
            self.signal     .pdf ,
            self.background .pdf 
            )
        # 
        self.alist2 = ROOT.RooArgList (
            self.s  ,
            self.b
            )
        
        self.nums = [ self.s , self.b ]
        
        i = 1

        self.more_components = components
        
        for c in components :
            
            if   isinstance ( c ,  ROOT.RooAbsPdf ) : self.alist1.add ( c     ) 
            elif hasattr    ( c ,'pdf' )            : self.alist1.add ( c.pdf )
            else :
                logger.warning('Unknown component type %s, skip it!' % type(c) )
                continue 
            
            si = makeVar ( None                            ,
                           "S%s_%d"       % ( suffix , i ) ,
                           "Signal(%d)%s" % ( i , suffix ) , None  ,  0  ,  1.e+6 )            
            self.alist2.add  ( si )
            
            setattr ( self , si.GetName() , si ) 
            
            self.nums.append ( si ) 
            i += 1
            
        #
        ## final PDF
        # 
        self.pdf  = ROOT.RooAddPdf  ( "model_"    + suffix ,
                                      "model(%s)" % suffix ,
                                      self.alist1 ,
                                      self.alist2 )
        #
        ## finally initialize the base
        #
        self.ss_ = ROOT.RooArgSet ( self.signal    .pdf )
        self.bs_ = ROOT.RooArgSet ( self.background.pdf ) 
        # 
        Fit1DBase.__init__ ( self , self.ss_ , self.bs_ ) 
        

# =============================================================================
## @class Fit2D
#  The actual model for 2D-fits 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2011-07-25
class Fit2D (object) :
    """
    The actual model for 2D-fits 
    """
    def __init__ ( self              ,
                   #
                   signal_1          , 
                   signal_2          ,
                   suffix = ''       ,
                   #
                   bkg1       = None ,
                   bkg2       = None ,
                   bkgA       = None ,
                   bkgB       = None ,
                   bkg2D      = None ,
                   #
                   power1     = 0    ,
                   power2     = 0    ,
                   powerA     = 0    ,
                   powerB     = 0    ,
                   ## additional components 
                   components = []   ) :
        
        self.signal1   = signal_1
        self.signal2   = signal_2
        
        self.m1        = signal_1.mass
        self.m2        = signal_2.mass

        #
        ## First component: Signal(1) and Signal(2)
        # 
        self.ss_pdf = ROOT.RooProdPdf ( "S1S2pdf" + suffix ,
                                        "Sig(1) x Sig(2)"  ,
                                        self.signal1.pdf   ,
                                        self.signal2.pdf   )
        
        if bkg1 : self.bkg1 = bkg1
        else    : self.bkg1 = Bkg_pdf ( 'Bkg(1)' + suffix , self.m1 , power1 )
        #
        ## Second component: Background(1) and Signal(2)
        # 
        self.bs_pdf = ROOT.RooProdPdf ( "B1S2pdf" + suffix  ,
                                        "Bkg(1) x Sig(2)"   ,
                                        self.bkg1.pdf       ,
                                        self.signal2.pdf    )
        
        if bkg2 : self.bkg2 = bkg2 
        else    : self.bkg2 = Bkg_pdf ( 'Bkg(2)' + suffix , self.m2 , power2 )
        #
        ## Third component:  Signal(1) and Background(2)
        # 
        self.sb_pdf = ROOT.RooProdPdf ( "S1B2pdf" + suffix  ,
                                        "Sig(1) x Bkg(2)"   ,
                                        self.signal1.pdf    ,
                                        self.bkg2.pdf       )
        
        ## 
        self._bkgs = ( bkg1 , bkg2 , bkgA , bkgB ) 
        #
        ## fourth component: Background(1) and Background(2) 
        # 
        if   bkg2D and isinstance ( bkg2D , ROOT.RooAbsPdf ) : self.bb_pdf = bkg2D 
        elif bkg2D and hasattr    ( bkg2D , 'pdf'          ) : self.bb_pdf = bkg2D.pdf
        else     :
            
            if bkgA : self.bkgA = bkgA
            else    : self.bkgA = Bkg_pdf ( 'Bkg(A)' + suffix , self.m1 , powerA )
            if bkgB : self.bkgB = bkgB
            else    : self.bkgB = Bkg_pdf ( 'Bkg(B)' + suffix , self.m2 , powerB )
            
            self.bb_pdf = ROOT.RooProdPdf ( "B1B2pdf" + suffix ,
                                            "Bkg(A) x Bkg(B)"  ,
                                            self.bkgA.pdf      ,
                                            self.bkgB.pdf      )
            
        #
        ## coefficients
        #
        self.ss = makeVar ( None ,
                            "S1S2"          + suffix ,
                            "Sig(1)&Sig(2)" + suffix , None , 1000  , 0 ,  1.e+6 )
        self.sb = makeVar ( None ,
                            "S1B2"          + suffix ,
                            "Sig(1)&Bkg(2)" + suffix , None ,  100  , 0 ,  1.e+6 )
        self.bs = makeVar ( None ,
                            "B1S2"          + suffix ,
                            "Bkg(1)&Sig(2)" + suffix , None ,  100  , 0 ,  1.e+6 )
        self.bb = makeVar ( None ,
                            "B1B2"          + suffix ,
                            "Bkg(1)&Bkg(2)" + suffix , None ,   10  , 0 ,  1.e+6 )

        self.alist1 = ROOT.RooArgList ( self.ss_pdf , self.sb_pdf ,
            self.bs_pdf ,
            self.bb_pdf )
        self.alist2 = ROOT.RooArgList (
            self.ss ,
            self.sb ,
            self.bs ,
            self.bb )

        #
        ## treat additional components (if specified)
        # 
        self.components  = components
        self._cmps       = []
        icmp = 0 
        for cmp in self.components :

            icmp += 1
            
            if   isinstance ( cmp , ROOT.RooAbsPdf         ) : pass 
            elif hasattr    ( cmp , 'pdf'                  ) : cmp = cmp.pdf 
            elif isinstance ( cmp , ( float , int , long ) ) and not isinstance ( cmp , bool ) :
                px  = ROOT.RooPolynomial ( 'Px%d'    % icmp + suffix ,
                                           'Px(%d)'  % icmp + suffix , self.m1 ) 
                py  = ROOT.RooPolynomial ( 'Py%d'    % icmp + suffix ,
                                           'Py(%d)'  % icmp + suffix , self.m2 ) 
                cmp = ROOT.RooProdPdf    ( "Pxy%d"   % icmp + suffix ,
                                           "Pxy(%d)" % icmp + suffix , px , py )  
                self._cmps += [ px,py,cmp]
            else :
                logger.error( 'Unknown type of component %d %d ' % ( imcp , type(cmp) ) )

                
            nn = makeVar ( None ,
                           'Cmp%d'   % icmp + suffix ,
                           'Cmp(%d)' % icmp + suffix ,
                           None ,  100  ,  0 , 1.e+6 )  
            self._cmps += [ nn ]

            self.alist1.add ( cmp )
            self.alist2.add ( nn  )

        #
        ## build final PDF 
        # 
        self.pdf  = ROOT.RooAddPdf  ( "model2D"      + suffix ,
                                      "Model2D(%s)"  % suffix ,
                                      self.alist1 ,
                                      self.alist2 )
        
        self._splots = []

    ## fit 
    def fitTo ( self            ,
                dataset         ,
                draw   = False  ,
                xbins  = 50     ,
                ybins  = 50     ,
                silent = False  , *args ) :
        """
        Perform the fit
        """
        result = self.pdf.fitTo ( dataset                  , 
                                  ROOT.RooFit.Save ()      ,
                                  ncpu ( len ( dataset ) ) ,
                                  *args                    )

        #
        ## keep dataset (for drawing)
        #
        self.dataset = dataset
        
        if hasattr ( self , 'alist2' ) :
            
            nsum = VE()            
            for i in self.alist2 :
                nsum += i.as_VE() 
                if i.getVal() > 0.9 * i.getMax() :
                    logger.warning ( 'Variable %s == %s [close to maximum %s]'
                                     % ( i.GetName() , i.getVal () , i.getMax () ) )
                    
            if not dataset.isWeighted() : 
                nl = nsum.value() - 0.05 * nsum.error()
                nr = nsum.value() + 0.05 * nsum.error()
                if not nl <= len ( dataset ) <= nr :
                    logger.error ( 'Fit is problematic:  sum %s != %s '
                                   % ( nsum , len( dataset ) ) )  
                    
        nbins = xbins 
        if not draw :
            return result,None
        
        return result, self.draw ( None , dataset , nbins , ybins , silent , *args ) 
    
    ## draw the projection over 1st variable 
    def draw1 ( self , dataset = None , nbins = 100 , silent = True , *args ) :
        """Draw the projection over 1st variable"""
        return self.draw ( self.m1 , dataset , nbins , 20    , silent , *args ) 
    ## draw the projection over 12nd variable 
    def draw2 ( self , dataset = None , nbins = 100 , silent = True , *args ) :
        """Draw the projection over 2nd variable"""
        return self.draw ( self.m2 , dataset , 100   , nbins , silent , *args ) 

    ## make 1D-plot 
    def draw ( self           ,
               drawvar = None ,
               dataset = None ,
               nbins   = 100  ,
               ybins   =  20  ,
               silent  = True ,
               *args )  : 
        """
        Make 1D-plot:
        """
        
        context = NoContext () 
        if silent : context = RooSilent()
        
        if not dataset :
            if hasattr ( self , 'dataset' ) : dataset = self.dataset 
            
        with context :
                
            if not drawvar :
                
                _xbins = ROOT.RooFit.Binning ( nbins ) 
                _ybins = ROOT.RooFit.Binning ( ybins ) 
                _yvar  = ROOT.RooFit.YVar    ( self.m2 , _ybins )
                _clst  = ROOT.RooLinkedList  ()
                hdata  = self.pdf.createHistogram ( hID() , self.m1 , _xbins , _yvar )
                hpdf   = self.pdf.createHistogram ( hID() , self.m1 , _xbins , _yvar )
                hdata.SetTitle(';;;')
                hpdf .SetTitle(';;;')
                _lst   = ROOT.RooArgList ( self.m1 , self.m2 )  
                if dataset : dataset.fillHistogram( hdata , _lst ) 
                self.pdf.fillHistogram  ( hpdf , _lst )

                hdata.lego ()
                hpdf .Draw ( 'same surf')
                
                return hpdf , hdata 
            
            frame = drawvar.frame( nbins )
            
            if dataset : dataset  .plotOn ( frame , *args )
            
            self.pdf .plotOn ( frame ,
                               ROOT.RooFit.Components ( self.sb_pdf.GetName() ) ,
                               ROOT.RooFit.LineStyle  ( ROOT.kDashed   ) ,
                               ROOT.RooFit.LineColor  ( ROOT.kGreen    ) , *args )
            
            self.pdf .plotOn ( frame ,
                               ROOT.RooFit.Components ( self.bs_pdf.GetName() ) ,
                               ROOT.RooFit.LineStyle  ( ROOT.kDotted   ) ,
                               ROOT.RooFit.LineColor  ( ROOT.kMagenta  ) , *args )
            
            self.pdf .plotOn ( frame ,
                               ROOT.RooFit.Components ( self.bb_pdf.GetName() ) ,                           
                               ROOT.RooFit.LineWidth  ( 1              ) ,
                               ROOT.RooFit.LineColor  ( ROOT.kBlack    ) , *args )
            
            self.pdf .plotOn ( frame ,
                               ROOT.RooFit.Components ( self.ss_pdf.GetName() ) ,
                               ROOT.RooFit.LineWidth  ( 1              ) ,
                               ROOT.RooFit.LineColor  ( ROOT.kRed      ) , *args )
            
            self.pdf .plotOn ( frame ,
                               ROOT.RooFit.LineColor  ( ROOT.kRed      ) , *args )
            
            frame.SetXTitle ( '' )
            frame.SetYTitle ( '' )
            frame.SetZTitle ( '' )
            
            frame.Draw()
            
            return frame

    ## fit the histogram (and draw it)
    def fitHisto ( self , histo , draw = False , silent = False , *args ) :
        """
        Fit the histogram (and draw it)
        """
        context = NoContext () 
        if silent : context = RooSilent() 
        
        ## convert it! 
        with context : 
            self.hdset = H2D_dset ( '',  histo , self.m1 , self.m2  )
            self.hset  = self.hdset.dset
            
        ## fit it!!
        return self.fitTo ( self.hset      ,
                            draw           ,
                            histo.nbinsx() ,
                            histo.nbinsy() ,
                            silent         , *args ) 
    
    ## make splot-analysis 
    def sPlot ( self     ,
                dataset  ,
                *args    ) : 
        """
        make sPlot analysis 
        """
        splot = ROOT.RooStats.SPlot ( rootID ( "sPlot_" ) ,
                                      "sPlot"             ,
                                      dataset             ,
                                      self.pdf            ,
                                      self.alist2         , *args ) 
        
        self._splots += [ splot ]
        
        return splot 

# =============================================================================
## simple class to adjust certaint PDF to avoid zeroes 
class Adjust1D(object) :
    """
    Simple class to adjust certaint PDF to avoid zeroes 
    """
    ## constructor
    def __init__ ( self          ,
                   name          ,
                   mass          ,
                   pdf           ,
                   value = 1.e-2 ) :
        
        self.mass    = mass
        self.old_pdf = pdf

        self.p0_pdf  = ROOT.RooPolynomial( 'p0_%s'     % name ,
                                           'poly0(%s)' % name , self.mass ) 
        
        self.num_s   = makeVar ( None , 'valueT_%s'    % name ,
                                 'value/true(%s)'      % name ,
                                 None ,
                                 0    , 1.e+6 )
        self.num_v   = makeVar ( None , 'valF_%s'       % name ,
                                 'value/fixtive(%s)'    % name ,
                                 value ,
                                 value , 0 , 10 )  
        
        #
        self.alist1 = ROOT.RooArgList (
            self.old_pdf ,
            self.p0_pdf    
            )
        
        self.alist2 = ROOT.RooArgList (
            self.num_s  ,
            self.num_v
            )
        
        #
        ## final PDF
        # 
        self.pdf  = ROOT.RooAddPdf  ( "adjust_"    + name ,
                                      "Adjust(%s)" % name ,
                                      self.alist1 ,
                                      self.alist2 )
        
        

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
