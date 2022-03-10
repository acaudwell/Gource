/*
    Copyright (C) 2009 Andrew Caudwell (acaudwell@gmail.com)

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version
    3 of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "file_colours.h"
#include <iostream>

RFileColours gRFileColours;

std::string RFileColours::find(std::string ext) {
    auto it = colours.find(ext);
    if(it != colours.end()) {
        return it->second;
    }

    return "#ffffff";
}

void RFileColours::init() {
    colours["c"] = "#555555";
    colours["h"] = "#555555";
    colours["cpp"] = "#f34b7d";
    colours["hpp"] = "#f34b7d";
    colours["html"] = "#e34c26";
    colours["xml"] = "#ff9900";
    colours["svg"] = "#ff9900";
    colours["go"] = "#00add8";
    colours["mod"] = "#00add8";
    colours["sum"] = "#00add8";
    colours["scss"] = "#c6538c";
    colours["sass"] = "#c6538c";
    colours["css"] = "#563d7c";
    colours["js"] = "#f1e05a";
    colours["ts"] = "#2b7489";
    colours["sh"] = "#89e051";
    colours["bash"] = "#89e051";
    colours["json"] = "#292929";
    colours["http"] = "#005C9C";
    colours["yml"] = "#cb171e";
    colours["yaml"] = "#cb171e";
    colours["toml"] = "#9c4221";
    colours["md"] = "#083fa1";
    colours["markdown"] = "#083fa1";
    colours["org"] = "#77aa99";
}
