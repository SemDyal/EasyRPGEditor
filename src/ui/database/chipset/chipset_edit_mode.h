/*
 * This file is part of EasyRPG Editor.
 *
 * EasyRPG Editor is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * EasyRPG Editor is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with EasyRPG Editor. If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

namespace Chipset {
    enum EditMode {
        EDIT_MODE_TERRAIN = 0,
        EDIT_MODE_PASSABILITY = 1,
        EDIT_MODE_EDGES = 2,
        EDIT_MODE_COUNTER = 3
    };
    enum PassableFlag {
        Down		= 0x01,
        Left		= 0x02,
        Right		= 0x04,
        Up      	= 0x08,
        Above		= 0x10,
        Wall		= 0x20,
        Counter		= 0x40
    };
}
