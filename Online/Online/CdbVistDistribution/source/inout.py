# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'inputOutput_dialog.ui'

# Created: Tue Jun 19 14:45:48 2007
     # by: The PyQt User Interface Compiler (pyuic) 3.13

# WARNING! All changes made in this file will be lost!


from qt import *
from cdbVisCore import *	#Constants
from objectclasses import *	#data objects


inputOutput0_data = \
    "\x89\x50\x4e\x47\x0d\x0a\x1a\x0a\x00\x00\x00\x0d" \
    "\x49\x48\x44\x52\x00\x00\x00\x18\x00\x00\x00\x18" \
    "\x08\x06\x00\x00\x00\xe0\x77\x3d\xf8\x00\x00\x01" \
    "\xb7\x49\x44\x41\x54\x78\x9c\xed\x94\xbf\x4a\xc3" \
    "\x50\x14\xc6\xbf\x68\xd0\x68\x45\xc5\x41\x1f\xa1" \
    "\x55\x11\x1c\xc4\xc1\x42\x45\x1c\x2c\x28\xb6\x43" \
    "\x77\x17\xc1\xe9\x4e\xba\x88\xbe\x82\x20\x74\x10" \
    "\x9f\x21\x1d\x1c\x2c\xd4\x4a\x97\x62\x3b\xd4\xbe" \
    "\x42\x37\x05\x41\xa5\xd2\x36\x35\xb9\xd1\x1c\x87" \
    "\xd0\xb4\xc9\x6d\xa3\xed\xa2\x83\xdf\x74\xff\x70" \
    "\xbe\xdf\xfd\xce\x81\x0b\xfc\xeb\xb7\x25\x79\x0f" \
    "\x18\x63\x34\x88\x51\x32\x99\x14\xbc\x04\x31\xc6" \
    "\x48\x55\x55\xea\x57\xaa\xaa\x52\xaf\x87\xc9\xde" \
    "\x83\xdd\x58\x0c\xdc\x34\x71\x74\x78\xd8\x77\x0a" \
    "\xc6\x18\x79\x93\x08\x00\x5d\xe7\x38\x3d\x39\x46" \
    "\x24\x12\x41\x22\x91\xe8\x0b\x90\x4a\xa5\x00\xc0" \
    "\x05\x11\x00\xef\x86\x0e\xc0\x4e\x92\xcd\x66\x51" \
    "\x28\x16\xc1\x0d\xc3\xb9\xd7\x34\x4d\x30\x0e\x04" \
    "\x02\x08\x87\xc3\xd8\x8d\xc5\x90\xcf\xe7\x5d\x77" \
    "\x22\xe0\x5d\x77\x92\xdc\x15\x0a\xd8\xdb\x3f\x80" \
    "\x05\x19\x80\xdd\xe2\x51\x65\xcc\x59\xdb\x92\x40" \
    "\x9f\x1c\x17\xe7\x67\x88\xac\x6f\x08\x70\x01\x60" \
    "\x70\xd3\x49\xf2\x56\xad\xa2\x66\x0e\xa1\xda\x68" \
    "\x3a\x96\x44\x4d\xc1\x64\x66\x42\x41\xb3\xa9\x39" \
    "\xe9\x7d\x01\xf5\x5a\xc3\x95\xe4\xb9\xa6\xe3\xa9" \
    "\xda\x10\x0a\x3b\x65\x91\xe4\xaa\xf1\x05\x78\x93" \
    "\xd4\x34\x8e\xd7\x3a\xf7\x05\x8c\x8f\x70\x57\xcd" \
    "\x8f\x00\xad\x24\x2f\x2f\xd3\x78\x78\x1c\xf7\x05" \
    "\x28\x34\xe2\xaa\xf9\x11\x00\x00\x64\x59\x86\x04" \
    "\x0b\xf2\x30\x40\x64\x8f\x96\x2c\xef\x88\xbb\x7c" \
    "\x07\x7e\x80\x72\xa9\xd8\x7e\x99\x32\x86\xd9\x49" \
    "\x0e\x89\x24\x10\x01\x16\x01\xd2\xf0\xa8\x00\x98" \
    "\x9b\xfa\x44\x45\x51\xbe\x07\xec\x6c\x6f\xe2\x3a" \
    "\x9d\x73\xf6\xc1\xd0\x3c\xee\x6f\x2f\x61\x7e\xb4" \
    "\x7b\x6b\xe8\xe2\x20\x2b\x8a\x82\x60\x68\xe1\x7b" \
    "\xc0\x56\x34\x2e\xdd\x64\xae\xe8\x3a\x9d\x43\xb9" \
    "\x54\xc4\xca\xea\x1a\x16\x97\x96\xbb\x16\x76\x53" \
    "\x67\xfa\x96\xba\xb6\xaf\x05\x19\x44\x3b\xdb\x9b" \
    "\xd8\x8a\xc6\x1d\xdf\x9e\xf3\xb9\xc9\x5c\x0d\xf4" \
    "\x6d\x77\x9a\xff\xeb\x6f\xe8\x0b\xcf\x00\xeb\xc3" \
    "\xaf\xdc\xc1\x53\x00\x00\x00\x00\x49\x45\x4e\x44" \
    "\xae\x42\x60\x82"
