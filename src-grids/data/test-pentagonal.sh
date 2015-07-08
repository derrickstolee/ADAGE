#!/bin/bash


GRID=pentagonal
GRIDSHORTHAND=pentagonal
RULEFILE=rules-pentagonal.txt


RULESHORTHAND=v1
RULES=--v1


PROBLEM=dominating

echo $GRIDSHORTHAND $PROBLEM $RULESHORTHAND
time ../grid-cplex.exe --$GRID --$PROBLEM --rules $RULEFILE $RULES --simplex --exact  >& output-$GRID-$PROBLEM-$RULESHORTHAND.txt



RULESHORTHAND=nearby
RULES=--nearby


PROBLEM=identifying

echo $GRIDSHORTHAND $PROBLEM $RULESHORTHAND
time ../grid-cplex.exe --$GRID --$PROBLEM --rules $RULEFILE $RULES --simplex --exact  >& output-$GRID-$PROBLEM-$RULESHORTHAND.txt



RULESHORTHAND=v2
RULES=--v2


PROBLEM=strongidentifying

echo $GRIDSHORTHAND $PROBLEM $RULESHORTHAND
time ../grid-cplex.exe --$GRID --$PROBLEM --rules $RULEFILE $RULES --simplex --exact  >& output-$GRID-$PROBLEM-$RULESHORTHAND.txt


RULESHORTHAND=v2
RULES=--v2


PROBLEM=locating

echo $GRIDSHORTHAND $PROBLEM $RULESHORTHAND
time ../grid-cplex.exe --$GRID --$PROBLEM --rules $RULEFILE $RULES --simplex --exact  >& output-$GRID-$PROBLEM-$RULESHORTHAND.txt





RULESHORTHAND=d2r2
RULES=--d1r2\ --d2r2


PROBLEM=openneighborhood

echo $GRIDSHORTHAND $PROBLEM $RULESHORTHAND
time ../grid-cplex.exe --$GRID --$PROBLEM --rules $RULEFILE $RULES --simplex --exact  >& output-$GRID-$PROBLEM-$RULESHORTHAND.txt


# PROBLEM=neighbor

# echo $GRIDSHORTHAND $PROBLEM $RULESHORTHAND
# time ./grid-cplex.exe --$GRID --$PROBLEM --rules $RULEFILE $RULES --simplex --exact --maxconstraints 1500000 >& output-$GRID-$PROBLEM-$RULESHORTHAND.txt


# PROBLEM=openneighborhood

# echo $GRIDSHORTHAND $PROBLEM $RULESHORTHAND
# time ./grid-cplex.exe --$GRID --$PROBLEM --rules $RULEFILE $RULES --simplex --exact --maxconstraints 1500000 >& output-$GRID-$PROBLEM-$RULESHORTHAND.txt


# PROBLEM=locating

# echo $GRIDSHORTHAND $PROBLEM $RULESHORTHAND
# time ./grid-cplex.exe --$GRID --$PROBLEM --rules $RULEFILE $RULES --simplex --exact --maxconstraints 1500000 >& output-$GRID-$PROBLEM-$RULESHORTHAND.txt


# PROBLEM=identifying

# echo $GRIDSHORTHAND $PROBLEM $RULESHORTHAND
# time ./grid-cplex.exe --$GRID --$PROBLEM --rules $RULEFILE $RULES --simplex --exact --maxconstraints 1500000 >& output-$GRID-$PROBLEM-$RULESHORTHAND.txt


# PROBLEM=strongidentifying

# echo $GRIDSHORTHAND $PROBLEM $RULESHORTHAND
# time ./grid-cplex.exe --$GRID --$PROBLEM --rules $RULEFILE $RULES --simplex --exact --maxconstraints 1500000 >& output-$GRID-$PROBLEM-$RULESHORTHAND.txt




# RULESHORTHAND=d1r2
# RULES=--d1r2


# PROBLEM=dominating

# echo $GRIDSHORTHAND $PROBLEM $RULESHORTHAND
# echo "./grid-cplex.exe --$GRID --$PROBLEM --rules $RULEFILE $RULES --simplex --exact --maxconstraints 1500000 >& output-$GRID-$PROBLEM-$RULESHORTHAND.txt"
# echo
# time ./grid-cplex.exe --$GRID --$PROBLEM --rules $RULEFILE $RULES --simplex --exact --maxconstraints 1500000 >& output-$GRID-$PROBLEM-$RULESHORTHAND.txt


