#include "Hash.h"
#include <cstdio>
#include <iostream>
#include <cstdlib>
using namespace std;

DB::DB(string s) {
	fileName = s;
	idxName = fileName + ".idx";
	datName = fileName + ".dat";
	err1 = fopen_s(&fp1, idxName.c_str(), "rb");
	err2 = fopen_s(&fp2, datName.c_str(), "rb");
	if ((err1 != 0) || (err2 != 0)) {
		cout << "An empty DB will be built!\n";
		fopen_s(&fp1, idxName.c_str(), "wb");
		fopen_s(&fp2, datName.c_str(), "wb");
		//
		last_dat_off = 0;
		last_idx_off = HASH_SIZE;
		//
		fwrite(&last_dat_off, sizeof(int), 1, fp2);
		fwrite(&last_idx_off, sizeof(int), 1, fp1);
		Idx index;
		for (int i = 0; i < KEYSIZE_MAX - 1; i++)
			index.key[i] = '\0';
		index.key[KEYSIZE_MAX - 1] = '\0';
		index.value_off = 0;
		index.off = 0;
		index.off_next = 0;
		index.len_key = 0;
		index.len_value = 0;
		index.isDelete = true;
		for (int i = 0; i < HASH_SIZE; i++)
			fwrite(&index, sizeof(index), 1, fp1);
		fclose(fp1);
		fclose(fp2);
	}
	else {
		cout << "Db has been found.\n";
		fclose(fp1);
		fclose(fp2);
	}
}


int DB::open() {
	cout << "Db has been open.\n";
	if ((err1 = fopen_s(&fp1, idxName.c_str(), "rb+")) != 0) {
		cout << "Cannot build index file.\n";
		return 0;
	}
	if ((err2 = fopen_s(&fp2, datName.c_str(), "rb+")) != 0) {
		cout << "Cannot build data file .\n";
		return 0;
	}
	int num;
	fread(&num, sizeof(int), 1, fp1);
	last_idx_off = num;
	fread(&num, sizeof(int), 1, fp2);
	last_dat_off = num;
	cout << "last_idx_off is : " << last_idx_off << endl;
	cout << "last_dat_off is : " << last_dat_off << endl;
	return 1;
}

int DB::close() {
	cout << "last_idx_off is : " << last_idx_off << endl;
	cout << "last_dat_off is : " << last_dat_off << endl;
	rewind(fp1);
	rewind(fp2);
	fwrite(&last_dat_off, sizeof(int), 1, fp2);
	fwrite(&last_idx_off, sizeof(int), 1, fp1);
	fclose(fp1);
	fclose(fp2);
	cout << "Db has been closed.\n";
	return 1;
}





unsigned int DB::hash(const char* key) {
	unsigned int seed = 1313131;
	unsigned int h = 0;
	while (*key) {
		h = h*seed + (*key++);
	}
	return ((h & 0x7FFFFFFF) % HASH_SIZE);
}

char*  DB::find(const char* key) {
	Idx* Idx_find = find_key(key);
	if (Idx_find == NULL) {
		return NULL;
	}
	else {
		unsigned int n = Idx_find->value_off;
		char*value = new char[Idx_find->len_value + 1];
		fseek(fp2, sizeof(int) + Idx_find->value_off, 0);
		fread(value, sizeof(char), Idx_find->len_value + 1, fp2);
		return value;
	}
}

bool DB::del(char* key) {
	Idx* Idx_find = find_key(key);
	if (Idx_find == NULL) {
		return false;
	}
	else {
		Idx_find->isDelete = true;
		for (int i = 0; i < KEYSIZE_MAX; i++)
			Idx_find->key[i] = '\0';
		fseek(fp1, sizeof(int) + sizeof(Idx)*Idx_find->off, 0);
		fwrite(Idx_find, sizeof(Idx), 1, fp1);//renew index file
		fflush(fp1);
		return true;
	}
}

