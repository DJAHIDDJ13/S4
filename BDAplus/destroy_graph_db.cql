drop index on :Country(name);
drop index on :City(name);
drop index on :Sport(name);
drop index on :Discipline(name);
drop index on :Athlete(name);
drop index on :Event(name);
drop index on :Year(name);
drop index on :Gender(name);
drop constraint on (c:Country) assert c.noc is unique;

MATCH (n)
DETACH DELETE n;
