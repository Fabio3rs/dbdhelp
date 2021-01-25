using System;
using System.Collections.Generic;
using System.Text;
using System.Data.SqlClient;
using System.Data;
using System.Diagnostics;

namespace ConsoleApp1
{
    class Migrate
    {
        static void set_migration_val(int n)
        {
            String sqlstr = "UPDATE migrations SET last = @param1";
            SqlCommand sqlccmd = new SqlCommand(sqlstr, SQLMGR.connect());
            sqlccmd.Parameters.AddWithValue("@param1", n);

            Debug.Assert(sqlccmd.ExecuteNonQuery() != 0);
        }

        public static void exec_migration()
        {
            SQLMGR sql = new SQLMGR();

            String sqlstr = "select * from migrations";
            DataTable dt = new DataTable();
            SqlDataAdapter da = new SqlDataAdapter(sqlstr, SQLMGR.connect());
            da.Fill(dt);

            int lastmigration = Convert.ToInt32(dt.Rows[0]["last"]);

<?lua
            for k,p in ipairs(migratedata) do
?>

            if (lastmigration < {{! k !}})
            {
                TableField[] create_for_{{! p["table"] !}} = new TableField[]
                {
<?lua
                comma = false
                for i,l in ipairs(p["fields"]) do -- (String n, int t, int l, bool nb, bool pk, bool ind, bool uns, bool ai)
                    if comma == true then
                        vp(",\n")
                    end
                    comma = true
?>
                    new TableField("{{! l["name"] !}}", {{! l["type"] !}}, {{! l["length"] !}}, {{! l["nullable"] !}}, {{! l["pkey"] !}}, {{! l["index"] !}}, {{! l["unsigned"] !}}, {{! l["autoincrement"] !}})<?lua
end
vp("\n")
?>
                };
                sql.<?lua if p["migrationtype"] == 0 then vp("create") else vp("alter") end ?>_table("{{! p["table"] !}}", create_for_{{! p["table"] !}});

                set_migration_val({{! k !}});
            }
            
<?lua
end
?>
        }
    }
}



