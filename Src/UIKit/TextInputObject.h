#pragma once

#include "Common/Precompile.h"

namespace d14engine::uikit
{
    struct TextInputObject
    {
        virtual Optional<COMPOSITIONFORM> GetCompositionForm()
        {
            // Where to display the input method's composition window?
            return std::nullopt;
        }

        void OnInputString(WstrViewParam content)
        {
            if (f_onInputStringOverride)
            {
                f_onInputStringOverride(this, content);
            }
            else
            {
                if (f_onInputStringBefore) f_onInputStringBefore(this, content);

                OnInputStringHelper(content);

                if (f_onInputStringAfter) f_onInputStringAfter(this, content);
            }
        }

        Function<void(TextInputObject*,WstrViewParam)>
            f_onInputStringOverride = {},
            f_onInputStringBefore = {},
            f_onInputStringAfter = {};

    protected:
        virtual void OnInputStringHelper(WstrViewParam content)
        {
            // TODO: add text-input-object input string logic.
        }
    };
}