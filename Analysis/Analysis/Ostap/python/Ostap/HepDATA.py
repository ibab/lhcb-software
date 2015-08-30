#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# $Id:$
# =============================================================================
## @file
#  Module with helper functions to convert objects into HepDATA format
#
#  Supported types: 
#  - TH1D, TH1D
#  - TGraphErrors
#  - TGraphAsymmErrors
#
#  @see http://hepdata.cedar.ac.uk/resource/sample.input
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
#  
#                    $Revision: 175248 $
#  Last modification $Date: 2014-07-19 15:56:55 +0200 (Sat, 19 Jul 2014) $
#  by                $Author: ibelyaev $
# =============================================================================
""" Helper functions to convert the objects into HepDATA format
Supported types: 
- TH1D, TH1D
- TGraphErrors
- TGraphAsymmErrors
"""
# =============================================================================
__version__ = "$Revision: 175248 $"
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__    = "2011-06-07"
__all__     = () 
# =============================================================================
import ROOT
# =============================================================================
# logging 
# =============================================================================
from AnalysisPython.Logger import getLogger 
if '__main__' ==  __name__ : logger = getLogger( 'Ostap.HepDATA' )
else                       : logger = getLogger( __name__ )
# =============================================================================
logger.debug ( 'HepDATA format and conversion routines')
# =============================================================================
## fields required for each dataset
dataset_fields = ( 'location'  ,
                   'dscomment' ,
                   'obskey'    ,
                   'reackey'   ,
                   'qual'      ,
                   'xheader'   ,
                   'yheader'   )
## fields required for each hepdata file 
hepfile_fields = ( 'author'    ,
                   'status'    ,
                   'reference' , ## format :  "REFERENCE : YEAR"
                   'doi'       , ## optional  
                   'detector'  ,
                   'spiresId'  , ## Id in SPIRES/INSPIRE
                   'cdsId'     , ## Id in CDS
                   'title'     ,
                   'comment'   )

from collections import defaultdict
METAINFO = defaultdict(list)

# =============================================================================
## Helper base class for HepDATA formatting 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
class HepDataBase(object) :
    """
    Helper base class for HepDATA formatting 
    """
    def __init__ ( self , metainfo = defaultdict(list) , **kwargs ) :
        ##
        self.meta = metainfo
        for k , v in kwargs.iteritems() :
            if isinstance ( v , (list,tuple) ) :
                for i in v : self.meta[k].append ( i )
            else : self.meta[k].append ( v )
                
    ## add meta-information
    def add ( self , key , info) : self.meta[key].append ( info ) 
        
    ## get missing keys 
    def missing ( self , keys ) :
        m = set()
        for key in keys : 
            if not self.meta.has_key ( key ) : m.add ( key )
        return m 
    
    def format ( self , header ) :
        "Make a proper valid representation of dataset"
        #
        lines = []
        if header : lines = [ '*%s:' % header ] 
        #
        for k,w in self.meta.iteritems():
            for i in w :
                lines.append ( '*%s: %s' % ( k , i ) )
                
        ## the actual format of the data 
        self.formatData( lines ) 
            
        return '\n'.join ( lines )

