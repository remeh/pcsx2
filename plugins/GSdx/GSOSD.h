/*
 *	Copyright (C) 2015 Rémy Mathieu
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with GNU Make; see the file COPYING.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA USA.
 *  http://www.gnu.org/copyleft/gpl.html
 *
 */

#pragma once

#include "stdafx.h"
#include <string>
#include <vector>
#include "GSTexture.h"
#include <ft2build.h>
#include FT_FREETYPE_H

#define GSOSD_FONT_SIZE 32
#define GSOSD_MAX_LINES 6 

class GSOSD {
	protected:
		bool active; // is the OSD feature active

		struct OSDLine {
			std::string text;
			// TODO(remy): color ?
			time_t expiration_time;
		};

		std::vector<OSDLine> lines;

		struct GlyphInfo {
			uint32 width;
			uint32 height;

			uint32 left;
			uint32 top;

			uint32 rendered_width; // offset to apply for the next rendering
			uint32 rendered_height; // offset to apply for the next rendering

			uint32 x_offset;
		};

		struct Atlas {
			bool generated;

			uint32 width;
			uint32 height;

			struct GlyphInfo glyphsInfo[96];
		} m_atlas;

		GSTexture* m_atlas_tex;
		GSTexture* m_osd_tex;

		void createAtlas();
		void destroyRes();

		FT_Library ft;
		FT_Face face; // loaded font

	public:
		GSOSD();
		virtual ~GSOSD();

		void init(std::string font_filepath);

		void addLine(std::string text, uint32 seconds);
		void clear();
		virtual void render() = 0;
};
