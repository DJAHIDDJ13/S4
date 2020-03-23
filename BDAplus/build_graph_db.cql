create index on :Country(name);
create index on :City(name);
create index on :Sport(name);
create index on :Discipline(name);
create index on :Athlete(name);
create index on :Event(name);
create index on :Year(name);
create index on :Gender(name);
create constraint on (c:Country) assert c.noc is unique;


//add the genders
create (g:Gender {name:"Men"});
create (g:Gender {name:"Women"});

//load countries
load csv with headers from
"file:///olympics_data/ioccountrycodes.csv" as csv
create (c:Country {name: csv.Country, noc: csv.NOC, iso: csv.ISOcode});

//load cities
load csv with headers from
"file:///olympics_data/cities.csv" as csv
match (c:Country {noc: csv.NOC})
merge (ci:City {name: csv.City})
merge (ci)-[:LOCATED_IN]->(c);

//load sports taxonomy
load csv with headers from
"file:///olympics_data/sportstaxonomy.csv" as csv
merge (s:Sport {name: csv.Sport})
merge (s)<-[:PART_OF]-(d:Discipline {name: csv.Discipline});

//load the games
load csv with headers from
"file:///olympics_data/medallists.csv" as csv
match (c:City {name: csv.City})
merge (c)-[:HOSTS_GAMES]->(y:Year {name: csv.Edition});

//load the events at the games
//cypher planner=rule
load csv with headers from
"file:///olympics_data/medallists.csv" as csv
match (y:Year {name: csv.Edition}), (d:Discipline {name: csv.Discipline})
merge (d)<-[:PART_OF]-(e:Event {name: csv.Event})-[:AT_GAMES]->(y);

//load the medallists
load csv with headers from
"file:///olympics_data/medallists.csv" as csv
match (g:Gender {name: csv.Gender})
merge (a:Athlete {name: csv.Athlete})-[:HAS_GENDER]->(g);

//cypher planner=rule
load csv with headers from
"file:///olympics_data/medallists.csv" as csv
match (a:Athlete {name: csv.Athlete}), (c:Country {noc: csv.NOC})
merge (a)-[:REPRESENTS]->(c);

//cypher planner=rule
//using periodic commit
load csv with headers from
"file:///olympics_data/medallists.csv" as csv
match (a:Athlete {name: csv.Athlete}), (d:Discipline {name: csv.Discipline})<--(e:Event {name: csv.Event})-->(y:Year {name: csv.Edition})
create (a)-[:WINS]->(m:Medal {type: csv.Medal})-[:REWARD_FOR]->(e);
