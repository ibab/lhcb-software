# $Id: StrippingDiElectron.py,v 1.3 2010-05-27 17:49:21 gcowan Exp $
## #####################################################################
# Stripping selections for inclusive di-electron, J/psi(1S) -> e+ e- 
#
# @authors P. Koppenburg, Jibo He
# @date 2010-05-20
# 
## #####################################################################

from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
from Configurables import FilterDesktop, CombineParticles, OfflineVertexFitter	
from PhysSelPython.Wrappers import Selection, SelectionSequence, DataOnDemand

class StrippingDiElectronConf(LHCbConfigurableUser):
    """
    Definition of Jpsi -> e e stripping.
    """
    __slots__ = {
        "LooseJpsi2ee_ElectronPT"               :  500.  # MeV
        ,"LooseJpsi2ee_VertexCHI2"              :   25.
        ,"LooseJpsi2ee_MinMass"                 : 2000.  # MeV
        ,"LooseJpsi2ee_MaxMass"                 : 3800.  # MeV
    
        ,"Jpsi2ee_ElectronPT"                   :  750.  # MeV
        ,"Jpsi2ee_VertexCHI2"                   :   25.
        ,"Jpsi2ee_MinMass"                      : 2000.  # MeV
        ,"Jpsi2ee_MaxMass"                      : 3800.  # MeV
        
        ,"IncDiElectron_ElectronPT"             :  750.  # MeV
        ,"IncDiElectron_MinMass"                : 2000.  # MeV
        ,"IncDiElectron_VertexCHI2"             :   25.
        
        ,"IncDiElectronLowMass_ElectronPT"      :  750.  # MeV
        ,"IncDiElectronLowMass_VertexCHI2"      :   25.
        ,"IncDiElectronLowMass_MinMass"         :   30.  # MeV        
        
        ,"BiasedIncDiElectron_ElectronPT"       :  300.  # MeV
        ,"BiasedIncDiElectron_ElectronMIPCHI2"  :    1.
        ,"BiasedIncDiElectron_VertexCHI2"       :   25.
        ,"BiasedIncDiElectron_MinMass"          : 2000.  # MeV
      
        ,"BiasedIncDiElectronLowMass_ElectronPT"       :  300.  # MeV
        ,"BiasedIncDiElectronLowMass_ElectronMIPCHI2"  :    1.
        ,"BiasedIncDiElectronLowMass_VertexCHI2"       :   25. 
        ,"BiasedIncDiElectronLowMass_MinMass"          :   30.  # MeV
        
        }

    #------------------------------------------------------
    # Loose Jpsi->ee using StdLooseJpsi2ee
    # with PT(e)> 500 MeV 
    #------------------------------------------------------
    def LooseJpsi2ee_line( self ):
        from StrippingConf.StrippingLine import StrippingLine
        
	_LooseJpsi2ee =  DataOnDemand(Location = 'Phys/StdLooseJpsi2ee')
        _RefineLooseJpsi2ee = FilterDesktop("RefineJpsi2ee",
                                            Code = "(MINTREE('e-'==ABSID,PT)> %(LooseJpsi2ee_ElectronPT)s *MeV)" \
                                            " & (VFASPF(VCHI2/VDOF) < %(LooseJpsi2ee_VertexCHI2)s)" \
                                            " & (MM > %(LooseJpsi2ee_MinMass)s *MeV)" \
                                            " & (MM < %(LooseJpsi2ee_MaxMass)s *MeV)" % self.getProps()
                                            )
        
	SelLooseJpsi2ee = Selection("SelLooseJpsi2ee",
                                    Algorithm = _RefineLooseJpsi2ee,
                                    RequiredSelections = [ _LooseJpsi2ee ]                           
                                    )
	SeqLooseJpsi2ee = SelectionSequence("SeqLooseJpsi2ee", TopSelection = SelLooseJpsi2ee )
	return StrippingLine('LooseJpsi2ee', prescale = 1, algos = [ SeqLooseJpsi2ee ])



    #------------------------------------------------------
    # Jpsi->ee using StdLooseJpsi2ee
    # with PT(e)> 750 MeV 
    #------------------------------------------------------
    def Jpsi2ee_line( self ):
        from StrippingConf.StrippingLine import StrippingLine
	_Jpsi2ee =  DataOnDemand(Location = 'Phys/StdLooseJpsi2ee')
        _RefineJpsi2ee = FilterDesktop("RefineJpsi2ee",
                                       Code = "(MINTREE('e-'==ABSID,PT)> %(Jpsi2ee_ElectronPT)s *MeV)"\
                                       " & (VFASPF(VCHI2/VDOF) < %(Jpsi2ee_VertexCHI2)s)" \
                                       " & (MM > %(Jpsi2ee_MinMass)s *MeV)" \
                                       " & (MM < %(Jpsi2ee_MaxMass)s *MeV)" % self.getProps()
                                       )
        
	SelJpsi2ee = Selection("SelJpsi2ee",
                               Algorithm = _RefineJpsi2ee,
                               RequiredSelections = [ _Jpsi2ee ]                           
                               )
	SeqJpsi2ee = SelectionSequence("SeqJpsi2ee", TopSelection = SelJpsi2ee )
	return StrippingLine('Jpsi2ee', prescale = 1, algos = [ SeqJpsi2ee ])


    
    #------------------------------------------------------
    # Inclusive DiEelctron using StdLooseDiElectron
    # with PT(e)> 750 MeV 
    #------------------------------------------------------    
    def IncDiElectron_line( self ):
        from StrippingConf.StrippingLine import StrippingLine
        
	_IncDiElectron =  DataOnDemand(Location = 'Phys/StdLooseDiElectron')
        _RefineIncDiElectron = FilterDesktop("RefineIncDiElectron",
                                             Code = "(MINTREE('e-'==ABSID,PT)> %(IncDiElectron_ElectronPT)s *MeV)"\
                                             " & (VFASPF(VCHI2/VDOF) < %(IncDiElectron_VertexCHI2)s)" \
                                             " & (MM>%(IncDiElectron_MinMass)s *MeV)" % self.getProps()
                                             ) 
        
	SelIncDiElectron = Selection("SelIncDiElectron",
                                     Algorithm = _RefineIncDiElectron,
                                     RequiredSelections = [ _IncDiElectron ]                           
                                     )
	SeqIncDiElectron = SelectionSequence("SeqIncDiElectron", TopSelection = SelIncDiElectron )
	return StrippingLine('IncDiElectron', prescale = 1, algos = [ SeqIncDiElectron ])

    
    #------------------------------------------------------
    # Inclusive DiEelctron using StdLooseDiElectron
    # with PT(e)> 750 MeV, Mass down to 30 MeV
    #------------------------------------------------------    
    def IncDiElectronLowMass_line( self ):
        from StrippingConf.StrippingLine import StrippingLine
        
	_IncDiElectronLowMass =  DataOnDemand(Location = 'Phys/StdLooseDiElectron')
        _RefineIncDiElectronLowMass = FilterDesktop("RefineIncDiElectronLowMass",
                                                    Code = "(MINTREE('e-'==ABSID,PT)> %(IncDiElectronLowMass_ElectronPT)s *MeV)"\
                                                    " & (VFASPF(VCHI2/VDOF) < %(IncDiElectronLowMass_VertexCHI2)s)" \
                                                    " & (MM>%(IncDiElectronLowMass_MinMass)s *MeV)" % self.getProps()
                                                    ) 
        
	SelIncDiElectronLowMass = Selection("SelIncDiElectronLowMass",
                                            Algorithm = _RefineIncDiElectronLowMass,
                                            RequiredSelections = [ _IncDiElectronLowMass ]                           
                                            )
	SeqIncDiElectronLowMass = SelectionSequence("SeqIncDiElectronLowMass", TopSelection = SelIncDiElectronLowMass )
	return StrippingLine('IncDiElectronLowMass', prescale = 1, algos = [ SeqIncDiElectronLowMass ])
    

    
    #------------------------------------------------------
    # Biased Inclusive DiEelctron using StdLooseDiElectron
    # with PT(e)> 300 MeV, MIN IPCHI2 w.r.t. PVs > 1. 
    #------------------------------------------------------    
    def BiasedIncDiElectron_line( self ):
        from StrippingConf.StrippingLine import StrippingLine
        
	_BiasedIncDiElectron =  DataOnDemand(Location = 'Phys/StdLooseDiElectron')
        _RefineBiasedIncDiElectron = FilterDesktop(
            "RefineBiasedIncDiElectron",
            Code = "(MINTREE('e-'==ABSID,PT)> %(BiasedIncDiElectron_ElectronPT)s *MeV)"\
            " & (MINTREE('e-'==ABSID,MIPCHI2DV(PRIMARY))> %(BiasedIncDiElectron_ElectronMIPCHI2)s )"\
            " & (VFASPF(VCHI2/VDOF) < %(BiasedIncDiElectron_VertexCHI2)s)" \
            " & (MM>%(BiasedIncDiElectron_MinMass)s *MeV)" % self.getProps()
            ) 
        
	SelBiasedIncDiElectron = Selection("SelBiasedIncDiElectron",
                                           Algorithm = _RefineBiasedIncDiElectron,
                                           RequiredSelections = [ _BiasedIncDiElectron ]                           
                                           )
	SeqBiasedIncDiElectron = SelectionSequence("SeqBiasedIncDiElectron", TopSelection = SelBiasedIncDiElectron )
	return StrippingLine('BiasedIncDiElectron', prescale = 1, algos = [ SeqBiasedIncDiElectron ])    

    
    #------------------------------------------------------
    # Biased Inclusive DiEelctron using StdLooseDiElectron
    # with PT(e)> 300 MeV, MIN IPCHI2 w.r.t. PVs > 1.
    # Mass down to 30 MeV
    #------------------------------------------------------    
    def BiasedIncDiElectronLowMass_line( self ):
        from StrippingConf.StrippingLine import StrippingLine
        
	_BiasedIncDiElectronLowMass =  DataOnDemand(Location = 'Phys/StdLooseDiElectron')
        _RefineBiasedIncDiElectronLowMass = FilterDesktop(
            "RefineBiasedIncDiElectronLowMass",
            Code = "(MINTREE('e-'==ABSID,PT)> %(BiasedIncDiElectronLowMass_ElectronPT)s *MeV)"\
            " & (MINTREE('e-'==ABSID,MIPCHI2DV(PRIMARY))> %(BiasedIncDiElectronLowMass_ElectronMIPCHI2)s )"\
            " & (VFASPF(VCHI2/VDOF) < %(BiasedIncDiElectronLowMass_VertexCHI2)s)" \
            " & (MM>%(BiasedIncDiElectronLowMass_MinMass)s *MeV)" % self.getProps()
            ) 
        
	SelBiasedIncDiElectronLowMass = Selection("SelBiasedIncDiElectronLowMass",
                                                  Algorithm = _RefineBiasedIncDiElectronLowMass,
                                                  RequiredSelections = [ _BiasedIncDiElectronLowMass ]                           
                                                  )
	SeqBiasedIncDiElectronLowMass = SelectionSequence("SeqBiasedIncDiElectronLowMass", TopSelection = SelBiasedIncDiElectronLowMass )
	return StrippingLine('BiasedIncDiElectronLowMass', prescale = 1, algos = [ SeqBiasedIncDiElectronLowMass ])
    

        
    def getProps(self) :
        """
        From HltLinesConfigurableUser
        @todo Should be shared between Hlt and stripping
        """
        d = dict()
        for (k,v) in self.getDefaultProperties().iteritems() :
            d[k] = getattr(self,k) if hasattr(self,k) else v
        return d

