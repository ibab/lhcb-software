__author__ = 'Greig Cowan'

from Gaudi.Configuration import *
from GaudiConf.Configuration import *
from Configurables import SelDSTWriter, DaVinci, FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import AutomaticData, SelectionSequence, MultiSelectionSequence, Selection

IODataManager().AgeLimit=2
# These lines are in the Dimuon stream

lines = { 'JpsiLines'	 : [   	'Jpsi2MuMuLine',
				'DiMuonSameSign_Prescale',
 				'DiMuonHighMassLine',
# 				'DiMuonHighMassSameSignLine'
				'FullDSTDiMuonIncLine',
				'FullDSTDiMuonSameSignLine'
				] }

def MakeXSeq( trunk, seq ):
	DiMuonFilter = FilterDesktop( "DimuonFilter" + seq,
            		       Code = "(2 == NINTREE(ISMUON & (PT > 0.7*GeV) & (P > 10*GeV))) & (MAXTREE('mu+'==ABSID, TRCHI2DOF) < 4 )" \
                  		" & (PT >2000) &((ADMASS('J/psi(1S)') < 80 ) | (ADMASS('psi(2S)') < 80 ) | (ADMASS('Upsilon(1S)') < 2000 ))")


	DiMuonSelection = Selection( "DimuonSelection" + seq,
            		     Algorithm = DiMuonFilter,
            		     RequiredSelections = [ AutomaticData (Location = trunk + '/' + seq ) ])

        CombinationCut = "in_range(3400, AM, 4150)"
        MotherCut = " (VFASPF(VCHI2PDOF) < 5.)"
        DaughtersCuts = { "pi+" : " (TRCHI2DOF < 4.) & (PT > 300.*MeV)"	} 

#         DaughtersCuts = { "pi+" : " (TRCHI2DOF < 4.) & (PT > 300.*MeV)",
# 			  "J/psi(1S)" : "MFIT"} # need to check that MFIT is doing what we think it should 
	
	XCombineRS = CombineParticles( "XCombineRS" + seq,
                                 DecayDescriptor = "X_1(3872) -> J/psi(1S) pi+ pi-",
                                 DaughtersCuts = DaughtersCuts,
                                 MotherCut = MotherCut,
                                 CombinationCut = CombinationCut)
	
	XCombineWS = CombineParticles( "XCombineWS" + seq,
				 DecayDescriptors = ["X_1(3872) -> J/psi(1S) pi+ pi+", "X_1(3872) -> J/psi(1S) pi- pi-"], # Greig advice

                                 DaughtersCuts = DaughtersCuts,
                                 MotherCut = MotherCut,
                                 CombinationCut = CombinationCut)
	
	XSelectionRS = Selection( "XSelectionRS" + seq,
				Algorithm = XCombineRS,
				RequiredSelections = [ DiMuonSelection,
						       AutomaticData(Location =  'Phys/StdLoosePions') ] )

	XSelectionWS = Selection( "XSelectionWS" + seq,
				Algorithm = XCombineWS,
				RequiredSelections = [ DiMuonSelection, 
                                        	       AutomaticData(Location = 'Phys/StdLoosePions') ] )

	
	return [SelectionSequence( "XSelSeqRS" + seq, TopSelection = XSelectionRS ), SelectionSequence( "XSelSeqWS" + seq, TopSelection = XSelectionWS )]


def selectionSequences( lines, trunk = '/Event/Dimuon/Phys' ): #Dimuon stream
# def selectionSequences( lines, trunk = '/Event/LeptonicFull/Phys' ): #full leptonic stream

    """
    Create a list of MultiSelectionSequences wrapping a list of TES locations.
    """
    selectionSequences = []
    for ( name, sequences ) in lines.items():
        seq = None
        seqs = []
        if len( sequences ) == 1:
            seq = SelectionSequence( name, TopSelection = AutomaticData (Location = trunk + '/' + sequences[ 0 ] ) )
        else:
	    for seq in sequences:
			Xsequences = MakeXSeq(trunk, seq)
			for x in Xsequences:
				seqs.append( x )
            seq = MultiSelectionSequence( name, Sequences = seqs )
        selectionSequences.append( seq )
    return selectionSequences

# Now create the DST
# The candiates are saved on the DST at the location /Event/Strip/JpsiLines/Dimuon/Phys/LineName
dstWriter = SelDSTWriter("MyDSTWriter",
	SelectionSequences = selectionSequences( lines ),
        OutputPrefix = 'Strip',
	OutputFileSuffix = 'EPFL'
        )

#DaVinci settings

