# $Id: StrippingD2KS0H.py,v 1.0 2010-02-21 sgregson
'''
Module for construction of D(s)->KS0H (H=pi/K) 
stripping Selections and StrippingLines.
Provides functions to build D(s) selections.
Provides class D2KS0HConf, which constructs the Selections and 
StrippingLines given a configuration dictionary.
Exported symbols (use python help!):
   - D2KS0HConf
   - makeKS02PiPi
   - makeD2KS0Pi
   - makeD2KS0K
  '''

__author__ = ['Sam Gregson']
__date__ = '02/21/2011'
__version__ = '$Revision: 1.0 $'

__all__ = ('D2KS0HConf',
           'makeKS02PiPi',
           'makeKS02PiPiDD',
           'makeD2KS0Pi',
           'makeD2KS0PiDD',
           'makeD2KS0K'
           'makeD2KS0KDD'
           'makeBachelorPions'
           'makeBachelorKaons'
           )

# Import some useful stuff
from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StandardParticles import StdLoosePions, StdLooseKaons
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder, checkConfig

## Example config dictionary - keep updated

## config = {
##   # KS0 daughter pion cuts
##        'KS0DaugP'  : 2000  
##       ,'KS0DaugPT'  : 200
##       ,'KS0DaugTrackChi2' : 4  
##       ,'KS0DaugMIPChi2' : 40
##       ,'KS0DaugPIDK' : 10

##   # KS0 mother cuts
##       ,'KS0MassWindow' : 35   
##       ,'KS0MIPChi2'     : 7
##       ,'KS0MIPChi2DD'     : 0
##       ,'KS0VertexChi2'  : 10
##       ,'KS0PT' : 1000
##       ,'KS0BPVVDCHI2' : 300 
##       ,'KS0BPVVDCHI2DD' : 200  


##   # Bachelor pion cuts
##       ,'BachPionP' : 2000               
##       ,'BachPionPT' : 200         
##       ,'BachPionTrackChi2' : 3
##       ,'BachPionMIPChi2' : 15 
##       ,'BachPionPIDK'   : 0  

##   # Bachelor kaon cuts
##       ,'BachKaonP' : 2000                 
##       ,'BachKaonPT'  : 200       
##       ,'BachKaonTrackChi2': 3
##       ,'BachKaonMIPChi2'   : 15  
##       ,'BachKaonPIDK'   : 0      

##   # D meson cuts - pion
##   # Combo cuts
##       ,'DMesonComboLowMass'  : 1760        
##       ,'DMesonComboHighMass'  : 2080
##       ,'DMesonComboDOCA' : 0.6
##       ,'DMesonAPT' : 1000
##       ,'DMesonADOCAChi2' : 11
##   # Mother cuts
##       ,'DMesonMotherLowMass' : 1770
##       ,'DMesonMotherHighMass'   : 2070
##       ,'DMesonMotherVertexChi2' : 10
##       ,'DMesonMotherMIPChi2'  : 15  
##       ,'DMesonMotherPT' : 1000
##       ,'KS0ZDiff' : 10
##       ,'DMesonFlightDistChi2' : 10
 
##   # Prescales
##       ,'PionLinePrescale' : 1
##       ,'KaonLinePrescale' : 1
##       ,'PionLinePrescaleDD' : 1
##       ,'KaonLinePrescaleDD' : 1

##   # Postscales     
##       ,'PionLinePostscale' : 1 
##       ,'KaonLinePostscale' : 1    
##       ,'PionLinePostscaleDD' : 1 
##       ,'KaonLinePostscaleDD' : 1          
##           }

#Class for lines - inherits from LineBuilder class

class D2KS0HConf(LineBuilder) :
    """
    Builder of D2KS0H stripping Selections and StrippingLines.
    Constructs D(s) -> KS0H (H=pi/K) Selections and StrippingLines from a configuration dictionary.
    """

