def viterbi(n, T, M, X0, A):
    T = [[]]
    pred = [[]]
    for i in range(n):
        T[0].append(X0[i] * A[i][])
