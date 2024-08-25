DROP SCHEMA IF EXISTS points CASCADE;

CREATE TABLE IF NOT EXISTS points (
    x integer NOT NULL,
    y integer NOT NULL,
    PRIMARY KEY (x, y)
);