# PROBLEM=neighbor

# echo $GRIDSHORTHAND $PROBLEM $RULESHORTHAND
# time ./grid-cplex.exe --$GRID --$PROBLEM --rules $RULEFILE $RULES --simplex --exact --maxconstraints 1500000 >& output-$GRID-$PROBLEM-$RULESHORTHAND.txt


# PROBLEM=openneighborhood

# echo $GRIDSHORTHAND $PROBLEM $RULESHORTHAND
# time ./grid-cplex.exe --$GRID --$PROBLEM --rules $RULEFILE $RULES --simplex --exact --maxconstraints 1500000 >& output-$GRID-$PROBLEM-$RULESHORTHAND.txt


# PROBLEM=locating

# echo $GRIDSHORTHAND $PROBLEM $RULESHORTHAND
# time ./grid-cplex.exe --$GRID --$PROBLEM --rules $RULEFILE $RULES --simplex --exact --maxconstraints 1500000 >& output-$GRID-$PROBLEM-$RULESHORTHAND.txt


# PROBLEM=identifying

# echo $GRIDSHORTHAND $PROBLEM $RULESHORTHAND
# time ./grid-cplex.exe --$GRID --$PROBLEM --rules $RULEFILE $RULES --simplex --exact --maxconstraints 1500000 >& output-$GRID-$PROBLEM-$RULESHORTHAND.txt


# PROBLEM=strongidentifying

# echo $GRIDSHORTHAND $PROBLEM $RULESHORTHAND
# time ./grid-cplex.exe --$GRID --$PROBLEM --rules $RULEFILE $RULES --simplex --exact --maxconstraints 1500000 >& output-$GRID-$PROBLEM-$RULESHORTHAND.txt


# RULESHORTHAND=d2r2
# RULES=--d2r2\ --d1r2


# PROBLEM=dominating

# echo $GRIDSHORTHAND $PROBLEM $RULESHORTHAND
# time ./grid-cplex.exe --$GRID --$PROBLEM --rules $RULEFILE $RULES  --maxconstraints 1500000 >& output-$GRID-$PROBLEM-$RULESHORTHAND.txt


# PROBLEM=neighbor

# echo $GRIDSHORTHAND $PROBLEM $RULESHORTHAND
# time ./grid-cplex.exe --$GRID --$PROBLEM --rules $RULEFILE $RULES  --maxconstraints 1500000 >& output-$GRID-$PROBLEM-$RULESHORTHAND.txt


# PROBLEM=openneighborhood

# echo $GRIDSHORTHAND $PROBLEM $RULESHORTHAND
# time ./grid-cplex.exe --$GRID --$PROBLEM --rules $RULEFILE $RULES  --maxconstraints 1500000 >& output-$GRID-$PROBLEM-$RULESHORTHAND.txt


# PROBLEM=locating

# echo $GRIDSHORTHAND $PROBLEM $RULESHORTHAND
# time ./grid-cplex.exe --$GRID --$PROBLEM --rules $RULEFILE $RULES --maxconstraints 1500000 >& output-$GRID-$PROBLEM-$RULESHORTHAND.txt


# PROBLEM=identifying

# echo $GRIDSHORTHAND $PROBLEM $RULESHORTHAND
# time ./grid-cplex.exe --$GRID --$PROBLEM --rules $RULEFILE $RULES --maxconstraints 1500000 >& output-$GRID-$PROBLEM-$RULESHORTHAND.txt


# PROBLEM=strongidentifying

# echo $GRIDSHORTHAND $PROBLEM $RULESHORTHAND
# time ./grid-cplex.exe --$GRID --$PROBLEM --rules $RULEFILE $RULES --maxconstraints 1500000 >& output-$GRID-$PROBLEM-$RULESHORTHAND.txt





# RULESHORTHAND=face1
# RULES=--face1

# PROBLEM=dominating

# echo $GRIDSHORTHAND $PROBLEM $RULESHORTHAND
# time ./grid-cplex.exe --$GRID --$PROBLEM --rules $RULEFILE $RULES --maxconstraints 1500000 >& output-$GRID-$PROBLEM-$RULESHORTHAND.txt


# PROBLEM=neighbor

# echo $GRIDSHORTHAND $PROBLEM $RULESHORTHAND
# time ./grid-cplex.exe --$GRID --$PROBLEM --rules $RULEFILE $RULES --maxconstraints 1500000 >& output-$GRID-$PROBLEM-$RULESHORTHAND.txt


