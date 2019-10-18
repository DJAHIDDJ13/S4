from BoiteNoire import *
from random import uniform
import matplotlib.pyplot as pp
import math

# integration pour n dimensions
def integral(fun, vals, n):
    surface = reduce(lambda x,y:x*y, [b-a for a,b in vals], 1.0)
    s = 0.0
    dim = len(vals)

    for _ in range(n):
        s += fun(*[uniform(a, b) for a,b in vals]) / n
    
    return surface * s

if __name__ == "__main__":
    N = 100
    print(integral(fonction1, [[0, 1]], N))
    print(integral(fonction2, [[0, math.sqrt(3)]], N))
    print(integral(fonction3, [[0, 1], [-1, 3]], N))
    print(integral(fonction4, [[3, 7], [-4 ,-1], [2, 3]], N))
    
    N = 100 
    X_axis = [float(i)/N for i in range(N+1)]
    print(X_axis)
    f = lambda x: x *(1-x) * math.sin(200 * x * (1-x))
    pp.plot(X_axis, [f(x) for x in X_axis])
    pp.show() 
