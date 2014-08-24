#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# $Id$
# =============================================================================
## @file FitSpeicifModels.py
#
#  A set of predefined ready-to-use shapes and PDFs 
#
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2011-07-25
# 
#                    $Revision$
#  Last modification $Date$
#                 by $Author$
# =============================================================================
"""A set of predefined ready-to-use shapes and PDFs"""
# =============================================================================
__version__ = "$Revision:"
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__    = "2011-07-25"
__all__     = (
    #
    'D0_pdf'  , ## PDF for D0        : Bukin 
    'Dp_pdf'  , ## PDF for D+        : Bukin
    'Ds_pdf'  , ## PDF for Ds+       : Bukin 
    'Lc_pdf'  , ## PDF for Lambda_c+ : Gauss
    #
    'Bd_pdf'  , ## pdf for B0        : double-sided Crystal Ball 
    'B0_pdf'  , ## pdf for B0        : double-sided Crystal Ball 
    'Bu_pdf'  , ## pdf for B+        : double-sided Crystal Ball 
    'Bs_pdf'  , ## pdf for Bs        : double-sided Crystal Ball 
    'Bc_pdf'  , ## pdf for Bc+       : double-sided Crystal Ball 
    #
    'Manca_pdf'  , ## Manca function to fit Y->mu mu spectrum  [Y(1S),Y(2S),Y(3S)]
    'Manca2_pdf' , ## Manca function to fit Y->mu mu spectrum  [Y(1S),Y(2S),Y(3S)]
    #
    )
# =============================================================================
import ROOT, math
from   Ostap.PyRoUts             import VE,cpp
from   GaudiKernel.SystemOfUnits import GeV 
# =============================================================================
from   AnalysisPython.Logger     import getLogger
if '__main__' ==  __name__ : logger = getLogger ( 'Ostap.FitSpecificModels' )
else                       : logger = getLogger ( __name__     )
# =============================================================================
# Specializations of double-sided Crystal Ball function 
# =============================================================================
from   Ostap.FitBasic            import PDF
from   Ostap.FitSignalModels     import CB2_pdf
# =============================================================================
## @class Bd_pdf
#  simple wrapper over CB2-pdf
#  @see Analysis::Models::CrystalBallDS
#  @see Gaudi::Math::CrystalBallDS
#  @attention: mass is mandatory variable! 
#  @author Vanya BELYAEV Ivan.Belyaeve@itep.ru
#  @date 2011-07-25
class Bd_pdf(CB2_pdf) :
    """
    Define PDF for Bd: Double sided Crystall Ball
    
    Attention: mass is mandatory argument!
    
    """
    def __init__ ( self                   ,
                   mass                   ,   ## mass is mandatory here! 
                   name      = 'Bd'       ,
                   mean      = 5.2791e+00 ,   ## to be released later 
                   sigma     = 7.2938e-03 ,   ## to be released later 
                   alphaL    = 1.4499e+00 ,   ## to be released later 
                   alphaR    = 1.9326e+00 ,   ## to be released later
                   nL        = 8.7234e+00 ,   ## to be released later 
                   nR        = 2.0377e+00 ) : ## to be released later 
        ## 
        CB2_pdf.__init__ ( self             ,
                           name             ,
                           mass.getMin()    ,
                           mass.getMax()    ,
                           mass             , 
                           mean             ,
                           sigma            ,
                           alphaL           ,
                           alphaR           ,
                           nL               ,
                           nR               )

B0_pdf = Bd_pdf 
# =============================================================================
## @class Bu_pdf
#  simple wrapper over CB2-pdf
#  @see Analysis::Models::CrystalBallDS
#  @see Gaudi::Math::CrystalBallDS
#  @author Vanya BELYAEV Ivan.Belyaeve@itep.ru
#  @date 2011-07-25
class Bu_pdf(CB2_pdf) :
    """
    Define PDF for B+: Double sided Crystall Ball 
    """
    def __init__ ( self                   ,
                   mass                   ,   ## mass is mandatory here! 
                   name      = 'Bu'       ,
                   mean      = 5.2791e+00 ,   ## to be released later 
                   sigma     = 7.2938e-03 ,   ## to be released later 
                   alphaL    = 1.4499e+00 ,   ## to be released later 
                   alphaR    = 1.9326e+00 ,   ## to be released later
                   nL        = 8.7234e+00 ,   ## to be released later 
                   nR        = 2.0377e+00 ) : ## to be released later 
        ## 
        CB2_pdf.__init__ ( self             ,
                           name             ,
                           mass.getMin()    ,
                           mass.getMax()    ,
                           mass             , 
                           mean             ,
                           sigma            ,
                           alphaL           ,
                           alphaR           ,
                           nL               ,
                           nR               )

