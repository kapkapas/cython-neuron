
import numpy as np
import scipy as sp
import pylab as pl

T_sim = 1000.

datapath = 'data/'


##################################
# Data from Blue Gene (Tobias)
##################################

vps_s550 = np.array([ 4096,  8192, 16384, 32768])

# (d.h. exakt ist N=6.1875*106, K/N=11250)
st_s550_nosd = np.array([334.7227783203121, 206.14506835937405, 139.53292663574103, 108.56503112793136])

vps_s100 = np.array([ 1024,  2048,  4096,  8192, 16384])

# (d.h. exakt ist N=1.125*106, K/N=11250)
st_s100_nosd = np.array([195.45021484375002, 114.33687500000008, 70.624736328125095, 50.796391601562469, 49.783812255859395])

##################################
# Data from Kei
##################################


#N_procs = np.array([8, 16, 32, 64])
N_Nodes = [2**k for k in range(8, 11)] + [1250, 1536]
n_threads_per_proc = 8
N_procs = [N * n_threads_per_proc for N in N_Nodes]

# plotting style
markersize = 20.


def get_data_old(N_procs, filedir, filestem, n_files_try):

  runtime_sim = []
  runtime_setup = []
  n_procs = []
  mem = []

  for i, N_proc in enumerate(N_procs):

    basedir = filedir + str(N_proc)
    print "loading ", basedir
    rt_sim_total = 0.
    rt_setup_total = 0.
    mem_tot = 0.
    n_files = 0
    for p in xrange(n_files_try):
      runtime_name = basedir + '/' + filestem + str(p) + '.dat'
      try:
        rt = pl.loadtxt(runtime_name)
        rt_setup_total += rt[0,2]
        rt_sim_total += rt[1,2]
        n_files += 1
      except:
        print "ignoring file ", runtime_name

    # was there at least one file?
    if n_files > 0:
      runtime_sim.append( rt_sim_total / n_files )
      runtime_setup.append( rt_setup_total / n_files )
      mem.append( mem_tot / n_files )
      n_procs.append(N_proc)

  return n_procs, runtime_sim, runtime_setup, mem


def get_data(N_procs, filedir, filestem, n_files_try):

  runtime_sim = []
  runtime_setup = []
  n_procs = []
  mem = []

  for i, N_proc in enumerate(N_procs):

    basedir = filedir + str(N_proc)
    print "loading ", basedir
    rt_sim_total = 0.
    rt_setup_total = 0.
    mem_tot = 0.
    n_files = 0
    for p in xrange(n_files_try):
      runtime_name = basedir + '/' + filestem + str(p) + '.dat'
      try:
        rt = pl.loadtxt(runtime_name)
        rt_setup_total += rt[4,2] + rt[5,2]
        rt_sim_total += rt[13,2]
        mem_tot += rt[11, 2]
        n_files += 1
      except:
        print "ignoring file ", runtime_name

    # was there at least one file?
    if n_files > 0:
      runtime_sim.append( rt_sim_total / n_files )
      runtime_setup.append( rt_setup_total / n_files )
      mem.append( mem_tot / n_files )
      n_procs.append(N_proc)

  return n_procs, runtime_sim, runtime_setup, mem

# simulated in /work/user0049/tobias_benchmark_ohno
threaded_n, threaded_sim, threaded_setup, threaded_mem = get_data_old(N_procs, 'data/sim_openmp_N', 'runtimes_', 30)

# simulated in /work/user0049/tobias_benchmark
threaded_new_n, threaded_new_sim, threaded_new_setup, threaded_new_mem = get_data(N_procs, 'data/sim_openmp_N', 'logfile_', 30)


threaded_all_n = threaded_n + threaded_new_n
threaded_all_sim = threaded_sim + threaded_new_sim
threaded_all_setup = threaded_setup + threaded_new_setup
threaded_all_mem = threaded_mem + threaded_new_mem

print threaded_all_n
print threaded_all_sim
print threaded_all_setup
print threaded_all_mem

# results in
# threaded_n = [2048, 4096, 8192, 12288, 10000]
# threaded_sim = [75.806999999999974, 49.205666666666694, 41.579666666666661, 101.90300000000001, 48.616666666666639]
# threaded_setup = [1.6413333333333333, 1.637333333333334, 1.6363333333333339, 1.639, 329.3893333333333]
# threaded_mem = [0.0, 0.0, 0.0, 0.0, 4427319.4666666668]


# simulated in /work/user0049/tobias_benchmark_ohno
flat_n, flat_sim, flat_setup, flat_mem = get_data_old(N_procs, 'data/sim_flatmpi_N', 'runtimes_', 30)

# results in
# flat_sim = [77.902666666666704, 65.022666666666652, 84.303333333333327, 112.70833333333333]


print flat_sim
print flat_setup
print flat_mem

pl.figure(1)
pl.clf()
pl.title(r'NEST trunk : stdp_bm.sli : $N = 10^6, K = 10^4, T = 1 s$')
pl.loglog(threaded_all_n, threaded_all_sim, '.', markersize=markersize, label='Kei 8 threads hybrid')
pl.loglog(flat_n, flat_sim, '.', markersize=markersize, label='Kei flat mpi')
pl.loglog(vps_s100, st_s100_nosd, '.', markersize=markersize, label='Blue Gene')
pl.xticks([1024, 2048, 4096, 8192, 12288, 16384], [r'$1024$', r'$2048$', r'$4096$', r'$8192$', r'$12288$', r'$16384$'])
pl.yticks([50, 100, 200], [r'$50$', r'$100$', r'$200$'])
pl.xlim([512, 32768])
pl.ylim([30, 250])
pl.ylabel(r'simulation time (s)')
pl.xlabel(r'number of cores')
pl.legend()
pl.savefig('sim_K_stdp_bm.eps')
pl.savefig('sim_K_stdp_bm.pdf')

#pl.figure(2)
#pl.clf()
#pl.title(r'NEST trunk : stdp_bm.sli : $N = 10^6, K = 10^4, T = 1 s$')
#pl.semilogx(threaded_n, np.array(flat_sim)/np.array(threaded_sim), '.', markersize=markersize)
#pl.ylabel(r'gain hybrid/flat MPI')
#pl.xticks([1024, 2048, 4096, 8192, 12288, 16384], [r'$1024$', r'$2048$', r'$4096$', r'$8192$', r'$12288$', r'$16384$'])
#pl.xlim([512, 32768])
#pl.xlabel(r'number of cores')
#pl.legend()
#pl.savefig('gain_K_stdp_bm.eps')
#pl.savefig('gain_K_stdp_bm.pdf')

pl.figure(3)
pl.clf()
pl.title('setup time (min)')
pl.loglog(threaded_n, np.array(threaded_setup)/60., '.', markersize=markersize, label='Kei 8 threads hybrid')
pl.loglog(flat_n, np.array(flat_setup)/60., '.', markersize=markersize, label='Kei flat MPI')
pl.xticks([1024, 2048, 4096, 8192, 12288, 16384], [r'$1024$', r'$2048$', r'$4096$', r'$8192$', r'$12288$', r'$16384$'])
pl.xticks([2., 4., 8., 16., 32, 64.], [r'$2$', r'$4$', r'$8$', r'$16$', r'$32$', r'$64$'])
pl.xlim([512, 32768])
pl.ylabel(r'setup time')
pl.xlabel(r'number of cores')
pl.legend()

pl.show()

