#!/usr/bin/env python
# =============================================================================
# $Id: Relations.py,v 1.4 2006-02-07 14:42:16 ibelyaev Exp $
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.4 $ 
# =============================================================================
# $Log: not supported by cvs2svn $
# Revision 1.3  2006/02/07 14:34:25  ibelyaev
#  fix type
#
# =============================================================================

import sys,os,os.path,datetime  
import PyCintex

def _loadDict_ ( name ) :
    """
    Helper *LOCAL* function to load the dictionaries
    """
    if 'win32' != sys.platform and 'lib' != name[:3] : name = 'lib' + name
    return PyCintex.loadDict ( name )

_loadDict_( 'RelationsDict' )

_gbl=PyCintex.makeNamespace('')

_range = _gbl.Relations.BaseRange

def _range_iter_ ( self ) :
    """
    Iterator function which allows the sequential iteration over the range 
    """
    _r = self
    _n = _r.size()
    while _i < _num :
        yield _r.at(_i)
        _i += 1

def _range_len_ ( self ) :
    """
    helper function which allows to ise len(range) fucntions 
    """
    return self.size()

def _range_getitem_ ( self , index ) :
    if index < self.size() : return self.at(index)
    raise IndexError 
    
_range.__iter__    = _range_iter_
_range.__len__     = _range_len_  
_range.__getitem__ = _range_getitem_ 

class Rel1D ( object ) :
    def __init__ ( self , From , To ) :
        self.From = From
        self.To   = To
    def name     ( self ) :
        return "LHCb::Relation1D<%s,%s>"          % ( self.From , self.To )
    def Dict     ( self ) :
        return "GaudiDict::Relation1DDict<%s,%s>" % ( self.From , self.To )
    def guid     ( self ) :
        return _gbl.Relations.guid( self.name() )

class Rel2D ( object ) :
    def __init__ ( self , From , To ) :
        self.From = From
        self.To   = To
    def name     ( self ) :
        return "LHCb::Relation2D<%s,%s>" % ( self.From , self.To )
    def Dict     ( self ) :
        return "GaudiDict::Relation2DDict<%s,%s>" % ( self.From , self.To )
    def guid     ( self ) :
        return _gbl.Relations.guid( self.name() )


class RelW1D ( Rel1D ) :
    def __init__ ( self , From , To , Weight ) :
        Rel1D.__init__( self , From , To ) 
        self.Weight = Weight
    def name     ( self ) :
        return "LHCb::RelationWeighted1D<%s,%s,%s>" % ( self.From , self.To , self.Weight )
    def Dict     ( self ) :
        return "GaudiDict::RelationWeighted1DDict<%s,%s,%s>" % ( self.From , self.To , self.Weight )

class RelW2D ( Rel2D ) :
    def __init__ ( self , From , To , Weight ) :
        Rel2D.__init__( self , From , To ) 
        self.Weight = Weight
    def name     ( self ) :
        return "LHCb::RelationWeighted2D<%s,%s,%s>" % ( self.From , self.To , self.Weight )
    def Dict     ( self ) :
        return "GaudiDict::RelationWeighted2DDict<%s,%s,%s>" % ( self.From , self.To , self.Weight )

def _write_xml_( lines , lst ) :
    
    lines += ['<!-- * $Id: Relations.py,v 1.4 2006-02-07 14:42:16 ibelyaev Exp $'] 
    lines += ['     * ========================================================================']
    lines += ['     * $CVS tag:$, version $Revision: 1.4 $ ']
    lines += ['     * ========================================================================']
    lines += ['-->']
    lines += ['']
    lines += ['<!-- * @file']
    lines += ['     *' ] 
    lines += ['     * The basic selection file to build the Reflex dictionaries']
    if   'USERNAME' in os.environ :
        lines += ['     * @author %s ' % os.environ['USERNAME'] ]
    elif 'USER'     in os.environ :
        lines += ['     * @author %s ' % os.environ['USER'] ]
    elif 'user'     in os.environ :
        lines += ['     * @author %s ' % os.environ['user'] ]
    elif 'username' in os.environ :
        lines += ['     * @author %s ' % os.environ['username'] ]
    else :
        lines += ['     * @author %s ' % '<Unknown>' ]
        
    lines += ['     * @date  %s ' % datetime.datetime.today() ]
    lines += ['     * ========================================================================']
    lines += ['--> ']  
    lines += ['']
    lines += ['']
    #
    lines += ['<lcgdict>']
    lines += ['']
    lines += ['   <class pattern="Relations::RelationBase<*>"         />']
    lines += ['   <class pattern="Relations::RelationWeightedBase<*>" />']
    lines += ['']
    lines += ['   <class pattern="Relations::Relation2<*>"            />']
    lines += ['   <class pattern="Relations::Relation2Weighted<*>"    />']
    lines += ['']    
    lines += ['   <class pattern="IRelatio*"        />']
    lines += ['   <class pattern="Relations:*Range" />']
    lines += ['   <class pattern="Relations:*Entry" />']
    lines += ['']
    lines += ['   <class pattern="std::vector<*Relation*Entry*>" />']
    lines += ['   <class pattern="std::pair<*std::vector<*Relation*>" />']
    lines += ['']
    lines += ['   <class pattern="Relations::Relation<*>" >']    
    lines += ['         <field name="m_inverse_aux" transient="true" />']    
    lines += ['   </class>']    
    lines += ['']
    lines += ['   <class pattern="Relations::RelationWeighted<*>" >']    
    lines += ['         <field name="m_inverse_aux" transient="true" />']    
    lines += ['   </class>']    
    lines += ['']
    for o in lst :
        lines += ['']    
        lines += ['    <class name="%s" ' % o.name() ]
        lines += ['           id="%s" />' % o.guid() ]
        
    lines += ['']    
    lines += ['</lcgdict>']    
    lines += ['']
    lines += ['<!-- * ===================================================================== -->']
    lines += ['<!-- * The END * =========================================================== -->']
    lines += ['<!-- * ===================================================================== -->']
    lines += ['']
    #
    return lines 


