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

#include "GSOSDOGL.h"
#include "GSTextureOGL.h"

GSOSDOGL::GSOSDOGL(GLuint fbo_read) :
	GSOSD()
	, m_fbo_read(fbo_read)
{
	printf("GSOSDOGL(%d)\n", m_fbo_read);
}

GSOSDOGL::~GSOSDOGL()
{
	printf("~GSOSDOGL()\n");
}

bool GSOSDOGL::generateAtlasTexture()
{
	printf("GSOSD: Generating OGL atlas texture.\n");

	// note that createAtlas has already
	// destroyed the old texture.
	m_atlas_tex = new GSTextureOGL(GSTextureOGL::Texture, m_atlas.width, m_atlas.height, GL_R8, m_fbo_read);

	// NOTE(remy): does NPOT textures affect the perf ?
	for (uint32 i = 32; i < 128; i++) {
		if (FT_Load_Char(face, i, FT_LOAD_RENDER)) {
			printf("GSOSDOGL: can't render %d\n", i);
			continue;
		}

		GlyphInfo glyph = m_atlas.glyphsInfo[i-32];
		GSVector4i r(glyph.x_offset, 0, glyph.x_offset + glyph.width, glyph.height);
		m_atlas_tex->Update(r, face->glyph->bitmap.buffer, glyph.width);
	}

	printf("GSOSDOGL: created a texture of %dx%d\n", m_atlas_tex->GetWidth(), m_atlas_tex->GetHeight());

	m_atlas.generated = true;
	return true;
}

void GSOSDOGL::render()
{
	// TODO(remy): re-render the OSD.
}
