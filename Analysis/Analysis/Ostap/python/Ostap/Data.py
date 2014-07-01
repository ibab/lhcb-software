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
""" Useful utilities to get access to datafiles & chains

Actualy it is a little bit modified version (with globbing) of
the original ``Ostap'' code by Alexander BARANOV

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
import ROOT, glob 
# =============================================================================
# logging 
# =============================================================================
from   AnalysisPython.Logger import getLogger 
if '__main__' ==  __name__ : logger = getLogger ( 'Ostap.Data' )
else                       : logger = getLogger ( __name__     )
# =============================================================================
if not hasattr ( ROOT.TTree , '__len__' ) :  
    ROOT.TTree. __len__ = lambda s : s.GetEntries()
# =============================================================================
## @class Files
#  Simple utility to pickup the list of files 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @author Alexander BARANOV a.baranov@cern.ch
#  @date   2014-06-08  
class Files(object):
    """
    Simple utility to pickup the list of files 
    
    >>> data  = Files( '*.root' )
    >>> files = data.files 
    """    
    def __init__( self , files ) :  
        #
        self.files    = []
        self.patterns = files  
        # 
        if isinstance ( files , str ) : files = [ files ]
        # 
        for pattern in files :
            
            for f in glob.iglob ( pattern ) :
                
                self.treatFile  ( f )
                
    ## the specific action for each file 
    def treatFile ( self, the_file ) :
        self.files.append ( the_file )
        
    ## printout 
    def __str__(self):
        """
        The specific printout
        """
        return "<#files: {}>".format( len ( self.files ) )
    
    def __repr__( self )  : return self.__str__()
    
# =============================================================================
## @class Data
#  Simple utility to access to certain chain in the set of ROOT-files
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @author Alexander BARANOV a.baranov@cern.ch
#  @date   2014-06-08  
class Data(Files):
    """
    Simple utility to access to certain chain in the set of ROOT-files
    
    >>> data  = Data('Bc/MyTree', '*.root' )
    >>> chain = data.chain
    >>> flist = data.files 
    """
    
    def __init__( self           ,
                  chain          ,
                  files  = []    ) :

        
        self.chain = ROOT.TChain ( chain )
        Files.__init__( self , files )
            
    ## the specific action for each file 
    def treatFile ( self, the_file ) :
        """
        Add the file to TChain
        """
        Files.treatFile ( self , the_file ) 
        self.chain.Add         ( the_file ) 
        
    ## printout 
    def __str__(self):
        return  "<#files: {}; Entries: {}>".format( len( self.files ) ,
                                                    len( self.chain ) )
    
# =============================================================================
## @class Data2
#  Simple utility to access two chains in the set of ROOT-files
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @author Alexander BARANOV a.baranov@cern.ch
#  @date   2014-06-08  
class Data2(Data):
    """
    Simple utility to access to certain chain in the set of ROOT-files
    
    >>> data  = Data('Bc/MyTree', '*.root' )
    >>> chain = data.chain
    >>> flist = data.files 
    """
    
    def __init__( self           ,
                  chain1         ,
                  chain2         , 
                  files  = []    ) :
        
        self.chain2 = ROOT.TChain ( chain2 )
        Data.__init__( self , chain1 , files )
        self.chain1 = self.chain 
            
    ## the specific action for each file 
    def treatFile ( self, the_file ) :
        """
        Add the file to TChain
        """
        Data.treatFile ( self , the_file ) 
        self.chain2.Add       ( the_file ) 
        
    ## printout 
    def __str__(self):
        return  "<#files: {}; Entries1: {}; Entries2: {}>".format ( len ( self.files  ) ,
                                                                    len ( self.chain1 ) ,
                                                                    len ( self.chain2 ) )
    
# =============================================================================
## @class DataAndLumi
#  Simple utility to access to certain chain in the set of ROOT-files
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @author Alexander BARANOV a.baranov@cern.ch
#  @date   2014-06-08  
class DataAndLumi(Data2):
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
                  lumi_chain  = 'GetIntegratedLuminosity/LumiTuple' ) :

        Data2.__init__ ( chain , lumi_chain , files ) 
        self.lumi = self.chain2 
        
    ## get the luminosity 
    def getLumi ( self ):
        """
        Get the luminosity from the 
        """
        from   Ostap.GetLumi import getLumi
        return getLumi( self.lumi)

    ## printout 
    def __str__(self):
        
        return "<Luminosity: {}pb-1; #files: {}; Entries: {}>".format(
            self.getLumi()     ,
            len ( self.files ) ,
            len ( self.chain ) ) 
    
# =============================================================================
if '__main__' == __name__ :
    
    import ostapline
    logger.info ( __file__ + '\n' + ostapline.line  )
    logger.info ( 80*'*' )
    logger.info ( __doc__  )
    logger.info ( 80*'*' )
    logger.info ( ' Author  : %s' %         __author__    ) 
    logger.info ( ' Version : %s' %         __version__   ) 
    logger.info ( ' Date    : %s' %         __date__      )
    logger.info ( ' Symbols : %s' %  list ( __all__     ) )
    logger.info ( 80*'*' ) 
    
# =============================================================================
# The END 
# =============================================================================
