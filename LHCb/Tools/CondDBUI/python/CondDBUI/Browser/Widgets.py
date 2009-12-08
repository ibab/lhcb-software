from PyQt4 import QtGui, QtCore
from PyQt4.QtCore import (QObject, SIGNAL, SLOT,
                          QDateTime, QDate, QTime,
                          Qt)
from PyQt4.QtGui import QSizePolicy

from Utils import valKeyToDateTime, dateTimeToValKey

from PyCool import cool

__all__ = ["TimePointEdit", "SearchableTextEdit"]


class TimePointEdit(QtGui.QWidget):
    __pyqtSignals__ = ("timeChanged(QTime)",
                       "dateChanged(QDate)",
                       "dateTimeChanged(QDateTime)",
                       "validityKeyChange(cool::ValidityKey)",
                       "maxChanged(bool)",
                       "maxChecked()"
                       "maxUnchecked()")
    
    def __init__(self, parent = None):
        super(TimePointEdit,self).__init__(parent)
        
        self._layout = QtGui.QHBoxLayout(self)
        self._layout.setObjectName("layout")
        self._layout.setContentsMargins(0,0,0,0)
        
        self._edit = QtGui.QDateTimeEdit(self)
        self._edit.setObjectName("edit")
        self._edit.setTimeSpec(Qt.UTC)
        self._edit.setContextMenuPolicy(Qt.NoContextMenu)
        
        # Set the time range from cool ValidityKeyMin/Max.
        self._minDateTime = valKeyToDateTime(cool.ValidityKeyMin)
        self._maxDateTime = valKeyToDateTime(cool.ValidityKeyMax)
        self._edit.setDateTimeRange(self._minDateTime, self._maxDateTime)
        self._edit.setDisplayFormat("dd-MM-yyyy hh:mm:ss")
        self._edit.setCalendarPopup(True)
        self._edit.setSizePolicy(QtGui.QSizePolicy.MinimumExpanding,
                                 QtGui.QSizePolicy.Minimum)
        self._layout.addWidget(self._edit)
        
        self._utc = QtGui.QCheckBox(self)
        self._utc.setObjectName("utc")
        self._utc.setText("UTC")
        self._utc.setChecked(True)
        self._utc.setSizePolicy(QtGui.QSizePolicy.Minimum,
                                QtGui.QSizePolicy.Minimum)
        self._layout.addWidget(self._utc)
        
        self._max = QtGui.QCheckBox(self)
        self._max.setObjectName("max")
        self._max.setText("Max")
        self._max.setChecked(False)
        self._max.setSizePolicy(QtGui.QSizePolicy.Minimum,
                                QtGui.QSizePolicy.Minimum)
        self._layout.addWidget(self._max)
        
        self.actionSet_to_minimum = QtGui.QAction(self)
        self.actionSet_to_minimum.setObjectName("actionSet_to_minimum")
        self.actionSet_to_minimum.setText("Set to &minimum")
        self.addAction(self.actionSet_to_minimum)
        QObject.connect(self.actionSet_to_minimum, SIGNAL("triggered()"),
                        self.setToMinimum)
        
        self.actionSet_to_now = QtGui.QAction(self)
        self.actionSet_to_now.setObjectName("actionSet_to_now")
        self.actionSet_to_now.setText("Set to &now")
        self.addAction(self.actionSet_to_now)
        QObject.connect(self.actionSet_to_now, SIGNAL("triggered()"),
                        self.setToNow)
        
        self.actionSet_to_maximum = QtGui.QAction(self)
        self.actionSet_to_maximum.setObjectName("actionSet_to_maximum")
        self.actionSet_to_maximum.setText("Set to ma&ximum")
        self.addAction(self.actionSet_to_maximum)
        QObject.connect(self.actionSet_to_maximum, SIGNAL("triggered()"),
                        self.setToMaximum)
        
        self.setContextMenuPolicy(Qt.ActionsContextMenu)
        
        QtCore.QMetaObject.connectSlotsByName(self)
        
        # propagate edit field signals:
        for signal in ["timeChanged(QTime)",
                       "dateChanged(QDate)",
                       "dateTimeChanged(QDateTime)"]:
            QObject.connect(self._edit, SIGNAL(signal), self, SIGNAL(signal))
        QObject.connect(self._edit, SIGNAL("dateTimeChanged(QDateTime)"),
                        self.emitValidityKeyChange)
    
    ## Value of the property minDateTime.
    def minimumDateTime(self):
        return self._edit.minimumDateTime()
    
    ## Value of the property maxDateTime.
    def maximumDateTime(self):
        return self._edit.maximumDateTime()
    
    ## Set the property minDateTime.
    def setMinimumDateTime(self, dateTime):
        if dateTime >= self._minDateTime:
            self._edit.setMinimumDateTime(dateTime)
        else:
            self._edit.setMinimumDateTime(self._minDateTime)
    
    ## Set the property maxDateTime.
    def setMaximumDateTime(self, dateTime):
        if dateTime <= self._maxDateTime:
            self._edit.setMaximumDateTime(dateTime)
        else:
            self._edit.setMaximumDateTime(self._maxDateTime)
    
    ## Reset the property minDateTime to its default value.
    def resetMinimumDateTime(self):
        self._edit.setMinimumDateTime(self._minDateTime)
    
    ## Reset the property maxDateTime to its default value.
    def resetMaximumDateTime(self):
        self._edit.setMaximumDateTime(self._maxDateTime)
    
    ## Value of the property maxEnabled.
    def maxEnabled(self):
        return self._max.isEnabled()
    
    ## Set the property maxEnabled.
    def setMaxEnabled(self, enable):
        self._max.setEnabled(enable)
    
    ## Value of the property maxChecked.
    #  Indicates if the "Max" checkbox is checked or not.
    def maxChecked(self):
        return self._max.isChecked()
    
    ## Set the status of the "Max" checkbox..
    def setMaxChecked(self, checked):
        self._max.setChecked(checked)
    
    ## Value of the property dateTime.
    def dateTime(self):
        return self._edit.dateTime()
    
    ## Set the value of the property dateTime.
    def setDateTime(self, dateTime):
        self._edit.setDateTime(dateTime)
    
    ## Format used to display the QDateTime value.
    #  @see QDateTimeEdit::displayFormat()
    def displayFormat(self):
        return self._edit.displayFormat()
    
    ## Set the format used to display the QDateTime value.
    #  @see QDateTimeEdit::setDisplayFormat()
    def setDisplayFormat(self, format):
        self._edit.setDisplayFormat(format)
    
    ## Slot to update internal state when the "UTC" checkbox is toggled.
    def on_utc_toggled(self, checked):
        if checked:
            spec = Qt.UTC
        else:
            spec = Qt.LocalTime
        self._edit.setTimeSpec(spec)
        # FIXME: This seems to be a bit stupid, but it is the only way I found
        # to trigger a correct re-paint of the displayed date-time.
        self._edit.setDateTime(self._edit.dateTime())
    
    ## Slot to update internal state when the "Max" checkbox is toggled.
    def on_max_toggled(self, checked):
        self._edit.setEnabled(not checked)
        self._utc.setEnabled(not checked)
        self.emit(SIGNAL("maxChanged(bool)"), checked)
        if checked:
            self._edit.setDateTime(self._maxDateTime)
            self.emit(SIGNAL("maxChecked()"))
        else:
            self.emit(SIGNAL("maxUnchecked()"))
            self.emit(SIGNAL("dateTimeChanged(QDateTime)"),self._edit.dateTime())

    ## Convert the internal QDateTime to a cool::ValidityKey.
    def toValidityKey(self):
        # Get the number of seconds since epoch and convert it to ns.
        if self._max.isChecked():
            return cool.ValidityKeyMax
        return dateTimeToValKey(self._edit.dateTime())

    ## Set the internal QDateTime from a cool::ValidityKey.
    def setValidityKey(self, valKey):
        if valKey == cool.ValidityKeyMax:
            self._edit.setDateTime(self._maxDateTime)
            self._max.setChecked(True)
        else:
            self._edit.setDateTime(valKeyToDateTime(valKey))
            self._max.setChecked(False)
    
    ## Slot called by a "dateTimeChanged" signal to propagate it as a
    #  cool::ValidityKey.
    def emitValidityKeyChange(self):
        self.emit(SIGNAL("validityKeyChange"), self.toValidityKey())

    ## Slot used to set the value equal to the minimum possible according to the
    #  current constraints.
    def setToMinimum(self):
        spec = self._edit.timeSpec()
        self.setDateTime(self.minimumDateTime().toTimeSpec(spec))
    ## Slot used to set the value equal to the maximum possible according to the
    #  current constraints.
    def setToMaximum(self):
        spec = self._edit.timeSpec()
        self.setDateTime(self.maximumDateTime().toTimeSpec(spec))    
    ## Slot used to set the value equal to the current time or to the closest
    #  limit if it is out of bounds.
    def setToNow(self):
        spec = self._edit.timeSpec()
        t = QDateTime.currentDateTime()
        if t > self.maximumDateTime():
            t = self.maximumDateTime()
        elif t < self.minimumDateTime():
            t = self.minimumDateTime()
        self.setDateTime(t.toTimeSpec(spec))

