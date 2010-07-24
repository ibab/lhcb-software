
__author__ = ['Francesca Dordei']
__date__ = '2010/07/15'
__version__ = '$Revision: 1.2 $'

'''
  Stripping for [Lambda_c+ -> p+ K- pi+]cc selection
            
'''
from Gaudi.Configuration import *
from LHCbKernel.Configuration import *

class StrippingLambdacConf(LHCbConfigurableUser):
    """
    Definition of [Lambda_c+ -> p+ K- pi+]cc stripping
    """
    __slots__ = { 'PrescaleLambdac'      : 1.
                  ,'LMassWin'            : 50.        # MeV
                  ,'LDIRA'               : 0.9997     #adimensional
                  ,'LENDVERTEX'          : 20         #adimensional
                  ,'pPID'                : 10         #adimensional
                  ,'piPID'               : 10         #adimensional
                  ,'KPID'                : 10         #adimensional
                  ,'LTAU'                :0.0012      # ns
                  ,'LFD'                 :8           #adimensional
                  ,'MinPiPt'             : 0          # MeV
                  ,'MinKPt'              : 0          # MeV
                  ,'MinPPt'              : 500        # MeV
                  , 'MinLambdaPt'        : 0          # MeV
                  , 'doca'               : 0.25       # mm
                  }
    
    def getProps(self) :
        """
        Copied from StrippingBu2LLKConf.
        Originally from HltLinesConfigurableUser.
        May someday be absorbed into the base class definition.
        """
        d = dict()
        for (k,v) in self.getDefaultProperties().iteritems() :
            d[k] = getattr(self,k) if hasattr(self,k) else v
        return d
  
    
    ####### Template for combine particles for [Lambda_c+ -> p+ K- pi+]cc 
    def combineLambdac(self) :
        from Configurables import CombineParticles
        from StrippingConf.StrippingLine import bindMembers
        from StrippingConf.StrippingLine import StrippingLine

        lambdacomb_combcut       =  "(ADAMASS('Lambda_c+')< %(LMassWin)s *MeV) & (ADOCAMAX('') < %(doca)s)"
        lambdacomb_lambdacut =  "(BPVDIRA> %(LDIRA)s) & (VFASPF(VCHI2/VDOF)< %(LENDVERTEX)s) & (BPVLTIME('PropertimeFitter/properTime:PUBLIC')<%(LTAU)s) & (BPVVDCHI2 > %(LFD)s) & (PT > %(MinLambdaPt)s)"
        lambdacomb_pcut = "((PIDp-PIDpi)> %(pPID)s) & (PT > %(MinPPt)s * MeV)"
	lambdacomb_Kcut = "((PIDK-PIDpi)> %(KPID)s) & (PT > %(MinKPt)s * MeV)"
	lambdacomb_picut = "((PIDK-PIDpi)< %(piPID)s) & (PT > %(MinPiPt)s * MeV)"
        
        lambdac_comb = CombineParticles( "LambdacComb" )
        lambdac_comb.DecayDescriptor = "[Lambda_c+ -> p+ K- pi+]cc"
        lambdac_comb.DaughtersCuts = {   "p+" : lambdacomb_pcut % self.getProps(),
                                         "K-" : lambdacomb_Kcut % self.getProps(),
                                         "pi+": lambdacomb_picut % self.getProps()
                                         }

           
        lambdac_comb.CombinationCut = lambdacomb_combcut % self.getProps()
        lambdac_comb.MotherCut =  lambdacomb_lambdacut % self.getProps()
        lambdac_comb.InputLocations = [ "StdNoPIDsPions", "StdNoPIDsKaons", "StdNoPIDsProtons" ]
        seq_lam =  bindMembers("seq_lam", algos = [ lambdac_comb])
        pres = "PrescaleLambdac"
        line = StrippingLine("LambdacKpipLine", algos = [ seq_lam ],
                             prescale = 1.,
                             postscale = 1.)##self.getProps()[ pres ])
        return line
 
    
    def lines ( self ) :
        from Configurables import GaudiSequencer, CombineParticles, FilterDesktop
        from StrippingConf.StrippingLine import StrippingLine, StrippingMember
        from CommonParticles.StdNoPIDsPions   import StdNoPIDsPions
        from CommonParticles.StdNoPIDsKaons   import StdNoPIDsKaons
        from CommonParticles.StdNoPIDsProtons import StdNoPIDsProtons
        
       
        line_lambda = self.combineLambdac()
        
        lines = [line_lambda]
        
        return lines
    

#    def __apply_configuration__(self) :
#        self.lines()




