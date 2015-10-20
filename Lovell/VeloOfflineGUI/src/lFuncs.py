from PyQt4.QtCore import *
from PyQt4.QtGui import *

def setPadding(grid_layout):
    p = 4
    grid_layout.setHorizontalSpacing(p)
    grid_layout.setVerticalSpacing(p)
    grid_layout.setContentsMargins(p, p, p, p)