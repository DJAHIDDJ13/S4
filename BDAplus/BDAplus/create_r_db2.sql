CREATE TABLE City (
   city_id INT NOT NULL UNIQUE PRIMARY KEY,
   city_name VARCHAR(64) NOT NULL,
   NOC VARCHAR(3) NOT NULL
);

CREATE TABLE CountryCode (
   NOC VARCHAR(3) NOT NULL PRIMARY KEY,
   country_name VARCHAR(64) NOT NULL ,
   ISOCode VARCHAR(2) NOT NULL
);

CREATE TABLE Discipline (
	discipline_id INT NOT NULL UNIQUE PRIMARY KEY,
   sport_name VARCHAR(64),
   discipline_name VARCHAR(64)
);

CREATE TYPE Event_Gender_t AS ENUM ('W', 'M', 'X');
CREATE TABLE Event(
   event_id INT NOT NULL UNIQUE PRIMARY KEY,
   event_name VARCHAR(64),
   event_gender Event_Gender_t,
	discipline_id INT NOT NULL REFERENCES Discipline(discipline_id),
   edition INT,
   city_id INT NOT NULL REFERENCES City(city_id)
);

CREATE TYPE Gender_t AS ENUM ('Women', 'Men');
CREATE TABLE Athlete(
   athlete_id INT NOT NULL UNIQUE PRIMARY KEY,
   athlete_name VARCHAR(64) NOT NULL,
   athlete_gender Gender_t
);

CREATE TYPE Medal_t AS ENUM ('Gold', 'Silver', 'Bronze');
CREATE TABLE Medal (
   athlete_id INT NOT NULL REFERENCES Athlete(athlete_id),
   event_id INT NOT NULL REFERENCES Event(event_id),
   represent_NOC VARCHAR(3) REFERENCES CountryCode(NOC),
   medal_type Medal_t
);
