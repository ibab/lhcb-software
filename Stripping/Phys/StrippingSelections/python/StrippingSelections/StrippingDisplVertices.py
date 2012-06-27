
"""
Module for the selection of events with displaced vertices.

The selection of displaced vertices is achieved in 3 steps :
      - Reconstruction of all vertices with >= 4 tracks
        with PatPV3D, with optimized cuts for smaller vertices
        than PV's.
      - RecVertices2Particles loops on all reconstructed vertices.
        The one with lowest z (upstream) is not considered
        Vertices with at least one backward track are not considered.
        Vertices close to the beam line removed.
        To allow for kinematical cuts, association is done between the Velo
        tracks participating to the vertex and the best Particles
        (from Rec/Track/Best) with best PID.
        Vertices that are found to be close to the detector material
        could be possibly eliminated.
      - DisplVertices : basic cuts on the selected candidates
        2 kinds of cuts are applied to the set of displaced vertices :
           >=1 prey passing thighter cuts
               --> when hunting single long-lived particles
               --> Achieved in SingleDisplVtx
           >=2 preys passing looser cuts
               --> when looking for 2 particles, coming from a mother one.
               --> Achieved in DoubleDisplVtx

"""

__author__ = ['Neal Gauvin', 'Victor Coco']
__date__ = '21/02/2011'
__version__ = '$Revision: 3.0 $'


from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder

from GaudiKernel.SystemOfUnits import *




__all__ = ('DisplVerticeLinesConf',
           'PatPV3DWrapper',
           'confdict')

confdict = {
    'NCands':{
       'SinglePS': 1 ,
       'JPsiHLT': 1 ,
       'MinBias': 1 ,
       'SingleDown': 1 ,
       'Double':2,
       'SingleHighMass':1,
       'SingleHighFD':1,
       'SingleMedium':1
    },
    'RCutMethod' : 'FromBeamSpot',
    'MinR':{
       'PreselVelo':     0.4*mm ,
       'SinglePS': 0.4*mm ,
       'JPsiHLT': 0.7*mm ,
       'MinBias': 0.7*mm ,
       'SingleDown': 3.0*mm ,
       'Double': 1.0*mm ,
       'SingleHighMass': 1.5*mm , 
       'SingleHighFD': 5.*mm,
       'SingleMedium': 3.2*mm
    },
    'MinMass':{
       'PreselVelo': 3.*GeV , 
       'SinglePS': 3.*GeV ,  
       'JPsiHLT': 5.5*GeV , 
       'MinBias': 5.5*GeV , 
       'SingleDown': 4.5*GeV,
       'Double': 6.*GeV , 
       'SingleHighMass':  15.*GeV,
       'SingleHighFD': 7.*GeV,
       'SingleMedium': 10.*GeV
    },
    'MinMass2':{
       'Double': 7.*GeV 
    },
    'MinSumPt':{
       'PreselVelo': 3.*GeV , 
       'SinglePS': 3.*GeV , 
       'JPsiHLT': 3.*GeV , 
       'MinBias': 3.*GeV , 
       'SingleDown': 3.*GeV,
       'Double': 3.*GeV , 
       'SingleHighMass':  3.*GeV,
       'SingleHighFD': 3.*GeV,
       'SingleMedium': 3.*GeV
    },
    'NTracks':{
       'PreselVelo':     4,
       'SinglePS': 4 ,
       'JPsiHLT': 6 ,
       'MinBias': 4 ,
       'SingleDown': 4 ,
       'Double':6 ,
       'SingleHighMass': 6,
       'SingleHighFD': 7,
       'SingleMedium': 6
    },
    'MinZ':{   
       'SingleDown': 150.*mm  ,
    },
    'MaterialVeto':{
       'PreselVelo': 5,
       'SinglePS': 0 ,
       'JPsiHLT': 0 ,
       'MinBias': 0 ,
       'SingleDown': 0 ,
       'Double': 5 ,
       'SingleHighMass':5,
       'SingleHighFD':5,
       'SingleMedium': 5
    },
    'prescale':{
       'SinglePS': 0.005 ,
       'JPsiHLT': 1. ,
       'MinBias': 1. ,
       'SingleDown': 1. ,
       'Double': 1. ,
       'SingleHighMass':1.,
       'SingleHighFD':1.,
       'SingleMedium': 1.
    },
    'HLT':{
       'SinglePS': '' ,
       'SingleDown': '' ,
       'Double': '' ,
       'SingleHighMass':'',
       'SingleHighFD':"",
       'SingleMedium':'',
       'JPsiHLT':"HLT_PASS_RE('Hlt2DiMuonJPsiDecision')",
       'MinBias':"HLT_PASS_RE('Hlt1MBNoBiasDecision')",
       'SingleHLTPS':"HLT_PASS_RE('Hlt2DisplVertices(Single|SingleMV)PostScaledDecision')",
       'DoubleHLTPS':"HLT_PASS_RE('Hlt2DisplVerticesDoublePostScaledDecision')"
    }
 }

