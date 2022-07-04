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
#include <AP_HAL/AP_HAL.h>

#if CONFIG_HAL_BOARD == HAL_BOARD_SITL

#include "AP_RangeFinder_SITL.h"

extern const AP_HAL::HAL& hal;

/*
  constructor - registers instance at top RangeFinder driver
 */
AP_RangeFinder_SITL::AP_RangeFinder_SITL(RangeFinder::RangeFinder_State &_state, AP_RangeFinder_Params &_params, uint8_t instance) :
    AP_RangeFinder_Backend(_state, _params),
    sitl(AP::sitl()),
    _instance(instance)
{}

/*
  update distance_cm
 */
void AP_RangeFinder_SITL::update(void)
{
    const float dist = sitl->get_rangefinder(_instance);

    // negative distance means nothing is connected
    if (is_negative(dist)) {
        state.status = RangeFinder::Status::NoData;
        return;
    }

    state.distance_cm = dist * 100.0f;
    state.last_reading_ms = AP_HAL::millis();

    // update range_valid state based on distance measured
    update_status();
}

#endif
