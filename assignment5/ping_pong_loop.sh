powers_of_two=(0 1 2 3 4 5 6 7 8)
for i in "${powers_of_two[@]}"
do
    srun -n 2 $SRUN_CONFIG ./pingpong $i
done