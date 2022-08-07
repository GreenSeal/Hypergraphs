import glob, os

names = []
cutSizes = []
reducedCutSizes = []
reducedRatios = []
timeSpent = []

os.chdir("../benchmark_set")
for file in glob.glob("*.result"):
    nameSize = len(file)
    test_name = file[:nameSize - 11]
    current_dir = os.getcwd()
    print(os.getcwd())
    print(os.path.exists(current_dir + "/" + test_name + ".result"))
    if not(os.path.exists(current_dir + "/" + test_name + ".result")):
        continue
    f1 = open(test_name + ".result", "r")
    f2 = open(file, "r")
    names.append(test_name)

    line1 = f1.readline()
    line2 = f2.readline()
    cutSizes.append([int(line1), int(line2)])

    line1 = f1.readline()
    line2 = f2.readline()
    reducedCutSizes.append([int(line1), int(line2)])

    line1 = f1.readline()
    line2 = f2.readline()
    reducedRatios.append([float(line1), float(line2)])

    line1 = f1.readline()
    line2 = f2.readline()
    timeSpent.append([float(line1), float(line2)])
    f1.close()
    f2.close()

os.chdir("..")

avg_ratio, avg_ratio_mod = 0, 0;
avg_time, avg_time_mod = 0, 0;
with open("result.csv", 'w') as f:
    f.write("Name" + "; " +
            "Result cutSize" + "; " +
            "Gain" + "; " +
            "Reduced cutSize ratio" + "; " +
            "time" + "; " +
            "Result cutSize mod" + "; " +
            "Gain mod" + "; " +
            "Reduced cutSize ratio mod" + "; " +
            "time mod" + "; " + "\n")
    for i in range(0, len(names)):
        str_to_write = names[i] + "; " + str(cutSizes[i][0]) + "; "
        str_to_write += str(reducedCutSizes[i][0]) + "; " + str(reducedRatios[i][0]) + "; "
        str_to_write += str(timeSpent[i][0]) + "; " + str(cutSizes[i][1]) + "; "
        str_to_write += str(reducedCutSizes[i][1]) + "; " + str(reducedRatios[i][1]) + "; "
        str_to_write += str(timeSpent[i][1]) + "\n"

        avg_ratio += reducedRatios[i][0]/float(len(names))
        avg_ratio_mod += reducedRatios[i][1]/float(len(names))

        avg_time += timeSpent[i][0]/len(names)
        avg_time_mod += timeSpent[i][1]/len(names)



        f.write(str_to_write)
    f.write(" ; ; ; " + str(avg_ratio) + "; " + str(avg_time) + "; " + "; ; " + str(avg_ratio_mod) + "; " + str(avg_time_mod) + "\n")
