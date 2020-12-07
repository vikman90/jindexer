-- December 6th, 2020

CREATE TABLE document (
    data BLOB
);

CREATE TABLE indexer (
    key TEXT NOT NULL,
    value_string TEXT,
    value_number REAL,
    document_id INT NOT NULL REFERENCES document (rowid)
);

CREATE INDEX indexer_key ON indexer (key);
CREATE INDEX indexer_value_string ON indexer (value_string);
CREATE INDEX indexer_value_number ON indexer (value_number);
