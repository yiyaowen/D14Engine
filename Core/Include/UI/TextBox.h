#pragma once

#include "Precompile.h"

#include "Label.h"
#include "MaskStyle.h"
#include "TextInputObject.h"

namespace d14engine::ui
{
    struct TextBox : Panel, protected MaskStyle, SolidStyle, TextInputObject
    {
        TextBox(const D2D1_RECT_F& rect, float roundRadius = 0.0f);

        D2D1_COLOR_F indicatorColor = (D2D1::ColorF)D2D1::ColorF::Black;
        float indicatorOpacity = 1.0f;

        // The vertical distance between the panel's top/bottom and the indicator's top/bottom.
        float indicatorExternalY = 5.0f;
        float indicatorBlinkIntervalSecs = 0.5f;

        void OnInitializeFinish() override;

    public:
        const Wstring& Text();
        void SetText(WstrViewParam text);
        void SetText(Wstring&& text);

        using CharacterRange = Label::CharacterRange;

        void AppendTextFragment(WstrViewParam fragment);
        void EraseTextFragment(const CharacterRange& range);
        void InsertTextFragment(WstrViewParam fragment, size_t offset);
        void ModifyTextFragment(WstrViewParam fragment, const CharacterRange& range);

        void SetTextForegroundColor(const D2D1_COLOR_F& color);
        void SetTextForegroundOpacity(float opacity);

        void SetTextBackgroundColor(const D2D1_COLOR_F& color);
        void SetTextBackgroundOpacity(float opacity);

        const Wstring& HiliteText();
        void SetHiliteText(WstrViewParam text);
        void SetHiliteText(Wstring&& text);

        const CharacterRange& HiliteTextRange();
        void SetHiliteTextRange(const CharacterRange& range);

        void SetHiliteTextForegroundColor(const D2D1_COLOR_F& color);
        void SetHiliteTextForegroundOpacity(float opacity);

        void SetHiliteTextBackgroundColor(const D2D1_COLOR_F& color);
        void SetHiliteTextBackgroundOpacity(float opacity);

        void SetTextFormat(ComPtrParam<IDWriteTextFormat> format);

        void SetIndicatorPosition(size_t characterOffset);

    protected:
        SharedPtr<Label> m_textLabel = {};

        CharacterRange m_hiliteTextRange = { 0, 0 };

        SharedPtr<Label> m_hiliteTextLabel = {};

        // To keep the text displayed within single line, the label's rect
        // must be set to infinite width (FLT_MAX), so we need to maintain
        // an extra protected field for the actual visible text rectangle.
        D2D1_RECT_F m_visibleTextRect = {};

        D2D1_RECT_F VisibleTextAbsoluteRect();

        D2D1_POINT_2F VisibleTextPosition();
        D2D1_POINT_2F VisibleTextAbsolutePosition();

        float VisibleTextWidth();
        float VisibleTextHeight();

        D2D1_SIZE_F VisibleTextSize();

        // This field decides whether to show the indicator.
        bool m_showIndicator = false;
        // This field is only used to help to blink the indicator.
        // The indicator will only be visible when both of
        // m_showIndicator and m_isIndicatorVisible are true.
        bool m_isIndicatorVisible = true;

        float m_indicatorBlinkElapsedSecs = 0.0f;

        // How many characters ahead of indicator.
        size_t m_indicatorOffsetCount = 0;
        // Indicator's relative horizontal position.
        float m_indicatorPositionX = 0.0f;

        // Hilite range's start index when trigger mouse-move event.
        size_t m_initialCharacterGapIndex = 0;

    public:
        void OnTextChange();
        virtual void OnTextChangeHelper();

        Function<void(TextBox*)>
            f_onTextChangeOverride = {},
            f_onTextChangeBefore = {},
            f_onTextChangeAfter = {};

    protected:
        void TriggerNormalInput(WstrViewParam content);
        void TriggerControlCommands(KeyboardEvent& e);

        void PerformCopyCommand();
        void PerformCutCommand();
        void PerformPasteCommand();

    public:
        // Override interface methods.

        // IDrawObject2D
        void OnRendererUpdateObject2DHelper(Renderer* rndr) override;

        void OnRendererDrawD2D1LayerHelper(Renderer* rndr) override;

        void OnRendererDrawD2D1ObjectHelper(Renderer* rndr) override;

        // Panel
        void OnSizeHelper(SizeEvent& e) override;

        void OnChangeThemeHelper(WstrViewParam themeName) override;

        bool OnGetFocusHelper() override;

        bool OnLoseFocusHelper() override;

        bool OnMouseButtonHelper(MouseButtonEvent& e) override;

        bool OnMouseMoveHelper(MouseMoveEvent& e) override;

        bool OnKeyboardHelper(KeyboardEvent& e) override;

        // TextInputObject
        Optional<COMPOSITIONFORM> GetCompositionForm() override;

        void OnInputStringHelper(WstrViewParam content) override;
    };
}