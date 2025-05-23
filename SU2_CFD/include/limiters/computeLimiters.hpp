/*!
 * \file computeLimiters.hpp
 * \brief Compute limiters wrapper function.
 * \author P. Gomes
 * \version 8.2.0 "Harrier"
 *
 * SU2 Project Website: https://su2code.github.io
 *
 * The SU2 Project is maintained by the SU2 Foundation
 * (http://su2foundation.org)
 *
 * Copyright 2012-2025, SU2 Contributors (cf. AUTHORS.md)
 *
 * SU2 is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * SU2 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with SU2. If not, see <http://www.gnu.org/licenses/>.
 */

#include "CLimiterDetails.hpp"
#include "computeLimiters_impl.hpp"

/*!
 * \brief A wrapper funtion that calls specialized implementations depending
 *        on "LimiterKind". Those implementations are generated by instantiating
 *        versions of "computeLimiters_impl" with appropriate specializations
 *        of "CLimiterDetails". See corresponding hpp files for further details.
 * \ingroup FvmAlgos
 */
template<class FieldType, class GradientType>
void computeLimiters(LIMITER LimiterKind,
                     CSolver* solver,
                     MPI_QUANTITIES kindMpiComm,
                     PERIODIC_QUANTITIES kindPeriodicComm1,
                     PERIODIC_QUANTITIES kindPeriodicComm2,
                     CGeometry& geometry,
                     const CConfig& config,
                     size_t varBegin,
                     size_t varEnd,
                     const FieldType& field,
                     const GradientType& gradient,
                     FieldType& fieldMin,
                     FieldType& fieldMax,
                     FieldType& limiter)
{
  if (geometry.GetnDim() != 2 && geometry.GetnDim() != 3)
    SU2_MPI::Error("Too many dimensions to compute limiters.", CURRENT_FUNCTION);

#define INSTANTIATE(KIND)\
if (geometry.GetnDim() == 2) {\
  computeLimiters_impl<2,KIND>(solver, kindMpiComm, kindPeriodicComm1, kindPeriodicComm2, geometry,\
                               config, varBegin, varEnd, field, gradient, fieldMin, fieldMax, limiter);\
} else {\
  computeLimiters_impl<3,KIND>(solver, kindMpiComm, kindPeriodicComm1, kindPeriodicComm2, geometry,\
                               config, varBegin, varEnd, field, gradient, fieldMin, fieldMax, limiter);\
}
  switch (LimiterKind) {
    case LIMITER::NONE:
    {
      SU2_OMP_FOR_STAT(512)
      for(size_t iPoint = 0; iPoint < geometry.GetnPoint(); ++iPoint)
        for(size_t iVar = varBegin; iVar < varEnd; ++iVar)
         limiter(iPoint, iVar) = 1.0;
      END_SU2_OMP_FOR
      break;
    }
    case LIMITER::BARTH_JESPERSEN:
    {
      INSTANTIATE(LIMITER::BARTH_JESPERSEN);
      break;
    }
    case LIMITER::VENKATAKRISHNAN:
    {
      INSTANTIATE(LIMITER::VENKATAKRISHNAN);
      break;
    }
    case LIMITER::NISHIKAWA_R3:
    {
      INSTANTIATE(LIMITER::NISHIKAWA_R3);
      break;
    }
    case LIMITER::NISHIKAWA_R4:
    {
      INSTANTIATE(LIMITER::NISHIKAWA_R4);
      break;
    }
    case LIMITER::NISHIKAWA_R5:
    {
      INSTANTIATE(LIMITER::NISHIKAWA_R5);
      break;
    }
    case LIMITER::VENKATAKRISHNAN_WANG:
    {
      INSTANTIATE(LIMITER::VENKATAKRISHNAN_WANG);
      break;
    }
    case LIMITER::WALL_DISTANCE:
    {
      INSTANTIATE(LIMITER::WALL_DISTANCE);
      break;
    }
    case LIMITER::SHARP_EDGES:
    {
      INSTANTIATE(LIMITER::SHARP_EDGES);
      break;
    }
    default:
    {
      SU2_MPI::Error("Unknown limiter type.", CURRENT_FUNCTION);
      break;
    }
  }
#undef INSTANTIATE
}
