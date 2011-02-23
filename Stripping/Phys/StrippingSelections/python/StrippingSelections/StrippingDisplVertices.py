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
from PhysSelPython.Wrappers import Selection, DataOnDemand,AutomaticData
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
import GaudiKernel.SystemOfUnits as units


__all__ = ('DisplVerticeLinesConf',
           'confdict')

confdict = {
    'NCands':{
       'SinglePS': 1 ,
       'SingleLowMass': 1 ,
       'SingleHighMass': 1 ,
       'SingleDown': 1 ,
       'Double':2
    },
    'RCutMethod' : 'FromUpstreamPV',
    'MinR':{
       'PreselVelo':     0.3*units.mm , 
       'SinglePS': 0.3*units.mm ,
       'SingleLowMass': 0.3*units.mm ,
       'SingleHighMass': 0.3*units.mm ,
       'SingleDown': 0.3*units.mm ,
       'Double': 0.3*units.mm
    },
    'MinMass':{
       'PreselVelo':     3.*units.GeV , 
       'SinglePS': 5.*units.GeV  ,
       'SingleLowMass': 6.8*units.GeV ,
       'SingleHighMass':  12.*units.GeV,
       'SingleDown':  3.*units.GeV,
       'Double': 3.*units.GeV 
    },
    'MinSumPt':{
       'PreselVelo':     3.*units.GeV ,
       'SinglePS': 5.*units.GeV  ,
       'SingleLowMass': 6.*units.GeV ,
       'SingleHighMass':  6.*units.GeV,
       'SingleDown':  3.2*units.GeV,
       'Double': 3.*units.GeV
    },
    'MaxMass':{
       'SinglePS': 14*units.TeV ,
       'SingleLowMass': 12.*units.GeV,
       'SingleHighMass':  14*units.TeV ,
       'SingleDown':  14*units.TeV,
       'Double': 14*units.TeV
    },
    'MaxSumPt':{
       'SinglePS': 14*units.TeV ,
       'SingleLowMass': 14*units.TeV,
       'SingleHighMass':  14*units.TeV ,
       'SingleDown':  14*units.TeV ,
       'Double': 14*units.TeV
    },
    'NTracks':{
       'PreselVelo':     4,
       'SinglePS': 5,
       'SingleLowMass': 6,
       'SingleHighMass':  5 ,
       'SingleDown':  4 ,
       'Double': 4
    },
    'MinZ':{
       'SinglePS': -10*units.m,
       'SingleLowMass':  -10*units.m,
       'SingleHighMass':   -10*units.m ,
       'SingleDown': 100*units.mm  ,
       'Double': -10*units.m
    },
    'MaterialVeto':{
       'PreselVelo':     5 ,
       'SinglePS': 0,
       'SingleLowMass': 5,
       'SingleHighMass':  0 ,
       'SingleDown':  0 ,
       'Double': 0
    },
    'prescale':{
       'SinglePS': 0.05,
       'SingleLowMass': 1.,
       'SingleHighMass':  1. ,
       'SingleDown':  1. ,
       'Double': 1.
    }
 }


