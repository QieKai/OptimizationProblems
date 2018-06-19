#!/usr/bin/env sage -python
import sys
import networkx as nx
from sage.all import *

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

  return G

cnt=1
print "\\noindent"
while True:
  S=[]
  G=readAdjMatGraph()
  if G.order()==0: break
  S.remove(0)
  d = {'#FF0000':[0],'#FFFFFF':S}
  p=G.plot(figsize=[6,6],vertex_size=800,vertex_labels=true,edge_labels=True,vertex_colors=d)
  #p.show()
  p.save("dcmst.drawings/"+graphname+".png")
  print "\\includegraphics[width=1.5cm]{dcmst.drawings/"+graphname+".png}"
  if cnt%11==0: print "\\\\"
  cnt+=1
exit
