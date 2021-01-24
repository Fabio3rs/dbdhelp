#include "Program.hpp"

static int migrationtest = 0;

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
        pr.dbs[dblid].tables[tblid].migrations[0].identifier = ++migrationtest;

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
        pr.dbs[dblid].tables[tblid].migrations.push_back({1, 0});
        pr.dbs[dblid].tables[tblid].migrations.back().identifier = ++migrationtest;

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
        
        FILE *outmodel = fopen((tb.name + ".cs").c_str(), "w");

        fprintf(outmodel, "class %s\n{", tb.name.c_str());
        fprintf(outmodel, "\tbool[] changed = new bool[%ld];\n", tb.fields.size());
        int fieldid = 0;
        for (auto &field : tb.fields)
        {
            std::string varTypename;

            switch(field.ft)
            {
            case integer:
            /*  if (field.unsignedval) varTypename += "unsigned "*/
                varTypename += "int";
                break;

            case string:
                varTypename += "String";
                break;

            case decimal:
                varTypename += "double";
                break;

            case date:
            case datetime:
                varTypename += "DateTime";
                break;

            case timestamp:
                varTypename += "Int64";
                break;
            }

            fprintf(outmodel, "\tprivate %s _%s; // %d\n", varTypename.c_str(), field.name.c_str(), fieldid);
            fprintf(outmodel, "\tpublic %s %s {\n\t\tget { return _%s; }\n\t\tset { changed[%d] = true; _%s = value; }\n\t}\n", varTypename.c_str(), field.name.c_str(), field.name.c_str(), fieldid, field.name.c_str());

            ++fieldid;
        }

        fprintf(outmodel, "\n");
        fprintf(outmodel, "\tpublic void save()\n\t{\n");
        fieldid = 0;
        fprintf(outmodel, "\t\tint paramnum = 0;\n\n");
        fprintf(outmodel, "\t\tString strsqlupdate = \"UPDATE %s SET \";\n", tb.name.c_str());
        for (auto &field : tb.fields)
        {
            if (field.fg != primary_key)
            {
                fprintf(outmodel, "\t\tif (changed[%d])\n\t\t{\n", fieldid);
                fprintf(outmodel, "\t\t\tif (paramnum != 0)\n\t\t\t{\n");
                fprintf(outmodel, "\t\t\t\tstrsqlupdate += \", \";\n");
                fprintf(outmodel, "\t\t\t}\n\n");
                fprintf(outmodel, "\t\t\tstrsqlupdate += \"%s = @param\" + paramnum.ToString();\n", field.name.c_str());
                fprintf(outmodel, "\t\t\tparamnum++;\n");
                fprintf(outmodel, "\t\t}\n\n");
            }

            ++fieldid;
        }
        fprintf(outmodel, "\t\tstrsqlupdate +=  \" WHERE id = \" + id.ToString();\n");

        fprintf(outmodel, "\t\tSqlCommand sqlccmd = new SqlCommand(strsqlupdate, sqlconn);\n");

        fieldid = 0;
        fprintf(outmodel, "\t\tparamnum = 0;\n\n");
        for (auto &field : tb.fields)
        {
            if (field.fg != primary_key)
            {
                fprintf(outmodel, "\t\tif (changed[%d])\n\t\t{\n", fieldid);
                fprintf(outmodel, "\t\t\tsqlccmd.Parameters.AddWithValue(\"@param\" + paramnum.ToString(), %s);\n", field.name.c_str());
                fprintf(outmodel, "\t\t\tparamnum++;\n");
                fprintf(outmodel, "\t\t}\n\n");
            }

            ++fieldid;
        }
        
        fprintf(outmodel, "\t\tConsole.WriteLine(sqlccmd.ExecuteNonQuery().ToString());\n\n");

        fprintf(outmodel, "\t}\n\n");


        fprintf(outmodel, "\tpublic void create()\n\t{\n");
        fprintf(outmodel, "\t\tSqlCommand cmdsqlcreate = new SqlCommand(\"INSERT INTO %s (", tb.name.c_str());

        bool comma = false;
        for (auto &field : tb.fields)
        {
            if (field.fg != primary_key)
            {
                if (comma)
                {
                    fprintf(outmodel, ", ");
                }
                comma = true;
                fprintf(outmodel, "%s", field.name.c_str());
            }
        }
        fprintf(outmodel, ") VALUES (");
        comma = false;

        fieldid = 0;
        for (auto &field : tb.fields)
        {
            if (field.fg != primary_key)
            {
                if (comma)
                {
                    fprintf(outmodel, ", ");
                }
                comma = true;
                fprintf(outmodel, "@param%d", fieldid);
            }
            
            ++fieldid;
        }
        fprintf(outmodel, "); SELECT SCOPE_IDENTITY()\", sqlconn);\n");


        fieldid = 0;
        for (auto &field : tb.fields)
        {
            if (field.fg != primary_key)
            {
                fprintf(outmodel, "\t\tcmdsqlcreate.Parameters.AddWithValue(\"@param%d\", %s);\n", fieldid, field.name.c_str());
            }

            ++fieldid;
        }                            
        fprintf(outmodel, "\t\t_id = Convert.ToInt32(cmdsqlcreate.ExecuteScalar());\n");

        fprintf(outmodel, "\t}\n\n");
        fprintf(outmodel, "}\n\n");

        fclose(outmodel);
        outmodel = nullptr;

        FILE *outpcs = fopen(("migrate" + tb.name + ".cs").c_str(), "w");

        for (auto &migration : tb.migrations)
        {
            fprintf(outpcs, "if (lastmigration < %d)\n{\n", migration.identifier);
            std::string nm = (migration.migtype? "alter" : "create");
            nm += "_for_" + tb.name;
            fprintf(outpcs, "TableField[] %s = new TableField[]\n{\n", nm.c_str());
            for (auto &field : migration.fields)
            {
                //  TableField(String n, int t, int l, bool nb, bool pk, bool ind, bool uns, bool ai)
                fprintf(outpcs, "    new TableField(\"%s\", %s, %d, %s, %s, %s, %s, %s),\n",
                    field.name.c_str(), std::to_string(field.ft).c_str(), field.ft == string? 255 : 0,  field.fg == primary_key? "false" : "true",
                    field.fg == primary_key? "true" : "false",
                    field.fg == primary_key? "true" : "false",
                    field.fg == primary_key? "true" : "false",
                    field.fg == primary_key? "true" : "false");
            }

            switch (migration.migtype)
            {
            case 0:
                fprintf(outpcs, "};\nsql.create_table(\"%s\", %s);\n", tb.name.c_str(), nm.c_str());
                break;

            case 1:
                fprintf(outpcs, "};\nsql.alter_table(\"%s\", %s);\n", tb.name.c_str(), nm.c_str());
                break;

            default:
                fprintf(outpcs, "error\n");
                break;
            }
            fprintf(outpcs, "}\n\n");
        }

        fclose(outpcs);
        outpcs = 0;
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
