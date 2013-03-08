"""
Simple python representation for tracking alignment and some helper methods
"""
__author__ = "Pieter David <pieter.david@nikhef.nl>"
__date__   = "August 2012"

__all__  = ( "Transformation"
           , "AlignableTreeNode"
           , "loopTrees"
           , "simpleDiff", "diffStat"
           )

import logging

############################################################
#                  Representation classes                  #
############################################################

class Transformation(object):
    """
    Trivial python class representation of a transformation

    Has Tx, Ty, Tz, Rx, Ry and Rz properties, and provides
    a string representation and (non)-equality operators.
    """
    __slots__ = { "Tx" : 0.0 , "Ty" : 0.0 , "Tz" : 0.0
                , "Rx" : 0.0 , "Ry" : 0.0 , "Rz" : 0.0 }

    def __init__( self, (Tx, Ty, Tz), (Rx, Ry, Rz) ):
        """
        Constructor

        From (Tx, Ty, Tz) and (Rx, Ry, Rz) tuples
        """
        for ct in Transformation.__slots__:
            setattr( self, ct, locals().get(ct) )
    def __str__( self ):
        """
        String representation "T ( Tx, Ty, Tz ), R ( Rx, Ry, Rz )"
        """
        return "T ( %s , %s, %s ), R ( %s , %s, %s )" % ( self.Tx, self.Ty, self.Tz, self.Rx, self.Ry, self.Rz )
    def __eq__( self, other ):
        """
        Equality of two transformations: all parameters need to be equal.
        """
        ret = True
        for const in Transformation.__slots__:
            if getattr(self, const) != getattr(other, const):
                ##print "Difference in %s : %s vs %s" % ( const, str(getattr(self, const)), str(getattr(other, const)) )
                ret = False
        return ret
    def __ne__( self, other ):
        """
        Non-equality: negation of equality
        """
        return not self.__eq__( other )

class DuplicateChildNameException(Exception):
    """
    Exception to raise if several child nodes have the same name
    """
    def __init__( self, node, name, matchingChildren ):
        self.node = node
        self.name = name
        self.matchingChildren = matchingChildren
        super(DuplicateChildNameException, self).__init__(self)
    def __str__( self ):
        return "Node %s has several children with name %s: [ %s ]" % ( self.node, self.name, ", ".join((c for c in self.matchingChildren)) )

class MisMatchingTreesException(Exception):
    """
    Exception to raise when trying to loop over trees with a different structure
    """
    def __init__( self, node1, node2 ):
        self.node1 = node1
        self.node2 = node2
        super(MisMatchingTreesException, self).__init__(self)
    def __str__( self ):
        return "Different structure : node %s vs %s" % ( self.node1, self.node2 )

class AlignableTreeNode(object):
    """
    Node in a simple alignment tree

    Provides a tree structure (similar to the detector geometry) to represent
    a tracking alignment.
    Every node is then an alignable, and the value is its Transformation
    """

    __slots__ = { "name" : "" , "value" : None , "children" : [] }

    def __init__( self, name, value, children=[] ):
        """
        Constructor

        The "children" parameter is currently expected to be a
        (possibly empty) list of AlignableTreeNodes, for efficiency.
        (child lookup by name is only required at construction)
        """
        self.name     = name
        self.value    = value
        self.children = children
    def setNodeValue( self, splitPath, value, create=True, overwrite=False ):
        """
        Set the value of specified node in the hierarchy

        Parameters:
         - splitPath : relative path (sequence of node names) to the node
         - value     : the value to be set
         - create    : create nodes, if non-existing [default: true]
         - overwrite : overwrite the current value of the node, if present [default: False]
        """
        nd = self.getNode(splitPath, create=create)
        if not nd.value or overwrite:
            nd.value = value
        else:
            logging.error("Trying to overwrite value %s of child %s with %s" % ( child.value, child.name, value ))
    def getNode( self, splitPath, create=False ):
        """
        Get the node from the tree

        Parameters:
         - splitPath : relative path (sequence of node names) to the node
         - create    : create nodes, if non-existing [default: true]
        """
        if len(splitPath) == 0:
            return self
        child = self.getChild(splitPath[0])
        if child:
            if len(splitPath) == 1:
                return child
            else:
                return child.getNode(splitPath[1:], create=create)
        elif create:
            newChild = AlignableTreeNode(splitPath[0], None, children=[])
            self.children.append(newChild)
            if len(splitPath) == 1:
                return newChild
            else:
                return newChild.getNode(splitPath[1:], create=create)
        else:
            return None
    def loop( self, parentName=None ):
        """
        Ordered iterator over all children, constructing the "full path"

        Generator yielding first the current node, then all its children.
        The full path is constructed by joining the (optional) parentName
        with the node names, separated by "/".
        """
        fullName = self.name
        if parentName:
            fullName = "/".join((parentName, self.name))
        yield fullName, self.value