# =============================================================================
## @class Bs_pdf
#  simple wrapper over CB2-pdf
#  @see Analysis::Models::CrystalBallDS
#  @see Gaudi::Math::CrystalBallDS
#  @author Vanya BELYAEV Ivan.Belyaeve@itep.ru
#  @date 2011-07-25
class Bs_pdf(CB2_pdf) :
    """
    Define PDF for Bs: Double sided Crystall Ball 
    """
    def __init__ ( self                   ,
                   mass                   ,    ## mass is mandatory here! 
                   name      = 'Bs'       ,
                   mean      = 5.3661e+00 ,    ## to be released later 
                   sigma     = 7.2938e-03 ,    ## to be released later 
                   alphaL    = 1.4499e+00 ,    ## to be released later 
                   alphaR    = 1.9326e+00 ,    ## to be released later
                   nL        = 8.7234e+00 ,    ## to be released later 
                   nR        = 2.0377e+00 ) :  ## to be released later 
        ## 
        CB2_pdf.__init__ ( self             ,
                           name             ,
                           mass.getMin()    ,
                           mass.getMax()    ,
                           mass             , 
                           mean             ,
                           sigma            ,
                           alphaL           ,
                           alphaR           ,
                           nL               ,
                           nR               )

# =============================================================================
## @class Bc_pdf
#  simple wrapper over CB2-pdf
#  @see Analysis::Models::CrystalBallDS
#  @see Gaudi::Math::CrystalBallDS
#  @author Vanya BELYAEV Ivan.Belyaeve@itep.ru
#  @date 2011-07-25
class Bc_pdf(CB2_pdf) :
    """
    Define PDF for Bc+ : Double sided Crystall Ball 
    """
    def __init__ ( self                   ,
                   mass                   ,   ## mass is mandatory here! 
                   name      = 'Bc'       ,
                   mean      = 6.267e+00  ,   ## to be released later 
                   sigma     = 7.2938e-03 ,   ## to be released later 
                   alphaL    = 1.4499e+00 ,   ## to be released later 
                   alphaR    = 1.9326e+00 ,   ## to be released later
                   nL        = 8.7234e+00 ,   ## to be released later 
                   nR        = 2.0377e+00 ) : ## to be released later 
        ## 
        CB2_pdf.__init__ ( self             ,
                           name             ,
                           mass.getMin()    ,
                           mass.getMax()    ,
                           mass             , 
                           mean             ,
                           sigma            ,
                           alphaL           ,
                           alphaR           ,
                           nL               ,
                           nR               )


# =============================================================================
# Specializations for Bukin function
# =============================================================================
from   Ostap.FitSignalModels   import Bukin_pdf 
# =============================================================================
## @class D0_pdf
#  simple wrapper over Bukin-pdf
#  @see RooBukinPdf
#  @author Vanya BELYAEV Ivan.Belyaeve@itep.ru
#  @date 2011-07-25
class D0_pdf(Bukin_pdf) :
    """
    Define PDF for D0
    """
    def __init__ ( self                   ,
                   mass                   , ## mass is mandatory here! 
                   name     = 'D0'        ,
                   mean     =  1.864      ,
                   sigma    =  7.2037e-03 ,
                   xi       =  2.4656e-03 ,
                   rhoL     =  2.8583e-01 ,
                   rhoR     =  2.7163e-01 ) :
        
        Bukin_pdf.__init__ ( self          ,
                             name          ,
                             mass.getMin() ,
                             mass.getMax() ,
                             mass          , 
                             mean          ,
                             sigma         ,
                             xi            , 
                             rhoL          ,
                             rhoR          ) 
                             
