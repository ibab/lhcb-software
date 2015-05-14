'''
Module for selecting J/psi and Psi(2S) using an ineheritance approach
'''

__author__=['Lucio Anderlini']
__date__ = '2013-12-16'
__version__= '$Revision: 1.0 $'


__all__ = (
    'default_config'
    )

default_config = {
    'NAME'       : 'Onia',
    'WGs'        : ['BandQ'],
    'STREAMS'    : ['Dimuon'],
    'BUILDERTYPE': 'DiMuonInherit',
    'CONFIG'     : {
    "DestinationFullDST" : True, ### Selects line type
                                 #####################
  #### Definition of all the DiMuon lines ############
  ###############
    "Lines" : {
      "VirtualBase" : {
        "Prescale"      : 0.0,
        "NeedFullDST"   : False,
        "Inherit"       : None,
        "checkPV"       : False,
        "maxPV"         : None,
        "TOScut"        : None,
        "InputDiMuon"   : "StdLooseDiMuon",
        "Cuts"          : {
          "MuonPt"        : "MINTREE('mu+'==ABSID,PT) > 650.0 *MeV",
          "MuonPIDmu"     : "MINTREE('mu+'==ABSID,PIDmu) > 0",
          "JpsiVtx"       : "VFASPF(VCHI2PDOF)< 20.0"
        }
      },

      "DiMuonHighMass" : {
        "Prescale"      : 1.0,
        "NeedFullDST"   : True,
        "Inherit"       : "VirtualBase",
        "Cuts"          : {
          "MuonP"         : "MINTREE('mu+'==ABSID,P ) > 8000.0 *MeV",
          "MuonPt"        : "MINTREE('mu+'==ABSID,PT) > 1000.0 *MeV",
          "Mass"          : "MM > 8500 *MeV"
        }
      },

      "DiMuonHighMassSameSign" : {
  #      "Prescale"      : 1.0,
  #      "NeedFullDST"   : True,
        "Inherit"       : "DiMuonHighMass",
        "InputDiMuon"   : "StdLooseDiMuonSameSign",
  #      "Cuts"          : None
      },

      "DiMuonNoPV" : {
        "Prescale"      : 1.0,
        "NeedFullDST"   : True,
        "Inherit"       : "VirtualBase",
        "maxPV"         : 0.5,
        "Cuts"          : {
          "BPVVDZ"        : "BPVVDZ < -1 *mm",
          "MuonPIDmu"     : None, #remove the cut 
          "Mass"          : "MM > 2900",
         }
      },
      
      "Jpsi2MuMuDetached" : {
        "Prescale"      : 1.0,
        "NeedFullDST"   : True,
        "Inherit"       : "VirtualBase",
        "checkPV"       : True,
        "InputDiMuon"   : "StdLooseJpsi2MuMu",
        "Cuts"          : {
          "MuonPt"        : "MINTREE('mu+'==ABSID,PT) > 550.0 *MeV",# replace!
          "Mass"          : "(MM > 2996.916) & (MM < 3196.916)",
          "Detachement"   : "((BPVDLS>3) | (BPVDLS<-3)) & (MINTREE('mu+'==ABSID,BPVIPCHI2())>4)",
        }
      },

      "Psi2MuMuDetached" : {
        "Prescale"      : 1.0,
        "NeedFullDST"   : True,
        "Inherit"       : "Jpsi2MuMuDetached",
        "InputDiMuon"   : "StdLooseDiMuon",
        "Cuts"          : {
          "Mass"          :  "ADMASS('psi(2S)') < 100.0 *MeV"
        }
      },
      
      "Jpsi2MuMuTOS" : {
        "Prescale"      : 1.0,
        "NeedFullDST"   : True,
        "Inherit"       : "VirtualBase",
        "InputDiMuon"   : "StdLooseJpsi2MuMu",
        "TOScut"        : { "L0.*Mu.*Decision%TOS":0,
                            "Hlt1DiMuonHighMassDecision%TOS" : 0,
                            "Hlt2DiMuonJPsiHighPTDecision%TOS" : 0 
                          },

        "Cuts"          : {
          "MuonP"         : "MINTREE('mu+'==ABSID,P) > 10 *GeV",
          "JpsiPt"        : "PT > 3 *GeV",
          "Mass"          : "(MM > 3010) & (MM < 3170)",
        }   
      },

      "Psi2MuMuTOS" : {
        "Prescale"      : 1.0,
        "NeedFullDST"   : True,
        "Inherit"       : "Jpsi2MuMuTOS",
        "InputDiMuon"   : "StdLooseDiMuon",
        "TOScut"        : { "L0.*Mu.*Decision%TOS":0,
                            "Hlt1DiMuonHighMassDecision%TOS" : 0,
                            "Hlt2DiMuonPsi2SHighPTDecision%TOS" : 0 
                          },
        "Cuts"          : {
          "Mass"        : "ADMASS('psi(2S)') < 100.0",
          "MuonPt"      : "MINTREE('mu+'==ABSID,PT) > 1 *GeV"
        }   
      },


  ## MicroDST lines

      "DiMuonInc" : {
        "Prescale"      : 1.0,
        "NeedFullDST"   : False,
        "Inherit"       : "VirtualBase",
        "InputDiMuon"   : "StdLooseDiMuon",
        "Cuts"          : {
          "Mass"          : "MM > 3000*MeV",
        }   
      },

      "DiMuonIncSameSign" : {
        "Prescale"              : 1.0,
        "Inherit"               : "DiMuonInc",
        "InputDiMuon"           : "StdLooseDiMuonSameSign",
        "Cuts"                  : None
      },

  ## Are these lines really needed? Aren't they included in Inclusive line?
      "Jpsi2MuMu" : {
        "Prescale"      : 1.0,
        "NeedFullDST"   : False,
        "checkPV"       : True,
        "Inherit"       : "VirtualBase",
        "InputDiMuon"   : "StdLooseDiMuon",
        "Cuts"          : {
          "Mass"        :   "(MM > 2976.916 *MeV) & (MM < 3216.916 *MeV)",
          "MuonPIDmu"   :   "MINTREE('mu+'==ABSID,PIDmu) > -5",
          "JpsiPt"      :   "(PT > 3*GeV)"
        }   
      },

      "Psi2MuMu" : {
        "Prescale"      : 1.0,
        "Inherit"       : "Jpsi2MuMu",
        "Cuts"          : {
          "Mass"        :   "ADMASS('psi(2S)') < 120.0 *MeV",
        }
      },
    }
  }
}

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder

