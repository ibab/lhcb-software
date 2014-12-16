import ROOT
from guidict import GUI_CONF_DICT, button_label, histpath_label, canvas_part


class MainFrame(ROOT.TGMainFrame):

    def __init__(self, parent, width, height):
        ROOT.TGMainFrame.__init__(self, parent, width, height)

        self.data_file = ROOT.TFile("functions1.root")
        self.dispatcher = ROOT.TPyDispatcher(self.clever_dispatching)

        self.embeded_canvas = ROOT.TRootEmbeddedCanvas('Canvas1', self, 400, 200)
        self.canvas = self.embeded_canvas.GetCanvas()
        self.canvas.Divide(2)
        self.AddFrame(self.embeded_canvas, ROOT.TGLayoutHints())

        self.buttons_frame = ROOT.TGHorizontalFrame(self, 400, 40)
        self.create_buttons()

        self.exitbutton = ROOT.TGTextButton(self.buttons_frame, '&Exit')
        self.exitbutton.SetCommand('TPython::Exec( "raise SystemExit" )')
        self.buttons_frame.AddFrame(self.exitbutton, ROOT.TGLayoutHints())

        self.AddFrame(self.buttons_frame, ROOT.TGLayoutHints())

        self.SetWindowName('GUI with clever disptacher prototype')
        self.MapSubwindows()
        self.Resize(self.GetDefaultSize())
        self.MapWindow()

    def __del__(self):
        self.data_file.Close()
        self.Cleanup()

    def create_buttons(self):
        for properties in GUI_CONF_DICT.itervalues():
            if button_label in properties:
                print "Found a button: {}".format(properties[button_label])
                print "Creating a button for: {}".format(properties[histpath_label])
                temp_button = ROOT.TGTextButton(self.buttons_frame, "&{}".format(properties[button_label]))
                temp_button.Connect('Clicked()', "TPyDispatcher", self.dispatcher, 'Dispatch()')
                self.buttons_frame.AddFrame(temp_button, ROOT.TGLayoutHints())

    def clever_dispatching(self):
        btn = ROOT.BindObject(ROOT.gTQSender, ROOT.TGTextButton)
        label = btn.GetText()
        print "Looking for a histogram for button: {}".format(label)
        for properties in GUI_CONF_DICT.itervalues():
            try:
                if properties[button_label].strip() == str(label).strip():
                    hist = self.data_file.Get(properties[histpath_label])
                    self.canvas.cd(properties[canvas_part])
                    print "Histogram has been succesfully found, drawing..."
                    hist.Draw()
                    ROOT.gPad.Update()
                    return
            except KeyError:
                continue
        else:
            print "ERROR: The button couldn't have been found"


if __name__ == '__main__':
    window = MainFrame(ROOT.gClient.GetRoot(), 400, 400)
