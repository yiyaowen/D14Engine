#include "Common/Precompile.h"

#include "Common/RuntimeError.h"
using namespace d14engine;

#include "Renderer/Camera.h"
#include "Renderer/Renderer.h"
using namespace d14engine::renderer;

#include "UIKit/AnimTextBox.h"
#include "UIKit/Application.h"
#include "UIKit/BitmapUtils.h"
#include "UIKit/Button.h"
#include "UIKit/CheckBox.h"
#include "UIKit/ConstraintLayout.h"
#include "UIKit/ElevatedButton.h"
#include "UIKit/FilledButton.h"
#include "UIKit/GridLayout.h"
#include "UIKit/ListView.h"
#include "UIKit/OnOffSwitch.h"
#include "UIKit/OutlinedButton.h"
#include "UIKit/ScrollView.h"
#include "UIKit/TabGroup.h"
#include "UIKit/ToggleButton.h"
#include "UIKit/Window.h"
using namespace d14engine::uikit;

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

            w->maximalHeight = w->Height();

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

            mainLayout->f_onParentSizeAfter = [&](Panel* p, SizeEvent& e)
            {
                auto parent = p->Parent();
                if (!parent.expired())
                {
                    auto wptr = std::dynamic_pointer_cast<Window>(parent.lock());
                    if (wptr != nullptr) p->Resize(wptr->Width(), wptr->ClientAreaHeight());
                }
            };

            // Tab Group

            auto tabGroup = MakeUIObject<TabGroup>(D2D1_RECT_F{ 0.0f, 0.0f, 300.0f, 0.0f });

            tabGroup->isLeftResizable = true;
            tabGroup->SetCardWidth(120);

            auto scrollView = MakeUIObject<ScrollView>(D2D1_RECT_F{});
            auto listView = MakeUIObject<ListView>(D2D1_RECT_F{});

            tabGroup->AppendPage({ MakeUIObject<Label>(L"新标签页", D2D1_RECT_F{}), scrollView });
            tabGroup->AppendPage({ MakeUIObject<Label>(L"列表视图", D2D1_RECT_F{}), listView });
            tabGroup->SelectPage(0);

            ConstraintLayout::GeometryInfo mainGeoInfo = {};

            mainGeoInfo.Right.ToRight = 0.0f;
            mainGeoInfo.Top.ToTop = 40.0f;
            mainGeoInfo.Bottom.ToBottom = 0.0f;
            mainGeoInfo.keepHeight = false;

            mainLayout->AddElement(tabGroup, mainGeoInfo);

            tabGroup->maximalWidth = tabGroup->Width();
            tabGroup->maximalHeight = tabGroup->Height();

            // Scroll View

            auto sideLayout = MakeUIObject<GridLayout>(tabGroup->SelfCoordRect(), 4, 8, 12.0f, 12.0f);

            scrollView->SetContent(sideLayout);

            auto l = MakeUIObject<Label>(L"名称", D2D1_RECT_F{});

            GridLayout::GeometryInfo sideGeoInfo = {};

            sideGeoInfo.isFixedSize = false;
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

            sideGeoInfo.isFixedSize = false;
            sideGeoInfo.axis.x = { 1, 3 };
            sideGeoInfo.axis.y = { 0, 1 };

            sideLayout->AddElement(tb, sideGeoInfo);

            tb->SetText(L"输入窗口标题");

            auto b = MakeUIObject<FlatButton>(L"编辑", D2D1_RECT_F{}, 5.0f);

            sideGeoInfo.isFixedSize = false;
            sideGeoInfo.axis.x = { 0, 2 };
            sideGeoInfo.axis.y = { 1, 1 };

            sideLayout->AddElement(b, sideGeoInfo);

            auto ob = MakeUIObject<OutlinedButton>(L"编辑", D2D1_RECT_F{}, 5.0f);

            sideGeoInfo.isFixedSize = false;
            sideGeoInfo.axis.x = { 2, 2 };
            sideGeoInfo.axis.y = { 1, 1 };

            sideLayout->AddElement(ob, sideGeoInfo);

            auto fb = MakeUIObject<FilledButton>(L"编辑", D2D1_RECT_F{}, 5.0f);

            sideGeoInfo.isFixedSize = false;
            sideGeoInfo.axis.x = { 0, 2 };
            sideGeoInfo.axis.y = { 2, 1 };

            sideLayout->AddElement(fb, sideGeoInfo);

            auto eb = MakeUIObject<ElevatedButton>(L"编辑", D2D1_RECT_F{}, 5.0f);

            sideGeoInfo.isFixedSize = false;
            sideGeoInfo.axis.x = { 2, 2 };
            sideGeoInfo.axis.y = { 2, 1 };

            sideLayout->AddElement(eb, sideGeoInfo);

            auto darkModeLabel = MakeUIObject<Label>(L"暗黑模式", D2D1_RECT_F{});

            sideGeoInfo.isFixedSize = false;
            sideGeoInfo.axis.x = { 0, 2 };
            sideGeoInfo.axis.y = { 3, 1 };

            sideLayout->AddElement(darkModeLabel, sideGeoInfo);

            auto oos = MakeUIObject<OnOffSwitch>();

            sideGeoInfo.isFixedSize = true;
            sideGeoInfo.axis.x = { 2, 1 };
            sideGeoInfo.axis.y = { 3, 1 };

            sideLayout->AddElement(oos, sideGeoInfo);

            oos->f_onStateChangeAfter = [&](OnOffSwitch::StatefulObjectType* oos, OnOffSwitch::StatefulObjectType::Event& e)
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

            auto dscb = MakeUIObject<CheckBox>();

            dscb->f_onStateChangeAfter =
                [&, bptr = (WeakPtr<FlatButton>)b, obptr = (WeakPtr<OutlinedButton>)ob,
                fbptr = (WeakPtr<FilledButton>)fb, ebptr = (WeakPtr<ElevatedButton>)eb]
                (CheckBox::StatefulObjectType* cb, CheckBox::StatefulObjectType::Event& e)
            { 
                if (e.Unchecked())
                {
                    if (!bptr.expired()) bptr.lock()->SetEnabled(true);
                    if (!obptr.expired()) obptr.lock()->SetEnabled(true);
                    if (!fbptr.expired()) fbptr.lock()->SetEnabled(true);
                    if (!ebptr.expired()) ebptr.lock()->SetEnabled(true);
                }
                else if (e.Checked())
                {
                    if (!bptr.expired()) bptr.lock()->SetEnabled(false);
                    if (!obptr.expired()) obptr.lock()->SetEnabled(false);
                    if (!fbptr.expired()) fbptr.lock()->SetEnabled(false);
                    if (!ebptr.expired()) ebptr.lock()->SetEnabled(false);
                }
            };

            sideGeoInfo.isFixedSize = true;
            sideGeoInfo.axis.x = { 0, 1 };
            sideGeoInfo.axis.y = { 4, 1 };

            sideLayout->AddElement(dscb, sideGeoInfo);

            auto doubleStateLabel = MakeUIObject<Label>(L"禁用按钮", D2D1_RECT_F{});

            doubleStateLabel->alignment.horizontal = DWRITE_TEXT_ALIGNMENT_LEADING;

            sideGeoInfo.isFixedSize = false;
            sideGeoInfo.axis.x = { 1, 3 };
            sideGeoInfo.axis.y = { 4, 1 };

            sideLayout->AddElement(doubleStateLabel, sideGeoInfo);

            auto tscb = MakeUIObject<CheckBox>(true);

            sideGeoInfo.isFixedSize = true;
            sideGeoInfo.axis.x = { 0, 1 };
            sideGeoInfo.axis.y = { 5, 1 };

            sideLayout->AddElement(tscb, sideGeoInfo);

            auto tripleStateLabel = MakeUIObject<Label>(L"Triple-State", D2D1_RECT_F{});

            tripleStateLabel->alignment.horizontal = DWRITE_TEXT_ALIGNMENT_LEADING;

            sideGeoInfo.isFixedSize = false;
            sideGeoInfo.axis.x = { 1, 3 };
            sideGeoInfo.axis.y = { 5, 1 };

            sideLayout->AddElement(tripleStateLabel, sideGeoInfo);

            auto toggleButton = MakeUIObject<ToggleButton>(L"状态", D2D1_RECT_F{}, 5.0f);

            sideGeoInfo.axis.x = { 0, 2 };
            sideGeoInfo.axis.y = { 6, 1 };

            sideLayout->AddElement(toggleButton, sideGeoInfo);

            // List View

            for (int i = 0; i < 30; ++i)
            {
                listView->AppendItem(MakeUIObject<ListViewItem>(
                    D2D1_RECT_F{ 0.0f, 0.0f, 0.0f, 30.0f },
                    MakeUIObject<Label>(std::to_wstring(i), D2D1_RECT_F{})));
            }
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