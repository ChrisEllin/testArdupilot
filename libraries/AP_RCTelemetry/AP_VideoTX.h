/*
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#pragma once

#include <AP_HAL/AP_HAL.h>
#include <AP_Param/AP_Param.h>

#define VTX_MAX_CHANNELS 8

class AP_VideoTX {
public:
    AP_VideoTX();
    ~AP_VideoTX();

    /* Do not allow copies */
    AP_VideoTX(const AP_VideoTX &other) = delete;
    AP_VideoTX &operator=(const AP_VideoTX&) = delete;

    // init - perform required initialisation
    bool init();

    // run any required updates
    void update();

    static AP_VideoTX *get_singleton(void) {
        return singleton;
    }
    static const struct AP_Param::GroupInfo var_info[];

    enum class VideoOptions {
        VTX_PITMODE = 1 << 0
    };

    enum VideoBand {
        BAND_A,
        BAND_B,
        BAND_E,
        FATSHARK,
        RACEBAND,
        LOW_RACEBAND,
        MAX_BANDS
    };

    static const uint16_t VIDEO_CHANNELS[MAX_BANDS][VTX_MAX_CHANNELS];

    static uint16_t get_frequency_mhz(uint8_t band, uint8_t channel) { return VIDEO_CHANNELS[band][channel]; }
    static bool get_band_and_channel(uint16_t freq, VideoBand& band, uint8_t& channel);

    void set_frequency_mhz(uint16_t freq) { _frequency_mhz.set_and_save(freq); }
    uint16_t get_frequency_mhz() const { return _frequency_mhz; }
    // get / set power level
    void set_power_mw(uint16_t power) { _current_power = power; }
    void set_power_dbm(uint8_t power);
    void set_configured_power_mw(uint16_t power);
    uint16_t get_configured_power_mw() const { return _power_mw; }
    uint16_t get_power_mw() const { return _current_power; }
    uint8_t get_configured_power_dbm() const;
    bool update_power() const { return _defaults_set && _power_mw != _current_power; }
    // get / set the frequency band
    void set_band(uint8_t band) { _current_band = band; }
    void set_configured_band(uint8_t band) { _band.set_and_save_ifchanged(band); }
    uint8_t get_configured_band() const { return _band; }
    uint8_t get_band() const { return _current_band; }
    bool update_band() const { return _defaults_set && _band != _current_band; }
    // get / set the frequency channel
    void set_channel(uint8_t channel) { _current_channel = channel; }
    void set_configured_channel(uint8_t channel) { _channel.set_and_save_ifchanged(channel); }
    uint8_t get_configured_channel() const { return _channel; }
    uint8_t get_channel() const { return _current_channel; }
    bool update_channel() const { return _defaults_set && _channel != _current_channel; }
    // get / set vtx option
    void set_options(uint8_t options) { _current_options = options; }
    void set_configured_options(uint8_t options) { _options.set_and_save_ifchanged(options); }
    uint8_t get_configured_options() const { return _options; }
    uint8_t get_options() const { return _current_options; }
    bool update_options() const { return _defaults_set && _options != _current_options; }
    // get / set whether the vtx is enabled
    void set_enabled(bool enabled);
    bool get_enabled() const { return _enabled; }
    bool update_enabled() const { return _defaults_set && _enabled != _current_enabled; }

    // have the parameters been updated
    bool have_params_changed() const;
    // set configured defaults from current settings
    void set_defaults();

    static AP_VideoTX *singleton;

private:
    // channel frequency
    AP_Int16 _frequency_mhz;
    
    // power output in mw
    AP_Int16 _power_mw;
    uint16_t _current_power;

    // frequency band
    AP_Int8 _band;
    uint16_t _current_band;

    // frequency channel
    AP_Int8 _channel;
    uint8_t _current_channel;

    // vtx options
    AP_Int8 _options;
    uint8_t _current_options;

    AP_Int8 _enabled;
    bool _current_enabled;

    bool _initialized;
    // when defaults have been configured
    bool _defaults_set;
};

namespace AP {
    AP_VideoTX& vtx();
};