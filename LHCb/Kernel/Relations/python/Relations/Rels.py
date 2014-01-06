#!/usr/bin/env python
# =============================================================================
# $Id: Rels.py,v 1.2 2008-11-02 16:44:38 ibelyaev Exp $
# =============================================================================
"""
Simple module for 'decorations' of various relation tables
for interactive work in (Gauid)python
"""
# =============================================================================
__author__ = "Vanya BELYAEV <ibelyaev@physics.syr.edu>"
# =============================================================================


import sys,os,os.path,datetime

try:
    import cppyy
except ImportError:
    # FIXME: backward compatibility
    print "# WARNING: using PyCintex as cppyy implementation"
    import PyCintex as cppyy
    import sys
    sys.modules['cppyy'] = cppyy

cppyy.loadDict('RelationsDict')

_gbl = GaudiPython.gbl

_range = _gbl.Relations.BaseRange

def _range_iter_ ( self ) :
    """
    Iterator function which allows the sequential iteration over the range

    >>> range = ...
    >>> for i in range : print i

    """
    _r   = self
    _num = _r.size()
    _i   = 0
    while _i < _num :
        yield _r.at(_i)
        _i += 1

def _range_len_ ( self ) :
    """
    Helper function which allows to use len(range) functions

    >>> range = ...
    >>> len( range )

    """
    return self.size()

_range.__iter__    = _range_iter_
_range.__len__     = _range_len_

def _entry_ ( _from , _to , _weight = None ) :
    """ get the corresponding entry type from the types """
    if _weight :
        name = "Relations::WEntry_<%s,%s,%s>" % ( _from , _to , _weight )
    else       :
        name = "Relations::Entry_<%s,%s>"     % ( _from , _to )
    return getattr(gbl, name)

def _range_ ( _from , _to , _weight = None ) :
    """ get the corresponding range type from the types """
    entry = _entry_ ( _from  , _to , _weight )
    entries = _gbl.std.vector( entry )
    return _gbl.Relations.Range_( entries )

def __unique_and_sort__ ( lst ) :
    """ LOCAL: return the unique elements of sorted list """
    _dct=  {}
    for item in lst : _dct[item] = item
    _lst = _dct.keys()
    _lst.sort()
    return _lst

def _substitute_stream_ ( klass ) :
    """ substitute '>>' with '> >' in (templated) class name """
    index = klass.find('>>')
    while -1 != index :
        klass = klass.replace('>>','> >')
        index = klass.find( '>>' )
    index = klass.find('  ')
    while -1 != index :
        klass = klass.replace('  ',' ')
        index = klass.find( '  ' )
    return klass

def _fmt_type_ ( fmt , klass ) :
    rep = fmt%klass
    return _substitute_stream_ ( rep )

def _writeLines_ ( klass ) :
    good1 = 0 == klass.find( 'Relations::Relation<' )
    good2 = 0 == klass.find( 'Relations::RelationWeighted<' )
    good3 = 0 == klass.find( 'Relations::Relation2<' )
    good4 = 0 == klass.find( 'Relations::Relation2Weighted<' )
    good5 = 0 == klass.find( 'Relations::Range_<' )
    if good1 or good2 :
        lines  = [ ' <class name="%s" >' % klass ]
        lines += [ '    <field name="m_inverse_aux" transient="true" />' ]
        lines += [ ' </class> ' ]
        return lines
    elif good3 or good4 :
        lines  = [ ' <class name="%s" >' % klass ]
        lines += [ '    <field name="m_inverse" transient="true" />' ]
        lines += [ ' </class> ' ]
        return lines
    elif good5 :
        lines  = [ ' <class name="%s" >' % klass ]
        lines += [ '    <field name="m_begin" transient="true" />' ]
        lines += [ '    <field name="m_end"   transient="true" />' ]
        lines += [ ' </class> ' ]
        return lines
    return [ ' <class name="%s" /> ' % klass ]

class Rel   ( object ) :
    def __hash__ ( self ) : return hash (                self.name() )
    def __repr__ ( self ) : return                       self.name()
    def   guid   ( self ) : return _gbl.Relations.guid ( self.name() )
    def __cmp__  ( self , other ) : return cmp( self.name() , other.name() )