from SelPy.utils import (UniquelyNamedObject,
                         ClonableObject,
                         SelectionBase)

from PhysSelPython.Wrappers import checkName

class PatPV3DWrapper(UniquelyNamedObject,
                                            ClonableObject,
                                            SelectionBase) :

    def __init__(self,
                 name,
                 Algorithm,
                 Output,
                 RequiredSelection = []):
    
        checkName(name)
        UniquelyNamedObject.__init__(self, name)
        ClonableObject.__init__(self, locals())
        
        alg= Algorithm.clone(name,OutputVerticesName=Output)
        SelectionBase.__init__(self,
                               algorithm = alg,
                               outputLocation = Output,
                               requiredSelections = RequiredSelection )

## class CopyDownstreamTracksWrapper(UniquelyNamedObject,
##                                             ClonableObject,
##                                             SelectionBase) :

##     def __init__(self,
##                  name,
##                  Algorithm,
##                  Output):
    
##         checkName(name)
##         UniquelyNamedObject.__init__(self, name)
##         ClonableObject.__init__(self, locals())
        
##         alg= Algorithm.clone(name,=Output)
##         SelectionBase.__init__(self,
##                                algorithm = alg,
##                                outputLocation = Output,
##                                requiredSelections = RequiredSelection )
        


class DisplVerticeLinesConf(LineBuilder) :
    """
    """

    __configuration_keys__ = (
            'NCands'
            ,'RCutMethod'
            ,'MinR'
            ,'MinMass'
            ,'MinMass2'
            ,'MinSumPt'
            ,'NTracks'
            ,'MinZ'
            ,'MaterialVeto'
            ,'prescale'
            ,'HLT'
        )

    __confdict__={}
        

    def __init__(self, name, config) :
        LineBuilder.__init__(self, name, config)
        self.__confdict__= config
        self.name = name
        '''
        Velo vertex reconstruction function  _dvVeloReco() is common to all lines using velo vertex
        Downstream vertex reconstruction function  _dvDownReco() is common to all lines using downstream vertex
        '''

        self.dvVeloReco = self._dvVeloReco(name)
        self.VeloDVPreselection = self._veloDVPreselection(name)
        self.DownstreamTracks =  self._dvDownstreamTrackSelection(name)
        self.dvDownReco = self._dvDownstreamReco(name)
        self.DownDVPreselection = self._downstreamDVPreselection(name)
        
        self.singlePSLine = self._makeDVLine('SinglePS', name,  hlt = self.__confdict__['HLT']['SinglePS'])
        self.downstreamLine = self._makeDVDownLine('SingleDown', name, hlt = self.__confdict__['HLT']['SingleDown'])
        self.singleHighMassLine = self._makeDVLine('SingleHighMass', name , hlt = self.__confdict__['HLT']['SingleHighMass'])
        self.singleHighFDLine = self._makeDVLine('SingleHighFD', name, hlt = self.__confdict__['HLT']['SingleHighFD'])
        self.singleMediumLine = self._makeDVLine('SingleMedium', name, hlt = self.__confdict__['HLT']['SingleMedium'])
        self.doubleLine = self._makeDVLine('Double', name, hlt = self.__confdict__['HLT']['Double'])
        self.doubleHLTPSLine =  self._makeDVLineFromTrigger('DoubleHLTPS', name, hlt = self.__confdict__['HLT']['DoubleHLTPS'])
        self.singleHLTPSLine =  self._makeDVLineFromTrigger('SingleHLTPS', name, hlt = self.__confdict__['HLT']['SingleHLTPS'])
        self.JPsiHLTLine =  self._makeDVLine('JPsiHLT', name, hlt = self.__confdict__['HLT']['JPsiHLT'])
        self.MinBias =  self._makeDVLine('MinBias', name, hlt = self.__confdict__['HLT']['MinBias'])
        
        self.registerLine(self.downstreamLine)
        self.registerLine(self.singlePSLine)
        self.registerLine(self.singleHighMassLine)
        self.registerLine(self.singleMediumLine)
        self.registerLine(self.singleHighFDLine)
        self.registerLine(self.doubleLine)
        self.registerLine(self.doubleHLTPSLine)
        self.registerLine(self.singleHLTPSLine)
        self.registerLine(self.JPsiHLTLine)
        self.registerLine(self.MinBias)
        
        self.Lines = []

       
    def _dvVeloReco( self,  name ):
        from Configurables import PatPV3D,PVOfflineTool,PVSeed3DTool,LSAdaptPV3DFitter
        _veloVertices = PatPV3D(name+'DV3DAlg')
        _veloVertices.OutputVerticesName = 'Rec/'+name+'DV3D/RecVertices'
        _veloVertices.addTool(PVOfflineTool)
        _veloVertices.PVOfflineTool.InputTracks = ["Rec/Track/Best"]
        _veloVertices.PVOfflineTool.PVsChi2Separation = 0
        _veloVertices.PVOfflineTool.PVsChi2SeparationLowMult = 0
        _veloVertices.PVOfflineTool.PVSeedingName = "PVSeed3DTool"
        _veloVertices.PVOfflineTool.addTool(PVSeed3DTool)
        _veloVertices.PVOfflineTool.PVSeed3DTool.MinCloseTracks = 3
        _veloVertices.PVOfflineTool.addTool( LSAdaptPV3DFitter )
        _veloVertices.PVOfflineTool.PVFitterName = "LSAdaptPV3DFitter"
        _veloVertices.PVOfflineTool.LSAdaptPV3DFitter.maxIP2PV = 2*mm
        _veloVertices.PVOfflineTool.LSAdaptPV3DFitter.MinTracks = 4
        return PatPV3DWrapper(name+ 'DV3D',_veloVertices,'Rec/'+name+'DV3D/RecVertices')