## Set up the keys/cuts which will receive values from the cut dictinaries in Phys/StrippingSettings

    __configuration_keys__ = (

  # KS0 daughter pion cuts
       'KS0DaugP'    
      ,'KS0DaugPT'  
      ,'KS0DaugTrackChi2'  
      ,'KS0DaugMIPChi2' 
      ,'KS0DaugPIDK' 

  # KS0 mother cuts
      ,'KS0MassWindow'   
      ,'KS0MIPChi2'
      ,'KS0MIPChi2DD' 
      ,'KS0VertexChi2'  
      ,'KS0PT'
      ,'KS0BPVVDCHI2'
      ,'KS0BPVVDCHI2DD' 

  # Bachelor pion cuts
      ,'BachPionP'               
      ,'BachPionPT'         
      ,'BachPionTrackChi2'
      ,'BachPionMIPChi2'  
      ,'BachPionPIDK'     

  # Bachelor kaon cuts
      ,'BachKaonP'                 
      ,'BachKaonPT'        
      ,'BachKaonTrackChi2'
      ,'BachKaonMIPChi2'   
      ,'BachKaonPIDK'      

  # D meson cuts - pion
  # Combo cuts
      ,'DMesonComboLowMass'          
      ,'DMesonComboHighMass'  
      ,'DMesonComboDOCA'
      ,'DMesonAPT'
      ,'DMesonADOCAChi2'
  # Mother cuts
      ,'DMesonMotherLowMass' 
      ,'DMesonMotherHighMass'   
      ,'DMesonMotherVertexChi2' 
      ,'DMesonMotherMIPChi2'    
      ,'DMesonMotherPT'
      ,'KS0ZDiff'
      ,'DMesonFlightDistChi2'
 
  # Prescales
      ,'PionLinePrescale' 
      ,'KaonLinePrescale'  
      ,'PionLinePrescaleDD' 
      ,'KaonLinePrescaleDD'
       
  # Postscales     
      ,'PionLinePostscale' 
      ,'KaonLinePostscale'
      ,'PionLinePostscaleDD' 
      ,'KaonLinePostscaleDD'
       
                              )

    # Class constructor
    def __init__(self, name, config) :
        
    # Use the constructor of the base class from which we inherit
        LineBuilder.__init__(self, name, config)

        self.name = name
        
        # Names for different lines
        pion_name = self.name+'Pion'
        kaon_name = self.name+'Kaon'
        pion_nameDD = self.name+'PionDD'
        kaon_nameDD = self.name+'KaonDD'
        

        # Define the selections that go forward to the lines - combine particles, combine particles etc 

        # Bachelor pions
        self.selBachPions = makeBachelorPions(self.name + 'BachelorPionsForD2KS0H'
                                    # Cuts
                                    # Bachelor pion cuts
                                    ,BachPionP = config['BachPionP']                 
                                    ,BachPionPT = config['BachPionPT']         
                                    ,BachPionTrackChi2 = config['BachPionTrackChi2']  
                                    ,BachPionMIPChi2 = config['BachPionMIPChi2']     
                                    ,BachPionPIDK = config['BachPionPIDK']   
                                                      )

        # Bachelor kaons
        self.selBachKaons = makeBachelorKaons(self.name + 'BachelorKaonsForD2KS0H'
                                    # Cuts
                                    # Bachelor kaon cuts
                                    ,BachKaonP = config['BachKaonP']                 
                                    ,BachKaonPT = config['BachKaonPT']         
                                    ,BachKaonTrackChi2 = config['BachKaonTrackChi2']  
                                    ,BachKaonMIPChi2 = config['BachKaonMIPChi2']     
                                    ,BachKaonPIDK = config['BachKaonPIDK']   
                                                      )

        # KS0 LL selection
        self.selKS0LL = makeKS02PiPi(self.name + 'KS0LLForD2KS0H'
                                    # Cuts
                                    # KS0 daughter pion cuts
                                   ,KS0DaugP = config['KS0DaugP']  
                                   ,KS0DaugPT = config['KS0DaugPT']
                                   ,KS0DaugTrackChi2 = config['KS0DaugTrackChi2'] 
                                   ,KS0DaugMIPChi2 = config['KS0DaugMIPChi2']
                                   ,KS0DaugPIDK = config['KS0DaugPIDK']
                                   # KS0 mother cuts
                                   ,KS0MassWindow = config['KS0MassWindow']   
                                   ,KS0MIPChi2 = config['KS0MIPChi2']     
                                   ,KS0VertexChi2 = config['KS0VertexChi2']  
                                   ,KS0PT = config['KS0PT']
                                   ,KS0BPVVDCHI2 = config['KS0BPVVDCHI2']
                                                   )

        # KS0 DD selection
        self.selKS0DD = makeKS02PiPiDD(self.name + 'KS0DDForD2KS0H'
                                    # Cuts
                                    # KS0 daughter pion cuts
                                   ,KS0DaugP = config['KS0DaugP']  
                                   ,KS0DaugPT = config['KS0DaugPT']
                                   ,KS0DaugTrackChi2 = config['KS0DaugTrackChi2'] 
                                   ,KS0DaugMIPChi2 = config['KS0DaugMIPChi2']
                                   ,KS0DaugPIDK = config['KS0DaugPIDK']
                                   # KS0 mother cuts
                                   ,KS0MassWindow = config['KS0MassWindow']   
                                   ,KS0MIPChi2DD = config['KS0MIPChi2DD']     
                                   ,KS0VertexChi2 = config['KS0VertexChi2']  
                                   ,KS0PT = config['KS0PT']
                                   ,KS0BPVVDCHI2DD = config['KS0BPVVDCHI2DD']   
                                                                  )

       
        # D(s)->KS0Pi selection
        self.selD2KS0Pi = makeD2KS0Pi(pion_name
                                     # Pass the KS0LL selection 
                                     ,KS0LLSel = self.selKS0LL
                                     # Pass the bachelor pions selection
                                     ,BachPionsSel = self.selBachPions 
                                     # D meson cuts
                                     # Combo cuts
                                     ,DMesonComboLowMass = config['DMesonComboLowMass']           
                                     ,DMesonComboHighMass = config['DMesonComboHighMass']    
                                     ,DMesonComboDOCA = config['DMesonComboDOCA']
                                     ,DMesonAPT = config['DMesonAPT']
                                     ,DMesonADOCAChi2 = config['DMesonADOCAChi2'] 
                                     # Mother cuts
                                     ,DMesonMotherLowMass = config['DMesonMotherLowMass']               
                                     ,DMesonMotherHighMass = config['DMesonMotherHighMass']     
                                     ,DMesonMotherVertexChi2 = config['DMesonMotherVertexChi2']   
                                     ,DMesonMotherMIPChi2 = config['DMesonMotherMIPChi2']    
                                     ,DMesonMotherPT = config['DMesonMotherPT']
                                     ,KS0ZDiff = config['KS0ZDiff']
                                     ,DMesonFlightDistChi2 = config['DMesonFlightDistChi2'] 
                                     )   

        # D(s)->KS0DDPi selection
        self.selD2KS0PiDD = makeD2KS0PiDD(pion_nameDD
                                     # Pass the KS0DD selection 
                                     ,KS0DDSel = self.selKS0DD
                                     # Pass the bachelor pions selection
                                     ,BachPionsSel = self.selBachPions 
                                     # D meson cuts
                                     # Combo cuts
                                     ,DMesonComboLowMass = config['DMesonComboLowMass']           
                                     ,DMesonComboHighMass = config['DMesonComboHighMass']    
                                     ,DMesonComboDOCA = config['DMesonComboDOCA']
                                     ,DMesonAPT = config['DMesonAPT']
                                     ,DMesonADOCAChi2 = config['DMesonADOCAChi2'] 
                                     # Mother cuts
                                     ,DMesonMotherLowMass = config['DMesonMotherLowMass']               
                                     ,DMesonMotherHighMass = config['DMesonMotherHighMass']     
                                     ,DMesonMotherVertexChi2 = config['DMesonMotherVertexChi2']   
                                     ,DMesonMotherMIPChi2 = config['DMesonMotherMIPChi2']    
                                     ,DMesonMotherPT = config['DMesonMotherPT']
                                     ,KS0ZDiff = config['KS0ZDiff']
                                     ,DMesonFlightDistChi2 = config['DMesonFlightDistChi2']      
                                     )  
                                

        # D(s)->KS0K selection
        self.selD2KS0K = makeD2KS0K(kaon_name
                                    # Pass the KS0LL selection 
                                   ,KS0LLSel = self.selKS0LL
                                    # Pass the bachelor kaons selection
                                   ,BachKaonsSel = self.selBachKaons  
                                    # D meson cuts
                                    # Combo cuts
                                   ,DMesonComboLowMass = config['DMesonComboLowMass']           
                                   ,DMesonComboHighMass = config['DMesonComboHighMass']    
                                   ,DMesonComboDOCA = config['DMesonComboDOCA']
                                   ,DMesonAPT = config['DMesonAPT']
                                   ,DMesonADOCAChi2 = config['DMesonADOCAChi2'] 
                                    # Mother cuts
                                   ,DMesonMotherLowMass = config['DMesonMotherLowMass']               
                                   ,DMesonMotherHighMass = config['DMesonMotherHighMass']     
                                   ,DMesonMotherVertexChi2 = config['DMesonMotherVertexChi2']   
                                   ,DMesonMotherMIPChi2 = config['DMesonMotherMIPChi2']    
                                   ,DMesonMotherPT = config['DMesonMotherPT']
                                   ,KS0ZDiff = config['KS0ZDiff']
                                   ,DMesonFlightDistChi2 = config['DMesonFlightDistChi2'] 
                                    )

        # D(s)->KS0DDK selection
        self.selD2KS0KDD = makeD2KS0KDD(kaon_nameDD
                                    # Pass the KS0LL selection 
                                   ,KS0DDSel = self.selKS0DD
                                    # Pass the bachelor kaons selection
                                   ,BachKaonsSel = self.selBachKaons  
                                    # D meson cuts
                                    # Combo cuts
                                   ,DMesonComboLowMass = config['DMesonComboLowMass']           
                                   ,DMesonComboHighMass = config['DMesonComboHighMass']    
                                   ,DMesonComboDOCA = config['DMesonComboDOCA']
                                   ,DMesonAPT = config['DMesonAPT']
                                   ,DMesonADOCAChi2 = config['DMesonADOCAChi2'] 
                                    # Mother cuts
                                   ,DMesonMotherLowMass = config['DMesonMotherLowMass']               
                                   ,DMesonMotherHighMass = config['DMesonMotherHighMass']     
                                   ,DMesonMotherVertexChi2 = config['DMesonMotherVertexChi2']   
                                   ,DMesonMotherMIPChi2 = config['DMesonMotherMIPChi2']    
                                   ,DMesonMotherPT = config['DMesonMotherPT']
                                   ,KS0ZDiff = config['KS0ZDiff']
                                   ,DMesonFlightDistChi2 = config['DMesonFlightDistChi2'] 
                                    )


        # Define/declare the lines
        self.pion_line = StrippingLine(pion_name+"Line",
                                       prescale = config['PionLinePrescale'],
                                       postscale = config['PionLinePostscale'],
                                       selection = self.selD2KS0Pi
                                       )
        self.kaon_line = StrippingLine(kaon_name+"Line",
                                       prescale = config['KaonLinePrescale'],
                                       postscale = config['KaonLinePostscale'],
                                       selection = self.selD2KS0K
                                       )
        self.pion_lineDD = StrippingLine(pion_name+"LineDD",
                                       prescale = config['PionLinePrescaleDD'],
                                       postscale = config['PionLinePostscaleDD'],
                                       selection = self.selD2KS0PiDD
                                       )
        self.kaon_lineDD = StrippingLine(kaon_name+"LineDD",
                                       prescale = config['KaonLinePrescaleDD'],
                                       postscale = config['KaonLinePostscaleDD'],
                                       selection = self.selD2KS0KDD
                                       )

        

        # Register the lines
        self.registerLine(self.pion_line)
        self.registerLine(self.kaon_line)
        self.registerLine(self.pion_lineDD)
        self.registerLine(self.kaon_lineDD)