## Simple customization of a QPlainTextEdit.
#  The extensions to a QPlainTextEdit are a "find" dialog (activated with Ctrl+F
#  or the contextual menu) and the possibility to switch to/from fixed width
#  font (via contextual menu). 
class SearchableTextEdit(QtGui.QPlainTextEdit):
    ## Contructor.
    def __init__(self, parent = None):
        super(SearchableTextEdit,self).__init__(parent)
        
        from Dialogs import FindDialog
        self.findDialog = FindDialog(self)
        
        self.actionFind = QtGui.QAction(self)
        self.actionFind.setObjectName("actionFind")
        self.actionFind.setText("&Find...")
        self.actionFind.setShortcut("Ctrl+F")
        self.addAction(self.actionFind)
        
        QObject.connect(self.actionFind, SIGNAL("triggered()"), self.findDialog.show)
        QObject.connect(self.findDialog, SIGNAL("find(QString,QTextDocument::FindFlags,bool)"),
                        self.findInText)
        
        self._defaultFont = self.font()
        self.actionFixedWidthFont = QtGui.QAction(self)
        self.actionFixedWidthFont.setObjectName("actionFixedWidthFont")
        self.actionFixedWidthFont.setText("Use fi&xed width font")
        self.actionFixedWidthFont.setCheckable(True)
        self.addAction(self.actionFixedWidthFont)
        if self._defaultFont.fixedPitch():
            self.actionFixedWidthFont.setChecked(True)
            self.actionFixedWidthFont.setEnabled(False)
        
        QObject.connect(self.actionFixedWidthFont, SIGNAL("triggered(bool)"),
                        self.setFixedWidthFont)
        
    def contextMenuEvent(self, event):
        menu = self.createStandardContextMenu()
        menu.addSeparator()
        for a in self.actions():
            menu.addAction(a)
        menu.exec_(event.globalPos())
        del menu
    
    ## Slot used by the find dialog to trigger a search in the data view
    def findInText(self, text, flags, wrapped):
        # look for the string
        found = self.find(text, flags)
        if not found and wrapped:
            # try again for wrapped search
            if flags & QtGui.QTextDocument.FindBackward:
                where = QtGui.QTextCursor.End
            else:
                where = QtGui.QTextCursor.Start
            self.moveCursor(where)
            found = self.find(text, flags)
        if not found:
            QtGui.QMessageBox.information(self, "Not found",
                                          "String '%s' not found in the document." % text)

    ## FixedWidthFont property
    def setFixedWidthFont(self, value):
        if self.actionFixedWidthFont.isEnabled():
            if value:
                font = QtGui.QFont("Currier")
                font.setFixedPitch(value)
                self.setFont(font)
            else:
                font = QtGui.QFont(self._defaultFont)
                font.setFixedPitch(value)
                self.setFont(font)
            if self.actionFixedWidthFont.isChecked() != value:
                self.actionFixedWidthFont.setChecked(value)
    ## FixedWidthFont property
    def isFixedWidthFont(self):
        return self.actionFixedWidthFont.isChecked()
