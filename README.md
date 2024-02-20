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
 - [ ] Change api to have a more abstract way of inserting and getting data (data with fixed and dynamic size + add a map between data type and size)
   - [ ] Rework structure of rows 
 - [ ] Basic query system lib (insert, get, remove, update)
 - [ ] Adding simple constraint system (primary keys, not null, etc...)
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
There is no documentation on how to use the library right now but you can check the exemple in exemples folder

## Architecture of a BLock of data

### header attributes
 - number of rows : u16
 - start of row pointers : u16
 - end of row pointers : u16
 - end of rows : u16

### representation of a block

[ number of rows | start of row pointers | end of row pointers | end of rows |
row ptr | row ptr | ... | ---->          ...         <---- | ... | row | row ]