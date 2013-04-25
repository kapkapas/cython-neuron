import sys

cdef class SpecialFunctions:
    def __cinit__(self):
        pass
        
    def __dealloc__(self):
        pass

    cdef double (*get_msFct)(int, long, double)
    cdef long (*get_tics_or_stepsFct)(int, int, long, double)
    cdef unsigned int (*get_scheduler_valueFct)(int, unsigned int)


cdef SpecialFunctions spFct = SpecialFunctions()

# base class
class Neuron:
    def __cinit__(self):
        pass

    def __dealloc__(self):
        pass

    def update(self):
        pass

    def calibrate(self):
        pass

    def setStatus(self):
        pass

    def get_ms_on_resolution(self):
        return spFct.get_msFct(0, -1, -1)

    def get_ms_on_tics(self, tics):
        return spFct.get_msFct(1, tics, -1)

    def get_ms_on_steps(self, steps):
        return spFct.get_msFct(2, steps, -1)

    def get_tics_on_resolution(self):
        return spFct.get_tics_or_stepsFct(0, 1, -1, -1)

    def get_tics_on_steps(self, steps):
        return spFct.get_tics_or_stepsFct(2, 1, steps, -1)

    def get_tics_on_ms(self, ms):
        return spFct.get_tics_or_stepsFct(3, 1, -1, ms)

    def get_tics_on_ms_stamp(self, ms_stamp):
        return spFct.get_tics_or_stepsFct(4, 1, -1, ms_stamp)

    def get_steps_on_resolution(self):
        return spFct.get_tics_or_stepsFct(0, 2, -1, -1)

    def get_steps_on_tics(self, tics):
        return spFct.get_tics_or_stepsFct(1, 2, tics, -1)

    def get_steps_on_ms(self, ms):
        return spFct.get_tics_or_stepsFct(3, 2, -1, ms)

    def get_steps_on_ms_stamp(self, ms_stamp):
        return spFct.get_tics_or_stepsFct(4, 2, -1, ms_stamp)

    def get_modulo(self, value):
        return spFct.get_scheduler_valueFct(0, value)

    def get_slice_modulo(self, value):
        return spFct.get_scheduler_valueFct(1, value)

    def get_min_delay(self):
        return spFct.get_scheduler_valueFct(2, -1)

    def get_max_delay(self):
        return spFct.get_scheduler_valueFct(3, -1)



# Here should be put the user herited class

# location symbol for anchoring the code : <!f>zg4"*$
import math
import sys

