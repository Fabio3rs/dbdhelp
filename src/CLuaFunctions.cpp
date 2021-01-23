#include "CLuaFunctions.hpp"
#include <iostream>
#include "CLog.h"
#include "Lua/lstate.h"

CLuaH::luaScript &lscript(lua_State *L)
{
	return *(CLuaH::luaScript*)L->script;
}

void CLuaFunctions::registerLuaFuncsAPI(std::function<int(lua_State*)> fun)
{
	registerFunctionsAPICBs.push_back(fun);
}

void CLuaFunctions::registerLuaGlobalsAPI(std::function<int(lua_State*)> fun)
{
	registerGlobalsAPICBs.push_back(fun);
}

void CLuaFunctions::registerFrameUpdateAPI(std::function<void(void)> fun)
{
	frameUpdateAPICBs.push_back(fun);
}

CLuaFunctions &CLuaFunctions::LuaF()
{
	static CLuaFunctions LuaF;
	return LuaF;
}

CLuaFunctions::LuaParams::LuaParams(lua_State *state)
{
	L = state;
	num_params = lua_gettop(L);

	ret = 0;
	stck = 1;
	fail_bit = 0;
}

CLuaFunctions::LuaParams &CLuaFunctions::LuaParams::operator<<(const std::string &param)
{
	lua_pushstring(L, param.c_str());
	++ret;

	return *this;
}

CLuaFunctions::LuaParams &CLuaFunctions::LuaParams::operator<<(double param)
{
	lua_pushnumber(L, param);
	++ret;

	return *this;
}

CLuaFunctions::LuaParams &CLuaFunctions::LuaParams::operator<<(int param)
{
	lua_pushinteger(L, param);
	++ret;

	return *this;
}

CLuaFunctions::LuaParams &CLuaFunctions::LuaParams::operator<<(size_t param)
{
	lua_pushinteger(L, param);
	++ret;

	return *this;
}

CLuaFunctions::LuaParams &CLuaFunctions::LuaParams::operator<<(bool param)
{
	lua_pushboolean(L, param);
	++ret;

	return *this;
}

CLuaFunctions::LuaParams &CLuaFunctions::LuaParams::operator>>(std::string &param)
{
	if (stck <= num_params){
		param = lua_tostring(L, stck);
		++stck;
	}
	else
	{
		fail_bit = 1;
	}

	return *this;
}

CLuaFunctions::LuaParams &CLuaFunctions::LuaParams::operator>>(double &param)
{
	if (stck <= num_params){
		param = lua_tonumber(L, stck);
		++stck;
	}
	else
	{
		fail_bit = 1;
	}

	return *this;
}

CLuaFunctions::LuaParams &CLuaFunctions::LuaParams::operator>>(int &param)
{
	if (stck <= num_params){
		param = lua_tointeger(L, stck);
		++stck;
	}
	else
	{
		fail_bit = 1;
	}

	return *this;
}

CLuaFunctions::LuaParams &CLuaFunctions::LuaParams::operator>>(void *&param)
{
	if (stck <= num_params){
		param = (void*)lua_topointer(L, stck);
		++stck;
	}
	else
	{
		fail_bit = 1;
	}

	return *this;
}

CLuaFunctions::LuaParams &CLuaFunctions::LuaParams::operator>>(bool &param)
{
	if (stck <= num_params){
		param = lua_toboolean(L, stck) != 0;
		++stck;
	}
	else
	{
		fail_bit = 1;
	}

	return *this;
}

bool CLuaFunctions::LuaParams::fail()
{
	return fail_bit != 0;
}

int CLuaFunctions::LuaParams::rtn()
{
	return ret;
}

int CLuaFunctions::LuaParams::getNumParams()
{
	return num_params;
}

CLuaFunctions::GameVariables &CLuaFunctions::GameVariables::gv()
{
	static GameVariables gvarsmgr;
	return gvarsmgr;
}

void CLuaFunctions::GameVariables::pushVar(const std::string &name, int &var)
{
	vard v;
	v.ptr = &var;
	v.t = integerv;

	vars[name] = v;
}

void CLuaFunctions::GameVariables::pushVar(const std::string &name, int64_t &var)
{
	vard v;
	v.ptr = &var;
	v.t = integer64;

	vars[name] = v;
}

void CLuaFunctions::GameVariables::pushVar(const std::string &name, double &var)
{
	vard v;
	v.ptr = &var;
	v.t = doublevar;

	vars[name] = v;
}

void CLuaFunctions::GameVariables::pushVar(const std::string &name, std::string &var)
{
	vard v;
	v.ptr = &var;
	v.t = stringvar;

	vars[name] = v;
}

