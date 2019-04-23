#include <Greet.h>

#include "World.h"

using namespace Greet;

class Game : public App
{
  private:

    World* world;

  public:
    Game()
      : App{"TIMber", 1440, 1440 * 9 / 16}
    {

    }

    ~Game()
    {
      delete world;
    }

    void Init()
    {
      Loaders::LoadTextures("res/textures.json");
      world = new World(2, 2);
      RenderEngine::Add3DScene(world, "3dscene");
    }

    void Tick()
    {

    }

    void Update(float elapsedTime)
    {

    }

    void Render() 
    {

    }
};

int main()
{
  Game game;
  game.Start();
  return 0;
}
