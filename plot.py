import matplotlib
import matplotlib.pyplot as plt
import numpy as np

import gi
gi.require_version('Gtk', '3.0')
from gi.repository import Gtk
from gi.repository import Pango as pango

from matplotlib.backends.backend_gtk3agg import FigureCanvasGTK3Agg as FC
from matplotlib.backends.backend_gtk3 import NavigationToolbar2GTK3 as NT

# https://matplotlib.org/3.1.1/gallery/subplots_axes_and_figures/subplots_demo.html
class PlotWindow(Gtk.Window):
    def __init__(self):
        Gtk.Window.__init__(self, title='Example Plot')
        self.set_icon_from_file("resources/CyberSolver_Icon.png")
        self.connect('destroy', Gtk.main_quit)

# Data for plotting
t = np.arange(0.0, 2.0, 0.01)
s = 1 + np.sin(2 * np.pi * t)

fig, ax = plt.subplots()
ax.plot(t, s)

ax.set(xlabel='time (s)', ylabel='voltage (mV)',
       title='About as simple as it gets, folks')
ax.grid()

#fig.savefig("test.png")

main = PlotWindow()
main.set_title("Example Plot")
main.set_default_size(800,600)
box = Gtk.VBox()
main.add(box)

fc = FC(fig)
box.pack_start(fc, expand=True, fill=True, padding=False)
nt = NT(fc, main)
box.pack_start(nt, expand=False, fill=False, padding=False)
main.show_all()
Gtk.main()

#plt.show()
