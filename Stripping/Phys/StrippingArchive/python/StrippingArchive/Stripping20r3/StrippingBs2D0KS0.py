'''
Module for construction of Bs->KsD0 stripping selections and lines

Exported symbols (use python help!):
   - Bs2D0KS0Conf
   - makeBs2D0KS0
   - makeD02Kpi
'''

__author__ = 'Jesse Mesman'
__email__ = 'Jesse.Mesman@student.uva.nl'
__date__ = '29/11/2012'
__version__ = '$ 1.0 $'

__all__=('Bs2D0KS0Conf',
         'makeBs2D0KS0')

default_config = { 
     "BIPCHI2"      : 20.0,         # Maximum Bs IP chi2.
     "BVCHI2"       : 10.0,         # Maximum Bs vertex chi2 per degree of freedom.
     "BsCombMassMin" : 5000.,        # Lower mass of D0+Ks combination
     "BsCombMassMax" : 5650.,        # Upper mass of D0+Ks combination 
     "BPVDLS"       : 5.,           # Minimum decay length significance
     "BsMassMin"    : 5100.,        # Lower mass of Bs
     "BsMassMax"    : 5550.,        # Upper mass of Bs
     "D0MassMin"    : 1804.,        # Lower mass of D0
     "D0MassMax"    : 1924.,        # Upper mass of D0
     "DTF_CHI2NDOF" : 5.            # Minimum vertex chi2 per degree of freedom after decay tree fitter fit
         }

from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
from CommonParticles.Utils import *
from Configurables import FilterDesktop, CombineParticles, OfflineVertexFitter	
from PhysSelPython.Wrappers import Selection, SelectionSequence, DataOnDemand, MergedSelection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from copy import copy

default_name = "Bs2D0KS0"


class Bs2D0KS0Conf(LineBuilder):
     """
     Builder of:
     - Bs-> D0 + Ks stripping line (Bs2D0KS0)
     """

     __configuration_keys__ = ("BVCHI2","BPVDLS","BsMassMin","BsMassMax","BIPCHI2","BsCombMassMin","BsCombMassMax","D0MassMin","D0MassMax","DTF_CHI2NDOF")


     def __init__(self, name, config) :

          LineBuilder.__init__(self, name, config)

          self.selBs2D0KS0 = makeBs2D0KS0(name,
                                            BVCHI2 = config['BVCHI2'],
                                            BPVDLS = config['BPVDLS'],
                                            BIPCHI2 = config['BIPCHI2'],
					    BsCombMassMin = config['BsCombMassMin'],
					    BsCombMassMax = config['BsCombMassMax'],
                                            BsMassMin = config['BsMassMin'],
                                            BsMassMax = config['BsMassMax'],
                                            D0MassMin = config['D0MassMin'],
                                            D0MassMax = config['D0MassMax'],
					    DTF_CHI2NDOF = config['DTF_CHI2NDOF'])
    
          self.Bs2D0KS0_line = StrippingLine(name+"Line",
                                              prescale = 1,
                                              postscale = 1,
                                              algos = [ self.selBs2D0KS0 ]
                                              )
          

          self.registerLine(self.Bs2D0KS0_line)

def makeBs2D0KS0(name,BIPCHI2,BVCHI2,BPVDLS,BsCombMassMin,BsCombMassMax,BsMassMin,BsMassMax,D0MassMin,D0MassMax,DTF_CHI2NDOF):

       """
       Create and return a Bs -> D0 KS selection
       """ 
       SelStdLooseKsDD = DataOnDemand(Location = 'Phys/StdLooseKsDD/Particles')
       SelStdLooseKsLL = DataOnDemand(Location = 'Phys/StdLooseKsLL/Particles')
       SelStdLooseKs = MergedSelection("KSFor"+name, RequiredSelections = [ SelStdLooseKsDD, SelStdLooseKsLL ] )
       SelStdLooseD0 = DataOnDemand(Location = 'Phys/StdLooseD02KPi/Particles')
       
       _motherCuts = "(VFASPF(VCHI2/VDOF) < %(BVCHI2)s)" \
                     "& (BPVDLS > %(BPVDLS)s)" \
                     "& (M > %(BsMassMin)s*MeV)" \
                     "& (M < %(BsMassMax)s*MeV)" \
                     "& (BPVIPCHI2() < %(BIPCHI2)s)" \
                     "& (DTF_CHI2NDOF(True) < %(DTF_CHI2NDOF)s)" % locals()    
       _combinationCut = "(AM > %(BsCombMassMin)s*MeV)" \
                         "& (AM < %(BsCombMassMax)s*MeV)" % locals() 
       _daughterCut = "(M > %(D0MassMin)s*MeV)" \
                      "& (M < %(D0MassMax)s*MeV)" \
                      "& (MAXTREE(ABSID=='K+',PIDK) > 0)" % locals()
       _Bs = CombineParticles('_'+name)
       _Bs.DecayDescriptor = "B0 -> D0 KS0"
       _Bs.CombinationCut = _combinationCut
       _Bs.MotherCut = _motherCuts
       _Bs.DaughtersCuts = {"D0" : _daughterCut}

       return Selection ( name,
                          Algorithm = _Bs,
                          RequiredSelections = [SelStdLooseD0,SelStdLooseKs])



