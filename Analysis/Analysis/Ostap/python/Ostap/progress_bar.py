#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# $Id$
# =============================================================================
## @file  progress bars for Ostap
#  @author Vanya Belyaev Ivan.Belyaev@itep.ru
#  @date   2011-12-01
#
#  - ProgressBar
# 
#  Use as class:
# 
#  @code
#  count = 0
#  total = 100000
#  bar   = ProgressBar(count, total, 77, mode='fixed', char='#')
#  while count <= total:
#    count += 1
#    bar   += 1 
#  @endcode
#  Better to use as context manager:
#  @code
#  count = 0
#  total = 100000
#  with ProgressBar(count, total, 77, mode='fixed', char='#') as bar  :
#    while count <= total:
#      count += 1
#      bar   += 1 
#  @endcode
#  With helper function:
#  @code 
#  for i in progress_bar  ( xrange(10000 ) ) :
#      .. do something here ...
#  @endcode 
#
#  - RunningBar
#  @code
#  with RunningBar () as bar :
#    for i in xrange(2000) :
#           ...
#           bar += 1 
#  @endcode
#  With helper function:
#  @code
#  for i in running_bar  ( xrange(10000 ) ) :
#      .. do something here ...
#  @endcode 
#
#  ProgressBar is an improvement from the original found at:
#    http://code.activestate.com/recipes/168639/
#
#                   $Revision$
# Last modification $Date$
# =============================================================================
""" Simple utilities for ``progrees bar'' 

- ProgressBar

Use as class:

>>> count = 0
>>> total = 100000
>>> bar   = ProgressBar(count, total, 77, mode='fixed', char='#')
>>> while count <= total:
...   count += 1
...   bar   += 1 

Better to use as context manager:

>>> with ProgressBar(count, total, 77, mode='fixed', char='#') as bar  :
>>>    while count <= total:
...       count += 1
...       bar   += 1 

Use with helper function:

>>> for i in progress_bar  ( xrange(10000 ) ) :
...       <do something here>

- RunningBar

>>> with RunningBar () as bar :
>>>    for i in xrange(2000) :
...       bar += 1

With helper function:

>>> for i in running_bar  ( xrange(10000 ) ) :
...    <do something here>


This class is an improvement from the original found at:
@see http://code.activestate.com/recipes/168639/
"""
# =============================================================================
__author__   = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__version__  = "$Revision$"
__date__     = "2011-12-01"
__all__      = (
    "ProgressBar"   , ## Progress bas as it is 
    "RunningBar"    , ## Running bar 
    "progress_bar"  , ## helper function fro ProgressBar 
    "running_bar"     ## helper function for RunningBar 
    )
# =============================================================================
import sys,os

# =============================================================================
## get number of columns for xterm
#  @code
#  ncols = columns()
#  @endcode
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
def columns ( ) :
    var  = 'COLUMNS'
    lvar = len(var) 
    from subprocess import Popen, PIPE
    p = Popen( 'resize' , stdout = PIPE , stderr = PIPE )
    out,err = p.stdout,p.stderr 
    for line in err : return -1 
    for line in out :
        p = line.find('COLUMNS')
        if 0 != line.find('COLUMNS') : continue
        try :
            ## accounts:                =    ;\n
            nc = int ( line[ p + lvar + 1 : -2 ] )
            return nc 
        except :
            pass 
    return -1 
