import matplotlib.pyplot as pp

attrs = ["alpha_beta", "depth", "avg_moves", "avg_time"]
results = {a:[] for a in attrs}

with open("output.csv", "r") as f:
    line = f.readline()
    while line:
        is_alpha, prof, avg_moves, avg_time = line.split(':')
        is_alpha, prof, avg_moves, avg_time = bool(is_alpha), int(prof), float(avg_moves),float(avg_time)
        
        results["alpha_beta"] += [is_alpha]
        results["depth"] += [prof]
        results["avg_moves"] += [avg_moves]
        results["avg_time"] += [avg_time]

        line = f.readline()

pp.title("Average moves")
pp.xlabel("Depth")
pp.ylabel("Elapsed time (ms)")
pp.plot(range(1, 3), [results["avg_time"][i] for i in range(1, 3)])
pp.savefig("plot.pdf")