# ============================================================================-
## @class HepDataFile
#  Helper class to prepare the file with HepDATA information
#  @code 
#  metainfo = {...}
#  hf = HepDataFile(  *metainfo )
#  print hf
#  @endcode
#  Or use it as context manager 
#  @code
#  with HepDataFile(  *metainfo ) as hf :
#  ...            <do something>
#  @encode 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2015-08-29
class HepDataFile(HepDataBase) :
    """
    Helper class to prepare the file with HepDATA information
    >>> metainfo = {...}
    >>> hf = HepDataFile(  *metainfo )
    >>> print hf
    Or as context manager 
    >>> with HepDataFile(  *metainfo ) as hf :
    ...       <do something> 
    """
    def __init__ ( self ,
                   filename = 'HepDATA.txt'     ,
                   datasets = []                ,
                   metainfo = defaultdict(list) , **kwargs ) :
        
        HepDataBase.__init__ ( self , metainfo , **kwargs ) 
        self.filename = filename 
        self.datasets = datasets
        m = self.missing ( hepfile_fields ) 
        if m : logger.warning('HepFILE missing keys: %s' % m )
        
    def append      ( self , dataset ) : self.datasets.append ( dataset )
    def add         ( self , dataset ) : self.append ( dataset ) 
    def __iadd__    ( self , dataset ) :
        self.append ( dataset )
        return self 
    def __lshift__  ( self , dataset ) :
        self += dataset
        return self
    def formatData ( self , lines ) : pass 
    def __enter__ ( self ) :
        
        self.file = open ( self.filename , 'w' )
        lines = [] 
        for k,w in self.meta.iteritems () :
            for i in w :
                lines.append ( '*%s: %s' % ( k , i ) )
        self.file.write ( '\n'.join ( lines ) + '\n' )
        return self
        
    def __exit__  ( self , *_ ) :
        for ds in self.datasets :
            self.file.write( '\n%s\n' % ds )
        self.file.write ('\n*e\n' )
        self.file.close ()

    def __str__  ( self ) :
        result = self.format('') + '\n'
        for ds in self.datasets :
            result += str( ds ) + '\n'
        return result + '\n*e'
    
# =============================================================================
## Convert simple object (presumably historgam or graph) to HepDATA format
#  @code
#  histo    = ...
#  metadata = { ... }
#  ds = HepData ( histo , **metadata  )
#  @encode
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
class HepData(HepDataBase) :
    """ Convert simple object (presumably historgam or graph) to HepDATA format
    >>> histo    = ...
    >>> metadata = { ... }
    >>> ds = HepData ( histo , **metadata )
    """
    def __init__ ( self  ,
                   histo ,
                   ##
                   syst1 = '' , ## the first  systematic 
                   syst2 = '' , ## the second systematic 
                   syst3 = '' , ## the third  systematic 
                   ##
                   metainfo = defaultdict(list) , **kwargs ) :
        ##
        HepDataBase.__init__ ( self , metainfo , **kwargs )
        self.histo = histo 
        m = self.missing ( dataset_fields ) 
        if m : logger.warning('HepData missing keys: %s' % m )
        self.syst1 = syst1
        self.syst2 = syst2
        self.syst3 = syst3
        
    ## the actual output :-) 
    def __str__ ( self ) : return self.format ('dataset')+'\n'

    ## the most important line: the proper delegation
    def formatData ( self , the_lines ) :
        """The most important line: the proper delegation"""
        args       = (self.syst1,self.syst2,self.syst3)
        ## delegate to the specific method of the histogram/graph
        result     = self.histo.toHepDATA ( *args )      ## invoke the specific method 
        the_lines += result.split('\n')

# ==============================================
## helper function to decode the systematic uncertainties
#  Systematic could be 
#   - just a string
#   - an object with index:   obj  [ibin]
#   - a kind of function:     func (ibin)
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07  
def get_syst ( syst , *index ) :
    """Helper function to decode the systematic uncertainties
    Systematic could be 
    - just a string
    - an object with index:   obj  [ibin]
    - a kind of function:     func (ibin)
    """
    if   isinstance ( syst , str )   : return syst
    elif syst and hasattr  ( syst , '__getitem__' )  : return str ( syst [  index ] )
    elif syst and callable ( syst )                  : return str ( syst ( *index ) )
    elif syst : raise AttributeError("Invalid ssystematic %s/%s" % ( syst , type( syst ) ) )
    
    return ''
    
