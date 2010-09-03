# $Id: StrippingNoPIDDstarWithD02RSKPi.py,v 1.1 2010-09-03 22:38:21 pxing Exp $

__author__ = ['Philip Xing', 'Andrew Powell']
__date__ = '03 September 2010'
__version__ = '$Revision: 1.1 $'

'''
Configurable for the RICH calibration using D*+ -> pi+ D0( K- pi+).
'''

from Gaudi.Configuration import *
from LHCbKernel.Configuration import *

class StrippingNoPIDDstarWithD02RSKPiConf(LHCbConfigurableUser) : # {
    """
    Definition of prompt D*+ -> D0( K- pi+) pi+ stripping lines.
    The the main user method is lines(), which returns a list of the
    StrippingLine objects defined and configured by this configurable.
    """
    __slots__ = {
                    'DaugPt'           : 0.300          ## GeV
                  , 'DaugP'            : 2.0            ## GeV
                  , 'DaugIPChi2'       : 16              ## unitless
                  , 'DaugTrkChi2'      : 10             ## unitless
                  , 'D0Pt'             : 1.5            ## GeV
                  , 'D0VtxChi2Ndof'    : 13             ## unitless
                  , 'D0PVDisp'         : 0.9            ## mm
                  , 'D0BPVDira'        : 0.9999         ## unitless
                  , 'SlowPiPt'         : 0.150          ## GeV
                  , 'DstarPt'          : 2.2            ## GeV
                  , 'DstarVtxChi2Ndof' : 13             ## unitless
                  , 'LinePrefix'       : 'StripDstarNoPID'
                }


    _rsLine = None

    ######## Line definition
    def lines(self) : # {
        """
        The the main user method that returns a list of the StrippingLine
        objects defined and configured by this configurable.
        """
        lclLineRS = self.lineRS()
        lineList = [ lclLineRS ]
        return lineList
    # }


    def lineRS(self) : # {
        """
        Returns a stripping line for the D0 -> K- pi+ (right sign).
        """
        
        if StrippingNoPIDDstarWithD02RSKPiConf._rsLine == None : 
        
    	    from StrippingConf.StrippingLine import StrippingLine

    	    lineName = "%(LinePrefix)sWithD02RSKPiLine" % self.getProps()
    	    lineSeq = self.seqWithD02RSKPi()

    	    StrippingNoPIDDstarWithD02RSKPiConf._rsLine = StrippingLine(lineName, algos = [ lineSeq ])
    	
        return StrippingNoPIDDstarWithD02RSKPiConf._rsLine
    # }


    ######## D* selection sequencer
    def seqWithD02RSKPi(self) : # {
        """
        Returns a sequence (or rather, a bindMembers object, which is a
        Stripping/Hlt extension for sequences) for the
        D*+ -> D0(K-pi+) pi+ reconstruction.
        """
        from StrippingConf.StrippingLine import bindMembers

        ## Make sure that StdNoPIDsPions has been registered with the
        ##  DoD service.
        from CommonParticles.StdNoPIDsPions import StdNoPIDsPions

        d0Seq = self.seqD02RSKPi()   ## Seq for D0 -> K- pi+ reconstruction
        dstar = self.combRSDstar()   ## CombineParticles for D*+ -> D0 pi+

        dstar.InputLocations = [ 'StdNoPIDsPions', d0Seq.outputLocation() ]

        seqName = "%(LinePrefix)sWithD02RSKPiSeq" % self.getProps()

        ## Since StdNoPIDsPions is DoD, no explicit dependence in the sequence
        seq =  bindMembers(seqName, algos = [d0Seq, dstar])

        return seq
    # }


    def combRSDstar(self) : # {
        """
        Returns a CombineParticles for D*+ -> D0 pi+ reconstruction.
        """
        from Configurables import CombineParticles

        slowPiCuts = "(PT>%(SlowPiPt)s*GeV)" % self.getProps()
        d0Cuts =     "(ALL)"
        ## Hard-code the mass windows for now.
        combiCut =  "(APT>%(DstarPt)s*GeV) & (ADAMASS('D*(2010)+')<75*MeV)" % self.getProps()
        motherCut =  "(VFASPF(VCHI2PDOF)<%(DstarVtxChi2Ndof)s) & (M-MAXTREE('D0'==ABSID,M)<(145.5+15)*MeV)" % self.getProps()

        combName = "%(LinePrefix)sWithD02RSKPiComb" % self.getProps()

        ## InputLocations must be defined by calling method.
        comb = CombineParticles( combName )
        comb.DecayDescriptor = '[D*(2010)+ -> D0 pi+]cc'
        comb.DaughtersCuts = { 'pi+' : slowPiCuts, 'D0' : d0Cuts }
        comb.CombinationCut = combiCut
        comb.MotherCut = motherCut

        return comb
    # }


    ######## D0 selection sequencer
    def seqD02RSKPi(self) : # {
        """
        Returns a sequence (or rather, a bindMembers object, which is a
        Stripping/Hlt extension for sequences) for the D0 -> K- pi+
        reconstruction.
        """
        from StrippingConf.StrippingLine import bindMembers

        d0Recon = self.filtRSD0()   ## FilterDesktop for D0 -> K- pi+

        seqName = "%(LinePrefix)sD02RSKPiSeq" % self.getProps()

        ## Since the base list StdLooseD02KPi is DoD, no explicit dependence
        ##   in the sequence
        seq =  bindMembers(seqName, algos = [d0Recon])

        return seq
    # }


    ###############################################################################
    ### D0 selections
    ###############################################################################
    def filtRSD0(self) : # {
        """
        D0 -> K- pi+ reconstruction.
        """
        from Configurables import CombineParticles
        from CommonParticles.StdNoPIDsPions import StdNoPIDsPions
        from CommonParticles.StdNoPIDsKaons import StdNoPIDsKaons

        PiCuts = "(PT>%(DaugPt)s*GeV) & (P>%(DaugP)s*GeV) & (TRCHI2DOF<%(DaugTrkChi2)s) & (MIPCHI2DV(PRIMARY)>%(DaugIPChi2)s)" % self.getProps()
        KCuts  = "(PT>%(DaugPt)s*GeV) & (P>%(DaugP)s*GeV) & (TRCHI2DOF<%(DaugTrkChi2)s) & (MIPCHI2DV(PRIMARY)>%(DaugIPChi2)s)" % self.getProps()
        combiCut =  "(ADAMASS('D0')<75*MeV)" % self.getProps() ## Hard-code the mass windows for now.
        motherCut =  "(PT>%(D0Pt)s*GeV) & (VFASPF(VCHI2PDOF)<%(D0VtxChi2Ndof)s) & (BPVVD>%(D0PVDisp)s) & (BPVDIRA>%(D0BPVDira)s)" % self.getProps()

        combName = "%(LinePrefix)sD02RSKPiFilt" % self.getProps()

        comb = CombineParticles( combName )
        comb.DecayDescriptor = '[D0 -> pi+ K-]cc'
        comb.DaughtersCuts = { 'pi+' : PiCuts, 'K-' : KCuts }
        comb.CombinationCut = combiCut
        comb.MotherCut = motherCut

        comb.InputLocations = [ 'StdNoPIDsPions', 'StdNoPIDsKaons' ]
        return comb
    # }


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


# }

