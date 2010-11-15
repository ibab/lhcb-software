"""Test suite for testing line builders
"""

__all__ = ('test_line_builder',
           'test_make_many_instances',
           'test_default_raises',
           'test_single_constructor_argument_raises',
           'test_lines',
           'test_line_locations',
           'test_line',
           'test_line_location',
           'test_duplicate_name_raises')

from py.test import raises
from StrippingConf.Configuration import StrippingLine

def test_line_builder(builderType, conf_dict) :
    """
    Run all tests on a line builder, given a configuration dictionary.
    All line builders should pass this test.
    """
    test_default_raises(builderType)
    test_duplicate_name_raises(builderType, conf_dict)
    test_make_many_instances(builderType, conf_dict)
    test_single_constructor_argument_raises(builderType, conf_dict)
    
def test_make_many_instances(builderType, conf_dict) :
    """
    Test that an arbitrary number of instances can be instantiated with different names.
    Access the lines of each instance to catch on-demand line building.
    """
    baseName = 'StrippingLineBuilderTest'
    for n in '0123456789' :
        b = builderType(baseName + n, conf_dict)
        lines = b.lines()

def test_duplicate_name_raises(builderType, conf_dict) :
    b0 = builderType('bob12345', conf_dict)
    raises(Exception, builderType, 'bob12345', conf_dict)
        
def test_default_raises(builderType) :
    raises(Exception, builderType)

def test_single_constructor_argument_raises(builderType, conf_dict) :
     raises(Exception, builderType, conf_dict)
     raises(Exception, builderType, 'SomeCrazyName')

def test_line(line) :
    assert type(line) == StrippingLine

def test_line_location(line) :
    """
    Test that the output location of a line called StrippingXYZ is .../XYZ.
    """
    assert 'Stripping'+line.outputLocation().split('/')[-1] == line.name()

def test_lines(builder) :
    lines = builder.lines()
    for line in lines :
        test_line(line)

def test_line_locations(builder) :
    lines = builder.lines()
    for line in lines :
        test_line_location(line)

