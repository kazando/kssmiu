import itertools
import gurobipy as gp
import sys
from typing import List, Tuple, Sequence, Optional
import time
import os

def read_phylip_full_matrix(filepath: str) -> Tuple[List[str], List[List[float]]]:
    names: List[str] = []
    full: List[List[float]] = []
    with open(filepath, 'r') as f:
        lines = [ln.strip() for ln in f if ln.strip()]
    if not lines:
        raise ValueError("Empty PHYLIP file")
    try:
        n = int(lines[0].split()[0])
    except Exception as e:
        raise ValueError("Could not parse number of taxa from first line") from e
    if len(lines) < n + 1:
        raise ValueError(f"PHYLIP file truncated: expected {n} data lines, got {len(lines)-1}")
    for i in range(n):
        parts = lines[i+1].split()
        if len(parts) < n + 1:
            raise ValueError(f"Line {i+2}: expected name + {n} distances, got {len(parts)}")
        name = parts[0]
        vals = [float(x) for x in parts[1:1+n]]
        names.append(name)
        full.append(vals)
    return names, full


args = sys.argv
filepath = args[1]
base = os.path.splitext(os.path.basename(filepath))[0]

_,M = read_phylip_full_matrix(filepath)

n = int(len(M))

V = range(1,n+1)
E = {(int(i+1),int(j+1)):float(v) for i in range(n) for j in range(n) if i < j for v in [M[i][j]]}

delta = [0]
tot=0.0
for i in range(1,n):
    for j in range(i+1,n+1):
        tot=tot+E[i,j]
        if E[i,j] not in delta:
            delta.append(E[i,j])

delta_len = len(delta)-1
delta.sort()
#print(delta)

def add_variables(m,delta):
	x={}
	for i in range(1,n+1):
		for j in range(i+1,n+1):
			for t in range(1,delta_len+1):
				vname = 'x_{0}_{1}_{2}'.format(i, j, t)
				x[i, j, t] = m.addVar(lb=0.0, ub=1.0, obj=delta[t] - delta[t-1],vtype=gp.GRB.BINARY, name=vname)
	m._vars = x

def add_layer_constraints(m):
    start = time.time()
    # add layer constraints
    n = m._n

    for t in range(1,delta_len+1):
         for i in range(1,n+1):
              for j in range(i+1,n+1):
                if E[i,j] > delta[t-1]:
                    m.addConstr(m._vars[i,j,t]==1)

    for t in range(1,delta_len):
        for i in range(1,n+1):
            for j in range(i+1,n+1):
                cname = 'layer_{0}_{1}_{2}'.format(i, j, t)
                m.addConstr(m._vars[i,j,t] - m._vars[i,j,t+1]>=0,cname)
    m.update()
    end = time.time()
    print('Time to add layer constraints = {0}'.format(end - start))

def add_triangle_constraints(m):
    # Add trianlge inequality explicitly
    n = m._n
    start = time.time()
    for t in range(1, delta_len+1):
        for i in range(1,n+1):
            for j in range(i + 1, n+1):
                for k in range(j + 1, n+1):
                    cname1 = 'triangle_{0}_{1}_{2}_{3}'.format(i, j, k, t)
                    cname2 = 'triangle_{0}_{1}_{2}_{3}'.format(i, k, j, t)
                    cname3 = 'triangle_{0}_{1}_{2}_{3}'.format(j, k, i, t)
                    m.addConstr(m._vars[i, j, t] <= m._vars[i, k, t] + m._vars[j, k, t], cname1)
                    m.addConstr(m._vars[i, k, t] <= m._vars[i, j, t] + m._vars[j, k, t], cname2)
                    m.addConstr(m._vars[j, k, t] <= m._vars[i, j, t] + m._vars[i, k, t], cname3)
    m.update()
    end = time.time()
    print('Time to add triangle constraints = {0}'.format(end - start))

def triangle_constraints(m):
    # Lazy triangle inequality
    n = m._n
    tol = 1e-5
    flag = False
    for i in range(1,n+1):
        for j in range(i + 1, n+1):
            for t in range(1,delta_len+1):
                v_ijt = m.getVarByName('x_' + str(i) + '_' + str(j) + '_' + str(t))
                x_ijt = m.cbGetSolution(v_ijt)
                for k in range(j + 1, n+1):
                    v_ikt = m.getVarByName('x_' + str(i) + '_' + str(k) + '_' + str(t))
                    v_jkt = m.getVarByName('x_' + str(j) + '_' + str(k) + '_' + str(t))
                    x_ikt = m.cbGetSolution(v_ikt)
                    x_jkt = m.cbGetSolution(v_jkt)
                    if x_ijt > x_ikt + x_jkt and abs(x_ijt - x_ikt - x_jkt) > tol:
                        flag = True
                        m.cbLazy(v_ikt + v_jkt >= v_ijt)
                    if x_ikt > x_ijt + x_jkt and abs(x_ikt - x_ijt - x_jkt) > tol:
                        flag = True
                        m.cbLazy(v_ijt + v_jkt >= v_ikt)
                    if x_jkt > x_ijt + x_ikt and abs(x_jkt - x_ijt - x_ikt) > tol:
                        flag = True
                        m.cbLazy(v_ijt + v_ikt >= v_jkt)
    return flag

def callback_function(m, where):
    # First cut off triangle, then spreading
    if where == gp.GRB.Callback.MIPSOL:
        triangle_constraints(m)
        #nontrivial_constraints(m)

def init_model(nn,delta):
    # Initiliaze Gurobi model
    m = gp.Model("Model")
    m._n = nn
    # Add variables
    add_variables(m,delta)
    # Add layer constraints
    add_layer_constraints(m)
	# Add triangle constraints 
    #add_triangle_constraints(m)
    #if not triangle:
    #    add_triangle_spreading_constraints(m)
    m.modelSense = gp.GRB.MINIMIZE
    m.params.LazyConstraints = 1
    return m

m = init_model(n,delta)

m.update()


m.params.LogToConsole = 0
m.params.OutputFlag = 1
logfilename = 'result_gurobi/' + str(n) + '/' + base + '.log'
m.params.LogFile = logfilename
#lp_name = 'result_gurobi/' + str(n) + '/' + base + '.lp'
#m.write(lp_name)


start = time.time()
print('Optimizing over model with input',base)
#flag =True
#count=0
#while flag and time.time() - start < 7200:
#    count=count+1
#    print("count = {}, Time_diff = {}".format(count,time.time() - start))
#    
#    m.optimize()
#
#    flag = triangle_constraints(m)
m.optimize(callback_function)
end = time.time()
print('Total time to optimize = {0}'.format(end - start))

val_opt = m.ObjVal-tot
#print("-------------------------------------------------------------")
#m.printAttr('X')
#v=m.getVars()
#print(len(v))
#for i in range(len(v)):
#    print(v[i].varName,v[i].x)
outfilename = 'result_gurobi/' + str(n) + '/' + base + '.out'
f = open(outfilename, 'w')
f.write(str(val_opt))
f.close()
print(m.ObjVal," ",val_opt, "diff=",m.ObjVal-val_opt)