# =============================================================================
## @class ProgressBar
#
#  This class is an improvement from the original found at:
#    http://code.activestate.com/recipes/168639/
#
# =============================================================================
# A Python Library to create a Progress Bar.
# Copyright (C) 2008  BJ Dierkes <wdierkes@5dollarwhitebox.org>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
# =============================================================================
#
#  - ProgressBar
# 
#  Use as class:
#  @code
#  count = 0
#  total = 100000
#  bar   = ProgressBar(count, total, 77, mode='fixed', char='#')
#  while count <= total:
#    count += 1
#    bar   += 1 
#  @endcode
#
#  Better to use as context manager:
#  @code
#  count = 0
#  total = 100000
#  with ProgressBar(count, total, 77, mode='fixed', char='#') as bar  :
#    while count <= total:
#      count += 1
#      bar   += 1 
#  @endcode
#
#  With helper function:
#  @code 
#  for i in progress_bar  ( xrange(10000 ) ) :
#      .. do something here ...
#  @endcode 
#  @author Vanya Belyaev Ivan.Belyaev@itep.ru
class ProgressBar(object):
    """ ProgressBar
    
    Use as class:
    
    >>> count = 0
    >>> total = 100000
    >>> bar   = ProgressBar(count, total, 77, mode='fixed', char='#')
    >>> while count <= total:
    ...   count += 1
    ...   bar   += 1 
    
    Better to use as context manager:
    
    >>> with ProgressBar(count, total, 77, mode='fixed', char='#') as bar  :
    >>>    while count <= total:
    ...       count += 1
    ...       bar   += 1 
    
    Use with helper function:
    
    >>> for i in progress_bar  ( xrange(10000 ) ) :
    ...       <do something here>
    """
    def __init__(self, min_value = 0, max_value = 100, width= 100 ,**kwargs):
        self.char = kwargs.get ( 'char' , '#'     )
        self.mode = kwargs.get ( 'mode' , 'fixed' ) # fixed or dynamic
        if not self.mode in ['fixed', 'dynamic']:
            self.mode = 'fixed'
            
        self.bar      = ''
        self.min      = min_value
        self.max      = max_value
        self.span     = max_value - min_value
        ## 
        ncols         = columns ()  - 7
        self.width    = min ( ncols , width ) if ncols > 0 else width
        
        self.prefix   = kwargs.get('description','')
        self.width    = self.width - len(self.prefix)
        
        self.amount   = 0    

        self._hashes  = -1 
        self._percent = -1 

        self.silent = kwargs.get( 'silent' , False ) 

        self.update_amount(0) 
                
    def increment_amount(self, add_amount = 1):
        return self.update_amount ( self.amount + add_amount )

    def update_amount(self, new_amount = None):
        """
        Update self.amount with 'new_amount', and then rebuild& show the bar 
        """
        if not new_amount: new_amount = self.amount
        if new_amount < self.min: new_amount = self.min
        if new_amount > self.max: new_amount = self.max
        self.amount = new_amount
        ##
        if self.build_bar() and not self.silent : self.show()
        ##
        return self

    def build_bar(self):
        """
        Figure new percent complete, and rebuild the bar string base on self.amount.
        """
        diff         = float ( self.amount - self.min )
        percent_done = int   ( round ( ( diff / float ( self.span ) ) * 100.0 ) )
 
        # figure the proper number of 'character' make up the bar 
        all_full     = self.width - 2
        num_hashes   = int(round((percent_done * all_full) / 100))

        if percent_done == self._percent and num_hashes == self._hashes : return False 
        
        if self.mode == 'dynamic':
            # build a progress bar with self.char (to create a dynamic bar
            # where the percent string moves along with the bar progress.
            self.bar = self.char * num_hashes
        else:
            # build a progress bar with self.char and spaces (to create a 
            # fixe bar (the percent string doesn't move)
            self.bar = self.char * num_hashes + ' ' * (all_full-num_hashes)
 
        percent_str = str(percent_done) + "%"
        self.bar     = '[ ' + self.bar + ' ] ' + percent_str

        self._hashes  = num_hashes
        self._percent = percent_done 

        return True

    def __iadd__ ( self , i ) : return self.increment_amount ( i)
    def __str__(self):
        return str(self.bar)

    def show ( self ) :
        if self.prefix : sys.stdout.write( self.prefix ) 
        sys.stdout.write( self.bar + '\r' ) 
        sys.stdout.flush()
        
    def end  ( seld  ) :
        if not self.silent :
            if self.prefix : sys.stdout.write( self.prefix ) 
            sys.stdout.write( self.bar + '\n' ) 
            sys.stdout.flush()
        
    def __enter__ ( self      ) : return self
    def __exit__  ( self , *_ ) :
        self.end()
        return False ##  allow propagation of exceptions 

# =============================================================================
## helper function to display progress bar
#  @code 
#  for i in progress_bar  ( xrange(10000 ) ) :
#      .. do something here ...
#  @endcode 
#  @author Vanya Belyaev Ivan.Belyaev@itep.ru
def progress_bar ( iterable , max_value = None , **kwargs ) :
    """ Helper function to display progress bar 
    
    >>> for i in progress_bar  ( xrange(10000 ) ) :
    ...       <do something here>
    """
    if max_value is None : max_value = len ( iterable )
    with ProgressBar ( max_value = max_value , **kwargs ) as bar : 
        for i in iterable :
            yield i
            bar += 1
            

# =============================================================================
_bar_  =  ( 'Running ... |\r'  , 
            'Running ... /\r'  , 
            'Running ... -\r'  ,
            'Running ... \\\r' )