# Define the bachelor pions filter sequence - arguments are cuts used

def makeBachelorPions(name
                      # Bachelor pion cuts
                     ,BachPionP               
                     ,BachPionPT         
                     ,BachPionTrackChi2 
                     ,BachPionMIPChi2   
                     ,BachPionPIDK  
                     ) :
        """
        Create and return a Bachelor pion selection object.
        Starts from DataOnDemand 'Phys/StdLoosePions'.
        Arguments:
        name             : name of the Selection.
        All others       : cuts used in the selection - self-explanatory naming.
        """
     # Define the cuts to be used

        _code = "(PIDK < %(BachPionPIDK)s) & (P > %(BachPionP)s*MeV) & (PT > %(BachPionPT)s*MeV) & (TRCHI2DOF < %(BachPionTrackChi2)s) & (MIPCHI2DV(PRIMARY) > %(BachPionMIPChi2)s)" % locals()
 
         # Define the pion filter
        _BachPionsFilter = FilterDesktop(Code = _code)
        _stdLoosePions = StdLoosePions

        return Selection (name,
                      Algorithm = _BachPionsFilter,
                      RequiredSelections = [_stdLoosePions])

# Define the bachelor pions filter sequence - arguments are cuts used

def makeBachelorKaons(name
                     # Bachelor pion cuts
                     ,BachKaonP               
                     ,BachKaonPT         
                     ,BachKaonTrackChi2 
                     ,BachKaonMIPChi2   
                     ,BachKaonPIDK  
                     ) :

        """
        Create and return a Bachelor kaon selection object.
        Starts from DataOnDemand 'Phys/StdLooseKaons'.
        Arguments:
        name             : name of the Selection.
        All others       : cuts used in the selection - self-explanatory naming.
        """
     # Define the cuts to be used
        _code = "(PIDK > %(BachKaonPIDK)s) & (P > %(BachKaonP)s*MeV) & (PT > %(BachKaonPT)s*MeV) & (TRCHI2DOF < %(BachKaonTrackChi2)s) & (MIPCHI2DV(PRIMARY) > %(BachKaonMIPChi2)s)" % locals()
 
         # Define the pion filter
        _BachKaonsFilter = FilterDesktop(Code = _code)
        _stdLooseKaons = StdLooseKaons

        return Selection (name,
                      Algorithm = _BachKaonsFilter,
                      RequiredSelections = [_stdLooseKaons])


