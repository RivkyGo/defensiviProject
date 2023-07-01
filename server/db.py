import sqlite3

CLIENT_TABLE = {}
FILE_TABLE = {}

def open_db():


    # Connect to the database
    conn = sqlite3.connect(r"server.db")

    # Create a cursor object
    cursor = conn.cursor()

    # Execute a query to create a table
    cursor.execute(
        '''CREATE TABLE clients (ID VARCHAR(16) , Name VARCHAR(255)  , PublicKey VARCHAR(160), LastSeen TIME, AES VARCHAR(16),AES_ENCRYPT VARCHAR(300) )''')
    cursor.execute(
        '''CREATE TABLE files (ID VARCHAR(16) , FileName VARCHAR(255)  , PathName VARCHAR(160), Verified BOOLEAN )''')
    # Commit the changes
    conn.commit()
    # Close the connection
    conn.close()
#
#
#     conn = sqlite3.connect(r"server.db")
#     cursor = conn.cursor()
#
#     # Execute a query
#     cursor.execute('SELECT * FROM my_table')
#
#     # Fetch the results
#     results = cursor.fetchall()
#
#     # Close the connection
#     conn.close()
def load_client_db():
    global CLIENT_TABLE

    conn = sqlite3.connect(r"server.db")
    cursor = conn.cursor()

    # Execute a SELECT query to retrieve data from the database
    cursor.execute('SELECT ID, Name, PublicKey, LastSeen, AES , AES_ENCRYPT FROM clients')
    rows = cursor.fetchall()
    for row in rows:
        row_dict = [row[1], row[2], row[3], row[4] , row[5]]
        CLIENT_TABLE[row[0]] = row_dict

    # Close the cursor and the database connection
    cursor.close()
    conn.close()

    # Print the data dictionary
    # print(CLIENT_TABLE)

def load_file_db():
    global FILE_TABLE

    conn = sqlite3.connect(r"server.db")
    cursor = conn.cursor()

    # Execute a SELECT query to retrieve data from the database
    cursor.execute('SELECT ID, FileName, PathName, Verified FROM files')
    rows = cursor.fetchall()
    for row in rows:
        row_dict = [row[1], row[2], row[3]]
        FILE_TABLE[row[0]] = row_dict

    # Close the cursor and the database connection
    cursor.close()
    conn.close()

    # Print the data dictionary
    # print(FILE_TABLE)