inputOutput1_data = \
    "\x89\x50\x4e\x47\x0d\x0a\x1a\x0a\x00\x00\x00\x0d" \
    "\x49\x48\x44\x52\x00\x00\x00\x18\x00\x00\x00\x18" \
    "\x08\x06\x00\x00\x00\xe0\x77\x3d\xf8\x00\x00\x01" \
    "\xf6\x49\x44\x41\x54\x78\x9c\xed\x95\x4d\x6b\x13" \
    "\x51\x14\x86\x9f\x49\xe6\x23\xb6\xd3\x52\x2d\x22" \
    "\x48\xd1\x2e\x2a\x1a\x6d\xbb\x51\x4a\x68\xc6\x4d" \
    "\x55\x1a\x84\x82\x82\x0e\xf4\x47\x44\xc1\x85\x1b" \
    "\x45\xfc\x05\x82\xe6\x47\x14\x24\xe0\x88\xad\xcc" \
    "\x20\xe9\x46\xa9\xd5\x5a\x37\xc6\x84\xd0\x41\x2d" \
    "\x28\x58\x41\x5c\xa8\x88\x4e\xcc\xb8\x98\x66\xe8" \
    "\x35\x93\xea\xa4\xdd\x08\xbe\xab\x7b\x0f\x87\xf7" \
    "\x39\xef\x7c\xdc\x0b\xff\xba\xa4\xdf\x0b\x8e\x6d" \
    "\xf9\x1b\xf7\x93\xb9\x33\x2d\x3d\x71\x24\xb7\x00" \
    "\x6a\x7e\x54\x5f\xc7\x12\xa6\x73\x6c\xcb\x77\x6a" \
    "\x3e\x17\xa6\x4f\x01\x70\x6b\xe6\x01\x93\x07\xe3" \
    "\x07\xd8\x98\x3a\x4c\xe0\xd8\x96\x3f\x3b\x57\x42" \
    "\x19\x9a\x10\x9a\x67\xe7\x4a\xb1\x01\x8e\x6d\xf9" \
    "\x4d\x88\xdc\x2c\xac\xac\xbc\xc6\xc8\x66\x78\xb2" \
    "\x06\x5e\xbd\x11\x36\x9f\x37\xcf\x85\xeb\x1d\xdd" \
    "\xbb\xfe\x68\xfe\xec\xe9\x82\x30\x54\x00\xa8\xf9" \
    "\xc0\x20\xaf\xd6\x60\xf4\xc0\x00\x1f\x3f\x7f\x07" \
    "\x82\xb5\xf5\xfc\x6d\xd8\xec\xb9\xc5\x4d\xcd\x0f" \
    "\xa5\x8f\x30\x96\xc9\x52\x79\x59\x16\x01\x00\x47" \
    "\xd3\xfb\xc9\x65\x0e\xe3\x35\x7c\xbe\x7c\xab\x03" \
    "\x90\x1b\x1f\x66\xca\x18\xc1\x5e\xac\xb0\x5c\x5d" \
    "\x25\x7d\xe2\x6a\x5b\xf3\xbd\xfd\x2a\xa5\xdb\xd7" \
    "\x18\xcb\x64\x85\xba\x0c\x70\xe3\xe2\x59\xe9\xd2" \
    "\xcd\x3b\x7e\x42\x82\xd3\xe3\xc3\xe1\x9b\x4f\x29" \
    "\x49\xee\x2f\x94\x59\xae\xae\x82\x7c\x0c\xf7\xdd" \
    "\x8f\x4d\x13\x44\x29\x4c\xe0\xb9\xf3\x2c\x31\x41" \
    "\x32\x21\x31\x65\x8c\x00\x70\xef\xd1\x0b\x96\x2a" \
    "\x81\x79\xa7\x12\xfe\x83\xe9\xe3\x43\xcc\x3c\x74" \
    "\x91\xa4\x20\xc3\x62\xf9\xcd\x96\xcc\x5b\x00\x3d" \
    "\xbd\x3a\x9e\x3b\xcf\x63\x82\x4f\x75\x60\x70\x1f" \
    "\xf0\xe1\xaf\x8c\x76\xf7\xeb\x54\xd7\x3d\xda\x02" \
    "\x54\x25\xd8\x9e\xcc\x8e\xf2\xfe\xd3\xd7\x58\x93" \
    "\x76\xa7\x64\xc1\x23\x12\xa0\x69\x2a\x00\x7d\xba" \
    "\x46\xbd\xd1\x20\x8e\xfa\x74\x4d\xf0\x88\x06\xa8" \
    "\x5a\xd8\x9c\x4c\xc4\x3b\x22\x7a\xba\x54\xc1\x23" \
    "\x12\x90\x5a\xa7\xef\xe9\xd5\xd9\xd9\xf5\x33\x16" \
    "\x40\x95\x93\x82\x47\x24\x40\x55\x15\x0c\xc3\xe0" \
    "\xfa\x95\xcb\xb1\xcc\x9b\x32\x0c\x03\x55\x55\xda" \
    "\x03\xee\x5a\x16\xa6\x69\x62\x9a\x66\x47\x00\x80" \
    "\x62\x51\x3c\x4e\x84\x07\x9d\xcf\xe7\xb7\xe5\x32" \
    "\x28\x14\x0a\x5b\xba\xa4\xfe\x6b\x7b\xf5\x0b\x3d" \
    "\xae\x86\xf4\xac\x24\x5e\xd9\x00\x00\x00\x00\x49" \
    "\x45\x4e\x44\xae\x42\x60\x82"
