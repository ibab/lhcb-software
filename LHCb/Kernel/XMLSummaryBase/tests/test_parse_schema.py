#!/usr/bin/env python
# =============================================================================
# $Id: test_parse_schema.py,v 1.2 2009-09-23 13:00:56 rlambert Exp $
# =============================================================================
"""
*******************************************************************************
*      test the schema parsing.                                               *
*      python  test_parse_schema.py                                           *
*******************************************************************************
"""
# =============================================================================
__author__ = 'Rob Lambert Rob.Lambert@cern.ch'
# =============================================================================


#test I can import it!
from   XMLSummaryBase import schema

topdir='$XMLSUMMARYBASEROOT/'
xsdfile='xml/XMLSummary.xsd'
xmlfile1='xml/Summary_Minimal.xml'
xmlfile2='xml/Summary_Example.xml'


#test I can parse the schema
xsd=schema.Schema(topdir+xsdfile)
print 'schema parsed'

#test the schema can make the default
sum_default=xsd.create_default(xsd.root())
if sum_default is None:
    raise AssertionError, 'problem with default summary'
else:
    print 'created default OK'

#test the schema can parse the examples
sum_minimal=xsd.parse(topdir+xmlfile1)
sum_example=xsd.parse(topdir+xmlfile2)
print 'parsed example xml OK'

#test that the schema prevents me from doing something stupid

try:
    sum_default.value('this should not work!!')
    raise AssertionError, 'rubbish was NOT prevented by the schema'
except (ValueError, TypeError, NameError, AttributeError):
    print 'assigning rubbish value was prevented by the schema'
try:
    sum_default.add('this should not work!!')
    raise AssertionError, 'adding rubbish child was NOT prevented by the schema'
except (ValueError, TypeError, NameError, AttributeError):
    print 'adding rubbish child was prevented by the schema'
try:
    sum_default.add(sum_default)
    raise AssertionError, 'adding child of the wrong type was NOT prevented by the schema'
except (ValueError, TypeError, NameError, AttributeError):
    print 'adding child of the wrong type was prevented by the schema'
try:
    sum_default.attrib('this should not work!!')
    raise TypeError, 'getting rubbish attribute was NOT prevented by the schema'
except (ValueError, TypeError, NameError, AttributeError):
    print 'referencing rubbish attribute was prevented by the schema'
try:
    sum_default.attrib('version', 'this should not work!!')
    raise AssertionError, 'rubbish attribute was NOT prevented by the schema'
except (ValueError, TypeError, NameError, AttributeError):
    print 'assigning rubbish attribute was prevented by the schema'

print 'get/setter tests complete'