class DisplVerticeLinesConf(LineBuilder) :
    """
    """

    __configuration_keys__ = (
            'NCands'
            ,'RCutMethod'
            ,'MinR'
            ,'MinMass'
            ,'MinSumPt'
            ,'MaxMass'
            ,'MaxSumPt'
            ,'NTracks'
            ,'MinZ'
            ,'MaterialVeto'
            ,'prescale'
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
        self.singlePSLine = self._makeDVLine('SinglePS', name)
        self.singleLowMassLine = self._makeDVLine('SingleLowMass', name)
        self.singleHighMassLine = self._makeDVLine('SingleHighMass', name)
        self.doubleLine = self._makeDVLine('Double', name)
        self.downstreamLine = self._makeDVDownLine('SingleDown', name)
        self.registerLine(self.singlePSLine)
        self.registerLine(self.singleLowMassLine)
        self.registerLine(self.singleHighMassLine)
        self.registerLine(self.doubleLine)
        self.registerLine(self.downstreamLine)
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
        _veloVertices.PVOfflineTool.addTool(PVSeed3DTool())
        _veloVertices.PVOfflineTool.PVSeed3DTool.MinCloseTracks = 3
        _veloVertices.PVOfflineTool.addTool( LSAdaptPV3DFitter())
        _veloVertices.PVOfflineTool.PVFitterName = "LSAdaptPV3DFitter"
        _veloVertices.PVOfflineTool.LSAdaptPV3DFitter.maxIP2PV = 2*units.mm
        _veloVertices.PVOfflineTool.LSAdaptPV3DFitter.MinTracks = 4
        return Selection( name+ 'DV3D',
                          Algorithm = _veloVertices,
                          Extension="RecVertices",
                          OutputBranch="Rec",
                          InputDataSetter=None)

    def _dvDownstreamTrackSelection( self,  name ):
        from Configurables import CopyDownstreamTracks
        _CopyDownstream = CopyDownstreamTracks( name+ "DownstreamTrCpy" )
        _CopyDownstream.TrackLocation = "Rec/Track/Best"
        _CopyDownstream.DownstreamTrackLocation = 'Rec/'+name+'DownstreamTr/Tracks'
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
        _downVertices.PVOfflineTool.addTool( LSAdaptPVFitter())
        _downVertices.PVOfflineTool.InputTracks = ['Rec/'+name+'DownstreamTr/Tracks']
        _downVertices.PVOfflineTool.RequireVelo = False
        _downVertices.PVOfflineTool.PVsChi2Separation = 0
        _downVertices.PVOfflineTool.PVsChi2SeparationLowMult = 0
        _downVertices.PVOfflineTool.addTool(PVSeed3DTool())
        _downVertices.PVOfflineTool.PVSeedingName = "PVSeed3DTool"
        _downVertices.PVOfflineTool.PVSeed3DTool.MinCloseTracks = 4
        _downVertices.PVOfflineTool.PVSeed3DTool.TrackPairMaxDistance = 2*units.mm
        _downVertices.PVOfflineTool.PVSeed3DTool.zMaxSpread = 20*units.mm
        _downVertices.PVOfflineTool.addTool( LSAdaptPVFitter())
        _downVertices.PVOfflineTool.PVFitterName = "LSAdaptPVFitter"
        _downVertices.PVOfflineTool.LSAdaptPVFitter.MinTracks = 4
        _downVertices.PVOfflineTool.LSAdaptPVFitter.maxChi2 = 400.0
        _downVertices.PVOfflineTool.LSAdaptPVFitter.maxDeltaZ = 0.0005 *units.mm
        _downVertices.PVOfflineTool.LSAdaptPVFitter.maxDeltaChi2NDoF =  0.002
        _downVertices.PVOfflineTool.LSAdaptPVFitter.acceptTrack = 0.000000001
        _downVertices.PVOfflineTool.LSAdaptPVFitter.trackMaxChi2 = 9
        _downVertices.PVOfflineTool.LSAdaptPVFitter.trackMaxChi2Remove = 64
        return Selection( name+ 'DownDV3D',
                          Algorithm = _downVertices,
                          RequiredSelections = [ self.DownstreamTracks ],
                          Extension="RecVertices",
                          OutputBranch="Rec",
                          InputDataSetter=None)



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
        selectionAlg.PreyMaxMass = self.__confdict__['MaxMass'][lineName]
        selectionAlg.NbTracks = self.__confdict__['NTracks'][lineName]
        selectionAlg.RemVtxFromDet = self.__confdict__['MaterialVeto'][lineName]
        return Selection ( "Sel" + lineName + name  , Algorithm = selectionAlg , RequiredSelections = [ self.VeloDVPreselection ] )

    def _makeDVDownSelection(self , lineName , name ):
        from Configurables import DisplVertices
        selectionAlg = DisplVertices('Sel'+lineName+name+'Alg')
        selectionAlg.MinNBCands = self.__confdict__['NCands'][lineName]
        selectionAlg.RCutMethod = self.__confdict__['RCutMethod']
        selectionAlg.RMin = self.__confdict__['MinR'][lineName]
        selectionAlg.PreyMinMass = self.__confdict__['MinMass'][lineName]
        selectionAlg.PreyMinSumpt = self.__confdict__['MinSumPt'][lineName]
        selectionAlg.PreyMaxMass = self.__confdict__['MaxMass'][lineName]
        selectionAlg.NbTracks = self.__confdict__['NTracks'][lineName]
        selectionAlg.MinZ = self.__confdict__['MinZ'][lineName]
        ##selectionAlg.SaveTuple = True
        selectionAlg.RemVtxFromDet = self.__confdict__['MaterialVeto'][lineName]
        return Selection ( "Sel" + lineName + name  , Algorithm = selectionAlg , RequiredSelections = [ self.DownDVPreselection ] )

    def _makeDVLine(self,lineName,name):
        return StrippingLine( lineName+name ,
                              prescale = self.__confdict__['prescale'][lineName] ,
                              selection = self._makeDVSelection(lineName,name))
    
    def _makeDVDownLine(self,lineName,name):
        return StrippingLine( lineName+name ,
                              prescale = self.__confdict__['prescale'][lineName] ,
                              selection = self._makeDVDownSelection(lineName,name))

## NEW
##  |                                    *Decision name*|  *Rate*|*Accepted*| *Mult*|*ms/evt*| *Errs*|*Incds*| *Slow*|
##  |_StrippingGlobal_                                  |0.001021|        47|       |   3.646|       |       |       |
##  |_StrippingSequenceStreamTestDV_                    |0.001021|        47|       |   3.640|       |       |       |
##  |!StrippingSinglePSDisplVtx                         |0.000195|         9|  0.000|   0.248|      0|      0|      1|
##  |!StrippingSingleLowMassDisplVtx                    |0.000413|        19|  0.000|   2.876|      0|      0|      5|
##  |!StrippingSingleHighMassDisplVtx                   |0.000304|        14|  0.000|   0.037|      0|      0|      0|
##  |!StrippingDoubleDisplVtx                           |0.000130|         6|  0.000|   0.034|      0|      0|      0|
                         



 

## OLD
##  |                                    *Decision name*|  *Rate*|*Accepted*| *Mult*|*ms/evt*| *Errs*|*Incds*| *Slow*|
##  |_StrippingGlobal_                                  |0.001064|        49|       |   3.693|       |       |       |
##  |_StrippingSequenceStreamTestDV_                    |0.001064|        49|       |   3.689|       |       |       |
##  |!StrippingSinglePSDisplVtx                         |0.000195|         9|  0.000|   0.252|      0|      0|      2|
##  |!StrippingSingleLowMassDisplVtx                    |0.000434|        20|  0.000|   2.923|      0|      0|     30|
##  |!StrippingSingleHighMassDisplVtx                   |0.000326|        15|  0.000|   0.034|      0|      0|      0|
##  |!StrippingDoubleDisplVtx                           |0.000130|         6|  0.000|   0.036|      0|      0|      0|


## Kaplan OLD
##  |                                    *Decision name*|  *Rate*|*Accepted*| *Mult*|*ms/evt*| *Errs*|*Incds*| *Slow*|
##  |_StrippingGlobal_                                  |0.276200|      1381|       |   3.652|       |       |       |
##  |_StrippingSequenceStreamTestDV_                    |0.276200|      1381|       |   3.646|       |       |       |
##  |!StrippingSinglePSDisplVtx                         |0.013400|        67|  0.000|   0.208|      0|      0|      0|
##  |!StrippingSingleLowMassDisplVtx                    |0.106800|       534|  0.000|   3.234|      0|      0|      4|
##  |!StrippingSingleHighMassDisplVtx                   |0.140400|       702|  0.000|   0.065|      0|      0|      0|
##  |!StrippingDoubleDisplVtx                           |0.131600|       658|  0.000|   0.066|      0|      0|      0|


## StrippingReport                                                INFO Event 5000, Good event 5000
##  |                                    *Decision name*|  *Rate*|*Accepted*| *Mult*|*ms/evt*| *Errs*|*Incds*| *Slow*|
##  |_StrippingGlobal_                                  |0.271600|      1358|       |   3.764|       |       |       |
##  |_StrippingSequenceStreamTestDV_                    |0.271600|      1358|       |   3.756|       |       |       |
##  |!StrippingSinglePSDisplVtx                         |0.013200|        66|  0.000|   0.205|      0|      0|      0|
##  |!StrippingSingleLowMassDisplVtx                    |0.105400|       527|  0.000|   3.336|      0|      0|      5|
##  |!StrippingSingleHighMassDisplVtx                   |0.137400|       687|  0.000|   0.074|      0|      0|      0|
##  |!StrippingDoubleDisplVtx                           |0.129200|       646|  0.000|   0.071|      0|      0|      0|
