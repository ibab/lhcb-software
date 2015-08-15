#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# $Id:$
# =============================================================================
## @file EOS.py 
#  (rather ugly) ``Globbing'' utility for EOS
#  - it goes through cretaion of fuse mount point
#  - it require proper installation of EOS&Co
#
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2015-08-15
#
#                    $Revision: 193591 $
#  Last modification $Date: 2015-08-12 15:10:32 +0200 (Wed, 12 Aug 2015) $
#  by                $Author: ibelyaev $
# =============================================================================
""" (rather ugly) ``Globbing'' utility for EOS
- it goes through cretaion of fuse mount point
- it require proper EOS&Co installation
"""
# =============================================================================
__version__ = "$Revision: 193591 $"
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__    = "2015-08-15"
__all__     = ( 'EOS',)
# =============================================================================
## @class EOS
#  (rather ugly) ``Globbing'' utility for EOS
#  @code
#  pattern = '/lhcb/grid/user/lhcb/user/i/ibelyaev/29/2014_03/72347/*/*EW*.mdst'
#  with EOS() as eos :
#    for i in eos.iglob (                                 pattern ) : print i 
#    for i in eos.iglob (                        '/eos' + pattern ) : print i 
#    for i in eos.iglob (         'root://eoslhcb//eos' + pattern ) : print i 
#    for i in eos.iglob ( 'root://eoslhcb.cern.ch//eos' + pattern ) : print i
#  @endcode
#  The matched name can be ``rootified'' to be directly used in ROOT.TFile.Open
#  @code
#  pattern = '/lhcb/grid/user/lhcb/user/i/ibelyaev/29/2014_03/72347/*/*EW*.mdst'
#  with EOS() as eos :
#    for i in eos.iglob ( pattern , root = True ) : print i 
#  @endcode
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2015-08-15
class EOS(object):
    """(rather ugly) ``Globbing'' utility for EOS
    >>> pattern = '/lhcb/grid/user/lhcb/user/i/ibelyaev/29/2014_03/72347/*/*EW*.mdst'
    >>> with EOS() as eos :
    ...   for i in eos.iglob (                                 pattern ) : print i 
    ...   for i in eos.iglob (                        '/eos' + pattern ) : print i 
    ...   for i in eos.iglob (         'root://eoslhcb//eos' + pattern ) : print i 
    ...   for i in eos.iglob ( 'root://eoslhcb.cern.ch//eos' + pattern ) : print i
    The matched name can be ``rootified//to be directly used in ROOT.TFile.Open
    >>> pattern = '/lhcb/grid/user/lhcb/user/i/ibelyaev/29/2014_03/72347/*/*EW*.mdst'
    >>> with EOS() as eos :
    ...   for i in eos.iglob ( pattern , root = True ) : print i
    """
    def __init__ (
        self                     ,
        eosmount  =   'eosmount' ,
        eosumount =  'eosumount' ,
        setup     = '/afs/cern.ch/project/eos/installation/client/etc/setup' ) :
        
        from   subprocess import Popen , PIPE
        
        ## check aliases/functions,
        def _check_ ( command ) :

            try :
                p   = Popen( command , stderr = PIPE , stdout = PIPE , shell = True )
                out,err = p.communicate()
                for l in err.splitlines() : return False
                return 0 == p.returncode  
            except OSError :
                return False 

        def _alias_ ( command ) :
            
            p   = Popen( [ '-i' , '-c' , 'alias -p'],
                         stderr = PIPE , stdout = PIPE , executable = '/bin/bash' )
            out,err = p.communicate()
            for l in err.splitlines() : return False 
            if 0 != p.returncode      : return False
            cmd = 'alias %s=' % command.strip() 
            for line in out.splitlines() :
                p = line.find( cmd )
                if 0 != p : continue
                return line[len(cmd):-1].strip('"').strip("'")
            return False 
                        
        def _read_setup_ ( command , setupfile ) :
            import os
            if not os.path.exists ( setupfile ) : return None
            cmd = 'alias %s=' % command 
            for line in open( setupfile, 'r') :
                p = line.find( cmd )
                if 0 != p : continue
                return line[len(cmd):-1].strip('"').strip("'")
            return None

        def _getcmd_ ( command )      :
            ## try to check it as command 
            if _check_    ( command ) : return command
            ## try to check as alias 
            cmd = _alias_ ( command )
            if cmd                    : return cmd
            ## try to read the configuration file
            cmd = _read_setup_ ( command , setup + '.sh' )
            if not cmd : raise OSError ( "Can't get correct command for '%s'" % command )
            return cmd

        eosmount  = eosmount if _check_ (  eosmount ) else _getcmd_ (  'eosmount' )
        eosumount = eosmount if _check_ ( eosumount ) else _getcmd_ ( 'eosumount' )
        
        self.cmd_start = """
        %s     %%s
        ls -al %%s/lhcb
        """ % eosmount
        self.cmd_exit  = """
        ls -al %%s/lhcb
        %s     %%s
        """ % eosumount

    ## context manager:  create EOS mount point  
    def __enter__ ( self ) :

        #
        ## create temporary directory
        #
        import tempfile
        self.tmpdir = tempfile.mkdtemp()

        #
        ## build proper commands:
        #
        self.cmd_start = self.cmd_start % ( self.tmpdir , self.tmpdir )
        self.cmd_exit  = self.cmd_exit  % ( self.tmpdir , self.tmpdir )

        self.ok = False 
        from   subprocess import Popen , PIPE
        p = Popen ( self.cmd_start , stderr = PIPE, stdout = PIPE, shell = True  )
        out,err = p.communicate()
        for l in err.splitlines() :
            if 0<= l.find('failed') :
                raise OSError ( 'Unable to create EOS mount point, check eosmount/umount')
        if 0 != p.returncode :
                raise OSError ( 'Unable to create EOS mount point, check eosmonut/umount (%s)' % p.returncode )

        ## everything fine? 
        self.ok = True 
        return self 

    ## context manager: destroy EOS mount point 
    def __exit__ ( self , *_ ) :

        if self.ok : 
            from   subprocess import Popen , PIPE
            p = Popen ( self.cmd_exit , stderr = PIPE, stdout = PIPE , shell = True )
            out,err = p.communicate()
            self.ok = not err and 0 == p.returncode
            
        import os 
        os.rmdir ( self.tmpdir)

    # =========================================================================
    ## ``globbing'' iterator
    #  @code
    #  with EOS() as eos :
    #  ...     for i in eos.iglob( pattern ) : print i 
    #  @endcode
    def iglob ( self , pattern , root = True ) :
        """
        ``globbing iterator''
        >>> with EOS() as eos :
        ...     for i in eos.iglob( pattern ) : print i 
        """
        ## default format: 
        if not root : fmt = '%s'
        else        : fmt = 'root://eoslhcb.cern.ch//eos%s'     ## ROOT 
        #
        prefix = '/eos/'
        if 0 == pattern.find ( prefix ) :
            pattern = pattern [len(prefix)-1:]
            if not root : fmt = '%s%%s' % prefix [:-1]
            else        : fmt = 'root://eoslhcb.cern.ch//eos%s' ## ROOT 
            
        prefix = 'root://eoslhcb//eos/' 
        if 0 == pattern.find ( prefix ) :
            pattern = pattern [len(prefix)-1:]
            if not root : fmt = '%s%%s' % prefix [:-1]
            else        : fmt = 'root://eoslhcb.cern.ch//eos%s' ## ROOT 

        prefix = 'root://eoslhcb.cern.ch//eos/' 
        if 0 == pattern.find ( prefix ) :
            pattern = pattern [len(prefix)-1:]
            if not root : fmt = '%s%%s' % prefix [:-1]
            else        : fmt = 'root://eoslhcb.cern.ch//eos%s'   ## ROOT 
            
        import glob             
        l = len(self.tmpdir)
        for i in glob.iglob ( self.tmpdir + pattern ) :
            yield fmt % i[l:]  

    # =========================================================================
    ## Get list of matched names via ``globbing''
    #  @code
    #  with EOS() as eos :
    #  ...  files = eos.glob( pattern )
    #  @endcode 
    def glob ( self , pattern , root = True ) :
        """Get list of matched names via ``globbing''
        >>> with EOS() as eos :
        ...   files = eos.glob( pattern )
        """
        return [ i for i in self.iglob ( self.tmpdir + pattern , root = root ) ]

# =============================================================================
if '__main__' == __name__ :
    
    ## from Ostap.Line import line 
    ## print __file__ + '\n' + line  
    print  100*'*'  
    print  __doc__  
    print  100*'*' 
    print  ' Author  : %s' %         __author__     
    print  ' Version : %s' %         __version__    
    print  ' Date    : %s' %         __date__      
    print  ' Symbols : %s' %  list ( __all__     ) 
    print  100*'*' 
    
    import sys

    args  = sys.argv[1:]
    if not args : 
        args = [ '/lhcb/grid/user/lhcb/user/i/ibelyaev/29/2014_03/72347/*/*EW*.mdst' ]
        
    files = []
    with EOS() as eos :
        for pattern in args  :
            for f in eos.iglob( pattern ) :
                print f 

    print  100*'*' 

# =============================================================================
# logging 
# =============================================================================