void CLuaFunctions::GameVariables::pushVar(const std::string &name, bool &var)
{
	vard v;
	v.ptr = &var;
	v.t = booleanvar;

	vars[name] = v;
}

void CLuaFunctions::GameVariables::setVar(const std::string &name, int64_t value)
{
	auto &v = vars[name];

	switch (v.t)
	{
	case integerv:
		*(int*)v.ptr = value;
		break;

	case integer64:
		*(int64_t*)v.ptr = value;
		break;

	case doublevar:
		*(double*)v.ptr = value;
		break;

	case booleanvar:
		*(bool*)v.ptr = value != 0;
		break;

	case stringvar:
		*(std::string*)v.ptr = std::to_string(value);
		break;
	}
}

void CLuaFunctions::GameVariables::setVar(const std::string &name, double value)
{
	auto &v = vars[name];

	switch (v.t)
	{
	case integerv:
		*(int*)v.ptr = value;
		break;

	case integer64:
		*(int64_t*)v.ptr = value;
		break;

	case doublevar:
		*(double*)v.ptr = value;
		break;

	case booleanvar:
		*(bool*)v.ptr = value != 0;
		break;

	case stringvar:
		*(std::string*)v.ptr = std::to_string(value);
		break;
	}
}

void CLuaFunctions::GameVariables::setVar(const std::string &name, std::string value)
{
	auto &v = vars[name];

	switch (v.t)
	{
	case integerv:
		*(int*)v.ptr = std::stoi(value);
		break;

	case integer64:
		*(int64_t*)v.ptr = std::stoll(value);
		break;

	case doublevar:
		*(double*)v.ptr = std::stod(value);
		break;

	case booleanvar:
		*(bool*)v.ptr = std::stoi(value) != 0;
		break;

	case stringvar:
		*(std::string*)v.ptr = value;
		break;
	}
}

void CLuaFunctions::GameVariables::setVar(const std::string &name, bool value)
{
	auto &v = vars[name];

	switch (v.t)
	{
	case integerv:
		*(int*)v.ptr = value;
		break;

	case integer64:
		*(int64_t*)v.ptr = value;
		break;

	case doublevar:
		*(double*)v.ptr = value;
		break;

	case booleanvar:
		*(bool*)v.ptr = value;
		break;

	case stringvar:
		*(std::string*)v.ptr = std::to_string(value);
		break;
	}
}

void CLuaFunctions::GameVariables::setVar(const std::string &name, int value)
{
	auto &v = vars[name];

	switch (v.t)
	{
	case integerv:
		*(int*)v.ptr = value;
		break;

	case integer64:
		*(int64_t*)v.ptr = value;
		break;

	case doublevar:
		*(double*)v.ptr = value;
		break;

	case booleanvar:
		*(bool*)v.ptr = value;
		break;

	case stringvar:
		*(std::string*)v.ptr = std::to_string(value);
		break;
	}
}

void CLuaFunctions::GameVariables::setVar(const std::string &name, lua_State *L, int stackIDX)
{
	CLuaH::customParam c;

	c.getFromArgs(L, stackIDX);

	auto &v = vars[name];

	switch (v.t)
	{
	case integerv:
		setVar(name, c.getNumber());
		break;

	case doublevar:
		setVar(name, c.getNumber());
		break;

	case booleanvar:
		setVar(name, c.getBoolean());
		break;

	case stringvar:
		setVar(name, c.getString());
		break;
	}
}

void CLuaFunctions::GameVariables::pushToLuaStack(const std::string &name, lua_State *L)
{
	auto &v = vars[name];
	CLuaH::customParam c;

	switch (v.t)
	{
	case integerv:
		c = CLuaH::customParam(*(int*)v.ptr);
		c.pushToLuaStack(L);
		break;

	case integer64:
		c = CLuaH::customParam(*(int64_t*)v.ptr);
		c.pushToLuaStack(L);
		break;

	case doublevar:
		c = CLuaH::customParam(*(double*)v.ptr);
		c.pushToLuaStack(L);
		break;

	case booleanvar:
		c = CLuaH::customParam(*(bool*)v.ptr);
		c.pushToLuaStack(L);
		break;

	case stringvar:
		c = CLuaH::customParam(*(std::string*)v.ptr);
		c.pushToLuaStack(L);
		break;
	}
}

CLuaFunctions::GameVariables::GameVariables()
{

}

