# YADB
mini sgbd for education purposes

# Features roadmap
 - [x] create a table
 - [x] create a row of data for a given table 
 - [x] Save/load metadata of tables
 - [x] Proper logging system 
 - [x] Save/load all data from a given table
 - [x] Handle file position of error for functions
 - [ ] Organize data in a block architecture like postgres
 - [ ] Basic query system lib
 - [ ] Handle file integrity on insert/save errors (will be made at the same time than block architecture implementation)
 - [ ] Basic constraints for fields
 - [ ] Basic SQL client to create table and query data
 
# Build from source

## build and run the simple exemple 
```bash
./build.sh
./build/simple_exemple
```
### result
```

create and dump rows :

|                   id |                  age |                 name | 
|                    0 |                   35 |             Jean BON | 
|                    1 |                   47 |          Guy LIGUILY | 

save/load and dump database :

table 'Person' (
   INTEGER(8) id
   INTEGER(8) age
   STRING(50) name
)
```

# Documentation

## Block representation of data
There is no documentation right now but you can check the exemple in exemples folder