# =============================================================================
## @class Dp_pdf
#  simple wrapper over Bukin-pdf
#  @see RooBukinPdf
#  @author Vanya BELYAEV Ivan.Belyaeve@itep.ru
#  @date 2011-07-25
class Dp_pdf(Bukin_pdf) :
    """
    Define PDF for D+
    """
    def __init__ ( self                    ,
                   mass                    , ## mass is mandatory here 
                   name     = 'Dp'         ,
                   mean     =  1.869       , 
                   sigma    =  None        ,
                   xi       = -7.7344e-03  ,
                   rhoL     =  3.0241e-01  , 
                   rhoR     =  3.7452e-01  ) :
        
        Bukin_pdf.__init__ ( self          ,
                             name          ,
                             mass.getMin() , 
                             mass.getMax() , 
                             mean          ,
                             sigma         ,
                             xi            ,                            
                             rhoL          ,
                             rhoR          ) 
        
# =============================================================================
## @class Ds_pdf
#  simple wrapper over Bukin-pdf
#  @see RooBukinPdf
#  @author Vanya BELYAEV Ivan.Belyaeve@itep.ru
#  @date 2011-07-25
class Ds_pdf(Bukin_pdf) :
    """
    Define PDF for Ds+
    """
    def __init__ ( self                    , 
                   mass     = None         , ## mass is mandatory 
                   name     = 'Ds'         ,
                   mean     =  1.969       ,
                   sigma    =  0.0068      ,
                   xi       = -6.45755e-04 ,
                   rhoL     =  3.0241e-01  , 
                   rhoR     =  3.7452e-01  ) :
        
        Bukin_pdf.__init__ ( self          ,
                             name          ,
                             mass.getMin() , 
                             mass.getMax() , 
                             mass          , 
                             mean          ,
                             sigma         ,
                             xi            ,
                             rhoL          ,
                             rhoR          ) 
        
# =============================================================================
## @class Lc_pdf
#  simple wrapper over Bukin-pdf
#  @see RooBukinPdf
#  @author Vanya BELYAEV Ivan.Belyaeve@itep.ru
#  @date 2011-07-25
class Lc_pdf(Bukin_pdf) :
    """
    Define PDF for Lc+
    """
    def __init__ ( self                     ,
                   mass                     , 
                   name     = 'Lc'          ,                   
                   mean     =  2.28590e+00  ,
                   sigma    =  5.11874e-03  ,
                   xi       =  1.82493e-03  ,
                   rhoL     =  3.0241e-01   , 
                   rhoR     =  3.7452e-01   ) :
        
        Bukin_pdf.__init__ ( self     ,
                             name     ,
                             mass.getMin() , 
                             mass.getMax() , 
                             mass     , 
                             mean     ,
                             sigma    ,
                             xi       ,
                             rhoL     ,
                             rhoR     ) 
        
