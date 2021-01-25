#include "CViewsManager.hpp"
#include <fstream>
#include "CLog.h"
#include <memory>
#include <dirent.h>
#include <cstring>
//#include "CMultiThreadScript.h"

CView &CViewsManager::get(size_t id)
{
	return viewList[id];
}

size_t CViewsManager::viewIdByPathAndName(const std::string &path, const std::string &name)
{
	std::string nameF = name + ".lua";

	for (size_t i = 0, size = viewList.size(); i < size; ++i)
	{
		if (viewList[i].getFileName() == nameF)
		{
			return i;
		}
	}

	return -1;
}

CViewsManager &CViewsManager::view()
{
	static CViewsManager views;
	return views;
}

CViewsManager::CViewsManager()
{
	std::string path = "views";
	auto extension_from_filename = [](const std::string &fname)
	{
		size_t s;
		return std::string(((s = fname.find_first_of('.')) != fname.npos) ? (&fname.c_str()[++s]) : (""));
	};

	DIR *direntd = opendir((std::string("./") + path).c_str());
	dirent *rrd = nullptr;

	if (direntd)
	{
		rrd = readdir(direntd);
		while ((rrd = readdir(direntd)) != nullptr)
		{
			if ((rrd->d_type & DT_DIR) == 0 && extension_from_filename(rrd->d_name) == "lua")
			{
				CLog::log() << ("Loading <<" + path + "/" + rrd->d_name + ">>");
				viewList.push_back(CView(path, rrd->d_name));
			}
		}
		closedir(direntd);
	}

}

void CView::parseAndLoadAsScript(std::fstream &fileStream)
{
	size_t s = 0xFFFF;
	std::unique_ptr<char[]> buffer(new char[s + 1]);

	bool luaScript = false;

	bool addedCall = false;

	bool printOpen = false;

	int lineNum = 0;

	auto calcPtrSub = [](void *ptr1, void *ptr2)
	{
		uintptr_t p1 = (uintptr_t)ptr1, p2 = (uintptr_t)ptr2;
		return p1 - p2;
	};

	while (!fileStream.getline(buffer.get(), s).fail())
	{
		char *p = buffer.get();

		if (lineNum > 0)
		{
			if (addedCall)
			{
				memoryScriptBuffer.push_back('\\');
				memoryScriptBuffer.push_back('n');
			}
			else
			{
				memoryScriptBuffer.push_back('\n');
			}
		}

		while (*p)
		{
			if (strncmp(p, "<?lua", sizeof("<?lua") - 1) == 0)
			{
				if (luaScript)
				{
					CLog::log() << "Unexpected <?lua at line " + std::to_string(lineNum) + " : " + std::to_string(calcPtrSub(p, buffer.get()));
					break;
				}

				luaScript = true;

				if (addedCall)
				{
					memoryScriptBuffer.push_back('"');
					memoryScriptBuffer.push_back(')');
					memoryScriptBuffer.push_back('\n');

					addedCall = false;
				}

				p += sizeof("<?lua") - 1;

				continue;
			}

			if (!luaScript && !printOpen && strncmp(p, "{{! ", sizeof("{{! ") - 1) == 0)
			{
				if (addedCall)
				{
					memoryScriptBuffer.push_back('"');
					memoryScriptBuffer.push_back(')');
					memoryScriptBuffer.push_back('\n');

					addedCall = false;
				}

				p += sizeof("{{! ") - 1;

				memoryScriptBuffer.push_back('v');
				memoryScriptBuffer.push_back('p');
				memoryScriptBuffer.push_back('(');

				printOpen = true;

				continue;
			}

			if (!luaScript && printOpen && strncmp(p, " !}}", sizeof(" !}}") - 1) == 0)
			{
				printOpen = false;

				p += sizeof(" !}}") - 1;

				memoryScriptBuffer.push_back(')');
				memoryScriptBuffer.push_back('\n');

				continue;
			}

			if (strncmp(p, "?>", sizeof("?>") - 1) == 0)
			{
				if (!luaScript)
				{
					CLog::log() << "Unexpected ?> at line " + std::to_string(lineNum) + " : " + std::to_string(calcPtrSub(p, buffer.get()));
					break;
				}

				luaScript = false;

				p += sizeof("?>") - 1;

				continue;
			}

			if (luaScript || printOpen)
			{
				memoryScriptBuffer.push_back(*p);
			}
			else
			{
				if (!addedCall)
				{
					memoryScriptBuffer.push_back('v');
					memoryScriptBuffer.push_back('p');
					memoryScriptBuffer.push_back('(');
					memoryScriptBuffer.push_back('"');

					addedCall = true;
				}

				if (*p == '"')
				{
					memoryScriptBuffer.push_back('\\');
				}

				if (*p == '\n' || *p == '\r')
				{
					//memoryScriptBuffer.push_back('\\');
					//memoryScriptBuffer.push_back('\n');
				}
				else
				{
					memoryScriptBuffer.push_back(*p);
				}

			}

			++p;
		}

		lineNum++;
	}

	if (addedCall)
	{
		memoryScriptBuffer.push_back('"');
		memoryScriptBuffer.push_back(')');
		memoryScriptBuffer.push_back('\n');

		addedCall = false;
	}

	auto script = CLuaH::Lua().newScriptRBuffer(memoryScriptBuffer, "test");

	memoryScriptBytecodeBuffer = script.dumpBytecode();
}

