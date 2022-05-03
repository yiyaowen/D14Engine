#include "Precompile.h"

#include "Renderer/Camera.h"
#include "Renderer/Renderer.h"
using namespace d14engine::renderer;

#include "UI/Application.h"
#include "UI/BitmapUtils.h"
#include "UI/ElevatedButton.h"
#include "UI/FilledButton.h"
#include "UI/FlatButton.h"
#include "UI/OutlinedButton.h"
#include "UI/Panel.h"
#include "UI/Window.h"
using namespace d14engine::ui;

using namespace d14engine;

int wmain(int argc, wchar_t* argv[])
{
    try
    {
        return Application(argc, argv).Run([&](Application* app)
        {
            app->RNDR->SelectMainCamera(std::make_shared<Camera>(app->RNDR));
            
            auto w = MakeRootUIObject<Window>(
                L"偏好设置", D2D1_RECT_F{ 100.0f, 100.0f, 900.0f, 700.0f });

            w->f_onCloseOverride = [&](Window* w) { Application::APP->Exit(); };
            w->f_onMinimizeOverride = [&](Window* w)
            {
                w->SetEnabled(false);
                w->SetVisible(false);
                w->ForeachChild([&](ShrdPtrParam<Panel> child)
                {
                    child->SetEnabled(false);
                    child->SetVisible(false);
                });

                Application::APP->PinUIObject(w->shared_from_this());
                w->SetKeyboardSensitive(true);
            };
            w->f_onMouseWheelAfter = [&](Panel* p, MouseWheelEvent& e)
            {
                float sign = e.keyState.SHIFT ? +10.0f : -10.0f;
                float afterWidth = p->Width() + e.deltaCount * sign;
                float afterHeight = p->Height() - e.deltaCount * sign;
                float minWidth = p->MinimalWidth();
                float minHeight = p->MinimalHeight();
                minWidth = max(minWidth, afterWidth);
                minHeight = max(minHeight, afterHeight);
                p->Resize(minWidth, minHeight);
                return false;
            };
            w->f_onKeyboardAfter = [&](Panel* p, KeyboardEvent& e)
            {
                if (e.vkey == VK_SPACE && e.status.Released())
                {
                    p->SetEnabled(true);
                    p->SetVisible(true);
                    p->ForeachChild([&](ShrdPtrParam<Panel> child) {
                        child->SetEnabled(true);
                        child->SetVisible(true);
                    });

                    Application::APP->UnpinUIObject(p->shared_from_this());
                }
                return false;
            };

            auto b = MakeManagedUIObject<FlatButton>(w,
                L"编 辑",
                nullptr,
                D2D1_RECT_F{ 100.0f, 100.0f, 250.0f, 150.0f },
                25.0f);

            auto ob = MakeManagedUIObject<OutlinedButton>(w,
                L"编 辑",
                nullptr,
                D2D1_RECT_F{ 100.0f, 200.0f, 250.0f, 250.0f },
                25.0f);

            auto fb = MakeManagedUIObject<FilledButton>(w,
                L"编 辑",
                nullptr,
                D2D1_RECT_F{ 100.0f, 300.0f, 250.0f, 350.0f },
                25.0f);

            auto eb = MakeManagedUIObject<ElevatedButton>(w,
                L"编 辑",
                nullptr,
                D2D1_RECT_F{ 100.0f, 400.0f, 250.0f, 450.0f },
                25.0f);
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