## return Selection( name+ 'DV3D',
##                           Algorithm = _veloVertices,
##                           Extension="RecVertices",
##                           OutputBranch="Rec",
##                           InputDataSetter=None)

    def _dvDownstreamTrackSelection( self,  name ):
        from Configurables import CopyDownstreamTracks
        _CopyDownstream = CopyDownstreamTracks( name+ "DownstreamTrCpy" )
        _CopyDownstream.Inputs = ["Rec/Track/Best"]
        _CopyDownstream.Output = 'Rec/'+name+'DownstreamTr/Tracks'
        #return PatPV3DWrapper(name+ 'DownstreamTr',_CopyDownstream,'Rec/'+name+'DownstreamTr/Tracks')
        return Selection( name+ 'DownstreamTr',
                          Algorithm = _CopyDownstream,
                          Extension="Tracks",
                          OutputBranch="Rec",
                          InputDataSetter=None)
        
    def _dvDownstreamReco( self,  name ):
        from Configurables import PatPV3D,PVOfflineTool,PVSeed3DTool,LSAdaptPVFitter
        _downVertices = PatPV3D(name+'DownDV3DAlg')
        _downVertices.OutputVerticesName = 'Rec/'+name+'DownDV3D/RecVertices'
        _downVertices.addTool(PVOfflineTool)
        _downVertices.PVOfflineTool.addTool( LSAdaptPVFitter )
        _downVertices.PVOfflineTool.InputTracks = ['Rec/'+name+'DownstreamTr/Tracks']
        _downVertices.PVOfflineTool.RequireVelo = False
        _downVertices.PVOfflineTool.PVsChi2Separation = 0
        _downVertices.PVOfflineTool.PVsChi2SeparationLowMult = 0
        _downVertices.PVOfflineTool.addTool(PVSeed3DTool )
        _downVertices.PVOfflineTool.PVSeedingName = "PVSeed3DTool"
        _downVertices.PVOfflineTool.PVSeed3DTool.MinCloseTracks = 4
        _downVertices.PVOfflineTool.PVSeed3DTool.TrackPairMaxDistance = 2*mm
        _downVertices.PVOfflineTool.PVSeed3DTool.zMaxSpread = 20*mm
        _downVertices.PVOfflineTool.addTool( LSAdaptPVFitter )
        _downVertices.PVOfflineTool.PVFitterName = "LSAdaptPVFitter"
        _downVertices.PVOfflineTool.LSAdaptPVFitter.MinTracks = 4
        _downVertices.PVOfflineTool.LSAdaptPVFitter.maxChi2 = 400.0
        _downVertices.PVOfflineTool.LSAdaptPVFitter.maxDeltaZ = 0.0005 *mm
        _downVertices.PVOfflineTool.LSAdaptPVFitter.maxDeltaChi2NDoF =  0.002
        _downVertices.PVOfflineTool.LSAdaptPVFitter.acceptTrack = 0.000000001
        _downVertices.PVOfflineTool.LSAdaptPVFitter.trackMaxChi2 = 9
        _downVertices.PVOfflineTool.LSAdaptPVFitter.trackMaxChi2Remove = 64
        
        return PatPV3DWrapper(name+'DownDV3D',_downVertices,'Rec/'+name+'DownDV3D/RecVertices',[ self.DownstreamTracks ])
