from random import randint
import matplotlib.pyplot as pp
import math

def simule_CCP(n):
    tirages = [False for _ in range(n)]
    somme_tirages = 0
    run_time = 1
    while somme_tirages < n:
        tirage = randint(0, n-1)
        if not tirages[tirage]:
            tirages[tirage] = True
            somme_tirages += 1

        run_time +=1
    
    return run_time

def moyenne_CCP(n, k):
    return sum([simule_CCP(n) for _ in range(k)])/k


if __name__ == "__main__":
    # moyenne de  k CCP
    N = 20 
    k_n = lambda n: int((100/5.7) * math.pi * n**2)
œ    
    X_axis = range(1, N+1)
    pp.plot(X_axis, [moyenne_CCP(n, k_n(n)) for n in X_axis], label="moyenn_CCP")
    pp.plot(X_axis, [n*math.log(n, 2) for n in X_axis], label="nlog(n)")
    pp.legend(loc="best")
    pp.show()

