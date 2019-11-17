
from random import *

#pour que tout le monde ait la meme sequence aleatoire, on fixe la graine
seed(a="51")



#Codage des couleurs :
#rouge = "rouge"
#bleu = "bleu"
#vert = "vert"
#jaune = "jaune"
#noir = "noir"

"""
Les etats (identifies aux cases) sont numerotes de 0 a 15, de sorte que
la case (i,j) a le numero 4*i+j
Ainsi l'abscisse i de la case a l'etat k est k//4, l'ordonnee j est k%4
"""



#le plateau contient les couleurs de chaque case
# c'est une variable globale a ne pas modifier
plateau = ["noir","rouge","jaune","bleu","vert","bleu","rouge","noir","rouge","noir","vert","jaune","noir","vert","jaune","bleu"]


def probas() :
	#retourne une liste a deux dimensions probabilites telle que
	#probabilites[k][l] est la probabilite d'aller de l'etat k a l'etat l.
	#il s'agit donc de la matrice de la chaine de markov.
	probabilites = [[0 for k in range(16)] for k in range(16)]
	
	
	#pour chaque case k, on regarde la probabilite d'aller 
	#dans chaque voisin
	for k in range(16) :
		#i et j sont les coordonnees de la case k.
		(i,j) = (k//4,k%4)
		
		#proba que le deplacement echoue et que l'on reste en place
		echec=0
		
		#on essaie d'aller en (i-1,j)
		if((i!=0) and (plateau[4*(i-1)+j]!="noir")) :
			probabilites[k][4*(i-1)+j]=0.25
		else :
			echec+=0.25
		
		#on essaie d'aller en (i+j,j)
		if((i!=3) and (plateau[4*(i+1)+j]!="noir")) :
			probabilites[k][4*(i+1)+j]=0.25
		else :
			echec+=0.25
		
		#on essaie d'aller en (i,j-1)
		if((j!=0) and (plateau[4*i + j-1]!="noir")) :
			probabilites[k][4*i + j-1]=0.25
		else :
			echec+=0.25
		
		#on essaie d'aller en (i,j+1)
		if((j!=3) and (plateau[4*i + j+1]!="noir")) :
			probabilites[k][4*i + j+1]=0.25
		else :
			echec+=0.25
		
		#on reste en place avec la proba echec.
		probabilites[k][k]=echec;

	return probabilites
	
# la variable globale probabilites contiendra la matrice des probas d'aller de tout etat a tout etat
probabilites = probas()




def macheAleatoire(etatInit=6, duree=30) :
	"""
	Fonction qui realise une marche aleatoire sur la chaine de Markov, 
	pendant duree etapes a partir de l'etat etatInit;
	on enregistre la sequence d'etats visites dans etatsCaches 
	et la sequence des couleurs observes dans observations
	"""
	etatsCaches = []
	observations = []

	etat = etatInit

	for t in range (duree):
		etatsCaches.append(etat)
		observations.append(plateau[etat])
		
		#on choisit l'etat suivant a partir de la matrice de probabilites
		#on ne passe pas par la matrice des probas cumulees, mais on calcule la somme au fur et a mesure
		d=uniform(0,1)
		
		etatSuivant=0
		
		somme = probabilites[etat][etatSuivant]
		
		while (d>somme) :
			etatSuivant+=1
			somme+=probabilites[etat][etatSuivant]
		#on a trouve l'etat suivant, qui est alors le nouvel etat
		etat=etatSuivant
	
	#on retourne etatsCaches et observations (sous forme de couple)
	return (etatsCaches, observations)
	
	
# Fonction probaObservation(etat, couleur) calculant la probabilite d'oberver
# la couleur "couleur" lorsque l'on est dans l'etat "etat".
def probaObservation(etat, couleur):
        return plateau[etat] == couleur

# a completer

def viterbi(etatInit, observation):
	"""
	Fonction implementant l'algorithme de Viterbi
	Cette fonction calcule donc la sequence d'etats la plus probable 
	compte tenu de l'etat initial et 
	de la sequence de couleurs observees donnes en parametre
	"""

	#duree de la simulation :
	n=len(observation)
	
	# T est un tableau de taille n * 16
	# T[t][k] contiendra la probabilite de la sequence d'etats la plus
	#probable se terminant a l'instant t dans l'etat k.
	T = [[0 for k in range (16)]for t in range (n)];

        # predecesseur[t][k] sera l'etat a l'instant precedent dans la sequence
        # d'etats la plus probable se terminant en k a l'instant t.
        predecesseur = [[0 for k in range (16)]for t in range (n)];

	#   Phase 1 : initialisation : donner des valeurs aux T[0][k]
        for i in range(len(plateau)):
                T[0][i] = (1 if i == etatInit else 0) * probaObservation(i, observation[0])

	#  Phase 2 : propagation. Calcul des tableaux T et predecesseur.
	for t in range(1, n):
                for j in range(len(plateau)):
                        p, pred = 0, 0
                        for i in range(len(plateau)):
                                score = T[t-1][i] * probabilites[i][j] * probaObservation(j, observation[t])
                                if score > p:
                                        p = score
                                        pred = i
                        T[t][j] = p
                        predecesseur[t][j] = pred
        	
	#  Phase 3 : decodage : trouver la probabilite maximum

	#calcul du dernier etat de la sequence la plus probable
	# a l'aide de T
	idxSeqMax = 0
	for t in range(len(plateau)):
                if T[n-1][idxSeqMax] < T[n-1][t]:
                        idxSeqMax = t
        

	# un tableau pour coder la sequence d'etats la plus probable :
	sequencePlusProbable = [0] *n

	# calcul de la sequence la plus probable a l'aide de predecesseur
	sequencePlusProbable[n-1] = idxSeqMax
        for t in range(n-2, -1, -1):
                sequencePlusProbable[t] = predecesseur[t+1][sequencePlusProbable[t+1]]

        # sequence des valeur entre 0 et 15 des etats les plus probables 
        sequenceCoordonees1D = [T[n-1][v] for v in sequencePlusProbable]
        
	return [(i//4, i%4) for i in sequenceCoordonees1D]


#Programme principal

duree = 30

etatInitial = 6

#On réalise une mache aléatoire
(etatsCaches,observations) = macheAleatoire(etatInitial,duree)

#On veut maintenant utiliser la sequence d'observations
# pour deviner avec viterbi par ou est passee la marche aleatoire.
guess = viterbi(etatInitial, observations)

#Affichage de la marche aléatoire
for t in range (duree) :
	print("Etape ",t,": couleur ",observations[t],"  \t case : ",etatsCaches[t]//4,",",etatsCaches[t]%4, " \t guess: ", guess[t], sep="")

