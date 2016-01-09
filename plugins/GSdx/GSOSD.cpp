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

#include <algorithm>
#include "GSOSD.h"

GSOSD::GSOSD() :
	active(false),
	m_atlas_tex(NULL),
	ft(NULL)
{
	memset(atlas.glyphsInfo, 0, sizeof(atlas.glyphsInfo));
}

GSOSD::~GSOSD()
{
	destroyRes();

	FT_Done_FreeType(ft);
}

// init inits the font rendering dependency,
// create the atlas texture and can decide do
// deactivate the OSD feature.
// TODO(remy): ifdef windows/linux ?
void GSOSD::init(std::string font_filepath)
{
	printf("GSOSD: Entering OSD initialization with %s\n", font_filepath.c_str());
	// NOTE(remy): Is FT available on windows ?
	// I think so, wx use it ?
	FT_Error ftError;
	ftError = FT_Init_FreeType(&ft);
	if (ftError) {
		fprintf(stderr, "GSOSD: can't init freetype : error %d", ftError);
		// TODO(remy): log
		active = false;
		return;
	}

	if (FT_New_Face(ft, font_filepath.c_str(), 0, &face)) {
		// TODO(remy): log
		fprintf(stderr, "GSOSD: can't open the font : %s.", font_filepath.c_str());
		active = false;
		return;
	}

	// set the font size
	FT_Set_Pixel_Sizes(face, 0, GSOSD_FONT_SIZE);

	// create the atlas
	createAtlas();

	// error while generating the atlas ? no atlas, no osd.
	if (!atlas.generated) {
		active = false;
	}
}

// destroyRes releases the memory of the internal object
// of GSOSD.
void GSOSD::destroyRes()
{
	lines.clear();

	if (m_atlas_tex) {
		delete m_atlas_tex;
		m_atlas_tex = NULL;
	}
}

// buildAltas creates an atlas with all the ascii
// chars of the given font. The atlas is stored
// in m_atlas_tex.
// NOTE(remeh): would a POT texture would speed up some things ?
void GSOSD::createAtlas()
{
	// destroy if previously generated.
	if (m_atlas_tex) {
		destroyRes();
	}

	// NOTE(remy): does NPOT textures affect the perf ?

	// we'll regenerate the atlas
	atlas.generated = false;
	atlas.width = atlas.height = 0;

	// compute the char infos and store the height of the taller glyph
	// skip 32 first chars which are control char
	for (int i = 32; i < 128; i++) {
		// Load a char (and render it with FT_LOAD_RENDER flag)
		if (FT_Load_Char(face, i, FT_LOAD_RENDER)) {
			// TODO(remy): log
			active = false;
			return;
		}

		if (!face->glyph) {
			// can't render this glyph ?! consider critical for the OSD subsystem ATM.
			// but something can be done to deal with it.
			// TODO(remy): log
			active = false;
			return;
		}

		// store useful information of the atlas
		atlas.width += face->glyph->bitmap.width;
		atlas.height = std::max(atlas.height, face->glyph->bitmap.rows);

		// store the glyph info
		struct GlyphInfo info;

		info.width = face->glyph->bitmap.width;
		info.height = face->glyph->bitmap.rows;
		info.rendered_width = face->glyph->advance.x >> 6;
		info.rendered_height = face->glyph->advance.y >> 6;

		atlas.glyphsInfo[i-32] = info;
	}
}

// clear removes all the displayed line.
void GSOSD::clear() {
	lines.clear();
}

// addLine inserts a new line to display into the OSD.
// If the OSD has too many lines to display, it can
// decide to remove some.
void GSOSD::addLine(std::string text, uint32 seconds) {
	if (lines.size() == GSOSD_MAX_LINES) {
		lines.erase(lines.begin());
	}

	OSDLine line;
	line.text = text;
	// TODO(remy): compute the expiration time.
	line.expiration_time = seconds;

	lines.push_back(line);
}
