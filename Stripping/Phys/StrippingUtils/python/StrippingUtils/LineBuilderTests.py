"""Test suite for testing line builders
"""

__all__ = ('test_line_builder',
           'test__build_many_instances',
           'test_second_instance_does_not_overwrite_lines',
           'test_default_raises',
           'test_single_constructor_argument_raises',
           'test_lines',
           'test_cannot_modify_lines',
           'test_line_locations',
           'test_line',
           'test_line_location',
           'test_duplicate_name_raises',
           'test_configuration_not_dictlike_raises',
           'test_bad_configuration_raises')

from py.test import raises
from copy import deepcopy
from StrippingConf.Configuration import StrippingLine

def test_line_builder(builderType, conf_dict) :
    """
    Run all tests on a line builder, given a configuration dictionary.
    All line builders should pass this test.
    """
    print 'test_line_builder', builderType.__name__, '...'
    test_default_raises(builderType)
    test_single_constructor_argument_raises(builderType, conf_dict)
    test_duplicate_name_raises(builderType, conf_dict)
    test_build_many_instances(builderType, conf_dict)
    test_second_instance_does_not_overwrite_lines(builderType, conf_dict)
    test_configuration_not_dictlike_raises(builderType, 0)
    test_bad_configuration_raises(builderType, conf_dict)
        
def test_build_many_instances(builderType, conf_dict) :
    print 'test_make_many_instances', builderType.__name__, '...'
    """
    Test that an arbitrary number of instances can be instantiated with different names.
    Access the lines of each instance to catch on-demand line building.
    """
    baseName =  builderType.__name__
    for n in '0123456789' :
        b = builderType(baseName + n, conf_dict)
        lines = b.lines()
        test_linebuilder_instance(b)

def test_second_instance_does_not_overwrite_lines(builderType, conf_dict) :
    print 'test_second_instance_does_not_overwrite_lines', builderType.__name__, '...'    
    b0 = builderType(builderType.__name__+'FirstBuilder', conf_dict)
    b0lines = b0.lines()
    b1 = builderType(builderType.__name__+'SecondBuilder', conf_dict)
    assert b0.lines() == b0lines
    
def test_linebuilder_instance(b) :
    test_lines(b)
    test_cannot_modify_lines(b)
    test_line_locations(b)
    test_lineNames_method(b)
    test_outputLocations_method(b)
    test_lineFromName_method(b)
        
def test_duplicate_name_raises(builderType, conf_dict) :
    print 'test_duplicate_name_raises', builderType.__name__, '...'
    name=builderType.__name__+'Instance'
    b0 = builderType(name, conf_dict)
    raises(Exception, builderType, name, conf_dict)
        
def test_default_raises(builderType) :
    print 'test_default_raises', builderType.__name__, '...'
    raises(Exception, builderType)

def test_single_constructor_argument_raises(builderType, conf_dict) :
    print 'test_single_constructor_argument_raises', builderType.__name__, '...'
    raises(Exception, builderType, conf_dict)
    raises(Exception, builderType, 'SomeCrazyName')

def test_configuration_not_dictlike_raises(builderType, conf_dict) :
    print 'test_configuration_not_dictlike_raises', builderType.__name__, '...'
    raises(AttributeError, builderType, 'TestBadConfDict', conf_dict)

def test_bad_configuration_raises(builderType, good_conf_dict) :
    print 'test_bad_configuration_raises', builderType.__name__, '...'
    assert hasattr(good_conf_dict, 'keys')
    assert hasattr(good_conf_dict, '__getitem__')
    bad_conf_dict = deepcopy(good_conf_dict)
    bad_conf_dict['BAD_KEY'] = 0.
    raises(KeyError, builderType, 'TestBadConfKeys', bad_conf_dict)
           
def test_line(line) :
    assert type(line) == StrippingLine

def test_line_location(line, allowEmptyLocation=True) :
    """
    Test that the output location of a line called StrippingXYZ is .../XYZ.
    If allowEmptyLocation = True accept an empty string.
    """
    if allowEmptyLocation :
        if not line.outputLocation() or line.outputLocation() =='':
            return
    assert 'Stripping'+line.outputLocation().split('/')[-2] == line.name()

def test_lines(builder) :
    print 'test_lines', type(builder).__name__, '...'
    lines = builder.lines()
    for line in lines :
        test_line(line)

def test_cannot_modify_lines(builder) :
    """
    Test that the builder.lines() field cannot be modified.
    If builder.lines() returns a list-like type that supports += [...] syntax,
    check that no modification ocurrs. Else, all is good. Check that there
    is no change anyway.
    """
    print 'test_cannot_modify_lines', type(builder).__name__, '...'    
    orig_lines = builder.lines()
    mod_lines = builder.lines()
    try :
        mod_lines += ['BAD LINE']
        assert builder.lines() != mod_lines
    except TypeError :
        pass
    assert builder.lines() == orig_lines

    
def test_line_locations(builder, allowEmptyLocation=True) :
    """
    Test that the output locations of a set of lines called
    StrippingXYZ, StrippingABC, .. is .../XYZ .../ABC, etc.
    If allowEmptyLocation = True accept empty strings.
    """
    print 'test_line_locations', type(builder).__name__, '...'
    lines = builder.lines()
    for line in lines :
        test_line_location(line, allowEmptyLocation)


def test_lineNames_method(builder):
    names = [l.name() for l in builder.lines()]
    for name in names :
        assert name in builder.lineNames()

def test_outputLocations_method(builder) :
    locations = [line.outputLocation() for line in builder.lines()]
    for location in locations :
        assert location in builder.outputLocations()

def test_lineFromName_method(builder) :
    for line in builder.lines() :
        assert line == builder.lineFromName(line.name())
