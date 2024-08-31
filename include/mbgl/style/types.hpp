#pragma once

#include <cstdint>

namespace mbgl {

namespace style {

// TODO: should be in public source.hpp header
enum class SourceType : uint8_t {
    Vector,
    Raster,
    RasterDEM,
    GeoJSON,
    Video,
    Annotations,
    Route,
    Image,
    CustomVector
};

enum class VisibilityType : bool {
    Visible,
    None,
};

// https://www.jianshu.com/p/55158aa236a4

// 端点
enum class LineCapType : uint8_t {
    Round,      // 圆头  c|------
    Butt,       // 齐平   |------
    Square,     // 方头  [|------
};


/*
         /
 Miter  <
         \

         /
 Bevel  |
         \

         /
 Round  c
         \
*/
// 线段中间点
enum class LineJoinType : uint8_t {
    Miter, // 尖角
    Bevel, // 切角
    Round, // 圆角
    // the following two types are for internal use only
    FakeRound,
    FlipBevel
};

enum class RasterResamplingType : bool {
    Linear,
    Nearest
};

enum class HillshadeIlluminationAnchorType : bool {
    Map,
    Viewport
};

enum class TranslateAnchorType : bool {
    Map,
    Viewport
};

enum class RotateAnchorType : bool {
    Map,
    Viewport,
};

enum class CirclePitchScaleType : bool {
    Map,
    Viewport,
};

enum class SymbolPlacementType : uint8_t {
    Point,
    Line,
    LineCenter
};
    
enum class SymbolZOrderType : uint8_t {
    Auto,
    ViewportY,
    Source
};

enum class AlignmentType : uint8_t {
    Map,
    Viewport,
    Auto,
};

enum class TextJustifyType : uint8_t {
    Auto,
    Center,
    Left,
    Right
};

enum class SymbolAnchorType : uint8_t {
    Center,
    Left,
    Right,
    Top,
    Bottom,
    TopLeft,
    TopRight,
    BottomLeft,
    BottomRight
};

using TextVariableAnchorType = SymbolAnchorType;

enum class TextTransformType : uint8_t {
    None,
    Uppercase,
    Lowercase,
};

enum class IconTextFitType : uint8_t {
    None,
    Both,
    Width,
    Height
};

enum class TextWritingModeType : uint8_t {
    Horizontal,
    Vertical
};

enum class LightAnchorType: bool {
    Map,
    Viewport
};

} // namespace style
} // namespace mbgl