int DB::insert(char* key, char*value) {
	if (find_key(key)) {
		return 0;
	}
	unsigned int off_new = hash(key);
	Idx Idx_new;
	fseek(fp1, sizeof(Idx)*off_new + sizeof(int), 0);
	fread(&Idx_new, sizeof(Idx), 1, fp1);
	if (Idx_new.off == 0) {
		//initialize the new Index struct
		Idx_new.len_key = strlen(key);
		Idx_new.len_value = strlen(value);
		Idx_new.off = off_new;
		Idx_new.isDelete = false;
		Idx_new.off_next = last_idx_off;
		Idx_new.value_off = last_dat_off;
		last_dat_off += (Idx_new.len_value + 1) * sizeof(char);
		for (int i = 0; i < Idx_new.len_key; i++) {
			Idx_new.key[i] = key[i];
		}
		fseek(fp1, sizeof(Idx)*Idx_new.off + sizeof(int), 0);
		fwrite(&Idx_new, sizeof(Idx), 1, fp1);
		fseek(fp2, sizeof(int) + Idx_new.value_off, 0);
		fwrite(value, sizeof(char), Idx_new.len_value + 1, fp2);
		fflush(fp1);
		fflush(fp2);
		return 1;
	}
	if (Idx_new.isDelete) {
		Idx_new.len_key = strlen(key);
		Idx_new.len_value = strlen(value);
		Idx_new.isDelete = false;
		Idx_new.value_off = last_dat_off;
		last_dat_off += (Idx_new.len_value + 1) * sizeof(char);
		for (int i = 0; i < Idx_new.len_key; i++) {
			Idx_new.key[i] = key[i];
		}
		fseek(fp1, sizeof(Idx)*Idx_new.off + sizeof(int), 0);
		fwrite(&Idx_new, sizeof(Idx), 1, fp1);
		fseek(fp2, sizeof(int) + Idx_new.value_off, 0);
		fwrite(value, sizeof(char), Idx_new.len_value + 1, fp2);
		fflush(fp1);
		fflush(fp2);
		return 1;
	}
	else {
		while (true) {
			fseek(fp1, sizeof(Idx)*Idx_new.off_next + sizeof(int), 0);
			int flag = fread(&Idx_new, sizeof(Idx), 1, fp1);
			if (flag == 0) { //no index in this place
				Idx_new.len_key = strlen(key);
				Idx_new.len_value = strlen(value);
				Idx_new.off = last_idx_off;
				last_idx_off += 1;
				Idx_new.isDelete = false;
				Idx_new.off_next = last_idx_off;
				Idx_new.value_off = last_dat_off;
				last_dat_off += (Idx_new.len_value + 1) * sizeof(char);
				for (int i = 0; i < Idx_new.len_key; i++) {
					Idx_new.key[i] = key[i];
				}
				fseek(fp1, sizeof(Idx)*Idx_new.off + sizeof(int), 0);
				fwrite(&Idx_new, sizeof(Idx), 1, fp1);
				fseek(fp2, sizeof(int) + Idx_new.value_off, 0);
				fwrite(value, sizeof(char), Idx_new.len_value + 1, fp2);
				fflush(fp1);
				fflush(fp2);
				return 1;
			}
			if (Idx_new.isDelete) {
				Idx_new.len_key = strlen(key);
				Idx_new.len_value = strlen(value);
				Idx_new.isDelete = false;
				Idx_new.value_off = last_dat_off;
				last_dat_off += (Idx_new.len_value + 1) * sizeof(char);
				for (int i = 0; i < Idx_new.len_key; i++) {
					Idx_new.key[i] = key[i];
				}
				fseek(fp1, sizeof(Idx)*Idx_new.off + sizeof(int), 0);
				fwrite(&Idx_new, sizeof(Idx), 1, fp1);
				fseek(fp2, sizeof(int) + Idx_new.value_off, 0);
				fwrite(value, sizeof(char), Idx_new.len_value + 1, fp2);
				fflush(fp1);
				fflush(fp2);
				return 1;
			}
		}
	}
}





bool DB::replace(char*key, char*value) {
	//cout << "replace function\n";
	Idx* Idx_find = find_key(key);
	if (Idx_find == NULL) {
		return false;
	}
	else {
		unsigned int n = Idx_find->value_off;
		Idx_find->len_value = strlen(value);
		Idx_find->value_off = last_dat_off;
		last_dat_off += (Idx_find->len_value + 1) * sizeof(char);
		fseek(fp2, sizeof(int) + Idx_find->value_off, 0);
		fwrite(value, sizeof(char), Idx_find->len_value + 1, fp2);//replace value
		fseek(fp1, sizeof(int) + sizeof(Idx)*Idx_find->off, 0);
		fwrite(Idx_find, sizeof(Idx), 1, fp1);//renew index file
		fflush(fp1);
		fflush(fp2);
		return true;
	}
}

Idx* DB::find_key(const char* key) {
	int len = strlen(key);
	unsigned int off_find = hash(key);
	Idx* Idx_find = new Idx;

	while (off_find) {
		fseek(fp1, (sizeof(Idx) * off_find) + sizeof(int), 0);
		if (fread(Idx_find, sizeof(Idx), 1, fp1) == 0) {
			return NULL;

		}
		if (Idx_find->isDelete) {
			off_find = Idx_find->off_next;
			continue;
		}
		
		if (len != Idx_find->len_key) {
			off_find = Idx_find->off_next;
			continue;
		}
		bool flag = true;
		for (int i = 0; i < len; i++) {
			if (key[i] != Idx_find->key[i]) {
				flag = false;
				break;
			}
		}
		if (flag ) {
			return Idx_find;
		}
		off_find = Idx_find->off_next;
	}
	return NULL;
}

void DB::traversal() {
	fseek(fp1, sizeof(int), 0);
	Idx idx;
	int i = 1;
	while (fread(&idx, sizeof(Idx), 1, fp1) != 0) {
		if ((idx.isDelete) || (idx.off == 0))
			continue;
		char*value = new char[idx.len_value + 1];
		fseek(fp2, sizeof(int) + idx.value_off, 0);
		fread(value, sizeof(char), idx.len_value + 1, fp2);
		cout <<i<< " key-------- " << idx.key << " ---------value-------- " << value << endl;
		++i;
		delete value;
	}
}

void DB::clear() {
	remove(idxName.c_str());
	remove(datName.c_str());
}