# =============================================================================
## @class Manca_pdf 
#  the final full PDF for Y->mu+mu- fit
#  This is physuically weell-motivated function for fist in narrow
#  bins in pt and rapidity  
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2011-08-02
class Manca_pdf (PDF) :
    """
    The final full PDF for Y->mu+mu- fit 
    """
    def __init__ ( self          ,
                   mass          ,
                   name   = 'Y'  ,
                   power  = 0    ) :

        #
        PDF.__init__ ( self , name )
        #
        if     mass.getMin() <  9.460 and   9.60  <= mass.getMax()  : gev_ =    1
        elif   mass.getMin() < 10.    and  10.500 <= mass.getMax()  : gev_ =    1
        elif   mass.getMin() < 10.0   and  10.200 <= mass.getMax()  : gev_ =    1
        elif   mass.getMin() <  9460  and  10355  <= mass.getMax()  : gev_ = 1000
        elif   mass.getMin() < 10000  and  10500  <= mass.getMax()  : gev_ = 1000
        elif   mass.getMin() < 10000  and  10200  <= mass.getMax()  : gev_ = 1000
        else : raise TypeError ( "Illegal mass range %s<m<%s"
                                 % ( mass.getMin() , mass.getMax() ) ) 
        
        m_y1s  =  9.46030     * gev_ 
        s_y1s  =  4.3679e-02  * gev_ 
        dm_y2s = 10.02326     * gev_ - m_y1s
        dm_y3s = 10.3552      * gev_ - m_y1s

        # 
        self.mass = mass

        # =====================================================================
        from   Ostap.FitBasic            import makeVar
        from   Ostap.FitBkgModels        import Bkg_pdf
        from   Ostap.FitSignalModels     import Needham_pdf
        # =====================================================================
        ## Y(1S)
        # =====================================================================
        
        self.a0   = makeVar ( None ,
                              'a0m_%s' % name ,
                              "a0 for Needham's function" ,
                              1.91               ,    0.1             ,   3.0            )
        self.a1   = makeVar ( None ,
                              'a1m_%s' % name ,
                              "a1 for Needham's function" ,
                              1.1174 / gev_      ,  -10.0  / gev_     ,  10.0  /gev_     ) 
        self.a2   = makeVar ( None ,
                              'a2m_%s' % name ,
                              "a2 for Needham's function" ,
                              -5.299 / gev_**2   , -100.0  / gev_**2  , 100.0  /gev_**2  )

        self.Y1S  = Needham_pdf (
            name + '1S'           ,
            mass.getMin()         ,
            mass.getMax()         ,
            fixMass  = m_y1s      ,
            fixSigma = s_y1s      ,
            mass     = self.mass  ,
            mean     = None       ,
            sigma    = None       ,
            a0       = self.a0    ,
            a1       = self.a1    ,
            a2       = self.a2    ) 
        #
        ## adjust a bit the values
        #
        self.m1s  = self.Y1S.mean
        self.m1s.release   ()
        self.m1s.setVal    ( m_y1s                )
        self.m1s.setMin    ( m_y1s - 0.15 * s_y1s )
        self.m1s.setMax    ( m_y1s + 0.15 * s_y1s )
        #
        self.sigma = self.Y1S.sigma
        self.sigma.release () 
        self.sigma.setVal  (       s_y1s )
        self.sigma.setMin  ( 0.3 * s_y1s )
        self.sigma.setMax  ( 3.5 * s_y1s )
        self.s1s  = self.sigma
        
        # =====================================================================
        ## Y(2S)
        # =====================================================================
        self.dm2s  = makeVar ( None ,
                               "dm2s"      + name ,
                               "dm2s(%s)"  % name    ,
                               dm_y2s                ,
                               dm_y2s - 0.20 * s_y1s , 
                               dm_y2s + 0.20 * s_y1s )
        
        self.aset11 = ROOT.RooArgList ( self.m1s , self.dm2s )
        self.m2s    = ROOT.RooFormulaVar (
            "m_" + name + '2S'   ,
            "m2s(%s)"  % name    ,
            "%s+%s" % ( self.m1s.GetName() , self.dm2s.GetName()  ) , 
            self.aset11       )
        
        self.aset12 = ROOT.RooArgList ( self.sigma , self.m1s , self.m2s ) 
        self.s2s    = ROOT.RooFormulaVar (
            "sigma_"  + name + '2S'    ,
            "#sigma_{Y2S}(%s)" % name  ,
            "%s*(%s/%s)"  % ( self.sigma.GetName() ,
                              self.m2s  .GetName() ,
                              self.m1s  .GetName() ) ,
            self.aset12  )
        
        self.Y2S   = Needham_pdf (
            name + '2S'           ,
            mass.getMin()         ,
            mass.getMax()         ,
            fixMass  = None       ,
            fixSigma = None       ,
            mass     = self.mass  ,
            mean     = self.m2s   ,
            sigma    = self.s2s   ,
            a0       = self.a0    ,
            a1       = self.a1    ,
            a2       = self.a2    ) 
        
        # =====================================================================
        ## Y(3S)
        # =====================================================================
        self.dm3s  = makeVar ( None ,
                               "dm3s"      + name ,
                               "dm3s(%s)"  % name    ,
                               dm_y3s                ,
                               dm_y3s - 0.20 * s_y1s , 
                               dm_y3s + 0.20 * s_y1s )
        
        self.aset21 = ROOT.RooArgList ( self.m1s , self.dm3s )
        self.m3s    = ROOT.RooFormulaVar (
            "m_"       + name + '(3S)' ,
            "m3s(%s)"  % name          ,
            "%s+%s" % ( self.m1s.GetName() , self.dm3s.GetName() ) ,
            self.aset21       )
        
        
        self.aset22 = ROOT.RooArgList ( self.sigma , self.m1s , self.m3s ) 
        self.s3s    = ROOT.RooFormulaVar (
            "sigma_"  + name + '3S'    ,
            "#sigma_{Y3S}(%s)" % name  ,
            "%s*(%s/%s)"  % ( self.sigma.GetName() ,
                              self.m3s  .GetName() ,
                              self.m1s  .GetName() ) , 
            self.aset22       )
        
        self.Y3S   = Needham_pdf (
            name + '3S'           ,
            mass.getMin()         ,
            mass.getMax()         ,
            fixMass  = None       ,
            fixSigma = None       ,
            mass     = self.mass  ,
            mean     = self.m3s   ,
            sigma    = self.s3s   ,
            a0       = self.a0    ,
            a1       = self.a1    ,
            a2       = self.a2    ) 
        
        #
        ## the actual signal PDFs
        # 
        self.y1s   = self.Y1S.pdf
        self.y2s   = self.Y2S.pdf
        self.y3s   = self.Y3S.pdf
        
        
        self.background = Bkg_pdf  ( 'Bkg%s' % name , self.mass , power = power )
        
        self.n1s = makeVar ( None ,
                             "N1S" + name  ,
                             "Signal(Y1S)" ,  None , 1000 ,  0 ,  1.e+7 )
        self.n2s = makeVar ( None ,
                             "N2S" + name  ,
                             "Signal(Y2S)" ,  None ,  300 ,  0 ,  1.e+6 )
        self.n3s = makeVar ( None ,
                             "N3S" + name  ,
                             "Signal(Y3S)" ,  None ,  100 ,  0 ,  1.e+5 )
        self.b   = makeVar ( None ,
                             "B"   + name  ,
                             "Background"  ,  None ,  100 ,  0 ,  1.e+8 )
        
        self.alist1 = ROOT.RooArgList ( self.y1s , self.y2s , self.y3s ) 
        self.alist2 = ROOT.RooArgList ( self.n1s , self.n2s , self.n3s ) 
        
        self.alist1 . add ( self.background.pdf )
        self.alist2 . add ( self.b              )
        
        self.pdf  = ROOT.RooAddPdf  (
            "manca_%s"  % name ,
            "manca(%s)" % name ,
            self.alist1 ,
            self.alist2 )
        
        self.dm2s.setConstant ( True )
        self.dm3s.setConstant ( True )
        
        self._splots = []
        
        self.s1_name = self.n1s.GetName ()
        self.s2_name = self.n2s.GetName ()
        self.s3_name = self.n3s.GetName ()

        # 
        ## finally declare components 
        #
        self.signals    () . add ( self.y1s )
        self.signals    () . add ( self.y2s )
        self.signals    () . add ( self.y3s )
        self.backgrounds() . add ( self.background.pdf ) 
        
    def alpha_1S ( self ) : return self.Y1S.pdf.alpha ()
    def alpha_2S ( self ) : return self.Y2S.pdf.alpha ()
    def alpha_3S ( self ) : return self.Y3S.pdf.alpha ()


