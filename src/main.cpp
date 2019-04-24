#include <Greet.h>

#include "World.h"

using namespace Greet;
class CrossHairLayer : public Layer
{
  Renderable2D* crossHair;
  public:
    CrossHairLayer(Renderable2D* crossHair)
      : Layer{new BatchRenderer(), ShaderFactory::DefaultShader(), Mat3::Orthographic(0,Window::GetWidth(), 0.0f, Window::GetHeight())}, crossHair{crossHair}
    {
      Add(crossHair);
      crossHair->SetPosition(Vec2((Window::GetWidth() - crossHair->GetSize().w) * 0.5f, (Window::GetHeight() - crossHair->GetSize().h)* 0.5f));
    }

    void OnEvent(Event& e) override
    {
      if(EVENT_IS_TYPE(e, EventType::WINDOW_RESIZE))
      {
        WindowResizeEvent& event = (WindowResizeEvent&)e;
        SetProjectionMatrix(Mat3::Orthographic(0,event.GetWidth(), 0, event.GetHeight()));
        crossHair->SetPosition(Vec2((event.GetWidth() - crossHair->GetSize().w) * 0.5f, (event.GetHeight() - crossHair->GetSize().h)* 0.5f));
      }
    }
};

class Game : public App
{
  private:

    World* world;
    CrossHairLayer* layer;
    Renderable2D* crossHair;

  public:
    Game()
      : App{"TIMber", 1440, 1440 * 9 / 16}
    {

    }

    ~Game()
    {
      delete world;
      delete layer;
    }

    void Init()
    {
      Loaders::LoadTextures("res/textures.json");
      float* noiseMap = Noise::GenNoise(512,512, 5, 32, 32, 0.75f);
      BYTE* image = ImageUtils::CreateHeightmapImage(noiseMap, 512, 512);
      TextureManager::Add("noiseMap", Texture2D(image, 512, 512));

      crossHair = new Renderable2D({0,0}, {20,20}, 0xffffffff, new Sprite(TextureManager::Get2D("crosshair")), nullptr);
      layer = new CrossHairLayer(crossHair);

      world = new World(2, 2);
      RenderEngine::Add3DScene(world, "3dscene");
      RenderEngine::Add2DScene(layer, "crosshair");
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
