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
}

GSOSDOGL::~GSOSDOGL()
{
}

bool GSOSDOGL::generateAtlasTexture()
{
	printf("GSOSD: Generating OGL atlas texture.\n");

	// note that createAtlas has already
	// destroy the old texture.
	m_atlas_tex = new GSTextureOGL(GSTextureOGL::Texture, atlas.width, atlas.height, GL_R8, m_fbo_read);

	// TODO(remy): generate the atlas texture.

	atlas.generated = true;
	return true;
}