# PROBLEM=openneighborhood

# echo $GRIDSHORTHAND $PROBLEM $RULESHORTHAND
# time ./grid-cplex.exe --$GRID --$PROBLEM --rules $RULEFILE $RULES --maxconstraints 1500000 >& output-$GRID-$PROBLEM-$RULESHORTHAND.txt


# PROBLEM=locating

# echo $GRIDSHORTHAND $PROBLEM $RULESHORTHAND
# time ./grid-cplex.exe --$GRID --$PROBLEM --rules $RULEFILE $RULES --maxconstraints 1500000 >& output-$GRID-$PROBLEM-$RULESHORTHAND.txt


# PROBLEM=identifying

# echo $GRIDSHORTHAND $PROBLEM $RULESHORTHAND
# time ./grid-cplex.exe --$GRID --$PROBLEM --rules $RULEFILE $RULES --maxconstraints 1500000 >& output-$GRID-$PROBLEM-$RULESHORTHAND.txt


# PROBLEM=strongidentifying

# echo $GRIDSHORTHAND $PROBLEM $RULESHORTHAND
# time ./grid-cplex.exe --$GRID --$PROBLEM --rules $RULEFILE $RULES  --maxconstraints 1500000 >& output-$GRID-$PROBLEM-$RULESHORTHAND.txt


# RULESHORTHAND=d2face1
# RULES=--d1face1\ --d2face1

# PROBLEM=dominating

# echo $GRIDSHORTHAND $PROBLEM $RULESHORTHAND
# time ./grid-cplex.exe --$GRID --$PROBLEM --rules $RULEFILE $RULES --maxconstraints 1500000 >& output-$GRID-$PROBLEM-$RULESHORTHAND.txt


# PROBLEM=neighbor

# echo $GRIDSHORTHAND $PROBLEM $RULESHORTHAND
# time ./grid-cplex.exe --$GRID --$PROBLEM --rules $RULEFILE $RULES --maxconstraints 1500000 >& output-$GRID-$PROBLEM-$RULESHORTHAND.txt


# PROBLEM=openneighborhood

# echo $GRIDSHORTHAND $PROBLEM $RULESHORTHAND
# time ./grid-cplex.exe --$GRID --$PROBLEM --rules $RULEFILE $RULES --maxconstraints 1500000 >& output-$GRID-$PROBLEM-$RULESHORTHAND.txt


# PROBLEM=locating

# echo $GRIDSHORTHAND $PROBLEM $RULESHORTHAND
# time ./grid-cplex.exe --$GRID --$PROBLEM --rules $RULEFILE $RULES --maxconstraints 1500000 >& output-$GRID-$PROBLEM-$RULESHORTHAND.txt


# PROBLEM=identifying

# echo $GRIDSHORTHAND $PROBLEM $RULESHORTHAND
# time ./grid-cplex.exe --$GRID --$PROBLEM --rules $RULEFILE $RULES --maxconstraints 1500000 >& output-$GRID-$PROBLEM-$RULESHORTHAND.txt


# PROBLEM=strongidentifying

# echo $GRIDSHORTHAND $PROBLEM $RULESHORTHAND
# time ./grid-cplex.exe --$GRID --$PROBLEM --rules $RULEFILE $RULES  --maxconstraints 1500000 >& output-$GRID-$PROBLEM-$RULESHORTHAND.txt


RULESHORTHAND=nearbykernel
RULES=--nearbykernel

# PROBLEM=dominating

# echo $GRIDSHORTHAND $PROBLEM $RULESHORTHAND
# time ./grid-cplex.exe --$GRID --$PROBLEM --rules $RULEFILE $RULES --maxconstraints 1500000 >& output-$GRID-$PROBLEM-$RULESHORTHAND.txt


# PROBLEM=neighbor

# echo $GRIDSHORTHAND $PROBLEM $RULESHORTHAND
# time ./grid-cplex.exe --$GRID --$PROBLEM --rules $RULEFILE $RULES --maxconstraints 1500000 >& output-$GRID-$PROBLEM-$RULESHORTHAND.txt


# PROBLEM=openneighborhood

# echo $GRIDSHORTHAND $PROBLEM $RULESHORTHAND
# time ./grid-cplex.exe --$GRID --$PROBLEM --rules $RULEFILE $RULES --maxconstraints 1500000 >& output-$GRID-$PROBLEM-$RULESHORTHAND.txt


