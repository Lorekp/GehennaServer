-- Empty local authentication schema. Provision fresh development keys separately.
CREATE TABLE IF NOT EXISTS KEYS (
    USED INT,
    HASH TEXT,
    PermissionLevel INTEGER NOT NULL DEFAULT 1
);
