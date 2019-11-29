import matplotlib.pyplot as pp
import inspect

sensor = {"close": {"x":[300 , 350, 1023], "y":[0, 1, 1]},
          "half":  {"x":[150 , 250, 350 ], "y":[0, 1, 0]},
          "far":   {"x":[0   , 100, 250 ], "y":[1, 1, 0]}}

motor = {"reverse":{"x":[-5  , -4 , -3  ], "y":[1, 1, 0]},
         "stop"   :{"x":[-0.3, 0  , 0.3 ], "y":[0, 1, 0]},
         "high"   :{"x":[3   , 4  , 5   ], "y":[0, 1, 1]}}

def plt_fuzzy(f, n):
    fig, ax = pp.subplots()

    xvals = []
    for name in f:
        s = f[name]
        xvals += s["x"]
        pp.plot(s["x"], s["y"], label=name)
    
    ax.xaxis.set_ticks(list(set(xvals)))
    
    pp.xticks(rotation=90)
    pp.legend(loc="best")
    pp.savefig(n + ".pdf")

plt_fuzzy(sensor, "sensor")
plt_fuzzy(motor, "motor")
