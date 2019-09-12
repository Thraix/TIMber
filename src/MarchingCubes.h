#pragma once

#include <common/Types.h>
#include <graphics/models/Mesh.h>
#include <graphics/models/MeshFactory.h>
#include "HalfEdgeMesh.h"
#include "MCPointData.h"
#include "MCClassification.h"

class MarchingCubes 
{

  private:
    Greet::Mesh* mesh;
    HalfEdgeMesh heMesh;
  public:
    MarchingCubes(MCPointData* data, uint width, uint height, uint length)
    {
      // Go through all the cubes
      for(uint z = 0;z<length-1;z++)
      {
        for(uint y = 0;y<height-1;y++)
        {
          for(uint x = 0;x<width-1;x++)
          {
            std::vector<Greet::Vec3<Greet::Vec3<float>>> faces = MCClassification::GetMarchingCubeFaces(data, x, y, z, width, height, length);
            for(auto&& face : faces)
            {
              heMesh.AddFace(face);
            }
          }
        }
      }
      //heMesh.CalculateNormals();
      mesh = new Greet::Mesh(heMesh.vertexArray.data(), heMesh.vertexArray.size(), heMesh.indices.data(), heMesh.indices.size());
      Greet::Vec3<float>* normals = Greet::MeshFactory::CalculateNormals(heMesh.vertexArray.data(), heMesh.vertexArray.size(), heMesh.indices.data(), heMesh.indices.size());
      mesh->AddAttribute(new Greet::AttributeData<Greet::Vec3<float>>(ATTRIBUTE_NORMAL, normals));
    }

    virtual ~MarchingCubes()
    {
      delete mesh;
    }

    Greet::Mesh* GetMesh()
    {
      return mesh;
    }

    HalfEdgeMesh* GetHalfEdgeMesh()
    {
      return &heMesh;
    }
};