class DiMuonInherit (LineBuilder):
    __configuration_keys__ = (
        'DestinationFullDST',
        'Lines'
        )
    
    def __init__(self, name, config ): 
        
        LineBuilder.__init__(self, name, config)
        self.name = name 
        self.config = config
        self.linesCfg  = config['Lines']

        ### Loops on the lines defined and get line titles
        for lineName in self.linesCfg:
          ### Translate the line name to a dictionary
          line = self.linesCfg[lineName] 
          ### Test the prescale. Zero prescale means virtual line.
          if self.getVariable(line,'Prescale') != None:
            if (self.getVariable(line,'Prescale')  != 0) and (config["DestinationFullDST"] == self.getVariable(line, 'NeedFullDST')):

              ### Defines the filter desktop with the cuts
              _tmpFilter = FilterDesktop(Code = self.compileCut(line))
              ### Binds the algorithm to a DaVinci Selection
              _tmpAlgorithm = Selection ( self.name + lineName + 'Sel',
                Algorithm = _tmpFilter,
                RequiredSelections = [DataOnDemand(
                  "Phys/"+self.getVariable(line,"InputDiMuon")+"/Particles")])

              ### If a check on maxPV is required defines the proper filter
              _tmpFilter = {}
              if self.getVariable(line, 'maxPV') != None:
                _tmpFilter = { 'Code'       : "recSummary(LHCb.RecSummary.nPVs, 'Rec/Vertex/Primary')<" + 
                                                str(self.getVariable(line, 'maxPV')) ,
                               'Preambulo'  : [ 'from LoKiTracks.decorators import *' ,
                                                'from LoKiNumbers.decorators import *',
                                                'from LoKiCore.functions import *' ]
                             }

              ### If a TOS check is required redefine  _tmpAlgorithm to point to TOS filter
              if self.getVariable(line, 'TOScut') != None:
                _tmpPlainAlgorithm = _tmpAlgorithm
                _tmpAlgorithm = self.filterTisTos( name + lineName + "TOSFilter",
                                                   DiMuonInput = _tmpPlainAlgorithm,
                                                   myTisTosSpecs = self.getVariable(line, 'TOScut')
                                                 )


              ## debug cut writer
              print "###################### DiMuonInh Debugger ########################"
              print "## " + name + lineName + 'Line'
              print "## Input:      " +  "Phys/"+self.getVariable(line,"InputDiMuon")+"/Particles" 
              print "## Parsed Cut: " + self.compileCut(line)
              print "##################################################################"

              ### StrippingLine
              _tmpDiMuonLine = StrippingLine (  name + lineName + 'Line',
                                                prescale  = self.getVariable(line, 'Prescale'),    
                                                FILTER    = _tmpFilter,
                                                checkPV   = self.getVariable(line, 'checkPV'),
                                                algos     = [ _tmpAlgorithm ]
                                             )

              self.registerLine( _tmpDiMuonLine )

    def getVariable (self, line, varName ):
      if varName in line:
        retVal = line[varName]
      else:
        if "Inherit" in line and line["Inherit"] != None : 
          retVal = self.getVariable ( self.linesCfg[line["Inherit"]] , varName )
        else:
          raise NameError("Missing definition of " + varName + " in line " + line + " and no inheritance defined")
          return None

      return retVal

    def compileCut ( self, line ):
      ### retrieves the dictionary
      cutDict = self.getCutDict ( line )
       
      ### compiles the dictionary into a cut set
      _compiledCut = ""
      for cut in cutDict:
         
        if cutDict[cut] != None:
          if _compiledCut != "" : _compiledCut += " & "
          _compiledCut += "(" + cutDict[cut] + ")"

      return _compiledCut;
        
    def getCutDict ( self, line ):
      ### Loads the current cut dict
      if "Cuts" in line and line["Cuts"] != None:
        dictCutCurr = line["Cuts"]
        
        ### Loads the inherited cut dict
        if "Inherit" in line and line["Inherit"] != None : 
          dictCutInh = self.getCutDict ( self.linesCfg[line["Inherit"]] )
        else:
          dictCutInh = {}

        ### Merges the two dictionaries
        for cut in dictCutInh:
          print "Consider inherited cut " + cut
          if cut not in dictCutCurr :
            print " not defined in current dictionary"
            dictCutCurr[cut] = dictCutInh[cut]

        ### returns the merged dict
        return dictCutCurr;

      return self.getCutDict ( self.linesCfg[line["Inherit"]] )



  
    def filterTisTos(self,
                     name,
                     DiMuonInput,
                     myTisTosSpecs ) :

        from Configurables import TisTosParticleTagger
        print "> TOS requirement included!"
        
        myTagger = TisTosParticleTagger(name + "_TisTosTagger")
        myTagger.TisTosSpecs = myTisTosSpecs
        
        # To speed it up, TisTos only with tracking system
        myTagger.ProjectTracksToCalo = False
        myTagger.CaloClustForCharged = False
        myTagger.CaloClustForNeutral = False
        myTagger.TOSFrac = { 4:0.0, 5:0.0 }
        
        return Selection(name + "_SelTisTos",
                         Algorithm = myTagger,
                         RequiredSelections = [ DiMuonInput ] )





