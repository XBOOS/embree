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

#include "scene_user_geometry.h"
#include "scene.h"

namespace embree
{
  UserGeometry::UserGeometry (Device* device, unsigned int items, unsigned int numTimeSteps) 
    : AccelSet(device,items,numTimeSteps) {}
  
  void UserGeometry::setUserData (void* ptr) {
    intersectors.ptr = ptr;
    Geometry::setUserData(ptr);
  }

  void UserGeometry::setMask (unsigned mask) 
  {
    this->mask = mask; 
    Geometry::update();
  }

  void UserGeometry::setBoundsFunction (RTCBoundsFunction bounds, void* userPtr) {
    this->boundsFunc = bounds;
  }

  void UserGeometry::setIntersectFunctionN (RTCIntersectFunctionN intersect) {
    intersectors.intersectorN.intersect = intersect;
  }

  void UserGeometry::setOccludedFunctionN (RTCOccludedFunctionN occluded) {
    intersectors.intersectorN.occluded = occluded;
  }
}