# PROBLEM=locating

# echo $GRIDSHORTHAND $PROBLEM $RULESHORTHAND
# time ./grid-cplex.exe --$GRID --$PROBLEM --rules $RULEFILE $RULES --maxconstraints 1500000 >& output-$GRID-$PROBLEM-$RULESHORTHAND.txt


# PROBLEM=identifying

# echo $GRIDSHORTHAND $PROBLEM $RULESHORTHAND
# time ./grid-cplex.exe --$GRID --$PROBLEM --rules $RULEFILE $RULES --maxconstraints 1500000 >& output-$GRID-$PROBLEM-$RULESHORTHAND.txt


# PROBLEM=strongidentifying

# echo $GRIDSHORTHAND $PROBLEM $RULESHORTHAND
# time ./grid-cplex.exe --$GRID --$PROBLEM --rules $RULEFILE $RULES  --maxconstraints 1500000 >& output-$GRID-$PROBLEM-$RULESHORTHAND.txt




RULESHORTHAND=nearby
RULES=--nearby


# PROBLEM=dominating

# echo $GRIDSHORTHAND $PROBLEM $RULESHORTHAND
# time ./grid-cplex.exe --$GRID --$PROBLEM --rules $RULEFILE $RULES --maxconstraints 1500000 >& output-$GRID-$PROBLEM-$RULESHORTHAND.txt


# PROBLEM=neighbor

# echo $GRIDSHORTHAND $PROBLEM $RULESHORTHAND
# time ./grid-cplex.exe --$GRID --$PROBLEM --rules $RULEFILE $RULES --maxconstraints 1500000 >& output-$GRID-$PROBLEM-$RULESHORTHAND.txt


# PROBLEM=openneighborhood

# echo $GRIDSHORTHAND $PROBLEM $RULESHORTHAND
# time ./grid-cplex.exe --$GRID --$PROBLEM --rules $RULEFILE $RULES --maxconstraints 1500000 >& output-$GRID-$PROBLEM-$RULESHORTHAND.txt


# PROBLEM=locating

# echo $GRIDSHORTHAND $PROBLEM $RULESHORTHAND
# time ./grid-cplex.exe --$GRID --$PROBLEM --rules $RULEFILE $RULES --maxconstraints 1500000 >& output-$GRID-$PROBLEM-$RULESHORTHAND.txt


# PROBLEM=identifying

# echo $GRIDSHORTHAND $PROBLEM $RULESHORTHAND
# time ./grid-cplex.exe --$GRID --$PROBLEM --rules $RULEFILE $RULES --maxconstraints 1500000 >& output-$GRID-$PROBLEM-$RULESHORTHAND.txt


# PROBLEM=strongidentifying

# echo $GRIDSHORTHAND $PROBLEM $RULESHORTHAND
# time ./grid-cplex.exe --$GRID --$PROBLEM --rules $RULEFILE $RULES  --maxconstraints 1500000 >& output-$GRID-$PROBLEM-$RULESHORTHAND.txt




RULESHORTHAND=v2
RULES=--v2

# PROBLEM=dominating

# echo $GRIDSHORTHAND $PROBLEM $RULESHORTHAND
# time ./grid-cplex.exe --$GRID --$PROBLEM --rules $RULEFILE $RULES --maxconstraints 1500000 >& output-$GRID-$PROBLEM-$RULESHORTHAND.txt


# PROBLEM=neighbor

# echo $GRIDSHORTHAND $PROBLEM $RULESHORTHAND
# time ./grid-cplex.exe --$GRID --$PROBLEM --rules $RULEFILE $RULES --maxconstraints 1500000 >& output-$GRID-$PROBLEM-$RULESHORTHAND.txt


# PROBLEM=openneighborhood

# echo $GRIDSHORTHAND $PROBLEM $RULESHORTHAND
# time ./grid-cplex.exe --$GRID --$PROBLEM --rules $RULEFILE $RULES --maxconstraints 1500000 >& output-$GRID-$PROBLEM-$RULESHORTHAND.txt


# PROBLEM=locating

# echo $GRIDSHORTHAND $PROBLEM $RULESHORTHAND
# time ./grid-cplex.exe --$GRID --$PROBLEM --rules $RULEFILE $RULES --maxconstraints 1500000 >& output-$GRID-$PROBLEM-$RULESHORTHAND.txt


