#pragma once
#include <ossia/editor/dataspace/dataspace_base.hpp>
#include <eggs/variant.hpp>
// Algorithms taken from http://www.easyrgb.com
namespace ossia
{

struct argb_u;
struct color_dataspace;
template<typename Impl>
struct color_unit
{
  using is_unit = std::true_type;
  using is_multidimensional = std::true_type; // number of dimensiosn -> decltype(value)::size_value
  using neutral_unit = argb_u;
  using concrete_type = Impl;
  using dataspace_type = color_dataspace;
};

struct argb_u : public color_unit<argb_u>
{
  static OSSIA_DECL_RELAXED_CONSTEXPR auto text()
  { return ossia::make_string_array("argb"); }

  static OSSIA_DECL_RELAXED_CONSTEXPR auto array_parameters()
  { return ossia::make_string_view("argb"); }

  using value_type = Vec4f;

  static OSSIA_DECL_RELAXED_CONSTEXPR strong_value<neutral_unit> to_neutral(strong_value<concrete_type> self)
  {
    return self;
  }

  static OSSIA_DECL_RELAXED_CONSTEXPR value_type from_neutral(strong_value<neutral_unit> self)
  {
    return self.dataspace_value;
  }
};

struct rgba_u : public color_unit<rgba_u>
{
  static OSSIA_DECL_RELAXED_CONSTEXPR auto text()
  { return ossia::make_string_array("rgba"); }

  static OSSIA_DECL_RELAXED_CONSTEXPR auto array_parameters()
  { return ossia::make_string_view("rgba"); }

  using value_type = Vec4f;

  static strong_value<neutral_unit> to_neutral(strong_value<concrete_type> self)
  {
    return {self.dataspace_value[3], self.dataspace_value[0], self.dataspace_value[1], self.dataspace_value[2]};
  }

  static value_type from_neutral(strong_value<neutral_unit> self)
  {
    return {self.dataspace_value[1], self.dataspace_value[2], self.dataspace_value[3], self.dataspace_value[0]};
  }
};

struct rgb_u : public color_unit<rgb_u>
{
  static OSSIA_DECL_RELAXED_CONSTEXPR auto text()
  { return ossia::make_string_array("rgb"); }

  static OSSIA_DECL_RELAXED_CONSTEXPR auto array_parameters()
  { return ossia::make_string_view("rgb"); }

  using value_type = Vec3f;

  static strong_value<neutral_unit> to_neutral(strong_value<concrete_type> self)
  {
    return {1., self.dataspace_value[0], self.dataspace_value[1], self.dataspace_value[2]};
  }

  static value_type from_neutral(strong_value<neutral_unit> self)
  {
    return {self.dataspace_value[1], self.dataspace_value[2], self.dataspace_value[3]};
  }
};

struct bgr_u : public color_unit<bgr_u>
{
  static OSSIA_DECL_RELAXED_CONSTEXPR auto text()
  { return ossia::make_string_array("bgr"); }

  static OSSIA_DECL_RELAXED_CONSTEXPR auto array_parameters()
  { return ossia::make_string_view("bgr"); }

  using value_type = Vec3f;

  static strong_value<neutral_unit> to_neutral(strong_value<concrete_type> self)
  {
    return {1., self.dataspace_value[2], self.dataspace_value[1], self.dataspace_value[0]};
  }

  static value_type from_neutral(strong_value<neutral_unit> self)
  {
    return {self.dataspace_value[3], self.dataspace_value[2], self.dataspace_value[1]};
  }
};

struct argb8_u : public color_unit<argb8_u>
{
  static OSSIA_DECL_RELAXED_CONSTEXPR auto text()
  { return ossia::make_string_array("argb8"); }

  static OSSIA_DECL_RELAXED_CONSTEXPR auto array_parameters()
  { return ossia::make_string_view("argb"); }

  using value_type = Vec4f;

  static strong_value<neutral_unit> to_neutral(strong_value<concrete_type> self)
  {
    return {
      self.dataspace_value[0] / 255.,
      self.dataspace_value[1] / 255.,
      self.dataspace_value[2] / 255.,
      self.dataspace_value[3] / 255.};
  }

  static value_type from_neutral(strong_value<neutral_unit> self)
  {
    return {
      self.dataspace_value[0] * 255.,
      self.dataspace_value[1] * 255.,
      self.dataspace_value[2] * 255.,
      self.dataspace_value[3] * 255.};
  }
};

struct hsv_u : public color_unit<hsv_u>
{
  static OSSIA_DECL_RELAXED_CONSTEXPR auto text()
  { return ossia::make_string_array("hsv"); }