inputOutput2_data = \
    "\x89\x50\x4e\x47\x0d\x0a\x1a\x0a\x00\x00\x00\x0d" \
    "\x49\x48\x44\x52\x00\x00\x00\x18\x00\x00\x00\x18" \
    "\x08\x06\x00\x00\x00\xe0\x77\x3d\xf8\x00\x00\x01" \
    "\xf7\x49\x44\x41\x54\x78\x9c\xed\x95\x41\x6b\x13" \
    "\x41\x18\x86\x9f\xc4\x34\x87\x82\x20\x78\x10\x44" \
    "\x68\x8b\x86\xb6\x49\x15\x41\x6c\xb6\x24\xa7\x14" \
    "\x9a\x42\x22\x26\xfd\x1f\x4b\xa1\x07\x2f\x82\xf4" \
    "\xe2\xc5\x43\xa0\xe4\x47\x08\xd9\x43\x7b\x50\xb0" \
    "\x87\x5e\xd2\xb5\x8a\xa6\x0a\xa5\x5a\xa4\x3d\x34" \
    "\x12\x0f\x9a\x20\x31\x4d\x93\xee\x66\x93\xf1\x50" \
    "\x09\x99\x66\x8d\xdd\xa6\xa0\x07\xdf\xd3\xce\xc7" \
    "\xce\xf3\x7c\x33\x30\x33\xf0\x3f\x7f\x3b\xae\xce" \
    "\x81\xaa\xaa\xe2\x3c\xa0\xe9\x74\xda\xd5\x55\x54" \
    "\x55\x55\x68\x9a\x26\xfa\x8d\xa6\x69\xa2\xb3\x51" \
    "\x4f\xa7\xe4\x7e\x22\x41\x26\x93\x41\xd7\xf5\x33" \
    "\x75\x1e\x0e\x87\x49\xce\xcd\x91\xcd\x66\xdb\x35" \
    "\x49\x60\x9a\x0d\x74\x5d\x67\xf1\xf1\x13\x4c\xab" \
    "\xe9\x08\xee\xf5\x5c\x60\xf1\xe1\x03\x62\xf1\x7b" \
    "\x52\x5d\x12\x1c\x19\x26\x00\x5f\x2b\x55\x0e\x6a" \
    "\xa6\x23\xc1\xc5\x41\xaf\xc4\xb0\x15\x18\xa6\x01" \
    "\x40\xb9\x6a\x50\xaa\xd4\x1d\x09\x9a\x2d\x21\x31" \
    "\xec\x05\xbf\xec\xe5\xaa\x41\xe9\x87\x33\x81\xc7" \
    "\xed\x96\x18\xb6\x02\xb3\x61\x01\x70\x78\x64\x51" \
    "\xae\x35\x4e\x0d\x2f\xec\x7f\x66\xfb\x04\xc3\x56" \
    "\x70\x50\xa9\x02\x50\x2c\x5d\xa2\xf0\x65\xf0\x74" \
    "\x74\x2b\xc7\xd4\xcd\x11\x00\x5e\x11\x69\x33\x6c" \
    "\x05\x8e\x63\xe5\x50\x26\x86\x89\x87\x26\x00\x10" \
    "\x42\xf0\x74\x7d\xef\x9c\x04\x56\x8e\xbb\xfe\x21" \
    "\xa2\x4a\x80\x4a\xfd\x78\x3b\xa3\x4a\x80\x66\x4b" \
    "\xf0\x96\x08\x90\xfe\xbd\xe0\xea\x65\x6f\x4f\xf6" \
    "\x5e\x7e\x83\x3b\xe3\x43\xcc\x04\xfd\xd4\x1b\x4d" \
    "\xaa\xf5\xe3\x7d\x17\xc0\x4c\xd0\x4f\x4b\xc0\xc2" \
    "\xd2\xb2\x48\xcd\x27\x5d\x5d\x82\xb1\xf1\x00\x6b" \
    "\x99\x47\x3d\x05\x03\x37\x22\x6c\xee\xe4\xd9\xdc" \
    "\xc9\x73\xcb\x77\x8d\xb1\xeb\xc3\x00\xe8\xef\xf7" \
    "\xd9\xda\x2d\x48\xff\x4a\x82\xdc\x9b\x0d\x26\x95" \
    "\x10\x93\x4a\xa8\xa7\xa0\x7e\xf8\xbd\xfd\xbd\xf2" \
    "\xae\x40\xe8\xf6\x28\x00\x5b\xbb\x05\x82\x57\x6a" \
    "\x14\x8b\xdf\xf0\xf9\x46\x48\x75\x0a\xe2\xb1\x69" \
    "\x9e\x3d\x5f\xe3\xe3\x87\xed\x2e\xe0\x9f\x56\x33" \
    "\xe0\x71\xb7\xc7\xfa\xcb\xd7\xc4\x63\xd3\x44\x67" \
    "\x13\x2e\x69\x05\xd1\xd9\x84\x6b\xf5\xc5\x8a\xe3" \
    "\xeb\x7a\xf5\x93\x3c\xa5\x13\x0e\x27\xde\x83\xb3" \
    "\x64\x61\x69\x59\x32\xa4\xe6\x93\x12\xb3\xbf\x73" \
    "\x00\x44\x47\xe5\x1e\x53\xfd\x02\xff\xb9\xfc\x04" \
    "\xd0\xa7\x00\x36\x08\xec\x11\x60\x00\x00\x00\x00" \
    "\x49\x45\x4e\x44\xae\x42\x60\x82"
