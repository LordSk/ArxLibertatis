/*
 * Copyright 2011-2012 Arx Libertatis Team (see the AUTHORS file)
 *
 * This file is part of Arx Libertatis.
 *
 * Arx Libertatis is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Arx Libertatis is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Arx Libertatis.  If not, see <http://www.gnu.org/licenses/>.
 */
/* Based on:
===========================================================================
ARX FATALIS GPL Source Code
Copyright (C) 1999-2010 Arkane Studios SA, a ZeniMax Media company.

This file is part of the Arx Fatalis GPL Source Code ('Arx Fatalis Source Code'). 

Arx Fatalis Source Code is free software: you can redistribute it and/or modify it under the terms of the GNU General Public 
License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

Arx Fatalis Source Code is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied 
warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Arx Fatalis Source Code.  If not, see 
<http://www.gnu.org/licenses/>.

In addition, the Arx Fatalis Source Code is also subject to certain additional terms. You should have received a copy of these 
additional terms immediately following the terms and conditions of the GNU General Public License which accompanied the Arx 
Fatalis Source Code. If not, please request a copy in writing from Arkane Studios at the address below.

If you have questions concerning this license or the applicable additional terms, you may contact in writing Arkane Studios, c/o 
ZeniMax Media Inc., Suite 120, Rockville, Maryland 20850 USA.
===========================================================================
*/

#ifndef ARX_ANIMATION_ANIMATIONRENDER_H
#define ARX_ANIMATION_ANIMATIONRENDER_H

#include "graphics/Color.h"
#include "math/MathFwd.h"

struct EERIE_3DOBJ;
struct ANIM_USE;
class Entity;
struct EERIEMATRIX;
struct EERIE_QUAT;
struct EERIE_MOD_INFO;
struct EERIE_LIGHT;
struct TexturedVertex;
class TextureContainer;

extern long MAX_LLIGHTS;
extern EERIE_LIGHT * llights[32];

void llightsInit();
void Insertllight(EERIE_LIGHT * el, float dist);
float GetColorz(float x, float y, float z);

float Cedric_GetScale(Entity *io);
float Cedric_GetInvisibility(Entity *io);
bool Cedric_IO_Visible(Vec3f *pos);
void Cedric_ApplyLightingFirstPartRefactor(Entity *io, Color3f &special_color, long &special_color_flag);

void PopAllTriangleList();
void PopAllTriangleListTransparency();

TexturedVertex * PushVertexInTableCull(TextureContainer * tex);
TexturedVertex * PushVertexInTableCull_TNormalTrans(TextureContainer * tex);
TexturedVertex * PushVertexInTableCull_TAdditive(TextureContainer * tex);
TexturedVertex * PushVertexInTableCull_TSubstractive(TextureContainer * tex);
TexturedVertex * PushVertexInTableCull_TMultiplicative(TextureContainer * tex);

void ARX_DrawPrimitive(TexturedVertex *, TexturedVertex *, TexturedVertex *, float _fAdd = 0.0f);

void CalculateInterZMapp(EERIE_3DOBJ * _pobj3dObj, long lIdList, long * _piInd, TextureContainer * _pTex, TexturedVertex * _pVertex);

void EERIEDrawAnimQuat(EERIE_3DOBJ *eobj, ANIM_USE * animlayer, Anglef *angle, Vec3f *pos, unsigned long time, Entity *io, bool render = true, bool update_movement = true);

void AnimatedEntityUpdate(Entity * entity);
void AnimatedEntityRender(Entity * entity);

void DrawEERIEInter(EERIE_3DOBJ *eobj, const EERIE_QUAT *rotation, Vec3f *pos, Entity *io, EERIE_MOD_INFO *modinfo = NULL, bool thrownEntity = false);

#endif // ARX_ANIMATION_ANIMATIONRENDER_H
