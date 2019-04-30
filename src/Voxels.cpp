#include "Voxels.h"

using namespace Greet;

std::set<Voxel> Voxel::voxels;

JSONObject Voxel::voxelData = JSONLoader::LoadJSON("res/data/voxels.json");

const Voxel Voxel::none = Voxel{"none"};
const Voxel Voxel::grass = Voxel{"grass"};
const Voxel Voxel::stone = Voxel{"stone"};
const Voxel Voxel::royalium = Voxel{"royalium"};
const Voxel Voxel::malachite = Voxel{"malachite"};
const Voxel Voxel::snow = Voxel{"snow"};
const Voxel Voxel::wood = Voxel{"wood"};
const Voxel Voxel::leaves = Voxel{"leaves"};
