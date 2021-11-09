array=("-O2")

for i in "${array[@]}"; do
	make clean
	echo "$i"
	sed -i "s/^CFLAGS.*/CFLAGS = $i/" Makefile
	sed -i "s/^#SBATCH -o.*/#SBATCH -o run_1/job_${i// /_}.out/" job_flags.scp
	sed -i "s/^#SBATCH -e.*/#SBATCH -e run_1/job_${i// /_}.out/" job_flags.scp
	make
	sbatch job_flags.scp & PIDJOB=$!
	wait $PIDJOB
done
