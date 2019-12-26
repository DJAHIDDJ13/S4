# script to add noise to the alphabet data set
# The data for the letters is harcoded here

from random import uniform

s = """0 1 1 0 1 0 0 1 1 1 1 1 1 0 0 1 1 0 0 1
1 1 1 0 1 0 0 1 1 1 1 1 1 0 0 1 1 1 1 0
1 1 1 1 1 0 0 0 1 0 0 0 1 0 0 0 1 1 1 1
1 1 1 0 1 0 0 1 1 0 0 1 1 0 0 1 1 1 1 0
1 1 1 1 1 0 0 0 1 1 1 1 1 0 0 0 1 1 1 1 
1 1 1 1 1 0 0 0 1 1 1 1 1 0 0 0 1 0 0 0
0 1 1 1 1 0 0 0 1 0 1 1 1 0 0 1 0 1 1 1 
1 0 0 1 1 0 0 1 1 1 1 1 1 0 0 1 1 0 0 1
1 1 1 1 0 1 1 0 0 1 1 0 0 1 1 0 1 1 1 1
0 1 1 1 0 0 1 0 0 0 1 0 1 0 1 0 1 1 1 0
1 0 0 1 1 0 1 0 1 1 0 0 1 0 1 0 1 0 0 1
1 0 0 0 1 0 0 0 1 0 0 0 1 0 0 0 1 1 1 1
1 0 0 1 1 1 1 1 1 0 0 1 1 0 0 1 1 0 0 1
1 0 0 1 1 1 0 1 1 1 0 1 1 0 1 1 1 0 0 1
0 1 1 0 1 0 0 1 1 0 0 1 1 0 0 1 0 1 1 0
1 1 1 0 1 0 0 1 1 1 1 0 1 0 0 0 1 0 0 0
0 1 1 0 1 0 0 1 1 0 0 1 1 0 1 1 0 1 1 1
1 1 1 0 1 0 0 1 1 1 1 0 1 0 1 0 1 0 0 1
0 1 1 1 1 0 0 0 0 1 1 0 0 0 0 1 1 1 1 0
1 1 1 1 0 1 1 0 0 1 1 0 0 1 1 0 0 1 1 0
1 0 0 1 1 0 0 1 1 0 0 1 1 0 0 1 1 1 1 1
1 0 0 1 1 0 0 1 1 0 0 1 1 0 0 1 0 1 1 0
1 0 0 1 1 0 0 1 1 0 0 1 1 1 1 1 1 0 0 1
1 0 0 1 1 0 0 1 0 1 1 0 1 0 0 1 1 0 0 1
1 0 0 1 1 0 0 1 0 1 1 0 0 1 1 0 0 1 1 0
1 1 1 1 0 0 1 0 0 1 0 0 0 1 0 0 1 1 1 1 """.split("\n")

test = lambda p: uniform(0, 1) < p # returns True with probability p
toggle = lambda x: 1-x # returns 0 if input is 1 and 1 if input is 0

# the probabily of reversing the pixel
prob = 0.1
for i, line in enumerate(s):
    for j, c in enumerate(line.split()):
        print(toggle(int(c)) if test(prob) else int(c), end=' ')
    print()
    
    for j in range(26):
        print(1 if j+1 == i else 0, end=' ')
    print('\n')

        
