# $Id: StrippingDstarPromptWithD02HH.py,v 1.7 2010-09-04 22:12:02 pxing Exp $

__author__ = ['Philip Xing', 'Patrick Spradlin']
__date__ = '03 September 2010'
__version__ = '$Revision: 1.7 $'

'''
Configurable for the y_CP analysis selections for D*+ -> pi+ D0(h h'').

This configurable uses bindMembers to compose sequences from StrippingMembers
in the same way that it is used (or perhaps abused) in Hlt2.

The initial implementation as a configurable borrows from StrippingBu2LLK.py
Rev. 1.2.
'''

from Gaudi.Configuration import *
from LHCbKernel.Configuration import *

class StrippingDstarPromptWithD02HHConf(LHCbConfigurableUser) : # {
    """
    Definition of prompt D*+ -> D0(h h') pi+ stripping lines.

    The the main user method is lines(), which returns a list of the
    StrippingLine objects defined and configured by this configurable.

    Standard or not, 4-space indentation is silly.
    """
    __slots__ = {
                    'DaugPt'           : 0.900          ## GeV
                  , 'DaugP'            : 5.0            ## GeV
                  , 'DaugTrkChi2'      : 5             ## unitless
                  , 'D0Pt'             : 3.3            ## GeV
                  , 'D0VtxChi2Ndof'    : 13             ## unitless
                  , 'D0PVDisp'         : 0.9             ## mm
                  , 'SlowPiPt'         : 0.260          ## GeV
                  , 'DstarPt'          : 3.6            ## GeV
                  , 'DstarVtxChi2Ndof' : 13             ## unitless
                  , 'LinePrefix'       : 'StripDstarPrompt'
                }
                
    _rsLine = None
    _wsLine = None
    _kkLine = None
    _pipiLine = None

    def lines(self) : # {
        """
        The the main user method that returns a list of the StrippingLine
        objects defined and configured by this configurable.
        """
        lclLineRS = self.lineRS()
        lclLineWS = self.lineWS()
        lclLineKK = self.lineKK()
        lclLinePiPi = self.linePiPi()

        lineList = [ lclLineRS, lclLineWS, lclLineKK, lclLinePiPi ]

        return lineList
    # }


    def linesDstarOnly(self) : # {
        """
        The the main user method that returns a list of the StrippingLine
        objects defined and configured by this configurable.
        """
        lclLineRS = self.lineRS()
        lclLineWS = self.lineWS()
        lclLineKK = self.lineKK()
        lclLinePiPi = self.linePiPi()

        lineList = [ lclLineRS, lclLineWS, lclLineKK, lclLinePiPi ]

        return lineList
    # }



    def lineRS(self) : # {
        """
        Returns a stripping line for the D0 -> K- pi+ (right sign).
        """
        
        if StrippingDstarPromptWithD02HHConf._rsLine == None : 
        
    	    from StrippingConf.StrippingLine import StrippingLine

    	    lineName = "%(LinePrefix)sWithD02RSKPiLine" % self.getProps()
    	    lineSeq = self.seqWithD02RSKPi()

    	    StrippingDstarPromptWithD02HHConf._rsLine = StrippingLine(lineName, algos = [ lineSeq ])
    	
        return StrippingDstarPromptWithD02HHConf._rsLine
    # }


    def lineWS(self) : # {
        """
        Returns a stripping line for the D0 -> pi- K+ (wrong sign).
        """
        
        if StrippingDstarPromptWithD02HHConf._wsLine == None : 
    	    from StrippingConf.StrippingLine import StrippingLine

    	    lineName = "%(LinePrefix)sWithD02WSKPiLine" % self.getProps()
    	    lineSeq = self.seqWithD02WSKPi()

    	    StrippingDstarPromptWithD02HHConf._wsLine = StrippingLine(lineName, algos = [ lineSeq ])

        return StrippingDstarPromptWithD02HHConf._wsLine
    # }


    def lineKK(self) : # {
        """
        Returns a stripping line for the D0 -> K- K+.
        """
        
        if StrippingDstarPromptWithD02HHConf._kkLine == None : 
        
    	    from StrippingConf.StrippingLine import StrippingLine

    	    lineName = "%(LinePrefix)sWithD02KKLine" % self.getProps()
    	    lineSeq = self.seqWithD02KK()

    	    StrippingDstarPromptWithD02HHConf._kkLine = StrippingLine(lineName, algos = [ lineSeq ])

        return StrippingDstarPromptWithD02HHConf._kkLine
    # }


    def linePiPi(self) : # {
        """
        Returns a stripping line for the D0 -> pi- pi+.
        """
        
        if StrippingDstarPromptWithD02HHConf._pipiLine == None : 
        
    	    from StrippingConf.StrippingLine import StrippingLine

    	    lineName = "%(LinePrefix)sWithD02PiPiLine" % self.getProps()
    	    lineSeq = self.seqWithD02PiPi()

    	    StrippingDstarPromptWithD02HHConf._pipiLine = StrippingLine(lineName, algos = [ lineSeq ])

        return StrippingDstarPromptWithD02HHConf._pipiLine
    # }



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


    def seqWithD02WSKPi(self) : # {
        """
        Returns a sequence (or rather, a bindMembers object, which is a
        Stripping/Hlt extension for sequences) for the
        D*+ -> D0(pi-K+) pi+ reconstruction.
        """
        from StrippingConf.StrippingLine import bindMembers

        ## Make sure that StdNoPIDsPions has been registered with the
        ##  DoD service.
        from CommonParticles.StdNoPIDsPions import StdNoPIDsPions

        d0Seq = self.seqD02WSKPi()   ## Seq for D0 -> pi- K+ reconstruction
        rsDstar = self.combRSDstar()   ## CombineParticles for D*+ -> D0 pi+

        combName = "%(LinePrefix)sWithD02WSKPiComb" % self.getProps()

        dstar = rsDstar.clone(combName)
        dstar.InputLocations = [ 'StdNoPIDsPions', d0Seq.outputLocation() ]

        seqName = "%(LinePrefix)sWithD02WSKPiSeq" % self.getProps()

        ## Since StdNoPIDsPions is DoD, no explicit dependence in the sequence
        seq =  bindMembers(seqName, algos = [d0Seq, dstar])

        return seq
    # }


    def seqWithD02KK(self) : # {
        """
        Returns a sequence (or rather, a bindMembers object, which is a
        Stripping/Hlt extension for sequences) for the
        D*+ -> D0(K-K+) pi+ reconstruction.
        """
        from StrippingConf.StrippingLine import bindMembers

        ## Make sure that StdNoPIDsPions has been registered with the
        ##  DoD service.
        from CommonParticles.StdNoPIDsPions import StdNoPIDsPions

        d0Seq = self.seqD02KK()   ## Seq for D0 -> pi- K+ reconstruction
        rsDstar = self.combRSDstar()   ## CombineParticles for D*+ -> D0 pi+

        combName = "%(LinePrefix)sWithD02KKComb" % self.getProps()

        dstar = rsDstar.clone(combName)
        dstar.InputLocations = [ 'StdNoPIDsPions', d0Seq.outputLocation() ]

        seqName = "%(LinePrefix)sWithD02KKSeq" % self.getProps()

        ## Since StdNoPIDsPions is DoD, no explicit dependence in the sequence
        seq =  bindMembers(seqName, algos = [d0Seq, dstar])

        return seq
    # }


    def seqWithD02PiPi(self) : # {
        """
        Returns a sequence (or rather, a bindMembers object, which is a
        Stripping/Hlt extension for sequences) for the
        D*+ -> D0(pi-pi+) pi+ reconstruction.
        """
        from StrippingConf.StrippingLine import bindMembers

        ## Make sure that StdNoPIDsPions has been registered with the
        ##  DoD service.
        from CommonParticles.StdNoPIDsPions import StdNoPIDsPions

        d0Seq = self.seqD02PiPi()   ## Seq for D0 -> pi- K+ reconstruction
        rsDstar = self.combRSDstar()   ## CombineParticles for D*+ -> D0 pi+

        combName = "%(LinePrefix)sWithD02PiPiComb" % self.getProps()

        dstar = rsDstar.clone(combName)
        dstar.InputLocations = [ 'StdNoPIDsPions', d0Seq.outputLocation() ]

        seqName = "%(LinePrefix)sWithD02PiPiSeq" % self.getProps()

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

    ########################################### D0 sequences
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


    def seqD02WSKPi(self) : # {
        """
        Returns a sequence (or rather, a bindMembers object, which is a
        Stripping/Hlt extension for sequences) for the D0 -> pi- K+
        reconstruction.
        """
        from StrippingConf.StrippingLine import bindMembers

        from CommonParticles.StdNoPIDsPions import StdNoPIDsPions
        from CommonParticles.StdNoPIDsKaons import StdNoPIDsKaons

        rsD0Recon = self.filtRSD0()   ## FilterDesktop for D0 -> K- pi+

        filtName = "%(LinePrefix)sD02WSKPiFilt" % self.getProps()

        d0Recon = rsD0Recon.clone(filtName)
        d0Recon.DecayDescriptor = "[D0 -> pi- K+]cc"
        d0Recon.InputLocations = [ 'StdNoPIDsPions', 'StdNoPIDsKaons' ]

        seqName = "%(LinePrefix)sD02WSKPiSeq" % self.getProps()

        ## Since the base list StdLooseD02KPiDCS is DoD, no explicit dependence
        ##   in the sequence
        seq =  bindMembers(seqName, algos = [d0Recon])

        return seq
    # }


    def seqD02KK(self) : # {
        """
        Returns a sequence (or rather, a bindMembers object, which is a
        Stripping/Hlt extension for sequences) for the D0 -> K- K+
        reconstruction.
        """
        from StrippingConf.StrippingLine import bindMembers

        from CommonParticles.StdNoPIDsKaons import StdNoPIDsKaons

        rsD0Recon = self.filtRSD0()   ## FilterDesktop for D0 -> K- pi+

        filtName = "%(LinePrefix)sD02KKFilt" % self.getProps()

        d0Recon = rsD0Recon.clone(filtName)
        d0Recon.DecayDescriptor = "[D0 -> K- K+]cc"
        d0Recon.InputLocations = [ 'StdNoPIDsKaons' ]

        seqName = "%(LinePrefix)sD02KKSeq" % self.getProps()

        ## Since the base list StdLooseD02KK is DoD, no explicit dependence
        ##   in the sequence
        seq =  bindMembers(seqName, algos = [d0Recon])

        return seq
    # }


    def seqD02PiPi(self) : # {
        """
        Returns a sequence (or rather, a bindMembers object, which is a
        Stripping/Hlt extension for sequences) for the D0 -> pi- pi+
        reconstruction.
        """
        from StrippingConf.StrippingLine import bindMembers

        from CommonParticles.StdNoPIDsPions import StdNoPIDsPions

        rsD0Recon = self.filtRSD0()   ## FilterDesktop for D0 -> K- pi+

        filtName = "%(LinePrefix)sD02PiPiFilt" % self.getProps()

        d0Recon = rsD0Recon.clone(filtName)
        d0Recon.DecayDescriptor = "[D0 -> pi- pi+]cc"
        d0Recon.InputLocations = [ 'StdNoPIDsPions' ]

        seqName = "%(LinePrefix)sD02PiPiSeq" % self.getProps()

        ## Since the base list StdLooseD02PiPi is DoD, no explicit dependence
        ##   in the sequence
        seq =  bindMembers(seqName, algos = [d0Recon])

        return seq
    # }


    ###############################################################################
    ### This is the main change
    ###############################################################################
    def filtRSD0(self) : # {
        """
        D0 -> K- pi+ reconstruction.
        """
        from Configurables import CombineParticles
        from CommonParticles.StdNoPIDsPions import StdNoPIDsPions
        from CommonParticles.StdNoPIDsKaons import StdNoPIDsKaons

        PiCuts = "(PT>%(DaugPt)s*GeV) & (P>%(DaugP)s*GeV) & (TRCHI2DOF<%(DaugTrkChi2)s) & ( PIDpi - PIDK > 5)" % self.getProps()
        KCuts  = "(PT>%(DaugPt)s*GeV) & (P>%(DaugP)s*GeV) & (TRCHI2DOF<%(DaugTrkChi2)s) & ( PIDK - PIDpi > 8)" % self.getProps()
        combiCut =  "(ADAMASS('D0')<75*MeV)" % self.getProps() ## Hard-code the mass windows for now.
        motherCut =  "(PT>%(D0Pt)s*GeV) & (VFASPF(VCHI2PDOF)<%(D0VtxChi2Ndof)s) & (BPVVD>%(D0PVDisp)s)" % self.getProps()

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

