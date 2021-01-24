print("")
print("")
print("")
print("Outra migração")
select_database("test_db")

migration = alter_table("users")
create_field(migration, "remember_token", "string")


print("DUMP DA TABELA INTEIRA")
dumptbl(migration)

print("migration end")

