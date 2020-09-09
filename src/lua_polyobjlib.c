// SONIC ROBO BLAST 2
//-----------------------------------------------------------------------------
// Copyright (C) 2020 by Iestyn "Monster Iestyn" Jealous.
// Copyright (C) 2020 by Sonic Team Junior.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  lua_polyobjlib.c
/// \brief polyobject library for Lua scripting

#include "doomdef.h"
#include "fastcmp.h"
#include "p_local.h"
#include "p_polyobj.h"
#include "lua_script.h"
#include "lua_libs.h"
#include "lua_hud.h" // hud_running errors

#define NOHUD if (hud_running)\
return luaL_error(L, "HUD rendering code should not call this function!");

enum polyobj_e {
	// properties
	polyobj_valid = 0,
	polyobj_id,
	polyobj_parent,
	polyobj_sector,
	polyobj_angle,
	polyobj_damage,
	polyobj_thrust,
	polyobj_flags,
	polyobj_translucency,
	polyobj_triggertag,
	// special functions - utility
	polyobj_pointInside,
	polyobj_mobjTouching,
	polyobj_mobjInside,
	// special functions - manipulation
	polyobj_moveXY,
	polyobj_rotate
};
static const char *const polyobj_opt[] = {
	// properties
	"valid",
	"id",
	"parent",
	"sector",
	"angle",
	"damage",
	"thrust",
	"flags",
	"translucency",
	"triggertag",
	// special functions - utility
	"pointInside",
	"mobjTouching",
	"mobjInside",
	// special functions - manipulation
	"moveXY",
	"rotate",
	NULL};

// special functions - utility
static int lib_polyobj_PointInside(lua_State *L)
{
	polyobj_t *po = *((polyobj_t **)luaL_checkudata(L, 1, META_POLYOBJ));
	fixed_t x = luaL_checkfixed(L, 2);
	fixed_t y = luaL_checkfixed(L, 3);
	INLEVEL
	if (!po)
		return LUA_ErrInvalid(L, "polyobj_t");
	lua_pushboolean(L, P_PointInsidePolyobj(po, x, y));
	return 1;
}

static int lib_polyobj_MobjTouching(lua_State *L)
{
	polyobj_t *po = *((polyobj_t **)luaL_checkudata(L, 1, META_POLYOBJ));
	mobj_t *mo = *((mobj_t **)luaL_checkudata(L, 2, META_MOBJ));
	INLEVEL
	if (!po)
		return LUA_ErrInvalid(L, "polyobj_t");
	if (!mo)
		return LUA_ErrInvalid(L, "mobj_t");
	lua_pushboolean(L, P_MobjTouchingPolyobj(po, mo));
	return 1;
}

static int lib_polyobj_MobjInside(lua_State *L)
{
	polyobj_t *po = *((polyobj_t **)luaL_checkudata(L, 1, META_POLYOBJ));
	mobj_t *mo = *((mobj_t **)luaL_checkudata(L, 2, META_MOBJ));
	INLEVEL
	if (!po)
		return LUA_ErrInvalid(L, "polyobj_t");
	if (!mo)
		return LUA_ErrInvalid(L, "mobj_t");
	lua_pushboolean(L, P_MobjInsidePolyobj(po, mo));
	return 1;
}

// special functions - manipulation
static int lib_polyobj_moveXY(lua_State *L)
{
	polyobj_t *po = *((polyobj_t **)luaL_checkudata(L, 1, META_POLYOBJ));
	fixed_t x = luaL_checkfixed(L, 2);
	fixed_t y = luaL_checkfixed(L, 3);
	boolean checkmobjs = lua_opttrueboolean(L, 4);
	NOHUD
	INLEVEL
	if (!po)
		return LUA_ErrInvalid(L, "polyobj_t");
	lua_pushboolean(L, Polyobj_moveXY(po, x, y, checkmobjs));
	return 1;
}

static int lib_polyobj_rotate(lua_State *L)
{
	polyobj_t *po = *((polyobj_t **)luaL_checkudata(L, 1, META_POLYOBJ));
	angle_t delta = luaL_checkangle(L, 2);
	UINT8 turnthings = (UINT8)luaL_optinteger(L, 3, 0); // don't turn anything by default? (could change this if not desired)
	boolean checkmobjs = lua_opttrueboolean(L, 4);
	NOHUD
	INLEVEL
	if (!po)
		return LUA_ErrInvalid(L, "polyobj_t");
	lua_pushboolean(L, Polyobj_rotate(po, delta, turnthings, checkmobjs));
	return 1;
}