visible_data = \
    "\x89\x50\x4e\x47\x0d\x0a\x1a\x0a\x00\x00\x00\x0d" \
    "\x49\x48\x44\x52\x00\x00\x00\x10\x00\x00\x00\x10" \
    "\x08\x06\x00\x00\x00\x1f\xf3\xff\x61\x00\x00\x00" \
    "\x96\x49\x44\x41\x54\x78\x9c\x63\x60\x18\x68\xc0" \
    "\x88\xcc\x39\xbd\x21\xe8\x3f\x21\x0d\xa6\x01\xeb" \
    "\x50\xf4\xc0\x39\xa7\x37\x04\xfd\xe7\x17\x90\x63" \
    "\x10\x16\x51\xc2\xa9\xf9\xed\x9b\x7b\x0c\x1f\x3f" \
    "\x3c\x42\x31\x84\x05\x59\x01\x4c\x33\x4c\x21\x0c" \
    "\xc0\x0c\x16\x16\x51\x42\x11\xc7\x30\x00\x06\x3e" \
    "\x7e\x78\xc4\xf0\x86\x35\xb2\x11\x2e\xf0\x61\x79" \
    "\x3d\x2e\x97\x61\x35\x80\x81\x81\x81\xc1\xd3\x3b" \
    "\xb4\x01\xc6\x3e\xbd\x21\xa8\x1e\x97\x3a\xac\x06" \
    "\xf0\x0b\xc8\xa1\x04\x28\xbf\x80\x1c\x2e\xfd\xd8" \
    "\x0d\x80\xf9\x97\x18\xc0\x44\x94\xaa\xe1\x6d\x00" \
    "\x4a\x20\xde\xbb\x73\x80\x7c\x03\x50\x12\x0e\x41" \
    "\xb0\x8e\x64\x8b\x70\x02\x00\x78\x5f\x2a\xe9\x56" \
    "\xf2\x43\xfc\x00\x00\x00\x00\x49\x45\x4e\x44\xae" \
    "\x42\x60\x82"