class MyNeuron(Neuron):
    def __cinit__(self):
        pass
    def __dealloc__(self):
        pass

    def calibrate(self):

        self.tau_m   = 10.0  # ms
        self.C_m     = 250.0 # pF
        self.t_ref   =  2.0 # ms
        self.E_L     = -70.0 # mV
        self.I_e_     = 0.0 # pA
        self.V_th    = -55.0 - self.E_L # mV, rel to U0_
        self.V_min_   = -sys.float_info.max # relative U0_-55.0-U0_),  // mV, rel to U0_
        self.V_reset = -70.0 - self.E_L
        self.with_refr_input_ = False
        self.y0_      = 0.0
        self.y3_      = 0.0
        self.r_       = 0
        self.refr_spikes_buffer_ = 0.0
        self.P30_ = 0.0
        self.P33_ = 0.0
        self.RefractoryCounts_ = 0

        self.ms_resolution = self.get_ms_on_resolution()
        self.P33_ = math.exp(-self.ms_resolution/self.tau_m)
        self.P30_ = 1/self.C_m*(1-self.P33_)*self.tau_m
        self.RefractoryCounts_ = self.get_steps_on_ms(self.t_ref)

    def setStatus(self):
        # if U0_ is changed, we need to adjust all variables defined relative to U0_
        ELold = self.E_L
        delta_EL = self.E_L - ELold
  // if U0_ is changed, we need to adjust all variables defined relative to U0_
  const double ELold = E_L_;
  updateValue<double>(d, names::E_L, E_L_);
  const double delta_EL = E_L_ - ELold;

  if(updateValue<double>(d, names::V_reset, V_reset_))
    V_reset_ -= E_L_;
  else
    V_reset_ -= delta_EL;

  if (updateValue<double>(d, names::V_th, V_th_)) 
    V_th_ -= E_L_;
  else
    V_th_ -= delta_EL;

  if (updateValue<double>(d, names::V_min, V_min_))
    V_min_ -= E_L_;
  else
    V_min_ -= delta_EL;
    
  updateValue<double>(d, names::I_e, I_e_);
  updateValue<double>(d, names::C_m, c_m_);
  updateValue<double>(d, names::tau_m, tau_m_);
  updateValue<double>(d, names::t_ref, t_ref_);

  if ( V_reset_ >= V_th_ )
    throw BadProperty("Reset potential must be smaller than threshold.");
    
  if ( c_m_ <= 0 )
    throw BadProperty("Capacitance must be >0.");

  if ( t_ref_ < 0 )
    throw BadProperty("Refractory time must not be negative.");
    
  if ( tau_m_ <= 0 )
    throw BadProperty("Membrane time constant must be > 0.");

  updateValue<bool>(d, "refractory_input", with_refr_input_);

  return delta_EL;










    def update(self):
        if self.r_ == 0:
            # neuron not refractory
            self.y3_ = self.P30_*(self.y0_ + self.I_e_) + self.P33_*self.y3_ + (self.ex_spikes + self.in_spikes)

            # if we have accumulated spikes from refractory period, 
            # add and reset accumulator
            if self.with_refr_input_ and self.refr_spikes_buffer_ != 0.0:
                self.y3_ += self.refr_spikes_buffer_
                self.refr_spikes_buffer_ = 0.0
      
            # lower bound of membrane potential
            if self.y3_ < self.V_min_:
                self.y3_ = self.V_min_	 
    
        else: # neuron is absolute refractory
            # read spikes from buffer and accumulate them, discounting
            # for decay until end of refractory period
            if self.with_refr_input_:
                self.refr_spikes_buffer_ += (self.ex_spikes + self.in_spikes) * math.exp(-self.r_ * self.ms_resolution / self.tau_m)
            else:
                self.ex_spikes = 0  # clear buffer entry, ignore spike
                self.in_spikes = 0

            self.r_ -= 1
   
        # threshold crossing
        if self.y3_ >= self.V_th:
            self.r_ = self.RefractoryCounts_
            self.y3_ = self.V_reset
            self.spike_emission = True
        else:
            self.spike_emission = False
# $*"4gz>f!< : location symbol for end of code anchoring

# End of the user herited class




# every neuron on the NEST side as a counterpart here. This avoids creating objects each time
neurons = []


# sets the special functions used by the neurons
cdef public void putSpecialFunctions(double (*get_ms)(int, long, double), long (*get_tics_or_steps)(int, int, long, double), unsigned int (*get_scheduler_value)(int, unsigned int)) with gil:
    spFct.get_msFct = get_ms
    spFct.get_tics_or_stepsFct = get_tics_or_steps
    spFct.get_scheduler_valueFct = get_scheduler_value


# creation of a new neuron and returning of the corresponding id. Now the id is just the location inside the list,
# but it will be enhanced in order to take into account neuron deletions (in which case two neurons could have the same id)
cdef public int createNeuron() with gil:
    n = MyNeuron()
    n.spike_emission = False
    neurons.append(n) ## init not called???					TODO
    return len(neurons) - 1


# used in order to set the neuron members
cdef public void setNeuronParams(int neuronID, dict members) with gil:
    for key, value in members.iteritems():
        if (key is not "") and (key is not "calibrate") and (key is not "update") and (key is not "setStatus"):
            setattr(neurons[neuronID], key, value)

# used in order to get the neuron members
cdef public dict getNeuronParams(int neuronID) with gil:
    return vars(neurons[neuronID])

cdef public void update(int neuronID) with gil:
    neurons[neuronID].update()

cdef public void calibrate(int neuronID) with gil:
    neurons[neuronID].calibrate()

cdef public void setStatus(int neuronID) with gil:
    neurons[neuronID].set()


