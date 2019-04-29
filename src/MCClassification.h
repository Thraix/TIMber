#pragma once

#include <math/Vec3.h>
#include <vector>
#include <logging/Log.h>
#include "MCPointData.h"

class MCClassification 
{
  private:
    static std::vector<Greet::Vec3<float>> vertices;
    static std::vector<std::pair<size_t, size_t>> edges;
    static std::vector<std::vector<Greet::Vec3<size_t>>> classifications;

  public:
    static Greet::Vec3<float> GetEdgeMidPoint(size_t edge, const std::vector<MCPointData>& data, uint x, uint y, uint z, uint width, uint height, uint length)
    {
      Greet::Vec3<float> v1 = vertices[edges[edge].first];
      Greet::Vec3<float> v2 = vertices[edges[edge].second];
      MCPointData d1 = data[(x + v1.x) + ((y + v1.y) + (z + v1.z) * height) * width];
      MCPointData d2 = data[(x + v2.x) + ((y + v2.y) + (z + v2.z) * height) * width];

      float t = 0.5;
      if(d1.inhabited)
      {
        t = 1.0f - d1.magnitude;
      }
      else
      {
        t = d2.magnitude;
      }
      return t * v1 + (1 - t) * v2;
    }

    static std::vector<Greet::Vec3<Greet::Vec3<float>>> GetMarchingCubeFaces(const std::vector<MCPointData>& data, uint x, uint y, uint z, uint width, uint height, uint length)
    {
      byte classification = 0;
      for(auto&& vertex : vertices)
      {
        classification >>= 1;
        const MCPointData& point = data[(x + (size_t)vertex.x) + (y + (size_t)vertex.y) * width  + (z + (size_t)vertex.z) * width * height];
        if(!point.inhabited)
          classification |= 0x80; // 0b1000000
      }
      std::vector<Greet::Vec3<size_t>> faces = classifications[classification];
      std::vector<Greet::Vec3<Greet::Vec3<float>>> result;
      Greet::Vec3<float> offset{x,y,z};
      for(auto&& edges : faces)
      {
        result.push_back({
            GetEdgeMidPoint(edges[1],data,x,y,z, width, height, length)+offset,
            GetEdgeMidPoint(edges[0],data,x,y,z, width, height, length)+offset,
            GetEdgeMidPoint(edges[2],data,x,y,z, width, height, length)+offset
            });
      }
      return result;
    }
};

