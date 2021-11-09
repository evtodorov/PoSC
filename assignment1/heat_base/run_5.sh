sed -i "s/run_1/run_5/g" job_flags.scp
sbatch job_flags.scp & PIDJOB=$!
wait $PIDJOB
