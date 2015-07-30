#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# $Id$
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
#                    $Revision$
#  Last modification $Date$
#  by                $Author$
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
__version__ = "$Revision$"
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
    def __init__( self                  ,
                  files                 ,
                  description = ""      ,
                  maxfiles    = 1000000 ) :  
        #
        self.files        = []
        self.patterns     = files
        self.description  = description
        self.maxfiles     = maxfiles 
        # 
        if isinstance ( files , str ) : files = [ files ]
        #
        _files = set() 
        for pattern in files :            
            for f in glob.iglob ( pattern ) :
                _files.add ( f )

        for f in _files : 
            if len ( self.files ) < self.maxfiles : self.treatFile  ( f )
            else :
                logger.warning ('Maxfiles limit is reached %s ' % self.maxfiles )
                break
                
    ## the specific action for each file 
    def treatFile ( self, the_file ) :
        self.files.append ( the_file )
        
    ## printout 
    def __str__(self):
        """
        The specific printout
        """
        return "<#files: %4d>" % len ( self.files ) 
    
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
    
    def __init__( self                  ,
                  chain                 ,
                  files       = []      ,
                  description = ''      , 
                  maxfiles    = 1000000 ) :  

        
        self.e_list1 = set()  
        self.chain   = ROOT.TChain ( chain )
        Files.__init__( self , files , description  , maxfiles )
        
    ## the specific action for each file 
    def treatFile ( self, the_file ) :
        """
        Add the file to TChain
        """
        Files.treatFile ( self , the_file )
        
        ## suppress Warning/Error messages from ROOT 
        from Ostap.Utils import rootError
        with rootError() :
            
            tmp = ROOT.TChain ( self.chain.GetName() )
            tmp.Add ( the_file )
        
            if tmp.GetEntries() and 0 < tmp.GetEntry(0) : self.chain.Add ( the_file ) 
            else                  :
                logger.warning("No/empty chain '%s' in file '%s'" % ( self.chain.GetName() , the_file ) ) 
                self.e_list1.add ( the_file ) 
                
    ## printout 
    def __str__(self):
        return  "<#files: {}; Entries: {}>".format ( len ( self.files ) ,
                                                     len ( self.chain ) )
    
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
    
    def __init__( self                  ,
                  chain1                ,
                  chain2                , 
                  files       = []      ,
                  description = ''      ,
                  maxfiles    = 1000000 ) :  
        
        self.e_list2 = set()
        self.chain2  = ROOT.TChain ( chain2 )
        Data.__init__( self , chain1 , files , description , maxfiles )
        self.chain1  = self.chain 
        
    ## the specific action for each file 
    def treatFile ( self, the_file ) :
        """
        Add the file to TChain
        """
        Data.treatFile ( self , the_file )

        ## suppress Warning/Error messages from ROOT 
        from Ostap.Utils import rootError
        with rootError() :
            
            tmp = ROOT.TChain ( self.chain2.GetName() )
            tmp.Add ( the_file )
        
            if tmp.GetEntries() and 0 < tmp.GetEntry(0) : self.chain2.Add ( the_file ) 
            else                  :
                logger.warning("No/empty chain '%s' in file '%s'" % ( self.chain2.GetName() , the_file ) ) 
                self.e_list2.add ( the_file ) 
        
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
                  description = ''   , 
                  lumi_chain  = 'GetIntegratedLuminosity/LumiTuple' , 
                  maxfiles    = 1000000                             ) :  

        Data2.__init__ ( self , chain , lumi_chain , files , description , maxfiles  ) 
        self.lumi = self.chain2 
        
    ## get the luminosity 
    def getLumi ( self ):
        """
        Get the luminosity
        """
        from   Ostap.GetLumi import getLumi
        return getLumi ( self.lumi )

    ## printout 
    def __str__(self):
        
        return "<Luminosity: {}pb-1; #files: {}; Entries: {}>".format(
            self.getLumi()     ,
            len ( self.files ) ,
            len ( self.chain ) ) 
    
# =============================================================================
if '__main__' == __name__ :
    
    from Ostap.Line import line 
    logger.info ( __file__ + '\n' + line  )
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
