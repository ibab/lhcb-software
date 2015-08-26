'''
Module for selecting J/psi and Psi(2S) using an ineheritance approach
'''

__author__=['Lucio Anderlini']
__date__ = '2015-05-15'
__version__= '$Revision: 1.0 $'


__all__ = (
    'default_config'
    )

default_config = {
    'NAME'       : 'DiMuonInherit',
    'WGs'        : ['BandQ'],
    'STREAMS'    : {'Dimuon' :[
                      "StrippingFullDSTDiMuonPsi2MuMuTOSLine",
                      "StrippingFullDSTDiMuonJpsi2MuMuTOSLine",
                      "StrippingFullDSTDiMuonPsi2MuMuDetachedLine",
                      "StrippingFullDSTDiMuonJpsi2MuMuDetachedLine",
                      "StrippingFullDSTDiMuonDiMuonNoPVLine",
                      'StrippingFullDSTDiMuonDiMuonHighMassLine',
                      'StrippingFullDSTDiMuonDiMuonHighMassSameSignLine'
                    ],
                    'Leptonic' :[
                      "StrippingMicroDSTDiMuonDiMuonSameSignLine",
                      "StrippingMicroDSTDiMuonDiMuonIncLine"
                    ]
    },
                   
    'BUILDERTYPE': 'DiMuonInherit',
    'CONFIG'     : {
  #### Definition of all the DiMuon lines ############
  ###############
    "Debug" : False,
    "Lines" : {
      "VirtualBase" : {
        "Prescale"      : 0.0,
        "Inherit"       : None,
        "checkPV"       : False,
        "maxPV"         : None,
        "TOScut"        : None,
        "RequiredRawEvents" : None,
        "InputDiMuon"   : "StdLooseDiMuon",
        "Cuts"          : {
          "MuonPt"        : "MINTREE('mu+'==ABSID,PT) > 650.0 *MeV",
          "JpsiVtx"       : "VFASPF(VCHI2PDOF)< 20.0"
        }
      },

      "FullDSTDiMuonDiMuonHighMassLine" : {
        "Prescale"      : 1.0,
        "Inherit"       : "VirtualBase",
        "Cuts"          : {
          "MuonP"         : "MINTREE('mu+'==ABSID,P ) > 8000.0 *MeV",
          "MuonPt"        : "MINTREE('mu+'==ABSID,PT) > 1000.0 *MeV",
          "Mass"          : "MM > 8500 *MeV",
          "MuonPIDmu"     : "MINTREE('mu+'==ABSID,PIDmu) > 0.0"
          }
      },

      "FullDSTDiMuonDiMuonHighMassSameSignLine" : {
    "Prescale"      : 0.2,
    "Inherit"       : "VirtualBase",
    #"Inherit"       : "FullDSTDiMuonDiMuonHighMassLine",
    "InputDiMuon"   : "StdLooseDiMuonSameSign",
    #"MuonPIDmu"     : None #remove the cut 
    #      "Cuts"          : None
    "Cuts"          : {
          "MuonP"         : "MINTREE('mu+'==ABSID,P ) > 8000.0 *MeV",
          "MuonPt"        : "MINTREE('mu+'==ABSID,PT) > 1000.0 *MeV",
          "Mass"          : "MM > 8500 *MeV",
          #"MuonPIDmu"     : "MINTREE('mu+'==ABSID,PIDmu) > 0.0"
          }
    },

      "FullDSTDiMuonDiMuonNoPVLine" : {
        "Prescale"      : 1.0,
        "Inherit"       : "VirtualBase",
        "maxPV"         : 0.5,
        "Cuts"          : {
          "BPVVDZ"        : "BPVVDZ < -1 *mm",
          #"MuonPIDmu"     : None, #remove the cut 
          "Mass"          : "MM > 2900",
          #"MuonPIDmu"     : "MINTREE('mu+'==ABSID,PIDmu) > 0.0"
          }
      },
      
      "FullDSTDiMuonJpsi2MuMuDetachedLine" : {
        "Prescale"      : 1.0,
        "Inherit"       : "VirtualBase",
        "checkPV"       : True,
        "RequiredRawEvents" : ["Trigger", "Muon", "Calo", "Rich", "Velo", "Tracker"],
        "InputDiMuon"   : "StdLooseJpsi2MuMu",
        "Cuts"          : {
          "MuonPt"        : "MINTREE('mu+'==ABSID,PT) > 500.0 *MeV",# replace!
          "Mass"          : "(MM > 2996.916) & (MM < 3196.916)",
          "Detachement"   : "((BPVDLS>3) | (BPVDLS<-3))",
          "MuonPIDmu"     : "MINTREE('mu+'==ABSID,PIDmu) > 0.0"
          }
      },

      "FullDSTDiMuonPsi2MuMuDetachedLine" : {
        "Prescale"      : 1.0,
        "Inherit"       : "FullDSTDiMuonJpsi2MuMuDetachedLine",
        "RequiredRawEvents" : ["Trigger", "Muon", "Calo", "Rich", "Velo", "Tracker"],
        "InputDiMuon"   : "StdLooseDiMuon",
        "Cuts"          : {
          "Mass"          :  "ADMASS('psi(2S)') < 100.0 *MeV"
        }
      },
      
      "FullDSTDiMuonJpsi2MuMuTOSLine" : {
        "Prescale"      : 1.0,
        "Inherit"       : "VirtualBase",
        "InputDiMuon"   : "StdLooseJpsi2MuMu",
#        "TOScut"        : { "L0.*Mu.*Decision%TOS":0,
#                            "Hlt1DiMuonHighMassDecision%TOS" : 0,
#                            "Hlt2DiMuonJPsiHighPTDecision%TOS" : 0 
#                          },
        "RequiredRawEvents" : None,
        "Cuts"          : {
          "MuonP"         : "MINTREE('mu+'==ABSID,P) > 10 *GeV",
          "JpsiPt"        : "PT > 3 *GeV",
          "Mass"          : "(MM > 3010) & (MM < 3170)",
          "MuonPIDmu"     : "MINTREE('mu+'==ABSID,PIDmu) > 0.0",
          "TOSL0"         : "TOS('L0.*Mu.*Decision', 'L0TriggerTisTos')",
          "TOSHlt1"       : "TOS('Hlt1DiMuonHighMassDecision', 'Hlt1TriggerTisTos')",
          "TOSHlt2"       : "TOS('Hlt2DiMuonJPsiHighPTDecision', 'Hlt2TriggerTisTos')",
          }   
      },

      "FullDSTDiMuonPsi2MuMuTOSLine" : {
        "Prescale"      : 1.0,
        "Inherit"       : "FullDSTDiMuonJpsi2MuMuTOSLine",
        "InputDiMuon"   : "StdLooseDiMuon",
#        "TOScut"        : { "L0.*Mu.*Decision%TOS":0,
#                            "Hlt1DiMuonHighMassDecision%TOS" : 0,
#                            "Hlt2DiMuonPsi2SHighPTDecision%TOS" : 0 
#                          },
        "RequiredRawEvents" : None,
        "Cuts"          : {
          "Mass"        : "ADMASS('psi(2S)') < 100.0",
          "MuonPt"      : "MINTREE('mu+'==ABSID,PT) > 1 *GeV",
          "MuonPIDmu"     : "MINTREE('mu+'==ABSID,PIDmu) > 0.0",
          "TOSL0"         : "TOS('L0.*Mu.*Decision', 'L0TriggerTisTos')",
          "TOSHlt1"       : "TOS('Hlt1DiMuonHighMassDecision', 'Hlt1TriggerTisTos')",
          "TOSHlt2"       : "TOS('Hlt2DiMuonPsi2SHighPTDecision', 'Hlt2TriggerTisTos')",
          }   
      },


  ## MicroDST lines

      "MicroDSTDiMuonDiMuonIncLine" : {
        "Prescale"      : 1.0,
        "Inherit"       : "VirtualBase",
        "InputDiMuon"   : "StdLooseDiMuon",
        "Cuts"          : {
          "Mass"          : "MM > 3000*MeV",
        }   
      },

      "MicroDSTDiMuonDiMuonSameSignLine" : {
        "Prescale"              : 0.5,
        "Inherit"               : "MicroDSTDiMuonDiMuonIncLine",
        "InputDiMuon"           : "StdLooseDiMuonSameSign",
        "Cuts"                  : None
      },

      ## Are these lines really needed? Aren't they included in Inclusive line?
      #"Jpsi2MuMu" : {
       # "Prescale"      : 1.0,
       # "checkPV"       : True,
       # "Inherit"       : "VirtualBase",
       # "InputDiMuon"   : "StdLooseDiMuon",
       # "Cuts"          : {
       #   "Mass"        :   "(MM > 2976.916 *MeV) & (MM < 3216.916 *MeV)",
       #   "MuonPIDmu"   :   "MINTREE('mu+'==ABSID,PIDmu) > -5",
       #   "JpsiPt"      :   "(PT > 3*GeV)"
       # }   
     # },

      #"Psi2MuMu" : {
      #  "Prescale"      : 1.0,
      #  "Inherit"       : "Jpsi2MuMu",
      #  "Cuts"          : {
      #    "Mass"        :   "ADMASS('psi(2S)') < 120.0 *MeV",
      #  }
      #},
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
        "Lines","Debug"
        )
    
    def __init__(self, name, config ): 
        
        LineBuilder.__init__(self, name, config)
        self.name = name 
        self.config = config
        self.linesCfg  = config['Lines']
        self.dbug = config['Debug']

        ### Loops on the lines defined and get line titles
        for lineName in self.linesCfg:
          ### Translate the line name to a dictionary
          line = self.linesCfg[lineName] 
          ### Test the prescale. Zero prescale means virtual line.
          if self.getVariable(line,'Prescale') != None:
            if (self.getVariable(line,'Prescale')  != 0) :

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


              if self.dbug:
                ## debug cut writer
                print "###################### DiMuonInh Debugger ########################"
                print "## " + name + lineName + 'Line'
                print "## Input:      " +  "Phys/"+self.getVariable(line,"InputDiMuon")+"/Particles" 
                print "## Parsed Cut: " + self.compileCut(line).replace(" &", " \n\t\t&" )


              ### If a TOS check is required redefine  _tmpAlgorithm to point to TOS filter
              if self.getVariable(line, 'TOScut') != None:
                _tmpPlainAlgorithm = _tmpAlgorithm
                _tmpAlgorithm = self.filterTisTos( name + lineName + "TOSFilter",
                                                   DiMuonInput = _tmpPlainAlgorithm,
                                                   myTisTosSpecs = self.getVariable(line, 'TOScut')
                                                 )

              if self.dbug:
                print "##################################################################"

              ### StrippingLine
              _tmpDiMuonLine = StrippingLine (  lineName, # name + lineName + 'Line',
                                                prescale  = self.getVariable(line, 'Prescale'),    
                                                FILTER    = _tmpFilter,
                                                checkPV   = self.getVariable(line, 'checkPV'),
                                                RequiredRawEvents = self.getVariable(line, 'RequiredRawEvents'),
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
          raise NameError("Missing definition of " + str(varName) + " in line " + str(line) + " and no inheritance defined")
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
#          if self.dbug: print "Consider inherited cut " + cut
          if cut not in dictCutCurr :
#            if self.dbug:      print " not defined in current dictionary"
            dictCutCurr[cut] = dictCutInh[cut]

        ### returns the merged dict
        return dictCutCurr;

      return self.getCutDict ( self.linesCfg[line["Inherit"]] )



  
    def filterTisTos(self,
                     name,
                     DiMuonInput,
                     myTisTosSpecs ) :

        from Configurables import TisTosParticleTagger
        if self.dbug:
          print "##  TOS requirement applied"
        
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




# =============================================================================
## logging
# =============================================================================
import logging
logger = logging.getLogger(__name__)
if not logger.handlers : logging.basicConfig()
logger.setLevel(logging.INFO)
# =============================================================================
# =============================================================================
if '__main__' == __name__ :
    
    
    logger.info ( 80*'*'  ) 
    logger.info (  __doc__ ) 
    logger.info ( ' Author :  %s' % __author__ ) 
    logger.info ( ' Date   :  %s' % __date__   )
    ##
    clines = set() 
    logger.info ( ' Lines declared in default_config["STREAMS"] are' )
    for stream in default_config['STREAMS'] :
        lines = default_config['STREAMS'][stream] 
        for l in lines :
            logger.info ( ' %-15s : %-50s ' % ( stream , l ) )
            clines.add ( l )
    ##
    logger.info ( ' The output locations for the default configuration: ' )
    ##
    config = default_config['CONFIG']
    config['Debug'] = True
    _conf = DiMuonInherit ( 'DiMuonInherit' ,  config = config)
    ##
    _ln   = ' ' + 61*'-' + '+' + 30*'-'
    logger.info ( _ln ) 
    logger.info ( '  %-60s| %-30s  ' % ( 'Output location', 'Stripping line name' ) ) 
    logger.info ( _ln )
    for l in _conf.lines() :
        lout  = l.outputLocation()
        lname = l.name() 
        logger.info ( '  %-60s| %-30s  ' % ( lout, lname ) )
        if not lname in clines :
            raise AttributeError ('Unknown Line %s' % lname )
        clines.remove ( lname )
    logger.info ( _ln ) 
    logger.info ( 80*'*'  ) 
    if clines :
        raise AttributeError('Undeclared lines: %s' % clines )

    ## make dot-graphs 
    try:    
        selections = _conf._selections_private() 
        for s in selections :
            from SelPy.graph import graph
            o = graph ( s , format = 'png' )
            if o : logger.info  ( "Generate DOT-graph: %s"          % o        )
            else : logger.error ( "Can't produce DOT=-graph for %s" % s.name() )
            
    except : pass
        
# =============================================================================
# The END 
# =============================================================================


