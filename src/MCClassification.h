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
    static Greet::Vec3<float> GetEdgeMidPoint(size_t edge)
    {
      return (vertices[edges[edge].first] + vertices[edges[edge].second])* 0.5f;
      float t = 0.5f;
      return t * vertices[edges[edge].first] + (1 - t) * vertices[edges[edge].second];
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
            GetEdgeMidPoint(edges[1])+offset,
            GetEdgeMidPoint(edges[0])+offset,
            GetEdgeMidPoint(edges[2])+offset
            });
      }
      return result;
    }
};

