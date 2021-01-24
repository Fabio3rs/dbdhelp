#pragma once
#ifndef _CViewsManager_h_
#define _CViewsManager_h_

#include "CLuaH.hpp"
#include "CLuaFunctions.hpp"
#include <fstream>

class CView
{
	std::string path, file;

	std::vector<char> memoryScriptBuffer;
	std::vector<char> memoryScriptBytecodeBuffer;

	void parseAndLoadAsScript(std::fstream &fileStream);

	static int vp(lua_State *L);

	void registerViewFunctions(lua_State *L);

public:
	CLuaH::luaScript instance(std::string &target, const CLuaH::multiVariableParams_t &vars);

	const std::string &getFileName()
	{
		return file;
	}

	const std::string &getFilePath()
	{
		return path;
	}

	void dump();

	CView(const std::string &path, const std::string &file);
};

class CViewsManager
{
	std::vector<CView> viewList;
public:

	CView &get(size_t id);
	size_t viewIdByPathAndName(const std::string &path, const std::string &name);

	static CViewsManager &view();

private:
	CViewsManager();
	CViewsManager(CViewsManager&) = delete;
};

#endif
