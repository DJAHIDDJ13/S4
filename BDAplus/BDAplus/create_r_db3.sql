CREATE TABLE CountryCode (
   NOC VARCHAR(3) NOT NULL PRIMARY KEY,
   country_name VARCHAR(64) NOT NULL ,
   ISOCode VARCHAR(2) NOT NULL
);

CREATE TABLE City (
   city_id INT NOT NULL UNIQUE PRIMARY KEY,
   city_name VARCHAR(64) NOT NULL,
   NOC VARCHAR(3) NOT NULL REFERENCES CountryCode(NOC)
);

CREATE TABLE Discipline (
	discipline_id INT NOT NULL UNIQUE PRIMARY KEY,
   sport_name VARCHAR(64),
   discipline_name VARCHAR(64)
);

CREATE TABLE EventActivity (
	event_activity_id INT NOT NULL UNIQUE PRIMARY KEY,
   discipline_id INT NOT NULL INT NOT NULL REFERENCES Discipline(discipline_id),
   event_id INT NOT NULL REFERENCES Event(event_id)
);

CREATE TABLE Event(
   event_id INT NOT NULL UNIQUE PRIMARY KEY,
   event_name VARCHAR(64),
   event_gender VARCHAR(1),
	discipline_id INT NOT NULL REFERENCES Discipline(discipline_id),
   edition INT,
   city_id INT NOT NULL REFERENCES City(city_id)
);

CREATE TABLE Athlete(
   athlete_id INT NOT NULL UNIQUE PRIMARY KEY,
   athlete_name VARCHAR(64) NOT NULL,
   athlete_gender VARCHAR(10)
);

CREATE TABLE Medal (
   medal_id INT NOT NULL UNIQUE PRIMARY KEY,
   athlete_id INT NOT NULL REFERENCES Athlete(athlete_id),
   event_id INT NOT NULL REFERENCES Event(event_id),
   represent_NOC VARCHAR(3) REFERENCES CountryCode(NOC),
   medal_type VARCHAR(10)
);
