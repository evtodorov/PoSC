sed -i "s/run_1/run_2/g" job_flags.scp
sbatch job_flags.scp & PIDJOB=$!
wait $PIDJOB
