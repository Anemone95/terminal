// Copyright (c) Microsoft Corporation
// Licensed under the MIT license.

#include "pch.h"
#include "ActionAndArgs.h"
#include "JsonUtils.h"
#include "TerminalSettingsSerializationHelpers.h"

#include "EnumMappings.h"
#include "EnumMappings.g.cpp"

using namespace winrt;
using namespace winrt::Windows::Foundation::Collections;
using namespace ::Microsoft::Terminal::Settings::Model;

#define DEFINE_ENUM_MAP(type, name)                                                          \
    winrt::Windows::Foundation::Collections::IMap<winrt::hstring, type> EnumMappings::name() \
    {                                                                                        \
        static IMap<winrt::hstring, type> enumMap = []() {                                   \
            auto map = single_threaded_map<winrt::hstring, type>();                          \
            for (auto [enumStr, enumVal] : JsonUtils::ConversionTrait<type>::mappings)       \
            {                                                                                \
                map.Insert(winrt::to_hstring(enumStr), enumVal);                             \
            }                                                                                \
            return map;                                                                      \
        }();                                                                                 \
        return enumMap;                                                                      \
    }

namespace winrt::Microsoft::Terminal::Settings::Model::implementation
{
    // Global Settings
    DEFINE_ENUM_MAP(winrt::Windows::UI::Xaml::ElementTheme, ElementTheme);
    DEFINE_ENUM_MAP(winrt::Microsoft::UI::Xaml::Controls::TabViewWidthMode, TabViewWidthMode);
    DEFINE_ENUM_MAP(Model::LaunchMode, LaunchMode);

    // Profile Settings
    DEFINE_ENUM_MAP(Model::CloseOnExitMode, CloseOnExitMode);
    DEFINE_ENUM_MAP(Microsoft::Terminal::TerminalControl::ScrollbarState, ScrollbarState);
    DEFINE_ENUM_MAP(Windows::UI::Xaml::Media::Stretch, BackgroundImageStretchMode);
    DEFINE_ENUM_MAP(Microsoft::Terminal::TerminalControl::TextAntialiasingMode, TextAntialiasingMode);
    DEFINE_ENUM_MAP(Microsoft::Terminal::TerminalControl::CursorStyle, CursorStyle);
    DEFINE_ENUM_MAP(Model::BellStyle, BellStyle);
    DEFINE_ENUM_MAP(Model::Alignment, BackgroundImageAlignment);
}