# =============================================================================
## Dump TH1 in HepData format with the optional specification of
#  systematic uncertainties
#  @code
#  h1 = ...
#  print h1.toHepDATA()
#  print h1.toHepDATA( syst1 ='0.01:detector' )
#  print h1.toHepDATA( syst1 ='0.01:detector' ,
#                      syst2 = [ 0.02 ,0.03 , .... , 0.8] )
#  @endcode
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07  
def _h1_hepdata_ ( histo      ,
                   syst1 = '' , 
                   syst2 = '' ,
                   syst3 = '' ) :
    """
    Dump -histogram in HepData -compatible format with \
    optional specification of up to three systematic uncertainnties 
    >>> h1 = ...
    >>> print h1.toHepDATA()
    >>> print h1.toHepDATA( syst1 ='0.01:detector' )
    >>> print h1.toHepDATA( syst1 ='0.01:detector' ,
    ...                     syst2 = [ 0.02 ,0.03 , .... , 0.8] )
    >>> print h1.toHepDATA( syst1 ='0.01:detector' ,
    ...                     syst2 = [ 0.02 ,0.03 , .... , 0.8]   ,
    ...                     syst3 = lambda i : '%s:last' % h2[i] ) 
    
    """
    
    ## the basic format 
    fmt   = '   %s TO %s ; %s +- %s ; '
    lines = [ '*data: x : y ' ]
    
    for item in histo.iteritems() :
        
        i = item[0] ## bin-number 
        x = item[1] ## x
        y = item[2] ## y 

        line = fmt %  ( x.value () - x.error () ,
                        x.value () + x.error () ,
                        y.value () , y.error () )
        
        #
        ## up to three systematic uncertainties 
        #

        s1   = get_syst ( syst1 , i ) 
        s2   = get_syst ( syst2 , i ) 
        s3   = get_syst ( syst3 , i ) 

        if s2 and not s1 :
            raise AttributeError ( "HepData: syst2 is specified without syst1" )
        if s3 and not s2 :
            raise AttributeError ( "HepData: syst3 is specified without syst2" )
        
        if s1 :
            line +=        " (DSYS=%s" % s1
            if s2 : line += ",DSYS=%s" % s2
            if s3 : line += ",DSYS=%s" % s3
            line += ") ; "
            
        lines.append (line)
        
    lines.append ('*dataend:')
    return '\n'.join(lines)+'\n'

# =============================================================================
## Dump TGraphAsymmErrore in HepData format with the optional specification of
#  systematic uncertainties
#  @code
#  graph = ...
#  print graph.toHepDATA()
#  print graph.toHepDATA ( syst1 ='0.01:detector' )
#  print graph.toHepDATA ( syst1 ='0.01:detector' ,
#                          syst2 = [ 0.02 ,0.03 , .... , 0.8] )
#  @endcode
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07  
def _tgae_hepdata_ ( graph      ,
                     syst1 = '' , 
                     syst2 = '' ,
                     syst3 = '' ) :
    """
    Dump -histogram in HepData -compatible format with \
    optional specification of up to three systematic uncertainnties 
    >>> h1 = ...
    >>> print h1.toHepDATA()
    >>> print h1.toHepDATA( syst1 ='0.01:detector' )
    >>> print h1.toHepDATA( syst1 ='0.01:detector' ,
    ...                     syst2 = [ 0.02 ,0.03 , .... , 0.8] )
    >>> print h1.toHepDATA( syst1 ='0.01:detector' ,
    ...                     syst2 = [ 0.02 ,0.03 , .... , 0.8]   ,
    ...                     syst3 = lambda i : '%s:last' % h2[i] ) 
    
    """
    
    ## the basic format 
    fmt   = '   %s TO %s ; %s +%s -%s; '
    lines = [ '*data: x : y ' ]

    for item in graph.iteritems() :
        
        i   = item[0] ## bin-number 
        x   = item[1] ## x
        exl = item[2]
        exh = item[3]
        y   = item[4] ## x
        eyl = item[5]
        eyh = item[6]
        
        line = fmt %  ( x  - abs ( exl ) ,
                        x  + abs ( exh ) ,
                        y  , abs ( eyh ) , abs ( eyl ) )
        #
        ## up to three systematic uncertainties 
        #

        s1   = get_syst ( syst1 , i ) 
        s2   = get_syst ( syst2 , i ) 
        s3   = get_syst ( syst3 , i ) 

        if s2 and not s1 :
            raise AttributeError ( "HepData: syst2 is specified without syst1" )
        if s3 and not s2 :
            raise AttributeError ( "HepData: syst3 is specified without syst2" )
        
        if s1 :
            line +=        " (DSYS=%s" % s1
            if s2 : line += ",DSYS=%s" % s2
            if s3 : line += ",DSYS=%s" % s3
            line += ") ; "
            
        lines.append (line)
        
    lines.append ('*dataend:')
    return '\n'.join(lines)+'\n'