# Define the KS02PiPi selection sequence - arguments need to be the cuts that are used....

def makeKS02PiPi(name,

                 # Cuts
                 # KS0 daughter pion cuts
                 KS0DaugP     
                ,KS0DaugPT
                ,KS0DaugTrackChi2  
                ,KS0DaugMIPChi2
                ,KS0DaugPIDK
                # KS0 mother cuts
                ,KS0MassWindow    
                ,KS0MIPChi2      
                ,KS0VertexChi2   
                ,KS0PT
                ,KS0BPVVDCHI2
                 ) :
        """
        Create and return a KS0 -> PiPi selection object.
        Starts from DataOnDemand 'Phys/StdLooseKS0LL'.
        Arguments:
        name             : name of the Selection.
        All others       : cuts used in the selection - self-explanatory naming.
        """

        # Define the cuts to be used
        _code = " (PT > %(KS0PT)s*MeV) & (VFASPF(VCHI2PDOF) < %(KS0VertexChi2)s) & CHILDCUT((TRCHI2DOF < %(KS0DaugTrackChi2)s),1) & CHILDCUT((TRCHI2DOF < %(KS0DaugTrackChi2)s),2) & CHILDCUT((MIPCHI2DV(PRIMARY) > %(KS0DaugMIPChi2)s),1) & CHILDCUT((MIPCHI2DV(PRIMARY) > %(KS0DaugMIPChi2)s),2) & (MIPCHI2DV(PRIMARY) > %(KS0MIPChi2)s) & (BPVVDCHI2> %(KS0BPVVDCHI2)s)" % locals()

     #   _code = "(PT > %(KS0PT)s*MeV) & (VFASPF(VCHI2PDOF) < %(KS0VertexChi2)s) & CHILDCUT((TRCHI2DOF < %(KS0DaugTrackChi2)s),1) & CHILDCUT((TRCHI2DOF < %(KS0DaugTrackChi2)s),2) & CHILDCUT((MIPCHI2DV(PRIMARY) > %(KS0DaugMIPChi2)s),1) & CHILDCUT((MIPCHI2DV(PRIMARY) > %(KS0DaugMIPChi2)s),2) & (BPVVDCHI2> %(KS0BPVVDCHI2)s) & (MIPCHI2DV(PRIMARY) > %(KS0MIPChi2)s)" % locals()


