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

# To accelerate the insertion using pandas' to_sql
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
    cities['city_id'] = cities.index + 1

   

    ## IOCCOUNTRYCODES.CSV
    # Here the na_filter=False is necessary since the country code for namibia
    # is NA which is interpreted as NaN by pandas
    ioc = pd.read_csv('olympics_data/ioccountrycodes.csv', header=0, na_filter=False)
    ioc = ioc.rename(columns={'Country': 'Country_name'})
    ioc.columns = lowercase(ioc.columns)



    # SPORTSTAXONOMY.CSV
    sports = pd.read_csv('olympics_data/sportstaxonomy.csv', header=0)
    sports = sports.rename(columns={'Sport': 'sport_name', 'Discipline': "discipline_name"}) 
    sports['discipline_id'] = sports.index + 1



    ## MEDALLISTS.CSV
    # For some reason the first two lines of the medallist csv are empty
    # (ignoring the comments)
    medal = pd.read_csv('olympics_data/medallists.csv', header=2, comment='"')

    # Extracting Athletes
    athlete = medal[['Athlete','Gender']].drop_duplicates().reset_index(drop=True)
    athlete = athlete.rename(columns={'Athlete': 'athlete_name', 'Gender': 'athlete_gender'})
    athlete['athlete_id'] = athlete.index + 1

    # Extracting events
    event = medal[['City', 'Event', 'Event_gender', 'Edition']]
    #gpb = event.groupby(['City', 'Edition', 'Event_gender', 'Discipline']).aggregate(lambda tdf: tdf.unique().tolist())
    #with pd.option_context('display.max_rows', None, 'display.max_columns', None):  # more options can be specified also
    #    print(gpb['Event'])

    event = event.rename(columns={'City':'city_name'})
    event = event.merge(cities, on='city_name')
    event = event[['Event', 'Event_gender', 'Edition', 'city_id']]
    event = event.drop_duplicates().reset_index(drop=True)
    event = event.rename(columns={'Event':'event_name', 'Event_gender':'event_gender', 'Edition':'edition'})
    event['event_id'] = event.index + 1 # to start at 1



    event_act = medal[['Event', 'City', 'Event_gender', 'Edition', 'Discipline']]
    event_act = event_act.rename(columns={'Event':'event_name', 
                                          'City': 'city_name',  
                                          'Edition': 'edition', 
                                          'Discipline': 'discipline_name', 
                                          'Event_gender': 'event_gender'})
    event_act = event_act.merge(sports, on='discipline_name')
    event_act = event_act.merge(cities, on='city_name')
    event_act = event_act.merge(event, on=['event_gender', 'event_name', 'edition', 'city_id'])
    event_act = event_act[['event_id', 'discipline_id']]
    event_act = event_act.drop_duplicates().reset_index(drop=True)
    event_act['event_activity_id'] = event_act.index + 1


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
    #medals = medals.merge(sports, on='discipline_name')
    medals = medals.merge(cities, on=['city_name', 'noc'])
    medals = medals.merge(event, on=['event_gender', 'event_name', 'edition', 'city_id'])
    medals = medals[['athlete_id', 'event_id', 'noc', 'Medal']]
    medals = medals.rename(columns={'noc':'represent_noc', 'Medal':'medal_type'})
    medals = medals.drop_duplicates().reset_index(drop=True)
    medals['medal_id'] = medals.index + 1

    
    # The pairs of table/dataframe
    SQLTables = reversed(["Medal", "EventActivity", "Event", "Athlete", "Discipline", "City", "CountryCode"])
    PandasDFs = reversed([medals , event_act      , event  , athlete  , sports      , cities,  ioc          ])

    # dropping the existing data and inserting new the new ones
    for table, df in zip(SQLTables, PandasDFs):
        print(f"\nInserting into {table}")

        # only print 10 lines 
        with pd.option_context('display.max_rows', 10, 'display.max_columns', None):
            print(df)

        # insert the new data
        try:
            df.to_sql(table, con=engine, if_exists='replace', index=False, method=psql_insert_copy)
        except:
            print("Data already inserted, skipping")
        print(pd.read_sql(sql=f'SELECT * FROM {table}', con=engine))

    #print(pd.read_sql(sql='SELECT * FROM City', con=engine))
    #print(pd.read_sql(sql='SELECT * FROM CountryCode', con=engine))
    #print(pd.read_sql(sql='SELECT * FROM Discipline', con=engine))
    #print(pd.read_sql(sql='SELECT * FROM Athlete', con=engine))
    #print(pd.read_sql(sql='SELECT * FROM Event', con=engine))
    #print(pd.read_sql(sql='SELECT * FROM Medal', con=engine))
if __name__ == "__main__":
    main()
