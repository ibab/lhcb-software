#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# $Id:$
# =============================================================================
## @file Data.py 
#
#  Useful utilities to get access to datafiles & chains
#  Actualy it is just a little bit modified version (with globbing) of
#  original ``Ostap'' code by Alexander BARANOV
#
#  @code
#
#  >>> data  = Data('Bc/MyTree', '*.root' )
#  >>> chain = data.chain
#  >>> flist = data.files 
#
#  >>> data  = DataAndLumi('Bc/MyTree', '*.root' )
#  >>> chain = data.chain
#  >>> flist = data.files 
#  >>> lumi  = data.lumi
#  >>> print data.getLumi()
#
#  @endcode
# 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @author Alexander BARANOV a.baranov@cern.ch
#  @date   2014-06-08
#
#                    $Revision: 173870 $
#  Last modification $Date: 2014-06-14 13:44:41 +0200 (Sat, 14 Jun 2014) $
#  by                $Author: ibelyaev $
# =============================================================================
"""
Useful utilities to get access to datafiles & chains
Actualy it is a little bit modified version (with globbing) of
original ``Ostap'' code by Alexander BARANOV

>>> data  = Data('Bc/MyTree', '*.root' )
>>> chain = data.chain
>>> flist = data.files 

>>> data  = Data('Bc/MyTree', 'a.root' )
>>> chain = data.chain
>>> flist = data.files 

>>> data  = Data('Bc/MyTree', [ 'a.root' , 'b.root' ] )
>>> chain = data.chain
>>> flist = data.files 

>>> data  = DataAndLumi('Bc/MyTree', '*.root' )
>>> chain = data.chain
>>> flist = data.files 
>>> lumi  = data.lumi
>>> print data.getLumi() 

>>> data  = DataAndLumi('Bc/MyTree', [ 'A/*.root' , 'B/B/D/*.root' ] )
>>> chain = data.chain
>>> flist = data.files 
>>> lumi  = data.lumi
>>> print data.getLumi() 

"""
# =============================================================================
__version__ = "$Revision: $"
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__    = "2014-06-08"
__all__     = ( 'Data', 'DataAndLumi' )
# =============================================================================
import ROOT
# =============================================================================
# logging 
# =============================================================================
from   AnalysisPython.Logger import getLogger 
logger = getLogger( __name__ )
# =============================================================================
## @class Data
#  Simple utility to access to certain chain in the set of ROOT-files
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @author Alexander BARANOV a.baranov@cern.ch
#  @date   2014-06-08  
class Data(object):
    """
    
    Simple utility to access to certain chain in the set of ROOT-files
    
    >>> data  = Data('Bc/MyTree', '*.root' )
    >>> chain = data.chain
    >>> flist = data.files 
    """
    
    def __init__( self           ,
                  chain          ,
                  files  = []    ,
                  useglob = True ):
        
        self.chain = ROOT.TChain ( chain )
        # 
        self.files = [] 
        if files :
            self.add_files ( files , useglob )
            
    def add_files ( self, patterns , useglob ):
        
        pats = patterns
        if isinstance ( pats , str ) : pats = [ pats ]
        
        if useglob :
            
            import glob
            for p in pats :
                for f in glob.iglob ( p ) :
                    self.files .append ( f ) 
                    self.chain .Add    ( f )
        else :
            
            for f in files :
                self.files.append ( f ) 
                self.chain.Add    ( f )

                
    def __str__(self):
        
        ret = "<#files: {}; Entries: {}>".format(len(self.files), self.chain.GetEntries() )
        
        return ret

# =============================================================================
## @class DataAndLumi
#  Simple utility to access to certain chain in the set of ROOT-files
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @author Alexander BARANOV a.baranov@cern.ch
#  @date   2014-06-08  
class DataAndLumi(Data):
    """
    Simple utility to access to certain chain in the set of ROOT-files
    
    >>> data  = DataAndLumi('Bc/MyTree', '*.root' )
    >>> chain = data.chain
    >>> flist = data.files 
    >>> lumi  = data.lumi
    >>> print data.getLumi() 
    """

    def __init__( self               ,
                  chain              ,
                  files       = []   ,
                  useglob     = True ,
                  lumi_chain  = 'GetIntegratedLuminosity/LumiTuple' ) :

        self.lumi = ROOT.TChain( lumi_chain )  
        Data.__init__ ( self , chain , files , useglob ) 
        
    #
    def add_files ( self, patterns , useglob ):
        
        pats = patterns
        if isinstance ( pats , str ) : pats = [ pats ]
        
        if useglob :
            
            import glob
            for p in pats :
                for f in glob.iglob ( p ) :
                    self.files.append ( f ) 
                    self.chain.Add    ( f )
                    self.lumi .Add    ( f )
        else :
            
            for f in files :
                
                self.files.append ( f ) 
                self.chain.Add    ( f )
                self.lumi .Add    ( f )

    ## get the luminosity 
    def getLumi ( self ):
        """
        Get the luminosity from the 
        """
        from   PyPAW.GetLumi import getLumi
        return getLumi(self.lumi)

    def __str__(self):
        ret = "<"
        ret += "Luminosity: {}pb-1; #files: {}; ".format(self.getLumi(), len(self.files))
        ret += "Entries: {}>".format( self.chain.GetEntries() )

        return ret
           
# =============================================================================
if '__main__' == __name__ :
    
    print 80*'*'
    print __doc__
    print ' Author  : ' , __author__
    print ' Version : ' , __version__
    print ' Date    : ' , __date__    
    print ' Symbols : ' , __all__    
    print 80*'*'

# =============================================================================
# The END 
# =============================================================================