# PROBLEM=identifying

# echo $GRIDSHORTHAND $PROBLEM $RULESHORTHAND
# time ./grid-cplex.exe --$GRID --$PROBLEM --rules $RULEFILE $RULES --maxconstraints 1500000 >& output-$GRID-$PROBLEM-$RULESHORTHAND.txt


# PROBLEM=strongidentifying

# echo $GRIDSHORTHAND $PROBLEM $RULESHORTHAND
# time ./grid-cplex.exe --$GRID --$PROBLEM --rules $RULEFILE $RULES  --maxconstraints 1500000 >& output-$GRID-$PROBLEM-$RULESHORTHAND.txt


RULESHORTHAND=face2
RULES=--face2

# PROBLEM=dominating

# echo $GRIDSHORTHAND $PROBLEM $RULESHORTHAND
# time ./grid-cplex.exe --$GRID --$PROBLEM --rules $RULEFILE $RULES --maxconstraints 1500000 >& output-$GRID-$PROBLEM-$RULESHORTHAND.txt


# PROBLEM=neighbor

# echo $GRIDSHORTHAND $PROBLEM $RULESHORTHAND
# time ./grid-cplex.exe --$GRID --$PROBLEM --rules $RULEFILE $RULES --maxconstraints 1500000 >& output-$GRID-$PROBLEM-$RULESHORTHAND.txt


# PROBLEM=openneighborhood

# echo $GRIDSHORTHAND $PROBLEM $RULESHORTHAND
# time ./grid-cplex.exe --$GRID --$PROBLEM --rules $RULEFILE $RULES --maxconstraints 1500000 >& output-$GRID-$PROBLEM-$RULESHORTHAND.txt


# PROBLEM=locating

# echo $GRIDSHORTHAND $PROBLEM $RULESHORTHAND
# time ./grid-cplex.exe --$GRID --$PROBLEM --rules $RULEFILE $RULES --maxconstraints 1500000 >& output-$GRID-$PROBLEM-$RULESHORTHAND.txt


# PROBLEM=identifying

# echo $GRIDSHORTHAND $PROBLEM $RULESHORTHAND
# time ./grid-cplex.exe --$GRID --$PROBLEM --rules $RULEFILE $RULES --maxconstraints 1500000 >& output-$GRID-$PROBLEM-$RULESHORTHAND.txt


# PROBLEM=strongidentifying

# echo $GRIDSHORTHAND $PROBLEM $RULESHORTHAND
# time ./grid-cplex.exe --$GRID --$PROBLEM --rules $RULEFILE $RULES  --maxconstraints 1500000 >& output-$GRID-$PROBLEM-$RULESHORTHAND.txt




# RULESHORTHAND=d2face2
# RULES=--d1face2\ --d2face1

# # PROBLEM=dominating

# # echo $GRIDSHORTHAND $PROBLEM $RULESHORTHAND
# # time ./grid-cplex.exe --$GRID --$PROBLEM --rules $RULEFILE $RULES --maxconstraints 1500000 >& output-$GRID-$PROBLEM-$RULESHORTHAND.txt


# # PROBLEM=neighbor

# # echo $GRIDSHORTHAND $PROBLEM $RULESHORTHAND
# # time ./grid-cplex.exe --$GRID --$PROBLEM --rules $RULEFILE $RULES --maxconstraints 1500000 >& output-$GRID-$PROBLEM-$RULESHORTHAND.txt


# # PROBLEM=openneighborhood

# # echo $GRIDSHORTHAND $PROBLEM $RULESHORTHAND
# # time ./grid-cplex.exe --$GRID --$PROBLEM --rules $RULEFILE $RULES --maxconstraints 1500000 >& output-$GRID-$PROBLEM-$RULESHORTHAND.txt


# PROBLEM=locating

# echo $GRIDSHORTHAND $PROBLEM $RULESHORTHAND
# time ./grid-cplex.exe --$GRID --$PROBLEM --rules $RULEFILE $RULES --maxconstraints 1500000 >& output-$GRID-$PROBLEM-$RULESHORTHAND.txt


# PROBLEM=identifying

# echo $GRIDSHORTHAND $PROBLEM $RULESHORTHAND
# time ./grid-cplex.exe --$GRID --$PROBLEM --rules $RULEFILE $RULES --maxconstraints 1500000 >& output-$GRID-$PROBLEM-$RULESHORTHAND.txt


