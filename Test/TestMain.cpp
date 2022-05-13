#include "Precompile.h"

#include "Renderer/Camera.h"
#include "Renderer/Renderer.h"
using namespace d14engine::renderer;

#include "UI/AnimTextBox.h"
#include "UI/Application.h"
#include "UI/BitmapUtils.h"
#include "UI/Button.h"
#include "UI/ElevatedButton.h"
#include "UI/FilledButton.h"
#include "UI/OutlinedButton.h"
#include "UI/Panel.h"
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
            app->RENDERER->SelectMainCamera(std::make_shared<Camera>(app->RENDERER));

            auto w = MakeRootUIObject<Window>(
                L"偏好设置", D2D1_RECT_F{ 100.0f, 100.0f, 900.0f, 700.0f });

            w->f_onCloseOverride = [&](Window* w) { Application::APP->Exit(); };
            w->f_onMinimizeOverride = [&](Window* w)
            {
                w->SetEnabled(false);
                w->SetVisible(false);
                w->ForeachChild([&](Panel* child)
                {
                    child->SetEnabled(false);
                    child->SetVisible(false);
                });

                Application::APP->PinUIObject(w->weak_from_this());
                w->SetKeyboardSensitive(true);
            };
            w->f_onMouseWheelAfter = [&](Panel* p, MouseWheelEvent& e)
            {
                float sign = e.keyState.SHIFT ? +10.0f : -10.0f;
                p->Resize(
                    std::max(p->MinimalWidth(), p->Width() + e.deltaCount * sign),
                    std::max(p->MinimalHeight(), p->Height() - e.deltaCount * sign));
                return false;
            };
            w->f_onKeyboardAfter = [&](Panel* p, KeyboardEvent& e)
            {
                if (e.vkey == VK_SPACE && e.status.Released())
                {
                    p->SetEnabled(true);
                    p->SetVisible(true);
                    p->ForeachChild([&](Panel* child)
                    {
                        child->SetEnabled(true);
                        child->SetVisible(true);
                    });

                    Application::APP->UnpinUIObject(p->weak_from_this());
                }
                return false;
            };

            auto b = MakeManagedUIObject<Button>(
                w,
                L"编 辑",
                D2D1_RECT_F{ 100.0f, 100.0f, 250.0f, 150.0f },
                25.0f);

            auto ob = MakeManagedUIObject<OutlinedButton>(
                w,
                L"编 辑",
                D2D1_RECT_F{ 100.0f, 200.0f, 250.0f, 250.0f },
                25.0f);

            auto fb = MakeManagedUIObject<FilledButton>(
                w,
                L"编 辑",
                D2D1_RECT_F{ 100.0f, 300.0f, 250.0f, 350.0f },
                25.0f);

            auto eb = MakeManagedUIObject<ElevatedButton>(
                w,
                L"编 辑",
                D2D1_RECT_F{ 100.0f, 400.0f, 250.0f, 450.0f },
                25.0f);

            auto atb = MakeManagedUIObject<AnimTextBox>(
                w,
                D2D1_RECT_F{ 100.0f, 500.0f, 300.0f, 540.0f },
                20.0f);

            atb->SetText(L"Hello, world! 你好，世界！");

            b->f_onReleaseAfter = [&, atbptr = (WeakPtr<AnimTextBox>)atb](Button* b, Button::Event& e)
            {
                if (e.Left() && !atbptr.expired())
                {
                    atbptr.lock()->AppendTextFragment(L"在这里输入你要搜索的内容");
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