##         return Selection( name+ 'DownDV3D',
##                           Algorithm = _downVertices,
##                           RequiredSelections = [ self.DownstreamTracks ],
##                           Extension="RecVertices",
##                           OutputBranch="Rec",
##                           InputDataSetter=None)



    def _veloDVPreselection(self , name ):
        from Configurables import RecVertices2Particles
        _makeVeloVerticesCand  = RecVertices2Particles("Presel"+name+"Alg")
        _makeVeloVerticesCand.RCutMethod = self.__confdict__['RCutMethod']
        _makeVeloVerticesCand.PreyMinMass =self. __confdict__['MinMass']['PreselVelo']
        _makeVeloVerticesCand.PreyMinSumpt = self.__confdict__['MinSumPt']['PreselVelo']
        _makeVeloVerticesCand.RMin =self.__confdict__['MinR']['PreselVelo']
        _makeVeloVerticesCand.NbTracks = self.__confdict__['NTracks']['PreselVelo']
        _makeVeloVerticesCand.RemVtxFromDet = self.__confdict__['MaterialVeto']['PreselVelo']
        _makeVeloVerticesCand.UsePartFromTES = False
        _makeVeloVerticesCand.TrackMaxChi2oNDOF = 5.
        return Selection( "Presel"+name ,
                          Algorithm = _makeVeloVerticesCand,
                          RequiredSelections = [self.dvVeloReco],
                          InputDataSetter='RecVerticesLocation' )

    def _downstreamDVPreselection(self , name ):
        from Configurables import RecVertices2Particles
        _makeDownVerticesCand  = RecVertices2Particles("PreselDown"+name+"Alg")
        _makeDownVerticesCand.RCutMethod = self.__confdict__['RCutMethod']
        _makeDownVerticesCand.PreyMinMass =self. __confdict__['MinMass']['PreselVelo']
        _makeDownVerticesCand.PreyMinSumpt = self.__confdict__['MinSumPt']['PreselVelo']
        _makeDownVerticesCand.RMin =self.__confdict__['MinR']['PreselVelo']
        _makeDownVerticesCand.NbTracks = self.__confdict__['NTracks']['PreselVelo']
        _makeDownVerticesCand.RemVtxFromDet = self.__confdict__['MaterialVeto']['PreselVelo']
        _makeDownVerticesCand.UsePartFromTES = False
        return Selection( "PreselDown"+name ,
                          Algorithm = _makeDownVerticesCand,
                          RequiredSelections = [self.dvDownReco],
                          InputDataSetter='RecVerticesLocation' )


    def _makeDVSelection(self , lineName , name ):
        from Configurables import DisplVertices
        selectionAlg = DisplVertices('Sel'+lineName+name+'Alg')
        selectionAlg.MinNBCands = self.__confdict__['NCands'][lineName]
        selectionAlg.RCutMethod = self.__confdict__['RCutMethod']
        selectionAlg.RMin = self.__confdict__['MinR'][lineName]
        selectionAlg.PreyMinMass = self.__confdict__['MinMass'][lineName]
        selectionAlg.PreyMinSumpt = self.__confdict__['MinSumPt'][lineName]
        if self.__confdict__['MinMass2'].has_key(lineName):
            selectionAlg.PreyMinMass2 =  self.__confdict__['MinMass2'][lineName]
        selectionAlg.NbTracks = self.__confdict__['NTracks'][lineName]
        #selectionAlg.SaveTuple = True
        selectionAlg.RemVtxFromDet = self.__confdict__['MaterialVeto'][lineName]
        return Selection ( "Sel" + lineName + name  , Algorithm = selectionAlg , RequiredSelections = [ self.VeloDVPreselection ] )
    
    def _makeEmptySelection(self , lineName , name ):
        return Selection ( "Sel" + lineName + name  , Algorithm = FilterDesktop( Code = "ALL" ) , RequiredSelections = [ self.VeloDVPreselection ] )

    def _makeDVDownSelection(self , lineName , name ):
        from Configurables import DisplVertices
        selectionAlg = DisplVertices('Sel'+lineName+name+'Alg')
        selectionAlg.MinNBCands = self.__confdict__['NCands'][lineName]
        selectionAlg.RCutMethod = self.__confdict__['RCutMethod']
        selectionAlg.RMin = self.__confdict__['MinR'][lineName]
        selectionAlg.PreyMinMass = self.__confdict__['MinMass'][lineName]
        selectionAlg.PreyMinSumpt = self.__confdict__['MinSumPt'][lineName]
        selectionAlg.NbTracks = self.__confdict__['NTracks'][lineName]
        selectionAlg.MinZ = self.__confdict__['MinZ'][lineName]
        selectionAlg.RemVtxFromDet = self.__confdict__['MaterialVeto'][lineName]
        return Selection ( "Sel" + lineName + name  , Algorithm = selectionAlg , RequiredSelections = [ self.DownDVPreselection ] )

    def _makeDVLine(self,lineName,name,hlt = ""):
        if hlt == "":
            return StrippingLine( lineName+name ,
                                  prescale = self.__confdict__['prescale'][lineName] ,
                                  selection = self._makeDVSelection(lineName,name))
        else :
            return StrippingLine( lineName+name ,
                                  prescale = self.__confdict__['prescale'][lineName] ,
                                  HLT = hlt,
                                  selection = self._makeDVSelection(lineName,name))

    
    def _makeDVLineFromTrigger(self,lineName,name,hlt):
        return StrippingLine( lineName+name ,
                              prescale = 1. ,
                              HLT = hlt,
                              selection = self._makeEmptySelection(lineName,name))
    
    
    def _makeDVDownLine(self,lineName,name,hlt = "" ):
        if hlt == "" :
            return StrippingLine( lineName+name ,
                                  prescale = self.__confdict__['prescale'][lineName] ,
                                  selection = self._makeDVDownSelection(lineName,name))
        else :
            return StrippingLine( lineName+name ,
                                  prescale = self.__confdict__['prescale'][lineName] ,
                                  HLT = hlt,
                                  selection = self._makeDVDownSelection(lineName,name))




