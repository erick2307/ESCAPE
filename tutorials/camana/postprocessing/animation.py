"""Figure each timestep."""
import csv
import matplotlib.pyplot as plt
import os
from matplotlib.animation import FuncAnimation
from screeninfo import get_monitors
from progressbar import ProgressBar
from os import path
from mpl_toolkits.axes_grid1.inset_locator import zoomed_inset_axes as zoom_inset_axes
#files
absolute_path = path.abspath(__file__)
directory_main = path.dirname(path.dirname(absolute_path))
directory_mesh = path.join(directory_main, "mesh/polyLinks")
directory_data = path.join(directory_main, "data/time/")
directory_export = path.join(directory_main, "postprocessing/snapshot/")

x1_values = []
x2_values = []
y1_values = []
y2_values = []

with open(directory_mesh, 'r') as csv_file:
    csv_reader = csv.reader(csv_file, delimiter=' ')

    for row in csv_reader:
        x1_values.append(float(row[0]))
        y1_values.append(float(row[1]))
        x2_values.append(float(row[2]))
        y2_values.append(float(row[3]))

entries = os.listdir(directory_data)
numeric_folders = [entry
                      for entry
                      in entries
                      if os.path.isdir(os.path.join(directory_data,
                                                    entry))
                      and entry.isdigit()]
sorted_numeric_folders = sorted(numeric_folders, key=int)
bar = ProgressBar(maxval=len(sorted_numeric_folders)).start()

width = 12
height = 5
# figure size
screen_width = get_monitors()[0].width
screen_height = get_monitors()[0].height
width = screen_width/100
height = screen_height/100

fig, ax = plt.subplots(1, 1, figsize=(width, height), tight_layout=True)
i_initial = int(sorted_numeric_folders[0])


def update(i):
    """Loop each timestep."""
    ax.clear()
    fileName = directory_data + sorted_numeric_folders[i] + "/xy"
    fileName2 = directory_data + sorted_numeric_folders[i] + "/U"
    fileName3 = directory_data + sorted_numeric_folders[i] + "/evacuatedPedestrianCount"
    x_values = []
    y_values = []
    magnitude = []
    evacuatedPedestrianCount = []

    with open(fileName, 'r') as csv_file:
        csv_reader = csv.reader(csv_file, delimiter=' ')
        for row in csv_reader:
            x_values.append(float(row[0]))
            y_values.append(float(row[1]))

    with open(fileName2, 'r') as csv_file2:
        csv_reader = csv.reader(csv_file2, delimiter=' ')
        for row in csv_reader:
            magnitude.append(float(row[0]))

    with open(fileName3, 'r') as csv_file3:
        csv_reader = csv.reader(csv_file3, delimiter=' ')
        for row in csv_reader:
            evacuatedPedestrianCount.append(float(row[0]))

    # lines or streets
    ax.plot([x1_values, x2_values], [y1_values, y2_values], c="k", lw=1)
    vmin, vmax = 0.0, 1.3
    # points or pedestrians
    scatter = ax.scatter(x_values, y_values, c=magnitude,
                         cmap="jet_r", marker='o', edgecolors="none",
                         vmin=vmin, vmax=vmax)

    axins = zoom_inset_axes(ax, zoom=5, loc='upper right')
    axins.plot([x1_values, x2_values], [y1_values, y2_values], c="k", lw=1)
    axins.scatter(x_values, y_values, c=magnitude,
                  cmap="jet_r", marker='o', edgecolors="none",
                  vmin=vmin, vmax=vmax)
    axins.set_xlim({746500, 747500})
    axins.set_ylim({8157000, 8157500})

    # text
    evacuated_count = str(int(evacuatedPedestrianCount[0]))
    text1 = "t = " + str(i) + " sec; evacuated: " + evacuated_count
    ax.text(0.05, 0.02, text1, fontsize=20, fontweight='normal',
            transform=ax.transAxes)
    # settings
    ax.axis('off')
    # ax.xaxis.set_label_coords(0, -0.06)
    # ax.set_axis_off()
    if i == i_initial:
        # cax = fig.add_axes([0.93, 0.1, 0.02, 0.8])
        plt.colorbar(scatter, ax=ax, fraction=0.03)
    # ax.set_axis_off()
    # if (i == "1"):
    #     xlim_auto = ax.get_xlim()
    #     ylim_auto = ax.get_ylim()
    # else:
    #     ax.set_xlim(xlim_auto)
    #     ax.set_ylim(ylim_auto)
    bar.update(i + 1)

    ax.set_xlim([743000, 756500])
    ax.set_ylim([8155500, 8161000])

ani = FuncAnimation(fig, update, len(sorted_numeric_folders)-1,
                    interval=100, repeat_delay=900)
ani.save(directory_export + 'animation.mp4')
bar.finish()