int CLuaFunctions::setGameVar(lua_State *L)
{
	LuaParams p(L);

	if (p.getNumParams() == 2 && lua_isstring(L, 1))
	{
		std::string varname;

		p >> varname;

		GameVariables::gv().setVar(varname, L, 2);
	}

	return p.rtn();
}

int CLuaFunctions::getGameVar(lua_State *L)
{
	LuaParams p(L);

	if (p.getNumParams() == 1 && lua_isstring(L, 1))
	{
		std::string varname;

		p >> varname;

		GameVariables::gv().pushToLuaStack(varname, L);
	}

	return p.rtn();
}

int CLuaFunctions::setConfigs(lua_State *L)
{
	

	return 0;
}

int CLuaFunctions::doNotRunAgain(lua_State *L)
{
	lscript(L).runAgain = false;

	return 0;
}

/*
setCallBackToEvent(event, function)
*/
int CLuaFunctions::setCallBackToEvent(lua_State *L)
{
	LuaParams p(L);

	if (p.getNumParams() == 2 && lua_isstring(L, 1) && lua_isfunction(L, 2)){
		std::string eventName = "teste";
		eventName = lua_tostring(L, 1);

		lua_pushvalue(L, 2);
		int	fnRef = luaL_ref(L, LUA_REGISTRYINDEX);


		lscript(L).callbacks[eventName] = fnRef;
		lscript(L).callbacksAdded = true;
	}


	return p.rtn();
}

/*
* Output a string in game log
*/
int CLuaFunctions::printTolog(lua_State *L)
{
	LuaParams p(L);

	std::string str = "Lua script log: ";

	for (int i = 1, size = p.getNumParams(); i <= size; i++){
		switch (lua_type(L, i))
		{
		case LUA_TNIL:
			str += " <null> ";
			break;

		case LUA_TNUMBER:
			str += " Number<";
			str += std::to_string(lua_tonumber(L, i));
			str += "> ";
			break;

		case LUA_TBOOLEAN:
			str += ((lua_toboolean(L, i) != 0)? " Boolean<True> " : " Boolean<False> ");
			break;

		case LUA_TSTRING:
			str += " String<";
			str += lua_tostring(L, i);
			str += "> ";
			break;

		case LUA_TTABLE:
			str += " <Get LUA_TTABLE: **TODO**> ";
			break;

		case LUA_TFUNCTION:
			str += " <Get LUA_TFUNCTION: **TODO**> ";
			break;

		case LUA_TUSERDATA:
			str += " <Get LUA_TUSERDATA: **TODO**> ";
			break;

		case LUA_TTHREAD:
			str += " <Get LUA_TTHREAD: **TODO**> ";
			break;

		case LUA_TLIGHTUSERDATA:
			str += " <Get LUA_TLIGHTUSERDATA: **TODO**> ";
			break;

		default:
			str += " <unknow argment type> ";
			break;
		}
	}
	CLog::log() << str;

	return 0;
}

int CLuaFunctions::printToString(lua_State * L)
{
	LuaParams p(L);

	int64_t ptr = 0;

	p >> ptr;

	std::string &str = *(std::string*)ptr;
	//str += "Lua script log: ";

	str += lua_tostring(L, 2);
	//CLog::log() << str;

	return 0;
}

void CLuaFunctions::registerFunctions(lua_State *L)
{
	lua_register(L, "setConfigs", setConfigs);
	lua_register(L, "doNotRunAgain", doNotRunAgain);
	lua_register(L, "setCallBackToEvent", setCallBackToEvent);
	lua_register(L, "printTolog", printTolog);
	lua_register(L, "printToString", printToString);
	lua_register(L, "setGameVar", setGameVar);
	lua_register(L, "getGameVar", getGameVar);

	auto &funList = LuaF().registerFunctionsAPICBs;

	for (auto &fun : funList)
	{
		fun(L);
	}
}


template<class T> void setLuaGlobal(lua_State *L, const std::string &name, const T &value)
{
	CLuaH::customParam(value).pushToLuaStack(L);
	lua_setglobal(L, name.c_str());
}

/*
* Register default game globals
*/
void CLuaFunctions::registerGlobals(lua_State *L)
{
	setLuaGlobal(L, "TESTE", "abcde");
	setLuaGlobal(L, "VSYNC_OFF", 0);
	setLuaGlobal(L, "VSYNC_ON", 1);
	setLuaGlobal(L, "VSYNC_HALF", 2);
	setLuaGlobal(L, "VSYNC_AUTO", 3);


	auto &funList = LuaF().registerGlobalsAPICBs;

	for (auto &fun : funList)
	{
		fun(L);
	}
}

CLuaFunctions::CLuaFunctions()
{

}