def _write_cpp_ ( lines , lst , includes = [] ) :
    lines += ['// $Id: Relations.py,v 1.4 2006-02-07 14:42:16 ibelyaev Exp $' ] 
    lines += ['// ====================================================================']
    lines += ['// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.4 $ ']
    lines += ['// ====================================================================']
    lines += ['// Incldue files']
    lines += ['// ====================================================================']
    lines += ['// STD & STL']
    lines += ['// ====================================================================']
    lines += ['#include <string>']
    lines += ['#include <vector>']
    lines += ['// ====================================================================']
    lines += ['// Relations']
    lines += ['// ====================================================================']
    lines += ['#include "Relations/RelationsDict.h"']
    lines += ['// ====================================================================']
    lines += ['// Provided include files: ']
    lines += ['// ====================================================================']
    for file in includes : lines += ['#include "%s"' % file ]
    lines += ['// ====================================================================']
    lines += ['// Other include files: put your own includes here:']
    lines += ['// ====================================================================']
    lines += ['//#include ...']
    lines += ['//#include ...']
    lines += ['//#include ...']
    lines += ['// ====================================================================']
    lines += ['']
    lines += ['/** @file ']
    lines += ['  *       ']
    lines += ['  * Helper file for building Reflex dictionaries for Relations ']
    if   'USERNAME' in os.environ :
        lines += ['   * @author %s ' % os.environ['USERNAME'] ]
    elif 'USER'     in os.environ :
        lines += ['   * @author %s ' % os.environ['USER'] ]
    elif 'user'     in os.environ :
        lines += ['   * @author %s ' % os.environ['user'] ]
    elif 'username' in os.environ :
        lines += ['   * @author %s ' % os.environ['username'] ]
    else :
        lines += ['   * @author %s ' % '<Unknown>' ]
            
    lines += ['  * @date  %s ' % datetime.datetime.today() ]
    lines += ['  *       ']
    lines += ['  */      ']
    lines += ['']
    lines += ['namespace Dict']
    lines += ['{']
    lines += ['  struct __Instantiations ']    
    lines += ['  {']
    _i = 0 
    for o in lst :
        _i+=1 
        lines += ['%s\t_%s ;' %( o.Dict() , _i ) ]
    lines += ['  };']
    lines += ['};']    
    lines += ['']    
    lines += ['// ====================================================================']
    lines += ['// The END ============================================================']
    lines += ['// ====================================================================']
    
    return lines 

def _backup_ ( file1 , suffix = "_old" ) :
    """
    Trivial function to move/backup/rename file 
    """
    # nothing to be moved 
    if not os.path.exists( file1 ) : return
    # construct the name for back-up
    file2 = file1 + suffix
    # backup the backup file is needed 
    if os.path.exists( file2 ) : _backup_ ( file2 , suffix )
    # move the file 
    os.rename( file1 , file2 )


def prepare ( classes                    ,
              includes = []              ,
              xmlname  = "selection.xml" ,
              cppname  = "selection.h"   ) :

    for o in classes :
        print " Prepare Reflex dictionaries for class %s"% o .name()
        
    xmllines = _write_xml_ ( [] , classes )
    hlines   = _write_cpp_ ( [] , classes , includes )

    # write XML-file
    _backup_( xmlname )
    xmlfile = open( xmlname , 'w' )
    for line in xmllines : xmlfile.write(line+'\n')
    print " XML-selection file prepared: '%s' " % xmlname
    #for line in xmllines : print line

    # write header file
    _backup_( cppname )
    hfile = open( cppname , 'w' )
    for line in hlines : hfile.write(line+'\n')
    print " C++-selection file prepared: '%s' " % cppname 
    #for line in hlines : print line

if "__main__" == __name__ :

    classes  = []
    classes += [ Rel1D('int'        ,'int') ] 
    classes += [ Rel2D('std::string','int') ]

    # make XML and C++ selction files:
    prepare ( classes ) 
    
# =============================================================================
# The END 
# =============================================================================
