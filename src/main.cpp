#include <iostream>
#include "Program.hpp"
#include "CLuaH.hpp"
#include "CLuaFunctions.hpp"

struct field
{
    std::string name;
    std::string type;
    std::string pkai;
    std::string comment;
};

struct tables
{
    std::string tablename;
    std::string migrationt;
    std::string connectioname;
    std::vector<field> fields;
};

std::deque<tables> tbls;

int create_table(lua_State *L)
{
	CLuaFunctions::LuaParams p(L);

	if (p.getNumParams() == 1 && lua_isstring(L, 1))
	{
        std::string tblname;
		p >> tblname;

        tbls.push_back({tblname, "CREATE"});
        p << tbls.size() - 1;
	}

	return p.rtn();
}

int alter_table(lua_State *L)
{
	CLuaFunctions::LuaParams p(L);

	if (p.getNumParams() == 1 && lua_isstring(L, 1))
	{
        std::string tblname;
		p >> tblname;

        tbls.push_back({tblname, "ALTER"});
        p << tbls.size() - 1;
	}

	return p.rtn();
}

int create_field(lua_State *L)
{
	CLuaFunctions::LuaParams p(L);

	if (p.getNumParams() == 3 && lua_isnumber(L, 1) && lua_isstring(L, 2) && lua_isstring(L, 3))
	{
        uint64_t migid;
        std::string fieldname, type;
		p >> migid >> fieldname >> type;

        tables &tb = tbls[migid];

        tb.fields.push_back({fieldname, type, "", ""});
	}

	return p.rtn();
}

int dump_sql(lua_State *L)
{
	CLuaFunctions::LuaParams p(L);

    if (p.getNumParams() == 1 && lua_isnumber(L, 1))
	{
        uint64_t migid;
		p >> migid;

        tables &tb = tbls[migid];
        
        std::cout << "BEGIN TRANSACTION" << std::endl;
        std::cout << "GO" << std::endl;
        std::cout << tb.migrationt << " TABLE dbo." << tb.tablename << std::endl;
        std::cout << "  (" << std::endl;

        bool fnm = false;
        for (field &f : tb.fields)
        {
            if (fnm)
            {
                std::cout << "," << std::endl;
            }
            fnm = true;
            std::cout << "  " << f.name << " " << f.type;
        }
        std::cout << std::endl;
        std::cout << "  )" << std::endl;
        std::cout << "GO" << std::endl;
        std::cout << "COMMIT" << std::endl;
	}

	return p.rtn();
}

int main(int argc, char *argv[])
{
    migmgr::Program &mgr = migmgr::Program::prog();

    mgr.load();
    mgr.run();
    
    /*CLuaH &lua = CLuaH::Lua();
    CLuaH::luaScript ls = lua.newScript(".", "test.lua");

	lua_register(ls.luaState, "create_table", create_table);
	lua_register(ls.luaState, "create_field", create_field);
	lua_register(ls.luaState, "alter_table", alter_table);
	lua_register(ls.luaState, "dump_sql", dump_sql);

    std::cout << lua.runScript(ls) << std::endl;*/
    return 0;
}

