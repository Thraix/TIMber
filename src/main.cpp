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
      : Layer{new BatchRenderer(ShaderFactory::Shader2D()), Mat3::OrthographicViewport()}, crossHair{crossHair}
    {
      Add(crossHair);
      crossHair->SetPosition(Vec2((Window::GetWidth() - crossHair->GetSize().w) * 0.5f, (Window::GetHeight() - crossHair->GetSize().h)* 0.5f));
    }

    void ViewportResize(ViewportResizeEvent& event) override
    {
      SetProjectionMatrix(Mat3::OrthographicViewport());
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
      FontManager::Add("roboto", FontContainer("res/fonts/Roboto-Black.ttf"));
      std::vector<float> noiseMap = Noise::GenNoise(512, 512, 5, 128, 128, 0.75f);
      std::vector<BYTE> image = ImageUtils::CreateHeightmapImage(noiseMap, 512, 512);
      TextureManager::AddTexture2D("noiseMap", Texture2D::Create(image, 512, 512));

      crossHair = new Renderable2D({0,0}, {20,20}, 0xffffffff, Sprite(TextureManager::LoadTexture2D("res/textures/crosshair.meta")));
      Renderable2D* noiseRenderable = new Renderable2D({0,0}, {512,512}, 0xffffffff, Sprite(TextureManager::LoadTexture2D("noiseMap")));
      RenderCommand::SetClearColor(Vec4(0.1f,0.1f,0.1f, 1.0f));
#ifdef DEMO
      demo = new MarchingDemo();
#else
#ifndef TESTING
      world = new World(3, 3);
#else
      world = new TestWorld();
#endif
      layer = new CrossHairLayer(crossHair);
      // layer->Add(noiseRenderable);
      GlobalSceneManager::GetSceneManager().Add3DScene(world, "world");
      GlobalSceneManager::GetSceneManager().Add2DScene(layer,"crosshair");
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
