#include "TextureLoader.h"

#include <graphics/textures/CubeMap.h>
#include <graphics/textures/Texture2D.h>
#include <graphics/textures/TextureManager.h>
#include <utils/json/JSONLoader.h>

using namespace Greet;

void TextureLoader::LoadFromJSON(const std::string& loader)
{
  JSONObject object = JSONLoader::LoadJSON(loader);
  std::string basePath = object.GetValue("basePath");
  const JSONArray& array = object.GetArray("textures");
  for(int i = 0;i<array.Size();i++)
  {
    const JSONObject& object = array.GetObject(i);
    if(object.GetValue("type") == "CubeMap")
      TextureManager::Add(object.GetValue("name"), CubeMap(basePath + object.GetValue("path")));
    else
      TextureManager::Add(object.GetValue("name"), Texture2D(basePath + object.GetValue("path")));
  }
}
