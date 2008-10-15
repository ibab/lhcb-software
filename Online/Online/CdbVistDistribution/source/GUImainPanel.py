# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'mainpanel.ui'
#
# Created: Thu Apr 26 10:25:27 2007
#      by: The PyQt User Interface Compiler (pyuic) 3.13
#
# WARNING! All changes made in this file will be lost!


from qt import *
from selectPanel import *
from visWindow import *


class GUImainPanel(QSplitter):
    def __init__(self,main=None,parent = None,name = None,modal = 0,fl = 0):
        QSplitter.__init__(self,parent,name)

        if not name:
            self.setName("Form1")
	self.main=main
	
# 	self.splitter = QSplitter(parent,"my Splitter")
	self.setOrientation(QSplitter.Horizontal)
	#self.setGeometry(0,0,800,602)
	
 	self.selectWindow = selectWindow(self.main,parent=self)
	self.selectWindow.setMinimumSize(QSize(250,0))
	
	canvas = QCanvas(self)
	self.visWindow = visWindow(self.main,canvas,parent=self)
	self.visWindow.setPaletteBackgroundColor(QColor(255,255,255))
	self.visWindow.setMinimumSize(QSize(450,0))
	self.visWindow.setSizePolicy(QSizePolicy(7,7,1,0,self.visWindow.sizePolicy().hasHeightForWidth()))
        
	self.languageChange()
        self.clearWState(Qt.WState_Polished)


    def languageChange(self):
        self.setCaption(self.__tr("mainPanel"))
#         self.comboBox.clear()
#         self.comboBox.insertItem(self.__tr("All"))
#         self.textLabel1.setText(self.__tr("Select by blabla"))


    def __tr(self,s,c = None):
        return qApp.translate("mainPanel",s,c)