  static OSSIA_DECL_RELAXED_CONSTEXPR auto array_parameters()
  { return ossia::make_string_view("hsv"); }

  using value_type = Vec3f;
  static strong_value<neutral_unit> to_neutral(strong_value<concrete_type> self)
  {
    const auto H = self.dataspace_value[0];
    const auto S = self.dataspace_value[1];
    const auto V = self.dataspace_value[2];
    if ( S == 0. )
    {
      return {1., V, V, V };
    }
    else
    {
      auto var_h = H * 6.;
      if ( var_h == 6. )
        var_h = 0.;      //H must be < 1
      int var_i = var_h;             //Or ... var_i = floor( var_h )
      auto var_1 = V * ( 1. - S );
      auto var_2 = V * ( 1. - S * ( var_h - var_i ) );
      auto var_3 = V * ( 1. - S * ( 1. - ( var_h - var_i ) ) );

      switch(var_i)
      {
        case 0:
          return { 1., V, var_3, var_1 };
        case 1:
          return { 1., var_2, V, var_1 };
        case 2:
          return { 1., var_1, V, var_3 };
        case 3:
          return { 1., var_1, var_2, V };
        case 4:
          return { 1., var_3, var_1, V };
        default:
          return { 1., V, var_1, var_2 };
      }
    }
  }

  static value_type from_neutral(strong_value<neutral_unit> self)
  {
    const auto var_R = self.dataspace_value[1];
    const auto var_G = self.dataspace_value[2];
    const auto var_B = self.dataspace_value[3];

    const auto var_Min = std::min(std::min(var_R, var_G), var_B);    //Min. value of RGB
    const auto var_Max = std::max(std::max( var_R, var_G), var_B );    //Max. value of RGB
    const auto del_Max = var_Max - var_Min;             //Delta RGB value


    if ( del_Max == 0. )                     //This is a gray, no chroma...
    {
      return { 0., 0., var_Max};
    }
    else                                    //Chromatic data...
    {
      double H{};
      auto S = del_Max / var_Max;
      auto V = var_Max;

      auto del_R = ( ( ( var_Max - var_R ) / 6. ) + ( del_Max / 2. ) ) / del_Max;
      auto del_G = ( ( ( var_Max - var_G ) / 6. ) + ( del_Max / 2. ) ) / del_Max;
      auto del_B = ( ( ( var_Max - var_B ) / 6. ) + ( del_Max / 2. ) ) / del_Max;

      if      ( var_R == var_Max ) H = del_B - del_G;
      else if ( var_G == var_Max ) H = ( 1. / 3. ) + del_R - del_B;
      else if ( var_B == var_Max ) H = ( 2. / 3. ) + del_G - del_R;

      if ( H < 0. ) H += 1.;
      if ( H > 1. ) H -= 1.;
      return { H, S, V };
    }
  }

};

struct hsl_u : public color_unit<hsl_u>
{
  static OSSIA_DECL_RELAXED_CONSTEXPR auto text()
  { return ossia::make_string_array("hsl"); }

  static OSSIA_DECL_RELAXED_CONSTEXPR auto array_parameters()
  { return ossia::make_string_view("hsl"); }

  using value_type = Vec3f;

};

struct cmy8_u : public color_unit<cmy8_u>
{
  static OSSIA_DECL_RELAXED_CONSTEXPR auto text()
  { return ossia::make_string_array("cmy8"); }

  static OSSIA_DECL_RELAXED_CONSTEXPR auto array_parameters()
  { return ossia::make_string_view("cmy"); }

  using value_type = Vec3f;

  static strong_value<neutral_unit> to_neutral(strong_value<concrete_type> self)
  {
    return {
          1.,
          (255. - self.dataspace_value[0]) / 255.,
          (255. - self.dataspace_value[1]) / 255.,
          (255. - self.dataspace_value[2]) / 255.};
  }

  static value_type from_neutral(strong_value<neutral_unit> self)
  {
    return {
      255. * (1. - self.dataspace_value[1]),
      255. * (1. - self.dataspace_value[2]),
      255. * (1. - self.dataspace_value[3])};
  }
};

struct cmyk8_u : public color_unit<cmyk8_u>
{
  static OSSIA_DECL_RELAXED_CONSTEXPR auto text()
  { return ossia::make_string_array("cmyk8"); }

  static OSSIA_DECL_RELAXED_CONSTEXPR auto array_parameters()
  { return ossia::make_string_view("cmyk"); }

  using value_type = Vec4f;

};

struct xyz_u : public color_unit<xyz_u>
{
  static OSSIA_DECL_RELAXED_CONSTEXPR auto text()
  { return ossia::make_string_array("xyz"); }

