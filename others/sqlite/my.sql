BEGIN TRANSACTION;
CREATE TABLE mytable(id integer primary key, value text);
INSERT INTO "mytable" VALUES(1, 'Micheal');
INSERT INTO "mytable" VALUES(2, 'Jenny');
INSERT INTO "mytable" VALUES(3, 'Francis');
INSERT INTO "mytable" VALUES(4, 'Kerk');
INSERT INTO "mytable" VALUES(5, 'Zhangjincai');
CREATE TABLE student_info(stu_no interger primary key, name text);
CREATE VIEW nameview as select * from mytable;
CREATE INDEX test_idx on mytable(value);
COMMIT;