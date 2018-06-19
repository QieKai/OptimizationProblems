#!/usr/bin/env sage -python
import sys
import networkx as nx
from sage.all import *

T=[]
S=[]
graphname=""
def readAdjMatGraph():
  global graphname
  graphname=sys.stdin.readline().strip()
  n=int(sys.stdin.readline().strip())
  for i in range(n):
      S.append(i)
  G = Graph(n)
  adj=[]
  for u in range(n):
      row=sys.stdin.readline().strip().split()
      for vs in row:
          v=int(vs)
          G.add_edges([(u,v,'1')])
          adj.append((u,v))
  idx=0
  for u in range(n):
      row=sys.stdin.readline().strip().split()
      for weight in row:
          (key,value)=adj[idx]
          G.set_edge_label(key,value,weight)
          idx=idx+1
  row=sys.stdin.readline().strip().split()
  for terminal in row:
      T.append(int(terminal))

  return G

cnt=1
print "\\noindent"
while True:
  T=[]
  S=[]
  G=readAdjMatGraph()
  if G.order()==0: break
  root=[]
  root.append(T[0])
  for v in T:
      S.remove(v)

  d = {'#FF0000':root,'#00FF00':T, '#FFFFFF':S}
  p=G.plot(figsize=[6,6],vertex_size=800,vertex_labels=true,edge_labels=True,vertex_colors=d)
  #p.show()
  p.save("steiner.drawings/"+graphname+".png")
  print "\\includegraphics[width=3cm]{steiner.drawings/"+graphname+".png}"
  if cnt%5==0: print "\\\\"
  cnt+=1
exit
