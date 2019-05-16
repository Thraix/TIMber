#include <Greet.h>

#include "World.h"
#include "TestWorld.h"
#include "demo/MarchingDemo.h"
#include "LineRenderer.h"

//#define TESTING
//#define DEMO

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

    void WindowResize(WindowResizeEvent& event) override
    {
      SetProjectionMatrix(Mat3::Orthographic(0,event.GetWidth(), 0, event.GetHeight()));
      crossHair->SetPosition(Vec2((event.GetWidth() - crossHair->GetSize().w) * 0.5f, (event.GetHeight() - crossHair->GetSize().h)* 0.5f));
    }
};

class Game : public App
{
  private:

    #ifdef DEMO
    MarchingDemo* demo;
#else
#ifndef TESTING
    World* world;
#else
    TestWorld* world;
#endif
#endif
    Layer3D* lineLayer;
    CrossHairLayer* layer;
    Renderable2D* crossHair;

  public:
    Game()
      : App{"TIMber", 1440, 1440 * 9 / 16}
    {

    }

    ~Game()
    {
#ifndef DEMO
      delete world;
      delete layer;
#else
      delete demo;
#endif
      LineRenderer::DestroyInstance();
    }

    void Init()
    {
      LineRenderer::CreateInstance();
      Loaders::LoadTextures("res/textures.json");
      FontManager::Add(new FontContainer("res/fonts/Roboto-Black.ttf","roboto"));
      float* noiseMap = Noise::GenNoise(512, 512, 5, 128, 128, 0.75f);
      BYTE* image = ImageUtils::CreateHeightmapImage(noiseMap, 512, 512);
      TextureManager::Add("noiseMap", Texture2D(image, 512, 512));

      crossHair = new Renderable2D({0,0}, {20,20}, 0xffffffff, Sprite(TextureManager::Get2D("crosshair")));
      Renderable2D* noiseRenderable = new Renderable2D({0,0}, {512,512}, 0xffffffff, Sprite(TextureManager::Get2D("noiseMap")));
#ifdef DEMO
      demo = new MarchingDemo();
      RenderEngine::Add3DScene(demo, "3dscene");
      Window::SetBackgroundColor(Vec4(0.5f,0.5f,0.5f, 1.0f));
#else 
#ifndef TESTING
      world = new World(3, 3);
#else
      world = new TestWorld();
#endif
      layer = new CrossHairLayer(crossHair);
      // layer->Add(noiseRenderable);
      RenderEngine::Add3DScene(world, "3dscene");
      RenderEngine::Add2DScene(layer, "crosshair");
#endif
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
