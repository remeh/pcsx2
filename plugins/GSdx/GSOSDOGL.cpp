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

#include "GSDeviceOGL.h"
#include "GSTextureOGL.h"

GSDeviceOGL::GSOSDOGL::GSOSDOGL(GSDeviceOGL* device, GLuint fbo_read) :
	GSOSD()
	, m_device(device)
	, m_fbo_read(fbo_read)
{
	printf("GSOSDOGL(%d)\n", m_fbo_read);
}

GSDeviceOGL::GSOSDOGL::~GSOSDOGL()
{
	printf("~GSOSDOGL()\n");
}

bool GSDeviceOGL::GSOSDOGL::generateAtlasTexture()
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
		if (r.width()) {
			m_atlas_tex->Update(r, face->glyph->bitmap.buffer, glyph.width);
		}
	}

	printf("GSOSDOGL: created an atlas texture of %dx%d\n", m_atlas_tex->GetWidth(), m_atlas_tex->GetHeight());

	m_atlas.generated = true;
	return true;
}

void GSDeviceOGL::GSOSDOGL::render()
{
	if (lines.size() == 0) {
		// TODO(remy): shouldn't we clear m_osd_tex ?
		return;
	}

	// the osd target texture hasn't been prepared yet.
	if (m_osd_tex == NULL) {
		printf("GSOSDGL: the m_osd_tex isn't ready but render() has already be called.");
		return;
	}

	m_device->BeginScene();

	m_device->m_shader->VS(m_device->m_convert.vs);
	m_device->m_shader->GS(0);
	m_device->m_shader->PS(m_device->m_convert.ps[18]);

	m_device->OMSetDepthStencilState(m_device->m_convert.dss);
	//OMSetBlendState(m_convert.bs, 0);
	//m_device->OMSetBlendState(m_device->m_merge_obj.bs, 0);
	m_device->OMSetRenderTargets(m_osd_tex, NULL);

	// FIXME that not efficient but let's do a proof-of-concept first
	GSVector4* vertices = textVertices();

	//m_device->IASetVertexState(m_vb_sr);
	m_device->IASetVertexBuffer(vertices, 6*lines[0].text.size());
	m_device->IASetPrimitiveTopology(GL_TRIANGLES);

	m_device->PSSetShaderResource(0, m_atlas_tex);
	m_device->PSSetSamplerState(m_device->m_convert.pt);

	m_device->DrawPrimitive();

	_aligned_free(vertices);
	m_device->EndScene();
}