##         # Vanya suggestion
##        # _Preambulo = [
##        #  goodChild =           " (TRCHI2DOF < %(KS0DaugTrackChi2)s)" % locals (), 
##        #  goodChild = goodChild & "(PIDK < %(KS0DaugPIDK)s)" % locals (), 
##        #  goodChild = goodChild & "(MIPCHI2DV(PRIMARY) > %(KS0DaugMIPChi2)s)" % locals (), 
##         #             ]
        
##         # Define the cuts to be used
##      #   _code = """
##      #  (PT > %(KS0PT)s*MeV) &
##      #  (VFASPF(VCHI2/VDOF) < %(KS0VertexChi2)s) &
##      #  CHILDCUT ( goodChild ,1) &
##      #  CHILDCUT ( goodchild, 2) &
##      #  (MIPCHI2DV(PRIMARY) > %(KS0MIPChi2)s)
##      #  """ % locals() 


        # Define the KS0 LL filter
        _KS0LLFilter = FilterDesktop(Code = _code)
        _stdKS0LL = DataOnDemand(Location = "Phys/StdLooseKsLL/Particles")

        return Selection (name,
                      Algorithm = _KS0LLFilter,
                      RequiredSelections = [_stdKS0LL])

def makeKS02PiPiDD(name,

                 # Cuts
                 # KS0 daughter pion cuts
                 KS0DaugP     
                ,KS0DaugPT
                ,KS0DaugTrackChi2  
                ,KS0DaugMIPChi2
                ,KS0DaugPIDK
                # KS0 mother cuts
                ,KS0MassWindow    
                ,KS0MIPChi2DD      
                ,KS0VertexChi2   
                ,KS0PT
                ,KS0BPVVDCHI2DD
                 ) :
        """
        Create and return a KS0 -> PiPi selection object.
        Starts from DataOnDemand 'Phys/StdLooseKS0DD'.
        Arguments:
        name             : name of the Selection.
        All others       : cuts used in the selection - self-explanatory naming.
        """

        # Define the cuts to be used
        _code = " (PT > %(KS0PT)s*MeV) & (VFASPF(VCHI2PDOF) < %(KS0VertexChi2)s) & CHILDCUT((TRCHI2DOF < %(KS0DaugTrackChi2)s),1) & CHILDCUT((TRCHI2DOF < %(KS0DaugTrackChi2)s),2) & CHILDCUT((MIPCHI2DV(PRIMARY) > %(KS0DaugMIPChi2)s),1) & CHILDCUT((MIPCHI2DV(PRIMARY) > %(KS0DaugMIPChi2)s),2) & (BPVVDCHI2> %(KS0BPVVDCHI2DD)s)" % locals()