class Rel1D ( Rel ) :
    def __init__ ( self , From , To ) :
        self.From = From
        self.To   = To
    def related ( self ) : return [] ##  Rel1D( self.To , self.From ) ]
    def types ( self ) :
        pair = ( self.From , self.To )
        table   = _fmt_type_ ( "LHCb::Relation1D<%s,%s>"         , pair    )
        ibase   = _fmt_type_ ( "IRelation<%s,%s>"                , pair    )
        base0   = _fmt_type_ ( "Relations::RelationBase<%s,%s>"  , pair    )
        base1   = _fmt_type_ ( "Relations::Relation<%s,%s>"      , pair    )
        entry   = _fmt_type_ ( "Relations::Entry_<%s,%s>"        , pair    )
        entries = _fmt_type_ ( "std::vector<%s>"                 , entry   )
        range   = _fmt_type_ ( "Relations::Range_<%s>"           , entries )
        return ( table , [ ibase , base0   , base1 ,
                           entry , entries , range ] )
    def name     ( self ) : return self.types()[0]
    def Dict     ( self ) :
        return _fmt_type_ ( "GaudiDict::Relation1DDict<%s,%s>" , ( self.From , self.To ) )

class Rel2D ( Rel ) :
    def __init__ ( self , From , To ) :
        self.From = From
        self.To   = To
    def related ( self ) : return [ Rel2D( self.To   , self.From ) ]
    def types  ( self ) :
        pair    = ( self.From , self.To )
        table   = _fmt_type_ ( "LHCb::Relation2D<%s,%s>"         , pair    )
        ibas1   = _fmt_type_ ( "IRelation<%s,%s>"                , pair    )
        ibas2   = _fmt_type_ ( "IRelation2D<%s,%s>"              , pair    )
        base0   = _fmt_type_ ( "Relations::RelationBase<%s,%s>"  , pair    )
        base1   = _fmt_type_ ( "Relations::Relation<%s,%s>"      , pair    )
        base2   = _fmt_type_ ( "Relations::Relation2<%s,%s>"     , pair    )
        entry   = _fmt_type_ ( "Relations::Entry_<%s,%s>"        , pair    )
        entries = _fmt_type_ ( "std::vector<%s>"                 , entry   )
        range   = _fmt_type_ ( "Relations::Range_<%s>"           , entries )
        return ( table , [ ibas1 , ibas2   , base0 , base1 , base2 ,
                           entry , entries , range ] )
    def name     ( self ) : return self.types()[0]
    def Dict     ( self ) :
        return _fmt_type_ ( "GaudiDict::Relation2DDict<%s,%s>" , ( self.From , self.To ) )

class RelW1D ( Rel ) :
    def __init__ ( self , From , To , Weight ) :
        self.From   = From
        self.To     = To
        self.Weight = Weight
    def related ( self ) : return [] ##  RelW1D( self.To , self.From , self.Weight ) ]
    def types   ( self ) :
        triplet = ( self.From , self.To , self.Weight )
        table   = _fmt_type_ ( "LHCb::RelationWeighted1D<%s,%s,%s>"        , triplet )
        ibase   = _fmt_type_ ( "IRelationWeighted<%s,%s,%s>"               , triplet )
        base0   = _fmt_type_ ( "Relations::RelationWeightedBase<%s,%s,%s>" , triplet )
        base1   = _fmt_type_ ( "Relations::RelationWeighted<%s,%s,%s>"     , triplet )
        entry   = _fmt_type_ ( "Relations::WEntry_<%s,%s,%s>"              , triplet )
        entries = _fmt_type_ ( "std::vector<%s>"                           , entry   )
        range   = _fmt_type_ ( "Relations::Range_<%s>"                     , entries )
        return ( table , [ ibase , base0   , base1 ,
                           entry , entries , range ] )
    def name     ( self ) : return self.types()[0]
    def Dict     ( self ) :
        return _fmt_type_ ( "GaudiDict::RelationWeighted1DDict<%s,%s,%s>", ( self.From , self.To , self.Weight ) )

