//#include <cassert>
#include <stdio.h>
#include <string.h>

#include "leveldb/db.h"
#include "leveldb/write_batch.h"

void writeKey(char *s_key, char *s_value, leveldb::DB *db);
void AtomicUpdates(char *key1, char *vaule1, char *key2, char *vaule2, leveldb::DB *db);
void ShowKeyValue(char *key, leveldb::DB *db);
void ShowDB(leveldb::DB *db);

int main(int argc, char **argv)
{
    leveldb::DB *db = NULL;
    leveldb::Options options;
    options.create_if_missing = true;
    leveldb::Status status = leveldb::DB::Open(options, "/tmp/testleveldb", &db);

    if (status.ok()) {
        printf("level db open success\n");
    } else {
        printf("level db open failed %s\n", status.ToString().c_str());
    }

    ShowDB(db);
    writeKey((char *)"lilu", (char *)"love wife", db);
    AtomicUpdates((char *)"lilu", NULL, (char *)"xiaopangzi", NULL, db);
    ShowKeyValue((char *)"xiaopangzi", db);
    ShowDB(db);


    if (db) {
        delete db;
        db = NULL;
    }

    return 0;
}

void writeKey(char *s_key, char *s_value, leveldb::DB *db)
{
    std::string value = s_value;
    std::string key = s_key;
    std::string t_value;
    std::string t_key;

    leveldb::Status s;
    s = db->Put(leveldb::WriteOptions(), key, value);
    if (s.ok()) {
        s = db->Get(leveldb::ReadOptions(), key, &t_value);
        if (s.ok()) {
            printf("level db read ok\n");
            printf("key: %s, value: %s\n", key.c_str(), t_value.c_str());
        } else {
            printf("level db read failed %s\n", s.ToString().c_str());
        }
    } else {
        printf("level db write failed %s\n", s.ToString().c_str());
    }
}

void AtomicUpdates(char *key1, char *vaule1, char *key2, char *vaule2, leveldb::DB *db)
{
    std::string value;
    leveldb::Status s = db->Get(leveldb::ReadOptions(), key1, &value);
    if (s.ok()) {
        leveldb::WriteBatch batch;
        batch.Delete(key1);
        batch.Put(key2, value);
        s = db->Write(leveldb::WriteOptions(), &batch);
        if (s.ok()) {
            printf("Atomic update ok\n");
        } else {
            printf("Atomic update failed %s\n", s.ToString().c_str());
        }
    }
}

void ShowKeyValue(char *key, leveldb::DB *db)
{
    std::string value;
    leveldb::Status s;

    s = db->Get(leveldb::ReadOptions(), key, &value);
    if (!s.ok()) {
        printf("Get key %s failed\n", key);
    } else {
        printf("Get key success\n");
        printf("Key: %s, value: %s\n", key, value.c_str());
    }
}


void ShowDB(leveldb::DB *db)
{
    printf("%s\n", __func__);
    leveldb::Iterator *it = db->NewIterator(leveldb::ReadOptions());

    for (it->SeekToFirst(); it->Valid(); it->Next()) {
        printf("Key: %s, vaule: %s\n", it->key().ToString().c_str(),
                                        it->value().ToString().c_str());
    }
    assert(it->status().ok());
    delete it;
}