#        _code = "(PT > %(KS0PT)s*MeV) & (VFASPF(VCHI2PDOF) < %(KS0VertexChi2)s) & CHILDCUT((TRCHI2DOF < %(KS0DaugTrackChi2)s),1) & CHILDCUT((TRCHI2DOF < %(KS0DaugTrackChi2)s),2) & CHILDCUT((MIPCHI2DV(PRIMARY) > %(KS0DaugMIPChi2)s),1) & CHILDCUT((MIPCHI2DV(PRIMARY) > %(KS0DaugMIPChi2)s),2) & (BPVVDCHI2> %(KS0BPVVDCHI2DD)s)" % locals()

##         # Vanya suggestion
##        # _Preambulo = [
##        #  goodChild =           " (TRCHI2DOF < %(KS0DaugTrackChi2)s)" % locals (), 
##        #  goodChild = goodChild & "(PIDK < %(KS0DaugPIDK)s)" % locals (), 
##        #  goodChild = goodChild & "(MIPCHI2DV(PRIMARY) > %(KS0DaugMIPChi2)s)" % locals (), 
##         #             ]
        
##         # Define the cuts to be used
##      #   _code = """
##      #  (PT > %(KS0PT)s*MeV) &
##      #  (VFASPF(VCHI2/VDOF) < %(KS0VertexChi2)s) &
##      #  CHILDCUT ( goodChild ,1) &
##      #  CHILDCUT ( goodchild, 2) &
##      #  (MIPCHI2DV(PRIMARY) > %(KS0MIPChi2)s)
##      #  """ % locals() 


        # Define the KS0 LL filter
        _KS0DDFilter = FilterDesktop(Code = _code)
        _stdKS0DD = DataOnDemand(Location = "Phys/StdLooseKsDD/Particles")

        return Selection (name,
                      Algorithm = _KS0DDFilter,
                      RequiredSelections = [_stdKS0DD])


def makeD2KS0Pi(name,
                # KS0 selection
                KS0LLSel
                # Bach pions selection
               ,BachPionsSel   
                # Cuts to be used                  
                # D meson cuts
                # Combo cuts
               ,DMesonComboLowMass         
               ,DMesonComboHighMass  
               ,DMesonComboDOCA
               ,DMesonAPT 
               ,DMesonADOCAChi2
               # Mother cuts
               ,DMesonMotherLowMass             
               ,DMesonMotherHighMass   
               ,DMesonMotherVertexChi2 
               ,DMesonMotherMIPChi2  
               ,DMesonMotherPT
               ,KS0ZDiff
               ,DMesonFlightDistChi2 
               ) :
        """
        Create and return a D -> KS0 Pi Selection object.
        Arguments:
        name             : name of the Selection.
        ks0llSel         : KS0LL -> pi+pi- Selection object.
        All rest   : Cuts used - self-explanatory naming
        """

        # Define the combination and mother cuts

        #_combCuts = "(APT > %(DMesonAPT)s) & (ACUTDOCACHI2(%(DMesonADOCAChi2)s,'')) & in_range(%(DMesonComboLowMass)s, AM, %(DMesonComboHighMass)s)" % locals()

        _combCuts = "(APT > %(DMesonAPT)s) & (ACUTDOCACHI2(%(DMesonADOCAChi2)s,'')) & in_range(%(DMesonComboLowMass)s, AM, %(DMesonComboHighMass)s)" % locals()


 #(AMAXDOCA('LoKi::DistanceCalculator') < %(DMesonComboDOCA)s * mm

        _motherCuts = "(PT > %(DMesonMotherPT)s*MeV) & (VFASPF(VCHI2PDOF) < %(DMesonMotherVertexChi2)s) & in_range(%(DMesonMotherLowMass)s, MM, %(DMesonMotherHighMass)s) & ((CHILD( VFASPF(VZ) , 'KS0' == ID ) - VFASPF(VZ)) > %(KS0ZDiff)s)  & (MIPCHI2DV(PRIMARY) < %(DMesonMotherMIPChi2)s) & (BPVVDCHI2 > %(DMesonFlightDistChi2)s)" % locals()

#        _motherCuts = "(PT > %(DMesonMotherPT)s*MeV) & (VFASPF(VCHI2PDOF) < %(DMesonMotherVertexChi2)s) & in_range(%(DMesonMotherLowMass)s, MM, %(DMesonMotherHighMass)s) & (MIPCHI2DV(PRIMARY) < %(DMesonMotherMIPChi2)s) & ((CHILD( VFASPF(VZ) , 'KS0' == ID ) - VFASPF(VZ)) > %(KS0ZDiff)s)  & (BPVVDCHI2 > %(DMesonFlightDistChi2)s)" % locals()
    
        # Define the combine particles
        _Dmeson = CombineParticles( DecayDescriptor = "[D+ -> KS0 pi+]cc", CombinationCut = _combCuts, MotherCut = _motherCuts)

        return Selection ( name,
                       Algorithm = _Dmeson,
                       RequiredSelections = [KS0LLSel, BachPionsSel])
