using System;
using System.Collections.Generic;
using System.Text;
using System.Data.SqlTypes;
using System.Data.SqlClient;
using System.Data;

namespace ConsoleApp1
{
    class {{! tblnicename !}}
    {
        bool[] changed = new bool[{{! #tabledata !}}];
    <?lua

    for k,p in ipairs(tabledata) do
    ?>
        private {{! p["type"] !}} _{{! p["name"] !}}; // {{! k - 1 !}}
        public {{! p["type"] !}} {{! p["name"] !}} {
            get { return _{{! p["name"] !}}; }
            set { changed[{{! k - 1 !}}] = true; _{{! p["name"] !}} = value; }
        }
    <?lua
    end
    ?>

        public void save()
        {
            int paramnum = 0;

            String strsqlupdate = "UPDATE users SET ";
    <?lua
    for k,p in ipairs(tabledata) do
        if p["pkey"] == false then
    ?>
            if (changed[{{! k - 1 !}}])
            {
                if (paramnum != 0)
                {
                    strsqlupdate += ", ";
                }

                strsqlupdate += "{{! p["name"] !}} = @param" + paramnum.ToString();
                paramnum++;
            }
    <?lua
        end
    end
    ?>
            strsqlupdate +=  " WHERE id = " + id.ToString();
            SqlCommand sqlccmd = new SqlCommand(strsqlupdate, SQLMGR.connect());
            paramnum = 0;

            <?lua
    for k,p in ipairs(tabledata) do
        if p["pkey"] == false then
    ?>
            if (changed[{{! k - 1 !}}])
            {
                sqlccmd.Parameters.AddWithValue("@param" + paramnum.ToString(), {{! p["name"] !}});
                paramnum++;
            }
    <?lua
        end
    end
    ?>
            Console.WriteLine(sqlccmd.ExecuteNonQuery().ToString());
        }

        public void create()
        {
            SqlCommand cmdsqlcreate = new SqlCommand("INSERT INTO {{! tblname !}} (<?lua
            comma = false
            for k,p in ipairs(tabledata) do
                if p["pkey"] == false then
                    if comma == true then
                        vp(", ")
                    end
                    comma = true
                    vp(p["name"])
                end
            end
            ?>) VALUES (<?lua
            comma = false
            for k,p in ipairs(tabledata) do
                if p["pkey"] == false then
                    if comma == true then
                        vp(", ")
                    end
                    comma = true
                    vp("@param" .. (k - 1))
                end
            end
            ?>); SELECT SCOPE_IDENTITY()", SQLMGR.connect());
    <?lua
    for k,p in ipairs(tabledata) do
        if p["pkey"] == false then
    ?>		cmdsqlcreate.Parameters.AddWithValue("@param{{! (k - 1) !}}", {{! p["name"] !}});
    <?lua
        end
    end
    ?>		_id = Convert.ToInt32(cmdsqlcreate.ExecuteScalar());
        }
    }
}
