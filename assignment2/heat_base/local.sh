export PAPI_EVENTS="PAPI_L2_TCM, PAPI_L2_TCA, PAPI_L3_TCM, PAPI_L3_TCA"
#export PAPI_EVENTS="PAPI_VEC_DP"
export PAPI_OUTPUT_DIRECTORY=~/pos_21_group12/assignment2/heat_base/papi_reports
export PAPI_REPORT = 1

./heat test.dat