def makeD2KS0PiDD(name,
                # KS0 selection
                KS0DDSel
                # Bach pions selection
               ,BachPionsSel   
                # Cuts to be used                  
                # D meson cuts
                # Combo cuts
               ,DMesonComboLowMass         
               ,DMesonComboHighMass  
               ,DMesonComboDOCA
               ,DMesonAPT 
               ,DMesonADOCAChi2
               # Mother cuts
               ,DMesonMotherLowMass             
               ,DMesonMotherHighMass   
               ,DMesonMotherVertexChi2 
               ,DMesonMotherMIPChi2  
               ,DMesonMotherPT
               ,KS0ZDiff
               ,DMesonFlightDistChi2
               ) :
        """
        Create and return a D -> KS0 Pi Selection object.
        Arguments:
        name             : name of the Selection.
        ks0llSel         : KS0DD -> pi+pi- Selection object.
        All rest   : Cuts used - self-explanatory naming
        """

        # Define the combination and mother cuts

#        _combCuts = "(APT > %(DMesonAPT)s) & (ACUTDOCACHI2(%(DMesonADOCAChi2)s,'')) & in_range(%(DMesonComboLowMass)s, AM, %(DMesonComboHighMass)s)" % locals()
        _combCuts = "(APT > %(DMesonAPT)s) & (ACUTDOCACHI2(%(DMesonADOCAChi2)s,'')) & in_range(%(DMesonComboLowMass)s, AM, %(DMesonComboHighMass)s)" % locals()

 #(AMAXDOCA('LoKi::DistanceCalculator') < %(DMesonComboDOCA)s * mm

        _motherCuts = "(PT > %(DMesonMotherPT)s*MeV) & (VFASPF(VCHI2PDOF) < %(DMesonMotherVertexChi2)s) & in_range(%(DMesonMotherLowMass)s, MM, %(DMesonMotherHighMass)s) & ((CHILD( VFASPF(VZ) , 'KS0' == ID ) - VFASPF(VZ)) > %(KS0ZDiff)s)  & (MIPCHI2DV(PRIMARY) < %(DMesonMotherMIPChi2)s) & (BPVVDCHI2 > %(DMesonFlightDistChi2)s)" % locals()

#        _motherCuts = "(PT > %(DMesonMotherPT)s*MeV) & (VFASPF(VCHI2PDOF) < %(DMesonMotherVertexChi2)s) & in_range(%(DMesonMotherLowMass)s, MM, %(DMesonMotherHighMass)s) & (MIPCHI2DV(PRIMARY) < %(DMesonMotherMIPChi2)s) & ((CHILD( VFASPF(VZ) , 'KS0' == ID ) - VFASPF(VZ)) > %(KS0ZDiff)s)" % locals()

        # Define the combine particles
        _Dmeson = CombineParticles( DecayDescriptor = "[D+ -> KS0 pi+]cc", CombinationCut = _combCuts, MotherCut = _motherCuts)

        return Selection ( name,
                       Algorithm = _Dmeson,
                       RequiredSelections = [KS0DDSel, BachPionsSel])

    
def makeD2KS0K(name,
               # KS0 selection
               KS0LLSel
               # Bach kaons selection
              ,BachKaonsSel 
               # Cuts to be used                  
               # D meson cuts
               # Combo cuts
              ,DMesonComboLowMass         
              ,DMesonComboHighMass  
              ,DMesonComboDOCA
              ,DMesonAPT 
              ,DMesonADOCAChi2
               # Mother cuts
              ,DMesonMotherLowMass             
              ,DMesonMotherHighMass   
              ,DMesonMotherVertexChi2 
              ,DMesonMotherMIPChi2  
              ,DMesonMotherPT
              ,KS0ZDiff
              ,DMesonFlightDistChi2 
                   ) :

        """
        Create and return a D(s) -> KS0 K Selection object.
        Arguments:
        name             : name of the Selection.
        ks0llSel         : KS0LL -> pi+pi- Selection object.
        All rest         : Cuts used - self-explanatory naming
        """

        # Define the combination, mother and daughter cuts
#        _combCuts = "(APT > %(DMesonAPT)s) & (ACUTDOCACHI2(%(DMesonADOCAChi2)s,'')) & in_range(%(DMesonComboLowMass)s, AM, %(DMesonComboHighMass)s)" % locals()
        _combCuts = "(APT > %(DMesonAPT)s) & (ACUTDOCACHI2(%(DMesonADOCAChi2)s,'')) & in_range(%(DMesonComboLowMass)s, AM, %(DMesonComboHighMass)s)" % locals()


