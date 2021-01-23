#include "Program.hpp"

int migmgr::Program::select_database(lua_State *L)
{
	CLuaFunctions::LuaParams p(L);

	if (p.getNumParams() == 1 && lua_isstring(L, 1))
	{
        std::string dbname;
		p >> dbname;

        if (prog().dbs.size() == 0)
            prog().dbs.push_back({dbname});

		CLuaH::customParam dbid;

        dbid.setinteger(prog().dbs.size() - 1);
		dbid.pushToLuaStack(L);
		lua_setglobal(L, "_databaseidx");

        p << prog().dbs.size() - 1;
	}

	return p.rtn();
}

int migmgr::Program::create_table(lua_State *L)
{
	CLuaFunctions::LuaParams p(L);

	if (p.getNumParams() == 1 && lua_isstring(L, 1))
	{
        int64_t dblid;
        std::string tblname;
		p  >> tblname;

	    lua_getglobal(L, "_databaseidx");
	    dblid = lua_tointeger(L, -1);

        Program &pr = prog();
        int tblid = pr.dbs[dblid].create_table_if_not_exists(tblname);

        p << tblid;
        //tbls.push_back({tblname, "CREATE"});
        //p << tbls.size() - 1;
	}

	return p.rtn();
}

int migmgr::Program::alter_table(lua_State *L)
{
	CLuaFunctions::LuaParams p(L);

	if (p.getNumParams() == 1 && lua_isstring(L, 1))
	{
        int64_t dblid;
        std::string tblname;
		p  >> tblname;

	    lua_getglobal(L, "_databaseidx");
	    dblid = lua_tointeger(L, -1);

        Program &pr = prog();
        int tblid = pr.dbs[dblid].create_table_if_not_exists(tblname);
        pr.dbs[dblid].tables[tblid].migrations.push_back({0, 0});

        p << tblid;
	}

	return p.rtn();
}

int migmgr::Program::field_id(lua_State *L)
{
	CLuaFunctions::LuaParams p(L);

    if (p.getNumParams() == 1 && lua_isnumber(L, 1))
	{
        uint64_t dblid, tblid;
		p >> tblid;

	    lua_getglobal(L, "_databaseidx");
	    dblid = lua_tointeger(L, -1);

        Program &pr = prog();

        pr.dbs[dblid].tables[tblid].create_field("id", integer, primary_key);

        //tbls.push_back({tblname, "CREATE"});
        //p << tbls.size() - 1;
	}

	return p.rtn();
}

int migmgr::Program::field_timestamps(lua_State *L)
{
	CLuaFunctions::LuaParams p(L);

	if (p.getNumParams() == 1 && lua_isnumber(L, 1))
	{
        uint64_t dblid, tblid;
		p >> tblid;

	    lua_getglobal(L, "_databaseidx");
	    dblid = lua_tointeger(L, -1);

        Program &pr = prog();

        pr.dbs[dblid].tables[tblid].create_field("created_at", datetime, normal);
        pr.dbs[dblid].tables[tblid].create_field("updated_at", datetime, normal);

        //tbls.push_back({tblname, "CREATE"});
        //p << tbls.size() - 1;
	}

	return p.rtn();
}

int migmgr::Program::create_field(lua_State *L)
{
	CLuaFunctions::LuaParams p(L);

	if (p.getNumParams() == 3 && lua_isnumber(L, 1) && lua_isstring(L, 2) && lua_isstring(L, 3))
	{
        static std::map<std::string, field_type> strtoft = {
            {"integer", integer},
            {"string", string},
            {"decimal", decimal},
            {"date", date},
            {"datetime", datetime},
            {"timestamp", timestamp}
        };

        uint64_t dblid, tblid;
        std::string fieldname, type;
		p >> tblid >> fieldname >> type;

	    lua_getglobal(L, "_databaseidx");
	    dblid = lua_tointeger(L, -1);

        Program &pr = prog();

        pr.dbs[dblid].tables[tblid].create_field(fieldname, strtoft[type], normal);
	}

	return p.rtn();
}

int migmgr::Program::dumptbl(lua_State *L)
{
	CLuaFunctions::LuaParams p(L);

	if (p.getNumParams() == 1 && lua_isnumber(L, 1))
	{
        uint64_t dblid, tblid;
		p >> tblid;

	    lua_getglobal(L, "_databaseidx");
	    dblid = lua_tointeger(L, -1);

        Program &pr = prog();
        auto &db = pr.dbs[dblid];
        auto &tb = db.tables[tblid];

        std::cout << "Database: " << db.name << std::endl;
        std::cout << "Table: " << tb.name << std::endl;
        std::cout << "\tNAME\t\tTYPE" << std::endl;
        for (auto &field : tb.fields)
        {
            std::cout << "\t" << field.name << "\t\t" << field.ft << std::endl;
        }

        //tbls.push_back({tblname, "CREATE"});
        //p << tbls.size() - 1;
	}

	return p.rtn();
}

int migmgr::Program::registerFunctions(lua_State* L)
{
	lua_register(L, "select_database", select_database);
	lua_register(L, "create_table", create_table);
	lua_register(L, "alter_table", alter_table);
	lua_register(L, "field_id", field_id);
	lua_register(L, "field_timestamps", field_timestamps);
	lua_register(L, "create_field", create_field);
	lua_register(L, "dumptbl", dumptbl);
    return 0;
}

int migmgr::Program::registerGlobals(lua_State* L)
{

    return 0;
}

bool migmgr::Program::load()
{
    if (loaded)
        return true;
    
    loaded = CLuaH::Lua().loadFilesDequeStorage(migrations_directory, scripts);

    return loaded;
}

void migmgr::Program::run()
{
    if (!loaded)
        return;
    
    CLuaH::Lua().runScriptsFromStorage(scripts);
}

migmgr::Program &migmgr::Program::prog()
{
    static Program inst;
    return inst;
}

migmgr::Program::Program() noexcept : config_script("Project.lua"), loaded(false)
{
    migrations_directory = "./migrations";

    CLuaFunctions::LuaF().registerLuaFuncsAPI(registerFunctions);
    CLuaFunctions::LuaF().registerLuaGlobalsAPI(registerGlobals);
}