int CView::vp(lua_State * L)
{
	CLuaFunctions::LuaParams p(L);

	if (p.getNumParams() == 1)
	{
		lua_getglobal(L, "viewTargetPtr");

		int64_t i = lua_tointeger(L, -1);

		std::string &targetStr = *(std::string*)i;

		const char* str = lua_tostring(L, 1);

		if (str)
			targetStr += str;
		else
		{
			if (lua_isboolean(L, 1))
			{
				int bl = lua_toboolean(L, 1);
				targetStr += bl? "true" : "false";
			}
			else
			{
				targetStr += "str failed ";
				const char* s = lua_tostring(L, -1);
				if (s == NULL) s = "Unrecognized Lua error";

				targetStr += s;
			}
		}
	}

	return 0;
}

void CView::registerViewFunctions(lua_State *L)
{
	lua_register(L, "vp", vp);
}

template<class T> void setLuaGlobal(lua_State *L, const std::string &name, const T &value)
{
	CLuaH::customParam(value).pushToLuaStack(L);
	lua_setglobal(L, name.c_str());
}

CLuaH::luaScript CView::instance(std::string &target, const CLuaH::multiVariableParams_t &vars)
{
	auto theScript = CLuaH::Lua().newScriptRBuffer(memoryScriptBytecodeBuffer, "test");

	registerViewFunctions(theScript.luaState);
	
	for (auto &p : vars)
	{
		p.second.pushToLuaStack(theScript.luaState);
		lua_setglobal(theScript.luaState, p.first.c_str());
	}
	
	setLuaGlobal(theScript.luaState, "viewTargetPtr", (int64_t)&target);

	return std::move(theScript);
}

void CView::dump()
{
	std::fstream viewResultTestFile("dump.html", std::ios::out | std::ios::trunc | std::ios::binary);

	viewResultTestFile.write(&memoryScriptBuffer[0], memoryScriptBuffer.size());

	viewResultTestFile.flush();
	viewResultTestFile.close();
}

CView::CView(const std::string &path, const std::string &file)
{
	std::string filePath = path + "/" + file;

	this->path = path;
	this->file = file;

	std::fstream fileStream(filePath, std::ios::binary | std::ios::in);

	if (!fileStream.is_open())
	{
		CLog::log() << "Open " + filePath + " failed";
		return;
	}

	parseAndLoadAsScript(fileStream);
}
