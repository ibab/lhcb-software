## @file DrawHelper.py
#
# @brief routines to help drawing in a sane way from a GUITree
#
# @author Manuel Schiller <manuel.schiller@nikhef.nl>
# @date 2013-05-06
#
## @package DrawHelper
#
# @brief routines to help drawing in a sane way from a GUITree
#
# @author Manuel Schiller <manuel.schiller@nikhef.nl>
# @date 2013-05-06
#
import ROOT

## flatten the given coordinate
#
# When drawing some quantity, that quantity may be scalar or it may have vector
# (of vector (of vector (...))) character. In the latter case, the qantity
# needs to be flattened to a list of scalars.
#
# @param c      (unflattened) coordinate
# @return       list of (flattened) values, scalars are converted to
#               one-element lists
def __flattenCoordinate(c):
    return sum(
            (__flattenCoordinate(item) if hasattr(item, '__iter__') or
                hasattr(item, '__len__') else [float(item)]
                for item in (c.itervalues() if hasattr(c, 'itervalues') else c)), []
            ) if hasattr(c, '__iter__') or hasattr(c, '__len__') else [float(c)]

## verify compatibility in shape of coordinates to be drawn, extend if needed
#
# When drawing a set of coordinates and weights, the tuples representing any
# one coordinate or weight must either all have the same length, or some must
# have the same length and the others must be a single number. Otherwise, it's
# hard to make sense of the operation. This function verifies this contraint.
#
# In the case where there is at least one coordinate with more that one data
# point inside, the length 1 coordinates and weights will be extended to the
# same length by repeating the single value given in the coordinates/weights
# concerned an appropriate number of times.
#
# @param c      tuple with coordinate values (each either scalar or vector)
# @param w      tuple with weights (either scalar or vector)
# @return       a tuple (coordinate(s), weight(s), extended as specified above
def __verifyAndExtendShape(c, w):
    maxlen = max(max(len(v) for v in c), len(w))
    minlen = min(min(len(v) for v in c), len(w))
    if 0 >= minlen or minlen > maxlen:
        raise Exception('Incompatible shapes.')
    if len(w) != maxlen and len(w) != 1:
        raise Exception('Incompatible shapes.')
    for v in c:
        if len(v) != maxlen and len(v) != 1:
            raise Exception('Incompatible shapes.')
    c += [w]
    c = list(v if len(v) == maxlen else maxlen * v for v in c)
    return c

## map number of dimensions to histogram type
__histoMap__ = {
        1: ROOT.TH1D,
        2: ROOT.TH2D,
        3: ROOT.TH3D
        }

## construct a histogram of the desired type
#
# @param tree   tree which is being drawn
# @param ndim   dimensionality of space drawn
# @param histoconstructorargs
#               histogram constructor arguments (optional)
# @param histotypehint
#               type (class) of histogram to br created
# @return histogram created
#
# If histotypehint is given, it takes precendence over the number of dimensions
# ndim seen during the drawing process. If histoconstructorargs is not given, a
# default histogram name, title and automatic axis scaling will be selected.
def __getHistoType(tree, ndim, histoconstructorargs = None, histotypehint =
        None):
    # try to figure out what kind of histogram we need
    if None == histotypehint:
        if ndim in __histoMap__:
            histotypehint = __histoMap__[ndim]
    if None == histotypehint:
        raise Exception('Do not know which type of histogram to use for %d '
                'dimensional data, please supply type hint!' % ndim)
    # if no constructor arguments have been supplied, synthesize a constructor
    # call fitting for THnD with automatic axis scaling
    if None == histoconstructorargs:
        hname = '%s_histo' % tree.GetName()
        htitle = 'histogram drawn from tree %s' % tree.GetTitle()
        histoconstructorargs = (hname, htitle) + ndim * (100, 0., 0.)
    return histotypehint(*histoconstructorargs)

## draw contents of a tree (GUITree)
#
# A simple example is given here: Suppose the tree has one (versioned) branch
# called 'occupancy', and branch called 'runnr', and we'd like to draw the
# occupancy as a function of runnr for the run range 100000-100010. The code to
# do this is:
#
# @code
# histo = Draw(tree, (
#       lambda t: t.runnr,              # run nr on x axis
#       lambda t: t.occupancy.value()   # occupancy on y axis
#       ),
#       ( lambda t: t.runnr >= 100000 and t.runnr <= 100010 )
#    )
# histo.Draw()
# @endcode
#
# The example above uses anonymous functions (defined by the lambda keyword),
# but it's just as valid to use normal named functions (as defined with
# "def fn(...):").
#
# @param tree   tree from which to draw
# @param coords tuple/list of coordinates to draw
# @param cuts   tuple/list of cuts to apply (optional)
# @param weight weight of data points draws (optional)
# @param histo  histogram into which to fill data (optional)
#               (you can pass anything which has a Fill method)
# @param histoconstructorargs
#               if a new histogram needs to be created, it's possible to supply
#               a list/tuple of arguments to pass to the constructor
# @param histotypehint
#               if a new histogram needs to be created, it's possible to supply
#               the type(class) of the desired histogram here (if the default
#               is insufficient)
#
# @return histogram filled by the method
def Draw(tree, coords, cuts = None, weight = None, histo = None,
        histoconstructorargs = None, histotypehint = None):
    if None == cuts: cuts = ( lambda t: True, )
    if None == weight: weight = lambda t: 1.
    ndim = None
    # loop over tree entries
    for dummy in tree:
        # figure out if entry passes cuts
        passed = True
        for c in cuts:
            if not c(tree):
                passed = False
                break
        if not passed: continue
        # yes, cuts passed, get what to draw
        c = list([c(tree)] for c in coords)
        if None == ndim:
            ndim = len(c)
            if None == histo:
                histo = __getHistoType(tree, ndim, histoconstructorargs,
                        histotypehint)
        else:
            if ndim != len(c):
                raise Exception("Shape change during plotting.")
        c = map(__flattenCoordinate, c)
        # verify shape of what the functors returned, extend individual
        # coordinates as needed to match
        c = __verifyAndExtendShape(c, __flattenCoordinate(weight(tree)))
        # fill the stuff into the histogram
        map(histo.Fill, *c)
    return histo

## get data points
def Data(tree, coords, cuts = None, weight = None):
    if None == cuts: cuts = ( lambda t: True, )
    if None == weight: weight = lambda t: 1.
    ndim = None
    result = []
    # loop over tree entries
    for dummy in tree:
        # figure out if entry passes cuts
        passed = True
        for c in cuts:
            if not c(tree):
                passed = False
                break
        if not passed: continue
        # yes, cuts passed, get what to draw
        c = list([c(tree)] for c in coords)
        if None == ndim:
            ndim = len(c)
        else:
            if ndim != len(c):
                raise Exception("Shape change during plotting.")
        c = map(__flattenCoordinate, c)
        # verify shape of what the functors returned, extend individual
        # coordinates as needed to match
        c = __verifyAndExtendShape(c, __flattenCoordinate(weight(tree)))
        # fill the stuff into the result
        result.append(c)
    return result