DaVinci().UserAlgorithms = [ dstWriter.sequence() ]
DaVinci().DataType  = "2010"
DaVinci().EvtMax    = -1
DaVinci().PrintFreq = 1000
DaVinci().Lumi = True
DaVinci().CondDBtag = 'head-20101112'

# from Configurables import CondDB
# CondDB().UseOracle = True

#EventSelector().Input   = [
#"   DATAFILE='PFN:/home/jbressie/X3872/00008395_00000889_1.dimuon.dst' TYP='POOL_ROOTTREE' OPT='READ'"
# "   DATAFILE='PFN:/castor/cern.ch/grid/lhcb/data/2010/DIMUON.DST/00008381/0000/00008381_00000083_1.dimuon.dst' TYP='POOL_ROOTTREE' OPT='READ'", #worked
# "   DATAFILE='PFN:/castor/cern.ch/grid/lhcb/data/2010/DIMUON.DST/00008381/0000/00008381_00000040_1.dimuon.dst' TYP='POOL_ROOTTREE' OPT='READ'"

# "   DATAFILE='PFN:/castor/cern.ch/grid/lhcb/data/2010/DIMUON.DST/00008381/0000/00008381_00000055_1.dimuon.dst' TYP='POOL_ROOTTREE' OPT='READ'",
# "   DATAFILE='PFN:/castor/cern.ch/grid/lhcb/data/2010/DIMUON.DST/00008381/0000/00008381_00000056_1.dimuon.dst' TYP='POOL_ROOTTREE' OPT='READ'",
# "   DATAFILE='PFN:/castor/cern.ch/grid/lhcb/data/2010/DIMUON.DST/00008381/0000/00008381_00000057_1.dimuon.dst' TYP='POOL_ROOTTREE' OPT='READ'",
# "   DATAFILE='PFN:/castor/cern.ch/grid/lhcb/data/2010/DIMUON.DST/00008381/0000/00008381_00000058_1.dimuon.dst' TYP='POOL_ROOTTREE' OPT='READ'",
# "   DATAFILE='PFN:/castor/cern.ch/grid/lhcb/data/2010/DIMUON.DST/00008381/0000/00008381_00000059_1.dimuon.dst' TYP='POOL_ROOTTREE' OPT='READ'",
# "   DATAFILE='PFN:/castor/cern.ch/grid/lhcb/data/2010/DIMUON.DST/00008381/0000/00008381_00000060_1.dimuon.dst' TYP='POOL_ROOTTREE' OPT='READ'",
# "   DATAFILE='PFN:/castor/cern.ch/grid/lhcb/data/2010/DIMUON.DST/00008381/0000/00008381_00000061_1.dimuon.dst' TYP='POOL_ROOTTREE' OPT='READ'",
# "   DATAFILE='PFN:/castor/cern.ch/grid/lhcb/data/2010/DIMUON.DST/00008381/0000/00008381_00000062_1.dimuon.dst' TYP='POOL_ROOTTREE' OPT='READ'",
# "   DATAFILE='PFN:/castor/cern.ch/grid/lhcb/data/2010/DIMUON.DST/00008381/0000/00008381_00000063_1.dimuon.dst' TYP='POOL_ROOTTREE' OPT='READ'",
# "   DATAFILE='PFN:/castor/cern.ch/grid/lhcb/data/2010/DIMUON.DST/00008381/0000/00008381_00000064_1.dimuon.dst' TYP='POOL_ROOTTREE' OPT='READ'",
# "   DATAFILE='PFN:/castor/cern.ch/grid/lhcb/data/2010/DIMUON.DST/00008381/0000/00008381_00000065_1.dimuon.dst' TYP='POOL_ROOTTREE' OPT='READ'",
# "   DATAFILE='PFN:/castor/cern.ch/grid/lhcb/data/2010/DIMUON.DST/00008381/0000/00008381_00000066_1.dimuon.dst' TYP='POOL_ROOTTREE' OPT='READ'",
# "   DATAFILE='PFN:/castor/cern.ch/grid/lhcb/data/2010/DIMUON.DST/00008381/0000/00008381_00000067_1.dimuon.dst' TYP='POOL_ROOTTREE' OPT='READ'",
# "   DATAFILE='PFN:/castor/cern.ch/grid/lhcb/data/2010/DIMUON.DST/00008381/0000/00008381_00000068_1.dimuon.dst' TYP='POOL_ROOTTREE' OPT='READ'",
# "   DATAFILE='PFN:/castor/cern.ch/grid/lhcb/data/2010/DIMUON.DST/00008381/0000/00008381_00000069_1.dimuon.dst' TYP='POOL_ROOTTREE' OPT='READ'",
# "   DATAFILE='PFN:/castor/cern.ch/grid/lhcb/data/2010/DIMUON.DST/00008381/0000/00008381_00000070_1.dimuon.dst' TYP='POOL_ROOTTREE' OPT='READ'",
# "   DATAFILE='PFN:/castor/cern.ch/grid/lhcb/data/2010/DIMUON.DST/00008381/0000/00008381_00000071_1.dimuon.dst' TYP='POOL_ROOTTREE' OPT='READ'",
# "   DATAFILE='PFN:/castor/cern.ch/grid/lhcb/data/2010/DIMUON.DST/00008381/0000/00008381_00000072_1.dimuon.dst' TYP='POOL_ROOTTREE' OPT='READ'",
# "   DATAFILE='PFN:/castor/cern.ch/grid/lhcb/data/2010/DIMUON.DST/00008381/0000/00008381_00000073_1.dimuon.dst' TYP='POOL_ROOTTREE' OPT='READ'",
# "   DATAFILE='PFN:/castor/cern.ch/grid/lhcb/data/2010/DIMUON.DST/00008381/0000/00008381_00000074_1.dimuon.dst' TYP='POOL_ROOTTREE' OPT='READ'",
# "   DATAFILE='PFN:/castor/cern.ch/grid/lhcb/data/2010/DIMUON.DST/00008381/0000/00008381_00000075_1.dimuon.dst' TYP='POOL_ROOTTREE' OPT='READ'",
# "   DATAFILE='PFN:/castor/cern.ch/grid/lhcb/data/2010/DIMUON.DST/00008381/0000/00008381_00000076_1.dimuon.dst' TYP='POOL_ROOTTREE' OPT='READ'",
# "   DATAFILE='PFN:/castor/cern.ch/grid/lhcb/data/2010/DIMUON.DST/00008381/0000/00008381_00000077_1.dimuon.dst' TYP='POOL_ROOTTREE' OPT='READ'",
# "   DATAFILE='PFN:/castor/cern.ch/grid/lhcb/data/2010/DIMUON.DST/00008381/0000/00008381_00000078_1.dimuon.dst' TYP='POOL_ROOTTREE' OPT='READ'",
# "   DATAFILE='PFN:/castor/cern.ch/grid/lhcb/data/2010/DIMUON.DST/00008381/0000/00008381_00000079_1.dimuon.dst' TYP='POOL_ROOTTREE' OPT='READ'",
# "   DATAFILE='PFN:/castor/cern.ch/grid/lhcb/data/2010/DIMUON.DST/00008381/0000/00008381_00000080_1.dimuon.dst' TYP='POOL_ROOTTREE' OPT='READ'",
# "   DATAFILE='PFN:/castor/cern.ch/grid/lhcb/data/2010/DIMUON.DST/00008381/0000/00008381_00000081_1.dimuon.dst' TYP='POOL_ROOTTREE' OPT='READ'",
# "   DATAFILE='PFN:/castor/cern.ch/grid/lhcb/data/2010/DIMUON.DST/00008381/0000/00008381_00000082_1.dimuon.dst' TYP='POOL_ROOTTREE' OPT='READ'",
# "   DATAFILE='PFN:/castor/cern.ch/grid/lhcb/data/2010/DIMUON.DST/00008381/0000/00008381_00000083_1.dimuon.dst' TYP='POOL_ROOTTREE' OPT='READ'",
# "   DATAFILE='PFN:/castor/cern.ch/grid/lhcb/data/2010/DIMUON.DST/00008381/0000/00008381_00000084_1.dimuon.dst' TYP='POOL_ROOTTREE' OPT='READ'",
# "   DATAFILE='PFN:/castor/cern.ch/grid/lhcb/data/2010/DIMUON.DST/00008381/0000/00008381_00000085_1.dimuon.dst' TYP='POOL_ROOTTREE' OPT='READ'",
# "   DATAFILE='PFN:/castor/cern.ch/grid/lhcb/data/2010/DIMUON.DST/00008381/0000/00008381_00000086_1.dimuon.dst' TYP='POOL_ROOTTREE' OPT='READ'",
# "   DATAFILE='PFN:/castor/cern.ch/grid/lhcb/data/2010/DIMUON.DST/00008381/0000/00008381_00000087_1.dimuon.dst' TYP='POOL_ROOTTREE' OPT='READ'",
# "   DATAFILE='PFN:/castor/cern.ch/grid/lhcb/data/2010/DIMUON.DST/00008381/0000/00008381_00000088_1.dimuon.dst' TYP='POOL_ROOTTREE' OPT='READ'"
#]

