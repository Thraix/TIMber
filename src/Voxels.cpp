#include "Voxels.h"

using namespace Greet;

std::set<Voxel> Voxel::voxels;

JSONObject Voxel::voxelData = JSONLoader::LoadJSON("res/data/voxels.json");

const Voxel Voxel::none = Voxel{"none"};
const Voxel Voxel::grass = Voxel{"grass"};
const Voxel Voxel::stone = Voxel{"stone"};
const Voxel Voxel::copper = Voxel{"copper"};
const Voxel Voxel::tin = Voxel{"tin"};
const Voxel Voxel::snow = Voxel{"snow"};
