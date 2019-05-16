#pragma once

#include <math/Vec3.h>
#include <vector>
#include <logging/Log.h>
#include "MCPointData.h"

class MCClassification 
{
  public:
    static const std::vector<Greet::Vec3<float>> vertices;
    static const std::vector<std::pair<size_t, size_t>> edges;
    static const std::vector<std::vector<Greet::Vec3<size_t>>> classifications;

  public:
    static Greet::Vec3<float> GetEdgeMidPoint(size_t edge, const std::vector<MCPointData>& data, uint x, uint y, uint z, uint width, uint height, uint length)
    {
      Greet::Vec3<float> v1 = vertices[edges[edge].first];
      Greet::Vec3<float> v2 = vertices[edges[edge].second];
      return (v1 + v2) * 0.5f;
      MCPointData d1 = data[(x + v1.x) + ((y + v1.y) + (z + v1.z) * height) * width];
      MCPointData d2 = data[(x + v2.x) + ((y + v2.y) + (z + v2.z) * height) * width];

      float t = 0.5;
      if(d1.Inhabited())
      {
        // d1.magnitude is negative we want abs total
        float total = d2.magnitude - d1.magnitude;
        float offset = d2.magnitude / total;
        t = offset;
      }
      else
      {
        float total = d1.magnitude - d2.magnitude;
        float offset = d1.magnitude / total;
        t = 1.0f - offset;
      }
      return t * v1 + (1 - t) * v2;
    }

    static const MCPointData& GetDataPoint(const std::vector<MCPointData>& data, uint x, uint y, uint z, uint width, uint height, uint length)
    {
      return data[x + y * width  + z * width * height];
    }

    static std::vector<Greet::Vec3<size_t>> GetMarchingCubeFaces(const std::vector<MCPointData>& data, uint x, uint y, uint z, uint width, uint height, uint length)
    {
      byte classification = 0;

      if(GetDataPoint(data, x  , y  , z  , width, height, length).magnitude < 0.0) classification |= 0x01;
      if(GetDataPoint(data, x+1, y  , z  , width, height, length).magnitude < 0.0) classification |= 0x02;
      if(GetDataPoint(data, x+1, y+1, z  , width, height, length).magnitude < 0.0) classification |= 0x04;
      if(GetDataPoint(data, x  , y+1, z  , width, height, length).magnitude < 0.0) classification |= 0x08;
      if(GetDataPoint(data, x  , y  , z+1, width, height, length).magnitude < 0.0) classification |= 0x10;
      if(GetDataPoint(data, x+1, y  , z+1, width, height, length).magnitude < 0.0) classification |= 0x20;
      if(GetDataPoint(data, x+1, y+1, z+1, width, height, length).magnitude < 0.0) classification |= 0x40;
      if(GetDataPoint(data, x  , y+1, z+1, width, height, length).magnitude < 0.0) classification |= 0x80;

      std::vector<Greet::Vec3<size_t>> faces = classifications[classification];
      std::vector<Greet::Vec3<size_t>> result;
      Greet::Vec3<float> offset{x,y,z};
      for(auto&& edges : faces)
      {
        result.push_back({
            edges[0],
            edges[1],
            edges[2]
            });
      }
      return result;
    }
};

