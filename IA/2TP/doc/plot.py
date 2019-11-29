import matplotlib.pyplot as pp

sensor = {"close":{"x":[200 , 300, 1023], "y":[0, 1, 1]},
          "half": {"x":[150 , 250, 300], "y": [0, 1, 0]},
          "far":  {"x":[0   , 100, 250], "y":[1, 1, 0]}}
for name in sensor:
    s = sensor[name]
    print(s["x"])
    pp.plot(s["x"], s["y"], label=name)
pp.show()
