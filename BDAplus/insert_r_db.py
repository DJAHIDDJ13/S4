#!/usr/bin/python3
import psycopg2
import sys
import pprint
import pandas as pd
import numpy as np
from sqlalchemy import create_engine
from sqlalchemy.sql import text as sa_text
from io import StringIO
import csv

# To accelerate the insertion using pandas's to_sql
# https://pandas.pydata.org/pandas-docs/stable/user_guide/io.html#io-sql-method
def psql_insert_copy(table, conn, keys, data_iter):
    # gets a DBAPI connection that can provide a cursor
    dbapi_conn = conn.connection
    with dbapi_conn.cursor() as cur:
        s_buf = StringIO()
        writer = csv.writer(s_buf)
        writer.writerows(data_iter)
        s_buf.seek(0)

        columns = ', '.join('"{}"'.format(k) for k in keys)
        if table.schema:
            table_name = '{}.{}'.format(table.schema, table.name)
        else:
            table_name = table.name

        sql = 'COPY {} ({}) FROM STDIN WITH CSV'.format(
            table_name, columns)
        cur.copy_expert(sql=sql, file=s_buf)

def main():
    db_user = 'postgres' # username
    db_pass = '123456' # password
    db_host = '127.0.0.1' # hostname
    db_name = 'olympics' # Database name
    
    # Constructing the connection string
    conn_string = f'postgresql+psycopg2://{db_user}:{db_pass}@{db_host}/{db_name}'
    print(f"Connecting to database\n\t-> {conn_string}")
    # Create the db engine
    engine = create_engine(conn_string, pool_recycle=3600)
    

    ## utility lambda functions
    lowercase = lambda a: [e.lower() for e in a] # tranform array of strings to lowercase


    ### READING THE CSV FILES AND RESHAPING

    ## CITIES.CSV
    cities = pd.read_csv('olympics_data/cities.csv', header=0)
    cities = cities.rename(columns={'City': 'city_name'}) # changed to conform to db
    del cities['Country'] # conforming to db
    cities.columns = lowercase(cities.columns)
    cities['city_id'] = cities.index

   

    ## IOCCOUNTRYCODES.CSV
    # Here the na_filter=False is necessary since the country code for namibia
    # is NA which is interpreted as NaN by pandas
    ioc = pd.read_csv('olympics_data/ioccountrycodes.csv', header=0, na_filter=False)
    ioc = ioc.rename(columns={'Country': 'Country_name'})
    ioc.columns = lowercase(ioc.columns)

    print(ioc)


    # SPORTSTAXONOMY.CSV
    sports = pd.read_csv('olympics_data/sportstaxonomy.csv', header=0)
    sports = sports.rename(columns={'Sport': 'sport_name', 'Discipline': "discipline_name"}) 
    sports['discipline_id'] = sports.index



    ## MEDALLISTS.CSV
    # For some reason the first two lines of the medallist csv are empty
    # (ignoring the comments)
    medal = pd.read_csv('olympics_data/medallists.csv', header=2, comment='"')

    # Extracting Athletes
    athlete = medal[['Athlete','Gender']].drop_duplicates().reset_index(drop=True)
    athlete = athlete.rename(columns={'Athlete': 'athlete_name', 'Gender': 'athlete_gender'})
    athlete['athlete_id'] = athlete.index

    # Extracting events
    event = medal[['City', 'Event', 'Event_gender', 'Discipline', 'Edition']]
    event = event.rename(columns={'City':'city_name', 'Discipline': 'discipline_name'})
    event = event.merge(cities, on='city_name').merge(sports, on='discipline_name')
    event = event.drop(['city_name', 'discipline_name', 'sport_name', 'noc'], axis=1)
    event = event.reset_index(drop=True)
    event = event.rename(columns={'Event':'event_name', 'Event_gender':'event_gender', 'Edition':'edition'})
    event['event_id'] = event.index

    # Extracting medals
    medals = medal.rename(columns={'Event_gender': 'event_gender', 
                                   'NOC': 'noc',
                                   'Event':'event_name',
                                   'Edition':'edition',
                                   'City':'city_name',
                                   'Discipline': 'discipline_name',
                                   'Athlete': 'athlete_name',
                                   'Gender': 'athlete_gender'})

    medals = medals.merge(athlete, on=['athlete_name', 'athlete_gender'])
    medals = medals.merge(sports, on='discipline_name')
    medals = medals.merge(cities, on=['city_name', 'noc'])
    medals = medals.merge(event, on=['event_gender', 'event_name', 'edition', 'discipline_id', 'city_id'])
    medals = medals[['athlete_id', 'event_id', 'noc', 'Medal']]
    medals = medals.rename(columns={'noc':'represent_noc', 'Medal':'medal_type'})
    
    
    
    # dropping the existing data and inserting new the new ones
    for table in ["City", "CountryCode", "Discipline", "Medal", "Athlete", "Event", "Medal"]:
        engine.execute(sa_text(f"TRUNCATE TABLE {table} CASCADE").execution_options(autocommit=True))

    ## Inserting the data frames to postgresql
    
    # inserting cities
    cities.to_sql('City', con=engine, if_exists='replace', index=False, method=psql_insert_copy)
    # inserting country codes
    ioc.to_sql('CountryCode', con=engine, if_exists='replace', index=False, method=psql_insert_copy)
    # inserting sports
    sports.to_sql('Discipline', con=engine, if_exists='replace', index=False, method=psql_insert_copy)
    # inserting athletes
    athlete.to_sql('Athlete', con=engine, if_exists='replace', index=False, method=psql_insert_copy)
    # inserting event
    event.to_sql('Event', con=engine, if_exists='replace', index=False, method=psql_insert_copy)
    # inserting medal
    medals.to_sql('Medal', con=engine, if_exists='replace', index=False, method=psql_insert_copy)

    #print(pd.read_sql(sql='SELECT * FROM City', con=engine))
    #print(pd.read_sql(sql='SELECT * FROM CountryCode', con=engine))
    #print(pd.read_sql(sql='SELECT * FROM Discipline', con=engine))
    #print(pd.read_sql(sql='SELECT * FROM Athlete', con=engine))
    #print(pd.read_sql(sql='SELECT * FROM Event', con=engine))
    #print(pd.read_sql(sql='SELECT * FROM Medal', con=engine))
if __name__ == "__main__":
    main()