class RelW2D ( Rel ) :
    def __init__ ( self , From , To , Weight ) :
        self.From   = From
        self.To     = To
        self.Weight = Weight
    def related ( self ) :
        lst = [ RelW2D( self.To   , self.From , self.Weight ) ]
        return lst
    def types  ( self ) :
        triplet = ( self.From , self.To , self.Weight )
        table   = _fmt_type_ ( "LHCb::RelationWeighted2D<%s,%s,%s>"              , triplet )
        ibas1   = _fmt_type_ ( "IRelationWeighted<%s,%s,%s>"                     , triplet )
        ibas2   = _fmt_type_ ( "IRelationWeighted2D<%s,%s,%s>"                   , triplet )
        base0   = _fmt_type_ ( "Relations::RelationWeightedBase<%s,%s,%s>"       , triplet )
        base1   = _fmt_type_ ( "Relations::RelationWeighted<%s,%s,%s>"           , triplet )
        base2   = _fmt_type_ ( "Relations::Relation2Weighted<%s,%s,%s>"          , triplet )
        entry   = _fmt_type_ ( "Relations::WEntry_<%s,%s,%s>"                    , triplet )
        entries = _fmt_type_ ( "std::vector<%s>"                                 , entry   )
        range   = _fmt_type_ ( "Relations::Range_<%s>"                           , entries )
        return ( table , [ ibas1  , ibas2   , base0 , base1 , base2 ,
                           entry , entries , range ] )
    def name     ( self ) : return self.types()[0]
    def Dict     ( self ) :
        return _fmt_type_ ( "GaudiDict::RelationWeighted2DDict<%s,%s,%s>" , ( self.From , self.To , self.Weight ) )

def _write_xml_( lines , lst ) :

    lines += ['<!-- * $Id: Rels.py,v 1.2 2008-11-02 16:44:38 ibelyaev Exp $ ']
    lines += ['     * ========================================================================']
    lines += ['     * $CVS tag:$, version $Revision: 1.2 $ ']
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

    classes = []
    for o in lst : classes += o.types()[1]
    classes = __unique_and_sort__ ( classes )

    for o in lst :
        lines += ['']
        types = o.types()
        klass = types[0]
        lines     += ['    <class name="%s" '  % klass    ]
        lines     += ['           id="%s" />'  % o.guid() ]
    lines += ['']
    for o in classes :
        lines     += _writeLines_ ( o )

    lines += ['']
    lines += [' <class pattern="Relations::Pointer<*>"   /> ']
    lines += [' <class pattern="Relations::Reference<*>" /> ']
    lines += ['']
    lines += [' <!-- * EXCLUSIONS                       * -->']
    lines += [' <!-- *  1) Not needed for interactivity * -->']
    lines += [' <!-- *  2) Not needed for persistency   * -->']
    lines += ['']
    lines += [' <exclusion>']
    lines += ['']
    lines += [' <class pattern="IRelation*">']
    lines += ['    <method name="="/>']
    lines += [' </class>']
    lines += [' <class pattern="LHCb::Relation*">']
    lines += ['    <method name="="/>']
    lines += [' </class>']
    lines += [' <class pattern="Relations::Relation*">']
    lines += ['    <method name="="/>']
    lines += [' </class>']
    lines += [' <class pattern="Relations::Entry_*">']
    lines += ['    <method name="="/>']
    lines += [' </class>']
    lines += [' <class pattern="Relations::WEntry_*">']
    lines += ['    <method name="="/>']
    lines += [' </class>']
    lines += [' <class pattern="Relations::Range_*">']
    lines += ['    <method name="="/>']
    lines += [' </class>']
    lines += [' <class pattern="Relations::Pointer*">']
    lines += ['    <method name="="/>']
    lines += [' </class>']
    lines += [' <class pattern="Relations::Reference*">']
    lines += ['    <method name="="/>']
    lines += [' </class>']
    lines += [' <class pattern="std::vector&lt;Relations::*&gt;">']
    lines += ['    <method name="="/>']
    lines += [' </class>']
    lines += ['']
    lines += [' </exclusion>']
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
    lines += ['// $Id: Rels.py,v 1.2 2008-11-02 16:44:38 ibelyaev Exp $' ]
    lines += ['// ====================================================================']
    lines += ['// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.2 $ ']
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
    lines += ['  struct _Instantiations ']
    lines += ['  {']
    _i = 0
    for o in lst :
        _i+=1
        lines += ['        %s\t_%s ;' %( o.Dict() , _i ) ]
    lines += ['  };']
    lines += ['} // end of namespace Dict ']
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

    def _helper_ ( lst , num = 10 ) :
        _lst = lst
        for i in range(0,num) :
            _tmp = []
            for o in _lst :
                _tmp += [ o ]
                _tmp += o.related()
            _lst = __unique_and_sort__ ( _tmp )
        return _lst
    classes = _helper_ ( classes )

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
    #classes += [ Rel1D('int'        ,'int') ]
    classes += [ Rel1D('std::string','int') ]

    # make XML and C++ selction files:
    prepare ( classes )

# =============================================================================
# The END
# =============================================================================