invisible_data = \
    "\x89\x50\x4e\x47\x0d\x0a\x1a\x0a\x00\x00\x00\x0d" \
    "\x49\x48\x44\x52\x00\x00\x00\x10\x00\x00\x00\x10" \
    "\x08\x06\x00\x00\x00\x1f\xf3\xff\x61\x00\x00\x00" \
    "\x97\x49\x44\x41\x54\x78\x9c\x63\x60\x18\x68\xc0" \
    "\x88\xcc\x79\x75\xa9\xf6\x3f\x21\x0d\x62\x7a\xcd" \
    "\x28\x7a\xe0\x9c\x57\x97\x6a\xff\xb3\x70\x2a\x30" \
    "\xb0\xf1\xe9\xe1\xd4\xfc\xeb\xd3\x25\x86\x3f\xdf" \
    "\x1f\xa0\x18\xc2\x82\xac\x00\xa6\x19\xa6\x10\xae" \
    "\x08\x6a\x30\x1b\x9f\x1e\x8a\x38\x86\x01\x30\xf0" \
    "\xe7\xfb\x03\x86\xd7\x8c\x3e\x8d\x30\xbe\xe8\xf7" \
    "\x2d\xf5\xb8\x5c\x86\xd5\x00\x06\x06\x06\x06\x6d" \
    "\x5d\xf3\x06\x18\xfb\xd5\xa5\xda\x7a\x5c\xea\xb0" \
    "\x1a\xc0\xc2\xa9\x80\x12\xa0\x2c\x9c\x0a\xb8\xf4" \
    "\x63\x37\x00\xe6\x5f\x62\x00\x13\x51\xaa\x86\xb7" \
    "\x01\x28\x81\xf8\xed\xe5\x26\xf2\x0d\x40\x4e\x38" \
    "\x84\x41\x33\xc9\x16\xe1\x04\x00\x9f\x05\x2b\x35" \
    "\x07\xd6\xdf\x91\x00\x00\x00\x00\x49\x45\x4e\x44" \
    "\xae\x42\x60\x82"
