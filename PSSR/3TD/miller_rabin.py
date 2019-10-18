
from random import *

def miller_rabin(n, k=10):
	"""
	Fonction implementant le test de primalite de Miller Rabin.
	n est le nombre teste
	k est le nombre d'iterations de test effectuees (fixe par defaut a 10).
	"""
	if n == 2:
		return True
	if not n & 1:
		return False

	def temoin(a, s, d, n):
		x = pow(a, d, n)
		if x == 1:
			return False
		for i in range(s - 1):
			if x == n - 1:
				return False
			x = pow(x, 2, n)
		return x != n - 1

	s = 0
	d = n - 1

	while d % 2 == 0:
		d >>= 1
		s += 1

	for i in range(k):
		a = randrange(2, n - 1)
		if temoin(a, s, d, n):
			return False
	return True



def getPrime(bits):
	"""
	Fonction qui retourne un nombre premier du nombre de bits choisi
	"""
	while(True) :
		# on continue a tirer des nombres tant que l'on n'a pas trouve de nombre premier
		p = getrandbits(bits)
		if(miller_rabin(p,100)) :
			return p
			

if __name__ == "__main__":
    bits = 1000
    N = 10
    k = 100
    nums = [getrandbits(bits) for _ in range(N)]
    for num in nums:
        print("prime " if miller_rabin(num, k) else "not prime ", num)

    