#        logging.debug("(in AlignableTreeNode.loop) fullName, self.value: %s, %s" % (fullName, self.value)) # super-verbose
        for c in sorted(self.children, key = lambda ch : ch.name):
            for ccN, ccV in c.loop(fullName):
                yield ccN, ccV
    def getChild( self, childName ):
        """
        Find a child node by name

        Returns None if no results.
        Raises a DuplicateChildNameException if more than one child node matches.
        """
        results = [ c for c in self.children if c.name == childName ]
        if len(results) < 1:
            return None
        elif len(results) == 1:
            return results[0]
        else:
            logging.error("Several child nodes with name %s" % childName)
            raise DuplicateChildNameException(self, childName, results)
    def __str__( self ):
        """
        String representation of the node
        """
        return self.toString()
    def toString(self, nLevels=0):
        """
        String representation, including the first nLevels generations of child nodes
        """
        if nLevels > 0:
            return "( %s : %s [ %s ] )" % ( self.name, str(self.value), ", ".join(( c.toString(nLevels=nLevels-1) for c in self.children )) )
        else:
            return "( %s : %s [ %i ] )" % ( self.name, str(self.value), len(self.children) )

def loopTrees( aligns ):
    """
    Sort of zipped iteritems() for several alignments' trees loop()

    Yields ( node name, ( val_0, val_1, ... val_(n-1) ) ), with val_i the node value in aligns[i]
    """
    detLoops = [ align.loop() for align in aligns ]
    for res in zip( *detLoops ):
        for alRes in res:
            if not alRes[0] == res[0][0]:
                logging.error( "Trees are different! Node %s vs %s" % ( alRes[0], res[0][0] ) )
                raise MisMatchingTreesException( alRes[0], res[0][0] )
        yield res[0][0], tuple( alRes[1] for alRes in res )


############################################################
#                       Diff methods                       #
############################################################

def simpleDiff( align1, align2 ):
    """
    Naive diff between two alignments

    Output format: a list of ( node full name, transformation in align1, transformation in align2 )
    """
    diff = []
    for name1, (transf1, transf2) in loopTrees(( align1, align2 )):
        #print "Comparing for element %s (%s vs %s)" % (name1, transf1, transf2)
        if not( transf1 == transf2 ):
            print "For element %s" % name1
            diff.append(( name1, transf1, transf2 ))
    return diff

def diffStat( diffList ):
    """
    Some statistics on a simpleDiff result

    Yields (constant, number of elements where constant is different, list of elements where constant is different )
    """
    histogram = [ 0 for c in Transformation.__slots__ ]
    listsPerDOF = [ [] for c in Transformation.__slots__ ]
    for elm, transf1, transf2 in diffList:
        for i, (c, cList) in enumerate(zip(Transformation.__slots__, listsPerDOF)):
            if getattr(transf1, c) != getattr(transf2, c):
                histogram[i] += 1
                cList.append(elm)
    return zip(Transformation.__slots__, histogram, listsPerDOF)