# &(AMAXDOCA('LoKi::DistanceCalculator') < %(DMesonComboDOCA)s * mm


        _motherCuts = "(PT > %(DMesonMotherPT)s*MeV) & (VFASPF(VCHI2/VDOF) < %(DMesonMotherVertexChi2)s) & in_range(%(DMesonMotherLowMass)s, MM, %(DMesonMotherHighMass)s) & ((CHILD( VFASPF(VZ) , 'KS0' == ID ) - VFASPF(VZ)) > %(KS0ZDiff)s)  & (MIPCHI2DV(PRIMARY) < %(DMesonMotherMIPChi2)s) & (BPVVDCHI2 > %(DMesonFlightDistChi2)s)" % locals()

#        _motherCuts = "(PT > %(DMesonMotherPT)s*MeV) & (VFASPF(VCHI2/VDOF) < %(DMesonMotherVertexChi2)s) & in_range(%(DMesonMotherLowMass)s, MM, %(DMesonMotherHighMass)s) & (MIPCHI2DV(PRIMARY) < %(DMesonMotherMIPChi2)s) & (BPVVDCHI2 > %(DMesonFlightDistChi2)s) & ((CHILD( VFASPF(VZ) , 'KS0' == ID ) - VFASPF(VZ)) > %(KS0ZDiff)s)" % locals()

    
        # Define the combine particles
        _Dmeson = CombineParticles( DecayDescriptor = "[D+ -> KS0 K+]cc", CombinationCut = _combCuts, MotherCut = _motherCuts)

        return Selection ( name,
                       Algorithm = _Dmeson,
                       RequiredSelections = [KS0LLSel, BachKaonsSel])

def makeD2KS0KDD(name,
               # KS0 selection
               KS0DDSel
               # Bach kaons selection
              ,BachKaonsSel 
               # Cuts to be used                  
               # D meson cuts
               # Combo cuts
              ,DMesonComboLowMass         
              ,DMesonComboHighMass  
              ,DMesonComboDOCA
              ,DMesonAPT 
              ,DMesonADOCAChi2
               # Mother cuts
              ,DMesonMotherLowMass             
              ,DMesonMotherHighMass   
              ,DMesonMotherVertexChi2 
              ,DMesonMotherMIPChi2  
              ,DMesonMotherPT
              ,KS0ZDiff
              ,DMesonFlightDistChi2 
                   ) :

        """
        Create and return a D(s) -> KS0 K Selection object.
        Arguments:
        name             : name of the Selection.
        ks0llSel         : KS0DD -> pi+pi- Selection object.
        All rest         : Cuts used - self-explanatory naming
        """

        # Define the combination, mother and daughter cuts
#        _combCuts = "(APT > %(DMesonAPT)s) & (ACUTDOCACHI2(%(DMesonADOCAChi2)s,'')) & in_range(%(DMesonComboLowMass)s, AM, %(DMesonComboHighMass)s)" % locals()
        _combCuts = "(APT > %(DMesonAPT)s) & (ACUTDOCACHI2(%(DMesonADOCAChi2)s,'')) & in_range(%(DMesonComboLowMass)s, AM, %(DMesonComboHighMass)s)" % locals()

# &(AMAXDOCA('LoKi::DistanceCalculator') < %(DMesonComboDOCA)s * mm


        _motherCuts = "(PT > %(DMesonMotherPT)s*MeV) & (VFASPF(VCHI2/VDOF) < %(DMesonMotherVertexChi2)s) & in_range(%(DMesonMotherLowMass)s, MM, %(DMesonMotherHighMass)s) & ((CHILD( VFASPF(VZ) , 'KS0' == ID ) - VFASPF(VZ)) > %(KS0ZDiff)s)  & (MIPCHI2DV(PRIMARY) < %(DMesonMotherMIPChi2)s) & (BPVVDCHI2 > %(DMesonFlightDistChi2)s)" % locals()

#        _motherCuts = "(PT > %(DMesonMotherPT)s*MeV) & (VFASPF(VCHI2/VDOF) < %(DMesonMotherVertexChi2)s) & in_range(%(DMesonMotherLowMass)s, MM, %(DMesonMotherHighMass)s) & (MIPCHI2DV(PRIMARY) < %(DMesonMotherMIPChi2)s) & ((CHILD( VFASPF(VZ) , 'KS0' == ID ) - VFASPF(VZ)) > %(KS0ZDiff)s)" % locals()
    
        # Define the combine particles
        _Dmeson = CombineParticles( DecayDescriptor = "[D+ -> KS0 K+]cc", CombinationCut = _combCuts, MotherCut = _motherCuts)

        return Selection ( name,
                       Algorithm = _Dmeson,
                       RequiredSelections = [KS0DDSel, BachKaonsSel])
