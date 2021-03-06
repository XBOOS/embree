// ======================================================================== //
// Copyright 2009-2018 Intel Corporation                                    //
//                                                                          //
// Licensed under the Apache License, Version 2.0 (the "License");          //
// you may not use this file except in compliance with the License.         //
// You may obtain a copy of the License at                                  //
//                                                                          //
//     http://www.apache.org/licenses/LICENSE-2.0                           //
//                                                                          //
// Unless required by applicable law or agreed to in writing, software      //
// distributed under the License is distributed on an "AS IS" BASIS,        //
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. //
// See the License for the specific language governing permissions and      //
// limitations under the License.                                           //
// ======================================================================== //

#pragma once

#include "accelset.h"

namespace embree
{
  class InstanceFactory
  {
  public:
    InstanceFactory(int features);
    DEFINE_SYMBOL2(RTCBoundsFunction,InstanceBoundsFunc);
    DEFINE_SYMBOL2(AccelSet::IntersectorN,InstanceIntersectorN);
  };

  /*! Instanced acceleration structure */
  struct Instance : public AccelSet
  {
    ALIGNED_STRUCT_(16);
    
  public:
    Instance (Device* device, Scene* object, unsigned int numTimeSteps);
    ~Instance();

  private:
    Instance (const Instance& other) DELETED; // do not implement
    Instance& operator= (const Instance& other) DELETED; // do not implement
    
  public:
    virtual void setNumTimeSteps (unsigned int numTimeSteps);
    virtual void setInstancedScene(const Ref<Scene>& scene);
    virtual void setTransform(const AffineSpace3fa& local2world, unsigned int timeStep);
    virtual AffineSpace3fa getTransform(float time);
    virtual void setMask (unsigned mask);
    virtual void build() {}

  public:

    __forceinline AffineSpace3fa getWorld2Local() const {
      return world2local0;
    }

    __forceinline AffineSpace3fa getWorld2Local(float t) const 
    {
      float ftime;
      const unsigned int itime = getTimeSegment(t, fnumTimeSegments, ftime);
      return rcp(lerp(local2world[itime+0],local2world[itime+1],ftime));
    }

    template<int K>
    __forceinline AffineSpace3vf<K> getWorld2Local(const vbool<K>& valid, const vfloat<K>& t) const
    { 
      vfloat<K> ftime;
      const vint<K> itime_k = getTimeSegment(t, vfloat<K>(fnumTimeSegments), ftime);
      assert(any(valid));
      const size_t index = __bsf(movemask(valid));
      const int itime = itime_k[index];
      const vfloat<K> t0 = vfloat<K>(1.0f)-ftime, t1 = ftime;
      if (likely(all(valid, itime_k == vint<K>(itime)))) {
        return rcp(t0*AffineSpace3vf<K>(local2world[itime+0]) + t1*AffineSpace3vf<K>(local2world[itime+1]));
      } else {
        AffineSpace3vf<K> space0,space1;
        vbool<K> valid1 = valid;
        while (any(valid1)) {
          vbool<K> valid2;
          const int itime = next_unique(valid1, itime_k, valid2);
          space0 = select(valid2, AffineSpace3vf<K>(local2world[itime+0]), space0);
          space1 = select(valid2, AffineSpace3vf<K>(local2world[itime+1]), space1);
        }
        return rcp(t0*space0 + t1*space1);
      }
    }
    
  public:
    Scene* object;                 //!< pointer to instanced acceleration structure
    AffineSpace3fa world2local0;   //!< transformation from world space to local space for timestep 0
    AffineSpace3fa* local2world;   //!< transformation from local space to world space for each timestep
  };
}