  static OSSIA_DECL_RELAXED_CONSTEXPR auto array_parameters()
  { return ossia::make_string_view("xyz"); }

  using value_type = Vec3f;

  static strong_value<neutral_unit> to_neutral(strong_value<concrete_type> self)
  {
    auto var_X = self.dataspace_value[0] / 100.;        //X from 0 to  95.047      (Observer = 2°, Illuminant = D65)
    auto var_Y = self.dataspace_value[1] / 100.;        //Y from 0 to 100.000
    auto var_Z = self.dataspace_value[2] / 100.;        //Z from 0 to 108.883

    auto var_R = var_X *  3.2406 + var_Y * -1.5372 + var_Z * -0.4986;
    auto var_G = var_X * -0.9689 + var_Y *  1.8758 + var_Z *  0.0415;
    auto var_B = var_X *  0.0557 + var_Y * -0.2040 + var_Z *  1.0570;

    auto translate = [] (auto var) {
      return
          var > 0.0031308
          ? 1.055 * ( std::pow(var, ( 1 / 2.4 )) ) - 0.055
          : var * 12.92;
    };

    return {
          1., translate(var_R), translate(var_G), translate(var_B)
        };
  }

  static value_type from_neutral(strong_value<neutral_unit> self)
  {
    auto translate = [] (auto var) {
      return
          100. * (( var > 0.04045 )
                  ? std::pow( ( var + 0.055 ) / 1.055, 2.4)
                  : var / 12.92);
    };
    auto var_R = translate(self.dataspace_value[1]);
    auto var_G = translate(self.dataspace_value[2]);
    auto var_B = translate(self.dataspace_value[3]);

    //Observer. = 2°, Illuminant = D65

    return {
        var_R * 0.4124 + var_G * 0.3576 + var_B * 0.1805,
        var_R * 0.2126 + var_G * 0.7152 + var_B * 0.0722,
        var_R * 0.0193 + var_G * 0.1192 + var_B * 0.9505
      };
  }
};

struct yxy_u : public color_unit<yxy_u>
{
  static OSSIA_DECL_RELAXED_CONSTEXPR auto text()
  { return ossia::make_string_array("Yxy"); }

  static OSSIA_DECL_RELAXED_CONSTEXPR auto array_parameters()
  { return ossia::make_string_view("Yxy"); }

  using value_type = Vec3f;
};

struct hunter_lab_u : public color_unit<hunter_lab_u>
{
  static OSSIA_DECL_RELAXED_CONSTEXPR auto text()
  { return ossia::make_string_array("hunter_lab"); }
  static OSSIA_DECL_RELAXED_CONSTEXPR auto array_parameters()
  { return ossia::make_string_view("lab"); }
  using value_type = Vec3f;
};

struct cie_lab_u : public color_unit<cie_lab_u>
{
  static OSSIA_DECL_RELAXED_CONSTEXPR auto text()
  { return ossia::make_string_array("cie_lab"); }
  static OSSIA_DECL_RELAXED_CONSTEXPR auto array_parameters()
  { return ossia::make_string_view("lab"); }
  using value_type = Vec3f;
};

struct cie_luv_u : public color_unit<cie_luv_u>
{
  static OSSIA_DECL_RELAXED_CONSTEXPR auto text()
  { return ossia::make_string_array("cie_luv"); }
  static OSSIA_DECL_RELAXED_CONSTEXPR auto array_parameters()
  { return ossia::make_string_view("luv"); }
  using value_type = Vec3f;
};


using color_u =
  eggs::variant<
    argb_u, rgba_u, rgb_u, bgr_u, argb8_u, hsv_u, cmy8_u, xyz_u
/*, hsl_u, cmyk8_u, yxy_u, hunter_lab_u, cie_lab_u, cie_luv_u*/>;

using argb = strong_value<argb_u>;
using rgba = strong_value<rgba_u>;
using rgb = strong_value<rgb_u>;
using bgr = strong_value<bgr_u>;
using argb8 = strong_value<argb8_u>;
using hsv = strong_value<hsv_u>;
using hsl = strong_value<hsl_u>;
using cmy8 = strong_value<cmy8_u>;
using cmyk8 = strong_value<cmyk8_u>;
using xyz = strong_value<xyz_u>;
using yxy = strong_value<yxy_u>;
using hunter_lab = strong_value<hunter_lab_u>;
using cie_lab = strong_value<cie_lab_u>;
using cie_luv = strong_value<cie_luv_u>;

template<>
struct dataspace_traits<color_u>
{
  static OSSIA_DECL_RELAXED_CONSTEXPR auto text()
  { return ossia::make_string_array("color"); }
};
}
