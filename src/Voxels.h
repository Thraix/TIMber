#pragma once

#include <common/Types.h>
#include <utils/json/JSONLoader.h>
#include <graphics/gui/GUIUtils.h>
#include <math/Vec4.h>
#include <set>

enum class Voxels 
{
  NONE, GRASS, STONE, COPPER, TIN, SNOW,
};

class Voxel
{
  private:
    static std::set<Voxel> voxels;
    static Greet::JSONObject voxelData;

  public:
    size_t id;
    std::string name;
    Greet::Color color;
  private:

    Voxel(const std::string& voxelName)
    {
      if(voxelData.HasObjectKey(voxelName))
      {
        Greet::JSONObject object = voxelData.GetObject(voxelName);
        if(object.HasValueKey("id"))
          id = object.GetValueAsInt("id");
        if(object.HasValueKey("name"))
          name = object.GetValue("name");
        if(object.HasValueKey("color"))
        {
          std::string colorString = object.GetValue("color");
          color = Greet::GUIUtils::GetColor(colorString);
        }
      }
      else
      {
        Greet::Log::Error("Could not find voxel with name \"", voxelName, "\"");
      }
      voxels.emplace(*this);
    }

  public:
    virtual ~Voxel()
    {}
    Voxel(size_t id) : id{id} {}
    Voxel& SetColor(const Greet::Color& color) { this->color = color; return *this; }

    bool operator<(const Voxel& rhs) const
    {
      return id < rhs.id;
    }

    bool operator!=(const Voxel& rhs) const
    {
      return id != rhs.id;
    }

    bool operator==(const Voxel& rhs) const
    {
      return id == rhs.id;
    }

  public:
    static const Voxel none;
    static const Voxel grass;
    static const Voxel stone;
    static const Voxel royalium;
    static const Voxel malachite;
    static const Voxel snow;
    static const Voxel wood;
    static const Voxel leaves;

    static const Voxel& FindById(size_t id);
};
