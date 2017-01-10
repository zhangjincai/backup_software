BEGIN TRANSACTION;
CREATE TABLE mytable(id integer primary key, value text);
INSERT INTO "mytable" VALUES(1, 'Tony');
INSERT INTO "mytable" VALUES(6, 'jhon');
INSERT INTO "mytable" VALUES(7, 'Marry');
COMMIT;
