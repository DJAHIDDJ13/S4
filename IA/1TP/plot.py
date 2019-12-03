#!/usr/bin/env python
# -*- coding: utf-8 -*- 
import matplotlib.pyplot as pp
import pandas as pd
import numpy as np

cols = ["with_ab", "depth","avg_moves", "avg_time", "avg_nodes"]
data = pd.read_csv("output.csv", delimiter=":", names=cols)

with_ab = data.query("with_ab")\
            .groupby('depth', as_index=False)\
            .mean()\
            .sort_values(by='depth')

without_ab = data.query("not with_ab")\
            .groupby('depth', as_index=False)\
            .mean()\
            .sort_values(by='depth')

pp.title("Comparaison entre negamax avec et sans alpha beta")
pp.xlabel("Profondeur")
pp.ylabel("Nombre des noueds traités (Knodes)")

pp.plot(with_ab['depth'], with_ab['avg_nodes'].div(1000), label='avec alpha/beta')
pp.plot(without_ab['depth'], without_ab['avg_nodes'].div(1000), label='sans alpha/beta')
pp.legend(loc='best')
#pp.show()
pp.savefig("doc/comp_nodes.pdf")
print('generated doc/comp_nodes.pdf')

pp.cla()
pp.title("Comparaison entre negamax avec et sans alpha beta")
pp.xlabel("Profondeur")
pp.ylabel("Temps d'execution (ms)")

pp.plot(with_ab['depth'], with_ab['avg_time'], label='avec alpha/beta')
pp.plot(without_ab['depth'], without_ab['avg_time'], label='sans alpha/beta')

pp.legend(loc='best')
#pp.show()
pp.savefig("doc/comp_times.pdf")
print('generated doc/comp_times.pdf')