_lbar  = len(_bar_)
_done_ =    'Done        %d             \n' 
# =============================================================================
## @class RunningBar 
#  - RunningBar
#  @code
#  with RunningBar () as bar :
#    for i in xrange(2000) :
#           ...
#           bar += 1 
#  @endcode
#  With helper function:
#  @code
#  for i in running_bar  ( xrange(10000 ) ) :
#      .. do something here ...
#  @endcode 
#  @author Vanya Belyaev Ivan.Belyaev@itep.ru
class RunningBar(object):
    """ RunningBar
    
    >>> with RunningBar () as bar :
    >>>    for i in xrange(2000) :
    ...       bar += 1
    
    With helper function:
    
    >>> for i in running_bar  ( xrange(10000 ) ) :
    ...    <do something here>
    
    """
    
    def __init__(self, *args ,**kwargs ) :
        self.amount   = 0 
        self.freq     = int( kwargs.get( 'frequence' , 100 ) )
        self.prefix   = kwargs.get('description' , '' ) 
        self.silent   = kwargs.get( 'silent' , False ) 
        self.update_amount() 
        
    def increment_amount(self, add_amount = 1):
        return self.update_amount ( self.amount + add_amount )

    def update_amount(self, new_amount = None ):
        """
        Update self.amount with 'new_amount', and then rebuild the bar string.
        """
        if not new_amount: new_amount = self.amount
        self.amount = new_amount 
        ##
        if not self.silent : self.show() 
        ##
        return self

    
    def __iadd__ ( self , i ) : return self.increment_amount ( i)
    def __str__(self):
        return str(self.bar)

    def show ( self ) :
        if self.prefix : sys.stdout.write (  self.prefix ) 
        iq = self.amount % self.freq
        ib = self.amount % _lbar  
        if iq : sys.stdout.write ( _bar_ [ ib ] )
        else  : sys.stdout.write ( _bar_ [ ib ][:-1] + ' ' + str(self.amount) + '\r' ) 
        sys.stdout.flush ()

    def end  ( self ) :
        if not self.silent : 
            if self.prefix : sys.stdout.write (  self.prefix ) 
            sys.stdout.write ( _done_ % self.amount )
            sys.stdout.flush ()
        
    def __enter__ ( self      ) : return self
    def __exit__  ( self , *_ ) :
        self.end() 
        return False ##  allow propagatuon of exceptions 

            
# ==============================================================================
## helper function to display running bar 
#  @code
#  for i in running_bar  ( xrange(10000 ) ) :
#      .. do something here ...
#  @endcode 
#  @author Vanya Belyaev Ivan.Belyaev@itep.ru
def running_bar ( iterable , frequency = 100 , description = '' , **kwargs ) :
    """ Helper function to display runnning bar 
    >>> for i in running_bar  ( xrange(10000 ) ) :
    ...    <do something here>    
    """
    with RunningBar ( frequency = frequency , description = description , **kwargs ) as bar :
        for i in iterable :
            yield i
            bar += 1

            
# =============================================================================

def main():

    limit = 1000000
    
    print 'Example 1: Fixed Bar'
    with ProgressBar(0, limit,  mode='fixed') as bar : 
        for i in xrange(limit+1):
            bar += 1 
 
    print '\n'
    
    print 'Example 2: Dynamic Bar'
    with ProgressBar(0, limit, mode='dynamic', char='-') as bar : 
        for i in xrange(limit+1):
            bar += 1 

    import time 
    for i in progress_bar( xrange(10000) , description = "Doing something ") : 
        time.sleep(0.001)
        
    for i in running_bar( xrange(10000) , description  = "Empty looping   ") :         
        time.sleep(0.001)

# ==============================================================================
if __name__ == '__main__':

    import Ostap.Line
    from   AnalysisPython.Logger import getLogger
    logger = getLogger('Ostap.progress_bar')
    
    logger.info ( __file__  + '\n' + Ostap.Line.line  ) 
    logger.info ( 80*'*'   )
    logger.info ( __doc__  )
    logger.info ( 80*'*' )
    logger.info ( ' Author  : %s' %         __author__    ) 
    logger.info ( ' Version : %s' %         __version__   ) 
    logger.info ( ' Date    : %s' %         __date__      )
    logger.info ( ' Symbols : %s' %  list ( __all__     ) )
    logger.info ( 80*'*' ) 
  
    main()
    logger.info ( 80*'*' ) 
    
# =============================================================================
# The END 
# =============================================================================