# PROBLEM=strongidentifying

# echo $GRIDSHORTHAND $PROBLEM $RULESHORTHAND
# time ./grid-cplex.exe --$GRID --$PROBLEM --rules $RULEFILE $RULES  --maxconstraints 1500000 >& output-$GRID-$PROBLEM-$RULESHORTHAND.txt


RULESHORTHAND=nearbyplus
RULES=--nearbyplus

# PROBLEM=dominating

# echo $GRIDSHORTHAND $PROBLEM $RULESHORTHAND
# time ./grid-cplex.exe --$GRID --$PROBLEM --rules $RULEFILE $RULES --maxconstraints 1500000 >& output-$GRID-$PROBLEM-$RULESHORTHAND.txt


# PROBLEM=neighbor

# echo $GRIDSHORTHAND $PROBLEM $RULESHORTHAND
# time ./grid-cplex.exe --$GRID --$PROBLEM --rules $RULEFILE $RULES --maxconstraints 1500000 >& output-$GRID-$PROBLEM-$RULESHORTHAND.txt


# PROBLEM=openneighborhood

# echo $GRIDSHORTHAND $PROBLEM $RULESHORTHAND
# time ./grid-cplex.exe --$GRID --$PROBLEM --rules $RULEFILE $RULES --maxconstraints 1500000 >& output-$GRID-$PROBLEM-$RULESHORTHAND.txt


PROBLEM=locating

# echo $GRIDSHORTHAND $PROBLEM $RULESHORTHAND
# time ./grid-cplex.exe --$GRID --$PROBLEM --rules $RULEFILE $RULES --maxconstraints 1500000 >& output-$GRID-$PROBLEM-$RULESHORTHAND.txt


# PROBLEM=identifying

# echo $GRIDSHORTHAND $PROBLEM $RULESHORTHAND
# time ./grid-cplex.exe --$GRID --$PROBLEM --rules $RULEFILE $RULES --maxconstraints 1500000 >& output-$GRID-$PROBLEM-$RULESHORTHAND.txt


# PROBLEM=strongidentifying

# echo $GRIDSHORTHAND $PROBLEM $RULESHORTHAND
# time ./grid-cplex.exe --$GRID --$PROBLEM --rules $RULEFILE $RULES  --maxconstraints 1500000 >& output-$GRID-$PROBLEM-$RULESHORTHAND.txt



RULESHORTHAND=nearbyplus2
RULES=--nearbyplus2

# PROBLEM=dominating

# echo $GRIDSHORTHAND $PROBLEM $RULESHORTHAND
# time ./grid-cplex.exe --$GRID --$PROBLEM --rules $RULEFILE $RULES --maxconstraints 1500000 >& output-$GRID-$PROBLEM-$RULESHORTHAND.txt


# PROBLEM=neighbor

# echo $GRIDSHORTHAND $PROBLEM $RULESHORTHAND
# time ./grid-cplex.exe --$GRID --$PROBLEM --rules $RULEFILE $RULES --maxconstraints 1500000 >& output-$GRID-$PROBLEM-$RULESHORTHAND.txt


# PROBLEM=openneighborhood

# echo $GRIDSHORTHAND $PROBLEM $RULESHORTHAND
# time ./grid-cplex.exe --$GRID --$PROBLEM --rules $RULEFILE $RULES --maxconstraints 1500000 >& output-$GRID-$PROBLEM-$RULESHORTHAND.txt


# PROBLEM=locating

# echo $GRIDSHORTHAND $PROBLEM $RULESHORTHAND
# time ./grid-cplex.exe --$GRID --$PROBLEM --rules $RULEFILE $RULES --maxconstraints 1500000 >& output-$GRID-$PROBLEM-$RULESHORTHAND.txt


# PROBLEM=identifying

# echo $GRIDSHORTHAND $PROBLEM $RULESHORTHAND
# time ./grid-cplex.exe --$GRID --$PROBLEM --rules $RULEFILE $RULES --maxconstraints 1500000 >& output-$GRID-$PROBLEM-$RULESHORTHAND.txt


# PROBLEM=strongidentifying

# echo $GRIDSHORTHAND $PROBLEM $RULESHORTHAND
# time ./grid-cplex.exe --$GRID --$PROBLEM --rules $RULEFILE $RULES  --maxconstraints 1500000 >& output-$GRID-$PROBLEM-$RULESHORTHAND.txt




