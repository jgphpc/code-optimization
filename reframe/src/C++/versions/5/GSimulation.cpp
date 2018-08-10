/*
    This file is part of the example codes which have been used
    for the "Code Optmization Workshop".
    
    Copyright (C) 2016  Fabio Baruffa <fbaru-dev@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "GSimulation.hpp"
#include "cpu_time.hpp"

GSimulation :: GSimulation()
{
  std::cout << "===============================" << std::endl;
  std::cout << " Initialize Gravity Simulation" << std::endl;
  set_npart(16000); 
  set_nsteps(10);
  set_tstep(0.1); 
  set_sfreq(1);
}

void GSimulation :: set_number_of_particles(int N)  
{
  set_npart(N);
}

void GSimulation :: set_number_of_steps(int N)  
{
  set_nsteps(N);
}

void GSimulation :: init_pos()  
{
  std::random_device rd;	//random number generator
  std::mt19937 gen(42);      
  std::uniform_real_distribution<real_type> unif_d(0,1.0);
  auto particles = _particles.access();
  for (int i = 0; i < get_npart(); ++i)
  {
      particles[i].pos_x() = unif_d(gen);
      particles[i].pos_y() = unif_d(gen);
      particles[i].pos_z() = unif_d(gen);
  }
}

void GSimulation :: init_vel()  
{
  std::random_device rd;        //random number generator
  std::mt19937 gen(42);
  std::uniform_real_distribution<real_type> unif_d(-1.0,1.0);
  auto particles = _particles.access();
  for(int i=0; i<get_npart(); ++i)
  {
     particles[i].vel_x() = unif_d(gen) * 1.0e-3f;
     particles[i].vel_y() = unif_d(gen) * 1.0e-3f;
     particles[i].vel_z() = unif_d(gen) * 1.0e-3f;
  }
}

void GSimulation :: init_acc() 
{
  auto particles = _particles.access();
  for(int i=0; i<get_npart(); ++i)
  {
     particles[i].acc_x() = 0.f;
     particles[i].acc_y() = 0.f;
     particles[i].acc_z() = 0.f;
  }
}

void GSimulation :: init_mass() 
{
  real_type n   = static_cast<real_type> (get_npart());
  std::random_device rd;        //random number generator
  std::mt19937 gen(42);
  std::uniform_real_distribution<real_type> unif_d(0.0,1.0);
  auto particles = _particles.access();
  for(int i=0; i<get_npart(); ++i)
  {
    particles[i].mass() = n * unif_d(gen); 
  }
}

void GSimulation :: start() 
{
  real_type energy;
  real_type dt = get_tstep();
  int n = get_npart();
  int i,j;
  
   //allocate particles
   _particles.resize(n);

  init_pos();	
  init_vel();
  init_acc();
  init_mass();
  
  print_header();
  
  _totTime = 0.; 
  
  const float softeningSquared = 1e-3f;
  // prevents explosion in the case the particles are really close to each other 
  const float G = 6.67259e-11f;
  
  CPUTime time;
  double ts0 = 0;
  double ts1 = 0;
  double nd = double(n);
  double gflops = 1e-9 * ( (11. + 18. ) * nd*nd  +  nd * 19. );
  double av=0.0, dev=0.0;
  int nf = 0;
  auto particles = _particles.access();
  const double t0 = time.start();
  for (int s=1; s<=get_nsteps(); ++s)
  {   
    ts0 += time.start(); 
    for (i = 0; i < n; i++)// update acceleration
    {
      real_type ax_i = particles[i].acc_x();
      real_type ay_i = particles[i].acc_y();
      real_type az_i = particles[i].acc_z();
      for (j = 0; j < n; j++)
      {
	  real_type dx, dy, dz;
	  real_type distanceSqr = 0.0f;
	  real_type distanceInv = 0.0f;
		  
	  dx = particles[j].pos_x() - particles[i].pos_x();	//1flop
	  dy = particles[j].pos_y() - particles[i].pos_y();	//1flop	
	  dz = particles[j].pos_z() - particles[i].pos_z();	//1flop
	
	  distanceSqr = dx*dx + dy*dy + dz*dz + softeningSquared;	//6flops
	  distanceInv = 1.0f / sqrtf(distanceSqr);			//1div+1sqrt
		  
	  ax_i += dx * G * particles[j].mass() * distanceInv * distanceInv * distanceInv;	//6flops
	  ay_i += dy * G * particles[j].mass() * distanceInv * distanceInv * distanceInv;	//6flops
	  az_i += dz * G * particles[j].mass() * distanceInv * distanceInv * distanceInv;	//6flops
      }
      particles[i].acc_x() = ax_i;
      particles[i].acc_y() = ay_i;
      particles[i].acc_z() = az_i;
    }
    energy = 0;

    for (i = 0; i < n; ++i)// update position and velocity
    {
      particles[i].vel_x() += particles[i].acc_x() * dt;	//2flops
      particles[i].vel_y() += particles[i].acc_y() * dt;	//2flops
      particles[i].vel_z() += particles[i].acc_z() * dt;	//2flops
	 
      particles[i].pos_x() += particles[i].vel_x() * dt;	//2flops
      particles[i].pos_y()+= particles[i].vel_y() * dt;	//2flops
      particles[i].pos_z() += particles[i].vel_z() * dt;	//2flops

      particles[i].acc_x() = 0.;
      particles[i].acc_y() = 0.;
      particles[i].acc_z() = 0.;
	
      energy += particles[i].mass() * (
		particles[i].vel_x()*particles[i].vel_x() + 
                particles[i].vel_y()*particles[i].vel_y() +
                particles[i].vel_z()*particles[i].vel_z()); //7flops
    }
  
    _kenergy = 0.5 * energy; 
    
    ts1 += time.stop();
    if(!(s%get_sfreq()) ) 
    {
      nf += 1;      
      std::cout << " " 
		<<  std::left << std::setw(8)  << s
		<<  std::left << std::setprecision(5) << std::setw(8)  << s*get_tstep()
		<<  std::left << std::setprecision(5) << std::setw(12) << _kenergy
		<<  std::left << std::setprecision(5) << std::setw(12) << (ts1 - ts0)
		<<  std::left << std::setprecision(5) << std::setw(12) << gflops*get_sfreq()/(ts1 - ts0)
		<<  std::endl;
      if(nf > 2) 
      {
	av  += gflops*get_sfreq()/(ts1 - ts0);
	dev += gflops*get_sfreq()*gflops*get_sfreq()/((ts1-ts0)*(ts1-ts0));
      }
      
      ts0 = 0;
      ts1 = 0;
    }
  
  } //end of the time step loop
  
  const double t1 = time.stop();
  _totTime  = (t1-t0);
  _totFlops = gflops*get_nsteps();
  
  av/=(double)(nf-2);
  dev=sqrt(dev/(double)(nf-2)-av*av);
  
  int nthreads=1;

  std::cout << std::endl;
  std::cout << "# Number Threads      : " << nthreads << std::endl;	   
  std::cout << "# Total Time (s)      : " << _totTime << std::endl;
  std::cout << "# Average Performance : " << av << " +- " <<  dev << std::endl;
  std::cout << "===============================" << std::endl;

}


void GSimulation :: print_header()
{
	    
  std::cout << " nPart = " << get_npart()  << "; " 
	    << "nSteps = " << get_nsteps() << "; " 
	    << "dt = "     << get_tstep()  << std::endl;
	    
  std::cout << "------------------------------------------------" << std::endl;
  std::cout << " " 
	    <<  std::left << std::setw(8)  << "s"
	    <<  std::left << std::setw(8)  << "dt"
	    <<  std::left << std::setw(12) << "kenergy"
	    <<  std::left << std::setw(12) << "time (s)"
	    <<  std::left << std::setw(12) << "GFlops"
	    <<  std::endl;
  std::cout << "------------------------------------------------" << std::endl;


}

GSimulation :: ~GSimulation()
{
}