class Input_output(QDialog):
    def __init__(self, parent = None, callback = None):
        """ Constructor.

	        Parameters:
                @parent - the parent window (usually MainWindow)
                @cdb    - the confDB instance that should be used (reference)
                @pos    - position of the path window
                @style  - window style
                @callback - callable object method that should
                  	    be called if a path is selected. 
                            callback must have 3 parameters: (self, dictionary deviceInfo, integer pathNr)
        """
        QDialog.__init__(self,parent,"Input/Output",0,0)  # Args = parent, name, modal, flags

        self.setMinimumSize(QSize(200,0))
        self.setFocusPolicy(QDialog.TabFocus)
        self.setModal(0)


        self.image0 = QPixmap()
        self.image0.loadFromData(inputOutput0_data,"PNG")
        self.image1 = QPixmap()
        self.image1.loadFromData(inputOutput1_data,"PNG")
        self.image2 = QPixmap()
        self.image2.loadFromData(inputOutput2_data,"PNG")
	self.visible = QPixmap()
        self.visible.loadFromData(visible_data,"PNG")
        self.invisible = QPixmap()
        self.invisible.loadFromData(invisible_data,"PNG")
	
        self.setMinimumSize(QSize(200,0))
        self.setIcon(self.image0)
        self.setFocusPolicy(QDialog.TabFocus)

        Input_outputLayout = QGridLayout(self,1,1,2,6,"Input_outputLayout")

        self.splitter15 = QSplitter(self,"splitter15")
        self.splitter15.setOrientation(QSplitter.Vertical)
        self.splitter15.setOpaqueResize(1)
        self.splitter15.setChildrenCollapsible(1)

        LayoutWidget = QWidget(self.splitter15,"layout13")
        layout13 = QVBoxLayout(LayoutWidget,11,6,"layout13")
        spacer2 = QSpacerItem(20,3,QSizePolicy.Minimum,QSizePolicy.Fixed)
        layout13.addItem(spacer2)

        layout12 = QHBoxLayout(None,0,6,"layout12")

        self.textLabel6 = QLabel(LayoutWidget,"textLabel6")
        self.textLabel6.setMinimumSize(QSize(0,0))
        self.textLabel6.setAlignment(QLabel.WordBreak | QLabel.AlignVCenter)
        layout12.addWidget(self.textLabel6)
        spacer4 = QSpacerItem(0,20,QSizePolicy.Expanding,QSizePolicy.Minimum)
        layout12.addItem(spacer4)

        self.textLabel8 = QLabel(LayoutWidget,"textLabel8")
        self.textLabel8.setMinimumSize(QSize(20,0))
        self.textLabel8.setPixmap(self.image1)
        layout12.addWidget(self.textLabel8)
        layout13.addLayout(layout12)

        self.inputDevices = QListView(LayoutWidget,"Input")
	self.inputDevices.addColumn("Intput devices")
        self.inputDevices.setMinimumSize(QSize(0,20))
        layout13.addWidget(self.inputDevices)

        LayoutWidget_2 = QWidget(self.splitter15,"layout17")
        layout17 = QVBoxLayout(LayoutWidget_2,11,6,"layout17")
        spacer7 = QSpacerItem(20,3,QSizePolicy.Minimum,QSizePolicy.Fixed)
        layout17.addItem(spacer7)

        layout16 = QHBoxLayout(None,0,6,"layout16")

        self.textLabel9 = QLabel(LayoutWidget_2,"textLabel9")
        layout16.addWidget(self.textLabel9)
        spacer8 = QSpacerItem(41,20,QSizePolicy.Expanding,QSizePolicy.Minimum)
        layout16.addItem(spacer8)

        self.textLabel10 = QLabel(LayoutWidget_2,"textLabel10")
        self.textLabel10.setMinimumSize(QSize(20,0))
        self.textLabel10.setPixmap(self.image2)
        layout16.addWidget(self.textLabel10)
        layout17.addLayout(layout16)

        self.outputDevices = QListView(LayoutWidget_2,"outputDevices")
	self.outputDevices.addColumn("Output devices")
        self.outputDevices.setMinimumSize(QSize(0,20))
        layout17.addWidget(self.outputDevices)

        Input_outputLayout.addWidget(self.splitter15,0,0)

        self.resize(QSize(200,376).expandedTo(self.minimumSizeHint()))
        self.clearWState(Qt.WState_Polished)
	
        self.setCaption(self.__tr("Input / Output devices"))
        self.textLabel6.setText(self.__tr("Input devices"))
	self.textLabel9.setText(self.__tr("Output devices"))
	#NON GUI PART
	self.connect(self.inputDevices,SIGNAL("doubleClicked(QListViewItem*)"),self.OnInputChoice)
	self.connect(self.outputDevices,SIGNAL("doubleClicked(QListViewItem*)"),self.OnOutputChoice)
	self.visWindow = callback
	self.inputLinks = []
	self.outputLinks = []
	self.origShape = None

	#self.main = parent
    def insertInput(self,item,index):
	if self.inputDevices.lastItem() and item==self.inputDevices.lastItem().text(0):
		elt = QListViewItem(self.inputDevices.lastItem(),item,str(index))
	else:
		elt = QListViewItem(self.inputDevices,item,str(index))
	elt.setPixmap(0,self.invisible)
	#self.inputDevices.insertItem(QListViewItem(self.inputDevices,item))

    def insertOutput(self,item,index):
	elt = QListViewItem(self.outputDevices,item,str(index))
	elt.setPixmap(0,self.invisible)
	#self.outputDevices.insertItem(QListViewItem(self.outputDevices,item))

    def clear(self):
	self.outputDevices.clear()
	self.inputDevices.clear()
	self.inputLinks = []
	self.outputLinks = []

    def setData(self,input,output,origShape):
	self.inputLinks = input
	self.outputLinks = output
	self.origShape = origShape

    def OnInputChoice(self,item):
	index = int(str(item.text(1)))
	link = self.inputLinks[index]
	##self.visWindow.addInputDevice(link,self.origShape)
	# Add each device in input links of the current device to the visual window, both the node and the link


	deviceobj = self.origShape.GetObject()
	
	tmp_link = link.GetObjectInfo(True)
	if self.visWindow.FindByName(tmp_link[DEV_SWITCH_FROM]):
		print "element deja present "
		return
	tmp_devobj = Device(self.visWindow.main.GetActiveSystem(),tmp_link[DEV_SWITCH_FROM],False)
	#shape,newNodes = self.AddNode(tmp_devobj,position,-1,False,len(inputLinks),node_pos.x,node_pos.y,refresh=False) #-1 from parentnode (y layer -1)
	index,portnr = self.origShape.FindCorrespondingPortIndex(link.GetPortTo(),link.GetPortTypeTo(),port_IN)
	port_pos = self.origShape.GetPortPosition(index,port_IN)
	newshape,newNodes = self.visWindow.AddNode(tmp_devobj,index,30,True)
	self.visWindow.CreateConnectedLink(link)
	
	self.visWindow.canvas.update()
	
    def OnOutputChoice(self,item):
	index = int(str(item.text(1)))
	link = self.outputLinks[index]
	##self.visWindow.addOutputDevice(link,self.origShape)
	
	deviceobj = self.origShape.GetObject()
	
	tmp_link = link.GetObjectInfo(True)
	if self.visWindow.FindByName(tmp_link[DEV_SWITCH_TO]):
		print "element deja present "
		return
	tmp_devobj = Device(self.visWindow.main.GetActiveSystem(),tmp_link[DEV_SWITCH_TO],False)
	#shape,newNodes = self.AddNode(tmp_devobj,position,-1,False,len(inputLinks),node_pos.x,node_pos.y,refresh=False) #-1 from parentnode (y layer -1)
	index,portnr = self.origShape.FindCorrespondingPortIndex(link.GetPortTo(),link.GetPortTypeTo(),port_OUT)
	port_pos = self.origShape.GetPortPosition(index,port_OUT)
	newshape,newNodes = self.visWindow.AddNode(tmp_devobj,index,30,False)
	self.visWindow.CreateConnectedLink(link)
	
	self.visWindow.canvas.update()

	
    def __tr(self,s,c = None):
        return qApp.translate("Input_output",s,c)

