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

#ifndef _PARTICLE_HPP
#define _PARTICLE_HPP
#include <cmath>
#include "types.hpp"
#include <sdlt/sdlt.h>
struct Particle
{
  public:
    Particle() { init();}
    void init() 
    {
      pos_x = 0.; pos_y = 0.; pos_z = 0.;
      vel_x = 0.; vel_y = 0.; vel_z = 0.;
      acc_x = 0.; acc_y = 0.; acc_z = 0.;
      mass  = 0.;
    }
    real_type pos_x, pos_y, pos_z;
    real_type vel_x, vel_y, vel_z;
    real_type acc_x, acc_y, acc_z;
    real_type mass;
};

SDLT_PRIMITIVE(Particle, pos_x, pos_y, pos_z, vel_x, vel_y, vel_z, acc_x, acc_y, acc_z, mass)

#endif