static int polyobj_get(lua_State *L)
{
	polyobj_t *polyobj = *((polyobj_t **)luaL_checkudata(L, 1, META_POLYOBJ));
	enum polyobj_e field = luaL_checkoption(L, 2, NULL, polyobj_opt);

	if (!polyobj) {
		if (field == polyobj_valid) {
			lua_pushboolean(L, false);
			return 1;
		}
		return LUA_ErrInvalid(L, "polyobj_t");
	}

	switch (field)
	{
	// properties
	case polyobj_valid:
		lua_pushboolean(L, true);
		break;
	case polyobj_id:
		lua_pushinteger(L, polyobj->id);
		break;
	case polyobj_parent:
		lua_pushinteger(L, polyobj->parent);
		break;
	case polyobj_sector: // shortcut that exists only in Lua!
		LUA_PushUserdata(L, polyobj->lines[0]->backsector, META_SECTOR);
		break;
	case polyobj_angle:
		lua_pushangle(L, polyobj->angle);
		break;
	case polyobj_damage:
		lua_pushinteger(L, polyobj->damage);
		break;
	case polyobj_thrust:
		lua_pushfixed(L, polyobj->thrust);
		break;
	case polyobj_flags:
		lua_pushinteger(L, polyobj->flags);
		break;
	case polyobj_translucency:
		lua_pushinteger(L, polyobj->translucency);
		break;
	case polyobj_triggertag:
		lua_pushinteger(L, polyobj->triggertag);
		break;
	// special functions - utility
	case polyobj_pointInside:
		lua_pushcfunction(L, lib_polyobj_PointInside);
		break;
	case polyobj_mobjTouching:
		lua_pushcfunction(L, lib_polyobj_MobjTouching);
		break;
	case polyobj_mobjInside:
		lua_pushcfunction(L, lib_polyobj_MobjInside);
		break;
	// special functions - manipulation
	case polyobj_moveXY:
		lua_pushcfunction(L, lib_polyobj_moveXY);
		break;
	case polyobj_rotate:
		lua_pushcfunction(L, lib_polyobj_rotate);
		break;
	}
	return 1;
}

static int polyobj_set(lua_State *L)
{
	return luaL_error(L, LUA_QL("polyobj_t") " struct cannot be edited by Lua."); // this is just temporary
}

static int polyobj_num(lua_State *L)
{
	polyobj_t *polyobj = *((polyobj_t **)luaL_checkudata(L, 1, META_POLYOBJ));
	if (!polyobj)
		return luaL_error(L, "accessed polyobj_t doesn't exist anymore.");
	lua_pushinteger(L, polyobj-PolyObjects);
	return 1;
}

static int lib_iteratePolyObjects(lua_State *L)
{
	INT32 i = -1;
	if (lua_gettop(L) < 2)
	{
		//return luaL_error(L, "Don't call PolyObjects.iterate() directly, use it as 'for polyobj in PolyObjects.iterate do <block> end'.");
		lua_pushcfunction(L, lib_iteratePolyObjects);
		return 1;
	}
	lua_settop(L, 2);
	lua_remove(L, 1); // state is unused.
	if (!lua_isnil(L, 1))
		i = (INT32)(*((polyobj_t **)luaL_checkudata(L, 1, META_POLYOBJ)) - PolyObjects);
	for (i++; i < numPolyObjects; i++)
	{
		LUA_PushUserdata(L, &PolyObjects[i], META_POLYOBJ);
		return 1;
	}
	return 0;
}

static int lib_PolyObject_getfornum(lua_State *L)
{
	INT32 id = (INT32)luaL_checkinteger(L, 1);

	if (!numPolyObjects)
		return 0; // if there's no PolyObjects then bail out here

	LUA_PushUserdata(L, Polyobj_GetForNum(id), META_POLYOBJ);
	return 1;
}

static int lib_getPolyObject(lua_State *L)
{
	const char *field;
	INT32 i;

	// find PolyObject by number
	if (lua_type(L, 2) == LUA_TNUMBER)
	{
		i = luaL_checkinteger(L, 2);
		if (i < 0 || i >= numPolyObjects)
			return luaL_error(L, "PolyObjects[] index %d out of range (0 - %d)", i, numPolyObjects-1);
		LUA_PushUserdata(L, &PolyObjects[i], META_POLYOBJ);
		return 1;
	}

	field = luaL_checkstring(L, 2);
	// special function iterate
	if (fastcmp(field,"iterate"))
	{
		lua_pushcfunction(L, lib_iteratePolyObjects);
		return 1;
	}
	// find PolyObject by ID
	else if (fastcmp(field,"GetForNum")) // name could probably be better
	{
		lua_pushcfunction(L, lib_PolyObject_getfornum);
		return 1;
	}
	return 0;
}

static int lib_numPolyObjects(lua_State *L)
{
	lua_pushinteger(L, numPolyObjects);
	return 1;
}

int LUA_PolyObjLib(lua_State *L)
{
	luaL_newmetatable(L, META_POLYOBJ);
		lua_pushcfunction(L, polyobj_get);
		lua_setfield(L, -2, "__index");

		lua_pushcfunction(L, polyobj_set);
		lua_setfield(L, -2, "__newindex");

		lua_pushcfunction(L, polyobj_num);
		lua_setfield(L, -2, "__len");
	lua_pop(L,1);

	lua_newuserdata(L, 0);
		lua_createtable(L, 0, 2);
			lua_pushcfunction(L, lib_getPolyObject);
			lua_setfield(L, -2, "__index");

			lua_pushcfunction(L, lib_numPolyObjects);
			lua_setfield(L, -2, "__len");
		lua_setmetatable(L, -2);
	lua_setglobal(L, "PolyObjects");
	return 0;
}