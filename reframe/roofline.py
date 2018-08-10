# jg/mkr (cscs)
# requires reframe version>=2.14-dev1 (build_system) !!!!
# ~/reframe.git/reframe.py --exec-policy async -C ~/reframe.git/config/cscs.py \
#   --system dom:mc --keep-stage-files --prefix=$SCRATCH/reframe -r -c roofline.py

import os

import reframe as rfm
import reframe.utility.sanity as sn
from reframe.core.fields import ScopedDict

@rfm.parameterized_test(['C++', 5])
#@rfm.parameterized_test(['C++', 0], ['C++', 1], ['C++', 2], ['C++', 3], ['C++', 4], ['C++', 5])
class Roofline(rfm.RegressionTest):
    def __init__(self, lang, codeversion):
        super().__init__()

        advstep1 = 'advixe-cl -verbose -collect survey              -project-dir=$SCRATCH/roofline/ver%s -data-limit=0 -- ' % codeversion
        advstep2 = 'advixe-cl -verbose -collect tripcounts -flop    -project-dir=$SCRATCH/roofline/ver%s -data-limit=0 -- ' % codeversion # depends on survey!
        advstep3 = 'advixe-cl -verbose -collect map                 -project-dir=$SCRATCH/roofline/ver%s -data-limit=0 -- ' % codeversion # depends on survey!
        advstep4 = 'advixe-cl -verbose -collect dependencies -mark-up-list=1,4 -project-dir=$SCRATCH/roofline/ver%s -data-limit=0 -- ' % codeversion # depends on survey!
        #advstep3 = 'advixe-cl -verbose -collect map -mark-up-list=3 -project-dir=$SCRATCH/roofline/ver%sm              -- ' % codeversion
        # intel bug? : same src but "different" exe in different dir will fail (gflop=0), even with --ignore-app-mismatch ...

        self.name = 'roofline_%s_ver%s' % (lang.replace('+', 'p'), codeversion)
        self.descr = '%s ver%s check' % (lang, codeversion)
        self.codeversion = codeversion
        self.language = lang
        self.valid_systems = ['daint:mc', 'dom:mc']

        self.valid_prog_environs = ['PrgEnv-intel']
        #self.valid_prog_environs = ['PrgEnv-cray', 'PrgEnv-gnu',
        #                            'PrgEnv-intel', 'PrgEnv-pgi']

        self.prgenv_flags = {
            'PrgEnv-intel': '-O2 -g -std=c++14',
            'PrgEnv-gnu': '-O2 -g -std=c++14',
            'PrgEnv-cray': '-O2 -g -hnoomp -hstd=c++14',
            'PrgEnv-pgi': '-O2 -g -std=c++11' # -std=c++14: undefined reference to operator delete
        }

        self.haswell_flags = {
            'PrgEnv-intel': '-xCORE-AVX2',
            'PrgEnv-gnu': '-march=core-avx2',
            'PrgEnv-cray': '-hcpu=haswell',
            'PrgEnv-pgi': '-tp=haswell'
        }

        self.broadwell_flags = {
            'PrgEnv-intel': '-xCORE-AVX2',
            'PrgEnv-gnu': '-march=broadwell',
            'PrgEnv-cray': '-hcpu=broadwell',
            'PrgEnv-pgi': '-tp=haswell' # pgi/18.5.0
        }

        self.sourcesdir = os.path.join('src', lang)
        self.build_system = 'Make'
        self.build_system.options = ['VER=%s' % codeversion]

        self.executable = '%s $PWD/exe 2>&1' % advstep1
        #self.executable = '%s $PWD/exe 2000 5 2>&1' % advstep3
        self.exclusive = True            # --exclusive
        self.num_tasks = 1               # --ntasks
        self.num_tasks_per_node = 1      # --ntasks-per-node
        self.num_cpus_per_task = 1       # --cpus-per-task
        self.num_tasks_per_core = 1      # --ntasks-per-core
        self.use_multithreading = False  # --hint=nomultithread
        self.time_limit = (0,10,0)        # --time= : tool adds some overhead

        self.variables = {
            'OMP_NUM_THREADS': str(self.num_cpus_per_task),
            'CRAYPE_LINK_TYPE': 'dynamic',  # mandatory
        }

        #self.pre_run = ['module rm craype-broadwell']
        #self.pre_run += ['unset CRAY_CPU_TARGET']
        self.pre_run  = ['source /apps/common/UES/intel/2019/advisor_2019/advixe-vars.sh ;echo 2>&1']
        self.pre_run += ['module list -t']
        self.pre_run += ['rm -fr $SCRATCH/roofline/ver%s' % codeversion]

        self.post_run  = ['srun %s $PWD/exe 2>&1' % advstep2]
        self.post_run += ['srun %s $PWD/exe 1000 4 2>&1' % advstep3]
        self.post_run += ['srun %s $PWD/exe 1000 4 2>&1' % advstep4]
        self.post_run += ['echo advixe-gui $SCRATCH/roofline/ver%s/' % codeversion]

        self.maintainers = ['JG', 'MKR']
        self.tags = {'production'}

    def compile(self):
        #print('### codeversion=%s %s' % (self.codeversion,type(self.codeversion)) )
        if self.codeversion == 0:
            self.build_system.options += ['NOCPU=1 SIMDFLAGS= ']
        else:
            self.build_system.options += ['SIMDFLAGS=%s' % (self.broadwell_flags[self.current_environ.name])]
            #self.build_system.options += ['SIMDFLAGS=-xCORE-AVX2']
        #prgenv_flags = self.prgenv_flags[self.current_environ.name]
        #self.current_environ.cxxflags = prgenv_flags
        #self.current_environ.ldflags = '-lm '
        super().compile()

    def setup(self, partition, environ, **job_opts):
        super().setup(partition, environ, **job_opts)

        self.job.launcher.options  = ['--unbuffered']
        self.job.launcher.options += ['--cpu_bind=verbose']
        #self.job.launcher.options += ['--ntasks=$SLURM_NTASKS']
        #self.job.launcher.options += ['--ntasks-per-node=$SLURM_NTASKS_PER_NODE']
        #self.job.launcher.options += ['--cpus-per-task=$SLURM_CPUS_PER_TASK']
        #self.job.launcher.options += ['--ntasks-per-core=$SLURM_NTASKS_PER_CORE']
        environ_name = self.current_environ.name

        self.sanity_patterns = sn.all([
            sn.assert_found('advixe: Collection started.', self.stdout),
            sn.assert_found('advixe: Collection stopped.', self.stdout),
        ])

#	 Initialize Gravity Simulation                  
#	 nPart = 16000; nSteps = 10; dt = 0.1           
#	------------------------------------------------
#	 s       dt      kenergy     time (s)    GFlops 
#	------------------------------------------------
#	 1       0.1     26.405      1.4236      5.2153 
#	 2       0.2     313.77      1.4336      5.1786 
#	 3       0.3     926.56      1.431       5.1883 
#	 4       0.4     1866.4      1.4308      5.1891 
#	 5       0.5     3135.6      1.4321      5.1842 
#	 6       0.6     4737.6      1.4309      5.1886 
#	 7       0.7     6676.6      1.4307      5.1893 
#	 8       0.8     8957.7      1.4303      5.1907 
#	 9       0.9     11587       1.43        5.1919 
#	 10      1       14572       1.4299      5.1921 
#	                                                
#	# Number Threads      : 1                       
#	# Total Time (s)      : 14.303                  
#	# Average Performance : 5.1893 +- 0.0023365     

