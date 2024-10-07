#pragma once
#include <cstdint>
#include <ostream>
#include <sstream>
#include <string>
#include <unordered_map>

#include "EngineUtils.h"

namespace Engine
{
    using namespace std;
    
    enum class CollisionLayer : uint8_t
    {
        None = 0,
        StoneWall = 1 << 0,
        Projectile = 1 << 1,
        Wall = 1 << 2,
        Player = 1 << 3,
        Enemy = 1 << 4,
        All = (1 << 8) - 1
    };

    const std::unordered_map<string, CollisionLayer> LayerNameToEnum =
     {
        {"None", CollisionLayer::None},
        {"StoneWall", CollisionLayer::StoneWall},
        {"Projectile", CollisionLayer::Projectile},
        {"Wall", CollisionLayer::Wall},
        {"Player", CollisionLayer::Player},
        {"Enemy", CollisionLayer::Enemy},
        {"All", CollisionLayer::All}
     };

    class CollisionUtils
    {
        public:
            //Converts list of layers in format "Layer1;Layer2" to bitmask
            static uint8_t StringToCollisionMask(const string& MaskString)
            {
                std::istringstream InputStream(MaskString);
                std::string LayerName;
                uint8_t ResultMask = 0;

                while (std::getline(InputStream, LayerName, ';'))
                {
                    auto it = LayerNameToEnum.find(LayerName);

                    if (const bool bIsLayerFound = it != LayerNameToEnum.end())
                    {
                        ResultMask |= static_cast<uint8_t>(it->second);
                    }
                    else
                    {
                        string ErrorStr = "Unknown layer name \"" + LayerName;
                        checkMsg(bIsLayerFound, ErrorStr.c_str());
                    }
                }
                return ResultMask;
            }
    };
}
