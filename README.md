# jindexer
Index JSON documents into an SQLite database

## Build

### Prepare environment

jindexer depends on the following software:

- g++
- GNU Make
- [cJSON library](https://github.com/DaveGamble/cJSON)
- SQLite3 library and CLI

We can get those packages on Debian and Ubuntu running this command:

```sh
make prepare
```

### Compile

```sh
make
```

### Clean

```sh
make clean
```

## Run

jindexer reads the incoming JSON documents from the standard input, one object per line.

```sh
bin/jindexer < /some/file.json
```

### Help

```sh
bin/jindexer -h
```

#### Syntax

```
bin/jindexer [OPTIONS] [INDEX_PATH]
```

- `INDEX_PATH`: Path to the index database. Default: var/index.db

#### Options

- `-d`: Delete all existing data in the index.
- `-h`: Show this help and exit.
- `-j mode`: Journal mode: delete|truncate|memory|wal|off. Default: delete.
- `-r`: Enable re-indexing.
- `-s`: Disable disk synchronization.
- `-t`: Enable transaction.
- `-v`: Verbose mode.

### Optimize performance

The following flags let jindexer use transactions, disable the disk synchronization, and set the journal mode to memory to improve the performance:

```sh
bin/jindexer -j memory -s -t
```

## Measuring performance

In this test we inserted 50.000 objects corresponding to alerts from [Wazuh](https://github.com/wazuh/wazuh).

### Command

```sh
time bin/jindexer -stj memory < ~/alerts.json.50000
```

### Results

|Item|Result|
|---|---|
|Execution time|18.101 s|
|Documents|50000|
|Ingestion performance|2762 objects/s|
|Keys|1651050|
|Insertion performance|67316 rows/s|

### Hardware

|Component|Model|Performance|
|---|---|---|
|**Host**|MacBook Pro 2016||
|**Processor**|Intel Core i7-2670QM|2.2 GHz - 6 MB cache|
|**Memory**||1 GiB - 1333 MHz|
|**Disk**|Apple SSD|3 GBps|
|**OS**|Ubuntu Groovy Gorilla||