# =============================================================================
## @class Manca2_pdf 
#  the final full PDF for Y->mu+mu- fit
#  This is an effective function for fit in global bin, without pt/y-binning 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2014-06-24
class Manca2_pdf (PDF) :
    """
    The final full PDF for Y->mu+mu- fit
    
    This is an effective function for fit in global bin, without pt/y-binning
    
    """
    def __init__ ( self          ,
                   mass          ,
                   name   = 'Y'  ,
                   power  = 0    ) :

        #
        PDF.__init__ ( self , name )
        # 
        if     mass.getMin() <  9.460 and   9.60  <= mass.getMax()  : gev_ =    1
        elif   mass.getMin() < 10.    and  10.500 <= mass.getMax()  : gev_ =    1
        elif   mass.getMin() < 10.0   and  10.200 <= mass.getMax()  : gev_ =    1
        elif   mass.getMin() <  9460  and  10355  <= mass.getMax()  : gev_ = 1000
        elif   mass.getMin() < 10000  and  10500  <= mass.getMax()  : gev_ = 1000
        elif   mass.getMin() < 10000  and  10200  <= mass.getMax()  : gev_ = 1000
        else : raise TypeError ( "Illegal mass range %s<m<%s"
                                 % ( mass.getMin() , mass.getMax() ) ) 
        
        m_y1s  =  9.46030     * gev_ 
        s_y1s  =  4.3679e-02  * gev_ 
        dm_y2s = 10.02326     * gev_ - m_y1s
        dm_y3s = 10.3552      * gev_ - m_y1s

        # 
        self.mass = mass

        # =====================================================================
        from   Ostap.FitBasic            import makeVar
        from   Ostap.FitBkgModels        import Bkg_pdf
        from   Ostap.FitSignalModels     import CB2_pdf
                
        # =====================================================================
        ## Y(1S)
        # =====================================================================
        self.aL    = makeVar ( None                    ,
                               "aL_%s"          % name ,
                               "#alpha_{L}(%s)" % name , 1.52 , 1.52 , 0 , 10 )
        self.nL    = makeVar ( None                    ,                     
                               "nL_%s"          % name ,
                               "n_{L}(%s)"      % name , 1.35 , 1.35 , 0 , 10 )
        self.aR    = makeVar ( None                    ,
                               "aR_%s"          % name ,
                               "#alpha_{R}(%s)" % name , 1.76 , 1.76 , 0 , 10 )
        self.nR    = makeVar ( None                    ,
                               "nR_%s"          % name ,
                               "n_{R}(%s)"      % name , 9    , 9    , 0 , 15 )
        
        self.Y1S  = CB2_pdf (
            name + '1S'           ,
            mass.getMin()         ,
            mass.getMax()         ,
            fixMass  = m_y1s      ,
            fixSigma = s_y1s      ,
            mass     = self.mass  ,
            mean     = None       ,
            sigma    = None       ,
            alphaL   = self.aL    ,
            alphaR   = self.aR    ,
            nL       = self.nL    ,
            nR       = self.nR    )
        
        #
        ## adjust a bit the values
        #
        self.m1s  = self.Y1S.mean
        self.m1s.release   ()
        self.m1s.setVal    ( m_y1s                )
        self.m1s.setMin    ( m_y1s - 0.15 * s_y1s )
        self.m1s.setMax    ( m_y1s + 0.15 * s_y1s )
        #
        self.sigma = self.Y1S.sigma
        self.sigma.release () 
        self.sigma.setVal  (       s_y1s )
        self.sigma.setMin  ( 0.3 * s_y1s )
        self.sigma.setMax  ( 3.5 * s_y1s )
        self.s1s  = self.sigma

        # =====================================================================
        ## Y(2S)
        # =====================================================================
        self.dm2s  = makeVar ( None ,
                               "dm2s"      + name ,
                               "dm2s(%s)"  % name    ,
                               dm_y2s                ,
                               dm_y2s - 0.20 * s_y1s , 
                               dm_y2s + 0.20 * s_y1s )
        
        self.aset11 = ROOT.RooArgList ( self.m1s , self.dm2s )
        self.m2s    = ROOT.RooFormulaVar (
            "m_" + name + '2S'   ,
            "m2s(%s)"  % name    ,
            "%s+%s" % ( self.m1s.GetName() , self.dm2s.GetName()  ) , 
            self.aset11       )
        
        self.aset12 = ROOT.RooArgList ( self.sigma , self.m1s , self.m2s ) 
        self.s2s    = ROOT.RooFormulaVar (
            "sigma_"  + name + '2S'    ,
            "#sigma_{Y2S}(%s)" % name  ,
            "%s*(%s/%s)"  % ( self.sigma.GetName() ,
                              self.m2s  .GetName() ,
                              self.m1s  .GetName() ) ,
            self.aset12  )
        
        self.Y2S  = CB2_pdf (
            name + '2S'           ,
            mass.getMin()         ,
            mass.getMax()         ,
            fixMass  = None       ,
            fixSigma = None       ,
            mass     = self.mass  ,
            mean     = self.m2s   ,
            sigma    = self.s2s   ,
            alphaL   = self.aL    ,
            alphaR   = self.aR    ,
            nL       = self.nL    ,
            nR       = self.nR    )
                
        # =====================================================================
        ## Y(3S)
        # =====================================================================
        self.dm3s  = makeVar ( None ,
                               "dm3s"      + name ,
                               "dm3s(%s)"  % name    ,
                               dm_y3s                ,
                               dm_y3s - 0.20 * s_y1s , 
                               dm_y3s + 0.20 * s_y1s )
        
        self.aset21 = ROOT.RooArgList ( self.m1s , self.dm3s )
        self.m3s    = ROOT.RooFormulaVar (
            "m_"       + name + '(3S)' ,
            "m3s(%s)"  % name          ,
            "%s+%s" % ( self.m1s.GetName() , self.dm3s.GetName() ) ,
            self.aset21       )
        
        
        self.aset22 = ROOT.RooArgList ( self.sigma , self.m1s , self.m3s ) 
        self.s3s    = ROOT.RooFormulaVar (
            "sigma_"  + name + '3S'    ,
            "#sigma_{Y3S}(%s)" % name  ,
            "%s*(%s/%s)"  % ( self.sigma.GetName() ,
                              self.m3s  .GetName() ,
                              self.m1s  .GetName() ) , 
            self.aset22       )
        
        self.Y3S  = CB2_pdf (
            name + '3S'           ,
            mass.getMin()         ,
            mass.getMax()         ,
            fixMass  = None       ,
            fixSigma = None       ,
            mass     = self.mass  ,
            mean     = self.m3s   ,
            sigma    = self.s3s   ,
            alphaL   = self.aL    ,
            alphaR   = self.aR    ,
            nL       = self.nL    ,
            nR       = self.nR    )
        
        #
        ## the actual signal PDFs
        # 
        self.y1s   = self.Y1S.pdf
        self.y2s   = self.Y2S.pdf
        self.y3s   = self.Y3S.pdf
        
        
        self.background = Bkg_pdf  ( 'Bkg%s' % name , self.mass , power = power )
        
        self.n1s = makeVar ( None ,
                             "N1S" + name  ,
                             "Signal(Y1S)" ,  None , 1000 ,  0 ,  1.e+7 )
        self.n2s = makeVar ( None ,
                             "N2S" + name  ,
                             "Signal(Y2S)" ,  None ,  300 ,  0 ,  1.e+6 )
        self.n3s = makeVar ( None ,
                             "N3S" + name  ,
                             "Signal(Y3S)" ,  None ,  100 ,  0 ,  1.e+6 )
        self.b   = makeVar ( None ,
                             "B"   + name  ,
                             "Background"  ,  None ,  100 ,  0 ,  1.e+8 )
        
        self.alist1 = ROOT.RooArgList ( self.y1s , self.y2s , self.y3s ) 
        self.alist2 = ROOT.RooArgList ( self.n1s , self.n2s , self.n3s ) 
        
        self.alist1 . add ( self.background.pdf )
        self.alist2 . add ( self.b              )
        
        self.pdf  = ROOT.RooAddPdf  (
            "manca_%s"  % name ,
            "manca(%s)" % name ,
            self.alist1 ,
            self.alist2 )
        
        self.dm2s.setConstant ( True )
        self.dm3s.setConstant ( True )
        
        self._splots = []
        
        self.s1_name = self.n1s.GetName ()
        self.s2_name = self.n2s.GetName ()
        self.s3_name = self.n3s.GetName ()

        # 
        ## finally declare components 
        #
        self.signals    () . add ( self.y1s )
        self.signals    () . add ( self.y2s )
        self.signals    () . add ( self.y3s )
        self.backgrounds() . add ( self.background.pdf ) 



        

# =============================================================================
if '__main__' == __name__ :
    
    import Ostap.Line
    logger.info ( __file__  + '\n' + Ostap.Line.line  ) 
    logger.info ( 80*'*'   )
    logger.info ( __doc__  )
    logger.info ( 80*'*' )
    logger.info ( ' Author  : %s' %         __author__    ) 
    logger.info ( ' Version : %s' %         __version__   ) 
    logger.info ( ' Date    : %s' %         __date__      )
    logger.info ( ' Symbols : %s' %  list ( __all__     ) )
    logger.info ( 80*'*' ) 

# =============================================================================
# The END 
# =============================================================================
