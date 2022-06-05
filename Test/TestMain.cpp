#include "Precompile.h"

#include "Renderer/Camera.h"
#include "Renderer/Renderer.h"
using namespace d14engine::renderer;

#include "UI/AnimTextBox.h"
#include "UI/Application.h"
#include "UI/BitmapUtils.h"
#include "UI/Button.h"
#include "UI/ConstraintLayout.h"
#include "UI/ElevatedButton.h"
#include "UI/FilledButton.h"
#include "UI/GridLayout.h"
#include "UI/OnOffSwitch.h"
#include "UI/OutlinedButton.h"
#include "UI/ScrollView.h"
#include "UI/TabGroup.h"
#include "UI/Window.h"
using namespace d14engine::ui;

using namespace d14engine;

int wmain(int argc, wchar_t* argv[])
{
    try
    {
        Application::CreateInfo info = {};

        return Application(argc, argv, info).Run([&](Application* app)
        {
            app->MainRenderer()->SelectMainCamera(std::make_shared<Camera>(app->MainRenderer()));

            // Window

            auto w = MakeRootUIObject<Window>(L"D14 Engine", D2D1_RECT_F{ 0.0f, 0.0f, 800.0f, 600.0f });

            w->f_onCloseOverride = [&](Window* w) { Application::APP->Exit(); };
            w->f_onMinimizeOverride = [&](Window* w)
            {
                w->SetVisible(false);
                w->SetEnabled(false);
                w->appEventFlags.keyboard = true;

                w->ForeachChild([&](ShrdPtrParam<Panel> child)
                {
                    child->SetEnabled(false);
                    child->SetVisible(false);
                });

                Application::APP->PinUIObject(w->weak_from_this());
            };
            w->f_onKeyboardAfter = [&](Panel* p, KeyboardEvent& e)
            {
                if (e.vkey == VK_SPACE && e.status.Released())
                {
                    p->SetVisible(true);
                    p->SetEnabled(true);

                    p->ForeachChild([&](ShrdPtrParam<Panel> child)
                    {
                        child->SetEnabled(true);
                        child->SetVisible(true);
                    });

                    Application::APP->UnpinUIObject(p->weak_from_this());
                }
                return false;
            };

            // Main Layout

            auto mainLayout = MakeManagedUIObject<ConstraintLayout>(w, w->ClientAreaRect());

            mainLayout->backgroundOpacity = 0.0f;
            mainLayout->strokeOpacity = 0.0f;

            mainLayout->f_onParentSizeAfter = [&](Panel* p, SizeEvent& e)
            {
                auto parent = p->Parent();
                if (!parent.expired())
                {
                    auto wptr = std::static_pointer_cast<Window>(parent.lock());
                    p->Resize(wptr->Width(), wptr->ClientAreaHeight());
                }
            };

            // Tab Group

            auto tabGroup = MakeUIObject<TabGroup>(D2D1_RECT_F{ 0.0f, 0.0f, 300.0f, 0.0f });

            tabGroup->isLeftResizable = true;

            auto scrollView = MakeUIObject<ScrollView>(D2D1_RECT_F{});

            tabGroup->AppendPage({ MakeUIObject<Label>(L"新标签页", D2D1_RECT_F{}), scrollView });

            tabGroup->SelectPage(0);
            tabGroup->SetCardWidth(200);

            ConstraintLayout::GeometryInfo mainGeoInfo = {};

            mainGeoInfo.Right.ToRight = 0.0f;
            mainGeoInfo.Top.ToTop = 50.0f;
            mainGeoInfo.Bottom.ToBottom = 0.0f;
            mainGeoInfo.keepHeight = false;

            mainLayout->AddElement(tabGroup, mainGeoInfo);

            auto sideLayout = MakeUIObject<GridLayout>(tabGroup->SelfCoordRect(), 4, 8, 12.0f, 12.0f);

            scrollView->SetContent(sideLayout);

            tabGroup->maximalWidth = tabGroup->Width();
            w->maximalHeight = w->Height();

            // Miscellaneous Widgets

            auto l = MakeUIObject<Label>(L"名称", D2D1_RECT_F{});

            GridLayout::GeometryInfo sideGeoInfo = {};

            sideGeoInfo.axis.x = { 0, 1 };
            sideGeoInfo.axis.y = { 0, 1 };

            sideLayout->AddElement(l, sideGeoInfo);

            auto tb = MakeUIObject<AnimTextBox>(D2D1_RECT_F{}, 5.0f);

            tb->f_onTextChangeAfter = [&, wptr = (WeakPtr<Window>)w](TextBox* tb)
            {
                if (!wptr.expired())
                {
                    wptr.lock()->SetTitle(tb->Text());
                }
            };

            sideGeoInfo.axis.x = { 1, 3 };
            sideGeoInfo.axis.y = { 0, 1 };

            sideLayout->AddElement(tb, sideGeoInfo);

            tb->SetText(L"输入文字");

            auto b = MakeUIObject<Button>(L"编辑", D2D1_RECT_F{}, 5.0f);

            sideGeoInfo.axis.x = { 0, 2 };
            sideGeoInfo.axis.y = { 1, 1 };

            sideLayout->AddElement(b, sideGeoInfo);

            auto ob = MakeUIObject<OutlinedButton>(L"编辑", D2D1_RECT_F{}, 5.0f);

            sideGeoInfo.axis.x = { 2, 2 };
            sideGeoInfo.axis.y = { 1, 1 };

            sideLayout->AddElement(ob, sideGeoInfo);

            auto fb = MakeUIObject<FilledButton>(L"编辑", D2D1_RECT_F{}, 5.0f);

            sideGeoInfo.axis.x = { 0, 2 };
            sideGeoInfo.axis.y = { 2, 1 };

            sideLayout->AddElement(fb, sideGeoInfo);

            auto eb = MakeUIObject<ElevatedButton>(L"编辑", D2D1_RECT_F{}, 5.0f);

            sideGeoInfo.axis.x = { 2, 2 };
            sideGeoInfo.axis.y = { 2, 1 };

            sideLayout->AddElement(eb, sideGeoInfo);

            auto darkModeLabel = MakeUIObject<Label>(L"暗黑模式", D2D1_RECT_F{});

            sideGeoInfo.axis.x = { 0, 2 };
            sideGeoInfo.axis.y = { 3, 1 };

            sideLayout->AddElement(darkModeLabel, sideGeoInfo);

            auto oos = MakeUIObject<OnOffSwitch>();
            {
                auto originValue = sideGeoInfo.isFixedSize;
                sideGeoInfo.isFixedSize = true;
                sideGeoInfo.axis.x = { 2, 1 };
                sideGeoInfo.axis.y = { 3, 1 };

                sideLayout->AddElement(oos, sideGeoInfo);
                sideGeoInfo.isFixedSize = originValue;
            }
            oos->f_onStateChangeAfter = [&](OnOffSwitch* oos, OnOffSwitch::Event& e)
            {
                if (e.SwitchOff())
                {
                    Application::APP->ChangeTheme(L"Light");
                    Application::APP->MainRenderer()->sceneColor = { 0.95f, 0.95f, 0.95f, 1.0f };
                }
                else if (e.SwitchOn())
                {
                    Application::APP->ChangeTheme(L"Dark");
                    Application::APP->MainRenderer()->sceneColor = { 0.15f, 0.15f, 0.15f, 1.0f };
                }
            };
        });
    }
    catch (RuntimeError& e)
    {
        MessageBox(nullptr, e.Message().c_str(), L"RuntimeError", MB_OK | MB_ICONERROR);
    }
    catch (std::exception& e)
    {
        MessageBoxA(nullptr, e.what(), "std::exception", MB_OK | MB_ICONERROR);
    }
    exit(EXIT_FAILURE);
}