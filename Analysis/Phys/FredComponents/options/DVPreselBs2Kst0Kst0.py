__author__ = 'Celestino Rodriguez Cobo'
__date__ = 'July 20, 2009'

from Gaudi.Configuration import *
from StrippingConf.StrippingLine import StrippingLine, StrippingMember
from Configurables import FilterDesktop, CombineParticles, OfflineVertexFitter, OnOfflineTool
import GaudiKernel.SystemOfUnits as Units
#importOptions("$COMMONPARTICLESROOT/python/CommonParticles/StdLooseDetachedKstar.py")
importOptions("StdLooseDetachedKstarR.py")
#importOptions("$COMMONPARTICLESROOT/python/CommonParticles/StdLooseKstar.py")




#########
# Kstar #
#########

KstCuts = "(monitor(ADMASS('K*(892)0'),h7,'Ksmass')<150*MeV)"
KstCuts+= "&(monitor(PT,h8,'Kspt') > 500*MeV)"
KstCuts+= "&(monitor(MIPDV(PRIMARY),h3,'Ksip')>0.03*mm)"
KstCuts += "& (INTREE( (ABSID=='K+') & (monitor(MIPDV(PRIMARY),h10,'kpiip')>0.03*mm)"
KstCuts += "& (monitor(PT ,h9,'kpipt')>200*MeV) & (monitor(P,h11,'kpiptot')>1000*MeV)))"
KstCuts += "& (INTREE((ABSID=='pi-') & (monitor(MIPDV(PRIMARY),h10,'kpiip' )>0.03*mm)"
KstCuts += "& (monitor(PT,h9,'kpipt')>200*MeV) & (monitor(P,h11,'kpiptot')>1000*MeV)))"

######
# Bs #
######

Bs2KstKstDet = CombineParticles("Bs2KstKstDet")
Bs2KstKstDet.DecayDescriptor = " B_s0 -> K*(892)0 K*(892)~0"
Bs2KstKstDet.InputLocations = ["Phys/StdLooseDetachedKst2Kpi"]
# Set the OfflineVertexFitter to keep the 4 tracks and not the J/Psi Kstar:
Bs2KstKstDet.addTool( OfflineVertexFitter() )
Bs2KstKstDet.VertexFitters.update( { "" : "OfflineVertexFitter"} )
Bs2KstKstDet.OfflineVertexFitter.useResonanceVertex = False
Bs2KstKstDet.addTool(OnOfflineTool())
Bs2KstKstDet.OnOfflineTool.OfflinePVRelatorName = 'GenericParticle2PVRelator__p2PVWithIP_OfflineDistanceCalculatorName_'
Bs2KstKstDet.ReFitPVs = True
Bs2KstKstDet.HistoProduce = True
Bs2KstKstDet.Preambulo += [ "h1 = Gaudi.Histo1DDef( 'Bmass ', 0 , 1000 , 100  ) " , "h2 = Gaudi.Histo1DDef( 'Bs IP mm ' , 0 , 5 , 100 ) " ,"h3 = Gaudi.Histo1DDef( 'Ks IP mm' , 0 , 3 , 100 ) ","h4 = Gaudi.Histo1DDef( 'dist mm ' , 0 , 10 , 100 ) ","h5 = Gaudi.Histo1DDef( 'dist chi2' , 0 , 10 , 100 ) ","h6 = Gaudi.Histo1DDef( 'BVert chi2/dof ' , 0 , 50 , 100 ) ","h7 = Gaudi.Histo1DDef( 'Kst mass d' , 0 , 300 , 100 ) ","h8 = Gaudi.Histo1DDef( 'Kst Pt ' , 0 , 6000 , 100 ) ","h9 = Gaudi.Histo1DDef( 'Kpi Pt ' , 0 , 6000 , 100 ) ","h10 = Gaudi.Histo1DDef( 'Kpi IP mm ' , 0 , 3, 100 ) ","h11 = Gaudi.Histo1DDef( 'Kpi Ptot ' , 0 , 50000 , 100 ) "]

Bs2KstKstDet.DaughtersCuts = {"K*(892)0" : KstCuts,"K*(892)~0" : KstCuts }
                               
Bs2KstKstDet.CombinationCut = "monitor( ADAMASS('B_s0'),h1,'histoBm') < 500.*MeV"

Bs2KstKstDet.MotherCut = "(monitor(MIPDV(PRIMARY),h2,'BsIPmm')<0.32*mm)& (monitor(BPVVDSIGN,h4,'distbmm') > 0.1*mm)"# & (monitor(VFASPF(VCHI2/VDOF),h6,'Vchi2'))"




############################################
# Create StrippingLine with this selection #
############################################
"""
Bd2JpsiKstDetLine = StrippingLine('Bd2JpsiKstDetLine'
               , prescale = 1
               , algos = [Bd2JpsiKstDet]
               , stream = 'BExclusive'
            )
"""