# class Input_output(QDialog):
    # def __init__(self,viswindow, parent = None,name = None,modal = 0,fl = 0):
       
        # QDialog.__init__(self,parent,"Input_Output",0,0)  # Args = parent, name, modal, flags

        # self.setMinimumSize(QSize(200,0))
        # self.setModal(0)	
	
        # self.image0 = QPixmap()
        # self.image0.loadFromData(inputOutput0_data,"PNG")
        # self.image1 = QPixmap()
        # self.image1.loadFromData(inputOutput1_data,"PNG")
        # self.image2 = QPixmap()
        # self.image2.loadFromData(inputOutput2_data,"PNG")
	
        # self.setMinimumSize(QSize(200,0))
        # self.setIcon(self.image0)
        # self.setFocusPolicy(QDialog.TabFocus)

        # Input_outputLayout = QGridLayout(self,1,1,2,6,"Input_outputLayout")

        # self.splitter15 = QSplitter(self,"splitter15")
        # self.splitter15.setOrientation(QSplitter.Vertical)
        # self.splitter15.setOpaqueResize(1)
        # self.splitter15.setChildrenCollapsible(1)

        # LayoutWidget = QWidget(self.splitter15,"layout13")
        # layout13 = QVBoxLayout(LayoutWidget,11,6,"layout13")
        # spacer2 = QSpacerItem(20,3,QSizePolicy.Minimum,QSizePolicy.Fixed)
        # layout13.addItem(spacer2)

        # layout12 = QHBoxLayout(None,0,6,"layout12")

        # self.textLabel6 = QLabel(LayoutWidget,"textLabel6")
        # self.textLabel6.setMinimumSize(QSize(0,0))
        # self.textLabel6.setAlignment(QLabel.WordBreak | QLabel.AlignVCenter)
        # layout12.addWidget(self.textLabel6)
        # spacer4 = QSpacerItem(0,20,QSizePolicy.Expanding,QSizePolicy.Minimum)
        # layout12.addItem(spacer4)

        # self.textLabel8 = QLabel(LayoutWidget,"textLabel8")
        # self.textLabel8.setMinimumSize(QSize(20,0))
        # self.textLabel8.setPixmap(self.image1)
        # layout12.addWidget(self.textLabel8)
        # layout13.addLayout(layout12)

        # self.inputDevices = QListBox(LayoutWidget,"inputDevices")
        # self.inputDevices.setMinimumSize(QSize(0,20))
        # layout13.addWidget(self.inputDevices)

        # LayoutWidget_2 = QWidget(self.splitter15,"layout17")
        # layout17 = QVBoxLayout(LayoutWidget_2,11,6,"layout17")
        # spacer7 = QSpacerItem(20,3,QSizePolicy.Minimum,QSizePolicy.Fixed)
        # layout17.addItem(spacer7)

        # layout16 = QHBoxLayout(None,0,6,"layout16")

        # self.textLabel9 = QLabel(LayoutWidget_2,"textLabel9")
        # layout16.addWidget(self.textLabel9)
        # spacer8 = QSpacerItem(41,20,QSizePolicy.Expanding,QSizePolicy.Minimum)
        # layout16.addItem(spacer8)

        # self.textLabel10 = QLabel(LayoutWidget_2,"textLabel10")
        # self.textLabel10.setMinimumSize(QSize(20,0))
        # self.textLabel10.setPixmap(self.image2)
        # layout16.addWidget(self.textLabel10)
        # layout17.addLayout(layout16)

        # self.outputDevices = QListBox(LayoutWidget_2,"outputDevices")
        # self.outputDevices.setMinimumSize(QSize(0,20))
        # layout17.addWidget(self.outputDevices)

        # Input_outputLayout.addWidget(self.splitter15,0,0)

        # self.languageChange()

        # self.resize(QSize(200,376).expandedTo(self.minimumSizeHint()))
        # self.clearWState(Qt.WState_Polished)


    # def languageChange(self):
        # self.setCaption(self.__tr("Input / Output devices"))
        # self.textLabel6.setText(self.__tr("Input devices"))
	# self.textLabel9.setText(self.__tr("Output devices"))
        # self.textLabel8.setText(QString.null)
        # self.inputDevices.clear()
        # self.inputDevices.insertItem(self.__tr("New Item"))
        # self.inputDevices.insertItem(self.__tr("New Item"))
        # self.inputDevices.insertItem(self.__tr("New Item"))
        # self.inputDevices.insertItem(self.__tr("New Item"))
        # self.inputDevices.insertItem(self.__tr("New Item"))
        # self.inputDevices.insertItem(self.__tr("New Item"))
        # self.inputDevices.insertItem(self.__tr("New Item"))
        # self.inputDevices.insertItem(self.__tr("New Item"))
        # self.inputDevices.insertItem(self.__tr("New Item"))
        # self.inputDevices.insertItem(self.__tr("New Item"))
        # self.inputDevices.insertItem(self.__tr("New Item"))
        # self.inputDevices.insertItem(self.__tr("New Item"))
        # self.textLabel9.setText(self.__tr("Output devices"))
        # self.textLabel10.setText(QString.null)
        # self.outputDevices.clear()
        # self.outputDevices.insertItem(self.__tr("New Item"))
        # self.outputDevices.insertItem(self.__tr("New Item"))
        # self.outputDevices.insertItem(self.__tr("New Item"))
        # self.outputDevices.insertItem(self.__tr("New Item"))
        # self.outputDevices.insertItem(self.__tr("New Item"))
        # self.outputDevices.insertItem(self.__tr("New Item"))
        # self.outputDevices.insertItem(self.__tr("New Item"))
        # self.outputDevices.insertItem(self.__tr("New Item"))
        # self.outputDevices.insertItem(self.__tr("New Item"))
        # self.outputDevices.insertItem(self.__tr("New Item"))
        # self.outputDevices.insertItem(self.__tr("New Item"))
        # self.outputDevices.insertItem(self.__tr("New Item"))


    # def __tr(self,s,c = None):
        # return qApp.translate("Input_output",s,c)
