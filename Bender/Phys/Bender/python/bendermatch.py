#!/usr/bin/env python
# =============================================================================
# $Id: bendermatch.py,v 1.1 2004-07-11 15:47:05 ibelyaev Exp $ 
# =============================================================================
# CVS tag $NAme:$ 
# =============================================================================
# $Log: not supported by cvs2svn $
# =============================================================================


# =============================================================================
# @file
#
# defintion of all LoKi/Bender MCMatch objects
#
# @date   2004-07-11
# @author Vanya BELYAEV Ivan.Belyaev@itep.ru
# =============================================================================

import PyLCGDict 

# inport global namespace 
from gaudimodule import gbl

# load the minimal set of dictionaries
PyLCGDict.loadDict('BenderDict')

# load 'intermediate objects 
BenderMCMatch = gbl.Bender.MCMatch

# extend teh intermediate object
class MCMatch(BenderMCMatch):
    """
    The basic object for easy manipulation with Monte Carlo Truth
    For detailes see 'LoKi User Guide' available at $LOKIDOC/doc directory
    The underlying C++ classes are:
    - Bender::MCMatch    (Tools/BenderLibs)   $BENDERLIBSROOT/Bender/MCMatch.h
    - LoKi::MCMatch      (Tools/LoKi)         $LOKIROOT/LoKi/MCMatch.h
    - LoKi::MCMatchObj   (Tools/LoKi)         $LOKIROOT/LoKi/MCMatchObj.h
    see also
    - IMCDecayFinder  (Phys/DaVinciMCTools)
    and especially $DAVINCIMCTOOLSROOT/src/MCDecayFinder.h
    """
    def __init__ ( self , obj ) :
        """
        Constructor from BenderMCMatch object
        """
        BenderMCMatch.__init__( self , obj ) 

    def __call__ ( self  , reco , mc  ) :
        """
        Match the reconstructed and Monte Carlo objects. ('Callable' interface)
        Usage:
        matched = matcher( reco , mc ) 
        where :
        - 'matcher' is the instance of MCMatch object 
        - 'reco'    is either reconstructed particle or range of reconstructed particle
        - 'mc'      is either Monte Carlo particle or range of Monte Carlo particles
        The Monte Calo matching procedure is described in 'LoKi User Guide'
        """
        return BenderMCMatch.match( self , reco , mc )
    
    def match    ( self , **args      ) :
        """
        Match the reconstructed and Monte Carlo objects . ('Python'-like interface)
        Usage :
        matched = matcher.match( reco = particle  , mc = mcparticle ) 
        where :
        -  matcher  is the instance of MCMatch object 
        - 'reco'    is either reconstructed particle or range of reconstructed particle
        - 'mc'      is either Monte Carlo particle or range of Monte Carlo particles
        The Monte Calo matching procedure is described in 'LoKi User Guide'
        """
        if not args.has_key ( 'reco' ) :
            raise TypeError, "  'reco'  is not specified "
        if not args.has_key ( 'mc'   ) :
            raise TypeError, "  'mc'    is not specified "
        reco = args.get ( 'reco' )
        mc   = args.get ( 'mc'   )
        return BenderMCMatch.match( self , reco , mc )
    
    def find     ( self , **args ) :
        """
        Find Monte Carlo decays using teh descriptor
        Usage :
        decays = mc.find( decay = 'D0 -> K- pi+' , address = address )
        where :
        -  mc       is the instance of MCMatch object 
        - 'decay'   is the decay descriptor 
        - 'address' is address of Monte Carlo Particles in Gaudi TES  (optional)
        Internally the nice tool MCDecayFinder by Olivier Dormond is used. 
        The decay descriptor semantics is described in detail in
        $DAVINCIMCTOOLSROOT/src/MCDecayFinder.h
        """
        if not args.has_key ( 'decay'   ) :
            raise TypeError, "  'decay'  is not specified "
        decay   = args.get ( 'decay')
        if not args.has_key ( 'address' ) :
            return BenderMCMatch.find( self , decay )
        address = args.get ( 'address'  )
        return BenderMCMatch.find( self , decay , address )
    
