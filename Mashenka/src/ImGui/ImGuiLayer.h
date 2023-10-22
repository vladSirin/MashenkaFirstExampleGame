﻿#pragma once
#include "Mashenka/Layer.h"

namespace Mashenka
{
    class MASHENKA_API ImGuiLayer: public Layer
    {
    public:
        ImGuiLayer();
        ~ImGuiLayer() override = default;

        void OnAttach() override;
        void OnDetach() override;
        void OnImGuiRender() override; // for render specific things and debugging UI

        // Begin and End for the rendering prepare and execution.
        void Begin();
        void End();

    private:
        // float m_Time = 0.0f;
        
    };
}

