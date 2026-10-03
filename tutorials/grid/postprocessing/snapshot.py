"""Figure each timestep."""
import csv
import matplotlib.pyplot as plt
import os
import progressbar
from screeninfo import get_monitors


pathMeshLink = "../mesh/polyLinks"
x1_values = []
x2_values = []
y1_values = []
y2_values = []

with open(pathMeshLink, 'r') as csv_file:
    csv_reader = csv.reader(csv_file, delimiter=' ')

    for row in csv_reader:
        x1_values.append(float(row[0]))
        y1_values.append(float(row[1]))
        x2_values.append(float(row[2]))
        y2_values.append(float(row[3]))


main_directory = '../data/time/'
entries = os.listdir(main_directory)
# locate folders
numeric_folders = [entry
                      for entry
                      in entries
                      if os.path.isdir(os.path.join(main_directory,
                                                    entry))
                      and entry.isdigit()]
sorted_numeric_folders = sorted(numeric_folders, key=int)

bar = progressbar.ProgressBar(maxval=len(sorted_numeric_folders)).start()
bar = progressbar.ProgressBar(maxval=len(sorted_numeric_folders)).start()
# figure size
screen_width = get_monitors()[0].width
screen_height = get_monitors()[0].height
width = screen_width/100
height = screen_height/100

for i in sorted_numeric_folders:
    fileName = main_directory + i + "/xy"
    fileName2 = main_directory + i + "/U"
    fileName3 = main_directory + i + "/evacuatedPedestrianCount"
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

    fig, ax = plt.subplots(1, 1, figsize=(width, height), tight_layout=True)
    # lines or streets
    ax.plot([x1_values, x2_values], [y1_values, y2_values], c="k", lw=1)
    vmin, vmax = 0.0, 1.3
    # points or pedestrians
    scatter = ax.scatter(x_values, y_values, c=magnitude,
                         cmap="jet_r", marker='o', edgecolors="none",
                         vmin=vmin, vmax=vmax)
    # text
    evacuated_count = str(int(evacuatedPedestrianCount[0]))
    text1 = "t = " + i + " sec; evacuated: " + evacuated_count
    fig.text(0.05, 0.02, text1, fontsize=20, fontweight='normal',
             transform=ax.transAxes)
    # colorbar
    plt.colorbar(scatter, ax=ax, fraction=0.03)
    # settings
    xlim_auto = ax.get_xlim()
    ylim_auto = ax.get_ylim()
    ax.set_xlim(xlim_auto)
    ax.set_ylim(ylim_auto)
    ax.axis('off')
    plt.savefig("snapshot/" + i)
    plt.close(fig)
    bar.update(sorted_numeric_folders.index(i) + 1)
bar.finish()