for t in ( ROOT.TH1D         ,
           ROOT.TH1F         ,
           ROOT.TGraphErrors ) : 
    
    t . toHepDATA = _h1_hepdata_
    t . toHepData = _h1_hepdata_

ROOT.TGraphAsymmErrors .toHepDATA = _tgae_hepdata_  ## different one 
ROOT.TGraphAsymmErrors .toHepData = _tgae_hepdata_  ## different one 

# =============================================================================


# =============================================================================
if '__main__' == __name__ :
    

    
    import Ostap.Line
    logger.info ( __file__  + '\n' + Ostap.Line.line  ) 
    logger.info ( 80*'*'   )
    logger.info ( __doc__  )
    logger.info ( 80*'*' )
    logger.info ( ' Author  : %s' %         __author__    ) 
    logger.info ( ' Version : %s' %         __version__   ) 
    logger.info ( ' Date    : %s' %         __date__      )
    logger.info ( ' Symbols : %s' %  list ( __all__     ) )
    logger.info ( 80*'*' ) 


    from Ostap.PyRoUts import VE 
    
    h1 = ROOT.TH1D('h1','',5,0,10)
    h1[1] = VE(100 ,10**2)
    h1[2] = VE( 50 ,10**2)
    h1[3] = VE( 10 , 2**2)
    h1[4] = VE(  1 , 1**2)
    h1[5] = VE(  0 , 1**2)
    
    meta = {
        'location'  : 'Figure 11 (a), red filled circles' ,
        'dscomment' : r"""Normalized differential cross-section
        $d\ln\sigma(pp\rightarrowJ/\psi D^0 X)/dp_T(J/\psi)$
        for $2<y(J/\psi)<4$, $p_T(J/\psi)<12$ GeV/$c$,
        $2<y(D^0)<4$, $3<p_T(D^0)<12$ GeV/$c$ region""", 
        'obskey'    : 'DLN(SIG)/DPT',
        'qual'      : [ 'D0 PT IN GEV : 3.0 TO 12.0',
                        'D0 YRAP : 2.0 TO 4.0',
                        'J/PSI PT IN GEV : 2.0 TO 12.0',
                        'J/PSI YRAP : 2.0 TO 4.0', 
                        'RE : P P --> J/PSI D0',
                        'SQRT(S) IN GEV : 7000.0' ] ,
        'yheader'   : 'DLN(SIG)/DPT IN 1/500 MEV',
        'xheader'   : 'PT IN GEV',
        }
    ds = HepData (
        h1 ,
        syst1 = '0.01:detector' ,
        syst2 = lambda i : "%s:ququ!" % i ,
        syst3 = lambda i : "%s:last"  % h1[i].error() , 
        **meta )

    gr = h1.toGraph2(0.1)
    ds2 = HepData (
        gr ,
        syst1 = '0.01:detector' ,
        syst2 = lambda i : "%s:ququ!" % i ,
        syst3 = lambda i : "%s:last"  % h1[i+1].error() , 
        **meta )    
        
    meta = { 'author' : 'AAIJ' , 'detector' : 'LHCb' } 
    with HepDataFile( **meta ) as hf :
        hf << ds 
        hf << ds2 

    logger.info ( 80*'*' ) 

# =============================================================================
# The END 
# =============================================================================
