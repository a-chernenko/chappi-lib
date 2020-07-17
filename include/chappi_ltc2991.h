/*

MIT License

Copyright (c) 2019 Alexander Chernenko (achernenko@mail.ru)

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

#pragma once

#include "chappi_base.h"

namespace chappi {

struct ltc2991_data {
  double Tint;
  double V1;
  double V2;
  double V3;
  double V4;
  double V5;
  double V6;
  double V7;
  double V8;
};

enum class ltc2991_channel { _1, _2, _3, _4, _5, _6, _7, _8 };

struct ltc2991_channel_data {
  ltc2991_channel_data() = default;
  ltc2991_channel channel{};
  double value{};
};

namespace detail {
struct ltc2991_counter {
  chips_counter<ltc2991_counter> data;
};
}  // namespace detail

template <typename ErrorType = int, ErrorType NoerrorValue = 0,
          typename DevAddrType = uint8_t, typename AddrType = uint8_t,
          typename ValueType = uint8_t>
class ltc2991 final : public chip_base<ErrorType, NoerrorValue, DevAddrType,
                                       AddrType, ValueType> {
  static constexpr auto _chip_name = "LTC2991";
  detail::ltc2991_counter _counter;

 public:
  CHIP_BASE_RESOLVE

  ltc2991(bool log_enable)
      : ltc2991{(log_enable) ? std::clog.rdbuf() : nullptr} {}
  ltc2991(std::streambuf *buf_ptr = {}, reg_read_fn reg_read = {},
          reg_write_fn reg_write = {})
      : chip_base<error_type, NoerrorValue, dev_addr_type, addr_type,
                  value_type>{buf_ptr} {
    log_created(get_name());
  }
  ~ltc2991() noexcept { log_destroyed(get_name()); }
  int get_num() const noexcept final { return _counter.data.get_num(); }
  int get_counts() const noexcept final { return _counter.data.get_counts(); }
  std::string get_name() const noexcept final {
    return get_name(_chip_name, get_num());
  }
  void enable_all_channels() const {
    value_type value{};
    read(0x01, value);
    value |= 0b11111000;
    write(0x01, value);
  }
  void enable_all_channels(error_type &error) const noexcept {
    helpers::noexcept_void_function<ltc2991, error_type, NoerrorValue,
                                    &ltc2991::enable_all_channels>(this, error);
  }
  void repeated_mode(bool enable) const {
    value_type value{};
    read(0x08, value);
    if (enable) {
      value |= value_type(0b00010000);
    } else {
      value &= value_type(~0b00010000);
    }
    write(0x08, value);
  }
  void repeated_mode(bool enable, error_type &error) const noexcept {
    helpers::noexcept_set_function<ltc2991, error_type, NoerrorValue, bool,
                                   &ltc2991::repeated_mode>(this, enable,
                                                            error);
  }
  void get_temperature(double &value) const {
    value_type lsb{}, msb{};
    read(0x1A, msb);
    read(0x1B, lsb);
    value = ((msb & 0b00011111) << 8 | lsb) / 16.0;
  }
  double get_temperature() const {
    double value{};
    get_temperature(value);
    return value;
  }
  double get_temperature(error_type &error) const noexcept {
    return helpers::noexcept_get_function<ltc2991, error_type, NoerrorValue,
                                          double, &ltc2991::get_temperature>(
        this, error);
  }
  void get_voltage(ltc2991_channel_data &data) const {
    _get_voltage(0x0A + (static_cast<int>(data.channel) << 1), data.value);
  }
  double get_voltage(ltc2991_channel channel) const {
    ltc2991_channel_data data{};
    data.channel = channel;
    get_voltage(data);
    return data.value;
  }
  double get_voltage(ltc2991_channel channel, error_type &error) const
      noexcept {
    ltc2991_channel_data data{};
    data.channel = channel;
    helpers::noexcept_get_function<ltc2991, error_type, NoerrorValue,
                                   ltc2991_channel_data, &ltc2991::get_voltage>(
        this, data, error);
    return data.value;
  }
  void get_data(ltc2991_data &value) const {
    value.Tint = get_temperature();
    value.V1 = get_voltage(ltc2991_channel::_1);
    value.V2 = get_voltage(ltc2991_channel::_2);
    value.V3 = get_voltage(ltc2991_channel::_3);
    value.V4 = get_voltage(ltc2991_channel::_4);
    value.V5 = get_voltage(ltc2991_channel::_5);
    value.V6 = get_voltage(ltc2991_channel::_6);
    value.V7 = get_voltage(ltc2991_channel::_7);
    value.V8 = get_voltage(ltc2991_channel::_8);
  }
  ltc2991_data get_data() const {
    ltc2991_data data{};
    get_data(data);
    return data;
  }
  ltc2991_data get_data(error_type &error) const {
    return helpers::noexcept_get_function<ltc2991, error_type, NoerrorValue,
                                          ltc2991_data, &ltc2991::get_data>(
        this, error);
  }

 private:
  void _get_voltage(addr_type addr_msb, double &value) const {
    value_type val_lsb{}, val_msb{};
    read(addr_msb, val_msb);
    read(value_type(addr_msb + 1), val_lsb);
    value = ((val_msb & 0b00111111) << 8 | val_lsb) * 0.000305180;
  }
};

}  // namespace chappi
