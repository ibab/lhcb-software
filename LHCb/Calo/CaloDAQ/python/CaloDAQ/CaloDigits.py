#!/usr/bin/env python
# =============================================================================
# @file CaloDAQ/CaloDigits.py
#  The configuration for CaloDigits  
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-10-26
# =============================================================================
"""
The configuration for CaloDigits  
"""
# =============================================================================
__author__  = 'Vanya BELYAEV Ivan.Belyaev@nikhef.nl'
__version__ = 'CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.2 $'
# =============================================================================
from Gaudi.Configuration import *

from CaloKernel.ConfUtils import getAlgo


import logging
_log = logging.getLogger('CaloDAQ')

# =============================================================================
## the digits 
def caloDigits ( context        ,
                 enableOnDemand ,
                 createADCs=False,
                 detectors=['Ecal','Hcal','Prs','Spd'],
                 ReadoutStatusConvert=False) :

    """
    Decoding of Calo-Digits 
    """
    _cntx = 'Offline'
    if context != _cntx :
        _log.warning('CaloDigit: Context is redefined to be Offline')
        
    from Configurables import ( CaloZSupAlg       ,
                                CaloDigitsFromRaw ,
                                RawBankReadoutStatusConverter,
                                GaudiSequencer    ) 
    
    
    

    conflist=[]
    alglist=[]
    if 'Ecal' in detectors :
        _log.debug('caloDigits : Ecal is added to the detector list')
        ecalSeq = GaudiSequencer ('EcalDigitsSeq',Context = _cntx)
        ecal = getAlgo ( CaloZSupAlg             , 
                         "EcalZSup"              ,
                         _cntx                   ,
                         "Raw/Ecal/Digits"       ,
                         enableOnDemand          )
        conflist.append(ecal)
        if ReadoutStatusConvert :
            ecalCnv = getAlgo ( RawBankReadoutStatusConverter, "EcalProcStatus",_cntx)
            ecalCnv.System='Ecal'
            ecalCnv.BankTypes=['EcalPacked']
            ecalSeq.Members = [ecal,ecalCnv]
            alglist.append(ecalSeq)
        else :
            alglist.append(ecal)

    if 'Hcal' in detectors :
        _log.debug('caloDigits : Hcal is added to the detector list')
        hcalSeq = GaudiSequencer ('HcalDigitsSeq',Context = _cntx)
        hcal = getAlgo ( CaloZSupAlg             , 
                         "HcalZSup"              ,
                         _cntx                   ,
                         "Raw/Hcal/Digits"       ,
                         enableOnDemand          )
        conflist.append(hcal)
        if ReadoutStatusConvert :
            hcalCnv = getAlgo ( RawBankReadoutStatusConverter, "HcalProcStatus",_cntx)
            hcalCnv.System='Hcal'
            hcalCnv.BankTypes=['HcalPacked']
            hcalSeq.Members = [hcal,hcalCnv]
            alglist.append(hcalSeq)
        else :
            alglist.append(hcal)

    if 'Prs'  in detectors :
        _log.debug('caloDigits : Prs is added to the detector list')
        prsSeq = GaudiSequencer ('PrsDigitsSeq',Context = _cntx)
        prs  = getAlgo ( CaloDigitsFromRaw       , 
                         "PrsFromRaw"            ,
                         _cntx                   ,
                         "Raw/Prs/Digits"        ,
                         enableOnDemand          )
        conflist.append(prs)
        if ReadoutStatusConvert :
            prsCnv = getAlgo ( RawBankReadoutStatusConverter, "PrsProcStatus",_cntx)
            prsCnv.System='Prs'
            prsCnv.BankTypes=['PrsPacked']
            prsSeq.Members = [prs,prsCnv]
            alglist.append(prsSeq)
        else :
            alglist.append(prs)


    if 'Spd'  in detectors :
        _log.debug('caloDigits : Spd is added to the detector list')
        spd  = getAlgo ( CaloDigitsFromRaw       , 
                         "SpdFromRaw"            ,
                         _cntx                   ,
                         "Raw/Spd/Digits"        ,
                         enableOnDemand          )
        conflist.append(spd)
        alglist.append(spd)


    if createADCs :
        for a in conflist :
            t = a.OutputType.upper()
            if   t in [ 'DIGIT', 'DIGITS' , 'CALODIGIT' , 'CALODIGITS' ] :
                t.OutputType = 'Both'
                _log.warning( 'Change OutputType for %s from %s to %'
                              % ( t.getFullName() , t , t.OutputType ) )
            elif t in [ 'ADC'  , 'ADCS'   , 'CALOADC'   , 'CALOADCS'   ] :
                pass
            elif t is 'BOTH' :
                pass
            else :
                t.OutputType = 'Adc'
                _log.warning( 'Change OutputType for %s from %s to %'
                              % ( t.getFullName() , t , t.OutputType ) )

               
            
    ## combine them into sequence  
    alg = GaudiSequencer ('CaloDigits',Context = context, IgnoreFilterPassed = True, Members=alglist)
    return alg 
    
    
# =============================================================================
if '__main__' == __name__ :
    print __doc__
    print __author__
    print __version__
    

# =============================================================================
# The END 
# =============================================================================
