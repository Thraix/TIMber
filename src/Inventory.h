#pragma once

#include <map>

#include "Voxels.h"

class Inventory 
{
  private:
    std::vector<std::pair<const Voxel&, float>> materials;
    size_t currentMaterial = 0;
  public:
    Inventory()
    {
      materials.push_back({Voxel::grass, 200});
      materials.push_back({Voxel::stone, 200});
      materials.push_back({Voxel::snow, 200});
    }
    virtual ~Inventory(){}

    float GetCurrentMaterialAmount() const
    {
      return materials[currentMaterial].second;
    }

    float GetMaterialAmount(size_t pos) const
    {
      return materials[pos].second;
    }

    void AddMaterials(size_t id, float amount)
    {
      for(auto it = materials.begin(); it!=materials.end();it++)
      {
        if(it->first.id == id)
        {
          it->second += amount;
          return;
        }
      }
      materials.push_back({Voxel::FindById(id), amount});
    }

    void RemoveMaterials(size_t id, float amount)
    {
      for(auto it = materials.begin(); it!=materials.end();it++)
      {
        if(it->first.id == id)
        {
          it->second -= amount;
        }
      }
    }

    void ChangeMaterial(int offset)
    {
      if(offset < 0)
        offset += GetInventorySize();

      currentMaterial += offset;
      currentMaterial %= GetInventorySize();
    }

    size_t GetCurrentPos() const
    {
      return currentMaterial;
    }

    size_t GetInventorySize() const
    {
      return materials.size();
    }

    const Voxel& GetCurrentMaterial() const
    {
      return materials[currentMaterial].first;
    }

    const Voxel& GetMaterial(size_t pos) const
    {
      return materials[pos].first;
    }
};

