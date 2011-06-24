
import os
import glob

# needs to be adapted
#nest_install = '/work/user0049/10kproject_install/bin/'
nest_install = '/work/user0049/nest2_openmp_install/bin/'
sli_script = 'stdp_bm.sli'

# duration of simulation
T_sim = 1000.

Nrn_per_core = [112, 225, 550]

# N_Nodes = ['1x1', '2x1', '2x2', '2x2x2']
N_Nodes = [2**k for k in range(5, 11)] + [1536]
#N_Nodes = [2**k for k in range(1,2)]
threads_per_node = 8

# must be at least 10 minutes, maximum 60

expected_time = 60 # min

N_procs = [N * threads_per_node for N in N_Nodes]

for j, N_per_core in enumerate(Nrn_per_core):

       for i, N_proc in enumerate(N_procs):

              scale = int(N_proc * N_per_core / 11250)

              base_dir = 'sim_openmp_weak_trunk_NPC'+str(N_per_core)+'_N' + str(N_proc)
              batch_name = 'stdp_scale_'+str(N_proc)+'.sh'

              os.system('mkdir -p ' + base_dir)
              os.system('rm -f ' + base_dir + '/' + batch_name + '*')

              # write a param conf file for the simulation
              # % parameters defined in param.conf.sli:
              # % nvp - number of virtual processes
              # % Tsim - total simulation time in ms
              # % recto - argument for /record_to variable of spike detectors
              # % path_name - path where all files will have to be written
              # % the file is usually created by a bash script
              f = open(base_dir + '/param.conf.sli', 'w')
              f.write(str(N_proc) + ' /nvp Set\n')
              f.write(str(T_sim) + ' /Tsim Set\n')
              f.write(str(scale) + ' /scale Set\n')
              f.write('(file) /recto Set\n')
              f.write('(.) /path_name Set\n')
              f.write('false /rtf Set\n')         # do not record spikes
              f.write('(logfile) /log_file Set\n')
              f.close()
 
              # copy simulation script to location
              os.system('cp *.sli ' + base_dir)

              # write a batch script and submit it

              f = open(base_dir + '/' + batch_name, 'w')
              f.write('#!/bin/bash -x\n')
              f.write('#PJM --rsc-list "node='+str(N_Nodes[i])+'"\n')
              f.write('#PJM --rsc-list "elapse=00:'+str(expected_time)+':00"\n') # maximum time
              f.write('#PJM --mpi "proc='+str(N_Nodes[i])+'"\n')
              f.write('#PJM -s\n')
              f.write('export OMP_NUM_THREADS=8\n') # check this syntax
              f.write('. /work/system/Env_base\n')

              f.write('time mpiexec lpgparm -s 4MB -d 4MB -h 4MB -t 4MB -p 4MB ' + nest_install + '/nest ' + sli_script + '\n')

              f.close()

              os.system('cd ' + base_dir + '; pjsub ' + batch_name)


